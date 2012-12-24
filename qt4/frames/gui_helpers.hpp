#ifndef MOVIESOAP_GUI_HELPERS_H
#define MOVIESOAP_GUI_HELPERS_H

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

#include <string>
using namespace std;
#include <cstdio>

namespace Moviesoap
{
	/* Add a QRadioButton to given layout */
	inline QRadioButton * addRadio(QBoxLayout * layout, const char * text=NULL) {
		QRadioButton * btn = text ? new QRadioButton(QString(text)) : new QRadioButton;
		layout->addWidget(btn, 0, Qt::AlignCenter);
		return btn;
	}
	/* Add a QLineEdit to given layout */
	inline QLineEdit * addText(QLayout * layout, const char * placeholder=NULL) {
		QLineEdit * text = new QLineEdit;
		if (placeholder) text->setPlaceholderText(QString(placeholder));
		layout->addWidget(text);
		return text;
	}
	inline QLineEdit * addTextToGrid(QGridLayout * gridLayout, int row, int col, const char * placeholder=NULL) {
		QLineEdit * text = new QLineEdit;
		if (placeholder) text->setPlaceholderText(QString(placeholder));
		gridLayout->addWidget(text, row, col);
		return text;
	}
	/* Add a QWidget to given layout */
	inline QLabel * addLabel(QLayout * layout, const char * text) {
		QLabel * lbl = new QLabel(QString(text));
		layout->addWidget(lbl);
		return lbl;
	}
	/* Add a QPushButton to given layout */
	inline QPushButton * addButton(QLayout * layout, const char * text) {
		QPushButton * btn = new QPushButton(QString(text));
		layout->addWidget(btn);
		return btn;
	}
	/* Add a label as a row to given QVBoxLayout */
	inline void addLabelRow(QBoxLayout * layout, const char * text) {
		QHBoxLayout * hbox = new QHBoxLayout;
		QLabel * lbl = new QLabel(QString(text));
		hbox->addWidget(lbl, 0);
		layout->addLayout(hbox);
	}
	/* Set value of QLineEdit */
	inline void setText(QLineEdit * input, const string &text) { input->setText(QObject::tr(text.c_str())); }
	/* Return a string 00:00:00.00 for time_t in us*MOVIESOAP_MOD_TIME_FACTOR (= centiseconds) */
	inline void strftime(uint32_t t, char dst[12]) {
		uint8_t cs, sec, min, hr;
		cs = t % 100;
		t /= 100;
		sec = t % 60;
		t /= 60;
		min = t % 60;
		t /= 60;
		hr = t;
		sprintf( dst, "%02d:%02d:%02d.%02d", hr, min, sec, cs );
	}
	/* Return a time in units us*MOVIESOAP_MOD_TIME_FACTOR (= centiseconds) from string formatted 00:00:00.00 */
	inline time_t strptime(char * text) {
		int n, // number of slots filled with sscanf
			i; // counter for iterator used later
		int ints[3];
		time_t centiseconds = 0;
		// get total before '.'
		n = sscanf(text, "%d:%d:%d", &ints[0], &ints[1], &ints[2]);
		for (i = 0; i < n; i++) {
			centiseconds *= 60;
			centiseconds += ints[i];
		}
		centiseconds *= 100; // seconds to centiseconds
		// get total after '.'
		for (i = 0; i < strlen(text); i ++)  // find decimal
			{ if (text[i] == '.') break; }
		n = sscanf(&text[i], ".%1d%1d%1d", &ints[0], &ints[1], &ints[2]);
		if (n >= 1) 
			centiseconds += 10 * ints[0]; // add tenths place
		if (n >= 2)
			centiseconds += ints[1]; // add hundredths place
		if (n >= 3 && ints[2] >= 5)
			centiseconds += 1; // consider thousandths place
		// return
		return centiseconds;
	}
	/* Returns first index of pointer whose button isChecked(). Defaults to -1. Takes an array of pointers. */
	inline int getChecked(QRadioButton *firstPointer[], int nPointers) {
		for (int i=0; i < nPointers; i++) {
			if (firstPointer[i]->isChecked()) return i;
		}
		return -1;
	}
}

#endif