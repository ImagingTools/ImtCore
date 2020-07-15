#include <imtloggui/CGraphicsViewComp.h>


// ImtBase includes
#include <imtloggui/CGraphicsView.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IViewController)

double CGraphicsViewComp::GetZoomFactorX() const
{
	return m_zoomFactorX;
}

double CGraphicsViewComp::GetZoomFactorY() const
{
	return m_zoomFactorY;
}


bool CGraphicsViewComp::SetZoomFactors(double zoomFactorX, double zoomFactorY)
{
	if (zoomFactorX > 0 && zoomFactorY > 0){
		m_zoomFactorX = zoomFactorX;
		m_zoomFactorY = zoomFactorY;
		return true;
	}

	return false;
}


double CGraphicsViewComp::GetPercentageScrollingStepX() const
{
	return m_percentageScrollingStepX;
}


double CGraphicsViewComp::GetPercentageScrollingStepY() const
{
	return m_percentageScrollingStepY;
}


bool CGraphicsViewComp::SetPercentageScrollingSteps(double scrollingStepX, double scrollingStepY)
{
	if (scrollingStepX > 0 && scrollingStepX <= 100 && scrollingStepY > 0 && scrollingStepY <= 100){
		m_percentageScrollingStepX = scrollingStepX;
		m_percentageScrollingStepY = scrollingStepY;
		return true;
	}

	return false;
}


bool CGraphicsViewComp::Scroll(double dx, double dy)
{
	QRectF viewRect = m_viewModel.GetViewRect();
	viewRect.translate(dx, dy);
	return m_viewModel.SetViewRect(viewRect);
}


bool CGraphicsViewComp::Zoom(const QPointF& zoomOrigin, double zoomFactorX, double zoomFactorY)
{
	QRectF viewRect = m_viewModel.GetViewRect();

	if (zoomFactorX <= 0){
		return false;
	}

	if (zoomFactorY <= 0){
		return false;
	}

	viewRect.setLeft((viewRect.left() - zoomOrigin.x()) / zoomFactorX + zoomOrigin.x());
	viewRect.setRight((viewRect.right() - zoomOrigin.x()) / zoomFactorX + zoomOrigin.x());
	viewRect.setTop((viewRect.top() - zoomOrigin.y()) / zoomFactorY + zoomOrigin.y());
	viewRect.setBottom((viewRect.bottom() - zoomOrigin.y()) / zoomFactorY + zoomOrigin.y());

	return m_viewModel.SetViewRect(viewRect);
}


QWidget* CGraphicsViewComp::CreateQtWidget(QWidget* parentPtr)
{
	Q_ASSERT(!IsGuiCreated());

	CGraphicsView* widgetPtr = new CGraphicsView(parentPtr);
	m_viewModel.AttachObserver(widgetPtr);

	return widgetPtr;
}


void CGraphicsViewComp::OnGuiCreated()
{
	if (m_graphicsSceneProviderCompPtr.IsValid()){
		CGraphicsView* viewPtr = dynamic_cast<CGraphicsView*>(GetWidget());
		viewPtr->setScene(m_graphicsSceneProviderCompPtr->GetScene());
	}
}


// private slots

void CGraphicsViewComp::OnResizeEvent(QResizeEvent *event)
{
	m_viewModel.SetViewPortSize(event->size());
}


void CGraphicsViewComp::OnMouseMoveEvent(QMouseEvent *event)
{
	CGraphicsView* viewPtr = dynamic_cast<CGraphicsView*>(GetWidget());

	if (event->buttons() & Qt::LeftButton){
		QPointF newlockedScenePoint = viewPtr->mapToScene(event->pos());
		QPointF delta = m_lockedScenePoint - newlockedScenePoint;

		Scroll(delta.x(), delta.y());

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
			Zoom(dynamic_cast<QGraphicsView*>(sender())->mapToScene(event->pos()), m_zoomFactorX, 1);
		}
		else{
			Zoom(dynamic_cast<QGraphicsView*>(sender())->mapToScene(event->pos()), 1 / m_zoomFactorX, 1);
		}
	}
	else{
		if (event->delta() > 0){
			Zoom(dynamic_cast<QGraphicsView*>(sender())->mapToScene(event->pos()), 1, m_zoomFactorY);
		}
		else{
			Zoom(dynamic_cast<QGraphicsView*>(sender())->mapToScene(event->pos()), 1, 1 / m_zoomFactorY);
		}
	}
}


void CGraphicsViewComp::OnKeyPressEvent(QKeyEvent *event)
{
	QRectF viewRect = m_viewModel.GetViewRect();

	switch (event->key()){
	case Qt::Key_Plus:
		{
			Zoom(viewRect.center(), m_zoomFactorX, m_zoomFactorY);
		}
		break;
	case Qt::Key_Minus:
		{
			Zoom(viewRect.center(), 1 / m_zoomFactorX, 1 / m_zoomFactorY);
		}
		break;
	case Qt::Key_Left:
		{
			Scroll(-viewRect.width() * m_percentageScrollingStepX, 0);
		}
		break;
	case Qt::Key_Right:
		{
			Scroll(viewRect.width() * m_percentageScrollingStepX, 0);
		}
		break;
	case Qt::Key_Up:
		{
			Scroll(0, -viewRect.height() * m_percentageScrollingStepY);
		}
		break;
	case Qt::Key_Down:
		{
			Scroll(0, viewRect.height() * m_percentageScrollingStepY);
		}
		break;
	}
}


void CGraphicsViewComp::OnKeyReleaseEvent(QKeyEvent *event)
{

}


} // namespace imtloggui


