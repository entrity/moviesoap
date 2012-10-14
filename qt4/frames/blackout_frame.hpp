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
		QLineEdit *startText, *stopText;
	public:
		BlackoutFrame(QWidget *);
	};
}

#endif