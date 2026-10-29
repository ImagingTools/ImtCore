// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpRequest.h>


// STL includes
#include <climits>

// Qt includes
#include <QtCore/QUrlQuery>
#include <QtCore/QUuid>
#if QT_CONFIG(ssl)
#include <QtNetwork/QSslSocket>
#endif
#include <QtWebSockets/QWebSocket>

// ImtCore includes
#include <imtrest/CConnectionTimeoutBehavior.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/CHttpSender.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


// The size of the body is announced by the client, so the memory is not reserved for more than this at once
static const quint64 s_maxBodyReserveSize = Q_UINT64_C(16) * 1024 * 1024;

static const QByteArray s_continueMessage("HTTP/1.1 100 Continue\r\n\r\n");


namespace
{


// The policy of a request without its own policy: the body is received with the limit of the server
class CDefaultBodyPolicy: virtual public IHttpRequestBodyPolicy
{
public:
	// reimplemented (IHttpRequestBodyPolicy)
	BodyDecision DecideBody(const CHttpRequest& /*requestHead*/) const override
	{
		return {};
	}
};


const IHttpRequestBodyPolicy& GetDefaultBodyPolicy()
{
	static const CDefaultBodyPolicy policy;

	return policy;
}


} // namespace


static http_parser_settings s_httpParserSettings
{
	&CHttpRequest::OnStartMessage,
	&CHttpRequest::OnUrl,
	&CHttpRequest::OnStatus,
	&CHttpRequest::OnHeaderField,
	&CHttpRequest::OnHeaderValue,
	&CHttpRequest::OnHeadersComplete,
	&CHttpRequest::OnBody,
	&CHttpRequest::OnMessageComplete,
	&CHttpRequest::OnChunkHeader,
	&CHttpRequest::OnChunkComplete
};


// public methods

CHttpRequest::CHttpRequest(const IRequestServlet& handler, const IProtocolEngine& engine)
	:m_state(RS_NON_STARTED),
	m_requestHandler(handler),
	m_engine(engine),
	m_bodyPolicy(GetDefaultBodyPolicy())
{
	m_requestId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();

	http_parser_init(&m_httpParser, HTTP_REQUEST);

	m_httpParser.data = this;
}


CHttpRequest::~CHttpRequest()
{
	AbortBodySink();
}


QByteArrayList CHttpRequest::GetHeaders() const
{
	return m_headers.keys();
}


QByteArray CHttpRequest::GetHeaderValue(const QByteArray& headerType) const
{
	const QByteArray headerKey = headerType.toLower();
	if (m_headers.contains(headerKey)){
		return m_headers[headerKey];
	}

	return {};
}


QUrl CHttpRequest::GetUrl() const
{
	return m_url;
}


CHttpRequest::MethodType CHttpRequest::GetMethodType() const
{
	switch (m_httpParser.method){
	case HTTP_GET:
		return MT_GET;
	case HTTP_POST:
		return MT_POST;
	case HTTP_PUT:
		return MT_PUT;
	case HTTP_PATCH:
		return MT_PATCH;
	case HTTP_DELETE:
		return MT_DELETE;
	case HTTP_HEAD:
		return MT_HEAD;
	case HTTP_OPTIONS:
		return MT_OPTIONS;
	default:
		return MT_UNKNOWN;
	}
}


QByteArray CHttpRequest::GetBody() const
{
	return m_body;
}


QHostAddress CHttpRequest::GetRemoteAddress() const
{
	return m_remoteAddress;
}


std::shared_ptr<IHttpRequestBodySink> CHttpRequest::GetBodySink() const
{
	return m_bodySinkPtr;
}


void CHttpRequest::SetHeader(const QByteArray& headerId, const QByteArray& value)
{
	m_headers.insert(headerId.toLower(), value);
}


void CHttpRequest::SetMethodType(const MethodType& methodType)
{
	int method = 0;
	switch (methodType){
	case MT_POST:
		method = HTTP_POST;
		break;
	case MT_GET:
		method = HTTP_GET;
		break;
	case MT_PUT:
		method = HTTP_PUT;
		break;
	default:
		break;
	}
	m_httpParser.method = method;
}


void CHttpRequest::SetBody(const QByteArray& body)
{
	m_body = body;
}


void CHttpRequest::SetCommandId(const QByteArray& commandId)
{
	m_url.setPath(commandId);
}


void CHttpRequest::SetBodyPolicy(const IHttpRequestBodyPolicy& policy)
{
	m_bodyPolicy = policy;
}


void CHttpRequest::ResetBodyPolicy()
{
	m_bodyPolicy = GetDefaultBodyPolicy();
}


void CHttpRequest::SetDefaultMaxBodySize(qint64 maxBodySize)
{
	m_defaultMaxBodySize = maxBodySize;
}


// public static methods

int CHttpRequest::OnStartMessage(http_parser* httpParser)
{
	Q_ASSERT(httpParser != nullptr);

	CHttpRequest* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
	Q_ASSERT(requestPtr != nullptr);

	requestPtr->m_state = CHttpRequest::RS_START_MESSAGE;
	requestPtr->m_urlData.clear();
	requestPtr->m_isHeaderFieldParsing = false;

	return 0;
}


int CHttpRequest::OnUrl(http_parser* httpParser, const char *at, size_t length)
{
	Q_ASSERT(httpParser != nullptr);

	CHttpRequest* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
	Q_ASSERT(requestPtr != nullptr);

	// Parsed in OnHeadersComplete, when all parts of the URL are known.
	requestPtr->m_urlData.append(at, int(length));

	requestPtr->m_state = CHttpRequest::RS_URL;

	return 0;
}


int CHttpRequest::OnStatus(http_parser* httpParser, const char * /*at*/, size_t /*length*/)
{
	Q_ASSERT(httpParser != nullptr);

	CHttpRequest* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
	Q_ASSERT(requestPtr != nullptr);

	requestPtr->m_state = CHttpRequest::RS_STATUS;

	return 0;
}


int CHttpRequest::OnHeaderField(http_parser* httpParser, const char* at, size_t length)
{
	Q_ASSERT(httpParser != nullptr);

	CHttpRequest* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
	Q_ASSERT(requestPtr != nullptr);

	QByteArray headerKeyPart = QByteArray(at, int(length)).toLower();

	// A name after a value starts a new header field, otherwise it continues the current name.
	if (requestPtr->m_isHeaderFieldParsing){
		requestPtr->m_lastHeader.append(headerKeyPart);
	}
	else{
		requestPtr->m_lastHeader = headerKeyPart;
		requestPtr->m_isHeaderFieldParsing = true;
	}

	requestPtr->m_state = CHttpRequest::RS_HEADERS;

	return 0;
}


int CHttpRequest::OnHeaderValue(http_parser* httpParser, const char* at, size_t length)
{
	Q_ASSERT(httpParser != nullptr);

	CHttpRequest* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
	Q_ASSERT(requestPtr != nullptr);

	requestPtr->m_state = CHttpRequest::RS_HEADERS;

	QByteArray& headerValue = requestPtr->m_headers[requestPtr->m_lastHeader];

	// Repeated fields are combined into one list (RFC 9110, 5.3), cookies with "; " (RFC 9113, 8.2.3).
	if (requestPtr->m_isHeaderFieldParsing && (length > 0) && !headerValue.isEmpty()){
		headerValue.append((requestPtr->m_lastHeader == "cookie") ? "; " : ", ");
	}

	headerValue.append(at, int(length));

	requestPtr->m_isHeaderFieldParsing = false;

	return 0;
}


int CHttpRequest::OnHeadersComplete(http_parser* httpParser)
{
	Q_ASSERT(httpParser != nullptr);

	CHttpRequest* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
	Q_ASSERT(requestPtr != nullptr);

	requestPtr->m_state = CHttpRequest::RS_HEADERS_COMPLETE;

	CHttpRequest::ParseUrl(requestPtr->m_urlData.constData(), size_t(requestPtr->m_urlData.size()), false, requestPtr->m_url);

	HeaderMap::const_iterator hostIter = requestPtr->m_headers.constFind("host");
	if (hostIter != requestPtr->m_headers.constEnd()){
		const QByteArray& host = hostIter.value();

		CHttpRequest::ParseUrl(host.constData(), size_t(host.size()), true, requestPtr->m_url);
	}

	// The body is not read yet, so a request can be refused without it
	requestPtr->DecideBody();

	return 0;
}


int CHttpRequest::OnBody(http_parser* httpParser, const char *at, size_t length)
{
	Q_ASSERT(httpParser != nullptr);

	auto* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
	Q_ASSERT(requestPtr != nullptr);

	requestPtr->m_state = CHttpRequest::RS_BODY;

	if ((requestPtr->m_maxBodySize > 0) && (requestPtr->m_receivedBodySize + qint64(length) > requestPtr->m_maxBodySize)){
		// The length of a chunked body is not known in advance, so it is checked while the body is received
		requestPtr->Reject(IProtocolEngine::SC_PAYLOAD_TOO_LARGE, IResponse::Headers());

		return 0;
	}

	if (requestPtr->m_bodySinkPtr){
		if (!requestPtr->m_bodySinkPtr->Write(at, qint64(length))){
			requestPtr->Reject(IProtocolEngine::SC_INTERNAL_SERVER_ERROR, IResponse::Headers());

			return 0;
		}

		requestPtr->m_receivedBodySize += qint64(length);

		return 0;
	}

	if (requestPtr->m_body.isEmpty()){
		const quint64 announcedSize = (httpParser->content_length == ULLONG_MAX) ? 0 : httpParser->content_length;

		requestPtr->m_body.reserve(qsizetype(qMin<quint64>(announcedSize, s_maxBodyReserveSize)) + qsizetype(length));
	}

	requestPtr->m_body.append(at, int(length));
	requestPtr->m_receivedBodySize += qint64(length);

	return 0;
}


int CHttpRequest::OnMessageComplete(http_parser* httpParser)
{
	Q_ASSERT(httpParser != nullptr);

	auto* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
	Q_ASSERT(requestPtr != nullptr);

	requestPtr->m_state = CHttpRequest::RS_MESSAGE_COMPLETE;

	// The parser stops at the end of the request, the next request is parsed by another object
	http_parser_pause(httpParser, 1);

	if (requestPtr->m_bodySinkPtr && !requestPtr->m_isBodySinkDone){
		// The receiver is not aborted after Finish(), even if it fails
		requestPtr->m_isBodySinkDone = true;

		if (!requestPtr->m_bodySinkPtr->Finish()){
			requestPtr->Reject(IProtocolEngine::SC_INTERNAL_SERVER_ERROR, IResponse::Headers());
		}
	}

	return 0;
}


int CHttpRequest::OnChunkHeader(http_parser* httpParser)
{
	Q_ASSERT(httpParser != nullptr);

	auto* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
	Q_ASSERT(requestPtr != nullptr);

	requestPtr->m_state = CHttpRequest::RS_CHUNK_HEADER;

	return 0;
}


int CHttpRequest::OnChunkComplete(http_parser* httpParser)
{
	Q_ASSERT(httpParser != nullptr);

	CHttpRequest* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
	Q_ASSERT(requestPtr != nullptr);

	requestPtr->m_state = CHttpRequest::RS_CHUNK_COMPLETE;

	return 0;
}


// reimplemented (IRequest)

IRequest::RequestState CHttpRequest::GetState() const
{
	return m_state;
}


QByteArray CHttpRequest::GetCommandId() const
{
	return m_url.path().toUtf8();
}


IRequest::CommandParams CHttpRequest::GetCommandParams() const
{
	if (m_url.hasQuery()){
		QUrlQuery query(m_url);

		CommandParams params;

		QList<QPair<QString, QString>> items = query.queryItems();
		for (const QPair<QString, QString>& item : items){
			params[item.first.toUtf8()] = item.second.toUtf8();
		}

		return params;
	}

	return CommandParams();
}


QByteArray CHttpRequest::GetRequestId() const
{
	return m_requestId;
}


bool CHttpRequest::IsConnectionPersistent() const
{
	return http_should_keep_alive(&m_httpParser) != 0;
}


bool CHttpRequest::IsResponseBodyOmitted() const
{
	return m_httpParser.method == HTTP_HEAD;
}


bool CHttpRequest::IsChunkedResponseAllowed() const
{
	return (m_httpParser.http_major > 1) || ((m_httpParser.http_major == 1) && (m_httpParser.http_minor >= 1));
}


IRequest::HeadAction CHttpRequest::TakeHeadAction(QByteArray& message)
{
	message.clear();

	if (m_rejectStatusCode != 0){
		message = CreateRejectMessage();

		return HA_REJECT;
	}

	if (m_isContinueExpected){
		m_isContinueExpected = false;

		// If a part of the body is received already, the client does not wait (RFC 9110, section 10.1.1)
		if (m_state == RS_HEADERS_COMPLETE){
			message = s_continueMessage;

			return HA_SEND_INTERIM;
		}
	}

	return HA_NONE;
}


// reimplemented (INetworkObject)

const IProtocolEngine& CHttpRequest::GetProtocolEngine() const
{
	return m_engine;
}


// reimplemented (istd::IChangeable)

bool CHttpRequest::ResetData(CompatibilityMode /*mode*/)
{
	AbortBodySink();
	m_bodySinkPtr.reset();
	m_isBodySinkDone = false;
	m_receivedBodySize = 0;

	m_body.clear();
	m_headers.clear();
	m_lastHeader.clear();
	m_isHeaderFieldParsing = false;
	m_urlData.clear();
	m_url.clear();
	m_data.clear();

	m_maxBodySize = 0;
	m_rejectHeaders.clear();
	m_isContinueExpected = false;

	if (m_rejectStatusCode != 0){
		// The parser was stopped in the middle of the request, it cannot go on with the next one
		m_rejectStatusCode = 0;

		http_parser_init(&m_httpParser, HTTP_REQUEST);

		m_httpParser.data = this;
	}

	// The object can parse the next request
	if (HTTP_PARSER_ERRNO(&m_httpParser) == HPE_PAUSED){
		http_parser_pause(&m_httpParser, 0);
	}

	m_state = RS_NON_STARTED;

	return true;
}


// protected methods

bool CHttpRequest::ParseDeviceData(QIODevice& device)
{
	// The data are not taken from the device before it is known how many of them belong to the request.
	const QByteArray data = device.peek(device.bytesAvailable());
	if (data.isEmpty()){
		return true;
	}

	qsizetype consumedCount = 0;
	const bool isParsed = ExecuteHttpParser(data, &device, consumedCount);

	device.skip(consumedCount);

	return isParsed;
}


bool CHttpRequest::ExecuteHttpParser(const QByteArray& data, const QObject* socketObjectPtr)
{
	qsizetype consumedCount = 0;

	return ExecuteHttpParser(data, socketObjectPtr, consumedCount);
}


bool CHttpRequest::ExecuteHttpParser(const QByteArray& data, const QObject* socketObjectPtr, qsizetype& consumedCount)
{
	consumedCount = 0;

#if QT_CONFIG(ssl)
	const QSslSocket* sslSocketPtr = qobject_cast<const QSslSocket*>(socketObjectPtr);
	if ((sslSocketPtr != nullptr) && sslSocketPtr->isEncrypted()){
		m_url.setScheme(QStringLiteral("https"));
	}
	else{
		m_url.setScheme(QStringLiteral("http"));
	}

	const QWebSocket* webSocketPtr = qobject_cast<const QWebSocket*>(socketObjectPtr);
	if (webSocketPtr != nullptr){
		m_url.setScheme(QStringLiteral("ws"));
	}
#else
	const QWebSocket* webSocketPtr = qobject_cast<QWebSocket*>(socketObjectPtr);
	if (webSocketPtr != nullptr){
		m_url.setScheme(QStringLiteral("ws"));
	}
	else{
		m_url.setScheme(QStringLiteral("http"));
	}
#endif

	const qsizetype parsedCount = qsizetype(http_parser_execute(
		&m_httpParser,
		&s_httpParserSettings,
		data.constData(),
		size_t(data.size())));

	consumedCount = parsedCount;

	// The request is complete, the parser is paused at its end (see OnMessageComplete()). The rest belongs to the next message.
	if (HTTP_PARSER_ERRNO(&m_httpParser) == HPE_PAUSED){
		return true;
	}

	if (parsedCount < data.size()){
		QByteArray dataMin = data.left(100);
		qDebug() << "parsedCount < data.size" << parsedCount << data.size() << "\t\t" << dataMin.toHex(' ');

		return false;
	}

	return true;
}


// private methods

void CHttpRequest::DecideBody()
{
	const IHttpRequestBodyPolicy::BodyDecision decision = m_bodyPolicy.get().DecideBody(*this);

	if (decision.mode == IHttpRequestBodyPolicy::BM_REJECT){
		// The status is required for the refusal, a policy without it is a mistake of the server
		const int statusCode = (decision.rejectStatusCode != IProtocolEngine::SC_NONE) ? decision.rejectStatusCode : int(IProtocolEngine::SC_INTERNAL_SERVER_ERROR);

		Reject(statusCode, decision.rejectHeaders);

		return;
	}

	if (decision.mode == IHttpRequestBodyPolicy::BM_STREAM){
		if (!decision.sinkPtr){
			// A policy that streams the body has to give the receiver, it is a mistake of the server
			Reject(IProtocolEngine::SC_INTERNAL_SERVER_ERROR, IResponse::Headers());

			return;
		}

		// The receiver is set before the other checks, so it is aborted if the request is refused
		m_bodySinkPtr = decision.sinkPtr;
		m_isBodySinkDone = false;
	}

	m_maxBodySize = (decision.maxBodySize < 0) ? m_defaultMaxBodySize : decision.maxBodySize;

	const bool isChunked = (m_httpParser.flags & F_CHUNKED) != 0;
	const bool isLengthKnown = m_httpParser.content_length != ULLONG_MAX;

	if ((m_maxBodySize > 0) && isLengthKnown && (m_httpParser.content_length > quint64(m_maxBodySize))){
		Reject(IProtocolEngine::SC_PAYLOAD_TOO_LARGE, IResponse::Headers());

		return;
	}

	const QByteArray expectation = GetHeaderValue("expect").trimmed();
	if (expectation.isEmpty()){
		return;
	}

	// The only expectation of HTTP is 100-continue, other expectations cannot be met (RFC 9110, section 10.1.1)
	if (expectation.compare("100-continue", Qt::CaseInsensitive) != 0){
		Reject(IProtocolEngine::SC_EXPECTATION_FAILED, IResponse::Headers());

		return;
	}

	// A server does not send 1xx responses to HTTP/1.0 and ignores the expectation (RFC 9110, sections 10.1.1 and 15.2),
	// a client does not wait for 100 if there is no body
	const bool isHttp11 = (m_httpParser.http_major > 1) || ((m_httpParser.http_major == 1) && (m_httpParser.http_minor >= 1));
	const bool isBodyExpected = isChunked || (isLengthKnown && (m_httpParser.content_length > 0));

	m_isContinueExpected = isHttp11 && isBodyExpected;
}


void CHttpRequest::Reject(int statusCode, const IResponse::Headers& headers)
{
	AbortBodySink();

	m_rejectStatusCode = statusCode;
	m_rejectHeaders = headers;
	m_isContinueExpected = false;

	// The parser stops, the rest of the request is not needed
	http_parser_pause(&m_httpParser, 1);
}


void CHttpRequest::AbortBodySink()
{
	if (m_bodySinkPtr && !m_isBodySinkDone){
		m_isBodySinkDone = true;

		m_bodySinkPtr->Abort();
	}
}


QByteArray CHttpRequest::CreateRejectMessage() const
{
	IResponseUniquePtr responsePtr = m_engine.CreateResponse(*this, m_rejectStatusCode, QByteArray(), QByteArray());
	if (responsePtr.IsValid()){
		auto* httpResponsePtr = dynamic_cast<CHttpResponse*>(responsePtr.GetPtr());
		if (httpResponsePtr != nullptr){
			IResponse::Headers headers = httpResponsePtr->GetHeaders();
			for (IResponse::Headers::const_iterator iter = m_rejectHeaders.constBegin(); iter != m_rejectHeaders.constEnd(); ++iter){
				headers.insert(iter.key(), iter.value());
			}

			httpResponsePtr->SetHeaders(headers);
		}

		// The rest of the request is not received, so the connection is closed after the response
		CHttpSender::ResponseOptions options;
		options.isConnectionClosing = true;
		options.isBodyOmitted = IsResponseBodyOmitted();

		QByteArray data;
		if (CHttpSender::BuildResponseData(*responsePtr.GetPtr(), data, options)){
			return data;
		}
	}

	qWarning() << "The response with the status" << m_rejectStatusCode << "cannot be built";

	return CConnectionTimeoutBehavior::CreateHttpInternalServerErrorResponse(QDateTime::currentDateTimeUtc(), true);
}


// private static methods

bool CHttpRequest::ParseUrl(const char* at, size_t length, bool connect, QUrl& url)
{
	struct http_parser_url parserUrl;
	http_parser_url_init(&parserUrl);

	int schemeMask = 1 << UF_SCHEMA;
	int hostMask = 1 << UF_HOST;
	int portMask = 1 << UF_PORT;
	int pathMask = 1 << UF_PATH;
	int queryMask = 1 << UF_QUERY;
	int fragmentMask = 1 << UF_FRAGMENT;
	int userInfoMask = 1 << UF_USERINFO;

	if (http_parser_parse_url(at, length, connect ? 1 : 0, &parserUrl) == 0){
		for (int i = 0; i < UF_MAX; i++){
			int mask = 1 << i;

			QString value = QString::fromUtf8(at + parserUrl.field_data[i].off, parserUrl.field_data[i].len);

			if ((parserUrl.field_set & schemeMask) && (schemeMask == mask)){
				url.setScheme(value);
			}

			if ((parserUrl.field_set & hostMask) && (hostMask == mask)){
				url.setHost(value);
			}

			if ((parserUrl.field_set & portMask) && (portMask == mask)){
				url.setPort(value.toInt());
			}

			if ((parserUrl.field_set & pathMask) && (pathMask == mask)){
				url.setPath(value, QUrl::TolerantMode);
			}

			if ((parserUrl.field_set & queryMask) && (queryMask == mask)){
				url.setQuery(value);
			}

			if ((parserUrl.field_set & fragmentMask) && (fragmentMask == mask)){
				url.setFragment(value);
			}

			if ((parserUrl.field_set & userInfoMask) && (userInfoMask == mask)){
				url.setUserInfo(value);
			}
		}

		return true;
	}

	return false;
}


} // namespace imtrest


