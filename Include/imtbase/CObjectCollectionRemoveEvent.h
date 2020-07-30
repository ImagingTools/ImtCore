#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionEventBase.h>


namespace imtbase
{


class CObjectCollectionRemoveEvent: public CObjectCollectionEventBase
{
public:
	typedef CObjectCollectionEventBase BaseClass;

	explicit CObjectCollectionRemoveEvent(const QByteArray& itemId);
};


} // namespace imtbase


