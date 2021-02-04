#include <imtloggui/CLoginEventFactoryComp.h>


// Acf includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtlog/CLoginEvent.h>
#include <imtloggui/CLoginEventItem.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IEventItemFactory)

IEventItem* CLoginEventFactoryComp::CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const
{
	if (!IsSupportedMessageId(message->GetInformationId())){
		return CreateInstanceWithSlaveFactory(message);
	}

	QIcon loginIcon;
	QIcon logoutIcon;
	int iconSize = 24;
	
	if (m_loginIconAttrPtr.IsValid()){
		loginIcon = QIcon(*m_loginIconAttrPtr);
	}

	if (m_logoutIconAttrPtr.IsValid()){
		logoutIcon = QIcon(*m_logoutIconAttrPtr);
	}

	if (m_iconSizeAttrPtr.IsValid()){
		iconSize = *m_iconSizeAttrPtr;
	}

	const imtlog::CLoginEvent* eventPtr = dynamic_cast<const imtlog::CLoginEvent*>(message.GetPtr());
	if (eventPtr != nullptr){
		CLoginEventItem* itemPtr = new imod::TModelWrap<CLoginEventItem>();
		itemPtr->SetParams(
			loginIcon,
			logoutIcon,
			QSize(iconSize, iconSize),
			message);

		return itemPtr;
	}

	return nullptr;
}


} // namespace imtloggui


