#include "main.hpp"
#include "menu.hpp"

#include <vlc_configuration.h>

#include <QCheckBox>

namespace Moviesoap
{

	/* initialize misc fields extern'ed from main.hpp */
	Mod newMod;
	Mod * p_editingMod = NULL;
	Moviesoap::Menu * p_GuiMenu = NULL;
	QAction * p_actionActive = NULL;
	QCheckBox * p_ultraQuickModCheckbox = NULL;


	const char * toleranceLabels[] = {
		"(unspecified)",
		"Bigotry",
		"Blasphemy",
		"Contention",
		"Disrespect family",
		"Other",
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

	bool isUltraQuickModCreationEnabled()
	{
		return (Moviesoap::p_ultraQuickModCheckbox && Moviesoap::p_ultraQuickModCheckbox->isChecked());
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
	void defaultizeMod(Mod * p_mod)
	{
		if (Moviesoap::p_input) {
			p_mod->mod.start = MoviesoapGetNow(Moviesoap::p_input) / MOVIESOAP_MOD_TIME_FACTOR;
			p_mod->mod.stop = p_mod->mod.start + (100);
			p_mod->mod.title = var_GetInteger(Moviesoap::p_input, "title");
		} else {
			p_mod->mod.start = 0;
			p_mod->mod.stop = 100;
			p_mod->mod.title = MOVIESOAP_UNIVERSAL_TITLE;
		}
		p_mod->mod.mode = MOVIESOAP_SKIP;
		p_mod->mod.category = MOVIESOAP_CAT_NONE;
		p_mod->mod.severity = MOVIESOAP_TOLERANCE_COUNT;
		p_mod->description = "";
	}
}