#pragma once


// ACF includes
#include <imtbase/IObjectContainer.h>


namespace imtbase
{


/**
	Interface for an object container.
*/
class ICompositeObjectProvider: virtual public istd::IChangeable
{
public:
	virtual const imtbase::IObjectContainer* GetCompositeObject() const = 0;
};


} // namespace imtbase


