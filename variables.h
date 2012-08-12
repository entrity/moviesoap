#ifndef MOVIESOAP_VARIABLES_H
#define MOVIESOAP_VARIABLES_H
#define MOVIESOAP_FILTER_VARNAME "moviesoap-filter"
#define MOVIESOAP_BLACKOUT_VARNAME "moviesoap-blackout-config"

#include "video-filter.h"

/************************************************
* moviesoap_mod_t
************************************************/

typedef struct {
	uint8_t mode,	// MOVIESOAP_SKIP, MOVIESOAP_MUTE, MOVIESOAP_BLACKOUT
		category,	// MOVIESOAP_CAT_NONE, MOVIESOAP_CAT_BIGOTRY, MOVIESOAP_CAT_BLASPHEMY, etc.
		severity,	// 1..5
		unused;	
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

/* Return pointer to currently loaded Moviesoap filter. */
moviesoap_filter_t * MoviesoapGetFilter( vlc_object_t * p_obj );

/* Set address for current Moviesoap filter. (You should usually free the existing filter before doing this by calling Moviesoap_FreeFilter.) */
void Moviesoap_SetFilter( vlc_object_t * p_obj, moviesoap_filter_t * p_filter );

/* Deallocate memory for currently loaded filter and its array of mods. */
void Moviesoap_FreeFilter( vlc_object_t * p_obj );

/* Set fields for blackout box (they'll be copied over from the supplied arg to the actual struct). */
void Moviesoap_SetBlackout( vlc_object_t * p_obj, moviesoap_blackout_config_t * p_new_blackout_config );

/************************************************
* Mod Modes
************************************************/

enum {
	MOVIESOAP_SKIP,
	MOVIESOAP_MUTE,
	MOVIESOAP_BLACKOUT
};

/************************************************
* Offensive content categories
************************************************/

enum {
	MOVIESOAP_CAT_NONE,
	MOVIESOAP_CAT_BIGOTRY, // racism, etc.
	MOVIESOAP_CAT_BLASPHEMY, // sacrilege
	MOVIESOAP_CAT_CONTENTION,
	MOVIESOAP_CAT_DISRESPECT_FAMILY, // dishonour parent
	MOVIESOAP_CAT_OTHER,
	MOVIESOAP_CAT_NUDITY, // immodesty
	MOVIESOAP_CAT_PROFANITY,
	MOVIESOAP_CAT_SEXUALITY, // non-nudity
	MOVIESOAP_CAT_VIOLENCE,
	MOVIESOAP_CAT_VULGARITY // crudity
};

#endif