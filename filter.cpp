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
	int Filter::save()
	{
		// define vars
		bool b;
		int i;
		uint16_t contentLength, paddingLength;
		string fpath = "output.tar"; // psz
		char header[USTAR_HEADER_SIZE];
		list<Mod>::iterator iter;

		// open stream
		ofstream out( fpath.c_str(), ios_base::out | ios_base::binary );
		
		// make meta file
		stringstream ss;
		ss << creator << endl << title << endl << isbn << endl << year << endl; // assemble filter data
		for (iter = modList.begin(); iter != modList.end(); iter++) // assemble mod descriptions
			ss << iter->description << endl;
		contentLength = ss.tellp();
		paddingLength = USTAR_HEADER_SIZE - (contentLength % USTAR_HEADER_SIZE);
		b = ustar_make_header("meta.txt", USTAR_REGULAR, contentLength, header); // build header
		out.write( (char *) &header, USTAR_HEADER_SIZE ); // write header
		out << ss.str(); // write text
		for (i = 0; i < paddingLength; i++) out.put(0); // write padding

		// make data file
		contentLength = modList.size() * sizeof(moviesoap_mod_t);	// calculate content size
		paddingLength = USTAR_HEADER_SIZE - (contentLength % USTAR_HEADER_SIZE);
		b = ustar_make_header("data", USTAR_REGULAR, contentLength, header); // build header
		out.write( (char *) &header, USTAR_HEADER_SIZE ); // write header
		for (iter = modList.begin(); iter != modList.end(); iter++) // write mods
			out.write( (char *) &iter->mod, sizeof(moviesoap_mod_t) );
		for (i = 0; i < paddingLength; i++) out.put(0); // write padding

		// write two empty blocks to signal end of file
		for (i = 0; i < 2*USTAR_HEADER_SIZE; i ++) out.put(0);
		// close stream
		out.close();
		
		printf("---- in filter save: %d\n", b);
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

		p_mod = new Mod( MOVIESOAP_BLACKOUT, 1, 10, 5, 6, 7, 8, 9, 2 );
		p_mod->description = "skip boilerplate";
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