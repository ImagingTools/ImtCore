#include <imtloggui/CProductionRepresentationControllerComp.h>


// Acf includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtloggui/CProductionRepresentationComp.h>


namespace imtloggui
{


// public methods

// reimplemented (icomp::CComponentBase)

void CProductionRepresentationControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_representationCompPtr.IsValid()){
		CProductionRepresentationComp* representationPtr = dynamic_cast<CProductionRepresentationComp*>(m_representationCompPtr.GetPtr());
		if (representationPtr != nullptr){
			if (m_granularityAttrPtr.IsValid()){
				representationPtr->SetGranularity(*m_granularityAttrPtr);
			}
			else{
				representationPtr->SetGranularity(60);
			}
		}
	}
}


// protected methods

void CProductionRepresentationControllerComp::BuildRepresentation(
			istd::IChangeable& representation,
			const imtlog::IEventProvider& eventProvider,
			const imtlog::IEventFilter* eventFilterPtr,
			const imtlog::IMessageFilterParams* messageFilterParamsPtr) const
{
	CProductionRepresentationComp* representationPtr = dynamic_cast<CProductionRepresentationComp*>(&representation);
	if (representationPtr != nullptr){
		if (eventFilterPtr != nullptr && messageFilterParamsPtr != nullptr){
			ilog::IMessageContainer::Messages messages = eventProvider.GetEvents(eventFilterPtr, messageFilterParamsPtr)->GetMessages();

			istd::CChangeNotifier notifier(representationPtr);

			representationPtr->ClearStatistics();

			for (int i = messages.count() - 1; i >= 0; i--){
				representationPtr->AddMessage(messages[i]);
			}
		}
	}
}




} // namespace imtloggui


