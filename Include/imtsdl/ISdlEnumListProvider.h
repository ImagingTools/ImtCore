#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtsdl/CSdlEnum.h>


namespace imtsdl
{


class ISdlEnumListProvider: virtual public istd::IPolymorphic
{

public:
	/**
	   \returns available enums
	   \param onlyLocal - if true - external types will be excluded
	 */
	virtual SdlEnumList GetEnums(bool onlyLocal) const = 0;
};


} // namespace imtsdl
