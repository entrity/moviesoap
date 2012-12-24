#include "filter.hpp"
#include "qt4/main.hpp"

#include <vlc_threads.h>
#include <vlc_variables.h>
#include <vlc_playlist.h> // needed for var_GetTime(input_thread_t *, char *)
#include <vlc_aout_intf.h>	// aout_ToggleMute

#include <list>
#include <algorithm> // for list::advance
#include <cstdio>	// for file io
using namespace std;

#ifndef INT64_C
# define INT64_C(c)  c ## LL
#endif
#define CALC_CLOCK_CYCLES(microseconds) (microseconds * CLOCK_FREQ / 1000000) // multiply microseconds by clock cycles per microsecond to get delay in terms of clock cycles

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
// temp
#ifdef MSDEBUG1
	#include <iomanip>
	#include <iostream>
using namespace std;
#endif

/* Non-member functions */
namespace Moviesoap
{
	void tryModStart(void * p_data) { tryModStart( (Mod *) p_data, MoviesoapGetNow(p_input) ); }
	void tryModStop(void * p_data) { tryModStop( (Mod *) p_data, MoviesoapGetNow(p_input) ); }

	/* 	Remove mod from scheduledMods list (if present).
		Schedule start or activate mod.
		Add to scheduledMods list if timer was set.
		If activated and has active effect, calls function to add to scheduledMods list and schedule deactivation.
		Calls Filter::loadNextMod.
		*/
	void tryModStart(Mod * p_mod, mtime_t now)
	{
		// Remove from scheduled list
		p_mod->removeFromScheduledList();
		// Calculate delay until start in microseconds
		mtime_t delay = p_mod->calcActivationDelay(now);
		if (delay) {
			// Reschedule start if start time is not reached
			p_mod->schedule( delay, tryModStart );
			// add to scheduled list
			p_mod->addToScheduledList();
		} else {
			// Activate effect
			p_mod->activate();
			// Schedule deactivation if mod produces an active effect
			if ( p_mod->producesActiveEffect() )
				tryModStop(p_mod);
			// Tell Filter to load next Mod
			p_mod->p_filter->loadNextMod(now);
		}
	}

	/*	Remove mod from scheduledMods list (if present).
		Schedule stop or deactivate mod.
		Add to scheduledMods list if timer was set.
		*/
	void tryModStop(Mod * p_mod, mtime_t now)
	{
		// Remove from scheduled list
		p_mod->removeFromScheduledList();
		// Calculate delay until stop in microseconds
		mtime_t delay = p_mod->calcDeactivationDelay(now);
		if (delay) {
			// Reschedule stop if stop time is not reached
			p_mod->schedule( delay, tryModStop );
			// add to scheduled list
			p_mod->addToScheduledList();
		} else {
			// Deactivate effect
			p_mod->deactivate();
		}
	}

	inline void setMute( bool on_off ) { // todo: needs intf lock; otherwise, danger of race condition (which, unfortunately, leads to interface freeze)
		cout << "is muted? " << aout_IsMuted(VLC_OBJECT(pl_Get(p_obj))) << endl;
		// if ( aout_IsMuted( VLC_OBJECT(pl_Get(p_obj)) ) != on_off )
			{ aout_SetMute( VLC_OBJECT(pl_Get(p_obj)), &volume, on_off ); }
		cout << "is muted? " << aout_IsMuted(VLC_OBJECT(pl_Get(p_obj))) << endl;
	}
}

/* Filter functions */
namespace Moviesoap
{
	/* Sort mods in modList by start time */
	void Filter::sort() { modList.sort(); }

	void Filter::nullify()
	{
		filepath.clear();
		creator.clear();
		title.clear();
		isbn.clear();
		year.clear();
		modList.clear();
		scheduledMods.clear();
		queuedMod = modList.begin();
	}

	/* Returns iterator to mod for index */
	list<Mod>::iterator Filter::getMod(int i)
	{
		if (i < 0 || i > modList.size())
			return list<Mod>::iterator(NULL);
		list<Mod>::iterator iter = modList.begin();
		advance(iter, i);
		return iter;
	}

	// ACTIVATING AND DEACTIVATING MODS

	void Filter::Restart() { Restart(MoviesoapGetNow(p_input)); }

	/* Set queuedMod to first mod. Call loadNextMod */
	void Filter::Restart(mtime_t now) {
		// Do nothing if there is no input thread.
		if (!p_input)
			return;
		queuedMod = modList.begin();
		#ifdef MSDEBUG2
			cout << "FILTER RESTART (first mod: " << (*queuedMod).description << ")" << endl;
		#endif
		loadNextMod( now );
	}

	/* Destroy active timers. Undo any active effects (mute & blackout). Empty scheduledMods list. */
	void Filter::Stop()
	{
		#ifdef MSDEBUG2
			cout << "FILTER STOP. " << scheduledMods.size() << " mods active." << endl;
		#endif
		// iterate through scheduled/active mods
		list<Mod*>::iterator iter;
		for ( iter = scheduledMods.begin(); iter != scheduledMods.end(); iter++ ) {
			Mod * p_mod = *iter;
			// Continue if iterator points to nothing
			if (!p_mod) continue;
			cout << "-- stop mod: " << p_mod->description << endl;
			// End effect
			p_mod->deactivate();
			cout << "-- stop mod: deactivated" << endl;
			// Destroy active timer
			vlc_timer_destroy( p_mod->timer );
			cout << "-- stop mod: timer destroyed" << endl;
		}
		#ifdef MSDEBUG2
			cout << "FILTER STOP: timers destroyed. " << endl;
		#endif
		scheduledMods.clear();
		#ifdef MSDEBUG2
			cout << "FILTER STOP COMPLETE. " << endl;
		#endif
	}

	// /* Load queuedMod. Stop Filter if end reached. */
	// void Filter::loadNextMod() { loadNextMod(MoviesoapGetNow(p_input)); }

	/* Load queuedMod. Stop Filter if end reached. (Arg 'now' should be in microseconds) */
	void Filter::loadNextMod(int64_t now)
	{
		cout << "--loadNextMod" << endl;
		// Find next mod whose stop time is not passed
		for ( ; queuedMod != modList.end(); queuedMod++ ) {
			#ifdef MSDEBUG2
				cout << "MOD SEEK -- now: " 
					<< now / MOVIESOAP_MOD_TIME_FACTOR << 
					" start: " << queuedMod->mod.start <<
					" stop: " << queuedMod->mod.stop << " " << (*queuedMod).description << endl;
			#endif
			if ( shouldEnqueueMod(&*queuedMod, now, MOVIESOAP_UNIVERSAL_TITLE, p_input) ) {
				#ifdef MSDEBUG2
					cout << "LOADING MOD..." << endl;
				#endif
				queuedMod->out(cout);
				Mod * p_mod = &*queuedMod;
				p_mod->p_filter = this;
				queuedMod++;
				tryModStart( p_mod, now );
				return; // break
			}
			#ifdef MSDEBUG2
				else {
					cout << "NOT LOADED" << endl;
				}
			#endif
		}
		cout << "No further mods to load." << endl;
	}
	
	/* Create a filter with a few test mods for testing */
	Filter * Filter::dummy()
	{
		Mod * p_mod;
		Filter * p_filter = new Filter();
		p_filter->filepath = "/home/markham/tmp/dummy-filter.cln";
		p_filter->creator = "MJ";
		p_filter->title = "Labour of Love";
		p_filter->isbn = "1234567890";
		p_filter->year = "1987";
		// add mod
		p_mod = new Mod( MOVIESOAP_SKIP, 500, 800, 0, 0 );
		p_mod->description = "skip talking";
		p_filter->modList.push_back( *p_mod );
		// add mod
		p_mod = new Mod( MOVIESOAP_BLACKOUT, 300, 1300, 5, 6, 5, 30, 400, 60 );
		p_mod->description = "blackout (cover) something";
		p_filter->modList.push_back( *p_mod );
		// add mod
		p_mod = new Mod( MOVIESOAP_MUTE, 1100, 2300, 0, 0 );
		p_mod->description = "mute the word \"foo\"";
		p_filter->modList.push_back( *p_mod );
		// sort
		p_filter->modList.sort();
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
	/* Constructor */
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

	bool Mod::operator<(const Mod& otherMod) const
	{
		if (mod.title == otherMod.mod.title)
			return mod.start < otherMod.mod.start;
		else
			return mod.title < otherMod.mod.title;
	}


	void Mod::activate()
	{
		#ifdef MSDEBUG2
			cout << setw(18) << "ACTIVATE MOD: " << "now(" << MoviesoapGetNow(p_input) / MOVIESOAP_MOD_TIME_FACTOR << ") " << description << endl;
		#endif
		// Implement effect
		switch(mod.mode) {
			case MOVIESOAP_SKIP:
				var_SetTime( Moviesoap::p_input, "time", stopTime() );
				break;
			case MOVIESOAP_MUTE:
				setMute( true );
				break;
			case MOVIESOAP_BLACKOUT:
				blackout_config.b_active = true;
				blackout_config.i_x1 = mod.x1;
				blackout_config.i_y1 = mod.y1;
				blackout_config.i_x2 = mod.x2;
				blackout_config.i_y2 = mod.y2;
				break;
		}
	}
	
	void Mod::deactivate()
	{
		#ifdef MSDEBUG2
			cout << setw(18) << "DEACTIVATE MOD: " << "now(" << MoviesoapGetNow(p_input) / MOVIESOAP_MOD_TIME_FACTOR << ") " << description << endl;
		#endif
		// Unimplement effect
		switch(mod.mode) {
			case MOVIESOAP_MUTE:
				setMute( false );
				break;
			case MOVIESOAP_BLACKOUT:
				blackout_config.b_active = false;
				break;
		}
	}

	void Mod::out(ostream & stream) { stream << "Mod: " << description << " (" << mod.start << "-" << mod.stop << ")" << endl; }

	void Mod::nullify()
	{
		memset(&mod, 0, sizeof(mod));
		description.clear();
		memset(&timer, 0, sizeof(timer));
	}

	void Mod::schedule( mtime_t delay_in_us, void (*callback)(void *) )
	{
		// create timer
		vlc_timer_create( &timer, callback, this );
		// calc delay in terms of clock cycles
		mtime_t delay = CALC_CLOCK_CYCLES(delay_in_us);
		// schedule timer
		vlc_timer_schedule( timer, false, delay, 0 );
	}

}

#undef CALC_CLOCK_CYCLES