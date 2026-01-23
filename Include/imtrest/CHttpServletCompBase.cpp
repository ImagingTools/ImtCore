#include <imtrest/CHttpServletCompBase.h>


// ImtCore includes
#include <imthttp/IRequest.h>
#include <imthttp/IResponse.h>
#include <imthttp/IProtocolEngine.h>
#include <imtrest/CHttpResponse.h>


namespace imtrest
{


// public methods

// reimplemented (IProtocolEngine)

imthttp::ConstResponsePtr CHttpServletCompBase::ProcessRequest(const imtrest::IRequest& request, const QByteArray& /*subCommandId*/) const
{
	imtrest::CHttpRequest* httpRequestPtr = dynamic_cast<imtrest::CHttpRequest*>(const_cast<imtrest::IRequest*>(&request));
	if (httpRequestPtr == nullptr){
		Q_ASSERT_X(false, "CHttpServletCompBase::ProcessRequest", "Invalid request type, a HTTP request was expected");
		SendErrorMessage(0, QString("Invalid request type, a HTTP request was expected"));

		return ConstResponsePtr();
	}

	return OnRequestReceived(*httpRequestPtr);
}


bool CHttpServletCompBase::IsCommandSupported(const QByteArray& commandId) const
{
	QByteArray clearCommandId = commandId;
	if (clearCommandId.startsWith('/')){
		clearCommandId = clearCommandId.remove(0, 1);
	}

	if (clearCommandId.endsWith('/')){
		clearCommandId = clearCommandId.remove(clearCommandId.length() - 1, 1);
	}

	QByteArrayList inputCommandList = clearCommandId.split('/');
	QByteArray localCommandId = *m_commandIdAttrPtr;

	if (m_commandIdCompPtr.IsValid()){
		localCommandId = m_commandIdCompPtr->GetId();
	}

	QByteArrayList localCommandList = localCommandId.split('/');

	bool retVal = true;

	for (int i = 0; i < localCommandList.count(); i++){
		if (localCommandList[i] == "*"){
			break;
		}

		if (i >= inputCommandList.count() || inputCommandList[i] != localCommandList[i]){
			retVal = false;

			break;
		}

		if (i == localCommandList.count() - 1 && localCommandList.count() < inputCommandList.count()){
			retVal = false;

			break;
		}
	}

	return retVal;
}


// protected methods

imthttp::ConstResponsePtr CHttpServletCompBase::OnRequestReceived(const imtrest::CHttpRequest& request) const
{
	QByteArray commandId;
	imtrest::imthttp::IRequest::CommandParams commandParams;
	HeadersMap headers;

	ExtractRequestInfo(request, commandId, commandParams, headers);

	switch (request.GetMethodType()){
	case imtrest::imthttp::CHttpRequest::MethodType::MT_GET:
		return OnGet(commandId, commandParams, headers, request);

	case imtrest::imthttp::CHttpRequest::MethodType::MT_POST:
		return OnPost(commandId, commandParams, headers, request);

	case imtrest::imthttp::CHttpRequest::MethodType::MT_DELETE:
		return OnDelete(commandId, commandParams, headers, request);

	case imtrest::imthttp::CHttpRequest::MethodType::MT_PATCH:
		return OnPatch(commandId, commandParams, headers, request);

	case imtrest::imthttp::CHttpRequest::MethodType::MT_PUT:
		return OnPut(commandId, commandParams, headers, request);

	case imtrest::imthttp::CHttpRequest::MethodType::MT_HEAD:
		return OnHead(commandId, commandParams, headers, request);

	case imtrest::imthttp::CHttpRequest::MethodType::MT_OPTIONS:
		return OnOptions(commandId, commandParams, headers, request);

	case imtrest::imthttp::CHttpRequest::MethodType::MT_UNKNOWN:
		return OnUnknown(commandId, commandParams, headers, request);

	default:
		return OnInvalid(commandId, commandParams, headers, request);
		break;
	}

	return ConstResponsePtr();
}


imthttp::ConstResponsePtr CHttpServletCompBase::OnGet(
			const QByteArray& commandId,
			const imtrest::imthttp::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);

	return CreateDefaultErrorResponse("GET request received but is NOT implemented!", request);
}


imthttp::ConstResponsePtr CHttpServletCompBase::OnPost(
			const QByteArray& commandId,
			const imtrest::imthttp::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);

	return CreateDefaultErrorResponse("POST request received but is NOT implemented!", request);
}


imthttp::ConstResponsePtr CHttpServletCompBase::OnDelete(
			const QByteArray& commandId,
			const imtrest::imthttp::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);

	return CreateDefaultErrorResponse("DELETE request received but is NOT implemented!", request);
}


imthttp::ConstResponsePtr CHttpServletCompBase::OnPatch(
			const QByteArray& commandId,
			const imtrest::imthttp::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);

	return CreateDefaultErrorResponse("PATCH request received but is NOT implemented!", request);
}


imthttp::ConstResponsePtr CHttpServletCompBase::OnPut(
			const QByteArray& commandId,
			const imtrest::imthttp::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);

	return CreateDefaultErrorResponse("PUT request received but is NOT implemented!", request);
}


imthttp::ConstResponsePtr CHttpServletCompBase::OnHead(
			const QByteArray& commandId,
			const imtrest::imthttp::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);

	return CreateDefaultErrorResponse("HEAD request received but is NOT implemented!", request);
}


imthttp::ConstResponsePtr CHttpServletCompBase::OnOptions(
			const QByteArray& commandId,
			const imtrest::imthttp::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);
	
	return CreateDefaultErrorResponse("OPTIONS request received but is NOT implemented!", request);
}

imthttp::ConstResponsePtr CHttpServletCompBase::OnUnknown(
			const QByteArray& commandId,
			const imtrest::imthttp::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);

	return CreateDefaultErrorResponse("UNKNOWN request received but is NOT implemented!", request);
}

imthttp::ConstResponsePtr CHttpServletCompBase::OnInvalid(
			const QByteArray& commandId,
			const imtrest::imthttp::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	Q_UNUSED(commandId);
	Q_UNUSED(commandParams);
	Q_UNUSED(headers);

	return CreateDefaultErrorResponse("INVALID request received but is NOT implemented!", request);
}

imthttp::ConstResponsePtr CHttpServletCompBase::CreateDefaultErrorResponse(const QByteArray& errorString, const imtrest::CHttpRequest& request) const
{
	qCritical() << __FILE__ << __LINE__ << __FUNCTION__ << errorString;
	SendErrorMessage(0, errorString);

	imthttp::ConstResponsePtr retVal;

	retVal.SetPtr(request.GetProtocolEngine().CreateResponse(
				request,
				imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE,
				errorString,
				QByteArray("text/plain; charset=utf-8")).PopInterfacePtr());

	return retVal;
}


void CHttpServletCompBase::ExtractRequestInfo(
			const imtrest::CHttpRequest& request,
			QByteArray& commandId,
			imtrest::imthttp::IRequest::CommandParams& commandParams,
			HeadersMap& headers)
{
	commandId = request.GetCommandId();
	commandParams = request.GetCommandParams();

	QByteArrayList headersKeys = request.GetHeaders();
	for (const QByteArray& key : ::std::as_const(headersKeys)){
		headers.insert(key, request.GetHeaderValue(key));
	}
}


} // namespace imtrest


