#include "qt4/main.hpp"
#include "variables.h"
#include "filter.hpp"

#include "tar/ustar.h"

#include <string>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
using namespace std;


namespace Moviesoap
{

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
		unsigned int size = sizeof(moviesoap_mod_t) * modList.length();
		// make header
		Entrity::Ustar::header header("data", size);
		// write header
		header.out(outs);
		// write data
		for (iter = modList.begin(); iter != modList.end(); iter++)
			outs.write(&iter->mod, sizeof(moviesoap_mod_t));
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
		dataOut(outs):
		// write two empty blocks & close stream & return
		for (int i=0; i < 512*2; i++) { outs.put(0); }
		outs.close();
		return MOVIESOAP_SUCCESS;
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
}