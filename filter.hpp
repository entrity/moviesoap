#ifndef MOVIESOAP_FILTER_HPP
#define MOVIESOAP_FILTER_HPP

#include <QWidget>

namespace Moviesoap {
	class Filter;
	class Mod;
}

#include "variables.h"
#include "qt4/main.hpp"
#include "qt4/config.hpp"
#include <vlc_common.h>
// #include <vlc_threads.h>
#include <vlc_input.h>
#include <list>
#include <string>
#include <iostream>
using namespace std;

namespace Moviesoap {
	
	/* Function prototypes */
	void tryModStart(void * mod_pointer);
	void tryModStop(void * mod_pointer);
	void tryModStart(Mod * p_mod, mtime_t now);
	void tryModStop(Mod * p_mod, mtime_t now);

	class Filter
	{
		public:
		/* fields */
		string
			filepath,
			creator,
			title,
			imdb,
			year,
			version;
		list<Mod> modList; // all mods for this filter
		list<Mod*> scheduledMods; // this list lets programme know which timers to destroy when filter is stopped
		list<Mod>::iterator queuedMod; // iterator on modList

		/* Constructors */
		Filter() {}
		Filter(Filter * other) { *this = *other; }
		/* Destructor */
		~Filter() { Stop(); }

		// TESTING, DIAGNOSTIC

		/* Returns a dummy Filter for testing. */
		static Filter * dummy();
		/* Output filter data to stdout */
		void toStdout();

		/* Sorts mods in order of ascending start time */
		void sort();
		/* Returns directory for saving Moviesoap filters. If this filter has valid filepath, returns its dir, else static saveDir, else application's datadir. */
		string& saveDir();
		/* Nullifies strings & pointers, empties lists */
		void nullify();
		/* Returns iterator to mod for index */
		list<Mod>::iterator getMod(int i);
				
		// FILE IO
		
		size_t metaSize();
		/* Write archive header, version, and padding to stream */
		int versionOut(ofstream &);
		int versionIn(istream &);
		/* Write archive header, data, and padding to stream */
		int metaOut(ofstream &);
		int metaIn(istream &);
		/* Write archive header, data, and padding to stream */
		int dataOut(ofstream &);
		int dataIn(istream &);
		/* Writes filter to file */
		int save();
		/* Reads filter from file. (Clears existing modList first) */
		int load(const string &);
		
		// PLAYING FUNCTIONS
	
		/* Set p_queuedMod to first mod, load same. */
		void Restart();
		/* Set p_queuedMod to first mod, load same. */
		void Restart(mtime_t);
		/* Destroy timers. Undo any active effects (mute & blackout). */
		void StopAndCleanup(bool leave_active_effects);
		/* Convenience method for StopAndCleanup. Destroy timers. Undo any active effects (mute & blackout). */
		void Stop();
		/* Convenience method for StopAndCleanup. Destroy timers, but leave active effects (mute & blackout) as they are. */
		void KillTimers();
		/* Kills timers on BLACKOUT mods in scheduledMods. (This is useful if a BLACKOUT mod is to be activated but another BLACKOUT is already running. When the first BLACKOUT expires, it would kill the effect even if the seond BLACKOUT weren't expired.) */
		void KillBlackouts();
		

		/* Load queuedMod. */
		// void loadNextMod();
		/* Load queuedMod. */
		void loadNextMod(mtime_t);
	};

	class Mod
	{
	public:
		// FIELDS
		moviesoap_mod_t mod; // struct holding data for implementation of mod
		string description; // used for meta file
		vlc_timer_t timer; // holds data for activation or deactivation of mod
		Filter * p_filter; // Largely safe to ignore. Set by Filter::loadNextMod(). Points to mod's owner so that mod can find Filter::scheduledMods, so that mod can remove itself from Filter::scheduledMods when tryModStop callback fires.
		
		/* Constructor(s) */
		Mod(uint8_t mode=0,
		uint32_t start=0, uint32_t stop=0,
		uint8_t category=0, uint8_t severity=0,
		uint16_t x1=0, uint16_t y1=0, uint16_t x2=0, uint16_t y2=0);

		/* Constructor. Reads moviesoap_mod_t data from stream. Creates mod with same. */
		Mod(istream & ins) { ins.read( (char *) &mod, sizeof(moviesoap_mod_t) ); }

		bool operator<(const Mod& otherMod) const;
		bool operator==(const Mod& otherMod) const { return this == &otherMod; }

		/* Returns true if this Mod's mode yields an active effect (requiring scheduling of its stop time) */
		bool producesActiveEffect() { return mod.mode != MOVIESOAP_SKIP; }

		/* Return Mod's start time in microseconds */
		uint32_t startTime() { return MOVIESOAP_MOD_TIME_TO_US(mod.start); }
		/* Return Mod's stop time in microseconds */
		uint32_t stopTime() { return MOVIESOAP_MOD_TIME_TO_US(mod.stop); }
		/* Return microseconds until Mod's start time */
		mtime_t calcActivationDelay(mtime_t now) { mtime_t stime = startTime(); return now < stime ? (stime - now) : 0; }
		/* Return microseconds until Mod's stop time */
		mtime_t calcDeactivationDelay(mtime_t now) { mtime_t stime = stopTime(); return now < stime ? (stime - now) : 0; }

		void addToScheduledList() { p_filter->scheduledMods.push_back( this ); }
		void removeFromScheduledList() { p_filter->scheduledMods.remove( this ); }

		/* Create and schedule timer. This does NOT add the Mod to scheduledMods list. */
		void schedule( mtime_t delay_in_us, void (*callback)(void *) );
		/* Implement effect */
		void activate();
		/* Unimplement effect */
		void deactivate();
		/* Debugging method: describes Mod in plain text */
		void out(ostream & stream);
		/* Sets all fields to 0 */
		void nullify();
	};

	/* Inline functions */
	/* Get current time in play (in us) */
	static inline int64_t MoviesoapGetNow(input_thread_t * p_input)
		{ int64_t us = 0; if (p_input) input_Control( p_input, INPUT_GET_TIME, &us ); return us; }

	/* If this function should derive current time and title on its own, set those to -1. */
	static inline bool shouldEnqueueMod(Mod * mod, int64_t i_now, int i_title, input_thread_t * p_input)
	{
		if (p_input) {
			if (i_now == -1) i_now = MoviesoapGetNow(p_input);
			if (i_title == MOVIESOAP_UNIVERSAL_TITLE && mod->mod.title != MOVIESOAP_UNIVERSAL_TITLE) i_title = var_GetInteger(p_input, "title");
			if (
				// check that current title matches mod title
				(mod->mod.title == MOVIESOAP_UNIVERSAL_TITLE || mod->mod.title == i_title)
				// check that stop time is not past
				&& (mod->mod.stop * MOVIESOAP_MOD_TIME_FACTOR) > i_now
				// check that mod severity exceeds user tolerance
				&& Moviesoap::config.modExceedsTolerance(mod)
				) return true;
		}
		return false;
	}
}

#endif