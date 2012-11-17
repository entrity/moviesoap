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
		Filter * p_filter; // Largely safe to ignore. Set by Filter::loadMod(). Points to mod's owner so that mod can find Filter::scheduledMods, so that mod can remove itself from Filter::scheduledMods when Filter::deactivateMod() callback fires.
		
		/* Constructor(s) */
		Mod(uint8_t mode=0,
		uint32_t start=0, uint32_t stop=0,
		uint8_t category=0, uint8_t severity=0,
		uint16_t x1=0, uint16_t y1=0, uint16_t x2=0, uint16_t y2=0);
		/* Constructor. Reads moviesoap_mod_t data from stream. Creates mod with same. */
		Mod(istream &);
		/* Destructor */
		~Mod();
		
		/* Used for sorting. */
		bool operator<(const Mod& otherMod) const;
		/* Used for removing a Mod from a list. Returns true if their address' are the same (i.e. they are actually the same object) */
		bool operator==(const Mod & other) const;

		uint32_t startTime();
		uint32_t stopTime();

		void activate();
		void deactivate();
		void out(ostream & stream);
		void nullify();
	};
}

#endif