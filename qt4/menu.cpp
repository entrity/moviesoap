#include "main.hpp"
#include "menu.hpp"
#include "config.hpp"
#include "frames/preferences.hpp"
#include "frames/filter_win.hpp"
#include "../filter.hpp"

#include <QMenu>
#include <QMenuBar>
#include <QAction>
// #include <QFileDialog>

// test
#include <iostream>
using namespace std;

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
		actionActive = new QAction("&Active...", this);
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
		connect( actionActive, SIGNAL(triggered()), this, SLOT(activeTriggered()) );
		connect( actionPreferences, SIGNAL(triggered()), this, SLOT(editPreferences()) );
		connect( actionEdit, SIGNAL(triggered()), this, SLOT(editFilter()) );
		connect( actionNew, SIGNAL(triggered()), this, SLOT(newFilter()) );
	}

	/* Called by menu creation in modules/gui/qt4/... */
	Menu * Menu::create(QMenuBar * bar) { return new Menu(bar); }

	void Menu::activeTriggered() { config.active = actionActive->isChecked(); }

	void Menu::editPreferences() { Moviesoap::PreferencesWin::openEditor(); }

	void Menu::editFilter() {
		cout << "loaded filter: " << hex << p_loadedFilter << endl;
		Moviesoap::FilterWin::openEditor(Moviesoap::p_loadedFilter);
	}

	void Menu::newFilter() {
		Moviesoap::FilterWin::openEditor(NULL);
	}

}