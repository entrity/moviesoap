#ifndef MOVIESOAP_FILTER_FRAME_H
#define MOVIESOAP_FILTER_FRAME_H

#include <QFrame>
class QLineEdit;
class QListWidget;

namespace Moviesoap {
	class FilterWin : public QFrame
	{
		Q_OBJECT
	protected:
		static FilterWin * p_window;
		QLineEdit *titleText, *yearText, *isbnText, *creatorText;
		QListWidget *modList;
	public:
		FilterWin();
		static void openEditor();
	public slots:
	};
}

#endif