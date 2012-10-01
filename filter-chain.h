#ifndef MOVIESOAP_FILTER_CHAIN_H
#define MOVIESOAP_FILTER_CHAIN_H

#include <vlc_common.h>
#include <vlc_filter.h>
#include <vlc_vout.h>
#include <src/video_output/vout_internal.h>



typedef struct chained_filter_t
{
    /* Public part of the filter structure */
    filter_t filter;
    /* Private filter chain data (shhhh!) */
    struct chained_filter_t *prev, *next;
    vlc_mouse_t *mouse;
    picture_t *pending;
} chained_filter_t;

static filter_t *p_blackout_filter;

// see video.c line 292
// see mosaic_bridge.c lines 413-4

// static picture_t *moviesoap_video_filter_buffer_new( filter_t *p_filter )
// {
//     // p_filter->fmt_out.video.i_chroma = p_filter->fmt_out.i_codec;
//     return picture_NewFromFormat( &p_filter->fmt_out.video );
// }

// static void moviesoap_video_filter_buffer_del( filter_t *p_filter, picture_t *p_pic )
// {
//     VLC_UNUSED(p_filter);
//     picture_Release( p_pic );
// }

// static int moviesoap_video_filter_allocation_init( filter_t *p_filter, void *p_data )
// {
//     VLC_UNUSED(p_data);
//     p_filter->pf_video_buffer_new = moviesoap_video_filter_buffer_new;
//     p_filter->pf_video_buffer_del = moviesoap_video_filter_buffer_del;
//     return VLC_SUCCESS;
// }

// static void moviesoap_video_filter_allocation_clean( filter_t *p_filter )
// {
//     VLC_UNUSED(p_filter);
// }

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

// /* Returns true if given filter chain contains given p_blackout_filter */
// inline bool chain_contains_blackout_filter( filter_chain_t *p_chain )
// {
// 	// ensure blackout filter exists
// 	if (!p_blackout_filter)
// 		return false;
// 	// iterate through filters in chain
// 	chained_filter_t *p_chained_filter = p_chain->first;
// 	while (p_chained_filter) {
// 		if ( (&p_chained_filter->filter) == p_blackout_filter )
// 			return true;
// 		p_chained_filter = p_chained_filter->next;
// 	}
// 	return false;
// }

/* Appends given  filter to filter chain and sets p_blackout_filter. */
// inline filter_t *add_blackout_filter_to_chain( filter_chain_t *p_chain, filter_t *p_filter )
// {
// 	p_blackout_filter = filter_chain_Append( p_filter_chain, "moviesoap_video_filter" );
// 	return p_blackout_filter;
// }

inline int add_blackout_filter_to_input( input_thread_t *p_input )
{
	filter_chain_t *p_filter_chain = get_first_filter_chain( p_input );
	if ( !p_filter_chain )
		return VLC_ENOOBJ; // No vout thread available (no biggie)
	if ( filter_chain_GetLength(p_filter_chain) )
		return VLC_ENOVAR; // There's already at least one filter in this chain; let's not risk it by adding our filter in case we added it already to this chain
	p_blackout_filter = filter_chain_AppendFilter( p_filter_chain, "moviesoap_video_filter", NULL, NULL, NULL );
	return VLC_SUCCESS; // Not really an error. Just indicates we made the change.
}

// // todo del
// static void analFCs( input_thread_t * p_input )
// {
// 	filter_chain_t * p_filter_chain;
// 	unsigned int n;
// 	msg_Info( p_input, "IN ANALFCS !");
// 	vout_thread_t **pp_vout;
// 	size_t i_vout;
// 	input_Control( p_input, INPUT_GET_VOUTS, &pp_vout, &i_vout );
// 	#ifdef MSDEBUG1
// 		msg_Info( p_input, "# of vout threads: %d", i_vout );
// 	#endif
// 	for (n=0; n<i_vout; n++) {
// 		p_filter_chain = get_filter_chain_static( pp_vout[n] );
// 		msg_Info( p_input, "# of filters in chain (sta): %d", filter_chain_GetLength(p_filter_chain) );
// 		p_filter_chain = get_filter_chain_interactive( pp_vout[n] );
// 		msg_Info( p_input, "# of filters in chain (int): %d", filter_chain_GetLength(p_filter_chain) );
// 	}
// 	if (i_vout == 1) {
// 		p_filter_chain = get_filter_chain_interactive( pp_vout[0] );
// 		n = filter_chain_GetLength( p_filter_chain );
// 		#ifdef MSDEBUG1
// 			msg_Info( p_input, "# of filters in chain: %d", n );
// 		#endif
// 		filter_chain_AppendFromString( p_filter_chain, "moviesoap_video_filter" );
// 	}
// }

#endif