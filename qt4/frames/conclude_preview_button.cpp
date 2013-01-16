#include "conclude_preview_button.hpp"
#include "filter_win.hpp"
#include <QCloseEvent>

namespace Moviesoap
{
	ConcludePreviewButton::ConcludePreviewButton(QString text, FilterWin *p_filterWin) : QPushButton(text)
	{
		connect(this, SIGNAL(clicked()), this, SLOT(closeClicked()));
	}

	void ConcludePreviewButton::closeEvent(QCloseEvent *event)
	{
		emit( closeSignal() );
		event->accept();
	}

	void ConcludePreviewButton::closeClicked()
	{
		close();
	}

	// bool ConcludePreviewButton::close()
	// {
	// 	emit( closeSignal() );
	// 	return QPushButton::close();
	// }
}