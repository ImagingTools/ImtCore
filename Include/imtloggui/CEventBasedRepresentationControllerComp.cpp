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

		CEventFilter filter;

		ilog::IMessageContainer::Messages messages = eventProvider.GetEvents(&filter, &timeRange);//, filterParams)->GetMessages(); // TODO: CHECK POINTER!!!
	
		istd::CChangeGroup notifier(representationModelPtr);

		representationModelPtr->ClearMessages();

		for (int i = messages.count() - 1; i >= 0; i--){
			representationModelPtr->AddMessage(messages[i]);
		}
	}
}


// public methods of the embedded class Filter

// reimplemented (imtlog::IMessageFilter)

bool CEventBasedRepresentationControllerComp::CEventFilter::IsMessageAccepted(
			const istd::IInformationProvider & message,
			const imtlog::CTimeRange* timeRangePtr,
			const imtlog::IMessageFilterParams * filterParamsPtr) const
{
	if ((timeRangePtr != nullptr) && timeRangePtr->IsClosed() && !timeRangePtr->Contains(message.GetInformationTimeStamp())){
		return false;
	}

	if (filterParamsPtr != nullptr){
		int filterMode = filterParamsPtr->GetFilterMode();

		if (filterMode == imtlog::IMessageFilterParams::FM_INCLUDE && !filterParamsPtr->GetMessageFilterIds().contains(message.GetInformationId())){
			return false;
		}

		if (filterMode == imtlog::IMessageFilterParams::FM_EXCLUDE && filterParamsPtr->GetMessageFilterIds().contains(message.GetInformationId())){
			return false;
		}
	}

	return true;
}

} // namespace imtloggui


