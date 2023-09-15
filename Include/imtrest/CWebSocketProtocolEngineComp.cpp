#include <imtrest/CWebSocketProtocolEngineComp.h>


// Acf includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtrest/CWebSocketRequest.h>
#include <imtrest/CWebSocketResponse.h>


namespace imtrest
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


bool CWebSocketProtocolEngineComp::GetProtocolStatusCode(int /*statusCode*/, int& protocolStatusCode, QByteArray& statusCodeLiteral) const
{
	protocolStatusCode = 200;
	statusCodeLiteral = "OK";
	return true;
}


IRequest* CWebSocketProtocolEngineComp::CreateRequest(const IRequestServlet& requestHandler) const
{
	CWebSocketRequest* webSocketRequest = new CWebSocketRequest(requestHandler, *this);

	return webSocketRequest;
}


IRequest *CWebSocketProtocolEngineComp::CreateRequestForSend(
		const IRequestServlet& requestHandler,
		int /*statusCode*/,
		const QByteArray& data,
		const QByteArray& /*dataTypeId*/) const
{
	CWebSocketRequest* webSocketRequest = new CWebSocketRequest(requestHandler, *this);
	webSocketRequest->SetBody(data);

	return webSocketRequest;
}


IResponse* CWebSocketProtocolEngineComp::CreateResponse(
			const IRequest& request,
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


} // namespace imtrest


