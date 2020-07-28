#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{


/**
	Interface for object collection events
*/
class IObjectCollectionEvent: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray GetItemId() const = 0;
};


} // namespace imtbase


