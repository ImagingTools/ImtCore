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
	
	/**
		Get type of the collection event.
		\sa EventType
	*/
	virtual int GetEventType() const = 0;

	/**
		Get ID of the related item in the collection.
	*/
	virtual QByteArray GetItemId() const = 0;
};


} // namespace imtbase


