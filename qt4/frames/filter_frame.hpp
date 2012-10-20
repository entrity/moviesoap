#ifndef MOVIESOAP_FILTER_FRAME_H
#define MOVIESOAP_FILTER_FRAME_H

#include <QFrame>
#include <QListWidgetItem>
class QLineEdit;
class QListWidget;

namespace Moviesoap
{
	class Filter;
	class Mod;
	class FilterWin;

	class FilterFrame : public QFrame
	{
		Q_OBJECT
	protected:
		QLineEdit
			*titleText, *yearText, *isbnText, *creatorText;
		QListWidget *modListWidget;
		FilterWin * filterWin;
	public:
		FilterFrame(FilterWin * parent);
		void load(Filter * filter=NULL);
		void dump(Filter * filter=NULL);
		/* Refreshes contents of list of mods */
		void refreshModListWidget(Filter *);
	public slots:
		void saveClicked();
		void saveAsClicked();
		void okClicked();
		void cancelClicked();
		void newModClicked();
		void editModClicked();
		// void delModClicked();
		// void modDblClicked();
		// void previewClicked();
	};
}

#endif