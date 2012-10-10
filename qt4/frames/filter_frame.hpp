#ifndef MOVIESOAP_FILTER_FRAME_H
#define MOVIESOAP_FILTER_FRAME_H

#include <QFrame>
class QLineEdit;
class QListWidget;

namespace Moviesoap {
	class Filter;

	class FilterFrame : public QFrame
	{
		Q_OBJECT
	protected:
		QLineEdit *titleText, *yearText, *isbnText, *creatorText;
		QListWidget *modListWidget;
	public:
		FilterFrame(QWidget * parent);
		void load(Filter *);
	// 	void dump(Filter *);
	public slots:
		// void saveClicked();
		// void saveAsClicked();
		// void okClicked() { parentWidget()->hide(); }
		// void newModClicked();
		// void editModClicked();
		// void previewClicked();
	};
}

#endif