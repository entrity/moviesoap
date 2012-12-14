#ifndef MOVIESOAP_MENU_H
#define MOVIESOAP_MENU_H

#include <QMenu>
class QWidget;
class QMenuBar;
class QAction;

namespace Moviesoap
{
	class Filter;

	class Menu : public QMenu
	{
		Q_OBJECT
		QAction * actionActive;
	public:
		static Menu * create(QMenuBar * bar);
		Menu( QWidget * parent );
		bool isActiveSelected() { return actionActive->isChecked(); }
	public slots:
		void activeTriggered();
		void editPreferences();
		void editFilter();
		void newFilter();
		void loadFilter();
	};
}

#endif