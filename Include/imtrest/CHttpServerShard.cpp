// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpServerShard.h>


#ifdef IMTREST_HTTP_SERVER_AVAILABLE


// Qt includes
#include <QtCore/QTimer>
#include <QtCore/QUuid>
#include <QtNetwork/QTcpSocket>
#if QT_CONFIG(ssl)
#include <QtNetwork/QSslSocket>
#endif
#include <QtNetwork/QHttpHeaders>
#include <QtHttpServer/QHttpServerRequest>


namespace imtrest
{


/**
	Maximal size of the request head used for the detection of the WebSocket upgrade requests.
	If the head of the first request is larger, the connection is handled as a normal HTTP connection.
*/
static const qint64 s_maxClassificationDataSize = 64 * 1024;


// public methods of the class CHttpConnectionQueue

CHttpConnectionQueue::CHttpConnectionQueue(QObject* parentPtr)
	:QTcpServer(parentPtr)
{
}


bool CHttpConnectionQueue::Open()
{
	if (!listen(QHostAddress::LocalHost, 0) && !listen(QHostAddress::LocalHostIPv6, 0)){
		return false;
	}

	pauseAccepting();

	return true;
}


void CHttpConnectionQueue::Enqueue(QTcpSocket* socketPtr)
{
	// QAbstractHttpServer takes the socket synchronously (direct connection to pendingConnectionAvailable):
	addPendingConnection(socketPtr);

	// QTcpServer::nextPendingConnection re-enables accepting, the queue must stay closed for real connections:
	pauseAccepting();
}


// protected methods of the class CHttpConnectionQueue

// reimplemented (QTcpServer)

void CHttpConnectionQueue::incomingConnection(qintptr socketDescriptor)
{
	// Real connections to the internal loopback port are not allowed:
	QTcpSocket socket;
	if (socket.setSocketDescriptor(socketDescriptor)){
		socket.abort();
	}

	pauseAccepting();
}


// public methods

CHttpServerShard::CHttpServerShard(
			CHttpTransport& transport,
			IHttpTransportHandler& handler,
			const CHttpTransport::Configuration& configuration)
	:QAbstractHttpServer(),
	m_transport(transport),
	m_handler(handler),
	m_configuration(configuration),
	m_connectionQueuePtr(nullptr)
{
}


CHttpServerShard::~CHttpServerShard()
{
	Shutdown();
}


bool CHttpServerShard::Initialize()
{
	if (m_connectionQueuePtr != nullptr){
		return true;
	}

	CHttpConnectionQueue* connectionQueuePtr = new CHttpConnectionQueue(this);
	if (!connectionQueuePtr->Open()){
		delete connectionQueuePtr;

		return false;
	}

	if (!bind(connectionQueuePtr)){
		delete connectionQueuePtr;

		return false;
	}

	m_connectionQueuePtr = connectionQueuePtr;

	return true;
}


int CHttpServerShard::GetConnectionCount() const
{
	return m_connectionCount.loadRelaxed();
}


void CHttpServerShard::ReserveConnection()
{
	m_connectionCount.ref();
}


void CHttpServerShard::AcceptSocket(qintptr socketDescriptor, bool isSecure, const QSslConfiguration& sslConfiguration)
{
	Q_ASSERT(QThread::currentThread() == thread());

	QTcpSocket* socketPtr = nullptr;
#if QT_CONFIG(ssl)
	QSslSocket* sslSocketPtr = nullptr;
	if (isSecure){
		sslSocketPtr = new QSslSocket(this);
		sslSocketPtr->setSslConfiguration(sslConfiguration);

		socketPtr = sslSocketPtr;
	}
	else{
		socketPtr = new QTcpSocket(this);
	}
#else
	Q_UNUSED(sslConfiguration);
	isSecure = false;

	socketPtr = new QTcpSocket(this);
#endif

	if (!socketPtr->setSocketDescriptor(socketDescriptor)){
		qWarning("CHttpServerShard: socket descriptor could not be taken over: %s", qPrintable(socketPtr->errorString()));

		delete socketPtr;

		ReleaseConnectionSlot();

		return;
	}

	Connection connection;
	connection.socketPtr = socketPtr;
	connection.isSecure = isSecure;
	connection.peerAddress = socketPtr->peerAddress();
	connection.peerPort = socketPtr->peerPort();
	connection.timerPtr = new QTimer(socketPtr);
	connection.timerPtr->setSingleShot(true);

	connect(connection.timerPtr, &QTimer::timeout, this, [this, socketPtr](){ OnConnectionTimeout(socketPtr); });
	connect(socketPtr, &QTcpSocket::readyRead, this, [this, socketPtr](){ OnSocketReadyRead(socketPtr); });
	connect(socketPtr, &QTcpSocket::disconnected, this, [this, socketPtr](){ OnSocketDisconnected(socketPtr); });
	connect(socketPtr, &QObject::destroyed, this, [this, socketPtr](){ ForgetConnection(socketPtr); });

	m_connections.insert(socketPtr, connection);
	m_peerSockets.insert(PeerKey(connection.peerAddress, connection.peerPort), socketPtr);

	RestartTimer(m_connections[socketPtr]);

#if QT_CONFIG(ssl)
	if (sslSocketPtr != nullptr){
		sslSocketPtr->startServerEncryption();
	}
#endif
}


void CHttpServerShard::SendResponse(const QByteArray& requestId, const IHttpTransportHandler::ResponseData& response)
{
	Q_ASSERT(QThread::currentThread() == thread());

	FinishResponse(requestId, response);
}


void CHttpServerShard::Shutdown()
{
	// Release all responders first, QtHttpServer requires them to be destroyed before the connection handlers:
	while (!m_pendingResponses.empty()){
		std::map<QByteArray, PendingResponse>::iterator iter = m_pendingResponses.begin();
		QByteArray requestId = iter->first;

		m_transport.UnregisterRequest(requestId);
		m_pendingResponses.erase(iter);

		m_handler.OnHttpRequestAborted(requestId);
	}

	const QList<QTcpSocket*> sockets = m_connections.keys();
	for (QTcpSocket* socketPtr : sockets){
		if (m_connections.contains(socketPtr)){
			socketPtr->abort();
		}

		if (m_connections.contains(socketPtr)){
			ForgetConnection(socketPtr);
		}
	}

	if (m_connectionQueuePtr != nullptr){
		m_connectionQueuePtr->close();
	}
}


// protected methods

// reimplemented (QAbstractHttpServer)

bool CHttpServerShard::handleRequest(const QHttpServerRequest& request, QHttpServerResponder& responder)
{
	QTcpSocket* socketPtr = m_peerSockets.value(PeerKey(request.remoteAddress(), request.remotePort()), nullptr);

	QHash<QTcpSocket*, Connection>::iterator connectionIter = m_connections.find(socketPtr);
	bool isSecure = (connectionIter != m_connections.end()) ? connectionIter->isSecure : false;

	// QHttpServerRequest is reused by QtHttpServer for the next request on this connection, all data must be copied:
	IHttpTransportHandler::RequestData requestData;
	requestData.requestId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	requestData.method = GetMethodName(request);
	requestData.url = request.url();
	requestData.url.setScheme(isSecure ? QStringLiteral("https") : QStringLiteral("http"));
	if (requestData.url.port() == 0){
		// QtHttpServer reports port 0 if the Host header contains no port
		requestData.url.setPort(-1);
	}
	requestData.body = request.body();
	requestData.remoteAddress = request.remoteAddress();
	requestData.remotePort = request.remotePort();
	requestData.isSecure = isSecure;

	bool isCloseRequested = false;
	bool isKeepAliveRequested = false;

	const QHttpHeaders& headers = request.headers();
	const qsizetype headerCount = headers.size();
	requestData.headers.reserve(headerCount);
	for (qsizetype i = 0; i < headerCount; ++i){
		const QLatin1StringView nameView = headers.nameAt(i);
		QByteArray name(nameView.data(), nameView.size());
		QByteArray value = headers.valueAt(i).toByteArray();

		if (name.compare(QByteArrayLiteral("connection"), Qt::CaseInsensitive) == 0){
			QByteArray connectionValue = value.toLower();
			if (connectionValue.contains(QByteArrayLiteral("close"))){
				isCloseRequested = true;
			}
			else if (connectionValue.contains(QByteArrayLiteral("keep-alive"))){
				isKeepAliveRequested = true;
			}
		}

		requestData.headers.append(qMakePair(name, value));
	}

	if (connectionIter != m_connections.end()){
		// HTTP/1.0 connections are persistent only if the client asked for it explicitly (RFC 9112, 9.3):
		if (connectionIter->isHttp10 && !isKeepAliveRequested){
			isCloseRequested = true;
		}

		connectionIter->pendingRequestCount++;

		RestartTimer(*connectionIter);
	}

	const QByteArray requestId = requestData.requestId;

	// The route must exist before the handler gets the request, because the response can come from any thread:
	m_transport.RegisterRequest(requestId, this);
	m_pendingResponses.emplace(requestId, PendingResponse(std::move(responder), socketPtr, isCloseRequested, isKeepAliveRequested));

	if (!m_handler.OnHttpRequest(requestData)){
		m_transport.UnregisterRequest(requestId);

		IHttpTransportHandler::ResponseData response;
		response.statusCode = int(QHttpServerResponder::StatusCode::ServiceUnavailable);

		FinishResponse(requestId, response);
	}

	return true;
}


void CHttpServerShard::missingHandler(const QHttpServerRequest& request, QHttpServerResponder& responder)
{
	// Called by QtHttpServer for a WebSocket upgrade which was not detected as the first request on a connection:
	if (request.value(QByteArrayLiteral("upgrade")).compare(QByteArrayLiteral("websocket"), Qt::CaseInsensitive) == 0){
		responder.write(QHttpServerResponder::StatusCode::BadRequest);

		// QtHttpServer closes the connection immediately after this call, the answer must be written out now:
		QTcpSocket* socketPtr = m_peerSockets.value(PeerKey(request.remoteAddress(), request.remotePort()), nullptr);
		if (socketPtr != nullptr){
			socketPtr->flush();
		}

		return;
	}

	responder.write(QHttpServerResponder::StatusCode::NotFound);
}


// private methods

void CHttpServerShard::OnSocketReadyRead(QTcpSocket* socketPtr)
{
	QHash<QTcpSocket*, Connection>::iterator connectionIter = m_connections.find(socketPtr);
	if (connectionIter == m_connections.end()){
		return;
	}

	if (connectionIter->state == CS_CLASSIFYING){
		ClassifyConnection(*connectionIter);

		return;
	}

	if (connectionIter->pendingRequestCount == 0){
		RestartTimer(*connectionIter);
	}
}


void CHttpServerShard::OnSocketDisconnected(QTcpSocket* socketPtr)
{
	DropPendingResponses(socketPtr);

	QHash<QTcpSocket*, Connection>::iterator connectionIter = m_connections.find(socketPtr);
	if (connectionIter == m_connections.end()){
		return;
	}

	// Before the hand-over to QtHttpServer the socket is owned by the shard:
	bool isOwned = (connectionIter->state == CS_CLASSIFYING);

	ForgetConnection(socketPtr);

	if (isOwned){
		socketPtr->deleteLater();
	}
}


void CHttpServerShard::OnConnectionTimeout(QTcpSocket* socketPtr)
{
	QHash<QTcpSocket*, Connection>::iterator connectionIter = m_connections.find(socketPtr);
	if (connectionIter == m_connections.end()){
		return;
	}

	if (connectionIter->pendingRequestCount > 0){
		return;
	}

	if (connectionIter->state == CS_HTTP){
		// Idle keep-alive connection:
		socketPtr->disconnectFromHost();

		return;
	}

	// TLS handshake or the first request head was not completed in time:
	socketPtr->abort();

	if (m_connections.contains(socketPtr)){
		ForgetConnection(socketPtr);

		socketPtr->deleteLater();
	}
}


void CHttpServerShard::ClassifyConnection(Connection& connection)
{
	const QByteArray data = connection.socketPtr->peek(s_maxClassificationDataSize);

	qsizetype headEnd = data.indexOf("\r\n\r\n");
	if ((headEnd < 0) && (data.size() < s_maxClassificationDataSize)){
		// Wait for the complete request head:
		return;
	}

	if ((headEnd >= 0) && m_configuration.isWebSocketEnabled && IsWebSocketUpgrade(data.left(headEnd))){
		HandOverToWebSocket(connection);

		return;
	}

	// QtHttpServer does not expose the protocol version of the request, take it from the request line:
	const qsizetype requestLineEnd = data.indexOf("\r\n");
	if (requestLineEnd >= 0){
		connection.isHttp10 = data.left(requestLineEnd).endsWith(QByteArrayLiteral(" HTTP/1.0"));
	}

	HandOverToHttp(connection);
}


void CHttpServerShard::HandOverToHttp(Connection& connection)
{
	Q_ASSERT(m_connectionQueuePtr != nullptr);

	QTcpSocket* socketPtr = connection.socketPtr;

	connection.state = CS_HTTP;

	RestartTimer(connection);

	// QtHttpServer takes the ownership of the socket:
	m_connectionQueuePtr->Enqueue(socketPtr);

	// The request data is already buffered in the socket, trigger the reading in QtHttpServer:
	QMetaObject::invokeMethod(socketPtr, &QTcpSocket::readyRead, Qt::QueuedConnection);
}


void CHttpServerShard::HandOverToWebSocket(Connection& connection)
{
	QTcpSocket* socketPtr = connection.socketPtr;

	delete connection.timerPtr;

	QObject::disconnect(socketPtr, &QTcpSocket::readyRead, this, nullptr);
	QObject::disconnect(socketPtr, &QTcpSocket::disconnected, this, nullptr);
	QObject::disconnect(socketPtr, &QObject::destroyed, this, nullptr);

	ForgetConnection(socketPtr);

	// The WebSocket server works in the thread of the transport:
	socketPtr->setParent(nullptr);
	socketPtr->moveToThread(m_transport.thread());

	m_transport.TransferWebSocket(socketPtr);
}


void CHttpServerShard::RestartTimer(Connection& connection)
{
	if (connection.timerPtr == nullptr){
		return;
	}

	int timeout = (connection.state == CS_CLASSIFYING) ? m_configuration.requestHeaderTimeout : m_configuration.keepAliveTimeout;
	if ((timeout <= 0) || (connection.pendingRequestCount > 0)){
		connection.timerPtr->stop();

		return;
	}

	connection.timerPtr->start(timeout);
}


void CHttpServerShard::ForgetConnection(QTcpSocket* socketPtr)
{
	QHash<QTcpSocket*, Connection>::iterator connectionIter = m_connections.find(socketPtr);
	if (connectionIter == m_connections.end()){
		return;
	}

	PeerKey peerKey(connectionIter->peerAddress, connectionIter->peerPort);
	if (m_peerSockets.value(peerKey, nullptr) == socketPtr){
		m_peerSockets.remove(peerKey);
	}

	m_connections.erase(connectionIter);

	ReleaseConnectionSlot();
}


void CHttpServerShard::DropPendingResponses(QTcpSocket* socketPtr)
{
	QByteArrayList abortedRequestIds;

	std::map<QByteArray, PendingResponse>::iterator iter = m_pendingResponses.begin();
	while (iter != m_pendingResponses.end()){
		if (iter->second.socketPtr != socketPtr){
			++iter;

			continue;
		}

		abortedRequestIds.append(iter->first);

		m_transport.UnregisterRequest(iter->first);

		// Destruction of the responder lets QtHttpServer clean up the connection:
		iter = m_pendingResponses.erase(iter);
	}

	QHash<QTcpSocket*, Connection>::iterator connectionIter = m_connections.find(socketPtr);
	if (connectionIter != m_connections.end()){
		connectionIter->pendingRequestCount = 0;
	}

	for (const QByteArray& requestId : abortedRequestIds){
		m_handler.OnHttpRequestAborted(requestId);
	}
}


void CHttpServerShard::FinishResponse(const QByteArray& requestId, const IHttpTransportHandler::ResponseData& response)
{
	std::map<QByteArray, PendingResponse>::iterator iter = m_pendingResponses.find(requestId);
	if (iter == m_pendingResponses.end()){
		// The client has already disconnected
		return;
	}

	QTcpSocket* socketPtr = iter->second.socketPtr;
	bool closeConnection = iter->second.closeConnection;

	QHttpHeaders headers;
	bool hasConnectionHeader = false;
	for (const QPair<QByteArray, QByteArray>& header : response.headers){
		if (header.first.compare(QByteArrayLiteral("content-length"), Qt::CaseInsensitive) == 0){
			// Is calculated by QHttpServerResponder
			continue;
		}

		if (header.first.compare(QByteArrayLiteral("connection"), Qt::CaseInsensitive) == 0){
			hasConnectionHeader = true;
		}

		headers.append(header.first, header.second);
	}

	if (!hasConnectionHeader){
		if (closeConnection){
			headers.append(QHttpHeaders::WellKnownHeader::Connection, "close");
		}
		else if (iter->second.keepAliveRequested){
			headers.append(QHttpHeaders::WellKnownHeader::Connection, "keep-alive");
		}
	}

	{
		PendingResponse pendingResponse(std::move(iter->second));

		m_pendingResponses.erase(iter);

		pendingResponse.responder.write(response.body, headers, QHttpServerResponder::StatusCode(response.statusCode));

		// Destruction of the responder resumes reading of the next request on this keep-alive connection
	}

	QHash<QTcpSocket*, Connection>::iterator connectionIter = m_connections.find(socketPtr);
	if (connectionIter == m_connections.end()){
		return;
	}

	if (connectionIter->pendingRequestCount > 0){
		connectionIter->pendingRequestCount--;
	}

	RestartTimer(*connectionIter);

	if (closeConnection){
		socketPtr->disconnectFromHost();
	}
}


void CHttpServerShard::ReleaseConnectionSlot()
{
	m_connectionCount.deref();
}


// private static methods

bool CHttpServerShard::IsWebSocketUpgrade(const QByteArray& headerData)
{
	const QList<QByteArray> lines = headerData.split('\n');
	for (qsizetype i = 1; i < lines.size(); ++i){
		const QByteArray& line = lines[i];

		qsizetype separatorIndex = line.indexOf(':');
		if (separatorIndex <= 0){
			continue;
		}

		if (line.left(separatorIndex).trimmed().compare(QByteArrayLiteral("upgrade"), Qt::CaseInsensitive) != 0){
			continue;
		}

		return line.mid(separatorIndex + 1).trimmed().toLower().contains(QByteArrayLiteral("websocket"));
	}

	return false;
}


QByteArray CHttpServerShard::GetMethodName(const QHttpServerRequest& request)
{
	switch (request.method()){
	case QHttpServerRequest::Method::Get:
		return QByteArrayLiteral("GET");
	case QHttpServerRequest::Method::Put:
		return QByteArrayLiteral("PUT");
	case QHttpServerRequest::Method::Delete:
		return QByteArrayLiteral("DELETE");
	case QHttpServerRequest::Method::Post:
		return QByteArrayLiteral("POST");
	case QHttpServerRequest::Method::Head:
		return QByteArrayLiteral("HEAD");
	case QHttpServerRequest::Method::Options:
		return QByteArrayLiteral("OPTIONS");
	case QHttpServerRequest::Method::Patch:
		return QByteArrayLiteral("PATCH");
	case QHttpServerRequest::Method::Connect:
		return QByteArrayLiteral("CONNECT");
	case QHttpServerRequest::Method::Trace:
		return QByteArrayLiteral("TRACE");
	default:
		return QByteArray();
	}
}


} // namespace imtrest


#endif // IMTREST_HTTP_SERVER_AVAILABLE


