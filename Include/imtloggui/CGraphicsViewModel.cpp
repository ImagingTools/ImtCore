 #include <imtloggui/CGraphicsViewModel.h>


// ACF includes
#include <istd/CChangeNotifier.h>

// Qt includes
#include <QtGui/QMouseEvent>
#include <QtWidgets/QScrollBar>

namespace imtloggui
{


// public methods

void CGraphicsViewModel::ShowAll()
{
	CViewRectChangeNotifier viewRectChangeNotifier(this);

	double viewWidth = m_sceneRect.width() / (1 - (double)(m_margins.left() + m_margins.right()) / m_viewPortRect.width());
	double viewHeight = m_sceneRect.height() / (1 - (double)(m_margins.top() + m_margins.bottom()) / m_viewPortRect.height());
	m_viewRect = QRectF(m_sceneRect.bottomRight(), QSizeF(viewWidth, viewHeight));

	UpdateViewRect();
}


QRectF CGraphicsViewModel::GetSceneRect() const
{
	return m_sceneRect;
}


bool CGraphicsViewModel::SetSceneRect(const QRectF& rect)
{
	if (m_sceneRect != rect){
		CViewRectChangeNotifier viewRectChangeNotifier(this);
		istd::CChangeNotifier changeNotifier(this, &istd::IChangeable::ChangeSet(CF_SCENE_RECT));

		m_sceneRect = rect;
		UpdateViewRect();		

		return true;
	}

	return false;
}


bool CGraphicsViewModel::SetViewRect(const QRectF& rect)
{
	if (m_viewRect != rect){
		CViewRectChangeNotifier viewRectChangeNotifier(this);

		m_viewRect = rect;
		UpdateViewRect();

		return true;
	}

	return false;
}


QMargins CGraphicsViewModel::GetMargins() const
{
	return m_margins;
}


bool CGraphicsViewModel::SetMargins(const QMargins& margins)
{
	if (m_margins != margins){
		CViewRectChangeNotifier viewRectChangeNotifier(this);
		istd::CChangeNotifier changeNotifier(this, &istd::IChangeable::ChangeSet(CF_MARGINS));

		m_margins = margins;
		UpdateViewRect();

		return true;
	}

	return false;
}


double CGraphicsViewModel::GetScaleX() const
{
	return m_viewPortRect.width() / m_viewRect.width();
}


double CGraphicsViewModel::GetScaleY() const
{
	return m_viewPortRect.height() / m_viewRect.height();
}


istd::CRange CGraphicsViewModel::GetScaleXRange() const
{
	return m_scaleXRange;
}


istd::CRange CGraphicsViewModel::GetScaleYRange() const
{
	return m_scaleYRange;
}


bool CGraphicsViewModel::SetScaleXRange(const istd::CRange& range)
{
	if (range.IsValidNonEmpty() && range.GetMinValue() > 0){
		CViewRectChangeNotifier viewRectChangeNotifier(this);
		istd::CChangeNotifier changeNotifier(this, &istd::IChangeable::ChangeSet(CF_SCALE_X_RANGE));

		m_scaleXRange = range;
		UpdateViewRect();

		return true;
	}

	return false;
}


bool CGraphicsViewModel::SetScaleYRange(const istd::CRange& range)
{
	if (range.IsValidNonEmpty() && range.GetMinValue() > 0){
		CViewRectChangeNotifier viewRectChangeNotifier(this);
		istd::CChangeNotifier changeNotifier(this, &istd::IChangeable::ChangeSet(CF_SCALE_Y_RANGE));

		m_scaleYRange = range;
		UpdateViewRect();

		return true;
	}

	return false;
}


bool CGraphicsViewModel::Scroll(const QPointF& delta, bool inPercents)
{
	CViewRectChangeNotifier viewRectChangeNotifier(this);

	m_viewRect.translate(delta);
	UpdateViewRect();

	return true;
}


bool CGraphicsViewModel::Zoom(const QPointF& factors, const QPointF& zoomOrigin)
{
	CViewRectChangeNotifier viewRectChangeNotifier(this);

	double factorX = factors.x();
	double factorY = factors.y();

	if (factorX == 1 || factorX == 0){
		return false;
	}

	if (factorY == 1 || factorY == 0){
		return false;
	}

	m_viewRect.setLeft((m_viewRect.left() - zoomOrigin.x()) / factorX + zoomOrigin.x());
	m_viewRect.setRight((m_viewRect.right() - zoomOrigin.x()) / factorX + zoomOrigin.x());
	m_viewRect.setTop((m_viewRect.top() - zoomOrigin.y()) / factorY + zoomOrigin.y());
	m_viewRect.setBottom((m_viewRect.bottom() - zoomOrigin.y()) / factorY + zoomOrigin.y());

	UpdateViewRect();

	return true;
}


QRectF CGraphicsViewModel::GetViewRect() const
{
	return m_viewRect;
}


// private methods

void CGraphicsViewModel::UpdateViewRect()
{
	CViewRectChangeNotifier notifier(this);

	// Validate scale range

	double scaleX = GetScaleX();
	double scaleY = GetScaleY();

	if (scaleX < m_scaleXRange.GetMinValue()){
		m_viewRect.setWidth(m_viewPortRect.width() / m_scaleXRange.GetMinValue());
	}

	if (scaleX > m_scaleYRange.GetMaxValue()){
		m_viewRect.setWidth(m_viewPortRect.width() / m_scaleXRange.GetMaxValue());
	}

	if (scaleY < m_scaleYRange.GetMinValue()){
		m_viewRect.setHeight(m_viewPortRect.height() / m_scaleYRange.GetMinValue());
	}

	if (scaleY > m_scaleYRange.GetMaxValue()){
		m_viewRect.setHeight(m_viewPortRect.height() / m_scaleYRange.GetMaxValue());
	}

	scaleX = GetScaleX();
	scaleY = GetScaleY();

	// Validate view rect

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


void CGraphicsViewModel::OnViewRectChanged()
{
	istd::CChangeNotifier notifier(this, &istd::IChangeable::ChangeSet(CF_VIEW_RECT));
}


// public methods of embedded CViewRectChangeNotifier

CGraphicsViewModel::CViewRectChangeNotifier::CViewRectChangeNotifier(CGraphicsViewModel* parent)
	: m_parentPtr(parent)
{
	Q_ASSERT(m_parentPtr != nullptr);

	if (m_parentPtr != nullptr){
		m_viewRect = m_parentPtr->GetViewRect();
	}
}


CGraphicsViewModel::CViewRectChangeNotifier::~CViewRectChangeNotifier()
{
	if (m_parentPtr != nullptr){
		if (m_viewRect != m_parentPtr->GetViewRect()){
			m_parentPtr->OnViewRectChanged();
		}
	}
}


} // namespace imtloggui


