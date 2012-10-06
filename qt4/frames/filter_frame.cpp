#include "filter.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QString>
#include <QLineEdit>
#include <QListWidget>

namespace Moviesoap
{
	/* Constructor */
	FilterWin::FilterWin() : QFrame(NULL)
	{
		QVBoxLayout * vbox = new QVBoxLayout;
		QHBoxLayout * hbox;
		// top div
		hbox = new QHBoxLayout;
		hbox->addWidget(new QLabel(QString("Title")));
		vbox->addLayout(hbox);
		// end
		setLayout(vbox);
	}

	/* Open editor window */
	void FilterWin::openEditor()
	{
		if (p_window == NULL)
			p_window = new FilterWin;
		p_window->show();
	}
}