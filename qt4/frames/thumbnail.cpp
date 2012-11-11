#include "thumbnail.hpp"
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>

#ifndef MIN
#define MIN( a, b ) ( a < b ? a : b )
#endif
#ifndef MAX
#define MAX( a, b ) ( a > b ? a : b )
#endif

//test
#include <iostream>
using namespace std;

namespace Moviesoap
{
	void Thumbnail::load(int x1, int y1, int x2, int y2)
	{
		this->x1 = x1 * sizeFactor;
		this->x2 = x2 * sizeFactor;
		this->y1 = y1 * sizeFactor;
		this->y2 = y2 * sizeFactor;
		QLabel::update();
	}

	void Thumbnail::setPixmap(const QPixmap & pixmap)
	{
		// Determine whether to scale by width or height
		bool b_scaleByWidth;
		float xRatio, yRatio;
		xRatio = pixmap.width() / MOVIESOAP_THUMBNAIL_MAX_WIDTH;
		yRatio = pixmap.height() / MOVIESOAP_THUMBNAIL_MAX_HEIGHT;
		if (xRatio > yRatio)
			b_scaleByWidth = xRatio > 1;
		else
			b_scaleByWidth = yRatio > 1;
		// Set sizeFactor & create new Pixmap
		QPixmap scaledPixmap;
		if (b_scaleByWidth) {
			sizeFactor = xRatio;
			scaledPixmap = pixmap.scaledToWidth( pixmap.width() / sizeFactor );
		} else {
			sizeFactor = yRatio;
			scaledPixmap = pixmap.scaledToHeight( pixmap.height() / sizeFactor );
		}
		// Call parent's method
		QLabel::setPixmap(scaledPixmap);
	}

	void Thumbnail::mousePressEvent(QMouseEvent *evt)
	{
		if (evt->button() != Qt::LeftButton)
			return;
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
		x2 = MIN( pos.x(), pixmap()->width() );
		y2 = MIN( pos.y(), pixmap()->height() );
		QLabel::update();
		emit blackoutChanged(x1*sizeFactor, y1*sizeFactor, x2*sizeFactor, y2*sizeFactor);
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