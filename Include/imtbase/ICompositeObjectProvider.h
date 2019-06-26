#pragma once


// ImtCore includes
#include <imtbase/IObjectManager.h>


namespace imtbase
{


/**
	Interface for a provider of a composite object.
*/
class ICompositeObjectProvider: virtual public istd::IChangeable
{
public:
	virtual const IObjectManager* GetCompositeObject() const = 0;
};


} // namespace imtbase


