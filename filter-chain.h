#ifndef MOVIESOAP_FILTER_CHAIN_H
#define MOVIESOAP_FILTER_CHAIN_H

#include <vlc_common.h>
#include <vlc_filter.h>
#include <vlc_vout.h>
#include <src/video_output/vout_internal.h>

namespace Moviesoap {

	filter_t * p_blackout_filter = NULL;

}

	static inline filter_chain_t * get_filter_chain_static( vout_thread_t * p_vout )
	{
		return p_vout->p->filter.chain_static;
	}

	static inline filter_chain_t * get_filter_chain_interactive( vout_thread_t * p_vout )
	{
		return p_vout->p->filter.chain_interactive;
	}

	/* Returns interactive filter chain for first vout thread on given input thread */
	inline filter_chain_t *get_first_filter_chain( input_thread_t *p_input )
	{
		size_t n_vout;
		vout_thread_t **pp_vout;
		input_Control( p_input, INPUT_GET_VOUTS, &pp_vout, &n_vout );
		if (n_vout)
			return get_filter_chain_interactive( pp_vout[0] );
		else
			return NULL;
	}

	/* Returns number of vout_thread_t on given input thread */
	inline int vout_thread_exists( input_thread_t * p_input )
	{
		size_t n_vout;
		vout_thread_t **pp_vout;
		input_Control( p_input, INPUT_GET_VOUTS, &pp_vout, &n_vout );
		return (int) n_vout;
	}

	/* Adds moviesoap_video_filter to input thread's filter chain unless the filter chain already contains at least one filter or there is no filter chain */
	int add_blackout_filter_to_input( input_thread_t *p_input )
	{
		filter_chain_t *p_filter_chain = get_first_filter_chain( p_input );
		if ( !p_filter_chain )
			return VLC_ENOOBJ; // No vout thread available (no biggie)
		Moviesoap::p_blackout_filter = filter_chain_AppendFilter( p_filter_chain, "moviesoap_video_filter", NULL, NULL, NULL );
		return VLC_SUCCESS; // Not really an error. Just indicates we made the change.
	} 


// typedef struct chained_filter_t
// {
//     /* Public part of the filter structure */
//     filter_t filter;
//      Private filter chain data (shhhh!) 
//     struct chained_filter_t *prev, *next;
//     vlc_mouse_t *mouse;
//     picture_t *pending;
// } chained_filter_t;

#endif