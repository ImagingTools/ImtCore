#include <imtgui/CApplicationChangedEventFactoryComp.h>


// ImtCore includes
#include <imtgui/CApplicationChangedEvent.h>
#include <imtgui/CApplicationChangedEventItem.h>


namespace imtgui
{


// public methods

// reimplemented (imtloggui::IEventItemFactory)

imtloggui::CEventItemBase* CApplicationChangedEventFactoryComp::CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const
{
	if (!IsSupportedMessageId(message->GetInformationId())){
		return CreateInstanceWithSlaveFactory(message);
	}

	CApplicationChangedEventItem* eventPtr = new CApplicationChangedEventItem(message);

	const CApplicationChangedEvent* applicationChangedEventPtr = dynamic_cast<const CApplicationChangedEvent*>(message.GetPtr());
	if (applicationChangedEventPtr != nullptr){
		QIcon icon;
		int iconSize = 24;

		if (m_iconAttrPtr.IsValid()){
			icon = QIcon(*m_iconAttrPtr);
		}

		if (m_iconSizeAttrPtr.IsValid()){
			iconSize = *m_iconSizeAttrPtr;
		}

		eventPtr->SetIcon(icon);
		eventPtr->SetIconSize(QSize(iconSize, iconSize));

		CApplicationChangedEvent::ApplicationChangesInfo info = applicationChangedEventPtr->GetApplicationChangesInfo();
		
		eventPtr->setToolTip(eventPtr->toolTip()
					+ "\n" + QObject::tr("Application name") + ": " + info.applicationName
					+ "\n" + QObject::tr("User name") + ": " + info.userName);
		eventPtr->AddMetaInfo(QObject::tr("Application name"), info.applicationName);
		eventPtr->AddMetaInfo(QObject::tr("User name"), info.userName);
	}

	return eventPtr;
}


} // namespace imtgui


