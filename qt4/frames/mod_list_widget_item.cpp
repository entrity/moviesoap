#include "mod_list_widget_item.hpp"
#include "../../filter.hpp"
#include "gui_helpers.hpp"
#include "filter_frame.hpp"

#include <QKeyEvent>

#include <string>
#include <iostream>
#include <sstream>
using namespace std;

namespace Moviesoap
{

	void ModListWidgetItem::buildText()
	{
		// build stringstream
		stringstream ss;
		// add start time
		char psz_time[12];
		strftime(p_mod->mod.start, psz_time);
		ss << psz_time << " ";
		// add mode string
		ss << '(';
		switch (p_mod->mod.mode)
		{
			case MOVIESOAP_SKIP:
				ss << "SKIP"; break;
			case MOVIESOAP_MUTE:
				ss << "MUTE"; break;
			case MOVIESOAP_BLACKOUT:
				ss << "BLACK"; break;
		}
		ss << ')';
		// add description
		ss << ' ';
		if (p_mod->description.length() <= 30)
			ss << p_mod->description;
		else
			ss << p_mod->description.substr(0,30);
		// set text
		setText( QString(ss.str().c_str()) );
	}

	void ModListWidget::keyReleaseEvent(QKeyEvent * p_evt)
	{
		if (p_evt->key() == Qt::Key_Delete) {
			parent->delModClicked();
		}
	}

}
