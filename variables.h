#ifndef MOVIESOAP_VARIABLES_H
#define MOVIESOAP_VARIABLES_H
#define MOVIESOAP_VERSION "0.3.0"
#define MOVIESOAP_FILTER_VARNAME "moviesoap-filter"
#define MOVIESOAP_BLACKOUT_VARNAME "moviesoap-blackout-config"
#define MOVIESOAP_TOLERANCE_COUNT 5
#define MOVIESOAP_FILE_EXT ".cln"
#define MOVIESOAP_FILECHOOSER_FILTER "Filter files (*.cln)"
#define MOVIESOAP_MOD_TIME_FACTOR 10000 // start and stop times on mod are stored in centiseconds, but the application uses microseconds for its time measurements
#define MOVIESOAP_MOD_TIME_TO_US(mod_time) (mod_time * MOVIESOAP_MOD_TIME_FACTOR) // calculate microseconds equivalent of moviesoap_mod_t times
#define MOVIESOAP_NO_RESTART -1
#define MOVIESOAP_UNIVERSAL_TITLE -1 // if a mod has a title of -1, it can be queued for any title on the DVD
#define MOVIESOAP_QUICK_MOD_CREATION_OFFSET_DEFAULT 100
#define MOVIESOAP_QUICK_MOD_CREATION_STOP_OFFSET_DEFAULT 100

#ifndef HAVE_POLL
	struct pollfd;
	int poll(struct pollfd*, unsigned, int);
	#define HAVE_POLL
#endif

#include <vlc_common.h>

#define MSDEBUG1
#define MSDEBUG2
#define MSDEBUG3

enum {
	MOVIESOAP_SUCCESS,
	MOVIESOAP_ERROR,
	MOVIESOAP_ENOMEM,
	MOVIESOAP_FORM_INVALID,
	MOVIESOAP_ENOFPATH,
	MOVIESOAP_CANCEL,
	MOVIESOAP_ENOFILE,
	MOVIESOAP_EFILEIO,
	MOVIESOAP_EFIO_WRNG_HDR
};

/************************************************
* blackout config
************************************************/

typedef struct {
	bool b_active;
	uint16_t i_x1, i_y1, i_x2, i_y2;
} moviesoap_blackout_config_t;

/************************************************
* moviesoap_mod_t
************************************************/

typedef struct {
	uint8_t mode,	// MOVIESOAP_SKIP, MOVIESOAP_MUTE, MOVIESOAP_BLACKOUT
		category,	// MOVIESOAP_CAT_NONE, MOVIESOAP_CAT_BIGOTRY, MOVIESOAP_CAT_BLASPHEMY, etc.
		severity;	// 1..5
	short title;		// which 'title', e.g. if DVD of TV episodes is playing
	uint32_t start, stop;		// trigger to start and cease this mod's application
	uint16_t x1, y1, x2, y2;	// for blackout box, if applicable
} moviesoap_mod_t;

/************************************************
* moviesoap_filter_t
************************************************/

typedef struct {
	uint16_t n, i; // quantity of mods, index of next mod
	moviesoap_mod_t * p_mods; // pointer to array of mods
} moviesoap_filter_t;

/************************************************
* Function prototypes
************************************************/

/* (Unused) Return pointer to currently loaded Moviesoap filter. */
moviesoap_filter_t * MoviesoapGetFilter( vlc_object_t * p_obj );

/* (Unused) Set address for current Moviesoap filter. (You should usually free the existing filter before doing this by calling Moviesoap_FreeFilter.) */
void Moviesoap_SetFilter( vlc_object_t * p_obj, moviesoap_filter_t * p_filter );

/* (Unused) Deallocate memory for currently loaded filter and its array of mods. */
void Moviesoap_FreeFilter( vlc_object_t * p_obj );

/* Set fields for blackout box (they'll be copied over from the supplied arg to the actual struct). This function requires that the module in moviesoap/video-filter.c have been loaded (it sets the its config address as a public variable, attached to p_libvlc). */
void Moviesoap_SetBlackout( vlc_object_t * p_obj, moviesoap_blackout_config_t * p_new_blackout_config );

/************************************************
* Mod Modes
************************************************/

enum {
	MOVIESOAP_SKIP,
	MOVIESOAP_MUTE,
	MOVIESOAP_BLACKOUT,
	MOVIESOAP_MODE_COUNT
};

/************************************************
* Offensive content categories
************************************************/

enum {
	MOVIESOAP_CAT_NONE,
	MOVIESOAP_CAT_BIGOTRY, // racism, etc.
	MOVIESOAP_CAT_BLASPHEMY, // sacrilege
	MOVIESOAP_CAT_DISRESPECT, // dishonour parent
	MOVIESOAP_CAT_NUDITY, // immodesty
	MOVIESOAP_CAT_PROFANITY,
	MOVIESOAP_CAT_SEXUALITY, // non-nudity
	MOVIESOAP_CAT_VIOLENCE,
	MOVIESOAP_CAT_VULGARITY, // crudity
	MOVIESOAP_CAT_COUNT
};

#endif