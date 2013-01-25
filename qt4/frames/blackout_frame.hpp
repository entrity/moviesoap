#ifndef MOVIESOAP_BLACKOUT_FRAME_H
#define MOVIESOAP_BLACKOUT_FRAME_H

#include "../../variables.h"

#include <QFrame>
class QLineEdit;
class QLabel;

namespace Moviesoap
{
	class Mod;
	class FilterWin;
	class Thumbnail;

	class BlackoutFrame : public QFrame 
	{
		Q_OBJECT
	protected:
		QLineEdit *x1Text, *x2Text, *y1Text, *y2Text;
		Thumbnail *thumbnail;
		FilterWin *filterWin;
	public:
		BlackoutFrame(FilterWin *);
		void load(Mod *);
		void dump();
		void loadImageFromFile();
		void loadImageFromBlock(block_t *);
		void captureAndLoadImage(Mod *);
		void getImage(Mod * mod);
		/* Returns block to bmp image. Requires block_Release() */
		block_t * takeSnapshot(mtime_t usec);
	public slots:
		void okClicked();
		void load(int x1, int y1, int x2, int y2);
	};
}

typedef struct {
	char bf_type[2],
	bf_size[4],
	bf_reserved_1[2],
	bf_reserved_2[2],
	bf_off_bits[4];
} bmp_file_header_t;

typedef struct {
	char size[4],
	width[4],
	height[4],
	planes[2],
	bitCount[2],
	compression[4],
	size_image[4],
	pixels_per_meter_x[4],
	pixels_per_meter_y[4],
	clr_used[4],
	clr_important[4];
} bmp_img_header_t;

#endif