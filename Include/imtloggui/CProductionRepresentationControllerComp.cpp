#include <imtloggui/CProductionRepresentationControllerComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtloggui/CProductionRepresentationComp.h>


namespace imtloggui
{


// protected methods

void CProductionRepresentationControllerComp::BuildRepresentation(
			istd::IChangeable& representation,
			const imtlog::IEventProvider& eventProvider,
			const imtlog::IEventFilter* eventFilterPtr,
			const imtlog::IMessageFilterParams* messageFilterParamsPtr) const
{
	CProductionRepresentationComp* representationModelPtr = dynamic_cast<CProductionRepresentationComp*>(&representation);
	if (representationModelPtr != nullptr){
		if (eventFilterPtr != nullptr && messageFilterParamsPtr != nullptr){
			ilog::IMessageContainer::Messages messages = eventProvider.GetEvents(eventFilterPtr, messageFilterParamsPtr)->GetMessages();

			istd::CChangeGroup notifier(representationModelPtr);

			representationModelPtr->ClearStatistics();

			for (int i = messages.count() - 1; i >= 0; i--){
				representationModelPtr->AddMessage(messages[i]);
			}
		}
	}
}




} // namespace imtloggui


