#include <imtloggui/CRepresentationEventsFactoryComp.h>


// Acf includes
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtloggui/CRepresentationEventsObject.h>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IRepresentationFactory)

IRepresentationFactory::RepresentationObjectPtr CRepresentationEventsFactoryComp::CreateRepresentationObject(
			const imtlog::CTimeRange& timeRange,
			const QList<int>& messageIdList,
			imtlog::IEventMessageIdFilter::Mode mode) const
{
	CRepresentationEventsObject* retVal = new CRepresentationEventsObject();

	if (m_timeRangeFilterCompPtr.IsValid()){
		m_timeRangeFilterCompPtr->SetEventTimeRangeFilter(timeRange);
		if (m_messageIdFilterCompPtr.IsValid()){
			m_messageIdFilterCompPtr->SetEventMessageIdFilterMode(mode);
			m_messageIdFilterCompPtr->SetEventMessageIdFilter(messageIdList);
			if (m_messageContainerCompPtr.IsValid()){
				imtlog::IMessageHistoryContainer::Messages messages = m_messageContainerCompPtr->GetMessages();
				for (imtlog::IMessageHistoryConsumer::Message message : messages){
					retVal->append(message);
				}
			}
		}
	}

	return istd::TSmartPtr<istd::IChangeable>(retVal);
}


} // namespace imtloggui


