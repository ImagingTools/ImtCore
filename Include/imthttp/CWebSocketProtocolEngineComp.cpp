#include <imthttp/CWebSocketProtocolEngineComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imthttp/CWebSocketRequest.h>
#include <imthttp/CWebSocketResponse.h>


namespace imthttp
{


// public methods

// reimplemented (IProtocolEngine)

QByteArray CWebSocketProtocolEngineComp::GetProtocolTypeId() const
{
	return "ws";
}


const iser::IVersionInfo* CWebSocketProtocolEngineComp::GetProtocolVersion() const
{
	return nullptr;
}


IProtocolEngine::ProtocolPattern CWebSocketProtocolEngineComp::GetProtocolPattern() const
{
	return PP_STREAMING;
}


bool CWebSocketProtocolEngineComp::GetProtocolStatusCode(int /*statusCode*/, int& protocolStatusCode, QByteArray& statusCodeLiteral) const
{
	protocolStatusCode = 200;
	statusCodeLiteral = "OK";
	return true;
}


imthttp::IRequestUniquePtr CWebSocketProtocolEngineComp::CreateRequest(const imtrest::IRequestServlet& requestHandler) const
{
	CWebSocketRequest* webSocketRequest = new CWebSocketRequest(requestHandler, *this);

	return webSocketRequest;
}


imthttp::IRequestUniquePtr CWebSocketProtocolEngineComp::CreateRequestForSend(
		const imtrest::IRequestServlet& requestHandler,
		int /*statusCode*/,
		const QByteArray& data,
		const QByteArray& /*dataTypeId*/) const
{
	CWebSocketRequest* webSocketRequest = new CWebSocketRequest(requestHandler, *this);
	webSocketRequest->SetBody(data);

	return webSocketRequest;
}


imthttp::IResponseUniquePtr CWebSocketProtocolEngineComp::CreateResponse(
			const IRequest& /*request*/,
			int statusCode,
			const QByteArray& data,
			const QByteArray& dataTypeId) const
{
	return new CWebSocketResponse(statusCode, data, dataTypeId, *this);
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CWebSocketProtocolEngineComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


} // namespace imthttp


