#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Iterface for access of general defined list of data objects.
*/
class IObjectProvider: virtual public istd::IChangeable
{
public:
	/**
		Get the count of the objects in the list.
	*/
	virtual int GetObjectCount() const = 0;

	/**
		Get the object instance at the given position.
	*/
	virtual const istd::IChangeable* GetObject(int objectIndex) const = 0;
};


} // namespace imtbase


