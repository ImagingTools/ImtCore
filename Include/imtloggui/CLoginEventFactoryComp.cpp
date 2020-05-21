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
		if (m_slaveEventFactoryCompPtr.IsValid()){
			return m_slaveEventFactoryCompPtr->CreateInstance(message);
		}

		return nullptr;
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

	return eventPtr;
}


} // namespace imtloggui


