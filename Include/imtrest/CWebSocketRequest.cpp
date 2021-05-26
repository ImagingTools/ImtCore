#include <imtrest/CWebSocketRequest.h>


// Qt includes
#include <QtCore/QUrlQuery>
#if QT_CONFIG(ssl)
#include <QtNetwork/QSslSocket>
#endif
#include <QtWebSockets/QWebSocket>


namespace imtrest
{


// public methods

CWebSocketRequest::CWebSocketRequest(QObject& socket, const IRequestHandler& handler, const IProtocolEngine& engine)
	:m_requestHandler(handler),
	m_engine(engine),
	m_socket(socket),
	m_state(RS_NON_STARTED)
{

	QWebSocket* webSocketPtr = dynamic_cast<QWebSocket*>(&socket);
	if (webSocketPtr != nullptr){
		m_remoteAddress = webSocketPtr->peerAddress();

		connect(webSocketPtr, &QWebSocket::textMessageReceived, this, &CWebSocketRequest::OnWebSocketTextMessage);
		connect(webSocketPtr, &QWebSocket::binaryMessageReceived, this, &CWebSocketRequest::OnWebSocketBinaryMessage);
		QObject::connect(webSocketPtr, &QWebSocket::disconnected, &QObject::deleteLater);
	}
}


QByteArrayList CWebSocketRequest::GetHeaders() const
{
	return m_headers.keys();
}


QByteArray CWebSocketRequest::GetHeaderValue(const QByteArray& headerType) const
{
	if (m_headers.contains(headerType)){
		return m_headers[headerType];
	}

	return QByteArray();
}


QUrl CWebSocketRequest::GetUrl() const
{
	return m_url;
}




QByteArray CWebSocketRequest::GetBody() const
{
	return m_body;
}


QHostAddress CWebSocketRequest::GetRemoteAddress() const
{
	return m_remoteAddress;
}

void CWebSocketRequest::SetBody(const QByteArray &body)
{
	this->m_body = body;
}


// reimplemented (IRequest)

IRequest::RequestState CWebSocketRequest::GetState() const
{
	return m_state;
}


QByteArray CWebSocketRequest::GetCommandId() const
{
	return m_url.path().toUtf8();
}


IRequest::CommandParams CWebSocketRequest::GetCommandParams() const
{
	if (m_url.hasQuery()){
		QUrlQuery query(m_url);

		CommandParams params;

		QList<QPair<QString, QString>> items = query.queryItems();
		for (const QPair<QString, QString>& item : items){
			params[item.first] = item.second;
		}

		return params;
	}

	return CommandParams();
}


// reimplemented (INetworkObject)

const IProtocolEngine& CWebSocketRequest::GetProtocolEngine() const
{
	return m_engine;
}


QObject& CWebSocketRequest::GetSocketObject() const
{
	return m_socket;
}


// reimplemented (istd::IChangeable)

bool CWebSocketRequest::ResetData(CompatibilityMode /*mode*/)
{
	m_body.clear();
	m_headers.clear();
	m_lastHeader.clear();
	m_url.clear();

	m_state = RS_NON_STARTED;

	return true;
}


// protected methods

void CWebSocketRequest::OnWebSocketTextMessage(const QString& textMessage)
{
	m_requestHandler.ProcessRequest(*this);
}


void CWebSocketRequest::OnWebSocketBinaryMessage(const QByteArray& dataMessage)
{
	m_requestHandler.ProcessRequest(*this);
}


} // namespace imtrest


