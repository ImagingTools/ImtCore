#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionEventBase.h>


namespace imtbase
{

/**
	Interface for object collection events
*/
class CObjectCollectionUpdateEvent: public CObjectCollectionEventBase
{
public:
	typedef CObjectCollectionEventBase BaseClass;

	explicit CObjectCollectionUpdateEvent(const QByteArray& itemId);
};


} // namespace imtbase


