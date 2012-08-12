#include "main.hpp"
#include "../filter.hpp"
#include <vlc_common.h>
#include <vlc_interface.h>
#include <QMenu>
#include <QMenuBar>

#include <cstdio>

namespace Moviesoap
{
	vlc_object_t * p_obj = NULL;
	Filter * loadedFilter = NULL;

	void init( intf_thread_t * p_intf, MainInterface * mainInterface )
	{
		// save vlc object
		p_obj = VLC_OBJECT(p_intf);
		printf("---- ---- IN MOVIEOSAP INIT\n");
		printf("---- %x\n", (unsigned int) p_obj);
		
		// test dummy filter
		Filter * p_filter = Moviesoap::Filter::dummy();
		p_filter->save();
	}
}


namespace Moviesoap
{
	Menu::Menu( QWidget * parent ) : QMenu(parent)
	{
		QAction * actionLoad = new QAction("&Load filter...", this);
		addAction(actionLoad);
		QAction * actionNew = new QAction("&New filter...", this);
		addAction(actionNew);
		QAction * actionEdit = new QAction("&Edit filter...", this);
		addAction(actionEdit);
		// connect(actionEdit, edit(), );
	}

	Menu * Menu::create( QMenuBar * bar )
	{
		Menu * menu = new Menu(bar);
		return menu;
	}

}