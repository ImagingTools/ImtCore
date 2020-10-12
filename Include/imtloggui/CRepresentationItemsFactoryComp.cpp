#include <imtloggui/CRepresentationItemsFactoryComp.h>


#include <qdebug>

// ACF includes
#include <ilog/CMessageContainer.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IRepresentationFactory)

IRepresentationFactory::RepresentationObjectPtr CRepresentationItemsFactoryComp::CreateRepresentationObject(
			const imtlog::CTimeRange& timeRange) const
{
	ilog::CMessageContainer* messageContainerPtr = new ilog::CMessageContainer();
	RepresentationObjectPtr retVal(messageContainerPtr);

	if (m_timeRangeFilterCompPtr.IsValid()){
		m_timeRangeFilterCompPtr->SetEventTimeRangeFilter(timeRange);
		if (m_messageContainerCompPtr.IsValid()){
			QTime time;
			time.start();
			ilog::IMessageContainer::Messages messages = m_messageContainerCompPtr->GetMessages();
			qDebug() << "Get messages" << time.elapsed() << messages.count();
			for (ilog::IMessageConsumer::MessagePtr message : messages){
				messageContainerPtr->AddMessage(message);
			}
		}
	}


	return retVal;
}


} // namespace imtloggui


