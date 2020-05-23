#include <imtloggui/CIconBasedEventFactoryComp.h>


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QDebug>
#include <QtGui/QPainter>

// ImtCore includes
#include <imtlog/IMessageGroupInfoProvider.h>
#include <imtloggui/CIconBasedEventItem.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IEventItemFactory)

QGraphicsItem* CIconBasedEventFactoryComp::CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const
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
	
	if (m_iconAttrPtr.IsValid()){
		icon = QIcon(*m_iconAttrPtr);
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


