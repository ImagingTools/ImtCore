#include <imtrest/CHttpRequest.h>


// Qt includes
#include <QtCore/QUrlQuery>
#if QT_CONFIG(ssl)
#include <QtNetwork/QSslSocket>
#endif
#include <QtWebSockets/QWebSocket>


namespace imtrest
{


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

CHttpRequest::CHttpRequest(QObject& socket, const IRequestServlet& handler, const IProtocolEngine& engine)
	:m_requestHandler(handler),
	m_engine(engine),
	m_socket(socket),
	m_state(RS_NON_STARTED)
{
	QAbstractSocket* socketPtr = dynamic_cast<QAbstractSocket*>(&socket);
	if (socketPtr != nullptr){
		m_remoteAddress = socketPtr->peerAddress();
		QObject::connect(socketPtr, &QAbstractSocket::readyRead, this, &CHttpRequest::HandleReadyRead);
		QObject::connect(socketPtr, &QAbstractSocket::disconnected, &QObject::deleteLater);
	}
	else{
		QWebSocket* webSocketPtr = dynamic_cast<QWebSocket*>(&socket);
		if (webSocketPtr != nullptr){
			m_remoteAddress = webSocketPtr->peerAddress();

			connect(webSocketPtr, &QWebSocket::textMessageReceived, this, &CHttpRequest::OnWebSocketTextMessage);
			connect(webSocketPtr, &QWebSocket::binaryMessageReceived, this, &CHttpRequest::OnWebSocketBinaryMessage);
			QObject::connect(webSocketPtr, &QWebSocket::disconnected, &QObject::deleteLater);
		}
	}

	http_parser_init(&m_httpParser, HTTP_REQUEST);

	m_httpParser.data = this;

}


QByteArrayList CHttpRequest::GetHeaders() const
{
	return m_headers.keys();
}


QByteArray CHttpRequest::GetHeaderValue(const QByteArray& headerType) const
{
	if (m_headers.contains(headerType)){
		return m_headers[headerType];
	}

	return QByteArray();
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


// public static methods

int CHttpRequest::OnStartMessage(http_parser* httpParser)
{
	Q_ASSERT(httpParser != nullptr);

	CHttpRequest* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
	Q_ASSERT(requestPtr != nullptr);

	requestPtr->m_state = CHttpRequest::RS_START_MESSAGE;

	return 0;
}


int CHttpRequest::OnUrl(http_parser* httpParser, const char *at, size_t length)
{
	Q_ASSERT(httpParser != nullptr);

	CHttpRequest* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
	Q_ASSERT(requestPtr != nullptr);

	CHttpRequest::ParseUrl(at, length, false, requestPtr->m_url);

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

	QByteArray headerKey = QByteArray(at, int(length));
	requestPtr->m_headers[headerKey] = QByteArray();

	requestPtr->m_lastHeader = headerKey;

	requestPtr->m_state = CHttpRequest::RS_HEADERS;

	return 0;
}


int CHttpRequest::OnHeaderValue(http_parser* httpParser, const char* at, size_t length)
{
	Q_ASSERT(httpParser != nullptr);

	CHttpRequest* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
	Q_ASSERT(requestPtr != nullptr);

	requestPtr->m_state = CHttpRequest::RS_HEADERS;

	QByteArray headerValue = QByteArray(at, int(length));
	requestPtr->m_headers[requestPtr->m_lastHeader] = headerValue;

	if (requestPtr->m_lastHeader == "host"){
		CHttpRequest::ParseUrl(at, length, true,requestPtr->m_url);
	}

	return 0;
}


int CHttpRequest::OnHeadersComplete(http_parser* httpParser)
{
	Q_ASSERT(httpParser != nullptr);

	CHttpRequest* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
	Q_ASSERT(requestPtr != nullptr);

	requestPtr->m_state = CHttpRequest::RS_HEADERS_COMPLETE;

	return 0;
}


int CHttpRequest::OnBody(http_parser* httpParser, const char *at, size_t length)
{
	Q_ASSERT(httpParser != nullptr);

	CHttpRequest* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
	Q_ASSERT(requestPtr != nullptr);

	requestPtr->m_state = CHttpRequest::RS_BODY;

	if (requestPtr->m_body.isEmpty()){
		requestPtr->m_body.reserve(static_cast<int>(httpParser->content_length) + static_cast<int>(length));
	}

	requestPtr->m_body.append(at, int(length));

	return 0;
}


int CHttpRequest::OnMessageComplete(http_parser* httpParser)
{
	Q_ASSERT(httpParser != nullptr);

	CHttpRequest* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
	Q_ASSERT(requestPtr != nullptr);

	requestPtr->m_state = CHttpRequest::RS_MESSAGE_COMPLETE;

	return 0;
}


int CHttpRequest::OnChunkHeader(http_parser* httpParser)
{
	Q_ASSERT(httpParser != nullptr);

	CHttpRequest* requestPtr = static_cast<CHttpRequest*>(httpParser->data);
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


// reimplemented (INetworkObject)

const IProtocolEngine& CHttpRequest::GetProtocolEngine() const
{
	return m_engine;
}


QObject& CHttpRequest::GetSocketObject() const
{
	return m_socket;
}


// reimplemented (istd::IChangeable)

bool CHttpRequest::ResetData(CompatibilityMode /*mode*/)
{
	m_body.clear();
	m_headers.clear();
	m_lastHeader.clear();
	m_url.clear();

	m_state = RS_NON_STARTED;

	return true;
}


// protected methods

bool CHttpRequest::ParseDeviceData(QIODevice& device)
{
	QByteArray data = device.readAll();
	if (!data.isEmpty()){
		return ExecuteHttpParser(data, &device);
	}

	return true;
}


bool CHttpRequest::ExecuteHttpParser(const QByteArray& data, const QObject* socketObjectPtr)
{
#if QT_CONFIG(ssl)
	const QSslSocket* sslSocketPtr = qobject_cast<const QSslSocket*>(socketObjectPtr);
	if ((sslSocketPtr != nullptr) && sslSocketPtr->isEncrypted()){
		m_url.setScheme(QStringLiteral("https"));
	}
	else{
		const QWebSocket* webSocketPtr = qobject_cast<const QWebSocket*>(socketObjectPtr);
		if (webSocketPtr != nullptr){
			m_url.setScheme(QStringLiteral("ws"));
		}
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

	int parsedCount = http_parser_execute(
		&m_httpParser,
		&s_httpParserSettings,
		data.constData(),
		size_t(data.size()));
	if (parsedCount < data.size()){
		return false;
	}

	return true;
}


// private slots

void CHttpRequest::HandleReadyRead()
{
	QAbstractSocket* socketPtr = qobject_cast<QAbstractSocket*>(sender());
	Q_ASSERT(socketPtr != nullptr);

	if (!socketPtr->isTransactionStarted()){
		socketPtr->startTransaction();
	}

	if (m_state == RS_MESSAGE_COMPLETE){
		ResetData();
	}

	// Get state of request data:
	if (!ParseDeviceData(*socketPtr)){
		socketPtr->disconnect();

		return;
	}

	// Not all data was read:
	if (m_state != RS_MESSAGE_COMPLETE){
		return;
	}

	// All request data was read:
	socketPtr->commitTransaction();

	// Start request handler:
	m_requestHandler.ProcessRequest(*this);
}


void CHttpRequest::OnWebSocketTextMessage(const QString& textMessage)
{
	if (ExecuteHttpParser(textMessage.toUtf8(), sender())){
		m_requestHandler.ProcessRequest(*this);
	}
}


void CHttpRequest::OnWebSocketBinaryMessage(const QByteArray& dataMessage)
{
	if (ExecuteHttpParser(dataMessage, sender())){
		m_requestHandler.ProcessRequest(*this);
	}
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


