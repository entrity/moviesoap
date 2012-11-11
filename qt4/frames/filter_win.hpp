#include "../../filter.hpp"
#include <QStackedWidget>
#include <QFrame>

namespace Moviesoap
{
	enum moviesoap_frame_t {
		MOVIESOAP_FILTER_FRAME,
		MOVIESOAP_MOD_FRAME,
		MOVIESOAP_BLACKOUT_FRAME
	};

	class FilterFrame;
	class ModFrame;
	class BlackoutFrame;

	class FilterWin : public QStackedWidget
	{
		Q_OBJECT
	protected:
		static FilterWin *p_window;
		FilterFrame * filterFrame;
		ModFrame * modFrame;
		BlackoutFrame * blackoutFrame;
	public:
		Filter filter;
		Filter blankFilter; // used to copy null fields to this->filter when new filter is wanted
		Mod mod; // extra mod for editing in gui if user is adding new mod
		Mod blankMod; // used to copy null fields to this->mod when new mod is wanted
		Mod * p_mod; // pointer to mod currently being edited. Must point to either this->mod or a mod in filter.modList
		/* Constructor */
		FilterWin();
		static FilterWin * window() { return p_window; } // todo del
		/* 'Open' this window */
		static void openEditor(Filter *);
		/* 'Close' this window */
		static void hideEditor();
		/* Return p_editingFilter. Appends a new mod to filter.modList() if NULL. */
		Mod * getOrCreateEditingMod();
		/* Show filterFrame */
		void toFilterFrame();
		/* Show modFrame */
		void toModFrame();
		/* Show blackoutFrame */
		void toBlackoutFrame();
		/* Load and open filter-editing pane */
		void editFilter(Filter *);
		/* Load and open mod-editing pane. To edit new Mod, supply no arg */
		void editMod(Mod * mod=NULL);
		/* Load and open mod-editing pane */
		void editMod(int i);
		/* Delete mod from filter */
		void deleteMod(int i);
		/* Refreshes contents of list of mods */
		void refreshModListWidget();
		/* If p_mod points to a new mod, add it to filter.modList and sort list */
		void addModToModListIfNew();
		/* updates Moviesoap::loadedFilter */
		void updateLoadedFilter();
		/* call loadedFilter->save() */
		void save();
		/* Get filepath from user. Then call loadedFilter->save() */
		void saveAs();
	};
}