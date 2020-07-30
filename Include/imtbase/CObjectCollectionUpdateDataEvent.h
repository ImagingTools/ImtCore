#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionEventBase.h>


namespace imtbase
{


class CObjectCollectionUpdateDataEvent: public CObjectCollectionEventBase
{
public:
	typedef CObjectCollectionEventBase BaseClass;

	explicit CObjectCollectionUpdateDataEvent(const QByteArray& itemId);
};


} // namespace imtbase


