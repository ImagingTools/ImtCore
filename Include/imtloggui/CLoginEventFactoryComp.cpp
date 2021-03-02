#include <imtloggui/CLoginEventFactoryComp.h>


// Qt includes
#include <QtGui/QIcon>

// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtlog/CLoginEvent.h>
#include <imtloggui/CLoginEventItem.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IEventItemFactory)

IEventItem* CLoginEventFactoryComp::CreateInstance(const ilog::IMessageConsumer::MessagePtr& messagePtr) const
{
	if (!IsSupportedMessageId(messagePtr->GetInformationId())){
		return CreateInstanceWithSlaveFactory(messagePtr);
	}

	int iconSize = 24;

	if (m_iconSizeAttrPtr.IsValid()){
		iconSize = *m_iconSizeAttrPtr;
	}

	if (m_loginIcon.isNull()){
		if (m_loginIconAttrPtr.IsValid()){
			m_loginIcon = QIcon(*m_loginIconAttrPtr).pixmap(iconSize);
		}
	}

	if (m_logoutIcon.isNull()){
		if (m_logoutIconAttrPtr.IsValid()){
			m_logoutIcon = QIcon(*m_logoutIconAttrPtr).pixmap(iconSize);
		}
	}

	const imtlog::CLoginEvent* eventPtr = dynamic_cast<const imtlog::CLoginEvent*>(messagePtr.GetPtr());
	if (eventPtr != nullptr){
		CLoginEventItem* itemPtr = new imod::TModelWrap<CLoginEventItem>();
		itemPtr->SetParams(
					m_loginIcon,
					m_logoutIcon,
					messagePtr);

		return itemPtr;
	}

	return nullptr;
}


} // namespace imtloggui


