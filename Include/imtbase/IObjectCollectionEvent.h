#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Interface for object collection events
*/
class IObjectCollectionEvent: virtual public istd::IChangeable
{
public:
	virtual QByteArray GetItemId() const = 0;
};


} // namespace imtbase


