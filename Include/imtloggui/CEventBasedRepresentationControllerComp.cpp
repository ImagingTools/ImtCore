#include <imtloggui/CEventBasedRepresentationControllerComp.h>


// Acf includes
#include <istd/CChangeGroup.h>
#include <ilog/CMessageContainer.h>


namespace imtloggui
{


// protected methods

void CEventBasedRepresentationControllerComp::BuildRepresentation(
			istd::IChangeable& representation,
			const imtlog::IEventProvider& eventProvider,
			const imtlog::IEventFilter* eventFilterPtr,
			const imtlog::IMessageFilterParams* messageFilterParamsPtr) const
{
	ilog::CMessageContainer* representationModelPtr = dynamic_cast<ilog::CMessageContainer*>(&representation);
	if (representationModelPtr != nullptr){
		if (eventFilterPtr != nullptr && messageFilterParamsPtr != nullptr){
			ilog::IMessageContainer::Messages messages = eventProvider.GetEvents(eventFilterPtr, messageFilterParamsPtr)->GetMessages();
	
			istd::CChangeGroup notifier(representationModelPtr);

			representationModelPtr->ClearMessages();

			for (int i = messages.count() - 1; i >= 0; i--){
				representationModelPtr->AddMessage(messages[i]);
			}
		}
	}
}


} // namespace imtloggui


