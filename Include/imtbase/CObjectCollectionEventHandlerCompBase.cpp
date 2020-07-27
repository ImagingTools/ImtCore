#include <imtbase/CObjectCollectionEventHandlerCompBase.h>



namespace imtbase
{


// reimplemented (imtbase::IObjectCollectionEventHandler)

void CObjectCollectionEventHandlerCompBase::OnInsert(const IObjectCollectionInfo::Id& objectId)
{
	if (m_slaveEventHandlerCompPtr.IsValid()){
		m_slaveEventHandlerCompPtr->OnInsert(objectId);
	}
}


void CObjectCollectionEventHandlerCompBase::OnUpdate(const IObjectCollectionInfo::Id& objectId)
{
	if (m_slaveEventHandlerCompPtr.IsValid()){
		m_slaveEventHandlerCompPtr->OnUpdate(objectId);
	}
}


void CObjectCollectionEventHandlerCompBase::OnRemove(const IObjectCollectionInfo::Id& objectId)
{
	if (m_slaveEventHandlerCompPtr.IsValid()){
		m_slaveEventHandlerCompPtr->OnRemove(objectId);
	}
}


void CObjectCollectionEventHandlerCompBase::OnRename(const IObjectCollectionInfo::Id& objectId)
{
	if (m_slaveEventHandlerCompPtr.IsValid()){
		m_slaveEventHandlerCompPtr->OnRename(objectId);
	}
}


void CObjectCollectionEventHandlerCompBase::OnDescriptionUpdate(const IObjectCollectionInfo::Id& objectId)
{
	if (m_slaveEventHandlerCompPtr.IsValid()){
		m_slaveEventHandlerCompPtr->OnDescriptionUpdate(objectId);
	}
}


void CObjectCollectionEventHandlerCompBase::OnEnableStateChange(const IObjectCollectionInfo::Id& objectId)
{
	if (m_slaveEventHandlerCompPtr.IsValid()){
		m_slaveEventHandlerCompPtr->OnEnableStateChange(objectId);
	}
}


} // namespace imtbase


