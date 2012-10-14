#include <QStackedWidget>
#include <QFrame>

namespace Moviesoap
{
	enum moviesoap_frame_t {
		MOVIESOAP_FILTER_FRAME,
		MOVIESOAP_MOD_FRAME,
		MOVIESOAP_BLACKOUT_FRAME
	};

	class Filter;
	class Mod;
	class FilterFrame;
	class ModFrame;
	class BlackoutFrame;

	class FilterWin : public QStackedWidget
	{
		Q_OBJECT
	protected:
		Filter * p_editingFilter;
		FilterFrame * filterFrame;
		ModFrame * modFrame;
		BlackoutFrame * blackoutFrame;
		static FilterWin *p_window;
	public:
		FilterWin();
		static FilterWin * window() { return p_window; }
		static void openEditor(Filter *);
		static void hideEditor();
		void editFilter(Filter *);
		void editMod(Mod *);
	};
}