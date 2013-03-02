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
		return outs ? MOVIESOAP_SUCCESS : MOVIESOAP_EFILEIO;
	}

	int Filter::metaIn(istream & ins)
	{
		// find archive header
		string filename = "version.txt";
		Entrity::Ustar::Header * p_header = Entrity::Ustar::Header::find(ins, filename);
		if (p_header == NULL) return MOVIESOAP_EFIO_WRNG_HDR;
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
		ins.seekg(p_header->padding(), ios::cur);
		// cleanup & return
		delete p_header;
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
		return outs ? MOVIESOAP_SUCCESS : MOVIESOAP_EFILEIO;
	}

	/* Read mods from tar file. Return err/success */
	int Filter::dataIn(istream & ins)
	{
		// find archive header
		string filename = "data";
		Entrity::Ustar::Header * p_header = Entrity::Ustar::Header::find(ins, filename);
		if (p_header == NULL) return MOVIESOAP_EFIO_WRNG_HDR;
		// calc number of mods
		size_t mods_n = p_header->size() / sizeof(moviesoap_mod_t);
		// push new mods onto list
		for (size_t i = 0; i < mods_n; i++ ) {
			if (!ins) return MOVIESOAP_EFILEIO;
			modList.push_back( Mod(ins) );
		}
		// seek past padding
		ins.seekg(p_header->padding(), ios::cur);
		// cleanup & return
		delete p_header;
		return MOVIESOAP_SUCCESS;
	}

	/* Write version number to archive file */
	int Filter::versionOut(ofstream & outs)
	{
		// prepare string representation of version
		version = MOVIESOAP_VERSION;
		// calculate size
		size_t size = 1 + version.length();
		// make header
		Entrity::Ustar::Header header("version.txt", size);
		// write header
		header.out(outs);
		// write archive content
		outs << version << endl;
		// write padding
		header.pad(outs);
		return outs ? MOVIESOAP_SUCCESS : MOVIESOAP_EFILEIO;
	}

	/* Read status from tar file. Return err/success */
	int Filter::versionIn(istream & ins)
	{
		// find archive header
		string filename = "version.txt";
		Entrity::Ustar::Header * p_header = Entrity::Ustar::Header::find(ins, filename);
		if (p_header == NULL) return MOVIESOAP_EFIO_WRNG_HDR;
		// read archive content
		getline(ins, version);
		if (!ins) return MOVIESOAP_EFILEIO;
		// seek past padding
		ins.seekg(p_header->padding(), ios::cur);
		// cleanup & return
		delete p_header;
		return MOVIESOAP_SUCCESS;
	}

	/* Write filter to tar file. Return 0 if success. */
	int Filter::save()
	{
		int status;
		// open file
		ofstream outs(filepath.c_str(), ios::binary );
		// return failure if couldn't write to file
		if (!outs.is_open()) return MOVIESOAP_ENOFILE;
		// write version
		status = versionOut(outs);
		if (status) return status;
		// write mods
		status = dataOut(outs);
		if (status) return status;
		// write meta
		status = metaOut(outs);
		if (status) return status;
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
		// load version number
		status = versionIn(ins);
		if (status) return status;
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