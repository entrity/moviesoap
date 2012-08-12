#ifndef MOVIESOAP_FILTER_HPP
#define MOVIESOAP_FILTER_HPP

#include <QWidget>

#include "variables.h"
#include <vlc_common.h>
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
		string
			filepath,
			creator,
			title,
			isbn,
			year;
		list<Mod> modList;
		/* Writes filter to file */
		int save();
		/* Reads filter from file */
		static Filter load();
		/* Sets this filter as active filter for player */
		void set();
		/* Sorts mods in order of ascending start time */
		void sortMods();
		/* Returns directory for saving Moviesoap filters. If this filter has valid filepath, returns its dir, else static saveDir, else application's datadir. */
		string& saveDir();
		/* Returns a dummy Filter for testing. */
		static Filter * dummy();
		int metaOut(ofstream & out);
		int dataOut(ofstream & out);
	};

	class Mod
	{
	public:
		/* Fields */
		moviesoap_mod_t mod;
		string description; // used for meta file
		/* Constructor */
		Mod(uint8_t mode,
		uint32_t start, uint32_t stop,
		uint8_t category=0, uint8_t severity=0,
		uint16_t x1=0, uint16_t y1=0, uint16_t x2=100, uint16_t y2=100);
		/* Used for sorting. */
		bool compare(Mod& otherMod);
	};
}

#endif