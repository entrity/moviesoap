#ifndef MOVIESOAP_MOD_LIST_WIDGET_ITEM_H
#define MOVIESOAP_MOD_LIST_WIDGET_ITEM_H

#include <QListWidgetItem>

namespace Moviesoap
{
	class Mod;

	class ModListWidgetItem : public QListWidgetItem
	{
		// Q_OBJECT
	protected:
		Mod * p_mod;
	public:
		void buildText();
		ModListWidgetItem(QListWidget * parent, Mod * p_mod) : QListWidgetItem(parent), p_mod(p_mod) { buildText(); }
	};

}

#endif