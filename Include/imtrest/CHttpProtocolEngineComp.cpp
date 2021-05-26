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
	auto setProtocolStatusCode = [&protocolStatusCode, &statusCodeLiteral](const int& protocolStatusCodeValue, const QByteArray& statusCodeLiteralValue) {
		protocolStatusCode = protocolStatusCodeValue;
		statusCodeLiteral = statusCodeLiteralValue;
		return true;
	};

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
		statusCodeLiteral = "Not Modified";
		return true;

	case SC_UNAUTHORIZED:
		protocolStatusCode = 401;
		statusCodeLiteral = "Unauthorized";
		return true;

	case SC_INTERNAL_ERROR:
		protocolStatusCode = 500;
		statusCodeLiteral = "Internal Server Error";
		return true;

	//1XX INFORMATIONAL STATUS
	case SC_CONTINUE						: return setProtocolStatusCode(100, "Continue"); break;
	case SC_SWITCHING_PROTOCOLS				: return setProtocolStatusCode(101, "Switching Protocols"); break;
	case SC_PROCESSING						: return setProtocolStatusCode(102, "Processing"); break;

	//2XX SUCCESS STATUS
	case SC_CREATED							: return setProtocolStatusCode(201, "Created"); break;
	case SC_ACCEPTED						: return setProtocolStatusCode(202, "Accepted"); break;
	case SC_NON_AUTHORITATIVE_INFORMATION	: return setProtocolStatusCode(203, "Non-Authoritative Information"); break;
	case SC_NO_CONTENT						: return setProtocolStatusCode(204, "No Content"); break;
	case SC_RESET_CONTENT					: return setProtocolStatusCode(205, "Reset Content"); break;
	case SC_PARTIAL_CONTENT					: return setProtocolStatusCode(206, "Partial Content"); break;
	case SC_MULTI_STATUS					: return setProtocolStatusCode(207, "Multi-Status"); break;
	case SC_ALREADY_REPORTED				: return setProtocolStatusCode(208, "Already Reported"); break;
	case SC_IM_USED							: return setProtocolStatusCode(226, "IM Used"); break;

	//3XX REDIRECT STATUS
	case SC_MULTIPLE_CHOICES				: return setProtocolStatusCode(300, "Multiple Choices"); break;
	case SC_MOVED_PERMANENTLY				: return setProtocolStatusCode(301, "Moved Permanently"); break;
	case SC_MOVED_TEMPORARILY				: return setProtocolStatusCode(302, "Moved Temporarily"); break;
	case SC_SEE_OTHER						: return setProtocolStatusCode(303, "See Other"); break;
	case SC_RESERVE							: return setProtocolStatusCode(306, "RESERVED"); break;
	case SC_TEMPORARY_REDIRECT				: return setProtocolStatusCode(307, "Temporary Redirect"); break;
	case SC_PERMANENT_REDIRECT				: return setProtocolStatusCode(308, "Permanent Redirect"); break;

	//4XX CLIENT-SIDE ERROR STATUS
	case SC_BAD_REQUEST						: return setProtocolStatusCode(400, "Bad Request"); break;
	case SC_PAYMENT_REQUIRED				: return setProtocolStatusCode(402, "Payment Required"); break;
	case SC_FORBIDDEN						: return setProtocolStatusCode(403, "Forbidden"); break;
	case SC_NOT_FOUND						: return setProtocolStatusCode(404, "Not Found"); break;
	case SC_METHOD_NOT_ALLOWED				: return setProtocolStatusCode(405, "Method Not Allowed"); break;
	case SC_NOT_ACCEPTABLE					: return setProtocolStatusCode(406, "Not Acceptable "); break;
	case SC_PROXY_AUTHENTICATION_REQUIRED	: return setProtocolStatusCode(407, "Proxy Authentication Required"); break;
	case SC_REQUEST_TIMEOUT					: return setProtocolStatusCode(408, "Request Timeout"); break;
	case SC_CONFLICT						: return setProtocolStatusCode(409, "Conflict"); break;
	case SC_GONE							: return setProtocolStatusCode(410, "Gone"); break;
	case SC_LENGTH_REQUIRED					: return setProtocolStatusCode(411, "Length Required"); break;
	case SC_PRECONDITION_FAILED				: return setProtocolStatusCode(412, "Precondition Failed"); break;
	case SC_PAYLOAD_TOO_LARGE				: return setProtocolStatusCode(413, "Payload Too Large"); break;
	case SC_URI_TOO_LONG					: return setProtocolStatusCode(414, "URI Too Long"); break;
	case SC_UNSUPPORTED_MEDIA_TYPE			: return setProtocolStatusCode(415, "Unsupported Media Type"); break;
	case SC_RANGE_NOT_SATISFIABLE			: return setProtocolStatusCode(416, "Range Not Satisfiable"); break;
	case SC_EXPECTATION_FAILED				: return setProtocolStatusCode(417, "Expectation Failed"); break;
	case SC_I_AM_A_TEAPOT					: return setProtocolStatusCode(418, "I’m a teapot"); break;
	case SC_MISDIRECTED_REQUEST				: return setProtocolStatusCode(421, "Misdirected Request"); break;
	case SC_UNPROCESSABLE_ENTITY			: return setProtocolStatusCode(422, "Unprocessable Entity"); break;
	case SC_LOCKED							: return setProtocolStatusCode(423, "Locked"); break;
	case SC_FAILED_DEPENDENCY				: return setProtocolStatusCode(424, "Failed Dependency"); break;
	case SC_UPGRADE_REQUIRED				: return setProtocolStatusCode(426, "Upgrade Required"); break;
	case SC_PRECONDITION_REQUIRED			: return setProtocolStatusCode(428, "Precondition Required"); break;
	case SC_TOO_MANY_REQUESTS				: return setProtocolStatusCode(429, "Too Many Requests"); break;
	case SC_REQUEST_HEADER_FIELDS_TOO_LARGE	: return setProtocolStatusCode(431, "Request Header Fields Too Large"); break;
	case SC_RETRY_WITH						: return setProtocolStatusCode(449, "Retry With"); break;
	case SC_UNAVAILABLE_FOR_LEGAL_REASONS	: return setProtocolStatusCode(451, "Unavailable For Legal Reasons"); break;

	//5XX SERVER-SIDE ERROR STATUS
	case SC_INTERNAL_SERVER_ERROR			: return setProtocolStatusCode(500, "Internal Server Error"); break;
	case SC_NOT_IMPLEMENTED					: return setProtocolStatusCode(501, "Not Implemented"); break;
	case SC_BAD_GATEWAY						: return setProtocolStatusCode(502, "Bad Gateway"); break;
	case SC_SERVICE_UNAVAILABLE				: return setProtocolStatusCode(503, "Service Unavailable"); break;
	case SC_GATEWAY_TIMEOUT					: return setProtocolStatusCode(504, "Gateway Timeout"); break;
	case SC_HTTP_VERSION_NOT_SUPPORTED		: return setProtocolStatusCode(505, "HTTP Version Not Supported"); break;
	case SC_VARIANT_ALSO_NEGOTIATES			: return setProtocolStatusCode(506, "Variant Also Negotiates"); break;
	case SC_INSUFFICIENT_STORAGE			: return setProtocolStatusCode(507, "Insufficient Storage"); break;
	case SC_LOOP_DETECTED					: return setProtocolStatusCode(508, "Loop Detected"); break;
	case SC_BANDWIDTH_LIMIT_EXCEEDED		: return setProtocolStatusCode(509, "Bandwidth Limit Exceeded"); break;
	case SC_NOT_EXTENDED					: return setProtocolStatusCode(510, "Not Extended"); break;
	case SC_NETWORK_AUTHENTICATION_REQUIRED	: return setProtocolStatusCode(511, "Network Authentication Required"); break;
	case SC_UNKNOWN_ERROR					: return setProtocolStatusCode(520, "Unknown Error"); break;
	case SC_WEB_SERVER_IS_DOWN				: return setProtocolStatusCode(521, "Web Server Is Down"); break;
	case SC_CONNECTION_TIMED_OUT			: return setProtocolStatusCode(522, "Connection Timed Out"); break;
	case SC_ORIGIN_IS_UNREACHABLE			: return setProtocolStatusCode(523, "Origin Is Unreachable"); break;
	case SC_A_TIMEOUT_OCCURRED				: return setProtocolStatusCode(524, "A Timeout Occurred"); break;
	case SC_SSL_HANDSHAKE_FAILED			: return setProtocolStatusCode(525, "SSL Handshake Failed"); break;
	case SC_INVALID_SSL_CERTIFICATE			: return setProtocolStatusCode(526, "Invalid SSL Certificate"); break;

	default:
		Q_ASSERT(false);
		break;
	}

	return false;
}


IRequest* CHttpProtocolEngineComp::CreateRequest(QObject* socketPtr, const IRequestHandler& requestHandler) const
{
	if (socketPtr != nullptr){
		return new CHttpRequest(*socketPtr, requestHandler, *this);
	}

	return nullptr;
}


IRequest* CHttpProtocolEngineComp::CreateRequestForSend(
		QObject* socketPtr,
		const IRequestHandler& requestHandler,
		int statusCode,
		const QByteArray& data,
		const QByteArray& dataTypeId) const
{
	if (socketPtr != nullptr){
		return new CHttpRequest(*socketPtr, requestHandler, *this);
	}

	return nullptr;
}


IResponse* CHttpProtocolEngineComp::CreateResponse(
			const IRequest& request,
			int statusCode,
			const QByteArray& data,
			const QByteArray& dataTypeId) const
{
	return new CHttpResponse(statusCode, data, dataTypeId, request.GetSocketObject(), *this);
}


const ISender& CHttpProtocolEngineComp::GetSender(const IRequest* /*requestPtr*/) const
{
	return m_sender;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CHttpProtocolEngineComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


} // namespace imtrest


