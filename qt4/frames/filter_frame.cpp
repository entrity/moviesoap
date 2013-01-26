#include "filter_frame.hpp"
#include "filter_win.hpp"
#include "mod_list_widget_item.hpp"
#include "gui_helpers.hpp"
#include "../main.hpp"
#include "../../filter.hpp"

#include <vlc_configuration.h>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QString>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTranslator>
#include <QMessageBox>
#include <QCheckBox>

#ifdef MSDEBUG1
	// #include <cstdio> // unused
	#include <iostream>
	using namespace std;
#endif

// todo : needs freeing?
#define QSTRING_TO_C(dst, qstr) do { \
	dst = qstr.toAscii().data(); \
	} while (0)


/* Create padded, bordered QFrame, using given layout */
static inline QFrame *newDiv(QLayout * layout)
{
	QFrame * frame = new QFrame;
	frame->setLayout(layout);
	frame->setLineWidth(1);
	frame->setFrameStyle(QFrame::Plain | QFrame::Box);
	frame->setContentsMargins(0,1,0,1);
	return frame;
}

static inline QHBoxLayout *newRow(QVBoxLayout * vbox)
{
	QHBoxLayout * hbox = new QHBoxLayout;
	vbox->addLayout(hbox);
	return hbox;
}

static inline void addLabelAndLine( QLayout *layout, QLineEdit **p_lineEdit, char *text, char *placeholder=NULL )
{
	layout->addWidget(new QLabel(QObject::tr(text)));
	*p_lineEdit = new QLineEdit;
	if (placeholder)
		(*p_lineEdit)->setPlaceholderText(QString(placeholder));
	layout->addWidget( *p_lineEdit );
}

namespace Moviesoap
{
	/* Fill GUI fields with Filter data */
	void FilterFrame::load(Filter * filter)
	{
		// default to use filterWin->p_editingFilter;
		if (!filter) filter = Moviesoap::p_loadedFilter;
		// text inputs
		setText(titleText, filter->title);
		setText(yearText, filter->year);
		setText(isbnText, filter->isbn);
		setText(creatorText, filter->creator);
		// list input
		refreshModListWidget(filter);
	}

	/* Set filter string data to GUI field values. */
	void FilterFrame::dump(Filter * filter)
	{
		// default to use filterWin->p_editingFilter;
		if (!filter) filter = Moviesoap::p_loadedFilter;
		// Get string values
		QSTRING_TO_C( filter->title, titleText->text() );
		QSTRING_TO_C( filter->year, yearText->text() );
		QSTRING_TO_C( filter->isbn, isbnText->text() );
		QSTRING_TO_C( filter->creator, creatorText->text() );
	}

	/* Slot. Save filter. */
	void FilterFrame::saveClicked()
	{
		dump();
		filterWin->save();
	}

	/* Slot. Save filter as. */
	void FilterFrame::saveAsClicked()
	{
		dump();
		filterWin->saveAs();
	}

	/* Slot. Update loadedFilter and close filter editor window. */
	void FilterFrame::okClicked()
	{
		dump();
		filterWin->hide();
	}

	/* Slot. Save; update loadedFilter; and close filter editor window */
	void FilterFrame::okAndSaveClicked()
	{
		dump();
		filterWin->save();
		filterWin->hide();
	}

	/* Slot. Add new mod to list */
	void FilterFrame::newModClicked() {
		dump();
		filterWin->editMod();
	}

	/* Slot. Delete selected mod from filter. */
	void FilterFrame::delModClicked()
	{
		int i = modListWidget->currentRow();
		filterWin->deleteMod(i);
		dump();
	}

	Mod * FilterFrame::getSelectedMod()
	{
		int i = modListWidget->currentRow();
		list<Mod>::iterator iter = Moviesoap::p_loadedFilter->getMod(i);
		return &*iter;
	}

	/* Slot. Edit currently selected mod in list */
	void FilterFrame::editModClicked()
	{
		dump();
		int i = modListWidget->currentRow();
		if (i == -1) {
			modListWidget->setFocus(Qt::OtherFocusReason);
			modListWidget->setCurrentRow(0);
		} else
			filterWin->editMod(i);
	}

	void FilterFrame::previewClicked()
	{
		Mod * mod = getSelectedMod();
		if (mod == NULL)
			return;
		filterWin->preview( mod );
	}

	void FilterFrame::refreshModListWidget(Filter * filter)
	{
		// sort
		filter->modList.sort();
		// clear
		modListWidget->clear();
		// add mods
		list<Mod>::iterator iter;
		for (iter = filter->modList.begin(); iter != filter->modList.end(); iter++)
			new ModListWidgetItem(modListWidget, &*iter);
	}

	void FilterFrame::quickCreateMod(uint8_t mode)
	{
		if (Moviesoap::p_input) {
			Moviesoap::quickCreateMod(mode);
		} else {
			QMessageBox::information( this,
					QString("No action taken"),
					QString("To make use of the quick-create functions, media must be playing."),
					QMessageBox::Ok);
		}
	}

	/* Constructor */
	FilterFrame::FilterFrame(FilterWin *parent) : QFrame(parent), filterWin(parent), p_quickCreatedMod(NULL)
	{
		QVBoxLayout *frameVLayout, *layout = new QVBoxLayout;
		QHBoxLayout *hbox;
		QGridLayout *frameGridLayout;
		QFrame *frame;
		// mods div (left)
		hbox = new QHBoxLayout;
		frame = newDiv(hbox);
		frameVLayout = new QVBoxLayout;
		frameVLayout->addWidget(new QLabel("Edits"));
		modListWidget = new ModListWidget(this);
		connect(modListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(editModClicked()));
		frameVLayout->addWidget(modListWidget);
		hbox->addLayout(frameVLayout);
		// mods div (right)
		frameVLayout = new QVBoxLayout;
		QPushButton * newModButton = new QPushButton(tr("&New mod")); 
		QPushButton * editModButton = new QPushButton(tr("&Edit mod"));
		QPushButton * delModButton = new QPushButton(tr("Delete mod"));
		QPushButton * previewButton = new QPushButton(QString("Preview\nselected mod"));
		connect(newModButton, SIGNAL(clicked()), this, SLOT(newModClicked()));
		connect(editModButton, SIGNAL(clicked()), this, SLOT(editModClicked()));
		connect(delModButton, SIGNAL(clicked()), this, SLOT(delModClicked()));
		connect(previewButton, SIGNAL(clicked()), this, SLOT(previewClicked()));
		frameVLayout->addWidget(newModButton);
		frameVLayout->addWidget(editModButton);
		frameVLayout->addWidget(delModButton);
		frameVLayout->addWidget(previewButton);
		hbox->addLayout(frameVLayout);
		layout->addWidget(frame);
		// quick mod creation div
		frameVLayout = new QVBoxLayout;
		frame = newDiv(frameVLayout);
		hbox = newRow(frameVLayout);
		addLabel(hbox, "Quick-create mod");
		hbox = newRow(frameVLayout);
		QPushButton * quickSkipModButton = addButton(hbox, "Ski&p");
		QPushButton * quickMuteModButton = addButton(hbox, "&Mute");
		QPushButton * quickBlackModButton = addButton(hbox, "&Black");
		connect(quickSkipModButton, SIGNAL(clicked()), this, SLOT(quickCreateSkipMod()));
		connect(quickMuteModButton, SIGNAL(clicked()), this, SLOT(quickCreateMuteMod()));
		connect(quickBlackModButton, SIGNAL(clicked()), this, SLOT(quickCreateBlackoutMod()));
		addLabel(hbox, " offset backward from now:");
		parent->p_quickCreateOffsetText = addText(hbox, "00:00:01.0");
		hbox = newRow(frameVLayout);
		parent->p_ultraQuickModCheckbox = new QCheckBox(tr("enable ultra-quick mod creation (hotkeys create mods)"));
		hbox->addWidget(parent->p_ultraQuickModCheckbox);
		layout->addWidget(frame);
		// film div
		frameVLayout = new QVBoxLayout;
		frame = newDiv(frameVLayout);
		hbox = newRow(frameVLayout);
		addLabelAndLine(hbox, &titleText, "Title");
		hbox = newRow(frameVLayout);
		addLabelAndLine(hbox, &yearText, "Year");
		addLabelAndLine(hbox, &isbnText, "Isbn");
		hbox = newRow(frameVLayout);
		addLabelAndLine(hbox, &creatorText, "Filter maker", "your name");
		layout->addWidget(frame);
		// creation buttons row		
		hbox = newRow(frameVLayout);
		QPushButton * okButton = new QPushButton(tr("&Ok"));
		QPushButton * okAndSaveButton = new QPushButton(tr("Save and O&k"));
		QPushButton * saveButton = new QPushButton(tr("&Save"));
		QPushButton * saveAsButton = new QPushButton(tr("S&ave as..."));
		connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
		connect(okAndSaveButton, SIGNAL(clicked()), this, SLOT(okAndSaveClicked()));
		connect(saveButton, SIGNAL(clicked()), this, SLOT(saveClicked()));
		connect(saveAsButton, SIGNAL(clicked()), this, SLOT(saveAsClicked()));
		hbox->addWidget(okButton);
		hbox->addWidget(okAndSaveButton);
		hbox->addWidget(saveButton);
		hbox->addWidget(saveAsButton);
		layout->addWidget(frame);
		// end
		setLayout(layout);
	}

}	