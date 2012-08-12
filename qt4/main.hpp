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
<<<<<<< HEAD

	extern vlc_object_t * p_obj;
	extern Filter * loadedFilter;
=======
	
	
	extern vlc_object_t * p_obj;

>>>>>>> 89a430f389d595358d1859b4f1dd357820c7c998
	// extern MainInterface * mainInterface;
	// bool isValidSavePath() { return true; } // todo

	/* Functions */

	extern void init( intf_thread_t * p_intf, MainInterface * mainInterface );
	/* Returns vlc's data dir. */
<<<<<<< HEAD
	static const char * datadir() { return Moviesoap::p_obj ? config_GetDataDir(Moviesoap::p_obj) : NULL; }
	/* Returns vlc's config dir. */
	static const char * confdir() { return config_GetConfDir(); }
=======
	// static const char * datadir() { return Moviesoap::p_obj ? config_GetDataDir(Moviesoap::p_obj) : NULL; }
	// /* Returns vlc's config dir. */
	// static const char * confdir() { return config_GetConfDir(); }
>>>>>>> 89a430f389d595358d1859b4f1dd357820c7c998


	class Menu : public QMenu
	{
		Q_OBJECT
	public:
		Menu( QWidget * parent );
		static Menu * create(QMenuBar * bar);
	// public slots:
<<<<<<< HEAD
	// 	void loadFilter();
	// 	void editFilter();
	// 	void newFilter();
=======
		// void loadFilter();
		// void editFilter();
		// void newFilter();
>>>>>>> 89a430f389d595358d1859b4f1dd357820c7c998
	};
}

#endif