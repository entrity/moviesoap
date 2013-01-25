#ifndef MOVIESOAP_MOD_LIST_WIDGET_ITEM_H
#define MOVIESOAP_MOD_LIST_WIDGET_ITEM_H

#include <QListWidget>
#include <QListWidgetItem>

class QKeyEvent;

namespace Moviesoap
{
	class Mod;
	class FilterFrame;

	class ModListWidgetItem : public QListWidgetItem
	{
		// Q_OBJECT
	protected:
		Mod * p_mod;
	public:
		void buildText();
		ModListWidgetItem(QListWidget * parent, Mod * p_mod) : QListWidgetItem(parent), p_mod(p_mod) { buildText(); }
	};

	class ModListWidget : public QListWidget
	{
		Q_OBJECT
	protected:
		FilterFrame * parent;
	public:
		ModListWidget(FilterFrame * parent) : parent(parent) {}
		void keyReleaseEvent(QKeyEvent *);
	};

}

#endif