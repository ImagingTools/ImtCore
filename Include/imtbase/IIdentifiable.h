#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


class IIdentifiable: virtual public istd::IChangeable
{
public:
	/**
		Get the global unique identifier of the object.
	*/
	virtual QByteArray GetObjectUuid() const = 0;
};


} // namespace imtbase


