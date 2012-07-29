#include "qt4/main.hpp"
#include "variables.h"
#include "filter.hpp"

#include "tar/ustar.h"

#include <string>
#include <cstdio>
#include <sstream>
using namespace std;


#define MOVIESOAP_FILTER_BUFFER_MAX 2048

namespace Moviesoap
{
	int Filter::save()
	{
		char header[USTAR_HEADER_SIZE];
		bool b = ustar_make_header("blorp.txt", USTAR_REGULAR, 0, header);

		
		printf("---- in filter save: %d\n", b);
		return MOVIESOAP_SUCCESS;
	}
	
	/* Create a filter with a few test mods for testing */
	Filter * Filter::dummy()
	{
		Filter * p_filter = new Filter();
		// p_filter->modList.push_back( Mod( MOVIESOAP_SKIP, 1, 10 ) );
		return p_filter;
	}
}