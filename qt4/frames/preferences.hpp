#ifndef MOVIESOAP_TOLERANCES_HPP
#define MOVIESOAP_TOLERANCES_HPP

#include <QFrame>
class QRadioButton;
#include "../../variables.h"
#include <vlc_common.h>

// test
#include <cstdio>
#include <iostream>
using namespace std;

namespace Moviesoap
{
	class PreferencesWin : public QFrame
	{
		Q_OBJECT
	protected:
		static PreferencesWin * p_window;
		QRadioButton * radios[MOVIESOAP_CAT_COUNT][MOVIESOAP_TOLERANCE_COUNT];
		void setRadio(uint8_t, uint8_t);
		uint8_t getRadio(uint8_t);
	public:
		/* Open editor window */
		static void openEditor();
		/* Constructor */
		PreferencesWin();
		/* Set values on QRadioButtons */
		void set(const uint8_t[]);
		/* Get values from QRadioButtons */
		void get(uint8_t[]);
	public slots:
		/* Write to disk and load into Moviesoap::config */
		void save();
	};
}

#endif