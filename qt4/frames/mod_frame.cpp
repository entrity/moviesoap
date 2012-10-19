#include "mod_frame.hpp"
#include "../main.hpp"
#include "gui_helpers.hpp"
#include "filter_win.hpp"

// temp
#include <iostream>
using namespace std;

namespace Moviesoap
{
	FilterWin *ModFrame::filterWin() { return (FilterWin *) parent(); }

	/* Read fields into Mod */
	void ModFrame::dump(Mod * mod)
	{
		// mode
		mod->mod.mode = getChecked(modeRadios, MOVIESOAP_MOD_MODE_COUNT);
		// times
		mod->mod.start = strptime(startText->text().toAscii().data());
		mod->mod.stop = strptime(stopText->text().toAscii().data());
		// severity
		mod->mod.severity = getChecked(severityRadios, MOVIESOAP_TOLERANCE_COUNT);
		// category
		mod->mod.category = categoryBox->currentIndex();
		// description
		mod->description = descText->text().toStdString();
	}

	/* Set fields, p_editingMod */
	void ModFrame::load(Mod * mod)
	{
		p_editingMod = mod;
		// mode
		modeRadios[mod->mod.mode]->setChecked(true);
		// times
		char buffer[12];		
		strftime(mod->mod.start, buffer);
		startText->setText(buffer);
		strftime(mod->mod.stop,  buffer);
		stopText->setText(buffer);
		// severity
		if (mod->mod.severity >= 0 && mod->mod.severity < MOVIESOAP_TOLERANCE_COUNT)
			severityRadios[mod->mod.severity]->setChecked(true);
		// category
		if (mod->mod.category >= 0 && mod->mod.category < MOVIESOAP_CAT_COUNT)
			categoryBox->setCurrentIndex(mod->mod.category);
		// description
		descText->setText(QString(mod->description.c_str()));
	}

	/* Constructor */
	ModFrame::ModFrame(QWidget *parent) : QFrame(parent)
	{
		
		// base layout
		QVBoxLayout * layout = new QVBoxLayout;
		QVBoxLayout * vbox;
		QHBoxLayout * hbox;
		QFrame * frame;
		int i;
		// mode div
		frame = new QFrame;
		vbox = new QVBoxLayout;
		frame->setLayout(vbox);
		addLabelRow(vbox, "Mode:");
		hbox = new QHBoxLayout;
		modeRadios[MOVIESOAP_SKIP] = addRadio(hbox, "Skip");
		modeRadios[MOVIESOAP_MUTE] = addRadio(hbox, "Mute");
		modeRadios[MOVIESOAP_BLACKOUT] = addRadio(hbox, "Blackout");
		QPushButton *blackoutButton = addButton(hbox, "Set blackout bounds");
		vbox->addLayout(hbox);
		layout->addWidget(frame);
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
		// severity/tolerance div
		frame = new QFrame;
		vbox = new QVBoxLayout;
		frame->setLayout(vbox);
		addLabel(hbox, "Offensiveness (scarcely <-> very)");
		hbox = new QHBoxLayout;
		for (i = 0; i < MOVIESOAP_TOLERANCE_COUNT-1; i++) {
			severityRadios[i] = addRadio(hbox);	}
		vbox->addLayout(hbox);
		layout->addWidget(frame);
		// category div
		hbox = new QHBoxLayout;
		addLabel(hbox, "Category:");
		categoryBox = new QComboBox;
		for (i = 0; i < MOVIESOAP_CAT_COUNT; i ++) {
			categoryBox->addItem(QString(toleranceLabels[i]), QVariant(i)); }
		hbox->addWidget(categoryBox);
		layout->addLayout(hbox);
		// description div
		hbox = new QHBoxLayout;
		addLabel(hbox, "Description:");
		descText = addText(hbox, "e.g. skip shootout scene");
		layout->addLayout(hbox);
		// Submit div
		hbox = new QHBoxLayout;
		QPushButton * okButton = addButton(hbox, "Ok");
		QPushButton * cancelButton = addButton(hbox, "<< (cancel)");
		layout->addLayout(hbox);
		connect( okButton, SIGNAL(clicked()), this, SLOT(okClicked()) );
		connect( cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()) );
		// finish
		hbox = new QHBoxLayout;
		layout->addLayout(hbox, 9);
		setLayout(layout);
	}

	/* Return to filterFrame view. */
	void ModFrame::cancelClicked()
	{
		filterWin()->setCurrentIndex(0);
	}

	/* Dump fields into mod in editingMod.modList. Return to filterFrame view. */
	void ModFrame::okClicked()
	{
		// make new mod, point to it
		if (!p_editingMod) {
			Filter * p_editingFilter = filterWin()->p_editingFilter;
			Mod newMod;
			p_editingFilter->modList.push_back(newMod);
			p_editingMod = &p_editingFilter->modList.back();
		}
		// dump data to editingMod
		dump(p_editingMod);
		// return to filterFrame view
		filterWin()->refreshModListWidget();
		filterWin()->setCurrentIndex(0);
	}

	
}