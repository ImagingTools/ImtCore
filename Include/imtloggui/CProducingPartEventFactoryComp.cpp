#include <imtloggui/CProducingPartEventFactoryComp.h>


// Acula includes
#include "aculaprod/CProducingPartEvent.h"


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IEventItemFactory)

CEventItemBase* CProducingPartEventFactoryComp::CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const
{
	CEventItemBase* eventItemPtr = BaseClass::CreateInstance(message);

	if (eventItemPtr != nullptr){
		const aculaprod::CProducingPartEvent* messagePtr = dynamic_cast<const aculaprod::CProducingPartEvent*>(message.GetPtr());
		if (messagePtr != nullptr){
			QString toolTip;
			toolTip += QObject::tr("Source") + ": " + message->GetInformationSource() + "\n" + QObject::tr("Message") + ": " + message->GetInformationDescription();
			toolTip += QObject::tr("\nApplication-ID") + ": " + messagePtr->GetProducingInfo().applicationId;
			toolTip += QObject::tr("\nApplication name") + ": " + messagePtr->GetProducingInfo().applicationName;
			toolTip += QObject::tr("\nPart serial number") + ": " + messagePtr->GetProducingInfo().serialNumber;
			toolTip += QObject::tr("\nResult ID") + ": " + messagePtr->GetProducingInfo().resultId;

			eventItemPtr->setToolTip(toolTip);
		}
	}

	return eventItemPtr;
}


} // namespace imtloggui


