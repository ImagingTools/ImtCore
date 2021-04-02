#pragma once


// ACF includes
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtrest/IResponse.h>


namespace imtrest
{


class IRequest;
class IProtocolEngine;
class IResponse;


class IRequestHandler: virtual public istd::IPolymorphic
{
public:
	typedef istd::TSmartPtr<const imtrest::IResponse> ConstResponsePtr;

	virtual ConstResponsePtr ProcessRequest(const IRequest& request) const = 0;
	virtual QByteArray GetSupportedCommandId() const = 0;
};


} // namespace imtrest


