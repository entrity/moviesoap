#include "filter_frame.hpp"
#include "../../filter.hpp"
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QString>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTranslator>

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

	void FilterFrame::load(Filter * filter)
	{
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

	/* Constructor */
	FilterFrame::FilterFrame(QWidget * parent) : QFrame(parent)
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
		QPushButton * saveButton = new QPushButton(tr("Save to file"));
		QPushButton * saveAsButton = new QPushButton(tr("Save as..."));
		QPushButton * okButton = new QPushButton(tr("Ok"));
		hbox->addWidget(saveButton);
		hbox->addWidget(saveAsButton);
		hbox->addWidget(okButton);
		layout->addWidget(frame);
		// end
		setLayout(layout);
	}

}