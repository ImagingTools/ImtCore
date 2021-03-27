#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Interface for object collection events
	\ingroup Collection
*/
class IObjectCollectionEvent: virtual public istd::IChangeable
{
public:
	enum EventType
	{
		ET_UPDATE,
		ET_INSERT,
		ET_REMOVE
	};
	
	virtual int GetEventType() const = 0;

	virtual QByteArray GetItemId() const = 0;
};


} // namespace imtbase


