#include "qt4/main.hpp"
#include "variables.h"
#include "filter.hpp"

#include "tar/ustar.h"

#include <vlc_common.h>
#include <vlc_threads.h>
#include <vlc_variables.h>

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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/*
 * mtime_t args should be in us (microseconds)
 */
static void scheduleTimer(vlc_timer_t timer, mtime_t now, mtime_t triggerTime)
{
	// calc delay
	mtime_t delay = triggerTime;
	delay -= now;
	delay *= CLOCK_FREQ / 1000000; // multiply microseconds by clock cycles per microsecond to get delay in terms of clock cycles
	// schedule timer
	printf("delay %d\n", delay);
	vlc_timer_schedule( timer, false, delay, 0 );
}


namespace Moviesoap
{

	// FILE IO

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

	/* Set p_queuedMod to first mod, load same. */		
	void Filter::Restart()
	{

	}

	/* Destroy timers. Undo any active effects (mute & blackout) */
	void Filter::Stop()
	{

	}

	/* Load queuedMod */
	void Filter::loadNextMod()
	{

	}

	/* Schedule start of mod or activate mod */
	void Filter::loadMod(Mod & mod)
	{
		// get current time
		mtime_t now = var_GetTime( Moviesoap::p_obj, "time" ); // 'now' in microseconds (us)
		// add to scheduledMods
		scheduledMods.push_back(mod);
		// schedule start (if start not passed)
		mtime_t start_us = mod.mod.start * 1000;
		if (now > start_us) {
			vlc_timer_create( &mod.timer, Filter::activateMod, &mod );
			scheduleTimer( mod.timer, now, start_us );
		}
		// activate (if start passed)
		else
			Filter::activateMod( &mod );
		
		printf("mod scheduled\n");
	}

	/* Implement mod effect, schedule stop, destroy start timer */
	void Filter::activateMod(void * p_data)
	{
		Mod * p_mod = (Mod *) p_data;
		printf("activate mod +++\n");
		// schedule stop
		if (p_mod->mod.mode == MOVIESOAP_SKIP) return;
		vlc_timer_create( &p_mod->timer, deactivateMod, p_mod );
	}

	/* Destroy stop timer. End effect. */
	void Filter::deactivateMod(void * p_data)
	{
		Mod * p_mod = (Mod *) p_data;
		// destroy timer
		vlc_timer_destroy( p_mod->timer );
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
		p_mod = new Mod( MOVIESOAP_SKIP, 1, 8, 0, 0 );
		p_mod->description = "skip talking";
		p_filter->modList.push_back( *p_mod );
		// add mod
		p_mod = new Mod( MOVIESOAP_BLACKOUT, 1, 10, 5, 6, 7, 8, 9, 2 );
		p_mod->description = "blackout (cover) something";
		p_filter->modList.push_back( *p_mod );
		// add mod
		p_mod = new Mod( MOVIESOAP_SKIP, 11, 13, 0, 0 );
		p_mod->description = "mute the word \"foo\"";
		p_filter->modList.push_back( *p_mod );
		return p_filter;
	}
}

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
		printf("ACTIVATE MOD\n");
	}
	
	void Mod::deactivate()
	{
		printf("DEACTIVATE MOD\n");
	}
}