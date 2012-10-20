#include "filter_win.hpp"
#include "filter_frame.hpp"
#include "mod_frame.hpp"
#include "blackout_frame.hpp"
#include "../main.hpp"
#include "../../filter.hpp"

#include <QFileDialog>

// test
#include <iostream>
using namespace std;

namespace Moviesoap
{
	/* Init vars */
	FilterWin * FilterWin::p_window = NULL;

	/* Constructor */
	FilterWin::FilterWin() : QStackedWidget(NULL), p_mod(NULL)
	{
		setWindowTitle(QString("Moviesoap Filter Editor"));
		filterFrame = new FilterFrame(this);
		addWidget(filterFrame);
		modFrame = new ModFrame(this);
		addWidget(modFrame);
		blackoutFrame = new BlackoutFrame(this);
		addWidget(blackoutFrame);
	}

	void FilterWin::openEditor(Filter * filterToEdit)
	{
		// create filter if necessary
		if (p_window == NULL)
			p_window = new FilterWin;
		p_window->editFilter(filterToEdit);
		// show window
		p_window->show();
		p_window->raise();
	}

	/* Set p_editingFilter. Load filter in filter frame. */
	void FilterWin::editFilter(Filter * filterToEdit)
	{
		if (filterToEdit)
			filter = *filterToEdit;
		else
			filter.nullify();
		// pass filter to pane(s)
		filterFrame->load(&filter);
		// set visible pane
		setCurrentWidget(filterFrame);
	}

	/* Load and show modFrame. Use this->mod if arg == NULL */
	void FilterWin::editMod(Mod * modToLoad)
	{
		// initialize null mod if arg == NULL
		if (!modToLoad) {
			mod = blankMod;
			p_mod = &mod;
		} else {
			p_mod = modToLoad;
		}
		// pass mod to pane
		modFrame->load(p_mod);
		// set visible pane
		setCurrentWidget(modFrame);
	}

	/* Load and show modFrame */
	void FilterWin::editMod(int i)
	{
		// ensure i corresponds to a mod in modList
		if (i >= 0 && i < filter.modList.size()) {
			list<Mod>::iterator iter = filter.modList.begin();
			advance(iter, i);
			editMod(&*iter);
		}
	}

	void FilterWin::refreshModListWidget() { filterFrame->refreshModListWidget(&filter); }

	void FilterWin::toFilterFrame()
	{
		filterFrame->load(&filter);
		setCurrentWidget(filterFrame);
	}

	void FilterWin::toModFrame()
	{
		modFrame->load(p_mod);
		setCurrentWidget(modFrame);
	}
	
	void FilterWin::toBlackoutFrame()
	{
		blackoutFrame->load(p_mod);
		setCurrentWidget(blackoutFrame);
	}

	void FilterWin::addModToModListIfNew()
	{
		if (p_mod == &mod) {
			filter.modList.push_back(mod);
			p_mod = &filter.modList.back();
			filter.modList.sort();
		}
	}

	void FilterWin::updateLoadedFilter() { *p_loadedFilter = filter; }

	void FilterWin::save()
	{
		// Attempt save. If fail, perform 'save as'
		if ( filter.filepath.empty() || filter.save() )
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
		filter.filepath = fileName.toStdString();
		// force file extension (*.cln)
		if ( !fileName.endsWith(QString(MOVIESOAP_FILE_EXT)) )
			filter.filepath += MOVIESOAP_FILE_EXT;
		// save
		filter.save();
	}
}