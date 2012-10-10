#include <QStackedWidget>
#include <QFrame>

namespace Moviesoap
{
	class Filter;
	class FilterFrame;

	class FilterWin : QStackedWidget
	{
		Q_OBJECT
	protected:
		static FilterWin *p_window;
		Filter * p_editingFilter;
		FilterFrame *filterFrame;
	public:
		FilterWin();
		static void openEditor(Filter *);
		void load(Filter *);
	};
}