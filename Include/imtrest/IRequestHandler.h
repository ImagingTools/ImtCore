#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtrest
{


class IRequest;
class IProtocolEngine;
class IResponse;


class IRequestHandler: virtual public istd::IPolymorphic
{
public:
        virtual const IResponse* ProcessRequest(const IRequest& request) const = 0;
    virtual QByteArray GetSupportedCommandId() const = 0;
};


} // namespace imtrest


