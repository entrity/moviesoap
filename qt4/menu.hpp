#ifndef MOVIESOAP_MENU_H
#define MOVIESOAP_MENU_H

#include <QMenu>
class QWidget;
class QMenuBar;

namespace Moviesoap
{
	class Menu : public QMenu
	{
		Q_OBJECT
	public:

		Menu( QWidget * parent );
		
		static Menu * create(QMenuBar * bar);

		

	};
}

#endif