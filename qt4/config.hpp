#ifndef MOVIESOAP_CONFIG_H
#define MOVIESOAP_CONFIG_H

#include "../variables.h"
#include <string>
using namespace std;

namespace Moviesoap
{
	class Mod;

	extern struct config_t {
		/* fields */
		bool active;
		uint8_t tolerances[MOVIESOAP_CAT_COUNT];

		/* functions */	
		/* Write user preferences (tolerances) to file */
		void save();
		/* Load user preferences (tolerances) from file */
		void load();
		/* Set necessary callbacks on playlist and input thread */
		void activate();
		/* Delete callbacks */
		void deactivate();
		/* Returns app data dir */
		inline string fpath();
		/* Returns whether given Mod should not be activated */
		bool modExceedsTolerance(Mod *);
		
	} config;

}

#endif