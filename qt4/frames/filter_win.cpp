#include "filter_win.hpp"
#include "filter_frame.hpp"
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
	}

	void FilterWin::openEditor(Filter * filterToEdit)
	{
		// create filter if necessary
		if (p_window == NULL)
			p_window = new FilterWin;
		p_window->load(filterToEdit);
		// show window
		p_window->show();
	}

	void FilterWin::hideEditor() { 
		// cout << "p_window ? "
			// << hex << p_window
			// << endl;
		if (p_window) p_window->close(); 
	}

	void FilterWin::load(Filter * filterToEdit)
	{
		// delete existing editingFilter
		if (p_editingFilter)
			delete p_editingFilter;
		// set current editingFilter
		p_editingFilter = filterToEdit ? new Filter(filterToEdit) : new Filter;
		// pass filter to pane(s)
		filterFrame->load(p_editingFilter);
	}
}