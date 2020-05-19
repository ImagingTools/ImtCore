#include <imtloggui/CLoginEventFactoryComp.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtGui/QPainter>

// ImtCore includes
#include <imtlog/IMessageGroupInfoProvider.h>
#include <imtloggui/CLoginEventItem.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IEventItemFactory)
QGraphicsItem* CLoginEventFactoryComp::CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const
{
	if (message->GetInformationId() != imtlog::IMessageGroupInfoProvider::MI_USER_LOGIN && message->GetInformationId() != imtlog::IMessageGroupInfoProvider::MI_USER_LOGOUT){
		return nullptr;
	}

	QIcon loginIcon;
	QIcon logoutIcon;
	int iconSize = 24;
	
	if (m_loginIcon.IsValid()){
		loginIcon = QIcon(*m_loginIcon);
	}

	if (m_logoutIcon.IsValid()){
		logoutIcon = QIcon(*m_logoutIcon);
	}

	if (m_iconSize.IsValid()){
		iconSize = *m_iconSize;
	}

	CLoginEventItem* eventPtr = new CLoginEventItem(message);
	eventPtr->SetIcons(loginIcon, logoutIcon);
	eventPtr->SetIconSize(QSize(iconSize, iconSize));

	return eventPtr;
}


} // namespace imtloggui


