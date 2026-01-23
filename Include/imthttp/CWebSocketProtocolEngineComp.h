#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imthttp/IProtocolEngine.h>
#include <imthttp/CWebSocketSender.h>
#include <imthttp/http_parser.h>


namespace imtrest
{
class IRequestServlet;
}


namespace imthttp
{


class CWebSocketProtocolEngineComp:
			public ilog::CLoggerComponentBase,
			virtual public IProtocolEngine
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CWebSocketProtocolEngineComp);
		I_REGISTER_INTERFACE(IProtocolEngine)
	I_END_COMPONENT

	// reimplemented (IProtocolEngine)
	virtual QByteArray GetProtocolTypeId() const override;
	virtual const iser::IVersionInfo* GetProtocolVersion() const override;
	virtual bool GetProtocolStatusCode(int statusCode, int& protocolStatusCode, QByteArray& statusCodeLiteral) const override;
	virtual imthttp::IRequestUniquePtr CreateRequest(const imtrest::IRequestServlet& requestHandler) const override;
	virtual imthttp::IRequestUniquePtr CreateRequestForSend(
				const imtrest::IRequestServlet& requestHandler,
				int statusCode,
				const QByteArray& data,
				const QByteArray& dataTypeId) const override;
	virtual imthttp::IResponseUniquePtr CreateResponse(
				const IRequest& request,
				int statusCode,
				const QByteArray& data,
				const QByteArray& dataTypeId) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
};


} // namespace imthttp


