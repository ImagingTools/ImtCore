#include <imtloggui/CEventGraphicsView.h>


// Qt includes
#include <QtGui/QMouseEvent>
#include <QtWidgets/QScrollBar>

namespace imtloggui
{


// public methods

CEventGraphicsView::CEventGraphicsView(QWidget* parent)
	: QGraphicsView(parent),
	m_timeAxisPtr(nullptr)
{
	connect(verticalScrollBar(), &QScrollBar::rangeChanged, this, &CEventGraphicsView::rangeChanged);
	connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &CEventGraphicsView::valueChanged);
	connect(horizontalScrollBar(), &QScrollBar::rangeChanged, this, &CEventGraphicsView::rangeChanged);
	connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, &CEventGraphicsView::valueChanged);
	connect(this, &CEventGraphicsView::AxisPositionChanged, this, &CEventGraphicsView::OnAxisPositionChanged, Qt::QueuedConnection);
}


void CEventGraphicsView::setTimeAxis(CTimeAxis* timeAxisPtr)
{
	m_timeAxisPtr = timeAxisPtr;
}


// public slots:

void CEventGraphicsView::OnAxisPositionChanged()
{
	if (m_timeAxisPtr == nullptr){
		return;
	}

	m_lastTransform = viewportTransform();

	QRectF visibleRect = SceneVisibleRect();
	m_timeAxisPtr->setPos(0, visibleRect.bottom() - m_timeAxisPtr->rect().height() / viewportTransform().m22());
	QRectF rect = sceneRect();
	if (m_timeAxisPtr != nullptr){
		rect.setLeft(m_timeAxisPtr->rect().left() - 100 / viewportTransform().m11());
		rect.setRight(m_timeAxisPtr->rect().right() + 100 / viewportTransform().m11());
		setSceneRect(rect);
	}
}


// protected methods

// reimplemented (QGraphicsView)

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
	const ViewportAnchor anchor = transformationAnchor();
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

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

	setTransformationAnchor(anchor);

	OnAxisPositionChanged();
}


void CEventGraphicsView::resizeEvent(QResizeEvent* event)
{
	BaseClass::resizeEvent(event);

	OnAxisPositionChanged();
}


// protected slots:

void CEventGraphicsView::rangeChanged(int /*min*/, int /*max*/)
{
	OnAxisPositionChanged();
}


void CEventGraphicsView::valueChanged(int /*value*/)
{
	OnAxisPositionChanged();
}


// private methods

QRectF CEventGraphicsView::SceneVisibleRect() const
{
	return mapToScene(viewport()->rect()).boundingRect();
}


}


