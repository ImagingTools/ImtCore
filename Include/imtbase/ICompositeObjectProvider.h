#pragma once


// ACF includes
#include <imtbase/IObjectProvider.h>


namespace imtbase
{


/**
	Interface for an object container.
*/
class ICompositeObjectProvider: virtual public istd::IChangeable
{
public:
	virtual const imtbase::IObjectProvider* GetCompositeObject() const = 0;
};


} // namespace imtbase


