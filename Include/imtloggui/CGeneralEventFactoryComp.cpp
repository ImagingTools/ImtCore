#include <imtloggui/CGeneralEventFactoryComp.h>


// Qt includes
#include <QtCore/QObject>
#include <QtGui/QPainter>

// ImtCore includes
#include <imtlog/IMessageGroupInfoProvider.h>
#include <imtloggui/CIconBasedEventItem.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IEventItemFactory)

CEventItemBase* CGeneralEventFactoryComp::CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const
{
	bool isIdFounded = false;

	if (m_idsAttrPtr.GetCount() == 0){
		isIdFounded  = true;
	}
	else {
		for (int i = 0; i < m_idsAttrPtr.GetCount(); i++){
			if (message->GetInformationId() == m_idsAttrPtr[i]){
				isIdFounded = true;
				break;
			}
		}
	}

	if (!isIdFounded){
		if (m_slaveEventFactoryCompPtr.IsValid()){
			return m_slaveEventFactoryCompPtr->CreateInstance(message);
		}
		else{
			return nullptr;
		}
	}

	QIcon icon;
	int iconSize = 24;
	
	switch (message->GetInformationCategory()){
	case istd::IInformationProvider::IC_NONE:
		icon = QIcon(":/Icons/StateUnknown");
		break;
	case istd::IInformationProvider::IC_INFO:
		icon = QIcon(":/Icons/StateOk");
		break;
	case istd::IInformationProvider::IC_WARNING:
		icon = QIcon(":/Icons/Warning");
		break;
	case istd::IInformationProvider::IC_ERROR:
		icon = QIcon(":/Icons/Error");
		break;
	case istd::IInformationProvider::IC_CRITICAL:
		icon = QIcon(":/Icons/StateInvalid");
		break;
	}

	if (m_iconSizeAttrPtr.IsValid()){
		iconSize = *m_iconSizeAttrPtr;
	}

	CIconBasedEventItem* eventPtr = new CIconBasedEventItem(message);
	eventPtr->SetIcon(icon);
	eventPtr->SetIconSize(QSize(iconSize, iconSize));
	eventPtr->setToolTip(QObject::tr("Source" )+ ": "  + message->GetInformationSource() + "\n" + QObject::tr("Message" ) + ": " + message->GetInformationDescription());

	return eventPtr;
}


} // namespace imtloggui


