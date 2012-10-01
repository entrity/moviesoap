#include "main.hpp"
#include "../filter.hpp"
#include "../variables.h"
#include <vlc_common.h>
#include <vlc_interface.h>
#include <vlc_modules.h>
#include <vlc_aout_intf.h>	// aout_ToggleMute

#include <cstdio>
#include <cassert>

#include <vlc_input.h> // temp used for input_GetState()
#include <vlc_threads.h> // temp used for vlc_timer...
#ifndef INT64_C	// ibid.
# define INT64_C(c)  c ## LL
#endif

#include "../filter-chain.h"

namespace Moviesoap
{
	vlc_object_t * p_obj;
	playlist_t * p_playlist;
	input_thread_t * p_input;
	Filter * p_loadedFilter;
	filter_chain_t * p_filter_chain; // chain holds blackout video filter
	audio_volume_t volume;
	vlc_mutex_t lock;
	moviesoap_blackout_config_t blackout_config;
	struct config_t config;
	static int PlaylistChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data );
	static int ItemChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data );
	static int StateChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data );
	static int InputChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data );
	static int PositionChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data );
	static int TimeChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data );
	static inline void StopAndStartFilter(mtime_t new_time);
	
	// static inline void InitBlackoutConfig();
	vlc_timer_t test_timer;
	char * test_str = "foo bar baz qux";
	static void tempCallback( void * p_data );
	static int TempCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data );

	/* Returns true if given content for given mod is tolerable to user's config. */
	bool config_t::ignoreMod(Mod & mod) {
		uint8_t tolerance = tolerances[mod.mod.category];
		return tolerance > mod.mod.severity;
	}

	void test()
	{
		/* timer test */
		// vlc_timer_t timer;
		// vlc_timer_create( &timer, tempCallback, NULL );
		// // msg_Info(p_obj, "timer created: q %s", timer->quit);
		// vlc_timer_destroy(timer);
		// // msg_Info(p_obj, "timer destroyed without being scheduled: q %s", timer.quit);
		// vlc_timer_create( &timer, tempCallback, NULL );
		// mtime_t delay = 999999999UL;
		// scheduleTimer( timer, 0, delay );
		// // msg_Info(p_obj, "timer scheduled: q %s", timer.quit);
		// vlc_timer_destroy(timer);
		// msg_Info(p_obj, "timer destroyed: q %s", timer.quit);
		/* timer that prints a message */
		// vlc_timer_create( &test_timer, tempCallback, test_str );
		// mtime_t delay = 1000000L;
		// vlc_timer_schedule( test_timer, false, 1, delay );


		// supply dummy values for blackout config
		blackout_config.b_active = false;
		blackout_config.i_x1 = 30;
		blackout_config.i_y1 = 10;
		blackout_config.i_x2 = 150;
		blackout_config.i_y2 = 100;

		/* test dummy filter */
		p_loadedFilter = Moviesoap::Filter::dummy();
		#ifdef MSDEBUG3
			p_loadedFilter->toStdout();
		#endif
		// p_loadedFilter->save();

		/* test config struct */
		// config.active = true;
		// config.tolerances[0] = 168;
		// printf("---- config.tolerances[0]: %d\n", config.tolerances[0]);
		// printf("foo & bar: %d %d\n", config.foo(), config.bar());

		// Mod * p_mod = &p_loadedFilter->modList.front();
		// p_loadedFilter->loadMod(*p_mod);
	}

	/* Set vars, config. (Called by VLCMenuBar::createMenuBar in menus.cpp) */
	void init( intf_thread_t * p_intf, MainInterface * mainInterface )
	{
		// Set namespace vars
		p_obj = VLC_OBJECT(p_intf);
		#ifdef MSDEBUG1
			msg_Info( p_obj, "---- ---- IN MOVIEOSAP INIT." );
		#endif
		vlc_mutex_init( &lock );
		// test (dummy filter)
		test();
		// build config (tolerances)
		// todo
		// Get playlist pointer
		playlist_t * p_playlist = pl_Get( p_intf );
		// Unmute because aout_IsMuted will incorrectly return true until this is done
		// cout << "is muted? " << aout_IsMuted(VLC_OBJECT(p_playlist)) << endl;
		// if ( aout_IsMuted(VLC_OBJECT(p_playlist)) )
		// 	{ aout_SetMute( VLC_OBJECT(p_playlist), &volume, false ); }
		// Create variable pointer to blackout config
		#define MOVIESOAP_BLACKOUT_CONFIG_POINTER
		var_CreateGetAddress( p_obj->p_libvlc, MOVIESOAP_BLACKOUT_VARNAME);
		var_SetAddress( p_obj->p_libvlc, MOVIESOAP_BLACKOUT_VARNAME, &blackout_config );
		// Add callback(s) to playlist
		var_AddCallback( p_playlist, "item-current", PlaylistChangeCallback, p_intf );
		// var_AddCallback( p_playlist, "activity", TempCallback, p_intf );
		// var_AddCallback( p_playlist, "input-current", TempCallback, p_intf );
	}

	/* Add callback to Input change. Start Filter object. */
	static int PlaylistChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data )
	{
		#ifdef MSDEBUG1
		msg_Info( p_this, "!!! playlist change !!! : %s : %d", psz_var, newval.i_int );
		#endif
		intf_thread_t * p_intf = ( intf_thread_t *) p_data;
		p_playlist = pl_Get( p_intf );
		if (p_playlist)
		{
			// Add callback to item-change (gets removed as soon as there is a vout_thread_t available)
			var_AddCallback( p_playlist, "item-change", ItemChangeCallback, NULL ); //needs to be added elsewhere
			// Get input thread
			p_input = playlist_CurrentInput( p_playlist );
			if (p_input) {
				// add callbacks for input change (see vlc_input.h "Input events and variables")
				var_AddCallback( p_input, "position", PositionChangeCallback, p_data );
				var_AddCallback( p_input, "time", TimeChangeCallback, p_data );
				// todo/unused
				var_AddCallback( p_input, "state", StateChangeCallback, p_data );
				// var_AddCallback( p_input, "time-offset", TempCallback, p_data );
				var_AddCallback( p_input, "navigation", InputChangeCallback, p_data );
				// start filter object if one exists
				if (p_loadedFilter) p_loadedFilter->Restart();
				return VLC_SUCCESS;
			} else
			msg_Err( p_this, "No current input thread found." );
		} else
		msg_Err( p_this, "No playlist found." );
		return VLC_ENOOBJ;
	}

	/* Dummy function */
	static int StateChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data )
	{
		#ifdef MSDEBUG3
		msg_Info( p_this, "!!! state change !!! : %s : %d", psz_var, newval.i_int );
		#endif
		return 0;
	}

	/* (Re)start Filter object. (This will kill existing timers.) Fires when someone moves forward or backward. */
	static int TimeChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data )
	{
		#ifdef MSDEBUG3
		msg_Info( p_this, "!!! time change !!! : %s", psz_var );
		#endif
		mtime_t new_time = newval.i_time;
		StopAndStartFilter(new_time);
		return 0;
	}

	/* (Re)start Filter object. (This will kill existing timers.) Fires when user clicks position bar. */
	static int PositionChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data )
	{
		// todo del:
		// analFCs( p_input );
		//
		#ifdef MSDEBUG3
		msg_Info( p_this, "!!! position change !!! : %s", psz_var );
		#endif
		mtime_t new_time = newval.f_float * var_GetTime( p_input, "length" );
		cout << "new time: " << new_time / 100000 << endl;
		StopAndStartFilter(new_time);
		return 0;
	}

	static int InputChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data )
	{
		#ifdef MSDEBUG3
		msg_Info( p_this, "!!! input change !!! : %s : %d", psz_var, newval.i_int );
		#endif
		return 0;
	}

	/* Set by PlaylistChangeCallback. If a vout_thread exists on input, removes this callback and attaches the blackout filter to the vout. */
	static int ItemChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data )
	{
		msg_Info( p_this, "ItemChangeCallback: %s", psz_var );
		cout << newval.i_int << endl << newval.f_float << endl;
		if (p_input) {
			size_t n_vout;
			vout_thread_t **pp_vout;
			input_Control( p_input, INPUT_GET_VOUTS, &pp_vout, &n_vout );
			cout << n_vout << " vout threads." << endl;
			if (n_vout) {

				// Unattach this callback from playlist
				// vlc_mutex_lock( &lock );
				// var_DelCallback( p_playlist, "item-change", ItemChangeCallback, NULL );
				// vlc_mutex_unlock( &lock );

				// Attach blackout filter to vout
				add_blackout_filter_to_input( p_input );
			}
		}
		return 0;
	}


	// todo del
	static void tempCallback( void * p_data )
	{
		char *p_str = (char *) p_data;
		cout << "temp callback " << p_str << endl;
	}

	// todo del
	static int TempCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data )
	{
		msg_Info( p_this, "TempCallback: %s", psz_var );
		cout << newval.i_int << endl << newval.f_float << endl;
		return 0;
	}

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


