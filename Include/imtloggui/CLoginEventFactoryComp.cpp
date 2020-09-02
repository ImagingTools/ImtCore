#include <imtloggui/CLoginEventFactoryComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
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

	CLoginEventItem* eventPtr = new imod::TModelWrap<CLoginEventItem>();
	eventPtr->SetParams(message);
	eventPtr->SetIcons(loginIcon, logoutIcon);
	eventPtr->SetIconSize(QSize(iconSize, iconSize));

	SetItemMetaInfo(eventPtr);

	return eventPtr;
}


// protected methods

void CLoginEventFactoryComp::SetItemMetaInfo(IEventItem* eventItem) const
{
	Q_ASSERT(eventItem != nullptr);

	BaseClass::SetItemMetaInfo(eventItem);

	eventItem->SetMetaInfo(CLoginEventItem::MIT_MESSAGE, QObject::tr("Message"));

	const istd::IInformationProvider* informationProviderPtr = eventItem->GetInformationProvider();
	Q_ASSERT(informationProviderPtr != nullptr);

	switch (informationProviderPtr->GetInformationId()){
	case imtlog::IMessageGroupInfoProvider::MI_USER_LOGIN:
		eventItem->SetMetaInfo(CLoginEventItem::MIT_ACTION, QObject::tr("Login"));
		break;
	case imtlog::IMessageGroupInfoProvider::MI_USER_LOGOUT:
		eventItem->SetMetaInfo(CLoginEventItem::MIT_ACTION, QObject::tr("Logout"));
		break;
	}

	eventItem->SetMetaInfo(CLoginEventItem::MIT_USER_NAME, informationProviderPtr->GetInformationDescription());
}


} // namespace imtloggui


