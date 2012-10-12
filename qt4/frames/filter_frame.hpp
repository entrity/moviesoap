#ifndef MOVIESOAP_FILTER_FRAME_H
#define MOVIESOAP_FILTER_FRAME_H

#include <QFrame>
#include <QListWidgetItem>
class QLineEdit;
class QListWidget;

namespace Moviesoap {
	class Filter;
	class Mod;

	class FilterFrame : public QFrame
	{
		Q_OBJECT
	protected:
		QLineEdit *titleText, *yearText, *isbnText, *creatorText;
		QListWidget *modListWidget;
		Filter * p_editingFilter;
	public:
		FilterFrame(QWidget * parent);
		void load(Filter *);
		void dump(Filter *);
		void closeWin();
	public slots:
		void saveClicked();
		void saveAsClicked();
		void okClicked();
		void cancelClicked();
		// void newModClicked();
		// void editModClicked();
		// void delModClicked();
		// void modDblClicked();
		// void previewClicked();
	};
}

#endif