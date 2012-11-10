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

	class BlackoutFrame : public QFrame 
	{
		Q_OBJECT
	protected:
		QLineEdit *x1Text, *x2Text, *y1Text, *y2Text;
		QLabel *thumbnail;
		FilterWin *filterWin;
	public:
		BlackoutFrame(FilterWin *);
		void load(Mod *);
		void dump(Mod *);
		void loadImageFromFile();
		void captureAndLoadImage(Mod *);
		void getImage(Mod * mod);
		/* Returns block to bmp image. Requires block_Release() */
		block_t * takeSnapshot(mtime_t usec);
	public slots:
		void okClicked();
	};
}

#endif