#pragma once


// ACF includes
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtrest/IResponse.h>


namespace imtrest
{


class IRequest;


class IRequestServlet: virtual public istd::IPolymorphic
{
public:
	virtual ConstResponsePtr ProcessRequest(const IRequest& request) const = 0;
	virtual QByteArray GetSupportedCommandId() const = 0;
};


} // namespace imtrest


