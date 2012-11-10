#include "thumbnail.hpp"
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>

//test
#include <iostream>
using namespace std;

namespace Moviesoap
{
	void Thumbnail::mousePressEvent(QMouseEvent *evt)
	{
		if (evt->button() != Qt::LeftButton)
			return;
		cout << "mouse down" << endl;
		mouseLeftDown = true;
		QPoint pos = evt->pos();
		x1 = pos.x();
		y1 = pos.y();
		update(evt);
	}

	void Thumbnail::mouseReleaseEvent(QMouseEvent *evt)
	{
		if (!mouseLeftDown)
			return;
		cout << "mouse up" << endl;
		update(evt);
	}
	
	void Thumbnail::mouseMoveEvent(QMouseEvent *evt)
	{
		if (!mouseLeftDown)
			return;
		update(evt);
		if (evt->button() == Qt::LeftButton)
			mouseLeftDown = false;
	}
	
	/* Set x2, y2. Require paintEvent. Emit signal. */
	void Thumbnail::update(QMouseEvent *evt)
	{
		QPoint pos = evt->pos();
		x2 = pos.x();
		y2 = pos.y();
		QLabel::update();
		emit blackoutChanged(x1, y1, x2, y2);
	}
	
	/* Paint widget */
	void Thumbnail::paintEvent(QPaintEvent *evt)
	{
		QPainter painter(this);
		// draw screenshot
		painter.drawPixmap(0, 0, *pixmap());
		// draw blackout rect
		painter.fillRect(x1, y1, x2-x1, y2-y1, Qt::black);
	}

}