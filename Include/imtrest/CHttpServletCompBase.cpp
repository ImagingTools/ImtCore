#include <imtrest/CHttpServletCompBase.h>


// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponder.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CHttpResponse.h>


namespace imtrest
{


// public methods

// reimplemented (IProtocolEngine)

IRequestHandler::ConstResponsePtr CHttpServletCompBase::ProcessRequest(const imtrest::IRequest& request) const
{
	imtrest::CHttpRequest* httpRequestPtr = dynamic_cast<imtrest::CHttpRequest*>(const_cast<imtrest::IRequest*>(&request));
	if (httpRequestPtr == nullptr){
		Q_ASSERT_X(false, "CHttpServletCompBase::ProcessRequest", "Invalid request type, a HTTP request was expected");

		return ConstResponsePtr();
	}

	return OnRequestReceived(*httpRequestPtr);
}


QByteArray CHttpServletCompBase::GetSupportedCommandId() const
{
	return *m_commandIdAttrPtr;
}


// protected methods

IRequestHandler::ConstResponsePtr CHttpServletCompBase::OnRequestReceived(const imtrest::CHttpRequest& request) const
{
	QByteArray commandId;
	imtrest::IRequest::CommandParams commandParams;
	HeadersMap headers;

	ExtractRequestInfo(request, commandId, commandParams, headers);

	switch (request.GetMethodType()){
	case imtrest::CHttpRequest::MethodType::MT_GET:
		return OnGet(commandId, commandParams, headers, request);

	case imtrest::CHttpRequest::MethodType::MT_POST:
		return OnPost(commandId, commandParams, headers, request);

	case imtrest::CHttpRequest::MethodType::MT_DELETE:
		return OnDelete(commandId, commandParams, headers, request);

	case imtrest::CHttpRequest::MethodType::MT_PATCH:
		return OnPatch(commandId, commandParams, headers, request);

	case imtrest::CHttpRequest::MethodType::MT_PUT:
		return OnPut(commandId, commandParams, headers, request);

	case imtrest::CHttpRequest::MethodType::MT_HEAD:
		return OnHead(commandId, commandParams, headers, request);

	case imtrest::CHttpRequest::MethodType::MT_OPTIONS:
		return OnOptions(commandId, commandParams, headers, request);

	case imtrest::CHttpRequest::MethodType::MT_UNKNOWN:
		return OnUnknown(commandId, commandParams, headers, request);

	default:
		return OnInvalid(commandId, commandParams, headers, request);
		break;
	}

	return ConstResponsePtr();
}


IRequestHandler::ConstResponsePtr CHttpServletCompBase::OnGet(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);

	return CreateDefaultErrorResponse("GET request received but is NOT implemented!", request);
}


IRequestHandler::ConstResponsePtr CHttpServletCompBase::OnPost(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);

	return CreateDefaultErrorResponse("POST request received but is NOT implemented!", request);
}


IRequestHandler::ConstResponsePtr CHttpServletCompBase::OnDelete(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);

	return CreateDefaultErrorResponse("DELETE request received but is NOT implemented!", request);
}


IRequestHandler::ConstResponsePtr CHttpServletCompBase::OnPatch(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);

	return CreateDefaultErrorResponse("PATCH request received but is NOT implemented!", request);
}


IRequestHandler::ConstResponsePtr CHttpServletCompBase::OnPut(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);

	return CreateDefaultErrorResponse("PUT request received but is NOT implemented!", request);
}


IRequestHandler::ConstResponsePtr CHttpServletCompBase::OnHead(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);

	return CreateDefaultErrorResponse("HEAD request received but is NOT implemented!", request);
}


IRequestHandler::ConstResponsePtr CHttpServletCompBase::OnOptions(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);
	
	return CreateDefaultErrorResponse("OPTIONS request received but is NOT implemented!", request);
}

IRequestHandler::ConstResponsePtr CHttpServletCompBase::OnUnknown(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);

	return CreateDefaultErrorResponse("UNKNOWN request received but is NOT implemented!", request);
}

IRequestHandler::ConstResponsePtr CHttpServletCompBase::OnInvalid(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);

	return CreateDefaultErrorResponse("INVALID request received but is NOT implemented!", request);
}

IRequestHandler::ConstResponsePtr CHttpServletCompBase::CreateDefaultErrorResponse(const QByteArray& errorString, const imtrest::CHttpRequest& request) const
{
	qCritical() << __FILE__ << __LINE__ << __FUNCTION__ << errorString;

	IRequestHandler::ConstResponsePtr retVal;

	retVal.SetPtr(request.GetProtocolEngine().CreateResponse(
				request,
				imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE,
				errorString,
				QByteArray("text/plain; charset=utf-8")));

	return retVal;
}


// private methods

void CHttpServletCompBase::ExtractRequestInfo(
			const imtrest::CHttpRequest& request,
			QByteArray& commandId,
			imtrest::IRequest::CommandParams& commandParams,
			HeadersMap& headers)
{
	commandId = request.GetCommandId();
	commandParams = request.GetCommandParams();

	QByteArrayList headersKeys = request.GetHeaders();
	for (const QByteArray& key : ::qAsConst(headersKeys)){
		headers.insert(key, request.GetHeaderValue(key));
	}
}


} // namespace imtrest


