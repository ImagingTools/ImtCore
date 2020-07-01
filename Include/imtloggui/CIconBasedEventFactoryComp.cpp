#include <imtloggui/CIconBasedEventFactoryComp.h>


// ImtCore includes
#include <imtloggui/CIconBasedEventItem.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IEventItemFactory)

CEventItemBase* CIconBasedEventFactoryComp::CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const
{
	if (!IsSupportedMessageId(message->GetInformationId())){
		return CreateInstanceWithSlaveFactory(message);
	}

	QString status;
	QIcon icon;
	int iconSize = 24;
	
	switch (message->GetInformationCategory()){
	case istd::IInformationProvider::IC_NONE:
		status = QObject::tr("UNKNOWN");
		break;
	case istd::IInformationProvider::IC_INFO:
		status = QObject::tr("OK");
		break;
	case istd::IInformationProvider::IC_WARNING:
		status = QObject::tr("WARNING");
		break;
	case istd::IInformationProvider::IC_ERROR:
		status = QObject::tr("ERROR");
		break;
	case istd::IInformationProvider::IC_CRITICAL:
		status = QObject::tr("CRITICAL");
		break;
	}

	if (m_iconAttrPtr.IsValid()){
		icon = QIcon(*m_iconAttrPtr);
	}

	if (m_iconSizeAttrPtr.IsValid()){
		iconSize = *m_iconSizeAttrPtr;
	}

	CIconBasedEventItem* eventPtr = new CIconBasedEventItem(message);
	eventPtr->SetIcon(icon);
	eventPtr->SetIconSize(QSize(iconSize, iconSize));
  	eventPtr->setToolTip(
				QObject::tr("Source") + ": " + message->GetInformationSource()
				+ QObject::tr("\nMessage") + ": " + message->GetInformationDescription()
				+ QObject::tr("\nStatus") + ": " + status);
	eventPtr->AddMetaInfo(QObject::tr("Source"), message->GetInformationSource());
	eventPtr->AddMetaInfo(QObject::tr("Message"), message->GetInformationDescription());
	eventPtr->AddMetaInfo(QObject::tr("Status"), status);

	return eventPtr;
}


} // namespace imtloggui


