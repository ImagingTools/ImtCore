#include <imtloggui/CRepresentationItemsFactoryComp.h>


// ACF includes
#include <ilog/CMessageContainer.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IRepresentationFactory)

IRepresentationFactory::RepresentationObjectPtr CRepresentationItemsFactoryComp::CreateRepresentationObject(
			const ilog::IMessageContainer::Messages& messages,
			const TimeRange& timeRange) const
{
	ilog::CMessageContainer* messageContainerPtr = new ilog::CMessageContainer();
	RepresentationObjectPtr retVal(messageContainerPtr);

	for (int i = 0; i < messages.count(); i++){
		QDateTime timestamp = messages[i]->GetInformationTimeStamp();
		if (timeRange.beginTime <= timestamp && timestamp <= timeRange.endTime){
			messageContainerPtr->AddMessage(messages[i]);
		}
	}

	return retVal;
}


} // namespace imtloggui


