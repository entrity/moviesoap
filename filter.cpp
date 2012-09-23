#include "qt4/main.hpp"
#include "variables.h"
#include "filter.hpp"

#include "tar/ustar.h"

#include <vlc_common.h>
#include <vlc_threads.h>
#include <vlc_variables.h>
#include <vlc_playlist.h> // needed for var_GetTime(input_thread_t *, char *)

#include <string>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
using namespace std;

#ifndef INT64_C
# define INT64_C(c)  c ## LL
#endif
#define CALC_CLOCK_CYCLES(microseconds) (microseconds * CLOCK_FREQ / 1000000) // multiply microseconds by clock cycles per microsecond to get delay in terms of clock cycles
#define MOVIESOAP_MOD_TIME_TO_US(mod_time) (mod_time * MOVIESOAP_MOD_TIME_FACTOR) // calculate microseconds equivalent of moviesoap_mod_t times

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
// temp
#include <iomanip>

/*
 * mtime_t args should be in us (microseconds)
 */
static inline void scheduleMod(Moviesoap::Mod * p_mod, mtime_t now, mtime_t trigger_us, void (*callback)(void *))
{
	// calc delay
	// mtime_t delay = (trigger_us - now) * CLOCK_FREQ / 1000000; // multiply microseconds by clock cycles per microsecond to get delay in terms of clock cycles
	mtime_t delay = CALC_CLOCK_CYCLES(trigger_us - now);
	// create, schedule timer
	vlc_timer_create( &p_mod->timer, callback, p_mod );
	vlc_timer_schedule( p_mod->timer, false, delay, 0 );
}

static inline mtime_t getNow() { return var_GetTime( Moviesoap::p_input, "time" ); }

/* Destroy timer and remove from scheduledMods list */
static inline void unscheduleMod(Moviesoap::Mod * p_mod)
{
	p_mod->p_filter->scheduledMods.remove( p_mod );
	// vlc_timer_destroy( p_mod->timer );
}

/* Non-member functions */
namespace Moviesoap
{
	/* 	Callback. Implement mod effect.
		Schedule mod deactivation (MUTE, BLACK) or destroy timer & remove from scheduledMods (SKIP). 
		Load next Mod. */
	void activateMod(void * p_data)
	{
		Mod * p_mod = (Mod *) p_data;
		// Begin effect
		p_mod->activate();
		// Destroy timer and remove from scheduledMods list (if SKIP)
		if (p_mod->mod.mode == MOVIESOAP_SKIP) {
			unscheduleMod(p_mod);
		}
		// Schedule mod deactivation (if MUTE or BLACKOUT)
		else {
			scheduleMod( p_mod, getNow(), MOVIESOAP_MOD_TIME_TO_US(p_mod->mod.stop), deactivateMod );
		}
		// load next mod
		p_mod->p_filter->loadNextMod(); // todo
	}

	/* Callback. Destroy stop timer. End effect. */
	void deactivateMod(void * p_data)
	{
		Mod * p_mod = (Mod *) p_data;
		// Destroy timer and remove from scheduledMods list
		unscheduleMod(p_mod);
		// End effect
		p_mod->deactivate();
	}
}

/* Filter functions */
namespace Moviesoap
{
	// FILE IO

	/* Write filter's meta data (mod descriptions) to outstream */
	int Filter::metaOut(ofstream & outs)
	{
		list<Mod>::iterator iter;
		// calculate data size
		unsigned int size = 0;
		for (iter = modList.begin(); iter != modList.end(); iter++) {
			size += iter->description.length();
			size += 1; // the +1 is for the endl char, which we'll write in the next loop
		}
		// make header
		Entrity::Ustar::Header header("meta.txt", size);
		// write header
		header.out(outs);
		// write data		
		for (iter = modList.begin(); iter != modList.end(); iter++)
			outs << iter->description << endl;
		// write padding
		header.pad(outs);
		return 0;
	}

	/* Write filters mod data to outstream */
	int Filter::dataOut(ofstream & outs)
	{
		list<Mod>::iterator iter;
		// calculate date size
		unsigned int size = sizeof(moviesoap_mod_t) * modList.size();
		// make header
		Entrity::Ustar::Header header("data", size);
		// write header
		header.out(outs);
		// write data
		for (iter = modList.begin(); iter != modList.end(); iter++)
			outs.write((char *) &iter->mod, sizeof(moviesoap_mod_t));
		// write padding
		header.pad(outs);
		return 0;
	}

	/* Write filter to tar file */
	int Filter::save()
	{
		string fpath = "output.tar"; // psz
		// open file & write meta & write data
		ofstream outs(fpath.c_str());
		metaOut(outs);
		dataOut(outs);
		// write two empty blocks & close stream & return
		for (int i=0; i < 512*2; i++) { outs.put(0); }
		outs.close();
		return MOVIESOAP_SUCCESS;
	}

	// ACTIVATING AND DEACTIVATING MODS

	/* Set queuedMod to first mod. Call loadNextMod */
	void Filter::Restart()
	{
		queuedMod = modList.begin();
		loadNextMod();
	}

	/* Destroy timers. Undo any active effects (mute & blackout) */
	void Filter::Stop()
	{
		list<Mod*>::iterator iter;
		for ( iter = scheduledMods.begin(); iter != scheduledMods.end(); iter++ )
			{ deactivateMod( *iter ); }
	}

	/* Load queuedMod. Stop Filter if end reached. */
	void Filter::loadNextMod()
	{
		list<Mod>::iterator iter;
		// Get current time in us (microseconds)
		mtime_t now = getNow();
		// Find next mod whose stop time is not passed
		for ( iter = queuedMod; iter != modList.end(); iter++ ) {
			if ( (queuedMod->mod.stop * MOVIESOAP_MOD_TIME_FACTOR) > now ) {
				cout << setw(18) << "QUEUE MOD: " << "now(" << now / MOVIESOAP_MOD_TIME_FACTOR << ") ";
				queuedMod->out(cout);
				Mod * p_mod = &*queuedMod++;
				loadMod( p_mod, now ); // Load mod if stop time is not passed
				return; // break
			}
		}
		cout << "No further mods to load." << endl;
	}

	void test2(void * test) { // todo remove
		// todo delete
		msg_Info(p_obj, "in callback");
	}

	/* Schedule start of mod or activate mod. Arg 'now' should be in us (microseconds). */
	void Filter::loadMod(Mod * p_mod, mtime_t now)
	{
		// add to scheduledMods
		p_mod->p_filter = this;
		scheduledMods.push_back(p_mod);
		// calc start time in microseconds (us)
		mtime_t start_us = MOVIESOAP_MOD_TIME_TO_US(p_mod->mod.start);
		// schedule start (if start not passed)
		if (now < start_us)
			{ scheduleMod( p_mod, now, start_us, activateMod ); }
		// activate (if start passed)
		else
			{ activateMod( p_mod ); }
	}
	
	/* Create a filter with a few test mods for testing */
	Filter * Filter::dummy()
	{
		Mod * p_mod;
		Filter * p_filter = new Filter();
		p_filter->creator = "MJ";
		p_filter->title = "Labour of Love";
		p_filter->isbn = "1234567890";
		p_filter->year = "1987";
		// add mod
		p_mod = new Mod( MOVIESOAP_SKIP, 100, 800, 0, 0 );
		p_mod->description = "skip talking";
		p_filter->modList.push_back( *p_mod );
		// add mod
		p_mod = new Mod( MOVIESOAP_BLACKOUT, 300, 1000, 5, 6, 7, 8, 9, 2 );
		p_mod->description = "blackout (cover) something";
		p_filter->modList.push_back( *p_mod );
		// add mod
		p_mod = new Mod( MOVIESOAP_MUTE, 1100, 1300, 0, 0 );
		p_mod->description = "mute the word \"foo\"";
		p_filter->modList.push_back( *p_mod );
		return p_filter;
	}

	void Filter::toStdout() {
		list<Mod>::iterator iter;
		cout << "============================" << endl;
		for (iter = modList.begin(); iter != modList.end(); iter++) {
			printf("MOD: %s\n\t%d - %d (%d :: %d) [(%d, %d) (%d, %d)]\n", 
				iter->description.c_str(), iter->mod.start, iter->mod.stop,
				iter->mod.category, iter->mod.severity,
				iter->mod.x1, iter->mod.y1, iter->mod.x2, iter->mod.y2);
		}
		cout << "============================" << endl;
	}
}

/* Mod functions */
namespace Moviesoap {
	Mod::Mod(uint8_t mode, uint32_t start, uint32_t stop, uint8_t category, uint8_t severity, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
	{
		mod.mode = mode;
		mod.start = start;
		mod.stop = stop;
		mod.category = category;
		mod.severity = severity;
		mod.x1 = x1;
		mod.x2 = x2;
		mod.y1 = y1;
		mod.y2 = y2;
	}

	bool Mod::compare(Mod & otherMod)
	{
		return mod.start < otherMod.mod.start;
	}

	void Mod::activate()
	{
		cout << setw(18) << "ACTIVATE MOD: " << "now(" << getNow() / MOVIESOAP_MOD_TIME_FACTOR << ") ";
		out(cout);
	}
	
	void Mod::deactivate()
	{
		cout << setw(18) << "DEACTIVATE MOD: " << "now(" << getNow() / MOVIESOAP_MOD_TIME_FACTOR << ") ";
		out(cout);
	}

	bool Mod::operator==(const Mod & other) const {
		return this == &other;
	}

	void Mod::out(ostream & stream) { stream << "Mod: " << description << " (" << mod.start << "-" << mod.stop << ")" << endl; }
}

#undef CALC_CLOCK_CYCLES