#ifndef MOVIESOAP_BLACKOUT_FRAME_H
#define MOVIESOAP_BLACKOUT_FRAME_H

#include "../../variables.h"

#include <QFrame>
class QWidget;
class QLineEdit;

namespace Moviesoap
{
	class Mod;

	class BlackoutFrame : public QFrame 
	{
		Q_OBJECT
	protected:
		QLineEdit *x1Text, *x2Text, *y1Text, *y2Text;
		Mod * p_editingMod;
	public:
		BlackoutFrame(QWidget *);
		void load(Mod *);
		void dump(Mod *);
	};
}

#endif