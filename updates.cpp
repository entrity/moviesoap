#include "updates.hpp"
#include "qt4/main.hpp"
#include "qt4/menu.hpp"
#include <vlc_common.h>
#include <vlc_network.h>

namespace Moviesoap
{

	/* Returns char* containing first line of body of HTTP response for update check */
	inline char * checkForUpdates(vlc_object_t * p_obj )
	{
		int fd = net_ConnectTCP (p_obj, MOVIESOAP_UPDATES_HOST, 80);
		if (fd != -1 ) {
			ssize_t resp1 = net_Printf( p_obj, fd, NULL,
			  "GET " MOVIESOAP_UPDATES_PATH "?ver=" MOVIESOAP_VERSION " HTTP/1.1\r\nHost: " MOVIESOAP_UPDATES_HOST "\r\n\r\n" 
			  );
			char * resp;
			bool readingHeaders = true;
			while (readingHeaders) {
				resp = net_Gets( p_obj, fd, NULL);
				readingHeaders = strlen(resp) > 2;
				free(resp);
			}
			return net_Gets( p_obj, fd, NULL);
		}
		return NULL;
	}

	/* Sends HTTP request to server for update info. Carries out action based on info. */
	void * handleUpdateCheck( void * p_data )
	{
		vlc_object_t * p_obj = (vlc_object_t *) p_data;
		char * psz_value = checkForUpdates( p_obj );
		if (psz_value) {
			switch(psz_value[0]) {
				case '-': // kill Moviesoap
    				Moviesoap::p_GuiMenu->menuAction()->setVisible(false);
					break;
				case '+': // notify of updates
					Moviesoap::p_GuiMenu->notifyUpdatesAvailable();
					break;
				case '*': // notify of updates & kill Moviesoap
					Moviesoap::p_GuiMenu->notifyUpdatesAvailable();
    				Moviesoap::p_GuiMenu->menuAction()->setVisible(false);
					break;
			}
			free( psz_value );
		}
		return NULL;
	}

}