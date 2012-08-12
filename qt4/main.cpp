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
<<<<<<< HEAD
	Filter * loadedFilter = NULL;
=======
>>>>>>> 89a430f389d595358d1859b4f1dd357820c7c998

	void init( intf_thread_t * p_intf, MainInterface * mainInterface )
	{
		// save vlc object
		p_obj = VLC_OBJECT(p_intf);
		printf("---- ---- IN MOVIEOSAP INIT\n");
<<<<<<< HEAD
		printf("---- %x\n", (unsigned int) p_obj);
=======
		printf("---- %x\n", p_obj);
>>>>>>> 89a430f389d595358d1859b4f1dd357820c7c998
		
		// test dummy filter
		Filter * p_filter = Moviesoap::Filter::dummy();
		p_filter->save();
	}
}


namespace Moviesoap
{
	Menu::Menu( QWidget * parent ) : QMenu(parent)
	{
<<<<<<< HEAD
		QAction * actionLoad = new QAction("&Load filter...", this);
		addAction(actionLoad);
		QAction * actionNew = new QAction("&New filter...", this);
		addAction(actionNew);
		QAction * actionEdit = new QAction("&Edit filter...", this);
		addAction(actionEdit);
		// connect(actionEdit, edit(), );
=======
		// QAction * actionLoad = new QAction("&Load filter...", this);
		// addAction(actionLoad);
>>>>>>> 89a430f389d595358d1859b4f1dd357820c7c998
	}

	Menu * Menu::create( QMenuBar * bar )
	{
		Menu * menu = new Menu(bar);
		return menu;
	}

}