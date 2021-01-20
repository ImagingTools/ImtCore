#include <imtloggui/CRepresentationControllerCompBase.h>


namespace imtloggui
{


// private methods

// reimplemented (imod::CSingleModelObserverBase)

void CRepresentationControllerCompBase::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_representationCompPtr.IsValid() && m_eventProviderCompPtr.IsValid()){
		BuildRepresentation(
					*m_representationCompPtr,
					*m_eventProviderCompPtr,
					m_eventFilterCompPtr.GetPtr(),
					m_messageFilterParamsCompPtr.GetPtr());
	}
}


} // namespace imtloggui


