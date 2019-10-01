#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtrest
{


class IRequest;
class IProtocolEngine;


class IRequestHandler: virtual public istd::IPolymorphic
{
public:
	virtual bool ProcessRequest(const IRequest& request) const = 0;
};


} // namespace imtrest


