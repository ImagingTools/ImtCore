#include <imtbase/CObjectCollectionUpdateNameEvent.h>


namespace imtbase
{


// public methods

CObjectCollectionUpdateNameEvent::CObjectCollectionUpdateNameEvent(const QByteArray& itemId, const QString& name)
	:BaseClass(itemId),
	m_name(name)
{
}


const QString& CObjectCollectionUpdateNameEvent::GetName() const
{
	return m_name;
}


} // namespace imtbase


