#include <imtbase/CObjectCollectionUpdateEnabledEvent.h>


namespace imtbase
{


// public methods

CObjectCollectionUpdateEnabledEvent::CObjectCollectionUpdateEnabledEvent(const QByteArray& itemId, bool enabled)
	:BaseClass(itemId),
	m_enabled(enabled)
{
}


bool CObjectCollectionUpdateEnabledEvent::IsEnabled()
{
	return m_enabled;
}


} // namespace imtbase


