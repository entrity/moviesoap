#ifndef MOVIESOAP_FILTER_HPP
#define MOVIESOAP_FILTER_HPP

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
		list<Mod> modList;
		list<Mod> scheduledMods;
		Mod * p_queuedMod;

		/* Returns a dummy Filter for testing. */
		static Filter * dummy();

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
		/* Destroy timers. Undo any active effects (mute & blackout) */
		void Stop();
		/* Load queuedMod */
		void loadNextMod();
		/* Schedule start of mod or activate mod */
		void loadMod(Mod &);
		/* Implement mod effect, schedule stop, destroy start timer */
		static void activateMod(void * p_data);
		/* Destroy stop timer. End effect. */
		static void deactivateMod(void * p_data);
	};

	class Mod
	{
	public:
		// FIELDS
		moviesoap_mod_t mod;
		string description; // used for meta file
		vlc_timer_t timer;
		
		/* Constructor */
		Mod(uint8_t mode,
		uint32_t start, uint32_t stop,
		uint8_t category=0, uint8_t severity=0,
		uint16_t x1=0, uint16_t y1=0, uint16_t x2=100, uint16_t y2=100);
		
		/* Used for sorting. */
		bool compare(Mod& otherMod);

		void activate();
		void deactivate();
	};
}

#endif