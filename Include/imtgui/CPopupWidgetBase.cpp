#include <imtgui/CPopupWidgetBase.h>


namespace imtgui
{


// public methods

CPopupWidgetBase::CPopupWidgetBase()
	:m_isClosingOnMouseClickAllowed(true)
{
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);

	setMouseTracking(true);
}


// reimplemented (IPopupWidget)

const ilog::IMessageConsumer::MessagePtr& CPopupWidgetBase::GetMessagePtr() const
{
	return m_messagePtr;
}


void CPopupWidgetBase::AllowClosingOnMouseClick(bool isAllowed)
{
	m_isClosingOnMouseClickAllowed = isAllowed;
}


// protected methods

// reimplemented (QWidget)

void CPopupWidgetBase::enterEvent(QEvent* event)
{
	BaseClass::enterEvent(event);

	Q_EMIT EmitHoverEnter(event);
}


void CPopupWidgetBase::leaveEvent(QEvent* event)
{
	BaseClass::enterEvent(event);

	Q_EMIT EmitHoverLeave(event);
}


void CPopupWidgetBase::mousePressEvent(QMouseEvent* event)
{
	if (m_isClosingOnMouseClickAllowed){
		Q_EMIT EmitCloseRequest();
	}
}


} // namespace imtgui


