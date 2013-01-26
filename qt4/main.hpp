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
class QAction;
class QCheckBox;
class QLineEdit;

// MISC (main.cpp)
namespace Moviesoap 
{
	class Menu;
	class FilterWin;
	// functions
	extern string saveDir();
	extern string extractDir(const string& str);
	extern bool isActiveSelected();
	extern Mod * addEditingModToModListIfNew();
	extern void defaultizeMod(Mod * p_mod, uint32_t start_offset_backward=0, uint32_t stop_offset_forward_from_start=MOVIESOAP_QUICK_MOD_CREATION_STOP_OFFSET_DEFAULT); // loads times and title from p_input
	extern void quickCreateMod(uint8_t mode);
	// variables
	extern const char * toleranceLabels[];
	extern Mod newMod;
	extern Mod * p_editingMod;
	extern Mod * p_quickCreatedMod;
	extern Moviesoap::Menu * p_GuiMenu; // this is the 'Moviesoap' menu in the main menu bar
	extern FilterWin *p_window; // pointer to FilterWin gui
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