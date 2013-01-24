#include "main.hpp"
#include "menu.hpp"

#include <vlc_configuration.h>

namespace Moviesoap
{

	/* initialize misc fields extern'ed from main.hpp */
	Moviesoap::Menu * p_GuiMenu = NULL;


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
}