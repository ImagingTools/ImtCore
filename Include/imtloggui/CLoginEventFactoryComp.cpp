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

	switch (message->GetInformationId()){
	case imtlog::IMessageGroupInfoProvider::MI_USER_LOGIN:
		eventPtr->AddMetaInfo(QObject::tr("Action"), QObject::tr("Login"));
		break;
	case imtlog::IMessageGroupInfoProvider::MI_USER_LOGOUT:
		eventPtr->AddMetaInfo(QObject::tr("Action"), QObject::tr("Logout"));
		break;
	}
	eventPtr->AddMetaInfo(QObject::tr("User name"), message->GetInformationDescription());

	return eventPtr;
}


} // namespace imtloggui


