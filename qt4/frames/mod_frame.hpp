#ifndef MOVIESOAP_MOD_FRAME_H
#define MOVIESOAP_MOD_FRAME_H
#define MOVIESOAP_MOD_MODE_COUNT 3

#include "../../variables.h"

#include <QFrame>
class QWidget;
class QLineEdit;
class QRadioButton;
class QComboBox;

namespace Moviesoap
{
	class Mod;
	class FilterWin;

	class ModFrame : public QFrame 
	{
		Q_OBJECT
	protected:
		QLineEdit *startText, *stopText, *descText;
		QRadioButton
			*modeRadios[MOVIESOAP_MOD_MODE_COUNT],
			*severityRadios[MOVIESOAP_TOLERANCE_COUNT],
			*categoryRadios[MOVIESOAP_CAT_COUNT];
		QComboBox * categoryBox;
		Mod * p_editingMod; // will be NULL if this is for a new Mod, else points to something in editingFilter->modList
		inline FilterWin *filterWin();
	public:
		ModFrame(QWidget *);
		void load(Mod *);
		void dump(Mod *);
	public slots:
		void cancelClicked();
		void okClicked();
	};
}

#endif