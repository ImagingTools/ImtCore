#include <imtgql/CObjectCollectionSubscriberControllerComp.h>


namespace imtgql
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CObjectCollectionSubscriberControllerComp::OnComponentCreated()
{
	if (m_objectCollectionModelCompPtr.IsValid()) {
		m_objectCollectionModelCompPtr->AttachObserver(this);
	}
}


// reimplemented (imod::CSingleModelObserverBase)

void CObjectCollectionSubscriberControllerComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	SetSubscriptions();
}


} // namespace imtgql


