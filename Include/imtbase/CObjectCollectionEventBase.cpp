#include <imtbase/CObjectCollectionEventBase.h>


namespace imtbase
{


// public methods

CObjectCollectionEventBase::CObjectCollectionEventBase(const QByteArray& itemId)
	:m_itemId(itemId)
{
}


// reimplemented (imtbase::IObjectCollectionEvent)

QByteArray CObjectCollectionEventBase::GetItemId() const
{
	return m_itemId;
}


} // namespace imtbase


