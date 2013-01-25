#ifndef MOVIESOAP_FILTER_WIN_H
#define MOVIESOAP_FILTER_WIN_H

#include "../../filter.hpp"
#include <QStackedWidget>
#include <QFrame>
#include <QFileDialog>
#include <QMessageBox>

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
	class ConcludePreviewButton;

	class FilterWin : public QStackedWidget
	{
		Q_OBJECT
	protected:
		static FilterWin *p_window;
		FilterFrame * filterFrame;
		ModFrame * modFrame;
		BlackoutFrame * blackoutFrame;
		ConcludePreviewButton * concludePreviewButton;
	public:
		Filter blankFilter; // used to copy null fields to this->filter when new filter is wanted
		/* Constructor */
		FilterWin();
		/* Destructor */
		~FilterWin() { free(concludePreviewButton); }
		// static FilterWin * window() { return p_window; } // todo del
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
		/* updates Moviesoap::loadedFilter */
		void updateLoadedFilter();
		/* call loadedFilter->save() */
		void save();
		/* Get filepath from user. Then call loadedFilter->save() */
		void saveAs();
		/* Swap this->filter for Moviesoap::filter, set play time, play main playing window, hide this, show concludePreviewButton */
		void preview(Mod * p_mod);
	public slots:
		/* Swap back this->filter for Moviesoap::filter, pause play, hide concludePreviewButton, show this */
		void concludePreview();
	};

	void loadFilterDialogue(QWidget * parent);
}

#endif