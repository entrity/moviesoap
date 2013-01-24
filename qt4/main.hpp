#ifndef MOVIESOAP_MAIN_H
#define MOVIESOAP_MAIN_H

#include "../filter.hpp"
#include "../variables.h"

#include <string>
using namespace std;

#include <vlc_common.h>
#include <vlc_interface.h>
#include <vlc_configuration.h>
#include <vlc_aout_volume.h>			// audio_volume_t
#include <vlc_playlist.h>		// playlist_t

class MainInterface;
class QMenu;

// MISC (main.cpp)
namespace Moviesoap 
{
	class Menu;

	extern string saveDir();
	extern string extractDir(const string& str);
	extern bool isActiveSelected();
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
	// Sets var Moviesoap::p_input & Moviesoap::p_playlist if not set or `force_overwrite`
	extern void spawn_set_p_input(bool force_overwrite);
	extern void spawn_restart_filter();
	extern void spawn_stop_filter();
}

// BLACKOUT (filter-chain.h)
namespace Moviesoap
{
	extern filter_t *p_blackout_filter;
}

// CONFIG (config.cpp)
// namespace Moviesoap
// {
// 	extern struct config_t config;
// }


#endif