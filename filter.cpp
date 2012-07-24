#include "qt4/main.hpp"
#include "variables.h"
// #include "minizip/zip.h"
// #include "minizip/unzip.h"
#include "filter.hpp"

#include <string>
#include <cstdio>
#include <sstream>
using namespace std;

#define MOVIESOAP_FILTER_BUFFER_MAX 2048

namespace Moviesoap
{
	int Filter::save()
	{
		int ret;
	// 	// zip_fileinfo zfi;

	// 	// string savePath = "test-filter.msf";
	// 	/* Create archive */
	// 	// zipFile zf = zipOpen(savePath.c_str(), APPEND_STATUS_CREATE);
	// 	// if (zf)
	// 	// 	printf("%s\n", "---- created zipfile");
	// 	// else
	// 	// 	return MOVIESOAP_ERROR;
	// 	/* Write meta file in archive */
	// 	// ret = zipOpenNewFileInZip(zf,
	// 	// 	"meta",
	// 	// 	&zfi,
	// 	// 	NULL, 0,
	// 	// 	NULL, 0,
	// 	// 	NULL, Z_DEFLATED, Z_NO_COMPRESSION);
	// 	// zipWriteInFileInZip(zf, "abc", 4);
	// 	// zipCloseFileInZip(zf);
	// 	// /* Write mods file in archive */
	// 	// 	// todo
	// 	// /* Close archive */
	// 	// zipClose(zf, "foo");
		printf("---- saving done\n");
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