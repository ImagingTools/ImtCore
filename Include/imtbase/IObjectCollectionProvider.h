#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


/**
	Interface for a provider of a object collection.
*/
class IObjectCollectionProvider: virtual public istd::IChangeable
{
public:
	virtual const IObjectCollection* GetObjectCollection() const = 0;
};


} // namespace imtbase


