#include "main.hpp"
#include "menu.hpp"

#include <QMenu>
#include <QMenuBar>
#include <QAction>
// #include <QFileDialog>


namespace Moviesoap
{
	/* Constructor */
	Menu::Menu( QWidget * parent ) : QMenu(parent)
	{
		// create actions
		QAction * actionLoad = new QAction("&Load filter...", this);
		QAction * actionNew = new QAction("&New filter...", this);
		QAction * actionEdit = new QAction("&Edit filter...", this);
		QAction * actionPreferences = new QAction("&Set Tolerances...", this);
		QAction * actionActive = new QAction("&Active...", this);
		// add actions
		addAction(actionLoad);
		addSeparator();
		addAction(actionNew);
		addAction(actionEdit);
		addSeparator();
		addAction(actionPreferences);
		addAction(actionActive);
		actionActive->setCheckable(true);
		actionActive->setChecked(true);
		// connect actions
		// connect( actionEdit, SIGNAL(triggered()), this, SLOT(editFilter()) );
	}


	//static Menu * create(QMenuBar * bar);
	Menu * Menu::create(QMenuBar * bar) { return new Menu(bar); }

	/* Public slot fired by menubar action. Opens file dialogue. */
	// void Menu::loadFilter()
	// {
	// 	const char * fstring;
	// 	QString filename = QFileDialog::getOpenFileName(
	// 		window(),
	// 		tr("Choose a filter file"),
	// 		datadir(),
	// 		tr("Mediasoap filters (*.msf)")
	// 	);
	// 	fstring = filename.toStdString().c_str();
	// 	printf( "fname as c: %s\n", fstring );
	// 	// todo
	// }

	// void Menu::editFilter()
	// {

	// }

}