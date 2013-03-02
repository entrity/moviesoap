#include "main.hpp"
#include "menu.hpp"
#include "frames/filter_win.hpp"

#include <vlc_configuration.h>

#include <QCheckBox>
#include <QLineEdit>

namespace Moviesoap
{

	/* initialize misc fields extern'ed from main.hpp */
	Mod newMod;
	Mod * p_editingMod = NULL;
	Mod * p_quickCreatedMod = NULL;
	Moviesoap::Menu * p_GuiMenu = NULL;
	QAction * p_actionActive = NULL;
	QCheckBox * p_ultraQuickModCheckbox = NULL;
	QLineEdit * p_quickCreateOffsetText = NULL;
	FilterWin *p_window = NULL;

	const char * toleranceLabels[] = {
		"(unspecified)",
		"Bigotry",
		"Blasphemy",
		"Disrespect",
		"Nudity",
		"Profanity",
		"Sexuality",
		"Violence",
		"Vulgarity"
	};

	/* Return file path without filename */
	string extractDir (const string& str)
	{
	  size_t slash_i = str.find_last_of("/\\");
	  size_t period_i = str.find_last_of(".");
	  if (period_i == string::npos || period_i < slash_i)
	  	return str;
	  else
	  	return str.substr(0, slash_i);
	}

	/* Return path to default save dir, drawn either from the loadedFilter or the config */
	string saveDir()
	{
		if (p_loadedFilter && !p_loadedFilter->filepath.empty()) {
			return extractDir(p_loadedFilter->filepath);
		} else {
			char * userDir = config_GetUserDir( VLC_HOME_DIR );
			string dir(userDir);
			free(userDir);
			return dir;
		}
	}

	bool isActiveSelected()
	{
		return p_GuiMenu ? p_GuiMenu->isActiveSelected() : false;
	}

	/* Push Moviesoap::newMod onto p_loadedFilter->modList if newMod is the mod currently being edited.
	Sets p_editingMod to the new mod in modList.
	If p_editingMod points to a different mod, it indicates that it's not the 'new mod', i.e. a mod that isn't yet in modList. */
	Mod * addEditingModToModListIfNew()
	{
		if (Moviesoap::p_loadedFilter && Moviesoap::p_editingMod == &Moviesoap::newMod) {
			Moviesoap::p_loadedFilter->modList.push_back( Moviesoap::newMod );
			Moviesoap::p_editingMod = &Moviesoap::p_loadedFilter->modList.back();
			Moviesoap::p_loadedFilter->modList.sort();
			return Moviesoap::p_editingMod;
		}
		return NULL;
	}

	/* Set intial values for new mod. */
	void defaultizeMod(Mod * p_mod, uint32_t start_offset_backward, uint32_t stop_offset_forward_from_start)
	{
		if (Moviesoap::p_input) {
			p_mod->mod.start = MoviesoapGetNow(Moviesoap::p_input) / MOVIESOAP_MOD_TIME_FACTOR;
			if (p_mod->mod.start < start_offset_backward)
				p_mod->mod.start = 0;
			else
				p_mod->mod.start -= start_offset_backward;
			p_mod->mod.stop = p_mod->mod.start + stop_offset_forward_from_start;
			p_mod->mod.title = var_GetInteger(Moviesoap::p_input, "title");
		} else {
			p_mod->mod.start = start_offset_backward;
			p_mod->mod.stop = stop_offset_forward_from_start;
			p_mod->mod.title = MOVIESOAP_UNIVERSAL_TITLE;
		}
		p_mod->mod.mode = MOVIESOAP_SKIP;
		p_mod->mod.category = MOVIESOAP_CAT_NONE;
		p_mod->mod.severity = MOVIESOAP_TOLERANCE_COUNT;
		p_mod->description = "";
	}

	/* Creates a new mod and adds it to modList */
	void quickCreateMod(uint8_t mode)
	{
		if (Moviesoap::p_input && Moviesoap::p_window) {
			// create new mod, add to modList
			defaultizeMod(&newMod);
			p_loadedFilter->modList.push_back( newMod );
			p_quickCreatedMod = &p_loadedFilter->modList.back();
			// set mode
			p_quickCreatedMod->mod.mode = mode;
			// set start & stop
			uint32_t offset = p_window->quickModCreationOffset();
			uint32_t now_cs = MoviesoapGetNow(Moviesoap::p_input) / MOVIESOAP_MOD_TIME_FACTOR;
			uint32_t start_cs = (offset > now_cs) ? 0 : now_cs - offset;
			uint32_t stop_cs = start_cs + MOVIESOAP_QUICK_MOD_CREATION_STOP_OFFSET_DEFAULT;
			p_quickCreatedMod->mod.start = start_cs;
			p_quickCreatedMod->mod.stop = stop_cs;
			// refresh modListWidget (also sorts modList)
			p_window->refreshModListWidget();
		}
	}

}