#ifndef MOVIESOAP_MAIN_H
#define MOVIESOAP_MAIN_H
#define MOVIESOAP_VERSION 0.3.0

#include <vlc_common.h>
#include <QMenu>

class MainInterface;
class QWidget;

enum {
	MOVIESOAP_SUCCESS,
	MOVIESOAP_ENOMEM,
	MOVIESOAP_FORM_INVALID,
	MOVIESOAP_ENOFPATH,
	MOVIESOAP_CANCEL
}

namespace Moviesoap
{
	/* Functions */
	void init( intf_thread_t * p_intf, MainInterface * mainInterface );
	string defaultSaveDir();
	/* Fields */
	extern MainInterface * mainInterface;
	extern vlc_object_t * p_obj;

	class Menu
	{
		Q_OBJECT
	public:
		Menu( QWidget * parent ) : QMenu(parent) {}
		static Menu * createMenu( QWidget * parent );
	public slots:
		void loadFilter();
		void editFilter();
		void newFilter();
	};
}