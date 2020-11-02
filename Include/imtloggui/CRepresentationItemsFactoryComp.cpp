#include <imtloggui/CRepresentationItemsFactoryComp.h>


// ACF includes
#include <ilog/CMessageContainer.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IRepresentationFactory)

IRepresentationFactory::RepresentationObjectPtr CRepresentationItemsFactoryComp::CreateRepresentationObject(
			const imtlog::CTimeRange& timeRange,
			const QList<int>& messageIdList,
			imtlog::IEventMessageIdFilter::Mode mode) const
{
	ilog::CMessageContainer* messageContainerPtr = new ilog::CMessageContainer();
	RepresentationObjectPtr retVal(messageContainerPtr);

	if (m_timeRangeFilterCompPtr.IsValid()){
		m_timeRangeFilterCompPtr->SetEventTimeRangeFilter(timeRange);
		if (m_messageIdFilterCompPtr.IsValid()){
			m_messageIdFilterCompPtr->SetEventMessageIdFilterMode(mode);
			m_messageIdFilterCompPtr->SetEventMessageIdFilter(messageIdList);
			if (m_messageContainerCompPtr.IsValid()){
				ilog::IMessageContainer::Messages messages = m_messageContainerCompPtr->GetMessages();
				for (ilog::IMessageConsumer::MessagePtr message : messages){
					messageContainerPtr->AddMessage(message);
				}
			}
		}
	}


	return retVal;
}


} // namespace imtloggui


