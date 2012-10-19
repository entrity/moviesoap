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
		FilterFrame * filterFrame;
		ModFrame * modFrame;
		BlackoutFrame * blackoutFrame;
		static FilterWin *p_window;
	public:
		Filter * p_editingFilter;
		/* Constructor */
		FilterWin();
		static FilterWin * window() { return p_window; }
		/* 'Open' this window */
		static void openEditor(Filter *);
		/* 'Close' this window */
		static void hideEditor();
		/* Load and open filter-editing pane */
		void editFilter(Filter *);
		/* Load and open mod-editing pane */
		void editMod(Mod *);
		/* Refreshes contents of list of mods */
		void refreshModListWidget();
	};
}