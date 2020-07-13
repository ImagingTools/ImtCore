#include <imtloggui/CGraphicsViewComp.h>


// ImtBase includes
#include <imtloggui/CGraphicsView.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IViewController)

QPointF CGraphicsViewComp::GetZoomFactors() const
{
	return m_zoomFactors;
}


bool CGraphicsViewComp::SetZoomFactors(const QPointF& factors)
{
	if (factors.x() > 0 && factors.y() > 0){
		m_zoomFactors = factors;
		return true;
	}

	return false;
}


QPointF CGraphicsViewComp::GetPercentageScrollingSteps() const
{
	return m_percentageScrollingSteps;
}


bool CGraphicsViewComp::SetPercentageScrollingSteps(const QPointF& steps)
{
	if (steps.x() > 0 && steps.x() <= 100 && steps.y() > 0 && steps.y() <= 100){
		m_percentageScrollingSteps = steps;
		return true;
	}

	return false;
}


QWidget* CGraphicsViewComp::CreateQtWidget(QWidget* parentPtr)
{
	Q_ASSERT(!IsGuiCreated());

	CGraphicsView* widgetPtr = new CGraphicsView(parentPtr);

	m_viewModel.AttachObserver(widgetPtr);

	return widgetPtr;
}


// private slots

void CGraphicsViewComp::OnMouseMoveEvent(QMouseEvent *event)
{
	CGraphicsView* viewPtr = dynamic_cast<CGraphicsView*>(GetWidget());

	if (event->buttons() & Qt::LeftButton){
		QPointF newlockedScenePoint = viewPtr->mapToScene(event->pos());
		QPointF delta = m_lockedScenePoint - newlockedScenePoint;

		m_viewModel.Scroll(delta);

		m_lockedScenePoint = newlockedScenePoint;
	}
}


void CGraphicsViewComp::OnMousePressEvent(QMouseEvent *event)
{
	CGraphicsView* viewPtr = dynamic_cast<CGraphicsView*>(GetWidget());
 
	m_lockedScenePoint = viewPtr->mapToScene(event->pos());
}


void CGraphicsViewComp::OnMouseReleaseEvent(QMouseEvent *event)
{

}


void CGraphicsViewComp::OnWheelEvent(QWheelEvent* event)
{
	CGraphicsView* viewPtr = dynamic_cast<CGraphicsView*>(GetWidget());

	if (event->modifiers() && Qt::Modifier::SHIFT){
		if (event->delta() > 0){
			m_viewModel.Zoom(QPointF(m_zoomFactors.x(), 1), viewPtr->mapToScene(event->pos()));
		}
		else{
			m_viewModel.Zoom(QPointF(1 / m_zoomFactors.x(), 1), viewPtr->mapToScene(event->pos()));
		}
	}
	else{
		if (event->delta() > 0){
			m_viewModel.Zoom(QPointF(1, m_zoomFactors.y()), viewPtr->mapToScene(event->pos()));
		}
		else{
			m_viewModel.Zoom(QPointF(1, 1 / m_zoomFactors.y()), viewPtr->mapToScene(event->pos()));
		}
	}
}


void CGraphicsViewComp::OnKeyPressEvent(QKeyEvent *event)
{

}


void CGraphicsViewComp::OnKeyReleaseEvent(QKeyEvent *event)
{

}


} // namespace imtloggui


