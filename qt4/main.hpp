#ifndef MOVIESOAP_MAIN_H
#define MOVIESOAP_MAIN_H
#define MOVIESOAP_VERSION 0.3.0

#include <vlc_common.h>
#include <vlc_interface.h>
#include <QMenu>

class MainInterface;
class QWidget;
class QMenuBar;

enum {
	MOVIESOAP_SUCCESS,
	MOVIESOAP_ENOMEM,
	MOVIESOAP_FORM_INVALID,
	MOVIESOAP_ENOFPATH,
	MOVIESOAP_CANCEL
};

namespace Moviesoap
{
	/* Functions */
	extern void init( intf_thread_t * p_intf, MainInterface * mainInterface );
	// string defaultSaveDir();
	// /* Fields */
	// extern MainInterface * mainInterface;
	// extern vlc_object_t * p_obj;
	// bool isValidSavePath() { return true; } // todo

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