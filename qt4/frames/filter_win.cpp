#include "filter_win.hpp"
#include "filter_frame.hpp"
#include "mod_frame.hpp"
#include "blackout_frame.hpp"
#include "../../filter.hpp"

// test
#include <iostream>
using namespace std;

namespace Moviesoap
{
	/* Init vars */
	FilterWin * FilterWin::p_window = NULL;

	FilterWin::FilterWin() : QStackedWidget(NULL), p_editingFilter(NULL)
	{
		setWindowTitle(QString("Moviesoap Filter Editor"));
		filterFrame = new FilterFrame(this);
		addWidget(filterFrame);
		modFrame = new ModFrame(this);
		addWidget(modFrame);
		blackoutFrame = new BlackoutFrame(this);
		addWidget(blackoutFrame);
	}

	void FilterWin::openEditor(Filter * filterToEdit)
	{
		// create filter if necessary
		if (p_window == NULL)
			p_window = new FilterWin;
		p_window->editFilter(filterToEdit);
		// show window
		p_window->show();
		p_window->raise();
	}

	/* Free editingFilter. Hide window */
	void FilterWin::hideEditor() {
		if (p_window) {
			if (p_window->p_editingFilter) {
				delete p_window->p_editingFilter;
				p_window->p_editingFilter = NULL;
			}
			p_window->hide();
		}
	}

	void FilterWin::editFilter(Filter * filterToEdit)
	{
		// delete existing editingFilter
		if (p_editingFilter)
			delete p_editingFilter;
		// set current editingFilter
		p_editingFilter = filterToEdit ? new Filter(filterToEdit) : new Filter;
		// pass filter to pane(s)
		filterFrame->load(p_editingFilter);
		// set visible pane
		setCurrentWidget(filterFrame);
	}

	void FilterWin::editMod(Mod * mod)
	{
		// pass mod to pane
		modFrame->load(mod);
		// set visible pane
		setCurrentWidget(modFrame);
	}

}