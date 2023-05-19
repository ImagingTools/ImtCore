#include <imtgui/CStandardClientAreaProviderComp.h>


// Qt includes
#include <QtGui/QResizeEvent>
#include <QtGui/QScreen>
#include <QtWidgets/QSlider>
#include <QtWidgets/QLabel>

// ACF includes
#include <ilog/CMessage.h>


namespace imtgui
{


// public methods

CStandardClientAreaProviderComp::CStandardClientAreaProviderComp()
	:m_corner(Qt::BottomRightCorner)
{
}


// reimplemented (IClientAreaProvider)

QPoint CStandardClientAreaProviderComp::GetAnchor() const
{
	return m_anchor;
}


QSize CStandardClientAreaProviderComp::GetSize() const
{
	return m_size;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CStandardClientAreaProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QWidget* windowPtr = GetWindow();
	if (windowPtr != nullptr){
		windowPtr->installEventFilter(this);
	}
}


// protected methods

// reimplemented (QObject)

bool CStandardClientAreaProviderComp::eventFilter(QObject* /*watched*/, QEvent* event)
{
	if (event->type() == QEvent::Move || event->type() == QEvent::Resize){
		QWidget* windowPtr = GetWindow();
		if (windowPtr != nullptr){
			SetRect(windowPtr->geometry());
		}
	}

	return false;
}


// private methods

QWidget* CStandardClientAreaProviderComp::GetWindow()
{
	if (m_clientGuiCompPtr.IsValid()){
		QWidget* widgetPtr = m_clientGuiCompPtr->GetWidget();
		if (widgetPtr != nullptr){
			return widgetPtr->window();
		}
	}

	return nullptr;
}


void CStandardClientAreaProviderComp::SetRect(const QRect& rect)
{
	QPoint anchor;
	QSize size;
	switch (m_corner){
	case Qt::TopLeftCorner:
		anchor = rect.topLeft();
		size.setWidth(rect.width());
		size.setHeight(rect.height());
		break;
	case Qt::TopRightCorner:
		anchor = rect.topRight();
		size.setWidth(-rect.width());
		size.setHeight(rect.height());
		break;
	case Qt::BottomLeftCorner:
		anchor = rect.bottomLeft();
		size.setWidth(rect.width());
		size.setHeight(-rect.height());
		break;
	case Qt::BottomRightCorner:
		anchor = rect.bottomRight();
		size.setWidth(-rect.width());
		size.setHeight(-rect.height());
		break;
	}

	if (m_anchor != anchor || m_size != size){
		istd::CChangeNotifier notifier(this);

		m_anchor = anchor;
		m_size = size;
	}
}


void CStandardClientAreaProviderComp::SetCorner(Qt::Corner corner)
{
	if (m_corner != corner){
		istd::CChangeNotifier notifier(this);

		m_corner = corner;

		QPoint pnt = m_anchor;
		if (m_size.width() < 0){
			pnt.rx() += m_size.width() + 1;
		}

		if (m_size.height() < 0){
			pnt.ry() += m_size.height() + 1;
		}

		QSize size;
		size.setWidth(qAbs(m_size.width()));
		size.setHeight(qAbs(m_size.height()));

		QRect rect(pnt, size);
		SetRect(rect);
	}
}


} // namespace imtgui


