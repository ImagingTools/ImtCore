#include <imtloggui/CIconBasedEventFactoryComp.h>


// Acf includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtloggui/CIconBasedEventItem.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IEventItemFactory)

IEventItem* CIconBasedEventFactoryComp::CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const
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

	CIconBasedEventItem* itemPtr = new imod::TModelWrap<CIconBasedEventItem>();
	itemPtr->SetParams(icon, QSize(iconSize, iconSize), message);

	return itemPtr;
}


} // namespace imtloggui


