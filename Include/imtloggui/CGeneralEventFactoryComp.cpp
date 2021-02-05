#include <imtloggui/CGeneralEventFactoryComp.h>


// Qt includes
#include <QtGui/QIcon>

// Acf includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtloggui/CIconBasedEventItem.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IEventItemFactory)

IEventItem* CGeneralEventFactoryComp::CreateInstance(const ilog::IMessageConsumer::MessagePtr& messagePtr) const
{
	if (!IsSupportedMessageId(messagePtr->GetInformationId())){
		return CreateInstanceWithSlaveFactory(messagePtr);
	}

	int iconSize = 24;

	if (m_iconSizeAttrPtr.IsValid()){
		iconSize = *m_iconSizeAttrPtr;
	}

	QPixmap icon;

	switch (messagePtr->GetInformationCategory()){
	case istd::IInformationProvider::IC_NONE:
		if (m_noneIcon.isNull()){
			m_noneIcon = QIcon(":/Icons/StateUnknown").pixmap(iconSize);
		}

		icon = m_noneIcon;
		break;
	case istd::IInformationProvider::IC_INFO:
		if (m_okIcon.isNull()){
			m_okIcon = QIcon(":/Icons/StateOk").pixmap(iconSize);
		}

		icon = m_okIcon;
		break;
	case istd::IInformationProvider::IC_WARNING:
		if (m_warningIcon.isNull()){
			m_warningIcon = QIcon(":/Icons/StateWarning").pixmap(iconSize);
		}

		icon = m_warningIcon;
		break;
	case istd::IInformationProvider::IC_ERROR:
		if (m_errorIcon.isNull()){
			m_errorIcon = QIcon(":/Icons/Error").pixmap(iconSize);
		}

		icon = m_errorIcon;
		break;
	case istd::IInformationProvider::IC_CRITICAL:
		if (m_invalidIcon.isNull()){
			m_invalidIcon = QIcon(":/Icons/StateInvalid").pixmap(iconSize);
		}

		icon = m_invalidIcon;
		break;
	}


	CIconBasedEventItem* itemPtr = new imod::TModelWrap<CIconBasedEventItem>();
	itemPtr->SetParams(icon, messagePtr);

	return itemPtr;
}


} // namespace imtloggui


