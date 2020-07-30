#include <imtbase/CObjectCollectionUpdateDescriptionEvent.h>


namespace imtbase
{


// public methods

CObjectCollectionUpdateDescriptionEvent::CObjectCollectionUpdateDescriptionEvent(const QByteArray& itemId, const QString& description)
	:BaseClass(itemId),
	m_description(description)
{
}


const QString& CObjectCollectionUpdateDescriptionEvent::GetDescription() const
{
	return m_description;
}


} // namespace imtbase


