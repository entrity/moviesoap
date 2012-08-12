#ifndef MOVIESOAP_VIDEO_FILTER_H
#define MOVIESOAP_VIDEO_FILTER_H

#include <vlc_common.h>

typedef struct {
	bool b_active;
	uint16_t i_x1, i_y1, i_x2, i_y2;
} moviesoap_blackout_config_t;

#endif