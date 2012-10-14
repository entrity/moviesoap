#include "mod_frame.hpp"
#include "../main.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

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

namespace Moviesoap
{
	void ModFrame::load(Mod * mod)
	{
		p_editingMod = mod;
	}

	/* Constructor */
	ModFrame::ModFrame(QWidget *parent) : QFrame(parent)
	{
		// base layout
		QVBoxLayout * layout = new QVBoxLayout;
		QHBoxLayout * hbox;
		int i;
		// mode div
		addLabelRow(layout, "Mode:");
		hbox = new QHBoxLayout;
		modeRadios[0] = addRadio(hbox, "Skip");
		modeRadios[1] = addRadio(hbox, "Mute");
		modeRadios[2] = addRadio(hbox, "Blackout");
		QPushButton *blackoutButton = addButton(hbox, "Set blackout bounds");
		layout->addLayout(hbox);
		// time div
		addLabelRow(layout, "Time:");
		hbox = new QHBoxLayout;
		hbox->addWidget(new QLabel(QString("from")));
		startText = addText(hbox, "00:00:00.00");
		startText->setFixedWidth(110);
		hbox->addWidget(new QLabel(QString("to")));
		stopText = addText(hbox, "00:00:00.00");
		stopText->setFixedWidth(110);
		layout->addLayout(hbox);
		// tolerance div
		hbox = new QHBoxLayout;
		addLabel(hbox, "Offensiveness (scarcely <-> very)");
		for (i = 0; i < MOVIESOAP_TOLERANCE_COUNT-1; i++) {
			severityRadios[i] = addRadio(hbox);	}
		layout->addLayout(hbox);
		// category div
		hbox = new QHBoxLayout;
		addLabel(hbox, "Category:");
		categoryBox = new QComboBox;
		for (i = 0; i < MOVIESOAP_CAT_COUNT; i ++) {
			categoryBox->addItem(QString(toleranceLabels[i]), QVariant(i)); }
		hbox->addWidget(categoryBox);
		layout->addLayout(hbox);
		// Submit div
		hbox = new QHBoxLayout;
		QPushButton *okButton = addButton(hbox, "Ok");
		QPushButton *cancelButton = addButton(hbox, "Cancel");
		layout->addLayout(hbox);
		// finish
		hbox = new QHBoxLayout;
		layout->addLayout(hbox, 9);
		setLayout(layout);
	}

	
}