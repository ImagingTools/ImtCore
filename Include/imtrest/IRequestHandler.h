#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtrest
{


class IRequest;
class IProtocolEngine;


/**
	Interface for a request handler (servlet)
*/
class IRequestHandler: virtual public istd::IPolymorphic
{
public:
	/**
		Process required request.
	*/
	virtual bool ProcessRequest(const IRequest& request) const = 0;

	/**
		Get supported command-ID (e.g. EST sub-path) for the servlet.
	*/
	virtual QByteArray GetSupportedCommandId() const = 0;
};


} // namespace imtrest


