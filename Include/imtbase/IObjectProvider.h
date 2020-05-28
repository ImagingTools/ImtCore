#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	General interface for access any data object.
*/
class IObjectProvider: virtual public istd::IChangeable
{
public:
	/**
		Get access to the object.
	*/
	virtual const istd::IChangeable* GetDataObject() const = 0;
};


} // namespace imtbase


