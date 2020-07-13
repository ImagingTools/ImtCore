 #include <imtloggui/CViewModel.h>


// Qt includes
#include <QtGui/QMouseEvent>
#include <QtWidgets/QScrollBar>

namespace imtloggui
{


//QRectF CGraphicsView::GetSceneRect() const
//{
//	return m_sceneRect;
//}
//
//
//bool CGraphicsView::SetSceneRect(const QRectF& rect)
//{
//	if (m_sceneRect != rect){
//		CViewRectChangeNotifier notifier(this);
//
//		QRectF oldSceneRect = m_sceneRect;
//		m_sceneRect = rect;
//
//		UpdateViewRect();
//		Q_EMIT EmitSceneRectChanged(oldSceneRect, rect);
//
//		return true;
//	}
//
//	return false;
//}
//
//
//QRectF CGraphicsView::GetViewRect() const
//{
//	return m_viewRect;
//}
//
//
//bool CGraphicsView::SetViewRect(const QRectF& rect)
//{
//	if (m_viewRect != rect){
//		CViewRectChangeNotifier notifier(this);
//
//		m_viewRect = rect;
//
//		UpdateViewRect();
//
//		return true;
//	}
//
//	return false;
//}
//
//
//QMargins CGraphicsView::GetMargins() const
//{
//	return m_margins;
//}
//
//
//bool CGraphicsView::SetMargins(const QMargins& margins)
//{
//	if (m_margins != margins){
//		CViewRectChangeNotifier notifier(this);
//
//		QMargins oldMargins = m_margins;
//		m_margins = margins;
//
//		UpdateViewRect();
//		Q_EMIT EmitMarginsChanged(oldMargins, margins);
//
//		return true;
//	}
//
//	return false;
//}
//
//
//double CGraphicsView::GetScaleX() const
//{
//	return viewport()->rect().width() / m_viewRect.width();
//}
//
//
//double CGraphicsView::GetScaleY() const
//{
//	return viewport()->rect().height() / m_viewRect.height();
//}
//
//
//istd::CRange CGraphicsView::GetScaleXRange() const
//{
//	return m_scaleXRange;
//}
//
//
//istd::CRange CGraphicsView::GetScaleYRange() const
//{
//	return m_scaleYRange;
//}
//
//
//bool CGraphicsView::SetScaleXRange(const istd::CRange& range)
//{
//	if (range.IsValidNonEmpty() && range.GetMinValue() > 0){
//		CViewRectChangeNotifier notifier(this);
//
//		istd::CRange oldRange = m_scaleXRange;
//		m_scaleXRange = range;
//		
//		UpdateViewRect();
//		Q_EMIT EmitScaleRangeXChanged(oldRange, range);
//
//		return true;
//	}
//
//	return false;
//}
//
//
//bool CGraphicsView::SetScaleYRange(const istd::CRange& range)
//{
//	if (range.IsValidNonEmpty() && range.GetMinValue() > 0){
//		CViewRectChangeNotifier notifier(this);
//
//		istd::CRange oldRange = m_scaleYRange;
//		m_scaleYRange = range;
//
//		UpdateViewRect();
//		Q_EMIT EmitScaleRangeYChanged(oldRange, range);
//
//		return true;
//	}
//
//	return false;
//}
//
//
//double CGraphicsView::GetScaleXStep() const
//{
//	return m_scaleXStep;
//}
//
//
//double CGraphicsView::GetScaleYStep() const
//{
//	return m_scaleYStep;
//}
//
//
//bool CGraphicsView::SetScaleXStep(double step)
//{
//	if (step != m_scaleXStep && step > DBL_EPSILON){
//		double oldStep = m_scaleXStep;
//		m_scaleXStep = step;
//
//		Q_EMIT EmitScaleXStepChanged(oldStep, step);
//
//		return true;
//	}
//
//	return false;
//}
//
//
//bool CGraphicsView::SetScaleYStep(double step)
//{
//	if (step != m_scaleYStep && step > DBL_EPSILON){
//		double oldStep = m_scaleYStep;
//		m_scaleYStep = step;
//
//		Q_EMIT EmitScaleXStepChanged(oldStep, step);
//
//		return true;
//	}
//
//	return false;
//}
//
//
//// public slots
//
//void CGraphicsView::OnShowAll()
//{
//	CViewRectChangeNotifier notifier(this);
//
//	double viewWidth = m_sceneRect.width() / (1 - (double)(m_margins.left() + m_margins.right()) / viewport()->rect().width());
//	double viewHeight = m_sceneRect.height() / (1 - (double)(m_margins.top() + m_margins.bottom()) / viewport()->rect().height());
//
//	double scaleY = viewport()->rect().height() / viewHeight;
//	if (scaleY < m_scaleYRange.GetMinValue()){
//		viewHeight = viewport()->rect().height() / m_scaleYRange.GetMinValue();
//	}
//
//	m_viewRect = QRectF(m_sceneRect.bottomRight(), QSizeF(viewWidth, viewHeight));
//
//	UpdateViewRect();
//}
//
//
//// protected methods
//
//// reimplemented (QGraphicsView)
//
//void CGraphicsView::wheelEvent(QWheelEvent* event)
//{
//	CViewRectChangeNotifier notifier(this);
//
//	double scaleX = GetScaleX();
//	double scaleY = GetScaleY();
//
//	if (event->modifiers() && Qt::Modifier::SHIFT){
//		if (event->delta() > 0){
//			if (scaleY * 1.1 > m_scaleYRange.GetMaxValue()){
//				ScaleViewRect(mapToScene(event->pos()), 1, m_scaleYRange.GetMaxValue() / scaleY);
//			}
//			else{
//				ScaleViewRect(mapToScene(event->pos()), 1, 1.1);
//			}
//		}
//		else{
//			if (scaleY / 1.1 < m_scaleYRange.GetMinValue()){
//				ScaleViewRect(mapToScene(event->pos()), 1, m_scaleYRange.GetMinValue() / scaleY);
//			}
//			else{
//				ScaleViewRect(mapToScene(event->pos()), 1, 1 / 1.1);
//			}
//		}
//	}
//	else{
//		if (event->delta() > 0){
//			if (scaleX * 1.1 > m_scaleXRange.GetMaxValue()){
//				ScaleViewRect(mapToScene(event->pos()), m_scaleXRange.GetMaxValue() / scaleX, 1);
//			}
//			else{
//				ScaleViewRect(mapToScene(event->pos()), 1 * 1.1, 1);
//			}
//		}
//		else{
//			if (scaleX / 1.1 < m_scaleXRange.GetMinValue()){
//				ScaleViewRect(mapToScene(event->pos()), m_scaleXRange.GetMinValue() / scaleX, 1);
//			}
//			else{
//				ScaleViewRect(mapToScene(event->pos()), 1 / 1.1, 1);
//			}
//		}
//	}
//}
//
//
//void CGraphicsView::resizeEvent(QResizeEvent* event)
//{
//	CViewRectChangeNotifier notifier(this);
//
//	BaseClass::resizeEvent(event);
//
//	UpdateViewRect();
//}
//
//
//void CGraphicsView::mouseMoveEvent(QMouseEvent *event)
//{
//	CViewRectChangeNotifier notifier(this);
//
//	BaseClass::mouseMoveEvent(event);
//
//	if (event->buttons() & Qt::LeftButton){
//		QPointF delta = m_lockedScenePoint - event->pos();
//		m_lockedScenePoint = event->pos();
//
//		MoveViewRect(delta.x() / GetScaleX(), delta.y() / GetScaleY());
//
//		event->accept();
//	}
//}
//
//
//void CGraphicsView::mousePressEvent(QMouseEvent *event)
//{
//	BaseClass::mousePressEvent(event);
//
//	m_userAction = true;
//	m_lockedScenePoint = event->pos();
//}
//
//
//void CGraphicsView::mouseReleaseEvent(QMouseEvent *event)
//{
//	BaseClass::mouseReleaseEvent(event);
//
//	m_userAction = false;
//}
//
//
//// private methods
//
//void CGraphicsView::MoveViewRect(double dX, double dY)
//{
//	CViewRectChangeNotifier notifier(this);
//
//	m_viewRect.translate(dX, dY);
//
//	UpdateViewRect();
//}
//
//
//void CGraphicsView::ScaleViewRect(const QPointF& center, double scaleX, double scaleY)
//{
//	CViewRectChangeNotifier notifier(this);
//
//	if (scaleX != 1){
//		m_viewRect.setLeft((m_viewRect.left() - center.x()) / scaleX + center.x());
//		m_viewRect.setRight((m_viewRect.right() - center.x()) / scaleX + center.x());
//	}
//
//	if (scaleY != 1){
//		m_viewRect.setTop((m_viewRect.top() - center.y()) / scaleY + center.y());
//		m_viewRect.setBottom((m_viewRect.bottom() - center.y()) / scaleY + center.y());
//	}
//
//	UpdateViewRect();
//}
//
//
//void CGraphicsView::UpdateViewRect()
//{
//	// Validate scale range
//
//	double scaleX = GetScaleX();
//	double scaleY = GetScaleY();
//
//	if (scaleX < m_scaleXRange.GetMinValue()){
//		m_viewRect.setWidth(viewport()->rect().width() / m_scaleXRange.GetMinValue());
//	}
//
//	if (scaleX > m_scaleYRange.GetMaxValue()){
//		m_viewRect.setWidth(viewport()->rect().width() / m_scaleXRange.GetMaxValue());
//	}
//
//	if (scaleY < m_scaleYRange.GetMinValue()){
//		m_viewRect.setHeight(viewport()->rect().height() / m_scaleYRange.GetMinValue());
//	}
//
//	if (scaleY > m_scaleYRange.GetMaxValue()){
//		m_viewRect.setHeight(viewport()->rect().height() / m_scaleYRange.GetMaxValue());
//	}
//
//	scaleX = GetScaleX();
//	scaleY = GetScaleY();
//
//	// Validate view rect
//
//	QMarginsF margins = QMarginsF(m_margins.left() / scaleX, m_margins.top() / scaleY, m_margins.right() / scaleX, m_margins.bottom() / scaleY);
//	QRectF sceneRect = m_sceneRect.marginsAdded(margins);
//
//	if (m_viewRect.width() > sceneRect.width()){
//		m_viewRect.setWidth(sceneRect.width());
//	}
//
//	if (m_viewRect.height() > sceneRect.height()){
//		m_viewRect.setHeight(sceneRect.height());
//	}
//
//	if (m_viewRect.top() < sceneRect.top()){
//		m_viewRect.translate(0, sceneRect.top() - m_viewRect.top());
//	}
//
//	if (m_viewRect.bottom() > sceneRect.bottom()){
//		m_viewRect.translate(0, sceneRect.bottom() - m_viewRect.bottom());
//	}
//
//	if (m_viewRect.left() < sceneRect.left()){
//		m_viewRect.translate(sceneRect.left() - m_viewRect.left(), 0);
//	}
//
//	if (m_viewRect.right() > sceneRect.right()){
//		m_viewRect.translate(sceneRect.right() - m_viewRect.right(), 0);
//	}
//
//	// Set new transformation
//
//	setSceneRect(m_viewRect);
//	QTransform matrix;
//	matrix.translate(m_viewRect.topLeft().x(), m_viewRect.topLeft().y());
//	matrix.scale(GetScaleX(), GetScaleY());
//
//	setTransform(matrix);
//}
//
//
//void CGraphicsView::ViewRectChangeNotify(QRectF before, QRectF after, bool userAction)
//{
//	Q_EMIT EmitViewRectChanged(before, after, m_userAction || userAction);
//}
//
//
//// public methods of embedded class CViewRectChangeNotifier
//
//CGraphicsView::CViewRectChangeNotifier::CViewRectChangeNotifier(CGraphicsView* graphicsView, bool userAction)
//{
//	m_graphicsViewPtr = graphicsView;
//	if (m_graphicsViewPtr != nullptr){
//		m_before = m_graphicsViewPtr->GetViewRect();
//		m_userAction = userAction;
//	}
//}
//
//
//CGraphicsView::CViewRectChangeNotifier::~CViewRectChangeNotifier()
//{
//	if (m_graphicsViewPtr != nullptr){
//		QRectF after = m_graphicsViewPtr->GetViewRect();
//		if (m_before != after){
//			m_graphicsViewPtr->ViewRectChangeNotify(m_before, after, m_userAction);
//		}
//	}
//}


} // namespace imtloggui


