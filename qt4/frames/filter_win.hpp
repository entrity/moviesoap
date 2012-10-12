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
		Filter * p_editingFilter;
		FilterFrame *filterFrame;
	public:
		FilterWin();
		static FilterWin *p_window;
		static void openEditor(Filter *);
		static void hideEditor();
		void load(Filter *);
	};
}