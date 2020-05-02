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
	connect(verticalScrollBar(), &QScrollBar::rangeChanged, this, &CEventGraphicsView::ScrollRangeChanged);
	connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &CEventGraphicsView::ScrollValueChanged);
	connect(horizontalScrollBar(), &QScrollBar::rangeChanged, this, &CEventGraphicsView::ScrollRangeChanged);
	connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, &CEventGraphicsView::ScrollValueChanged);
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

	BaseClass::wheelEvent(event);
}


void CEventGraphicsView::resizeEvent(QResizeEvent* event)
{
	BaseClass::resizeEvent(event);
}


// reimplemented (QAbstractScrollArea)

bool CEventGraphicsView::viewportEvent(QEvent *event)
{
	if (m_timeAxisPtr != nullptr){
		switch (event->type()){
		case QEvent::Wheel:
			break;
		case QEvent::DragMove:
			break;
		}
	}

	return BaseClass::viewportEvent(event);
}


// private slots

void CEventGraphicsView::ScrollRangeChanged(int min, int max)
{
	if (m_timeAxisPtr != nullptr){
		m_timeAxisPtr->update();
	}
}


void CEventGraphicsView::ScrollValueChanged(int value)
{
	if (m_timeAxisPtr != nullptr){
		m_timeAxisPtr->update();
	}
}


}


