#include <imtrest/CHttpRequest.h>


namespace imtrest
{


static http_parser_settings s_httpParserSettings
{
	&CHttpRequest::OnMessageBegin,
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

CHttpRequest::CHttpRequest(const QAbstractSocket& socket, const IRequestHandler& handler, const IProtocolEngine& engine)
	:m_requestHandler(handler),
	m_engine(engine)
{
	m_remoteAddress = socket.peerAddress();

	http_parser_init(&m_httpParser, HTTP_REQUEST);

	m_httpParser.data = this;

	QObject::connect(&socket, &QAbstractSocket::readyRead, this, &CHttpRequest::HandleReadyRead);
	QObject::connect(&socket, &QAbstractSocket::disconnected, &QObject::deleteLater);
}


// public static methods

int CHttpRequest::OnMessageBegin(http_parser* httpParser)
{
	return 0;
}


int CHttpRequest::OnUrl(http_parser* httpParser, const char *at, size_t length)
{
	return 0;
}


int CHttpRequest::OnStatus(http_parser* httpParser, const char *at, size_t length)
{
	return 0;
}


int CHttpRequest::OnHeaderField(http_parser* httpParser, const char *at, size_t length)
{
	return 0;
}


int CHttpRequest::OnHeaderValue(http_parser*httpParser, const char *at, size_t length)
{
	return 0;
}


int CHttpRequest::OnHeadersComplete(http_parser* httpParser)
{
	return 0;
}


int CHttpRequest::OnBody(http_parser* httpParser, const char *at, size_t length)
{
	return 0;
}


int CHttpRequest::OnMessageComplete(http_parser* httpParser)
{
	return 0;
}


int CHttpRequest::OnChunkHeader(http_parser* httpParser)
{
	return 0;
}


int CHttpRequest::OnChunkComplete(http_parser* httpParser)
{
	return 0;
}


// reimplemented (IRequest)

IRequest::RequestState CHttpRequest::GetState() const
{
	return m_state;
}


QByteArray CHttpRequest::GetRequestId() const
{
	return QByteArray();
}


bool CHttpRequest::ReadFromDevice(QIODevice& device)
{
	QByteArray data = device.readAll();
	if (!data.isEmpty()){
#if QT_CONFIG(ssl)
		QSslSocket* sslSocketPtr = qobject_cast<QSslSocket*>(&device);
		url.setScheme(sslSocketPtr && sslSocketPtr->isEncrypted() ? QStringLiteral("https") : QStringLiteral("http"));
#else
		m_url.setScheme(QStringLiteral("http"));
#endif
		int parsedCount = http_parser_execute(
					&m_httpParser,
					&s_httpParserSettings,
					data.constData(),
					size_t(data.size()));
		if (parsedCount < data.size()){
			return false;
		}
	}

	return true;
}


const IProtocolEngine& CHttpRequest::GetProtocolEngine() const
{
	return m_engine;
}


// private slots

void CHttpRequest::HandleReadyRead()
{
	QAbstractSocket* socketPtr = qobject_cast<QAbstractSocket*>(sender());
	Q_ASSERT(socketPtr != nullptr);

	if (!socketPtr->isTransactionStarted()){
		socketPtr->startTransaction();
	}

#if _DEBUG
	QByteArray data = socketPtr->readAll();
	QString peerName = socketPtr->peerName();
	QString peerAddress = socketPtr->peerAddress().toString();
//	SendVerboseMessage(QString("Incomming data from '%1''%2': %3").arg(peerAddress).arg(peerName).arg(qPrintable(data)));
#endif

	// Get state of request data:
	if (!ReadFromDevice(*socketPtr)){
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

} // namespace imtrest


