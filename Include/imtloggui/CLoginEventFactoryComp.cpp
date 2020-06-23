#include <imtloggui/CLoginEventFactoryComp.h>


// ImtCore includes
#include <imtloggui/CLoginEventItem.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IEventItemFactory)

CEventItemBase* CLoginEventFactoryComp::CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const
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

	CLoginEventItem* eventPtr = new CLoginEventItem(message);
	eventPtr->SetIcons(loginIcon, logoutIcon);
	eventPtr->SetIconSize(QSize(iconSize, iconSize));

	CLoginEventItem::MetaInfoItem metaInfoItem;
	metaInfoItem.key = QObject::tr("User name");
	metaInfoItem.value = message->GetInformationDescription();
	eventPtr->AddMetaInfo(metaInfoItem);

	return eventPtr;
}


} // namespace imtloggui


