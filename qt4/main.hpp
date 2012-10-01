#ifndef MOVIESOAP_MAIN_H
#define MOVIESOAP_MAIN_H
#define MOVIESOAP_VERSION 0.3.0

#include <vlc_common.h>
#include <vlc_interface.h>
#include <vlc_configuration.h>
#include <vlc_aout.h>			// audio_volume_t
#include <vlc_playlist.h>		// playlist_t
#include <QMenu>

#include "../filter.hpp"
#include "../variables.h"

class MainInterface;
class QWidget;
class QMenuBar;

enum {
	MOVIESOAP_SUCCESS,
	MOVIESOAP_ERROR,
	MOVIESOAP_ENOMEM,
	MOVIESOAP_FORM_INVALID,
	MOVIESOAP_ENOFPATH,
	MOVIESOAP_CANCEL
};

namespace Moviesoap {

	/* Fields */

	extern vlc_object_t * p_obj;
	extern playlist_t * p_playlist;
	extern input_thread_t * p_input;
	extern Filter * loadedFilter;
	extern audio_volume_t volume; // holds value to make mute possible
	extern vlc_mutex_t lock;
	extern moviesoap_blackout_config_t blackout_config;
	extern struct config_t {
		/* fields */
		bool active;
		uint8_t tolerances[MOVIESOAP_CAT_COUNT];
		/* functions */
		bool ignoreMod(Mod & mod); // Returns whether given Mod should not be activated
	} config;
	// extern MainInterface * mainInterface;
	// bool isValidSavePath() { return true; } // todo

	/* Functions */

	extern void init( intf_thread_t * p_intf, MainInterface * mainInterface );
	/* Returns vlc's data dir. */
	// static const char * datadir() { return Moviesoap::p_obj ? config_GetDataDir(Moviesoap::p_obj) : NULL; }
	/* Returns vlc's config dir. */
	// static const char * confdir() { return config_GetConfDir(); }
}


#endif