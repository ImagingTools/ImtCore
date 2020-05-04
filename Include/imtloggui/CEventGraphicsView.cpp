#include <imtloggui/CEventGraphicsView.h>


// Qt includes
#include <QtGui/QMouseEvent>
#include <QDebug>
#include <qscrollbar.h>


namespace imtloggui
{


// public methods

CEventGraphicsView::CEventGraphicsView(QWidget* parent)
	: QGraphicsView(parent),
	m_timeAxisPtr(nullptr)
{
}


void CEventGraphicsView::setTimeAxis(CTimeAxis* timeAxisPtr)
{
	m_timeAxisPtr = timeAxisPtr;
}


// protected methods

void CEventGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	BaseClass::mouseMoveEvent(event);
}


void CEventGraphicsView::mousePressEvent(QMouseEvent* event)
{
	BaseClass::mousePressEvent(event);
}


void CEventGraphicsView::wheelEvent(QWheelEvent* event)
{
	QPointF scenePnt = mapToScene(event->pos());

	if (event->modifiers() && Qt::Modifier::SHIFT){
		if (event->delta() > 0){
			scale(1, 1.1);
		}
		else{
			scale(1, 1 / 1.1);
		}
	}
	else{
		if (event->delta() > 0){
			scale(1.1, 1);
		}
		else{
			scale(1 / 1.1, 1);
		}
	}
}


void CEventGraphicsView::resizeEvent(QResizeEvent* event)
{
	BaseClass::resizeEvent(event);
}


// reimplemented (QAbstractScrollArea)

bool CEventGraphicsView::viewportEvent(QEvent *event)
{
	if (event->type() == QEvent::Paint && m_timeAxisPtr != nullptr){
		QRectF visibleRect = SceneVisibleRect();
		m_timeAxisPtr->setPos(0, visibleRect.bottom());
	}

	return BaseClass::viewportEvent(event);
}


// private methods

QRectF CEventGraphicsView::SceneVisibleRect() const
{
	return mapToScene(viewport()->rect()).boundingRect();
}


}


