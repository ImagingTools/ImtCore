#include <imtrest/CWebSocketProtocolEngineComp.h>


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


bool CWebSocketProtocolEngineComp::GetProtocolStatusCode(int statusCode, int& protocolStatusCode, QByteArray& statusCodeLiteral) const
{
	protocolStatusCode = 200;
	statusCodeLiteral = "OK";
	return true;
}


IRequest* CWebSocketProtocolEngineComp::CreateRequest(QObject* socketPtr, const IRequestServlet& requestHandler) const
{
	if (socketPtr != nullptr){
		return new CWebSocketRequest(*socketPtr, requestHandler, *this);
	}

	return nullptr;
}


IRequest *CWebSocketProtocolEngineComp::CreateRequestForSend(
		QObject* socketPtr,
		const IRequestServlet& requestHandler,
		int statusCode,
		const QByteArray &data,
		const QByteArray &dataTypeId) const
{
	CWebSocketRequest *webSocketRequest = new CWebSocketRequest(*socketPtr, requestHandler, *this);
	webSocketRequest->SetBody(data);
	return webSocketRequest;
}


IResponse* CWebSocketProtocolEngineComp::CreateResponse(
			const IRequest& request,
			int statusCode,
			const QByteArray& data,
			const QByteArray& dataTypeId) const
{
	return new CWebSocketResponse(statusCode, data, dataTypeId, request.GetSocketObject(), *this);
}


const ISender& CWebSocketProtocolEngineComp::GetSender(const IRequest* /*requestPtr*/) const
{
	return m_sender;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CWebSocketProtocolEngineComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}




} // namespace imtrest


