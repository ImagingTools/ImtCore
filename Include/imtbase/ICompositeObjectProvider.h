#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


/**
	Interface for a provider of a composite object.
*/
class ICompositeObjectProvider: virtual public istd::IChangeable
{
public:
	virtual const IObjectCollection* GetCompositeObject() const = 0;
};


} // namespace imtbase


