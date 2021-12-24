#include <imtloggui/CEventBasedRepresentationControllerComp.h>


// ACF includes
#include <istd/CChangeGroup.h>
#include <ilog/CMessageContainer.h>


namespace imtloggui
{


// protected methods

void CEventBasedRepresentationControllerComp::BuildRepresentation(
				istd::IChangeable& representation,
				imtlog::IEventProvider::EventContainerPtr containerPtr,
				const imtbase::CTimeRange& /*timeRange*/) const
{
	ilog::CMessageContainer* representationModelPtr = dynamic_cast<ilog::CMessageContainer*>(&representation);
	if (representationModelPtr != nullptr){	
		istd::CChangeGroup notifier(representationModelPtr);

		representationModelPtr->ClearMessages();

		if (containerPtr.IsValid()){
			ilog::IMessageContainer::Messages messages = containerPtr->GetMessages();

			for (int i = messages.count() - 1; i >= 0; i--){
				representationModelPtr->AddMessage(messages[i]);
			}
		}
	}
}


} // namespace imtloggui


