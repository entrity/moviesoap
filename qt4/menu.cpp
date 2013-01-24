#include "main.hpp"
#include "menu.hpp"
#include "config.hpp"
#include "frames/preferences.hpp"
#include "frames/filter_win.hpp"
#include "../filter.hpp"

#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>

#include <iostream>
#include <sstream>
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
		connect( actionLoad, SIGNAL(triggered()), this, SLOT(loadFilter()) );
		connect( actionActive, SIGNAL(triggered()), this, SLOT(activeTriggered()) );
		connect( actionPreferences, SIGNAL(triggered()), this, SLOT(editPreferences()) );
		connect( actionEdit, SIGNAL(triggered()), this, SLOT(editFilter()) );
		connect( actionNew, SIGNAL(triggered()), this, SLOT(newFilter()) );
		// connect other slots
		connect( this, SIGNAL(updatesAvailable()), this, SLOT(openUpdatesMessageBox()) );
	}

	/* Called by menu creation in modules/gui/qt4/... */
	Menu * Menu::create(QMenuBar * bar) { return new Menu(bar); }

	void Menu::activeTriggered() { config.active = actionActive->isChecked(); }

	void Menu::editPreferences() { Moviesoap::PreferencesWin::openEditor(); }

	void Menu::editFilter() { Moviesoap::FilterWin::openEditor(Moviesoap::p_loadedFilter); }

	void Menu::newFilter() {
		Moviesoap::p_loadedFilter = new Moviesoap::Filter;
		Moviesoap::FilterWin::openEditor(NULL);
	}

	void Menu::loadFilter() {
		// Get filepath from user
		QString filepath = QFileDialog::getOpenFileName( this,
			QString("Open Moviesoap filter file"),
			QString(saveDir().c_str()),
			QString(MOVIESOAP_FILECHOOSER_FILTER));
		if ( !filepath.isEmpty() ) {			
			vlc_mutex_lock( &Moviesoap::lock );
			// Ensure existence of loaded filter
			if (Moviesoap::p_loadedFilter == NULL)
				Moviesoap::p_loadedFilter = new Filter;
			// Stop loaded filter in case it is running
			vlc_mutex_unlock( &Moviesoap::lock );
			Moviesoap::spawn_stop_filter();
			// Overwrite loaded filter with data from filter file
			const char * c_filepath = qPrintable(filepath);
			int err = Moviesoap::p_loadedFilter->load( c_filepath );
			// Display error (if any) in QMessageBox
			if (err) {
				stringstream msgs;
				msgs << "Failure to load filter from file.\nError code " << err;
				QMessageBox::warning( this, 
					tr("File IO failure"),
					QString(msgs.str().c_str()),
					QMessageBox::Ok);
				return;
			}
			// if no error:
			#ifdef MSDEBUG1
				msg_Info( p_obj, "OLD FILTER OVERWRITTEN" );
				msg_Info( p_obj, "IS ACTIVE SELECTED: %d", isActiveSelected() );
			#endif
			// Start loaded filter if menu has active selected
			if ( isActiveSelected() ) {
				if (Moviesoap::p_input == NULL)
					 spawn_set_p_input(false);
				#ifdef MSDEBUG1
					msg_Info(p_obj, "p input: %x", p_input);
				#endif
				if (Moviesoap::p_input)
					Moviesoap::spawn_restart_filter();
			}
		}
	}

	void Menu::notifyUpdatesAvailable() { emit updatesAvailable(); }
	
	void Menu::openUpdatesMessageBox()
	{
		QMessageBox::information( Moviesoap::p_GuiMenu, 
					QMessageBox::tr("Update notification"),
					QString("Updates are available for Moviesoap.\nPlease go to http://moviesoap.org to download the latest version."),
					QMessageBox::Ok);
	}

}