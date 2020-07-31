#include <imtbase/CObjectCollectionUpdateEvent.h>


namespace imtbase
{


// public methods

CObjectCollectionUpdateEvent::CObjectCollectionUpdateEvent(const QByteArray& itemId, UpdateType updateType)
	:BaseClass(itemId),
	m_updateType(updateType)
{
}


CObjectCollectionUpdateEvent::UpdateType CObjectCollectionUpdateEvent::GetUpdateType() const
{
	return m_updateType;
}


} // namespace imtbase


