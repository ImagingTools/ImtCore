#include <imtloggui/CEventBasedRepresentationControllerComp.h>


// ACF includes
#include <istd/CChangeGroup.h>
#include <ilog/CMessageContainer.h>


namespace imtloggui
{


// protected methods

void CEventBasedRepresentationControllerComp::BuildRepresentation(
			istd::IChangeable& representation,
			const imtlog::IEventProvider& eventProvider,
			const imtlog::ITimeRangeProvider* timeRangeProviderPtr) const
{
	ilog::CMessageContainer* representationModelPtr = dynamic_cast<ilog::CMessageContainer*>(&representation);
	if (representationModelPtr != nullptr){
		imtlog::CTimeRange timeRange;

		if (timeRangeProviderPtr != nullptr){
			timeRange = timeRangeProviderPtr->GetTimeRange();
		}

		QList<int> messageIds;
		imtlog::IEventFilter::FilterMode filterMode;

		imtlog::IEventProvider::EventFilterPtr filterPtr = imtlog::IEventProvider::EventFilterPtr(new Filter(timeRange, messageIds, filterMode));

		ilog::IMessageContainer::Messages messages = eventProvider.GetEvents(filterPtr)->GetMessages(); // TODO: CHECK POINTER!!!
	
		istd::CChangeGroup notifier(representationModelPtr);

		representationModelPtr->ClearMessages();

		for (int i = messages.count() - 1; i >= 0; i--){
			representationModelPtr->AddMessage(messages[i]);
		}
	}
}


// public methods of the embedded class Filter

// reimplemented (imtlog::IMessageFilter)

bool CEventBasedRepresentationControllerComp::Filter::IsMessageAccepted(const istd::IInformationProvider* messagePtr) const
{
	if (m_timeRange.IsClosed() && !m_timeRange.Contains(messagePtr->GetInformationTimeStamp())){
		return false;
	}

	if (m_filterMode == FM_INCLUDE && !m_messageIdList.contains(messagePtr->GetInformationId())){
		return false;
	}

	if (m_filterMode == FM_EXCLUDE && m_messageIdList.contains(messagePtr->GetInformationId())){
		return false;
	}

	return true;
}


QList<int> CEventBasedRepresentationControllerComp::Filter::GetGroupMessageIds() const
{
	return m_messageIdList;
}


// reimplemented (imtlog::ITimeRangeProvider)

imtlog::CTimeRange CEventBasedRepresentationControllerComp::Filter::GetTimeRange() const
{
	return m_timeRange;
}


} // namespace imtloggui


