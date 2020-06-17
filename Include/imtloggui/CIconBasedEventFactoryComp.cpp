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

	QIcon icon;
	int iconSize = 24;
	
	if (m_iconAttrPtr.IsValid()){
		icon = QIcon(*m_iconAttrPtr);
	}

	if (m_iconSizeAttrPtr.IsValid()){
		iconSize = *m_iconSizeAttrPtr;
	}

	CIconBasedEventItem* eventPtr = new CIconBasedEventItem(message);
	eventPtr->SetIcon(icon);
	eventPtr->SetIconSize(QSize(iconSize, iconSize));
	eventPtr->setToolTip(QObject::tr("Source" )+ ": "  + message->GetInformationSource() + "\n" + QObject::tr("Message" ) + ": " + message->GetInformationDescription());

	return eventPtr;
}


} // namespace imtloggui


