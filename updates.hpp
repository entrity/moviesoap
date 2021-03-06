#ifndef MOVIESOAP_UPDATES_H
#define MOVIESOAP_UPDATES_H

#define MOVIESOAP_UPDATES_HOST "moviesoap.org"
#define MOVIESOAP_UPDATES_PATH "/update_check"

namespace Moviesoap
{
	/* Sends HTTP request to server for update info. Carries out action based on info. */
	void * handleUpdateCheck( void * p_data );
}

#endif