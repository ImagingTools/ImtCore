#include <imtloggui/CGeneralEventFactoryComp.h>

// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtloggui/CIconBasedEventItem.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IEventItemFactory)

IEventItem* CGeneralEventFactoryComp::CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const
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

	CIconBasedEventItem* itemPtr = new imod::TModelWrap<CIconBasedEventItem>();
	itemPtr->SetParams(icon, QSize(iconSize, iconSize), message);

	return itemPtr;
}


} // namespace imtloggui


