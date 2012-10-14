#ifndef MOVIESOAP_MOD_FRAME_H
#define MOVIESOAP_MOD_FRAME_H

#include "../../variables.h"

#include <QFrame>
class QWidget;
class QLineEdit;
class QRadioButton;
class QComboBox;

namespace Moviesoap
{
	class Mod;

	class ModFrame : public QFrame 
	{
		Q_OBJECT
	protected:
		QLineEdit *startText, *stopText;
		QRadioButton
			*modeRadios[3],
			*severityRadios[MOVIESOAP_TOLERANCE_COUNT],
			*categoryRadios[MOVIESOAP_CAT_COUNT];
		QComboBox * categoryBox;
		Mod * p_editingMod;
	public:
		ModFrame(QWidget *);
		void load(Mod *);
	};
}

#endif