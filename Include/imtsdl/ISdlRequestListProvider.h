#pragma once


// ImtCore includes
#include <imtsdl/CSdlRequest.h>


namespace imtsdl
{


class ISdlRequestListProvider: virtual public istd::IPolymorphic
{

public:
	virtual SdlRequestList GetRequests(bool onlyLocal) const = 0;
};


} // namespace imtsdl
