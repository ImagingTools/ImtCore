 #include <imtgui/CPopupController.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QRect>
#include <QtCore/QUuid>
#include <QtGui/QGuiApplication>
#include <QtWidgets/QLayout>
#include <QtGui/QScreen>

// ImtCore includes
#include <imtgui/CStandardPopupWidget.h>


namespace imtgui
{


// public methods

CPopupController::CPopupController()
	:m_enabled(false),
	m_corner(Qt::BottomRightCorner),
	m_parentPtr(nullptr),
	m_screenPtr(nullptr),
	m_ratioX(0),
	m_ratioY(0),
	m_spacing(5),
	m_timeout(5000),
	m_widgetFactoryPtr(nullptr),
	m_mouseOnPopup(false)
{

	qRegisterMetaType<MessagePtr>("MessagePtr");
	connect(this, &CPopupController::EmitAddMessage, this, &CPopupController::OnAddMessage, Qt::QueuedConnection);
}


void CPopupController::SetAnchorCorner(Qt::Corner corner)
{
	m_corner = corner;
	ArrangeVisibleItems();
}


void CPopupController::SetParentWidget(QWidget* parentPtr)
{
	if (m_parentPtr != nullptr){
		m_parentPtr->removeEventFilter(this);
	}

	m_screenPtr = nullptr;
	m_parentPtr = parentPtr;

	if (m_parentPtr == nullptr){
		for (PopupItemPtr itemPtr : m_items){
			itemPtr->widgetPtr->setParent(nullptr);
		}

		for (PopupItemPtr itemPtr : m_visibleItems){
			itemPtr->widgetPtr->setParent(nullptr);
		}
	}
	else{
		m_parentPtr->installEventFilter(this);
	}

	SetEnabled(true);
}

void CPopupController::SetScreen(QScreen* screenPtr)
{
	m_parentPtr = nullptr;
	m_screenPtr = screenPtr;

	SetEnabled(true);
}


void CPopupController::SetRatioX(double ratio)
{
	m_ratioX = ratio;
	ArrangeVisibleItems();
}


void CPopupController::SetRatioY(double ratio)
{
	m_ratioY = ratio;
	ArrangeVisibleItems();
}


void CPopupController::SetSpacing(double spacing)
{
	m_spacing = spacing;
	ValidateVisibleItems();
	ArrangeVisibleItems();
}


void CPopupController::SetDefaultTimeOut(int msec)
{
	m_timeout = msec;
}


void CPopupController::SetPopupWidgetFactory(IPopupWidgetFactory* factoryPtr)
{
	m_widgetFactoryPtr = factoryPtr;
}


QByteArray CPopupController::AddPopupEvenHandler(IPopupEventHandler* /*handlerPtr*/)
{
	return QByteArray();
}


QByteArray CPopupController::RemovePopupEvenHandler(QByteArray)
{
	return QByteArray();
}


void CPopupController::SetEnabled(bool /*enable*/)
{
}


// reimplemented (IPopupController)

QByteArray CPopupController::AddPopup(const MessagePtr& messagePtr, int timeout, bool isClosingOnMouseClickAllowed, QWidget* contentWidgetPtr)
{
	return CreatePopupItem(messagePtr, timeout, isClosingOnMouseClickAllowed, contentWidgetPtr);
}


bool CPopupController::ClosePopup(const QByteArray& popupId)
{
	int index = GetVisibleItemIndex(popupId);
	if (index >= 0){
		QByteArray id = m_visibleItems[index]->id;

		m_visibleItems[index]->widgetPtr->hide();
		m_visibleItems.removeAt(index);

		for (IPopupEventHandler* handlerPtr : m_handlers){
			handlerPtr->OnPopupClosed(id);
		}

		ShowItems();
		ArrangeVisibleItems();

		return true;
	}

	return false;
}


// reimplemented (ilog::IMessageConsumer)

bool CPopupController::IsMessageSupported(
			int /*messageCategory*/,
			int /*messageId*/,
			const istd::IInformationProvider* /*messagePtr*/) const
{
	return true;
}


void CPopupController::AddMessage(const MessagePtr& messagePtr)
{
	Q_EMIT EmitAddMessage(messagePtr);
}


// reimplemented (icomp::CComponentBase)

// private slots

void CPopupController::OnAddMessage(const MessagePtr& messagePtr)
{
	CreatePopupItem(messagePtr, m_timeout, true, nullptr);
}


void CPopupController::OnScreenGeometryChanged(const QRect& /*rect*/)
{
}


void CPopupController::OnHoverEnter(QEvent* /*event*/)
{
	for (PopupItemPtr& itemPtr : m_visibleItems){
		itemPtr->timeoutTimer.stop();
		itemPtr->fadeAnimation.stop();
		itemPtr->effect.setOpacity(1);
	}

	m_mouseOnPopup = true;
}


void CPopupController::OnHoverLeave(QEvent* /*event*/)
{
	for (PopupItemPtr& itemPtr : m_visibleItems){
		if (itemPtr->timeout > 0){
			itemPtr->timeoutTimer.start(itemPtr->timeout);
		}
	}

	m_mouseOnPopup = false;
}


void CPopupController::OnCloseRequest()
{
	int index = GetVisibleItemIndex(dynamic_cast<QWidget*>(sender()));
	if (index >= 0){
		ClosePopup(m_visibleItems[index]->id);
	}
}


void CPopupController::OnPopupTimeout()
{
	int index = GetVisibleItemIndex(sender());
	if (index >= 0){
		m_visibleItems[index]->fadeAnimation.setDirection(QAbstractAnimation::Backward);
		m_visibleItems[index]->fadeAnimation.start();
	}
}


void CPopupController::OnFadeFinished()
{
	int index = GetVisibleItemIndex(sender());
	if (index >= 0){
		if (m_visibleItems[index]->effect.opacity() == 1){
			if (m_visibleItems[index]->timeout > 0){
				m_visibleItems[index]->timeoutTimer.start(m_visibleItems[index]->timeout);
			}
		}

		if (m_visibleItems[index]->effect.opacity() == 0){
			m_visibleItems[index]->widgetPtr->hide();
			m_visibleItems.removeAt(index);
			ShowItems();
			ArrangeVisibleItems();
		}
	}
}


// private methods

QByteArray CPopupController::CreatePopupItem(const MessagePtr& messagePtr, int timeout, bool isClosingOnMouseClickAllowed, QWidget* contentWidgetPtr)
{
	istd::TDelPtr<IPopupWidget> popupWidgetPtr;
	if (m_widgetFactoryPtr != nullptr){
		popupWidgetPtr.SetPtr(m_widgetFactoryPtr->CreateInstance());
	}
	else{
		popupWidgetPtr.SetPtr(new CStandardPopupWidget());
	}

	CPopupWidgetBase* widgetPtr = dynamic_cast<CPopupWidgetBase*>(popupWidgetPtr.GetPtr());
	if (widgetPtr != nullptr){
		widgetPtr->setParent(m_parentPtr);
		widgetPtr->AllowClosingOnMouseClick(isClosingOnMouseClickAllowed);
		popupWidgetPtr->SetContent(messagePtr, contentWidgetPtr);
		popupWidgetPtr.PopPtr();

		PopupItemPtr itemPtr(new PopupItem());
		itemPtr->id = QUuid::createUuid().toByteArray();
		itemPtr->widgetPtr = widgetPtr;
		itemPtr->widgetPtr->setGraphicsEffect(&itemPtr->effect);
		itemPtr->timeout = timeout;

		connect(widgetPtr, &CPopupWidgetBase::EmitHoverEnter, this, &CPopupController::OnHoverEnter);
		connect(widgetPtr, &CPopupWidgetBase::EmitHoverLeave, this, &CPopupController::OnHoverLeave);
		connect(widgetPtr, &CPopupWidgetBase::EmitCloseRequest, this, &CPopupController::OnCloseRequest);
		connect(&itemPtr->timeoutTimer, &QTimer::timeout, this, &CPopupController::OnPopupTimeout);
		connect(&itemPtr->fadeAnimation, &QAbstractAnimation::finished, this, &CPopupController::OnFadeFinished);

		m_items.append(itemPtr);

		ShowItems();
		ArrangeVisibleItems();

		return itemPtr->id;
	}

	return QByteArray();
}


void CPopupController::RemovePopupItem(int /*index*/)
{

}


int CPopupController::GetVisibleItemIndex(const QByteArray& id)
{
	for (int i = 0; i < m_visibleItems.count(); i++){
		if (m_visibleItems[i]->id == id){
			return i;
		}
	}

	return -1;
}


int CPopupController::GetVisibleItemIndex(const QObject* memberPtr)
{
	for (int i = 0; i < m_visibleItems.count(); i++){
		if (m_visibleItems[i]->widgetPtr == memberPtr
					|| &m_visibleItems[i]->fadeAnimation == memberPtr
					|| &m_visibleItems[i]->timeoutTimer == memberPtr){
			return i;
		}
	}

	return -1;
}


void CPopupController::ValidateVisibleItems()
{
	int totalHeight = -m_spacing;
	for (int visibleIndex = 0; visibleIndex < m_visibleItems.count(); visibleIndex++){
		totalHeight += m_spacing + m_visibleItems[visibleIndex]->widgetPtr->height();
		if (totalHeight > qAbs(m_rect.height())){
			for (int removeIndex = m_visibleItems.count() - 1; removeIndex >= visibleIndex; removeIndex--){
				Q_ASSERT(removeIndex >= 0);
				m_visibleItems[removeIndex]->fadeAnimation.stop();
				m_visibleItems[removeIndex]->timeoutTimer.stop();
				m_visibleItems[removeIndex]->widgetPtr->hide();

				m_items.prepend(m_visibleItems[removeIndex]);
				m_visibleItems.removeAt(removeIndex);
			}

			break;
		}
	}
}


void CPopupController::ArrangeVisibleItems()
{
	QPoint refPos;

	QSize size;
	switch (m_corner){
	case Qt::TopLeftCorner:
		refPos = m_rect.topLeft();
		size.setWidth(m_rect.width());
		size.setHeight(m_rect.height());
		break;
	case Qt::TopRightCorner:
		refPos = m_rect.topRight();
		size.setWidth(-m_rect.width());
		size.setHeight(m_rect.height());
		break;
	case Qt::BottomLeftCorner:
		refPos = m_rect.bottomLeft();
		size.setWidth(m_rect.width());
		size.setHeight(-m_rect.height());
		break;
	case Qt::BottomRightCorner:
		refPos = m_rect.bottomRight();
		size.setWidth(-m_rect.width());
		size.setHeight(-m_rect.height());
		break;
	}

	for (PopupItemPtr itemPtr : m_visibleItems){
		QPoint itemPos = refPos;
		if (size.width() < 0){
			itemPos.rx() -= itemPtr->widgetPtr->width();
		}

		if (size.height() < 0){
			itemPos.ry() -= itemPtr->widgetPtr->height();
		}

		itemPtr->widgetPtr->move(itemPos);

		if (size.height() >= 0){
			refPos.ry() += itemPtr->widgetPtr->height() + m_spacing;
		}
		else{
			refPos.ry() -= itemPtr->widgetPtr->height() + m_spacing;
		}
	}
}


void CPopupController::ShowItems()
{
	int totalHeight = -m_spacing;
	for (PopupItemPtr itemPtr : m_visibleItems){
		totalHeight += m_spacing + itemPtr->widgetPtr->height();
	}

	while (!m_items.isEmpty() && (totalHeight + m_spacing + m_items.first()->widgetPtr->height() < qAbs(m_rect.height()))){
		PopupItemPtr itemPtr = m_items.first();
		totalHeight += m_spacing + m_items.first()->widgetPtr->height();

		m_items.removeFirst();
		m_visibleItems.append(itemPtr);

		itemPtr->effect.setOpacity(0);
		itemPtr->fadeAnimation.setDirection(QAbstractAnimation::Forward);
		itemPtr->fadeAnimation.start();
		itemPtr->widgetPtr->show();
	}
}


void CPopupController::HideItems()
{
}


bool CPopupController::eventFilter(QObject* /*watched*/, QEvent* event)
{
	if (event->type() == QEvent::Resize){
		QSize size = m_parentPtr->size();
		m_rect = QRect(0, 0, size.width(), size.height());
	}

	ValidateVisibleItems();
	ArrangeVisibleItems();

	return false;
}


} // namespace imtgui


