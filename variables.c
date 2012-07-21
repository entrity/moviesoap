#include <vlc_common.h>
#include "variables.h"
#include "video-filter.h"

/* 
 * Return pointer to currently loaded Moviesoap filter.
 */
moviesoap_filter_t * Moviesoap_GetFilter( vlc_object_t * p_obj )
{
	return (moviesoap_filter_t *) var_GetAddress(p_obj->p_libvlc, MOVIESOAP_FILTER_VARNAME);
}

/*
 * Set address for current Moviesoap filter. (You should usually free the existing filter before doing this by calling Moviesoap_FreeFilter.)
 */
void Moviesoap_SetFilter( vlc_object_t * p_obj, moviesoap_filter_t * p_filter )
{
	var_SetAddress(p_obj->p_libvlc, MOVIESOAP_FILTER_VARNAME, p_filter);
}

/*
 * Deallocate memory for currently loaded filter and its array of mods.
 */
void Moviesoap_FreeFilter( vlc_object_t * p_obj )
{
	moviesoap_filter_t * p_filter = (moviesoap_filter_t *) var_GetAddress(p_obj->p_libvlc, MOVIESOAP_FILTER_VARNAME);
	if (p_filter) {
		if (p_filter->p_mods)
			free(p_filter->p_mods);
		free(p_filter);
	}
}

/*
 * Set fields for blackout box (they'll be copied over from the supplied arg to the actual struct).
 */
void Moviesoap_SetBlackout( vlc_object_t * p_obj, moviesoap_blackout_config_t * p_new_blackout_config )
{
	moviesoap_blackout_config_t * p_actual_config =
		(moviesoap_blackout_config_t *) var_GetAddress(p_obj->p_libvlc, MOVIESOAP_BLACKOUT_VARNAME);
	*p_actual_config = *p_new_blackout_config;
}