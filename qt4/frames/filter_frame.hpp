#ifndef MOVIESOAP_FILTER_FRAME_H
#define MOVIESOAP_FILTER_FRAME_H

#include <QFrame>
#include <QListWidgetItem>
class QLineEdit;
class QListWidget;

#include "../../variables.h"

namespace Moviesoap
{
	class Filter;
	class Mod;
	class FilterWin;
	class ModListWidget;

	class FilterFrame : public QFrame
	{
		Q_OBJECT
	protected:
		QLineEdit
			*titleText, *yearText, *isbnText, *creatorText, *previewOffsetText, *quickCreateOffsetText;
		ModListWidget *modListWidget;
		FilterWin * filterWin;
		Mod * p_quickCreatedMod;
	public:
		FilterFrame(FilterWin * parent);
		void load(Filter * filter=NULL);
		void dump(Filter * filter=NULL);
		/* Refreshes contents of list of mods */
		void refreshModListWidget(Filter *);
		Mod * getSelectedMod();
	public slots:
		void okClicked();
		void okAndSaveClicked();
		void saveClicked();
		void saveAsClicked();
		void newModClicked();
		void editModClicked();
		void delModClicked();
		void previewClicked();
		void quickCreateMod(uint8_t mode);
		void quickCreateSkipMod() { quickCreateMod(MOVIESOAP_SKIP); }
		void quickCreateMuteMod() { quickCreateMod(MOVIESOAP_MUTE); }
		void quickCreateBlackoutMod() { quickCreateMod(MOVIESOAP_BLACKOUT); }
	};
}

#endif