#include <QPushButton>

class QCloseEvent;

namespace Moviesoap
{
	class FilterWin;

	class  ConcludePreviewButton : public QPushButton {
		Q_OBJECT
		void closeEvent(QCloseEvent *event);
	public:
		ConcludePreviewButton(QString text, FilterWin *p_filterWin);
	public slots:
		void closeClicked();
	signals:
		void closeSignal();
	};
}