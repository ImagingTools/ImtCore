 #include <imtloggui/CEventGraphicsView.h>


// Qt includes
#include <QtGui/QMouseEvent>
#include <QtWidgets/QScrollBar>

// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtloggui
{


// public methods

CEventGraphicsView::CEventGraphicsView(QWidget* parent)
	:QGraphicsView(parent),
	m_minimumVerticalScale(1),
	m_userAction(false)
{
}


// reimplemented (imtloggui::IViewPropertyProvider)

QRectF CEventGraphicsView::GetSceneRect() const
{
	return m_sceneRect;
}


QRectF CEventGraphicsView::GetViewRect() const
{
	return m_viewRect;
}


double CEventGraphicsView::GetScaleX() const
{
	QRect rect = viewport()->rect();

	return viewport()->rect().width() / m_viewRect.width();
}


double CEventGraphicsView::GetScaleY() const
{
	QRect rect = viewport()->rect();

	return viewport()->rect().height() / m_viewRect.height();
}


QMargins CEventGraphicsView::GetMargins() const
{
	return m_margins;
}


istd::CRange CEventGraphicsView::GetScaleRangeX() const
{
	return m_scaleRangeX;
}


istd::CRange CEventGraphicsView::GetScaleRangeY() const
{
	return m_scaleRangeY;
}


// reimplemented (imtloggui::IViewPropertyManager)

bool CEventGraphicsView::SetSceneRect(const QRectF& rect)
{
	if (rect != m_sceneRect){
		istd::CChangeNotifier notifier(this);

		m_sceneRect = rect;

		ValidateViewRect();
		UpdateViewRect();

		return true;
	}

	return false;
}


bool CEventGraphicsView::SetViewRect(const QRectF& rect)
{
	if (rect != m_viewRect){
		istd::CChangeNotifier notifier(this);

		m_viewRect = rect;

		ValidateViewRect();
		UpdateViewRect();

		Q_EMIT EmitViewPortChanged(false);
	}

	return false;
}


bool CEventGraphicsView::SetMargins(const QMargins& margins)
{
	if (margins != m_margins){
		istd::CChangeNotifier notifier(this);

		m_margins = margins;

		ValidateViewRect();
		UpdateViewRect();

		return true;
	}

	return false;	
}


bool CEventGraphicsView::SetScaleRangeX(const istd::CRange& range)
{
	m_scaleRangeX = range;

	ValidateViewRect();
	UpdateViewRect();

	return true;
}


bool CEventGraphicsView::SetScaleRangeY(const istd::CRange& range)
{
	m_scaleRangeX = range;

	ValidateViewRect();
	UpdateViewRect();

	return true;
}


// public slots

void CEventGraphicsView::OnShowAll()
{
	double viewWidth = m_sceneRect.width() / (1 - (double)(m_margins.left() + m_margins.right()) / viewport()->rect().width());
	double viewHeight = m_sceneRect.height() / (1 - (double)(m_margins.top() + m_margins.bottom()) / viewport()->rect().height());

	double scaleY = viewport()->rect().height() / viewHeight;
	if (scaleY < m_minimumVerticalScale){
		viewHeight = viewport()->rect().height() / m_minimumVerticalScale;
	}

	m_viewRect = QRectF(m_sceneRect.bottomRight(), QSizeF(viewWidth, viewHeight));

	ValidateViewRect();
	UpdateViewRect();
	Q_EMIT EmitViewPortChanged(true);
}


// protected methods

// reimplemented (QGraphicsView)

void CEventGraphicsView::wheelEvent(QWheelEvent* event)
{
	istd::CChangeNotifier notifier(this);

	double scaleX = GetScaleX();
	double scaleY = GetScaleY();

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
}


void CEventGraphicsView::resizeEvent(QResizeEvent* event)
{
	istd::CChangeNotifier notifier(this);

	BaseClass::resizeEvent(event);

	ValidateViewRect();
	UpdateViewRect();

	Q_EMIT EmitViewPortChanged(false);
}


void CEventGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	istd::CChangeNotifier notifier(this);

	BaseClass::mouseMoveEvent(event);

	if (event->buttons() & Qt::LeftButton){
		QPointF delta = m_lockedScenePoint - event->pos();
		m_lockedScenePoint = event->pos();

		MoveViewRect(delta.x() / GetScaleX(), delta.y() / GetScaleY());

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
	UpdateViewRect();
}


void CEventGraphicsView::ValidateViewRect()
{
	double scaleX = GetScaleX();
	double scaleY = GetScaleY();

	QMarginsF margins = QMarginsF(m_margins.left() / scaleX, m_margins.top() / scaleY, m_margins.right() / scaleX, m_margins.bottom() / scaleY);
	QRectF sceneRect = m_sceneRect.marginsAdded(margins);

	if (m_viewRect.width() > sceneRect.width()){
		m_viewRect.setWidth(sceneRect.width());
	}

	if (m_viewRect.height() > sceneRect.height()){
		m_viewRect.setHeight(sceneRect.height());
	}

	if (m_viewRect.top() < sceneRect.top()){
		m_viewRect.translate(0, sceneRect.top() - m_viewRect.top());
	}

	if (m_viewRect.bottom() > sceneRect.bottom()){
		m_viewRect.translate(0, sceneRect.bottom() - m_viewRect.bottom());
	}

	if (m_viewRect.left() < sceneRect.left()){
		m_viewRect.translate(sceneRect.left() - m_viewRect.left(), 0);
	}

	if (m_viewRect.right() > sceneRect.right()){
		m_viewRect.translate(sceneRect.right() - m_viewRect.right(), 0);
	}
}


void CEventGraphicsView::UpdateViewRect()
{
	setSceneRect(m_viewRect);
	QTransform matrix;
	matrix.translate(m_viewRect.topLeft().x(), m_viewRect.topLeft().y());
	matrix.scale(GetScaleX(), GetScaleY());

	setTransform(matrix);
}


}


