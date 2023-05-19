#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Something what can be identified inside of a closed system.
*/
class IIdentifiable: virtual public istd::IChangeable
{
public:
	/**
		Get the global unique identifier of the object.
	*/
	virtual QByteArray GetObjectUuid() const = 0;
};


} // namespace imtbase


