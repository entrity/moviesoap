#include "config.hpp"
#include "main.hpp"
#include "../filter.hpp"
#include <fstream>
#include <iostream>
using namespace std;

#define MOVIESOAP_CONF_FNAME "/moviesoap.conf" // used for config file io

namespace Moviesoap
{
	/* Initialize extern vars */
	struct config_t config;

	/* Returns true if given content for given mod is not tolerable to user's config. */
	bool config_t::modExceedsTolerance(Moviesoap::Mod * p_mod)
	{
		uint8_t tolerance = tolerances[p_mod->mod.category];
		return tolerance < p_mod->mod.severity;
	}

	void config_t::save()
	{
		ofstream out;
		out.open(fpath().c_str(), ios_base::out | ios_base::trunc | ios_base::binary);
		if (!out) { cerr << "Could not open file in Moviesoap::config_t::save." << endl; exit(1); }
		out.write( (char *)&tolerances, MOVIESOAP_CAT_COUNT*sizeof(uint8_t) );
		if (!out) { cerr << "Could not write to file in Moviesoap::config_t::save." << endl; exit(1); }
		out.close();
	}

	void config_t::load()
	{
		ifstream in;
		in.open(fpath().c_str(), ios::in | ios::binary);
		if (!in) { cout << "No in config file." << endl; return; };
		in.read( (char *)&tolerances, MOVIESOAP_CAT_COUNT*sizeof(uint8_t) );
		if (!in) { cerr << "Could not read from file in Moviesoap::config_t::load." << endl; exit(1); }
		in.close();
	}

	/* Returns path of config dir. */
	string config_t::fpath()
	{
		char * dir = config_GetDataDir();
		// string confPath = (char *) malloc(strlen(dir) + strlen(MOVIESOAP_CONF_FNAME) + 1);
		string confPath(dir);
		free(dir);
		confPath += MOVIESOAP_CONF_FNAME;
		return confPath;
	}

}

#undef MOVIESOAP_CONF_FNAME