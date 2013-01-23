#include "filter_frame.hpp"
#include "filter_win.hpp"
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
// temp
#include <cstdio> // unused
#include <iostream>

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

static inline void addLabelAndLine( QLayout *layout, QLineEdit **p_lineEdit, char *text )
{
	layout->addWidget(new QLabel(QObject::tr(text)));
	*p_lineEdit = new QLineEdit;
	layout->addWidget( *p_lineEdit );
}

namespace Moviesoap
{
	/* Fill GUI fields with Filter data */
	void FilterFrame::load(Filter * filter)
	{
		// default to use filterWin->p_editingFilter;
		if (!filter) filter = &filterWin->filter;
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
		if (!filter) filter = &filterWin->filter;
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
		filterWin->updateLoadedFilter();
		filterWin->hide();
	}

	/* Slot. Save filter as. */
	void FilterFrame::saveAsClicked()
	{
		dump();
		filterWin->saveAs();
		filterWin->updateLoadedFilter();
		filterWin->hide();
	}

	/* Slot. Update loadedFilter and close filter editor window. */
	void FilterFrame::okClicked()
	{
		dump();
		filterWin->updateLoadedFilter();
		filterWin->hide();
	}

	/* Slot. Hide editor without update. */
	void FilterFrame::cancelClicked() { filterWin->hide(); }

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
	}

	Mod * FilterFrame::getSelectedMod()
	{
		int i = modListWidget->currentRow();
		list<Mod>::iterator iter = filterWin->filter.getMod(i);
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
			new QListWidgetItem(QString(iter->description.c_str()), modListWidget);
	}

	/* Constructor */
	FilterFrame::FilterFrame(FilterWin *parent) : QFrame(parent), filterWin(parent)
	{
		QVBoxLayout *frameVLayout, *layout = new QVBoxLayout;
		QHBoxLayout *hbox;
		QGridLayout *frameGridLayout;
		QFrame *frame;
		// film div
		frameVLayout = new QVBoxLayout;
		frame = newDiv(frameVLayout);
		hbox = newRow(frameVLayout);
		addLabelAndLine(hbox, &titleText, "Title");
		hbox = newRow(frameVLayout);
		addLabelAndLine(hbox, &yearText, "Year");
		addLabelAndLine(hbox, &isbnText, "Isbn");
		layout->addWidget(frame);
		// mods div (left)
		hbox = new QHBoxLayout;
		frame = newDiv(hbox);
		frameVLayout = new QVBoxLayout;
		frameVLayout->addWidget(new QLabel("Edits"));
		modListWidget = new QListWidget;
		connect(modListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(editModClicked()));
		frameVLayout->addWidget(modListWidget);
		hbox->addLayout(frameVLayout);
		// mods div (right)
		frameVLayout = new QVBoxLayout;
		QPushButton * newModButton = new QPushButton(tr("&New edit")); 
		QPushButton * editModButton = new QPushButton(tr("&Edit edit"));
		QPushButton * delModButton = new QPushButton(tr("&Delete edit"));
		connect(newModButton, SIGNAL(clicked()), this, SLOT(newModClicked()));
		connect(editModButton, SIGNAL(clicked()), this, SLOT(editModClicked()));
		connect(delModButton, SIGNAL(clicked()), this, SLOT(delModClicked()));
		frameVLayout->addWidget(newModButton);
		frameVLayout->addWidget(editModButton);
		frameVLayout->addWidget(delModButton);
		hbox->addLayout(frameVLayout);
		layout->addWidget(frame);
		// preview div
		hbox = new QHBoxLayout;
		frame = newDiv(hbox);
		QPushButton * previewButton = new QPushButton(QString("Preview selected edit"));
		hbox->addWidget(previewButton);
		connect(previewButton, SIGNAL(clicked()), this, SLOT(previewClicked()));
		layout->addWidget(frame);
		// creation div
		frameVLayout = new QVBoxLayout;
		frame = newDiv(frameVLayout);
		hbox = newRow(frameVLayout);
		addLabelAndLine(hbox, &creatorText, "Filter maker");
		hbox = newRow(frameVLayout);
		QPushButton * saveButton = new QPushButton(tr("&Save"));
		QPushButton * saveAsButton = new QPushButton(tr("S&ave as..."));
		QPushButton * okButton = new QPushButton(tr("&Update w/out save"));
		QPushButton * cancelButton = new QPushButton(tr("&Cancel"));
		connect(saveButton, SIGNAL(clicked()), this, SLOT(saveClicked()));
		connect(saveAsButton, SIGNAL(clicked()), this, SLOT(saveAsClicked()));
		connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
		connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
		hbox->addWidget(saveButton);
		hbox->addWidget(saveAsButton);
		hbox->addWidget(okButton);
		hbox->addWidget(cancelButton);
		layout->addWidget(frame);
		// end
		setLayout(layout);
	}

}