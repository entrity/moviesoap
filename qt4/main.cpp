#include "main.hpp"
#include <vlc_common.h>
#include <vlc_interface.h>
#include <QMenu>
#include <QMenuBar>

namespace Moviesoap
{
	void init( intf_thread_t * p_intf, MainInterface * mainInterface )
	{
		// todo
	}
}


namespace Moviesoap
{
	Menu::Menu( QWidget * parent ) : QMenu(parent)
	{
		// QAction * actionLoad = new QAction("&Load filter...", this);
		// addAction(actionLoad);
	}

	Menu * Menu::create( QMenuBar * bar )
	{
		Menu * menu = new Menu(bar);
		return menu;
	}

}