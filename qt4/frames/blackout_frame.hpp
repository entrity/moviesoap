#ifndef MOVIESOAP_BLACKOUT_FRAME_H
#define MOVIESOAP_BLACKOUT_FRAME_H

#include "../../variables.h"

#include <QFrame>
class QLineEdit;

namespace Moviesoap
{
	class Mod;
	class FilterWin;

	class BlackoutFrame : public QFrame 
	{
		Q_OBJECT
	protected:
		QLineEdit *x1Text, *x2Text, *y1Text, *y2Text;
		FilterWin *filterWin;
	public:
		BlackoutFrame(FilterWin *);
		void load(Mod *);
		void dump(Mod *);
	public slots:
		void okClicked();
	};
}

#endif