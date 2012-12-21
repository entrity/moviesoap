#ifndef MOVIESOAP_MAIN_H
#define MOVIESOAP_MAIN_H
#define MOVIESOAP_VERSION 0.3.0

#include "../filter.hpp"
#include "../variables.h"

#include <string>
using namespace std;

#include <vlc_common.h>
#include <vlc_interface.h>
#include <vlc_configuration.h>
#include <vlc_aout.h>			// audio_volume_t
#include <vlc_playlist.h>		// playlist_t

class MainInterface;
class QMenu;

// MISC (main.cpp)
namespace Moviesoap 
{
	class Menu;

	extern string saveDir();
	extern string extractDir(const string& str);
	extern const char * toleranceLabels[];
	extern Moviesoap::Menu * p_GuiMenu; // this is the 'Moviesoap' menu in the main menu bar
}

// CALLBACKS (callbacks.cpp)
namespace Moviesoap
{
	/* Fields */
	extern vlc_object_t * p_obj;
	extern playlist_t * p_playlist;
	extern input_thread_t * p_input;
	extern Filter * p_loadedFilter;
	extern audio_volume_t volume; // holds value to make mute possible
	extern vlc_mutex_t lock;
	extern moviesoap_blackout_config_t blackout_config;

	/* Functions */
	extern void init( intf_thread_t * p_intf, MainInterface * mainInterface, QMenu * p_menu );
}

// BLACKOUT (filter-chain.h)
namespace Moviesoap
{
	extern filter_t *p_blackout_filter;
}

// CONFIG (config.cpp)
namespace Moviesoap
{
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
		bool ignoreMod(Mod &);
		
	} config;
}


#endif