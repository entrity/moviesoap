#include <QPushButton>

namespace Moviesoap
{
	class FilterWin;

	class  ConcludePreviewButton : public QPushButton {
		Q_OBJECT
	public:
		ConcludePreviewButton(QString text, FilterWin *p_filterWin);
	};
}