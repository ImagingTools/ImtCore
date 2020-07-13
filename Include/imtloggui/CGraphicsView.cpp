 #include <imtloggui/CGraphicsView.h>


namespace imtloggui
{


// public methods

CGraphicsView::CGraphicsView(QWidget *parent)
	: QGraphicsView(parent)
{
}

// protected methods

// reimplemented (imod::TSingleModelObserverBase)

void CGraphicsView::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	IViewRectProvider* modelPtr = GetObservedObject();

	QRectF rect = modelPtr->GetViewRect();

	double scaleX = viewport()->rect().width() / rect.width();
	double scaleY = viewport()->rect().width() / rect.width();

	setSceneRect(rect);
	QTransform transform;
	transform.translate(rect.topLeft().x(), rect.topLeft().y());
	transform.scale(scaleX, scaleY);

	setTransform(transform);
}


// reimplemented (QGraphicsView)

void CGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	Q_EMIT EmitMouseMoveEvent(event);
}


void CGraphicsView::mousePressEvent(QMouseEvent *event)
{
	Q_EMIT EmitMousePressEvent(event);
}


void CGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	Q_EMIT EmitMouseReleaseEvent(event);
}


void CGraphicsView::wheelEvent(QWheelEvent* event)
{
	Q_EMIT EmitWheelEvent(event);
}


void CGraphicsView::keyPressEvent(QKeyEvent *event)
{
	Q_EMIT EmitKeyPressEvent(event);
}


void CGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
	Q_EMIT EmitKeyReleaseEvent(event);
}


} // namespace imtloggui


