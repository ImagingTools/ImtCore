#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>


namespace imtsdl
{


class ISdlUnionListProvider: virtual public istd::IPolymorphic
{
public:
	/**
	   \returns available enums
	   \param onlyLocal - if true - external types will be excluded
	 */
	virtual SdlUnionList GetUnions(bool onlyLocal) const = 0;
};


} // namespace imtsdl
