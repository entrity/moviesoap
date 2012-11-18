/* INPUT EVEN VARIABLES
 * The read-write variables are:
 *  - state (\see input_state_e)
 *  - rate
 *  - position, position-offset
 *  - time, time-offset
 *  - title, next-title, prev-title
 *  - chapter, next-chapter, next-chapter-prev
 *  - program, audio-es, video-es, spu-es
 *  - audio-delay, spu-delay
 *  - bookmark (bookmark list)
 *  - record
 *  - frame-next
 *  - navigation (list of "title %2i")
 *  - "title %2i"
 */

#include <cstdio>
#include <cassert>

#include <vlc_common.h>
#include <vlc_interface.h>
#include <vlc_modules.h>
#include <vlc_aout_intf.h>	// aout_ToggleMute

#include "main.hpp"
#include "config.hpp"
#include "../filter.hpp"
#include "../variables.h"
#include "../filter-chain.h"

#ifndef INT64_C	// ibid.
# define INT64_C(c)  c ## LL
#endif

#define MOVIESOAP_CALLBACK(name) int name( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data )

// test
#include <vlc_input.h> // temp used for input_GetState()
#include <vlc_threads.h> // temp used for vlc_timer...
#include <iostream>
using namespace std;

namespace Moviesoap
{
	/* Initialize static fields */
	vlc_object_t * p_obj = NULL;
	playlist_t * p_playlist = NULL;
	input_thread_t * p_input = NULL;
	Filter * p_loadedFilter = NULL;
	filter_chain_t * p_filter_chain = NULL; // chain holds blackout video filter
	audio_volume_t volume;
	vlc_mutex_t lock;
	moviesoap_blackout_config_t blackout_config;

	
	/* Playlist callbacks */
	static MOVIESOAP_CALLBACK(PlaylistCbItemChange);
	static MOVIESOAP_CALLBACK(PlaylistCbItemCurrent);
	/* Input callbacks */
	static MOVIESOAP_CALLBACK(InputCbState);
	static MOVIESOAP_CALLBACK(InputCbNavigation);
	static MOVIESOAP_CALLBACK(InputCbPosition);
	static MOVIESOAP_CALLBACK(InputCbTime);
	/* Other local prototypes */
	static inline void StopAndStartFilter();
	static inline void StopAndStartFilter(mtime_t new_time);
	
	/* Set vars, config. (Called by VLCMenuBar::createMenuBar in menus.cpp) */
	void init( intf_thread_t * p_intf, MainInterface * mainInterface )
	{
		// Set namespace vars
		Moviesoap::p_obj = VLC_OBJECT(p_intf);
		Moviesoap::p_playlist = pl_Get(p_intf);
		vlc_mutex_init( &lock );
		// build config (tolerances) from file or defaults
		config.load();
		// Create variable pointer to blackout config
		#define MOVIESOAP_BLACKOUT_CONFIG_POINTER
		var_CreateGetAddress( p_obj->p_libvlc, MOVIESOAP_BLACKOUT_VARNAME);
		var_SetAddress( p_obj->p_libvlc, MOVIESOAP_BLACKOUT_VARNAME, &blackout_config );
		// Add callback(s) to playlist
		var_AddCallback( p_playlist, "item-current", PlaylistCbItemCurrent, NULL );
	}

	/* Set by PlaylistChangeCallback. If a vout_thread exists on input, removes this callback and attaches the blackout filter to the vout. */
	static MOVIESOAP_CALLBACK(PlaylistCbItemChange)
	{
		#ifdef MSDEBUG1
		msg_Info( p_this, "!!! CALLBACK playlist item-change !!! : %s ... new: %d ... old: %d", psz_var, (int) newval.i_int, (int) oldval.i_int );
		#endif
		input_thread_t * p_input = playlist_CurrentInput( p_playlist );
		// Add blackout filter to vout if vout thread exists on input thread
		if (p_input) {
			if ( vout_thread_exists( p_input ) ) {
				// var_DelCallback( p_playlist, "item-change", PlaylistCbItemChange, NULL );
				// add_blackout_filter_to_input( p_input );
			} else {
				msg_Err( p_this, "No vout thread found for blackout." );
				if (p_blackout_filter)
					p_blackout_filter = NULL;
			}
		}
		return MOVIESOAP_SUCCESS;
	}

	/* Add callback to Input change. Start Filter object. */
	static MOVIESOAP_CALLBACK(PlaylistCbItemCurrent)
	{
		#ifdef MSDEBUG1
		msg_Info( p_this, "!!! CALLBACK playlist item-current !!! : %s ... new: %d ... old: %d", psz_var, (int) newval.i_int, (int) oldval.i_int );
		#endif
		p_playlist = (playlist_t *) p_this;
		if (p_playlist)
		{
			// Add callback(s) to playlist (for purpose of adding video filter to chain)
			var_AddCallback( p_playlist, "item-change", PlaylistCbItemChange, NULL );
			// Update p_input
			p_input = playlist_CurrentInput( p_playlist );
			if (p_input) {
				// Add callback(s) to input thread
				var_AddCallback( p_input, "position", InputCbPosition, NULL );
				var_AddCallback( p_input, "time", InputCbTime, NULL );
				var_AddCallback( p_input, "navigation", InputCbNavigation, NULL );
				var_AddCallback( p_input, "state", InputCbState, NULL );
				// start filter object if one exists
				if (p_loadedFilter) p_loadedFilter->Restart();
				return VLC_SUCCESS;
			} else {
				msg_Err( p_this, "No current input thread found." );
			}
		} else {
			msg_Err( p_this, "No playlist found." );
		}
		return VLC_ENOOBJ;
	}

	static MOVIESOAP_CALLBACK(InputCbState)
	{
		#ifdef MSDEBUG3
		msg_Info( p_this, "!!! CALLBACK input state !!! : %s ... new: %d ... old: %d", psz_var, (int) newval.i_int, (int) oldval.i_int );
		#endif
		// Stop filter if PAUSE
		// todo
		// Start filter if PLAY
		// todo
		return 0;
	}

	/* (Re)start Filter object. (This will kill existing timers.) Fires when someone moves forward or backward. */
	static MOVIESOAP_CALLBACK(InputCbTime)
	{
		#ifdef MSDEBUG3
		msg_Info( p_this, "!!! CALLBACK input time !!! : %s ... new: %d ... old: %d", psz_var, (int) newval.i_int, (int) oldval.i_int );
		#endif
		mtime_t new_time = newval.i_time;
		// StopAndStartFilter(new_time);
		return 0;
	}

	/* (Re)start Filter object. (This will kill existing timers.) Fires when user clicks position bar. */
	static MOVIESOAP_CALLBACK(InputCbPosition)
	{
		#ifdef MSDEBUG3
		msg_Info( p_this, "!!! CALLBACK input position !!! : %s ... new: %d ... old: %d", psz_var, (int) newval.i_int, (int) oldval.i_int );
		#endif
		mtime_t new_time = newval.f_float * var_GetTime( p_input, "length" );
		StopAndStartFilter(new_time);
		return 0;
	}

	static MOVIESOAP_CALLBACK(InputCbNavigation)
	{
		#ifdef MSDEBUG3
		msg_Info( p_this, "!!! CALLBACK input navigation !!! : %s ... new: %d ... old: %d", psz_var, (int) newval.i_int, (int) oldval.i_int );
		#endif
		StopAndStartFilter();
		return 0;
	}

	/* Does nothing if p_loadedFilter is null. Else, stops filter & restarts it. */
	static inline void StopAndStartFilter()
	{
		if (p_loadedFilter) {
			vlc_mutex_lock( &lock );
			p_loadedFilter->Stop();
			p_loadedFilter->Restart();
			vlc_mutex_unlock( &lock );
		}
	}

	/* Does nothing if p_loadedFilter is null. Else, stops filter & restarts it. */
	static inline void StopAndStartFilter(mtime_t new_time)
	{
		if (p_loadedFilter) {
			vlc_mutex_lock( &lock );
			p_loadedFilter->Stop();
			p_loadedFilter->Restart( new_time );
			vlc_mutex_unlock( &lock );
		}
	}

}
