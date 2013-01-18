#include <cstdio>
#include <cassert>

#include "main.hpp"
#include "config.hpp"
#include "../filter.hpp"
#include "../variables.h"
#include "../filter-chain.h"
#include "../updates.hpp"

#include <vlc_common.h>
#include <vlc_interface.h>
#include <vlc_modules.h>
#include <vlc_aout_volume.h>	// aout_ToggleMute
// #include <vlc_threads.h> // temp used for vlc_timer...

#ifndef INT64_C	// ibid.
# define INT64_C(c)  c ## LL
#endif

#define MOVIESOAP_CALLBACK(name) int name( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data )

// test
#include <vlc_input.h> // temp used for input_GetState()
#include <iostream>
#include <inttypes.h>
using namespace std;
#include "frames/gui_helpers.hpp"

class QMenu;

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

	/* Initialize local static fields */
	static mtime_t target_time; // gets set by a callback that forks another thread
	static vlc_thread_t thread_for_filter_restart,
		thread_for_getting_input_thread,
		thread_for_item_change_cb,
		thread_for_http; // used after Time change callback
	
	/* Playlist callbacks */
	static MOVIESOAP_CALLBACK(PCB_ItemChange);
	static MOVIESOAP_CALLBACK(PCB_TryToAttachFilter);
	/* Input callbacks */
	static MOVIESOAP_CALLBACK(InputCbState);
	static MOVIESOAP_CALLBACK(InputCbGeneric);
	static MOVIESOAP_CALLBACK(InputCbPosition);
	static MOVIESOAP_CALLBACK(InputCbTime);
	/* Other local prototypes */
	static inline void StopAndStartFilter(mtime_t new_time);
	static void* EP_StopAndStartFilter(void *data);
	static void* EP_StartFilter(void *data);
	static void* EP_StopFilter(void *data);
	static void* EP_KillTimers(void *data);
	static void* EP_EnableBlackout(void *data);
	static void* EP_SetMoviesoapP_Input(void *data);
	static void* EP_dummy(void *data);

	/* Set vars, config. (Called by VLCMenuBar::createMenuBar in menus.cpp) */
	void init( intf_thread_t * p_intf, MainInterface * mainInterface, QMenu * p_menu )
	{
		// Set namespace vars
		Moviesoap::p_GuiMenu = (Moviesoap::Menu *) p_menu;
		Moviesoap::p_obj = VLC_OBJECT(p_intf);
		Moviesoap::p_playlist = pl_Get(p_intf);
		vlc_mutex_init( &lock );
		#ifdef MSDEBUG1
			msg_Info( p_obj, "Moviesoap::init in progress..." );
		#endif
		// build config (tolerances) from file or defaults
		config.load();
		// Create variable pointer to blackout config
		#define MOVIESOAP_BLACKOUT_CONFIG_POINTER
		var_CreateGetAddress( p_obj->p_libvlc, MOVIESOAP_BLACKOUT_VARNAME);
		var_SetAddress( p_obj->p_libvlc, MOVIESOAP_BLACKOUT_VARNAME, &blackout_config );
		// create all vlc_thread_t instances used by this file
		vlc_clone( &thread_for_http, EP_dummy, NULL, VLC_THREAD_PRIORITY_LOW );
		vlc_clone( &thread_for_filter_restart, EP_dummy, NULL, VLC_THREAD_PRIORITY_LOW );
		vlc_clone( &thread_for_item_change_cb, EP_dummy, NULL, VLC_THREAD_PRIORITY_LOW );
		vlc_clone( &thread_for_getting_input_thread, EP_dummy, NULL, VLC_THREAD_PRIORITY_LOW );
		// Add callback(s) to playlist
		var_AddCallback( p_playlist, "item-change", PCB_ItemChange, NULL );
		// Check for updates
		vlc_clone( &thread_for_http, handleUpdateCheck, p_intf, VLC_THREAD_PRIORITY_LOW );
	}

	/*
	 * Playlist callbacks
	 */

	 static MOVIESOAP_CALLBACK(PlaylistCbLeafToParent)
	 {
	 	VLC_UNUSED( p_this ); VLC_UNUSED( psz_var ); VLC_UNUSED( oldval );
	 	msg_Info( p_this, "LEAF TO PARENT CALLED %d", newval.i_int );
	 }

	/* Set by PlaylistChangeCallback. If a vout_thread exists on input, removes this callback and attaches the blackout filter to the vout. */
	static MOVIESOAP_CALLBACK(PCB_ItemChange)
	{
		// if input item has changed: (newval.p_address is a input_item_t *)
		if (newval.p_address != oldval.p_address) {
			spawn_set_p_input(true);
		}
		return MOVIESOAP_SUCCESS;
	}

	static MOVIESOAP_CALLBACK(PCB_TryToAttachFilter)
	{
		// spawn new thread to handle blackout and removal of this callback
		msg_Info( p_obj, "try to attach filter");
		vlc_clone( &thread_for_item_change_cb, EP_EnableBlackout, NULL, VLC_THREAD_PRIORITY_LOW );
		return MOVIESOAP_SUCCESS;
	}

	/* 
	 * Input thread callbacks
	 * 
	 * INPUT EVEN VARIABLES
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

	static MOVIESOAP_CALLBACK(InputCbState)
	{
		#ifdef MSDEBUG3
		msg_Info( p_this, "!!! CALLBACK input state !!! : %s ... new: %d ... old: %d", psz_var, (int) newval.i_int, (int) oldval.i_int );
		#endif

		// Stop or Start filter
		if (p_loadedFilter) {
			// ensures that if ancillary thread already exists, it has completed before new spawning
			vlc_join( thread_for_filter_restart, NULL );
			// spawn new thread to handle Filter restart
			if (newval.i_int == PLAYING_S)
				vlc_clone( &thread_for_filter_restart, EP_StartFilter, NULL, VLC_THREAD_PRIORITY_LOW );
			else
				vlc_clone( &thread_for_filter_restart, EP_KillTimers, NULL, VLC_THREAD_PRIORITY_LOW );
		}
		return 0;
	}

	/* Fires when someone moves forward or backward or when program sets Time. - Restart loaded Filter. */
	static MOVIESOAP_CALLBACK(InputCbTime)
	{
		#ifdef MSDEBUG3
		msg_Info( p_this, "!!! CALLBACK input time !!! : %s ... new: %d ... old: %d", psz_var, (int) newval.i_int, (int) oldval.i_int );
		#endif
		
		if (p_loadedFilter) {
			// ensures that if ancillary thread already exists, it has completed before new spawning
			vlc_join( thread_for_filter_restart, NULL );
			// sets new time to a heap variable
			target_time = newval.i_time;
			// spawn new thread to handle Filter restart
			vlc_clone( &thread_for_filter_restart, EP_StopAndStartFilter, NULL, VLC_THREAD_PRIORITY_LOW );
		}
		return 0;
	}

	/* Fires when user clicks position bar. - Restart loaded Filter. */
	static MOVIESOAP_CALLBACK(InputCbPosition)
	{
		#ifdef MSDEBUG3
		msg_Info( p_this, "!!! CALLBACK input position !!! : %s ... new: %f ... old: %f", psz_var, (float) newval.f_float, (float) oldval.f_float );
		#endif

		if (p_loadedFilter) {
			mtime_t new_time = newval.f_float * var_GetTime( p_input, "length" );
			#ifdef MSDEBUG2
				char buffer[12];
				strftime(new_time / MOVIESOAP_MOD_TIME_FACTOR, buffer);
				msg_Info(p_this, "new time %lld\n%s", new_time, &buffer[0]);
			#endif
			StopAndStartFilter(new_time);
		}
		return 0;
	}

	static MOVIESOAP_CALLBACK(InputCbGeneric)
	{
		if (p_loadedFilter) {
			switch(newval.i_int) {
				case INPUT_EVENT_TITLE:
				case INPUT_EVENT_CHAPTER:
				case INPUT_EVENT_PROGRAM:
    			case INPUT_EVENT_BOOKMARK:
					StopAndStartFilter( MoviesoapGetNow(p_input) );
					#ifdef MSDEBUG3
						msg_Info( p_this, "!!! CALLBACK input generic !!! : %s ... new: %d ... old: %d", psz_var, (int) newval.i_int, (int) oldval.i_int );
					#endif
					break;
			}
		}
		return 0;
	}

	/*
	 * Support functions
	 */

	/* Entry point for a thread created by a callback to stop and restart the filter */
	static void* EP_StopAndStartFilter(void *data)
	{
		vlc_mutex_lock( &Moviesoap::lock );
		if (p_loadedFilter) {
			p_loadedFilter->Stop();
			if (target_time != MOVIESOAP_NO_RESTART)
				p_loadedFilter->Restart( target_time );
		}
		vlc_mutex_unlock( &Moviesoap::lock );
		return NULL;
	}

	static void* EP_KillTimers(void *data)
	{
		vlc_mutex_lock( &Moviesoap::lock );
		if (p_loadedFilter)
			p_loadedFilter->KillTimers();
		vlc_mutex_unlock( &Moviesoap::lock );
		return NULL;	
	} 

	static void* EP_StopFilter(void *data)
	{
		vlc_mutex_lock( &Moviesoap::lock );
		if (p_loadedFilter)
			p_loadedFilter->Stop();
		vlc_mutex_unlock( &Moviesoap::lock );
		return NULL;	
	}

	static void* EP_StartFilter(void *data)
	{
		vlc_mutex_lock( &Moviesoap::lock );
		if (p_loadedFilter)
			p_loadedFilter->Restart();
		vlc_mutex_unlock( &Moviesoap::lock );
		return NULL;	
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

	/* If vout thread exists on input thread, add blackout filter to vout and remove item-change callback */
	static void* EP_EnableBlackout(void *data)
	{
		bool b_no_vout_thread_found = true;
		if (p_input) {
			if ( vout_thread_exists( p_input ) ) {
				b_no_vout_thread_found = false;
				vlc_mutex_lock( &lock );
				if (!p_blackout_filter) {
					var_DelCallback( p_playlist, "item-change", PCB_TryToAttachFilter, NULL );
					add_blackout_filter_to_input( p_input );
					#ifdef MSDEBUG1
						msg_Info(p_obj, "Moviesoap Blackout added to filter chain.");
					#endif
				}
				vlc_mutex_unlock( &lock );
				return NULL;
			}
		}
		if (b_no_vout_thread_found && p_blackout_filter)
			p_blackout_filter = NULL;
		return NULL;
	}

	/* Sets Moviesoap::p_input */
	static void* EP_SetMoviesoapP_Input(void *data)
	{
		// require Moviesoap::p_playlist
		if (Moviesoap::p_playlist == NULL) {
			playlist_t * p_playlist = pl_Get( Moviesoap::p_obj );
			if (p_playlist) {
				vlc_mutex_lock( &Moviesoap::lock );
				Moviesoap::p_playlist = p_playlist;
				vlc_mutex_unlock( &Moviesoap::lock );
			} else {
				return NULL;
			}
		}
		// get p_input
		input_thread_t * p_input = playlist_CurrentInput( p_playlist );
		if (p_input) {
			// set Moviesoap::p_input
			vlc_mutex_lock( &Moviesoap::lock );
			Moviesoap::p_input = p_input;
			vlc_mutex_unlock( &Moviesoap::lock );
			// Add callback(s) to input thread
			var_AddCallback( p_input, "position", InputCbPosition, NULL );
			var_AddCallback( p_input, "time", InputCbTime, NULL );
			var_AddCallback( p_input, "intf-event", InputCbGeneric, NULL );
			var_AddCallback( p_input, "state", InputCbState, NULL );
			// start filter object if one exists
			if (Moviesoap::p_loadedFilter) Moviesoap::p_loadedFilter->Restart();
		}
		// Add callback to enable blackout
		var_AddCallback( p_playlist, "item-change", PCB_TryToAttachFilter, NULL );
		return p_input;
	}

	/* A dummy function. Called in init() so that all vlc_thread_t instances can be created
	and therefore vlc_join can be called on them even prior to any earnest call of vlc_clone 
	on them. */
	static void* EP_dummy(void *data) { return NULL; }

	/* Set Moviesoap::p_input using a new thread */
	void spawn_set_p_input(bool force_overwrite)
	{
		if (p_obj) {
			if (p_playlist == NULL || p_input == NULL || force_overwrite) {
				vlc_join( thread_for_getting_input_thread, NULL );
				vlc_mutex_lock( &Moviesoap::lock );
				vlc_clone( &thread_for_getting_input_thread, EP_SetMoviesoapP_Input, NULL, VLC_THREAD_PRIORITY_LOW );
				vlc_mutex_unlock( &Moviesoap::lock );	
			}
		}
	}

	/* Call Filter::Restart in a new thread */
	void spawn_restart_filter()
	{
		vlc_join( thread_for_filter_restart, NULL );
		vlc_clone( &thread_for_filter_restart, EP_StartFilter, NULL, VLC_THREAD_PRIORITY_LOW );
	}

	/* Call Filter::Stop in a new thread */
	void spawn_stop_filter() {
		vlc_join( thread_for_filter_restart, NULL );
		vlc_clone( &thread_for_filter_restart, EP_StopFilter, NULL, VLC_THREAD_PRIORITY_LOW );
	}

}
