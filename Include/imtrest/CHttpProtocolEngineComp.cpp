#include <imtrest/CHttpProtocolEngineComp.h>


// ImtCore includes
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpResponse.h>


namespace imtrest
{


// public methods

// reimplemented (IProtocolEngine)

QByteArray CHttpProtocolEngineComp::GetProtocolTypeId() const
{
	return "http";
}


const iser::IVersionInfo* CHttpProtocolEngineComp::GetProtocolVersion() const
{
	return nullptr;
}


bool CHttpProtocolEngineComp::GetProtocolStatusCode(int statusCode, int& protocolStatusCode, QByteArray& statusCodeLiteral) const
{
	switch (statusCode){
	case SC_OK:
		protocolStatusCode = 200;
		statusCodeLiteral = "OK";
		return true;

	case SC_OPERATION_NOT_AVAILABLE:
		protocolStatusCode = 405;
		statusCodeLiteral = "Method Not Allowed";
		return true;

	case SC_RESOURCE_NOT_AVAILABLE:
		protocolStatusCode = 404;
		statusCodeLiteral = "Not Found";
		return true;

	case SC_NOT_MODIFIED:
		protocolStatusCode = 304;
		statusCodeLiteral = "Not Found";
		return true;

	case SC_UNAUTHORIZED:
		protocolStatusCode = 401;
		statusCodeLiteral = "Unauthorized";
		return true;

	case SC_INTERNAL_ERROR:
		protocolStatusCode = 500;
		statusCodeLiteral = "Internal Server Error";
		return true;

	default:
		Q_ASSERT(false);
		break;
	}

	return false;
}


IRequest* CHttpProtocolEngineComp::CreateRequest(QAbstractSocket* socketPtr, const IRequestHandler& requestHandler) const
{
	if (socketPtr != nullptr){
		return new CHttpRequest(*socketPtr, requestHandler, *this);
	}

	return nullptr;
}


IResponse* CHttpProtocolEngineComp::CreateResponse(
			const IRequest& request,
			const QByteArray& data,
			int statusCode,
			const QByteArray& dataTypeId) const
{
	return new CHttpResponse(statusCode, data, dataTypeId, request.GetSocket(), *this);
}


const IResponder& CHttpProtocolEngineComp::GetResponder(const IRequest* /*requestPtr*/) const
{
	return m_responder;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CHttpProtocolEngineComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


} // namespace imtrest


