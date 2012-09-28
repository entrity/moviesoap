#include "main.hpp"
#include "../filter.hpp"
#include <vlc_common.h>
#include <vlc_interface.h>

#include <cstdio>

#include <vlc_input.h> // temp used for input_GetState()
#include <vlc_threads.h> // temp used for vlc_timer...
#ifndef INT64_C	// ibid.
# define INT64_C(c)  c ## LL
#endif

namespace Moviesoap
{
	vlc_object_t * p_obj;
	playlist_t * p_playlist;
	input_thread_t * p_input;
	Filter * p_loadedFilter;
	audio_volume_t volume;
	struct config_t config;
	static int PlaylistChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data );
	static int InputChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data );
	static int PositionChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data );

	void test()
	{
		msg_Info( p_obj, "---- ---- IN MOVIEOSAP INIT. %x", p_obj );

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

		/* test dummy filter */
		p_loadedFilter = Moviesoap::Filter::dummy();
		p_loadedFilter->toStdout();
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
		// set vars
		p_obj = VLC_OBJECT(p_intf);
		// test
		test();
		// build config (tolerances)

		// add callback to Playlist change
		playlist_t * p_playlist = pl_Get( p_intf );
		var_AddCallback( p_playlist, "item-current", PlaylistChangeCallback, p_intf );
	}

	/* Add callback to Input change. Start Filter object. */
	static int PlaylistChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data )
	{
		intf_thread_t * p_intf = ( intf_thread_t *) p_data;
		p_playlist = pl_Get( p_intf );
		if (p_playlist)
		{
			p_input = playlist_CurrentInput( p_playlist );
			if (p_input) {
				// add callbacks for input change (see vlc_input.h "Input events and variables")
				var_AddCallback( p_input, "state", InputChangeCallback, p_data );
				var_AddCallback( p_input, "navigation", InputChangeCallback, p_data );
				var_AddCallback( p_input, "position", PositionChangeCallback, p_data );
				// start filter object if one exists
				if (p_loadedFilter) p_loadedFilter->Restart();
			} else
			msg_Err( p_this, "No current input thread found." );
		} else
		msg_Err( p_this, "No playlist found." );
		return 0;
	}

	/* (Re)start Filter object. (This will kill existing timers.) */
	static int InputChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data )
	{
		msg_Info( p_this, "!!! input change !!! : %s", psz_var );
		// restart filter object if extant
		if (p_loadedFilter) p_loadedFilter->Restart();
		return 0;
	}

	static int PositionChangeCallback( vlc_object_t *p_this, const char *psz_var, vlc_value_t oldval, vlc_value_t newval, void *p_data )
	{
		msg_Info( p_this, "!!! input change !!! : %s", psz_var );
		mtime_t new_time = newval.f_float * var_GetTime( p_input, "length" );
		if (p_loadedFilter) p_loadedFilter->Restart( new_time );
		return 0;
	}

	bool config_t::ignoreMod(Mod & mod) {
		uint8_t tolerance = tolerances[mod.mod.category];
		return tolerance > mod.mod.severity;
	}
}

