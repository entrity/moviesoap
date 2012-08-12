#include "main.hpp"
#include "../filter.hpp"
#include <vlc_common.h>
#include <vlc_interface.h>
#include <QMenu>
#include <QMenuBar>
// #include <QFileDialog>

#include <cstdio>

namespace Moviesoap
{
	vlc_object_t * p_obj = NULL;
	Filter * loadedFilter = NULL;

	void init( intf_thread_t * p_intf, MainInterface * mainInterface )
	{
		// save vlc object
		p_obj = VLC_OBJECT(p_intf);
		printf("---- ---- IN MOVIEOSAP INIT\n");
		printf("---- %x\n", (unsigned int) p_obj);

		// test dummy filter
		Filter * p_filter = Moviesoap::Filter::dummy();
		p_filter->save();
	}
}

namespace Moviesoap
{
	/* Constructor */
	Menu::Menu( QWidget * parent ) : QMenu(parent)
	{
		// create actions
		QAction * actionLoad = new QAction("&Load filter...", this);
		QAction * actionNew = new QAction("&New filter...", this);
		QAction * actionEdit = new QAction("&Edit filter...", this);
		// add actions
		// addAction(actionLoad);
		// addSeparator();
		// addAction(actionNew);
		// addAction(actionEdit);
		// connect actions
		// connect( actionEdit, SIGNAL(triggered()), this, SLOT(editFilter()) );
	}

	/* Method called in gui/qt4/menus.cpp */
	Menu * Menu::create( QMenuBar * bar ) { return new Menu(bar); }

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