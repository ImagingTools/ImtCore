#include <imtloggui/CGeneralEventFactoryComp.h>

// Acf includes
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

	static QIcon noneIcon = QIcon(":/Icons/StateUnknown");
	static QIcon okIcon(":/Icons/StateOk");
	static QIcon warningIcon(":/Icons/StateWarning");
	static QIcon errorIcon(":/Icons/Error");
	static QIcon invalidIcon(":/Icons/StateInvalid");

	switch (message->GetInformationCategory()){
	case istd::IInformationProvider::IC_NONE:
		icon = noneIcon;
		break;
	case istd::IInformationProvider::IC_INFO:
		icon = okIcon;
		break;
	case istd::IInformationProvider::IC_WARNING:
		icon = warningIcon;
		break;
	case istd::IInformationProvider::IC_ERROR:
		icon = errorIcon;
		break;
	case istd::IInformationProvider::IC_CRITICAL:
		icon = invalidIcon;
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


