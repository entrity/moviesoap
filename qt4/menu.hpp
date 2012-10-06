#ifndef MOVIESOAP_MENU_H
#define MOVIESOAP_MENU_H

#include "preferences.hpp"

#include <QMenu>
class QWidget;
class QMenuBar;

namespace Moviesoap
{
	class Menu : public QMenu
	{
		Q_OBJECT
	public:
		static Menu * create(QMenuBar * bar);
		Menu( QWidget * parent );
	public slots:
		void editPreferences() { Moviesoap::PreferencesWin::openEditor(); }
	};
}

#endif