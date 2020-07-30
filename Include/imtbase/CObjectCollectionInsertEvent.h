#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionEventBase.h>


namespace imtbase
{


class CObjectCollectionInsertEvent: public CObjectCollectionEventBase
{
public:
	typedef CObjectCollectionEventBase BaseClass;

	explicit CObjectCollectionInsertEvent(const QByteArray& itemId);
};


} // namespace imtbase


