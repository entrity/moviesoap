#include "main.hpp"
#include "../filter.hpp"
#include <vlc_common.h>
#include <vlc_interface.h>

#include <cstdio>

namespace Moviesoap
{
	vlc_object_t * p_obj = NULL;
	Filter * loadedFilter = NULL;
	struct config_t config;

	void test()
	{
		// save vlc object
		printf("---- ---- IN MOVIEOSAP INIT\n");
		printf("---- %x\n", (unsigned int) p_obj);

		// test dummy filter
		Filter * p_filter = Moviesoap::Filter::dummy();
		// p_filter->save();

		// test config struct
		config.active = true;
		config.tolerances[0] = 168;
		printf("---- config.tolerances[0]: %d\n", config.tolerances[0]);
		// printf("foo & bar: %d %d\n", config.foo(), config.bar());

		// Mod * p_mod = &p_filter->modList.front();
		// p_filter->loadMod(*p_mod);
	}

	// int config_t::bar() {  return 678; }

	void init( intf_thread_t * p_intf, MainInterface * mainInterface )
	{
		// set vars
		p_obj = VLC_OBJECT(p_intf);
		// test
		test();
		// build config
	}

	bool config_t::ignoreMod(Mod & mod) {
		uint8_t tolerance = tolerances[mod.mod.category];
		return tolerance > mod.mod.severity;
	}
}

