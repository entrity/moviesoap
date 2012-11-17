#include "conclude_preview_button.hpp"
#include "filter_win.hpp"

namespace Moviesoap
{
	ConcludePreviewButton::ConcludePreviewButton(QString text, FilterWin *p_filterWin) : QPushButton(text)
	{
		connect(this, SIGNAL(close()), p_filterWin, SLOT(concludePreview()));
	}

}