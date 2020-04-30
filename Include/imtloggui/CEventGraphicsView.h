#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtWidgets/QGraphicsView>


namespace imtloggui
{


class CEventGraphicsView: public QGraphicsView
{
public:
	CEventGraphicsView(QWidget *parent = nullptr);

protected:
	QRect m_lockedRect;
	QPointF m_prevPos;

	// reimplemented (QGraphicsView)
	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
};


} // namespace imtloggui


