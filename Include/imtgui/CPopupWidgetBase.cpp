#include <imtgui/CPopupWidgetBase.h>

// Qt includes
#include <QtGui/QEnterEvent>


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

#if QT_VERSION < 0x060000

void CPopupWidgetBase::enterEvent(QEvent* event)
{
	BaseClass::enterEvent(event);

	Q_EMIT EmitHoverEnter(event);
}

#else

void CPopupWidgetBase::enterEvent(QEnterEvent *event)
{
	BaseClass::enterEvent(event);

	Q_EMIT EmitHoverEnter(event);
}
#endif


void CPopupWidgetBase::leaveEvent(QEvent* event)
{
	BaseClass::leaveEvent(event);

	Q_EMIT EmitHoverLeave(event);
}


void CPopupWidgetBase::mousePressEvent(QMouseEvent* /*event*/)
{
	if (m_isClosingOnMouseClickAllowed){
		Q_EMIT EmitCloseRequest();
	}
}


} // namespace imtgui


