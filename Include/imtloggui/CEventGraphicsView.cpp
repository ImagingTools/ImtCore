 #include <imtloggui/CEventGraphicsView.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QScrollBar>

namespace imtloggui
{


// public methods

CEventGraphicsView::CEventGraphicsView(QWidget* parent)
	:QGraphicsView(parent),
	m_timeAxisPtr(nullptr),
	m_minimumVerticalScale(1),
	m_userAction(false)
{
	connect(verticalScrollBar(), &QScrollBar::rangeChanged, this, &CEventGraphicsView::OnRangeChanged);
	connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &CEventGraphicsView::OnValueChanged);
	connect(horizontalScrollBar(), &QScrollBar::rangeChanged, this, &CEventGraphicsView::OnRangeChanged);
	connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, &CEventGraphicsView::OnValueChanged);
	connect(this, &CEventGraphicsView::EmitAxisPositionChanged, this, &CEventGraphicsView::OnAxisPositionChanged, Qt::QueuedConnection);

	horizontalScrollBar()->installEventFilter(this);
}


void CEventGraphicsView::SetTimeAxis(CTimeAxis* timeAxisPtr)
{
	m_timeAxisPtr = timeAxisPtr;
}


// public slots

void CEventGraphicsView::OnAxisPositionChanged()
{
	if (m_timeAxisPtr == nullptr){
		return;
	}

	QRectF visibleRect = SceneVisibleRect();
	m_timeAxisPtr->setPos(0, visibleRect.bottom() - m_timeAxisPtr->rect().height() / viewportTransform().m22());
	QRectF rect = sceneRect();
	if (m_timeAxisPtr != nullptr){
		rect.setLeft(m_timeAxisPtr->rect().left() - 100 / viewportTransform().m11());
		rect.setRight(m_timeAxisPtr->rect().right() + 100 / viewportTransform().m11());
		setSceneRect(rect);
	}
}


void CEventGraphicsView::OnMinimumVerticalScaleChanged(double minScale)
{
	m_minimumVerticalScale = minScale;

	if (transform().m22() < minScale){
		scale(1, minScale / transform().m22());
	}
}


// protected methods

// reimplemented (QGraphicsView)

void CEventGraphicsView::wheelEvent(QWheelEvent* event)
{
	const ViewportAnchor anchor = transformationAnchor();
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	if (event->modifiers() && Qt::Modifier::SHIFT){
		if (event->delta() > 0){
			scale(1, 1.1);
		}
		else{
			if (transform().m22() / 1.1 < m_minimumVerticalScale){
				scale(1, m_minimumVerticalScale / transform().m22());
			}
			else{
				scale(1, 1 / 1.1);
			}
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

	Q_EMIT EmitAxisPositionChanged();
	Q_EMIT EmitViewPortChanged(true);
}


void CEventGraphicsView::resizeEvent(QResizeEvent* event)
{
	BaseClass::resizeEvent(event);

	Q_EMIT EmitAxisPositionChanged();
	Q_EMIT EmitViewPortChanged(true);
}


void CEventGraphicsView::mousePressEvent(QMouseEvent *event)
{
	m_userAction = true;
	BaseClass::mousePressEvent(event);
}


void CEventGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	m_userAction = false;
	BaseClass::mouseReleaseEvent(event);
}


// reimplemented (QObject)

bool CEventGraphicsView::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress){
		m_userAction = true;
	}

	if (event->type() == QEvent::MouseButtonRelease){
		m_userAction = false;
	}

	return false;
}


// private slots:

void CEventGraphicsView::OnRangeChanged(int /*min*/, int /*max*/)
{
	Q_EMIT EmitAxisPositionChanged();
	Q_EMIT EmitViewPortChanged(m_userAction);
}


void CEventGraphicsView::OnValueChanged(int /*value*/)
{
	Q_EMIT EmitAxisPositionChanged();
	Q_EMIT EmitViewPortChanged(m_userAction);
}


// private methods

QRectF CEventGraphicsView::SceneVisibleRect() const
{
	return mapToScene(viewport()->rect()).boundingRect();
}


}


