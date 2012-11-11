#ifndef MOVIESOAP_BLACKOUT_THUMBNAIL_H
#define MOVIESOAP_BLACKOUT_THUMBNAIL_H

#define MOVIESOAP_THUMBNAIL_MAX_WIDTH 640.0
#define MOVIESOAP_THUMBNAIL_MAX_HEIGHT 480.0

#include <QLabel>

class QMouseEvent;
class QPaintEvent;

namespace Moviesoap
{
	class Thumbnail : public QLabel
	{
		Q_OBJECT
	protected:
		int x1, y1, x2, y2;
		float sizeFactor; // the ratio between these coords, which are matched against a pixmap and the coords for the actual screenshot
		bool mouseLeftDown;
		void mouseReleaseEvent(QMouseEvent *);
		void mousePressEvent(QMouseEvent *);
		void mouseMoveEvent(QMouseEvent *);
		void paintEvent(QPaintEvent *);
		inline void update(QMouseEvent *);
	public:
		Thumbnail(QWidget * parent = 0, Qt::WindowFlags f = 0) :
			QLabel(parent, f), mouseLeftDown(false), sizeFactor(1) {}
		Thumbnail(const QString & text, QWidget * parent = 0, Qt::WindowFlags f = 0) :
			QLabel(text, parent, f), mouseLeftDown(false), sizeFactor(1) {}
		int getX1() { return x1; }
		int getY1() { return y1; }
		int getX2() { return x2; }
		int getY2() { return y2; }
		void setPixmap(const QPixmap &);
	public slots:
		void load(int x1, int y1, int x2, int y2);
	signals:
		void blackoutChanged(int x1, int y1, int x2, int y2);
	};
}

#endif