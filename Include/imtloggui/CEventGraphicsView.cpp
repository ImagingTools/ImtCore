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
	m_userAction(false)//,
	//m_containerPtr(nullptr)
{
}


void CEventGraphicsView::SetTimeAxis(CTimeAxis* timeAxisPtr)
{
	m_timeAxisPtr = timeAxisPtr;
}


void CEventGraphicsView::SetContainer(QGraphicsItem* containerPtr)
{
	//m_containerPtr= containerPtr;
}


QRectF CEventGraphicsView::GetSceneVisibleRect() const
{
	return m_viewRect;
}


double CEventGraphicsView::GetScaleX() const
{
	return viewportTransform().m11();
}


double CEventGraphicsView::GetScaleY() const
{
	return viewportTransform().m22();
}


QRectF CEventGraphicsView::GetSceneRect()
{
	return m_sceneRect;
}


void CEventGraphicsView::SetSceneRect(const QRectF& rect)
{
	m_sceneRect = rect;
	ValidateViewRect();
}


QRectF CEventGraphicsView::GetViewRect()
{
	return m_viewRect;
}


void CEventGraphicsView::SetViewRect(const QRectF& rect)
{
	m_viewRect = rect;
	ValidateViewRect();

	//m_containerPtr->setPos(-m_viewRect.left(), 0);
	//m_viewRect.translate(-m_viewRect.left(), 0);

	UpdateViewRect();
	Q_EMIT EmitViewPortChanged(false);
}


// public slots

void CEventGraphicsView::OnMinimumVerticalScaleChanged(double minScale)
{
	m_minimumVerticalScale = minScale;

	if (viewportTransform().m22() < minScale){
		scale(1, minScale / viewportTransform().m22());
	}
}


// protected methods

// reimplemented (QGraphicsView)

void CEventGraphicsView::wheelEvent(QWheelEvent* event)
{
	//const ViewportAnchor anchor = transformationAnchor();
	//setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	QRectF rect = sceneRect();
	double scaleX = viewportTransform().m11();
	double scaleY = viewportTransform().m22();

	if (event->modifiers() && Qt::Modifier::SHIFT){
		if (event->delta() > 0){
			ScaleViewRect(mapToScene(event->pos()), 1, 1.1);
		}
		else{
			if (scaleY / 1.1 < m_minimumVerticalScale){
				ScaleViewRect(mapToScene(event->pos()), 1, m_minimumVerticalScale / scaleY);
			}
			else{
				ScaleViewRect(mapToScene(event->pos()), 1, 1 / 1.1);
			}
		}
	}
	else{
		if (event->delta() > 0){
			if (scaleX * 1.1 > 500000){
				ScaleViewRect(mapToScene(event->pos()), 500000 / scaleX, 1);
			}
			else{
				ScaleViewRect(mapToScene(event->pos()), 1 * 1.1, 1);
			}
		}
		else{
			ScaleViewRect(mapToScene(event->pos()), 1 / 1.1, 1);
		}
	}

	Q_EMIT EmitViewPortChanged(true);

	//setTransformationAnchor(anchor);
}


void CEventGraphicsView::resizeEvent(QResizeEvent* event)
{
	BaseClass::resizeEvent(event);

	if (m_timeAxisPtr != nullptr){
		m_sceneRect.setLeft(0);
		m_sceneRect.setRight((m_timeAxisPtr->GetEndTime().toMSecsSinceEpoch() - m_timeAxisPtr->GetBeginTime().toMSecsSinceEpoch())/ 1000.);
		m_viewRect = m_sceneRect;
	}

	ValidateViewRect();
	UpdateViewRect();

	Q_EMIT EmitViewPortChanged(true);
}


void CEventGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	BaseClass::mouseMoveEvent(event);

	if (event->buttons() & Qt::LeftButton){
		QPointF delta = m_lockedScenePoint - event->pos();
		m_lockedScenePoint = event->pos();

		MoveViewRect(delta.x() / viewportTransform().m11(), delta.y() / viewportTransform().m22());

		event->accept();
	}

	Q_EMIT EmitViewPortChanged(true);
}


void CEventGraphicsView::mousePressEvent(QMouseEvent *event)
{
	BaseClass::mousePressEvent(event);

	m_userAction = true;
	m_lockedScenePoint = event->pos();
}


void CEventGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	BaseClass::mouseReleaseEvent(event);
	m_userAction = false;
}


// private methods

void CEventGraphicsView::MoveViewRect(double dX, double dY)
{
	//QPointF containerOrigin = m_containerPtr->pos();
	//qDebug() << "*** " << containerOrigin;

	//containerOrigin.rx() -= dX;

	//m_containerPtr->setPos(1000000000000, 0);
	//m_containerPtr->setPos(containerOrigin);
	//m_viewRect.translate(0, dY);
	m_viewRect.translate(dX, dY);

	ValidateViewRect();
	UpdateViewRect();
}


void CEventGraphicsView::ScaleViewRect(const QPointF& center, double scaleX, double scaleY)
{
	if (scaleX != 1){
		m_viewRect.setLeft((m_viewRect.left() - center.x()) / scaleX + center.x());
		m_viewRect.setRight((m_viewRect.right() - center.x()) / scaleX + center.x());
	}

	if (scaleY != 1){
		m_viewRect.setTop((m_viewRect.top() - center.y()) / scaleY + center.y());
		m_viewRect.setBottom((m_viewRect.bottom() - center.y()) / scaleY + center.y());
	}

	ValidateViewRect();
	//QPointF containerOrigin = m_containerPtr->pos();

	//containerOrigin.rx() -= m_viewRect.left();
	//m_containerPtr->setPos(containerOrigin);
	//m_viewRect.translate(-m_viewRect.left(), 0);

	UpdateViewRect();
}


void CEventGraphicsView::ValidateViewRect()
{
	if (m_viewRect.width() > m_sceneRect.width()){
		m_viewRect.setWidth(m_sceneRect.width());
	}

	if (m_viewRect.height() > m_sceneRect.height()){
		m_viewRect.setHeight(m_sceneRect.height());
	}

	if (m_viewRect.top() < m_sceneRect.top()){
		m_viewRect.translate(0, m_sceneRect.top() - m_viewRect.top());
	}

	if (m_viewRect.bottom() > m_sceneRect.bottom()){
		m_viewRect.translate(0, m_sceneRect.bottom() - m_viewRect.bottom());
	}

	if (m_viewRect.left() < m_sceneRect.left()){
		m_viewRect.translate(m_sceneRect.left() - m_viewRect.left(), 0);
	}

	if (m_viewRect.right() > m_sceneRect.right()){
		m_viewRect.translate(m_sceneRect.right() - m_viewRect.right(), 0);
	}
}


void CEventGraphicsView::UpdateViewRect()
{
	setSceneRect(m_viewRect);
	QTransform matrix;
	matrix.translate(m_viewRect.topLeft().x(), m_viewRect.topLeft().y());
	matrix.scale(viewport()->rect().width() / m_viewRect.width(), viewport()->rect().height() / m_viewRect.height());

	setTransform(matrix);

	//scene()->update(m_viewRect);	

	//qDebug() << QString::number(m_containerPtr->pos().x(), 'f', 30) << m_viewRect << transform();
	//qDebug() << viewportTransform();
}


}


