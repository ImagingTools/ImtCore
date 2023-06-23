 #include <imtgui/CPopupControllerComp.h>


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

CPopupControllerComp::CPopupControllerComp()
	:m_clientAreaObserver(*this),
	m_mouseOnPopup(false)
{
}


// reimplemented (IPopupController)

QByteArray CPopupControllerComp::AddPopup(const MessagePtr& messagePtr, int timeout, bool isClosingOnMouseClickAllowed, QWidget* contentWidgetPtr)
{
	return CreatePopupItem(messagePtr, timeout, isClosingOnMouseClickAllowed, contentWidgetPtr);
}


bool CPopupControllerComp::ClosePopup(const QByteArray& popupId)
{
	int index = GetVisibleItemIndex(popupId);
	if (index >= 0){
		QByteArray id = m_visibleItems[index]->id;

		m_visibleItems[index]->widgetPtr->hide();
		m_visibleItems.removeAt(index);

		if (m_popupEventHandlerCompPtr.IsValid()){
			for (int i = 0; i < m_popupEventHandlerCompPtr.GetCount(); i++){
				m_popupEventHandlerCompPtr[i]->OnPopupClosed(id);
			}
		}

		ShowItems();
		ArrangeVisibleItems();

		return true;
	}

	return false;
}


// reimplemented (ilog::IMessageConsumer)

bool CPopupControllerComp::IsMessageSupported(
			int /*messageCategory*/,
			int /*messageId*/,
			const istd::IInformationProvider* /*messagePtr*/) const
{
	return true;
}


void CPopupControllerComp::AddMessage(const MessagePtr& messagePtr)
{
	CreatePopupItem(messagePtr, *m_defaultTimeoutAttrPtr, true, nullptr);
}


// reimplemented (icomp::CComponentBase)

void CPopupControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_clientAreaProviderCompPtr.IsValid()){
		m_clientAreaObserver.RegisterObject(m_clientAreaProviderCompPtr.GetPtr(), &CPopupControllerComp::OnClientAreaUpdated);
	}
	else{
		CalculatePopupAreaFromScreen();
	}
}


void CPopupControllerComp::OnComponentDestroyed()
{
	m_clientAreaObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private slots

void CPopupControllerComp::OnPrimaryScreenChanged()
{
	if (!m_clientAreaProviderCompPtr.IsValid()){
		CalculatePopupAreaFromScreen();
	}
}


void CPopupControllerComp::OnScreenGeometryChanged(const QRect& /*geometry*/)
{
	if (!m_clientAreaProviderCompPtr.IsValid()){
		CalculatePopupAreaFromScreen();
	}
}


void CPopupControllerComp::OnClientAreaUpdated(const istd::IChangeable::ChangeSet& /*changeSet*/, const IClientAreaProvider* objectPtr)
{
	m_anchor = objectPtr->GetAnchor();
	m_size = objectPtr->GetSize();

	ValidateVisibleItems();
	ShowItems();
	ArrangeVisibleItems();
}


void CPopupControllerComp::OnHoverEnter(QEvent* /*event*/)
{
	for (PopupItemPtr& itemPtr : m_visibleItems){
		itemPtr->timeoutTimer.stop();
		itemPtr->fadeAnimation.stop();
		itemPtr->effect.setOpacity(1);
	}

	m_mouseOnPopup = true;
}


void CPopupControllerComp::OnHoverLeave(QEvent* /*event*/)
{
	for (PopupItemPtr& itemPtr : m_visibleItems){
		if (itemPtr->timeout > 0){
			itemPtr->timeoutTimer.start(itemPtr->timeout);
		}
	}

	m_mouseOnPopup = false;
}


void CPopupControllerComp::OnCloseRequest()
{
	int index = GetVisibleItemIndex(dynamic_cast<QWidget*>(sender()));
	if (index >= 0){
		ClosePopup(m_visibleItems[index]->id);
	}
}


void CPopupControllerComp::OnPopupTimeout()
{
	int index = GetVisibleItemIndex(sender());
	if (index >= 0){
		m_visibleItems[index]->fadeAnimation.setDirection(QAbstractAnimation::Backward);
		m_visibleItems[index]->fadeAnimation.start();
	}
}


void CPopupControllerComp::OnFadeFinished()
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

void CPopupControllerComp::CalculatePopupAreaFromScreen()
{
	QGuiApplication* appPtr= dynamic_cast<QGuiApplication*>(qApp);
	if (appPtr != nullptr){
		QScreen* screenPtr = appPtr->primaryScreen();
		if (screenPtr != nullptr){
			QRect rect = screenPtr->geometry();
			m_anchor = rect.bottomRight();
			m_size.setWidth(-rect.width() * *m_xRatioAttrPtr);
			m_size.setHeight(-rect.height() * *m_yRatioAttrPtr);

			return;
		}
	}

	m_size = QSize();
}


QByteArray CPopupControllerComp::CreatePopupItem(const MessagePtr& messagePtr, int timeout, bool isClosingOnMouseClickAllowed, QWidget* contentWidgetPtr)
{
	istd::TDelPtr<IPopupWidget> popupWidgetPtr;
	if (m_popupWidgetFactoryCompPtr.IsValid()){
		popupWidgetPtr.SetPtr(m_popupWidgetFactoryCompPtr->CreateInstance());

	}
	else{
		popupWidgetPtr.SetPtr(new CStandardPopupWidget());
	}

	CPopupWidgetBase* widgetPtr = dynamic_cast<CPopupWidgetBase*>(popupWidgetPtr.GetPtr());
	if (widgetPtr != nullptr){
		widgetPtr->AllowClosingOnMouseClick(isClosingOnMouseClickAllowed);
		popupWidgetPtr->SetContent(messagePtr, contentWidgetPtr);
		popupWidgetPtr.PopPtr();

		PopupItemPtr itemPtr(new PopupItem());
		itemPtr->id = QUuid::createUuid().toByteArray();
		itemPtr->widgetPtr = widgetPtr;
		itemPtr->widgetPtr->setGraphicsEffect(&itemPtr->effect);
		itemPtr->timeout = timeout;

		connect(widgetPtr, &CPopupWidgetBase::EmitHoverEnter, this, &CPopupControllerComp::OnHoverEnter);
		connect(widgetPtr, &CPopupWidgetBase::EmitHoverLeave, this, &CPopupControllerComp::OnHoverLeave);
		connect(widgetPtr, &CPopupWidgetBase::EmitCloseRequest, this, &CPopupControllerComp::OnCloseRequest);
		connect(&itemPtr->timeoutTimer, &QTimer::timeout, this, &CPopupControllerComp::OnPopupTimeout);
		connect(&itemPtr->fadeAnimation, &QAbstractAnimation::finished, this, &CPopupControllerComp::OnFadeFinished);

		m_items.append(itemPtr);

		ShowItems();
		ArrangeVisibleItems();

		return itemPtr->id;
	}

	return QByteArray();
}


void CPopupControllerComp::RemovePopupItem(int /*index*/)
{
}


int CPopupControllerComp::GetVisibleItemIndex(const QByteArray& id)
{
	for (int i = 0; i < m_visibleItems.count(); i++){
		if (m_visibleItems[i]->id == id){
			return i;
		}
	}

	return -1;
}


int CPopupControllerComp::GetVisibleItemIndex(const QObject* memberPtr)
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


void CPopupControllerComp::ValidateVisibleItems()
{
	int totalHeight = - *m_spacingAttrPtr;
	for (int visibleIndex = 0; visibleIndex < m_visibleItems.count(); visibleIndex++){
		totalHeight += *m_spacingAttrPtr + m_visibleItems[visibleIndex]->widgetPtr->height();
		if (totalHeight > qAbs(m_size.height())){
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


void CPopupControllerComp::ArrangeVisibleItems()
{
	QPoint refPos = m_anchor;
	for (PopupItemPtr itemPtr : m_visibleItems){
		QPoint itemPos = refPos;
		if (m_size.width() < 0){
			itemPos.rx() -= itemPtr->widgetPtr->width();
		}

		if (m_size.height() < 0){
			itemPos.ry() -= itemPtr->widgetPtr->height();
		}

		itemPtr->widgetPtr->move(itemPos);

		if (m_size.height() >= 0){
			refPos.ry() += itemPtr->widgetPtr->height() + *m_spacingAttrPtr;
		}
		else{
			refPos.ry() -= itemPtr->widgetPtr->height() + *m_spacingAttrPtr;
		}
	}
}


void CPopupControllerComp::ShowItems()
{
	int totalHeight = -*m_spacingAttrPtr;
	for (PopupItemPtr itemPtr : m_visibleItems){
		totalHeight += *m_spacingAttrPtr + itemPtr->widgetPtr->height();
	}

	while (!m_items.isEmpty() && (totalHeight + *m_spacingAttrPtr + m_items.first()->widgetPtr->height() < qAbs(m_size.height()))){
		PopupItemPtr itemPtr = m_items.first();
		totalHeight += *m_spacingAttrPtr + m_items.first()->widgetPtr->height();

		m_items.removeFirst();
		m_visibleItems.append(itemPtr);

		itemPtr->effect.setOpacity(0);
		itemPtr->fadeAnimation.setDirection(QAbstractAnimation::Forward);
		itemPtr->fadeAnimation.start();
		itemPtr->widgetPtr->show();
	}
}


} // namespace imtgui


