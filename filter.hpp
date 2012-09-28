#ifndef MOVIESOAP_FILTER_HPP
#define MOVIESOAP_FILTER_HPP
#define MOVIESOAP_MOD_TIME_FACTOR 10000

#include <QWidget>

#include "variables.h"
#include <vlc_common.h>
#include <vlc_threads.h>
#include <list>
#include <string>
#include <iostream>
using namespace std;

namespace Moviesoap {
	class Filter;
	class Mod;

	/* Implement mod effect, schedule stop, destroy start timer */
	void activateMod(void * p_data);
	/* Destroy stop timer. End effect. */
	void deactivateMod(void * p_data);

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

		// TESTING, DIAGNOSTIC

		/* Returns a dummy Filter for testing. */
		static Filter * dummy();
		/* Output filter data to stdout */
		void toStdout();

		/* Sets this filter as active filter for player */
		void set();
		/* Sorts mods in order of ascending start time */
		void sortMods();
		/* Returns directory for saving Moviesoap filters. If this filter has valid filepath, returns its dir, else static saveDir, else application's datadir. */
		string& saveDir();
				
		// FILE IO
		
		/* Write tar header, data and padding to stream */
		int metaOut(ofstream & out);
		/* Write tar header, data and padding to stream */
		int dataOut(ofstream & out);
		/* Writes filter to file */
		int save();
		/* Reads filter from file */
		static Filter load();
		
		// PLAYING FUNCTIONS
	
		/* Set p_queuedMod to first mod, load same. */
		void Restart();
		/* Set p_queuedMod to first mod, load same. */
		void Restart(mtime_t);
		/* Destroy timers. Undo any active effects (mute & blackout) */
		void Stop();
		/* Load queuedMod. */
		void loadNextMod();
		/* Load queuedMod. */
		void loadNextMod(mtime_t);
		/* Schedule start of mod or activate mod */
		void loadMod(Mod *, mtime_t);
	};

	class Mod
	{
	public:
		// FIELDS
		moviesoap_mod_t mod; // struct holding data for implementation of mod
		string description; // used for meta file
		vlc_timer_t timer; // holds data for activation or deactivation of mod
		Filter * p_filter; // holds list of Mods with active timers (may include this Mod); used for Moviesoap::deactivateMod
		
		/* Constructor */
		Mod(uint8_t mode,
		uint32_t start, uint32_t stop,
		uint8_t category=0, uint8_t severity=0,
		uint16_t x1=0, uint16_t y1=0, uint16_t x2=100, uint16_t y2=100);
		
		/* Used for sorting. */
		bool compare(Mod& otherMod);
		/* Used for removing a Mod from a list. Returns true if their address' are the same (i.e. they are actually the same object) */
		bool operator==(const Mod & other) const;

		void activate();
		void deactivate();
		void out(ostream & stream);
	};
}

#endif