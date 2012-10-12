#include "../main.hpp"
#include "filter_frame.hpp"
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
#include <QFileDialog>
#include <cstdio> // unused

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
	inline void setText(QLineEdit * input, const string &text) { input->setText(QString(text.c_str())); }

	/* Update loadedFilter and hide filter editor window. */
	void FilterFrame::close()
	{
		*p_loadedFilter = *p_editingFilter;
		((QFrame *) parentWidget())->hide();
		delete p_editingFilter;
		p_editingFilter = NULL;
	}

	/* Fill GUI fields with Filter data */
	void FilterFrame::load(Filter * filter)
	{
		p_editingFilter = filter;
		// text inputs
		setText(titleText, filter->title);
		setText(yearText, filter->year);
		setText(isbnText, filter->isbn);
		setText(creatorText, filter->creator);
		// list input
		modListWidget->clear();
		list<Mod>::iterator iter;
		for (iter = filter->modList.begin(); iter != filter->modList.end(); iter++)
			new QListWidgetItem(QString(iter->description.c_str()), modListWidget);
	}

	/* Set filter string data to GUI field values */
	void FilterFrame::dump(Filter * filter)
	{
		// Get string values
		filter->title = titleText->text().toStdString();
		filter->year = yearText->text().toStdString();
		filter->isbn = isbnText->text().toStdString();
		filter->creator = creatorText->text().toStdString();
		// Mods should be updated as they are changed in the QListWidget
	}

	/* Slot. Save filter. */
	void FilterFrame::saveClicked()
	{
		// Return if no filter exists
		if (!p_editingFilter)
			{ cout << "no filter" << endl; return;}
		// update editingFilter
		dump(p_editingFilter);
		// Attempt save; if fail, perform 'save as'
		if (p_editingFilter->filepath.empty() || !p_editingFilter->save())
			saveAsClicked();
		// close editor window
		close();
	}

	/* Slot. Save filter as. */
	void FilterFrame::saveAsClicked()
	{
		// get filepath from user
		char * userDir = config_GetUserDir( VLC_HOME_DIR );
		QString fileName = QFileDialog::getSaveFileName(this, QString("Save filter file"), userDir, QString("Filter files (*.cln)"));
		VLC_UNUSED(userDir);
		if (fileName.isNull())
			return;
		// set filepath on editingFilter
		p_editingFilter->filepath = fileName.toStdString();
		// force .cln file extension
		if ( !fileName.endsWith(QString(MOVIESOAP_FILE_EXT)) )
			p_editingFilter->filepath += MOVIESOAP_FILE_EXT;
		// save
		p_editingFilter->save();
		// close editor window
		close();
	}

	/* Slot. Update loadedFilter and close filter editor window. */
	void FilterFrame::okClicked() {
		close(); 
	}

	/* Constructor */
	FilterFrame::FilterFrame(QWidget *parent) : QFrame(parent), p_editingFilter(NULL)
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
		frameVLayout->addWidget(modListWidget);
		hbox->addLayout(frameVLayout);
		// mods div (right)
		frameVLayout = new QVBoxLayout;
		QPushButton * newModButton = new QPushButton(tr("New edit")); 
		QPushButton * editModButton = new QPushButton(tr("Edit edit"));
		QPushButton * delModButton = new QPushButton(tr("Delete edit"));
		QPushButton * previewButton = new QPushButton(tr("Preview"));
		frameVLayout->addWidget(newModButton);
		frameVLayout->addWidget(editModButton);
		frameVLayout->addWidget(previewButton);
		hbox->addLayout(frameVLayout);
		layout->addWidget(frame);
		// creation div
		frameVLayout = new QVBoxLayout;
		frame = newDiv(frameVLayout);
		hbox = newRow(frameVLayout);
		addLabelAndLine(hbox, &creatorText, "Filter maker");
		hbox = newRow(frameVLayout);
		QPushButton * saveButton = new QPushButton(tr("Save"));
		QPushButton * saveAsButton = new QPushButton(tr("Save as..."));
		QPushButton * okButton = new QPushButton(tr("Update without saving"));
		connect(saveButton, SIGNAL(clicked()), this, SLOT(saveClicked()));
		connect(saveAsButton, SIGNAL(clicked()), this, SLOT(saveAsClicked()));
		connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
		hbox->addWidget(saveButton);
		hbox->addWidget(saveAsButton);
		hbox->addWidget(okButton);
		layout->addWidget(frame);
		// end
		setLayout(layout);
	}

}