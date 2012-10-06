#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "../main.hpp"
#include "preferences.hpp"
#include "../config.hpp"

#define SOAP_BUILD_RADIO_BUTTON(layout, name, row, col) { \
	radios[row][col] = new QRadioButton; \
	layout->addWidget(radios[row][col], row+1, col+1); \
	btnGroup->addButton(radios[row][col]); \
}
#define SOAP_BUILD_RADIO_BUTTONS(layout, name, row) { \
	btnGroup = new QButtonGroup; \
	layout->addWidget(new QLabel(QString(name)), row+1, 0); \
	for (uint8_t col = 0; col < MOVIESOAP_TOLERANCE_COUNT; col++) \
		SOAP_BUILD_RADIO_BUTTON( layout, name, row, col ) \
}
#define SOAP_BUILD_TOLERANCE_LABEL(i, text) grid->addWidget(new QLabel(QString(text)), 0, i+1, 1, 1, Qt::AlignCenter);

namespace Moviesoap
{
	/* Local fields */
	static const char * toleranceLabels[] = {
		"(unspecified)",
		"Bigotry",
		"Blasphemy",
		"Contention",
		"Disrespect family",
		"Other",
		"Nudity",
		"Profanity",
		"Sexuality",
		"Violence",
		"Vulgarity"
	};

	/* Local function prototypes */

	/* Initialize static vars */
	PreferencesWin * PreferencesWin::p_window = NULL;

	/* Constructor */
	PreferencesWin::PreferencesWin() : QFrame(NULL)
	{
		setWindowTitle(QString("Moviesoap Tolerance"));
		QGridLayout * grid = new QGridLayout;
		// labels 0-5
		for (char i = 0; i < MOVIESOAP_TOLERANCE_COUNT; i++)
			SOAP_BUILD_TOLERANCE_LABEL(i, "x")
		// rows of radio buttons (w/ text labels)
		QButtonGroup * btnGroup;
		for (uint8_t i = 0; i < MOVIESOAP_CAT_COUNT; i++)
			SOAP_BUILD_RADIO_BUTTONS(grid, toleranceLabels[i], i)
		// save button
		QPushButton * saveBtn = new QPushButton(QString("Save preferences"));
		grid->addWidget( saveBtn, MOVIESOAP_CAT_COUNT+2, 0 );
		connect( saveBtn, SIGNAL(clicked()), this, SLOT(save()) );
		// load config from file
		set(Moviesoap::config.tolerances);
		// set the layout
		setLayout(grid);
	}

	/* Open editor window */
	void PreferencesWin::openEditor()
	{
		if (p_window == NULL)
			p_window = new PreferencesWin;
		p_window->show();
	}

	/* Set values on QRadioButtons */
	void PreferencesWin::set(const uint8_t arr[])
	{
		for( uint8_t i = 0; i < MOVIESOAP_CAT_COUNT; i ++ ) {
			if (arr[i] < MOVIESOAP_TOLERANCE_COUNT)
				setRadio(i, arr[i]);
		}
	}

	/* Get values from QRadioButtons */
	void PreferencesWin::get(uint8_t arr[])
	{
		for ( uint8_t i = 0; i < MOVIESOAP_CAT_COUNT; i++ )
			{ arr[i] = getRadio(i); }
	}

	/* Write to disk and load into Moviesoap::config */
	void PreferencesWin::save()
	{
		get(Moviesoap::config.tolerances);
		Moviesoap::config.save();
		hide();
	}

	/* Sets checked for given radiobutton row and col/value */
	inline void PreferencesWin::setRadio(uint8_t row, uint8_t col)
	{
		radios[row][col]->setChecked(true);
	}

	/* Returns col of checked radio. Defaults to max tolerance. */
	inline uint8_t PreferencesWin::getRadio(uint8_t row)
	{
		for (uint8_t col = 0; col < MOVIESOAP_TOLERANCE_COUNT; col++) {
			if (radios[row][col]->isChecked()) {
				return col;
			}
		}
		return MOVIESOAP_TOLERANCE_COUNT;
	}

}

#undef SOAP_BUILD_TOLERANCE_LABEL
#undef SOAP_BUILD_RADIO_BUTTONS
#undef SOAP_BUILD_RADIO_BUTTON