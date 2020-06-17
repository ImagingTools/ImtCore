#include <imtloggui/CGeneralEventFactoryComp.h>


// ImtCore includes
#include <imtloggui/CIconBasedEventItem.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IEventItemFactory)

CEventItemBase* CGeneralEventFactoryComp::CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const
{
	if (!IsSupportedMessageId(message->GetInformationId())){
		return CreateInstanceWithSlaveFactory(message);
	}

	QIcon icon;
	int iconSize = 24;
	
	switch (message->GetInformationCategory()){
	case istd::IInformationProvider::IC_NONE:
		icon = QIcon(":/Icons/StateUnknown");
		break;
	case istd::IInformationProvider::IC_INFO:
		icon = QIcon(":/Icons/StateOk");
		break;
	case istd::IInformationProvider::IC_WARNING:
		icon = QIcon(":/Icons/StateWarning");
		break;
	case istd::IInformationProvider::IC_ERROR:
		icon = QIcon(":/Icons/Error");
		break;
	case istd::IInformationProvider::IC_CRITICAL:
		icon = QIcon(":/Icons/StateInvalid");
		break;
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


