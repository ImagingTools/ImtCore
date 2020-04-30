#include <imtloggui/CEventGraphicsView.h>


// Qt includes
#include <QtGui/QMouseEvent>
#include <QDebug>
#include <qscrollbar.h>


namespace imtloggui
{


// public methods

CEventGraphicsView::CEventGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
}


void CEventGraphicsView::wheelEvent(QWheelEvent *event)
{
	QPointF scenePnt = mapToScene(event->pos());
	QTransform transform(transform());

	if (event->modifiers() & Qt::ShiftModifier){
		if (event->delta() > 0){
			scale(1, 1.1);
		}
		else{
			scale(1, 1/1.1);
		}
		return;
	}

	if (event->delta() > 0){
		scale(1.1, 1);
	}
	else{
		scale(1 / 1.1, 1);
	}

	qDebug() << horizontalScrollBar()->minimum() << horizontalScrollBar()->maximum();
}


void CEventGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	double delta = event->localPos().x() - m_prevPos.x();

	double sceneDelta = delta / transform().m11();

	QScrollBar *bar = horizontalScrollBar();

	double visibleSceneWidth = mapToScene(rect()).boundingRect().width();
	double scrollFactor = (bar->maximum() - bar->minimum()) / (sceneRect().width() - visibleSceneWidth);

	double aaa = mapToScene(rect()).boundingRect().right();
	bar->setValue(bar->value() - sceneDelta * scrollFactor);
	double bbb = mapToScene(rect()).boundingRect().right();
	//qDebug() << sceneDelta * scrollFactor << int(sceneDelta * scrollFactor);

	m_prevPos = event->localPos();
}


void CEventGraphicsView::mousePressEvent(QMouseEvent *event)
{
	m_prevPos = event->localPos();
}


void CEventGraphicsView::resizeEvent(QResizeEvent *event)
{
}


}


