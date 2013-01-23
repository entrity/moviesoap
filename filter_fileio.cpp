#include "filter.hpp"
#include "tar/ustar.h"

#include <sstream>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;


namespace Moviesoap
{	
	/* Calculate size of meta file (meta + mod descriptions) */
	size_t Filter::metaSize()
	{
		size_t size = 0;
		// calc length of filter meta + "\n"
		size += 1 + creator.length();
		size += 1 + title.length();
		size += 1 + isbn.length();
		size += 1 + year.length();
		// calc length of mod descriptions + "\n"
		for (list<Mod>::iterator iter = modList.begin(); iter != modList.end(); iter++) {
			size += 1 + iter->description.length(); // the +1 is for the endl char, which we'll write in the next loop
		}
		return size;
	}

	// FILE IO

	/* Write filter's text data (meta & mod descriptions) to outstream */
	int Filter::metaOut(ofstream & outs)
	{
		// make header
		Entrity::Ustar::Header header("meta.txt", metaSize());
		// write tar header
		header.out(outs);
		// write file meta
		outs << creator << endl;
		outs << title << endl;
		outs << isbn << endl;
		outs << year << endl;
		// write mod descriptions
		for (list<Mod>::iterator iter = modList.begin(); iter != modList.end(); iter++) {
			outs << iter->description << endl;
			if (!outs) return MOVIESOAP_EFILEIO;
		}
		// write padding
		header.pad(outs);
		return outs ? MOVIESOAP_EFILEIO : MOVIESOAP_SUCCESS;
	}

	int Filter::metaIn(istream & ins)
	{
		// read tar file header
		Entrity::Ustar::Header header(ins);
		if (header.name().compare("meta.txt")) return MOVIESOAP_ENOFPATH; // if wrong archive file
		// read file meta
		getline(ins, creator);
		getline(ins, title);
		getline(ins, isbn);
		getline(ins, year);
		if (!ins) return MOVIESOAP_EFILEIO;
		// read mod descriptions
		for (list<Mod>::iterator iter = modList.begin(); iter != modList.end(); iter++) {
			getline(ins, iter->description);
			if (ins.eof()) break;
			if (!ins) return MOVIESOAP_EFILEIO;
		}
		// seek past padding
		ins.seekg(header.padding(), ios::cur);
		// return
		return MOVIESOAP_SUCCESS;
	}

	/* Write filters mod data to outstream */
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
		for (iter = modList.begin(); iter != modList.end(); iter++) {
			outs.write((char *) &iter->mod, sizeof(moviesoap_mod_t));
			if (!outs) return MOVIESOAP_EFILEIO;
		}
		// write padding
		header.pad(outs);
		// return
		return outs ? MOVIESOAP_EFILEIO : MOVIESOAP_SUCCESS;
	}

	/* Read mods from tar file. Return err/success */
	int Filter::dataIn(istream & ins)
	{
		// read tar file header
		Entrity::Ustar::Header header(ins);
		if (header.name().compare("data")) return MOVIESOAP_ENOFPATH; // if wrong archive file
		// calc number of mods
		size_t mods_n = header.size() / sizeof(moviesoap_mod_t);
		// push new mods onto list
		for (size_t i = 0; i < mods_n; i++ ) {
			if (!ins) return MOVIESOAP_EFILEIO;
			modList.push_back( Mod(ins) );
		}
		// seek past padding
		ins.seekg(header.padding(), ios::cur);
		// return
		return MOVIESOAP_SUCCESS;
	}

	/* Write filter to tar file. Return 0 if success. */
	int Filter::save()
	{
		// open file
		ofstream outs(filepath.c_str(), ios::binary );
		// return failure if couldn't write to file
		if (!outs.is_open()) return MOVIESOAP_ENOFILE;
		// write mods
		dataOut(outs);
		if (!outs) return MOVIESOAP_EFILEIO;
		// write meta
		metaOut(outs);
		if (!outs) return MOVIESOAP_EFILEIO;
		// write two empty blocks & close stream & return
		for (int i=0; i < 512*2; i++) { outs.put(0); }
		if (!outs) return MOVIESOAP_EFILEIO;
		outs.close();
		return MOVIESOAP_SUCCESS;
	}

	int Filter::load(const string & newfpath)
	{
		int status;
		ifstream ins( newfpath.c_str(), ios::binary );
		if (!ins.is_open()) return MOVIESOAP_ENOFILE;
		// set filepath
		filepath = newfpath;
		// load mods
		modList.clear();
		status = dataIn(ins);
		if (status) return status;
		// load meta
		status = metaIn(ins);
		if (status) return status;
		return MOVIESOAP_SUCCESS;
	}
}