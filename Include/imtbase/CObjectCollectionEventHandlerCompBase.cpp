#include <imtbase/CObjectCollectionEventHandlerCompBase.h>


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectCollectionEvent.h>


namespace imtbase
{


// reimplemented (imtbase::IObjectCollectionEventHandler)

void CObjectCollectionEventHandlerCompBase::OnEvent(const IObjectCollection* objectCollection, const IObjectCollectionEvent* event)
{
	if (m_slaveEventHandlerCompPtr.IsValid()){
		m_slaveEventHandlerCompPtr->OnEvent(objectCollection, event);
	}
}


} // namespace imtbase


