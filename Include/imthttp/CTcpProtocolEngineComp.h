#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imthttp/IProtocolEngine.h>


namespace imthttp
{


class CTcpProtocolEngineComp:
			public ilog::CLoggerComponentBase,
			virtual public IProtocolEngine
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTcpProtocolEngineComp);
		I_REGISTER_INTERFACE(IProtocolEngine)
	I_END_COMPONENT

	// reimplemented (IProtocolEngine)
	virtual QByteArray GetProtocolTypeId() const override;
	virtual const iser::IVersionInfo* GetProtocolVersion() const override;
	virtual bool GetProtocolStatusCode(int statusCode, int& protocolStatusCode, QByteArray& statusCodeLiteral) const override;
	virtual imthttp::IRequestUniquePtr CreateRequest(const IRequestServlet& requestHandler) const override;
	virtual imthttp::IRequestUniquePtr CreateRequestForSend(
				const IRequestServlet& requestHandler,
				int statusCode,
				const QByteArray& data,
				const QByteArray& dataTypeId) const override;
	virtual imthttp::IResponseUniquePtr CreateResponse(
				const IRequest& request,
				int statusCode,
				const QByteArray& data,
				const QByteArray& dataTypeId) const override;
};


} // namespace imthttp


