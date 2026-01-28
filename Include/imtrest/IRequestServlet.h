#pragma once


// ACF includes
#include <istd/TInterfacePtr.h>

// ImtCore includes
#include <imthttp/IRequest.h>
#include <imthttp/IResponse.h>


namespace imtrest
{


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
	virtual imthttp::ConstResponsePtr ProcessRequest(const imthttp::IRequest& request, const QByteArray& subCommandId = QByteArray()) const = 0;
};


typedef istd::TUniqueInterfacePtr<IRequestServlet> IRequestServletPtr;


} // namespace imtrest


