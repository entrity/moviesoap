#include "config.hpp"
#include "main.hpp"
#include "../filter.hpp"
#include <fstream>
using namespace std;

#define MOVIESOAP_CONF_FNAME "/moviesoap.conf" // used for config file io

namespace Moviesoap
{
	/* Initialize extern vars */
	struct config_t config;

	/* Returns true if given content for given mod is tolerable to user's config. */
	bool config_t::ignoreMod(Moviesoap::Mod & mod)
	{
		uint8_t tolerance = tolerances[mod.mod.category];
		return tolerance > mod.mod.severity;
	}

	void config_t::save()
	{
		ofstream out;
		out.open(ConfigFpath(), ios_base::out | ios_base::trunc | ios_base::binary);
		if (!out) { cerr << "Could not open file in Moviesoap::config_t::save." << endl; exit(1); }
		out.write( (char *)&tolerances, MOVIESOAP_CAT_COUNT*sizeof(uint8_t) );
		if (!out) { cerr << "Could not write to file in Moviesoap::config_t::save." << endl; exit(1); }
		out.close();
	}

	void config_t::load()
	{
		ifstream in;
		in.open(ConfigFpath(), ios::in | ios::binary);
		if (!in) { cout << "No in config file." << endl; return; };
		in.read( (char *)&tolerances, MOVIESOAP_CAT_COUNT*sizeof(uint8_t) );
		if (!in) { cerr << "Could not read from file in Moviesoap::config_t::load." << endl; exit(1); }
		in.close();

		cout << "read bytes: " << MOVIESOAP_CAT_COUNT*sizeof(uint8_t) << endl;
		for (int i = 0; i < MOVIESOAP_CAT_COUNT; i++)
			cout << " " << dec << (unsigned int) tolerances[i];
		cout << endl;
	}

	/* Returns path of config dir. */
	char * config_t::ConfigFpath()
	{
		if (!confPath && Moviesoap::p_obj) {
			char * dir = config_GetDataDir( Moviesoap::p_obj );
			confPath = (char *) malloc(strlen(dir) + strlen(MOVIESOAP_CONF_FNAME) + 1);
			strcpy(confPath, dir);
			free(dir);
			strcat(confPath, MOVIESOAP_CONF_FNAME);
		}
		return confPath;
	}

}

#undef MOVIESOAP_CONF_FNAME