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
	/**
		Returns \c true if the command (also a hierachical command) is supported by the servlet.
		Only supported commands can be processed and are supported in \c ProcessRequest.
	*/
	virtual bool IsCommandSupported(const QByteArray& commandId) const = 0;

	/**
		Process a given request.
	*/
	virtual ConstResponsePtr ProcessRequest(const IRequest& request, const QByteArray& subCommandId = QByteArray()) const = 0;
};


} // namespace imtrest


