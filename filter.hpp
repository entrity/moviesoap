#ifndef MOVIESOAP_FILTER_HPP
#define MOVIESOAP_FILTER_HPP

#include <QWidget>

#include "variables.h"
#include <vlc_common.h>
#include <vlc_threads.h>
#include <list>
#include <string>
using namespace std;

namespace Moviesoap {
	class Filter;
	class Mod;

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
			isbn,
			year;
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
		/* Write tar header, data and padding to stream */
		int metaOut(ofstream &);
		int metaIn(istream &);
		/* Write tar header, data and padding to stream */
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
		/* Destroy timers. Undo any active effects (mute & blackout) */
		void Stop();
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

		bool operator<(const Mod& otherMod) const { return mod.start < otherMod.mod.start; }
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
}

#endif