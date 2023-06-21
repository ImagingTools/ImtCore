#include <imtloggui/CProductionRepresentationControllerComp.h>


// ACF includes
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
			imtlog::IEventProvider::EventContainerPtr containerPtr,
			const imtbase::CTimeRange& /*timeRange*/) const
{
	CProductionRepresentationComp* representationPtr = dynamic_cast<CProductionRepresentationComp*>(&representation);
	if (representationPtr != nullptr){
		istd::CChangeNotifier notifier(representationPtr);

		representationPtr->ClearStatistics();

		if (m_granularityAttrPtr.IsValid()){
			representationPtr->SetGranularity(*m_granularityAttrPtr);
		}
		else{
			representationPtr->SetGranularity(60);
		}

		if (containerPtr.IsValid()){
			ilog::IMessageContainer::Messages messages = containerPtr->GetMessages();
			for (ilog::IMessageContainer::Messages::reverse_iterator it = messages.rbegin(); it != messages.rend(); it++){
				representationPtr->AddMessage(*it);
			}
		}
	}
}


} // namespace imtloggui


