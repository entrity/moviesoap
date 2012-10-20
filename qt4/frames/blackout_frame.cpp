#include "blackout_frame.hpp"
#include "gui_helpers.hpp"
#include "filter_win.hpp"
#include "../../filter.hpp"
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QGridLayout>

// test
#include <iostream>
using namespace std;

#define MOVIESOAP_DUMP_COORD_TEXT(name) (mod->mod.name = name##Text->text().toInt())
#define MOVIESOAP_LOAD_COORD_TEXT(name) (name##Text->setText(QString::number(mod->mod.name)))

namespace Moviesoap
{
	void BlackoutFrame::okClicked()
	{
		// update p_editingMod
		dump(filterWin->p_mod);
		// show mod pane
		filterWin->toModFrame();
	}

	void BlackoutFrame::load(Mod * mod)
	{
		MOVIESOAP_LOAD_COORD_TEXT(x1);
		MOVIESOAP_LOAD_COORD_TEXT(x2);
		MOVIESOAP_LOAD_COORD_TEXT(y1);
		MOVIESOAP_LOAD_COORD_TEXT(y2);
	}

	void BlackoutFrame::dump(Mod * mod)
	{
		MOVIESOAP_DUMP_COORD_TEXT(x1);
		MOVIESOAP_DUMP_COORD_TEXT(x2);
		MOVIESOAP_DUMP_COORD_TEXT(y1);
		MOVIESOAP_DUMP_COORD_TEXT(y2);
	}
	
	/* Constructor */
	BlackoutFrame::BlackoutFrame(FilterWin *parent) : QFrame(parent), filterWin(parent)
	{
		QVBoxLayout *layout = new QVBoxLayout;
		setLayout(layout);
		// coord div
		QGridLayout *grid = new QGridLayout;
		y1Text = addTextToGrid(grid, 0, 1, "top");
		x1Text = addTextToGrid(grid, 1, 0, "left");
		x2Text = addTextToGrid(grid, 1, 2, "right");
		y2Text = addTextToGrid(grid, 2, 1, "bottom");
		layout->addLayout(grid);
		// button div
		QPushButton *okButton = new QPushButton(QString("&Ok"));
		connect( okButton, SIGNAL(clicked()), this, SLOT(okClicked()) );
		layout->addWidget(okButton);
	}
}

#undef MOVIESOAP_LOAD_COORD_TEXT
#undef MOVIESOAP_DUMP_COORD_TEXT