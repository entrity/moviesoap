#include "filter_win.hpp"
#include "filter_frame.hpp"
#include "mod_frame.hpp"
#include "blackout_frame.hpp"
#include "../main.hpp"
#include "../../filter.hpp"
#include "conclude_preview_button.hpp"

#include <vlc_playlist.h>

#include <iostream>
#include <sstream>
using namespace std;

namespace Moviesoap
{
	/* Init vars */
	FilterWin * FilterWin::p_window = NULL;

	/* Constructor */
	FilterWin::FilterWin() : QStackedWidget(NULL)
	{
		setWindowTitle(QString("Moviesoap Filter Editor"));
		filterFrame = new FilterFrame(this);
		addWidget(filterFrame);
		modFrame = new ModFrame(this);
		addWidget(modFrame);
		blackoutFrame = new BlackoutFrame(this);
		addWidget(blackoutFrame);
		concludePreviewButton = new ConcludePreviewButton(QString("Conclude Preview Button"), this);
		// connect(concludePreviewButton, SIGNAL(clicked()), this, SLOT(concludePreview()));
		connect(concludePreviewButton, SIGNAL(closeSignal()), this, SLOT(concludePreview()));
	}

	void FilterWin::openEditor(Filter * filterToEdit)
	{
		// create window if necessary
		if (p_window == NULL)
			p_window = new FilterWin;
		// open file dialogue if no filter is loaded
		if (Moviesoap::p_loadedFilter == NULL)
			Moviesoap::loadFilterDialogue(p_window);
		if (Moviesoap::p_loadedFilter) {
			p_window->editFilter(NULL);
			// show window
			p_window->show();
			p_window->raise();
		}
	}

	/* Set p_editingFilter. Load filter in filter frame. */
	void FilterWin::editFilter(Filter * filterToEdit)
	{
		// pass filter to pane(s)
		filterFrame->load();
		// set visible pane
		setCurrentWidget(filterFrame);
	}

	/* Load and show modFrame. Use this->mod if arg == NULL */
	void FilterWin::editMod(Mod * modToLoad)
	{
		// initialize null mod if arg == NULL
		if (!modToLoad) {
			Moviesoap::defaultizeMod(&Moviesoap::newMod);
			Moviesoap::p_editingMod = &Moviesoap::newMod;
		} else {
			Moviesoap::p_editingMod = modToLoad;
		}
		// pass mod to pane
		modFrame->load( Moviesoap::p_editingMod );
		// set visible pane
		setCurrentWidget(modFrame);
	}

	/* Load and show modFrame */
	void FilterWin::editMod(int i)
	{
		// ensure i corresponds to a mod in modList
		if (i >= 0 && i < Moviesoap::p_loadedFilter->modList.size()) {
			list<Mod>::iterator iter = Moviesoap::p_loadedFilter->modList.begin();
			advance(iter, i);
			editMod(&*iter);
		}
	}

	void FilterWin::deleteMod(int i) {
		// ensure i corresponds to a mod in modList
		if (i >= 0 && i < Moviesoap::p_loadedFilter->modList.size()) {
			// get iterator pointing to mod to be deleted
			list<Mod>::iterator iter = Moviesoap::p_loadedFilter->modList.begin();
			advance(iter, i);
			// delete mod
			Moviesoap::p_loadedFilter->modList.erase(iter);
			// update mod list widget
			filterFrame->refreshModListWidget(Moviesoap::p_loadedFilter);
		}
	}

	void FilterWin::refreshModListWidget() { filterFrame->refreshModListWidget(Moviesoap::p_loadedFilter); }

	void FilterWin::toFilterFrame()
	{
		filterFrame->load();
		setCurrentWidget(filterFrame);
	}

	void FilterWin::toModFrame()
	{
		modFrame->load(Moviesoap::p_editingMod);
		setCurrentWidget(modFrame);
	}
	
	void FilterWin::toBlackoutFrame()
	{
		#ifdef MSDEBUG1
			msg_Info( p_obj, "calling blackoutFrame->load" );
		#endif
		blackoutFrame->load(Moviesoap::p_editingMod);
		#ifdef MSDEBUG1
			msg_Info( p_obj, "done blackoutFrame->load" );
		#endif
		setCurrentWidget(blackoutFrame);
	}

	void FilterWin::save()
	{
		// Attempt save. If fail, perform 'save as'
		if ( Moviesoap::p_loadedFilter->filepath.empty() || Moviesoap::p_loadedFilter->save() )
			saveAs();
	}

	void FilterWin::saveAs()
	{
		// get filepath from user
		char * userDir = config_GetUserDir( VLC_HOME_DIR );
		QString fileName = QFileDialog::getSaveFileName(this, QString("Save filter file"), userDir, QString(MOVIESOAP_FILECHOOSER_FILTER));
		free(userDir);
		if (fileName.isNull())
			return;
		// set filepath on filter obj
		char * cstr = (char *) fileName.toAscii().data();
		Moviesoap::p_loadedFilter->filepath = cstr;
		// force file extension (*.cln)
		if ( !fileName.endsWith(QString(MOVIESOAP_FILE_EXT)) )
			Moviesoap::p_loadedFilter->filepath += MOVIESOAP_FILE_EXT;
		// save
		Moviesoap::p_loadedFilter->save();
	}

	/* Set play time, play, hide this, show concludePreviewButton */
	void FilterWin::preview( Mod * p_mod )
	{
		// require input thread and playlist
		if (p_input == NULL || p_playlist == NULL)
			return;
		// require p_mod
		if (p_mod == NULL)
			p_mod = Moviesoap::p_editingMod;
		if (p_mod == NULL)
			return;
		// dump filter frame to Filter
		filterFrame->dump();
		// calculate start of preview
		mtime_t t_offset = 1000000;
		mtime_t start = p_mod->startTime();
		start = (start > t_offset) ? start - t_offset : 0;
		// change the time and control
		vlc_mutex_lock(&Moviesoap::lock); // todo del?
		var_SetTime( p_input, "time", start );
		vlc_mutex_unlock(&Moviesoap::lock); // todo del?
		playlist_Control( Moviesoap::p_playlist, PLAYLIST_PLAY, false );
		hide();
		concludePreviewButton->show();
	}

	/* Pause play, hide concludePreviewButton, show this */
	void FilterWin::concludePreview()
	{
		show();
		playlist_Control( Moviesoap::p_playlist, PLAYLIST_PAUSE, false );
	}

	void loadFilterDialogue(QWidget * parent)
	{		// Get filepath from user
		QString filepath = QFileDialog::getOpenFileName( parent,
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
				QMessageBox::warning( parent, 
					QString("File IO failure"),
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
}