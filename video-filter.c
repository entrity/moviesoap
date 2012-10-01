/*
 * HOW TO:
 *
 * The config (blackout_config a.k.a. MOVIESOAP_BLACKOUT_VARNAME) is attached to 
 * p_libvlc by this module. The moviesoap interface (actually, the qt4 interface 
 * with some additions of mine) must use
 *      var_GetAddress( [p_vlc_obj]->p_libvlc, MOVIESOAP_BLACKOUT_VARNAME )
 * to get the address of the config, then change the config fields (which are 
 * defined in moviesoap/video-filter.h) to alter the activity and bounds of the
 * blackout box produced by this filter module. *
 *
 * The filter is active when blackout_config.b_active is true.
 * The dimensions of the blackout box are set by the remaining fields of 
 * moviesoap_blackout_config_t.
 * 
 * Much of this file was extracted from the erase.c module
 */

/*****************************************************************************
* includes
*****************************************************************************/
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vlc_common.h>
#include <vlc_plugin.h>
#include <vlc_sout.h>
#include <vlc_image.h>
#include <vlc_block.h>
#include <vlc_picture.h>
#include <vlc_playlist.h>

#include "variables.h"

/*****************************************************************************
* Local prototypes
*****************************************************************************/

static int Create ( vlc_object_t * );
static void Destroy ( vlc_object_t * );
static picture_t *Filter( filter_t *, picture_t * );
static void FilterFrame( filter_t *, picture_t *, picture_t * );
static void TestSettings();
struct filter_sys_t {
    vlc_mutex_t lock;
};

/*****************************************************************************
* Blackout config
*****************************************************************************/

static moviesoap_blackout_config_t blackout_config;
static moviesoap_blackout_config_t * p_blackout_config;

/*****************************************************************************
* Module descriptor
*****************************************************************************/

#define MOVIESOAP_HELP N_("Make custom filters to remove violence, sexuality, and profanity from your movie.")
#define CFG_PREFIX "moviesoap-"
#define POSX1_TEXT N_("left")
#define POSY1_TEXT N_("top")
#define POSX2_TEXT N_("right")
#define POSY2_TEXT N_("bottom")
#define POSX1_LONGTEXT N_("left coord of blackout box")
#define POSY1_LONGTEXT N_("top coord of blackout box")
#define POSX2_LONGTEXT N_("right coord of blackout box")
#define POSY2_LONGTEXT N_("bottom coord of blackout box")

vlc_module_begin ()
    set_description( N_("MovieSoap video filter for making blackout") )
    set_shortname( N_( "MovieSoap Video Filter" ))
    set_capability( "video filter2", 0)
    set_help(MOVIESOAP_HELP)
    set_category( CAT_VIDEO )
    set_subcategory( SUBCAT_VIDEO_VFILTER )
    set_callbacks( Create, Destroy )
    // add_bool( CFG_PREFIX "active", 0, N_("active"), N_("whether the blackout is in effect"), false )
    // add_address( CFG_PREFIX "x1", 0, POSX1_TEXT, POSX1_LONGTEXT, false )
vlc_module_end ()

/*****************************************************************************
* Create
*****************************************************************************/

static int Create( vlc_object_t *p_this )
{
    filter_t * p_filter = (filter_t *) p_this;
    #ifdef MSDEBUG1
        msg_Info(p_this, "----- CREATE MOVIESOAP VIDEO FILTER");
    #endif
    
    /* Init filter (this) */
    p_filter->p_sys = malloc( sizeof( filter_sys_t ) );
    if( p_filter->p_sys == NULL ) return VLC_ENOMEM;
    p_filter->pf_video_filter = Filter;

    /* Set local pointer to blackout config */
    p_blackout_config = var_GetAddress(p_this->p_libvlc, MOVIESOAP_BLACKOUT_VARNAME);
    msg_Info( p_this, "blackout config address (2): %x", p_blackout_config );

    switch( p_filter->fmt_in.video.i_chroma )
    {
        case VLC_CODEC_I420:
        case VLC_CODEC_J420:
        case VLC_CODEC_YV12:
        case VLC_CODEC_I422:
        case VLC_CODEC_J422:
            break;
        default:
            msg_Err( p_filter, "Unsupported input chroma (%4.4s)", (char*)&(p_filter->fmt_in.video.i_chroma) );
            return VLC_EGENERIC;
    }
  
    /* Init mutex */
    vlc_mutex_init( &p_filter->p_sys->lock );

    #ifdef MSDEBUG2
        msg_Info( p_this, "video filter module creation complete");
    #endif

    /* End #Create() */
    return VLC_SUCCESS;
}

/*****************************************************************************
* Destroy - todo
*****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    vlc_mutex_destroy( &p_filter->p_sys->lock );
    free( p_filter->p_sys );
}

/*****************************************************************************
* Filter
*****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_inpic )
{
    picture_t *p_outpic;
    filter_sys_t *p_sys = p_filter->p_sys;

    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        picture_Release( p_inpic );
        return NULL;
    }

    /* If the filter is inactive: just copy the image, else make blackout rectangle */
    vlc_mutex_lock( &p_filter->p_sys->lock );
    if ( p_blackout_config->b_active )
        FilterFrame( p_filter, p_inpic, p_outpic );
    else
        picture_CopyPixels( p_outpic, p_inpic );
    vlc_mutex_unlock( &p_filter->p_sys->lock );

    picture_CopyProperties( p_outpic, p_inpic );
    picture_Release( p_inpic );

    return p_outpic;
}

/*****************************************************************************
* FilterFrame
*****************************************************************************/
static void FilterFrame( filter_t *p_filter, picture_t *p_inpic, picture_t *p_outpic )
{
    int i_plane;

    for( i_plane = 0; i_plane < p_inpic->i_planes; i_plane++ )
    {
        /* Copy original pixel buffer */
        plane_CopyPixels( &p_outpic->p[i_plane], &p_inpic->p[i_plane] );
    
        uint16_t i_x1 = p_blackout_config->i_x1;
        uint16_t i_y1 = p_blackout_config->i_y1;
        uint16_t i_x2 = p_blackout_config->i_x2;
        uint16_t i_y2 = p_blackout_config->i_y2;

        const int i_visible_pitch = p_inpic->p[i_plane].i_visible_pitch;
        const int i_visible_lines = p_inpic->p[i_plane].i_visible_lines;            
        const bool b_line_factor = ( i_plane /* U_PLANE or V_PLANE */ &&
            !( p_inpic->format.i_chroma == VLC_CODEC_I422
            || p_inpic->format.i_chroma == VLC_CODEC_J422 ) );

        if( i_plane ) /* U_PLANE or V_PLANE */
        {
            i_x2 >>= 1; // this was originally 'width' does it need to change?
            i_x1 >>= 1;
        }
        if( b_line_factor )
        {
            i_y2 >>= 1; // this was originally 'height' does it need to change?
            i_y1 >>= 1;
        }

        // ensure that coords fall inside picture
        i_x1 = __MIN( i_visible_pitch, i_x1 );
        i_x2 = __MIN( i_visible_pitch, i_x2 );
        i_y1 = __MIN( i_visible_lines, i_y1 );
        i_y2 = __MIN( i_visible_lines, i_y2 );

        // choose colour that equals black
        const uint8_t colour = i_plane ? 128 : 16;

        // set ordained pixels to black        
        int i_pixel, x, y;
        for( y = i_y1; y < i_y2; y++ ) {
            for( x = i_x1; x < i_x2; x++ ) {
                i_pixel = ( y * i_visible_pitch ) + x;
                p_outpic->p[i_plane].p_pixels[i_pixel] = colour;
            }
        }
    }
}

/* Supplies values for the blackout config. Used for testing. */
void TestSettings( vlc_object_t * p_obj )
{
	// set config by direct access
    p_blackout_config->b_active = true;
    p_blackout_config->i_x1 = 30;
    p_blackout_config->i_x2 = 330;
    p_blackout_config->i_y1 = 70;
    p_blackout_config->i_y2 = 200;
    
    // create new config struct
    moviesoap_blackout_config_t newconfig;
    newconfig.b_active = true;
    newconfig.i_x1 = 100;
    newconfig.i_y1 = 100;
    newconfig.i_x2 = 150;
    newconfig.i_y2 = 200;

    // set via the method in variables.h
    Moviesoap_SetBlackout( p_obj, &newconfig );
}