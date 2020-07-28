#include <imtbase/CObjectCollectionUpdateEvent.h>


namespace imtbase
{


// reimplemented (imtbase::IObjectCollectionEvent)

CObjectCollectionUpdateEvent::CObjectCollectionUpdateEvent(const QByteArray& itemId)
	:BaseClass(itemId)
{
}


} // namespace imtbase


