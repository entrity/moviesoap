#ifndef MOVIESOAP_MENU_H
#define MOVIESOAP_MENU_H

#include <QMenu>
class QWidget;
class QMenuBar;
class QAction;

namespace Moviesoap
{
	class Menu : public QMenu
	{
		Q_OBJECT
		QAction * actionActive;
	public:
		static Menu * create(QMenuBar * bar);
		Menu( QWidget * parent );
	public slots:
		void editPreferences();
		void activeTriggered();
	};
}

#endif