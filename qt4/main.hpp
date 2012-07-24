#ifndef MOVIESOAP_MAIN_H
#define MOVIESOAP_MAIN_H
#define MOVIESOAP_VERSION 0.3.0

#include <vlc_common.h>
#include <vlc_interface.h>
#include <vlc_configuration.h>
#include <QMenu>

class MainInterface;
class QWidget;
class QMenuBar;

enum {
	MOVIESOAP_SUCCESS,
	MOVIESOAP_ERROR,
	MOVIESOAP_ENOMEM,
	MOVIESOAP_FORM_INVALID,
	MOVIESOAP_ENOFPATH,
	MOVIESOAP_CANCEL
};

namespace Moviesoap
{
	/* Fields */
	
	
	extern vlc_object_t * p_obj;

	// extern MainInterface * mainInterface;
	// bool isValidSavePath() { return true; } // todo

	/* Functions */

	extern void init( intf_thread_t * p_intf, MainInterface * mainInterface );
	/* Returns vlc's data dir. */
	// static const char * datadir() { return Moviesoap::p_obj ? config_GetDataDir(Moviesoap::p_obj) : NULL; }
	// /* Returns vlc's config dir. */
	// static const char * confdir() { return config_GetConfDir(); }


	class Menu : public QMenu
	{
		Q_OBJECT
	public:
		Menu( QWidget * parent );
		static Menu * create(QMenuBar * bar);
	// public slots:
		// void loadFilter();
		// void editFilter();
		// void newFilter();
	};
}

#endif