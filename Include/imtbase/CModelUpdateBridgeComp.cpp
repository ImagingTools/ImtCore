#include <imtbase/CModelUpdateBridgeComp.h>


namespace imtbase
{


// public methods

// reimplemented (imod::CSingleModelObserverBase)

void CModelUpdateBridgeComp::BeforeUpdate(imod::IModel* modelPtr)
{
	if (m_slaveModelCompPtr.IsValid()){
		for (int i = 0; i < m_slaveModelCompPtr.GetCount(); i++){
			if (m_slaveModelCompPtr[i] != nullptr){
				m_slaveModelCompPtr[i]->BeginChanges(istd::IChangeable::GetAnyChange());
			}
		}
	}
}


void CModelUpdateBridgeComp::AfterUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet)
{
	if (m_slaveModelCompPtr.IsValid()){
		for (int i = 0; i < m_slaveModelCompPtr.GetCount(); i++){
			if (m_slaveModelCompPtr[i] != nullptr){
				m_slaveModelCompPtr[i]->EndChanges(istd::IChangeable::GetAnyChange());
			}
		}
	}
}

void CModelUpdateBridgeComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


} // namespace imtbase


