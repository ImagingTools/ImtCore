// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CMultiThreadServer.h>
#include <imtrest/CServerSslTools.h>
#include <imtrest/CSocketTimeoutWatcher.h>
#include <imtrest/CHttpRequest.h>

// Qt includes
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslKey>


namespace imtrest
{


namespace
{


// While a request is handled and its response is sent, nobody reads the socket, and Qt does not limit its read buffer by default:
// a client could fill the memory of the server with the data of the next request, that is not parsed yet and so is not limited by
// the policy of the body or by the timeouts. With the limit Qt stops reading from the network, and TCP slows the client down. The limit is not exact:
// the part that is read already stays in the buffer (for TLS it is a whole record or more).
const qint64 s_busyReadBufferSize = qint64(64) * 1024;


} // namespace


CSocket::CSocket(CSocketThread* rootSocket, IRequest* request, bool secureConnection, const QSslConfiguration& sslConfiguration, qintptr socketDescriptor)
	:QObject(),
	m_rootSocket(rootSocket),
	m_requestPtr(request)
{
	if (secureConnection){
		m_socket = new QSslSocket();

#if 0
		qDebug() << "SSL Build Version:" << QSslSocket::sslLibraryBuildVersionString();
		qDebug() << "Supports SSL:" << QSslSocket::supportsSsl();
		qDebug() << "SSL Library Version:" << QSslSocket::sslLibraryVersionString();
#endif
		Q_ASSERT(QSslSocket::supportsSsl());
	}
	else {
		m_socket = new QTcpSocket();
	}

	if (!m_socket->setSocketDescriptor(socketDescriptor)){
		qDebug() << "Socket error" << m_socket->error();

		// something's wrong, we just Q_EMIT a signal
		Q_EMIT m_rootSocket->Error(m_socket->error());

		return;
	}

	if (secureConnection){
		QSslSocket* socketPtr = dynamic_cast<QSslSocket*>(m_socket.data());
		connect(socketPtr, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(OnHandleSslErrors(QList<QSslError>)));

		Q_ASSERT_X(socketPtr != nullptr, __func__, "Invalid socket!");
		CServerSslTools::ApplyConfiguration(*socketPtr, sslConfiguration);

		socketPtr->startServerEncryption();
	}

	m_behavior = CConnectionTimeoutBehavior::CreateForProtocol(m_rootSocket->GetProtocolTypeId());

	m_timeoutWatcherPtr = new CSocketTimeoutWatcher(*m_socket, m_rootSocket->GetConnectionTimeoutConfig(), m_behavior, this);

	if (m_behavior.createStreamingResponseSender){
		m_streamingSenderPtr = m_behavior.createStreamingResponseSender(*m_socket, *m_timeoutWatcherPtr, m_rootSocket->GetWriteHighWatermark());
	}

	connect(m_socket.data(), &QTcpSocket::disconnected, this, &CSocket::Disconnected, Qt::DirectConnection);
	connect(m_socket.data(), &QTcpSocket::readyRead, this, &CSocket::HandleReadyRead, Qt::DirectConnection);

	// After the watcher, that is connected earlier: it has registered the written data when the request is finished
	connect(m_socket.data(), &QTcpSocket::bytesWritten, this, &CSocket::OnBytesWritten, Qt::DirectConnection);
}


CSocket::~CSocket()
{
	if (!m_socket.isNull()){
		m_socket->abort();
		m_socket->deleteLater();
	}
}


void CSocket::Abort()
{
	if (!m_socket.isNull() && m_socket->isValid()){
		m_socket->abort();
		m_socket->deleteLater();
	}
}


void CSocket::OnHandleSslErrors(QList<QSslError> errorList)
{
	for (const QSslError& error : errorList){
		qDebug() << error.errorString();
	}
}


void CSocket::HandleReadyRead()
{
	// The connection is passed to the WebSocket server, see StartWebSocketUpgrade()
	if (m_timeoutWatcherPtr == nullptr){
		return;
	}

	if (m_timeoutWatcherPtr->IsClosing() || m_timeoutWatcherPtr->IsCloseRequested()){
		// The client should only close the connection after the response of the timeout or of the rejection (RFC 9112,
		// section 9.6) and should not send requests after a request that asked to close it (RFC 9112, section 9.6).
		m_socket->readAll();

		return;
	}

	// The responses go in the order of the requests, so the next request waits for the response to the current one.
	// Its data stay in the socket and are parsed after the response, see OnSendResponse().
	if (m_isRequestInProgress){
		return;
	}

	imtrest::IRequestServlet* requestHandlerPtr = m_rootSocket->GetRequestServlet();
	if (requestHandlerPtr == nullptr){
		// The server is not configured: there is nobody to process requests, the data are useless
		qWarning() << "The server has no request handler";

		m_socket->readAll();

		return;
	}

	if (m_socket->bytesAvailable() <= 0){
		return;
	}

	m_timeoutWatcherPtr->OnDataReceived();

	if (!m_requestPtr.IsValid()){
		m_requestPtr.SetPtr(m_rootSocket->CreateRequest().PopInterfacePtr());
	}

	if (!m_requestPtr.IsValid()){
		qWarning() << "The request cannot be created";

		m_socket->readAll();

		return;
	}

	// The head of a request that can switch to WebSocket stays restorable: the WebSocket server reads the handshake itself
	if ((m_rootSocket->GetWebSocketUpgradeHandler() != nullptr) && (m_requestPtr->GetState() == IRequest::RS_NON_STARTED) && !m_socket->isTransactionStarted()){
		m_socket->startTransaction();
	}

	// The request takes only its own data from the socket, so they are not kept twice and the data of the next
	// request are not mixed with it.
	if (!m_requestPtr->ParseDeviceData(*m_socket)){
		if (m_socket->isTransactionStarted()){
			m_socket->commitTransaction();
		}

		RejectRequest();

		return;
	}

	if (m_socket->isTransactionStarted()){
		// The states after the head follow RS_HEADERS_COMPLETE
		if (m_requestPtr->GetState() < IRequest::RS_HEADERS_COMPLETE){
			return;
		}

		if (StartWebSocketUpgrade()){
			return;
		}

		m_socket->commitTransaction();
	}

	// The client can wait for the reaction to the head of the request before it sends the body
	QByteArray headMessage;
	switch (m_requestPtr->TakeHeadAction(headMessage)){
	case IRequest::HA_REJECT:
		RejectRequest(headMessage);

		return;

	case IRequest::HA_SEND_INTERIM:
		m_timeoutWatcherPtr->WriteInterim(headMessage);
		break;

	case IRequest::HA_NONE:
		break;
	}

	// Not all data of the request are received:
	if (m_requestPtr->GetState() != IRequest::RS_MESSAGE_COMPLETE){
		return;
	}

	const bool isConnectionClosing = !m_requestPtr->IsConnectionPersistent();
	m_rootSocket->SetResponseHints(isConnectionClosing, m_requestPtr->IsResponseBodyOmitted(), m_requestPtr->IsChunkedResponseAllowed());

	m_timeoutWatcherPtr->OnRequestCompleted();

	if (isConnectionClosing){
		m_timeoutWatcherPtr->CloseAfterResponse();
	}

	// The response comes through OnSendResponse() later, so the flag is set before the handler starts.
	m_isRequestInProgress = m_behavior.IsRequestResponseCycle();
	if (m_isRequestInProgress){
		m_socket->setReadBufferSize(s_busyReadBufferSize);
	}

	// Start request handler. A failure of the handler must not take the connection thread down and must not leave the
	// client without a response.
	IRequest* requestPtr = m_requestPtr.PopPtr();

	try{
		requestHandlerPtr->ProcessRequest(*requestPtr);
	}
	catch (...){
		// The request belongs to the handler since the call and it is not known whether the handler has taken it, so it is not deleted
		// here: a possible leak is better than deleting a request that the handler uses. The handler is expected not to throw (the
		// exception is possible only if it runs out of memory), so this branch is a safety net and normally is never executed.
		qWarning() << "The request handler failed";

		if (m_behavior.createServerErrorMessage){
			OnSendResponse(m_behavior.createServerErrorMessage());
		}
	}
}


void CSocket::Disconnected()
{
	Q_EMIT m_rootSocket->SocketDisconnected(m_rootSocket->GetRequestId());
}


void CSocket::OnSendResponse(QByteArray data)
{
	if (!m_socket.isNull() && m_socket->isOpen() && (m_timeoutWatcherPtr != nullptr)){
		m_timeoutWatcherPtr->WriteResponse(data);
	}

	FinishRequest();
}


void CSocket::OnSendStreamingResponse(imtrest::ConstResponsePtr response, bool isConnectionClosing, bool isBodyOmitted, bool isChunkedAllowed)
{
	if (!m_streamingSenderPtr){
		qWarning() << "The protocol of the connection has no streaming responses";

		SendServerError();

		return;
	}

	IStreamingResponseSender::Hints hints;
	hints.isConnectionClosing = isConnectionClosing;
	hints.isBodyOmitted = isBodyOmitted;
	hints.isChunkedAllowed = isChunkedAllowed;

	const IStreamingResponseSender::StartResult result = m_streamingSenderPtr->Start(response, hints, [this](){
		FinishRequest();
	});

	switch (result){
	case IStreamingResponseSender::SR_STARTED:
		// The request is finished by the handler, when the body is sent
		break;

	case IStreamingResponseSender::SR_FAILED:
		SendServerError();
		break;

	case IStreamingResponseSender::SR_COMPLETED:
	case IStreamingResponseSender::SR_DROPPED:
		FinishRequest();
		break;
	}
}


// private methods

void CSocket::OnBytesWritten()
{
	if (m_isFinishPending){
		FinishRequest();
	}
}


void CSocket::FinishRequest()
{
	if (!m_isRequestInProgress){
		return;
	}

	// The response is written to the socket, but a slow client can be still receiving it. The next request waits until the written data
	// are sent, otherwise the responses of the pipelined requests pile up in the memory. The read limit stays meanwhile.
	if (!m_socket.isNull() && m_socket->isOpen() && (m_socket->bytesToWrite() > 0)){
		m_isFinishPending = true;

		return;
	}

	m_isFinishPending = false;
	m_isRequestInProgress = false;

	// The socket is read again: the limit is removed, so Qt goes on reading from the network
	if (!m_socket.isNull()){
		m_socket->setReadBufferSize(0);
	}

	// The next request can be already in the socket, no signal will tell about it
	if (!m_socket.isNull() && (m_socket->bytesAvailable() > 0)){
		QMetaObject::invokeMethod(this, &CSocket::HandleReadyRead, Qt::QueuedConnection);
	}
}


void CSocket::SendServerError()
{
	if (m_behavior.createServerErrorMessage){
		OnSendResponse(m_behavior.createServerErrorMessage());
	}
	else{
		FinishRequest();
	}
}


void CSocket::RejectRequest()
{
	qDebug() << "The data are not a valid request";

	// The rest of the data is not a request, nothing can be done with it
	m_socket->readAll();

	m_timeoutWatcherPtr->RejectRequest();
}


void CSocket::RejectRequest(const QByteArray& message)
{
	// The rest of the request is not needed and is not handled
	m_socket->readAll();
	m_requestPtr.Reset();

	m_timeoutWatcherPtr->RejectRequest(message);
}


bool CSocket::StartWebSocketUpgrade()
{
	const CHttpRequest* httpRequestPtr = dynamic_cast<const CHttpRequest*>(m_requestPtr.GetPtr());
	if ((httpRequestPtr == nullptr) || (httpRequestPtr->GetMethodType() != CHttpRequest::MT_GET)){
		return false;
	}

	if (httpRequestPtr->GetHeaderValue(QByteArrayLiteral("upgrade")).trimmed().toLower() != QByteArrayLiteral("websocket")){
		return false;
	}

	// Put the handshake back into the read buffer, QWebSocketServer parses it itself
	m_socket->rollbackTransaction();
	m_requestPtr.Reset();

	// The connection belongs to the WebSocket server from now on: nothing here watches it or writes to it
	disconnect(m_socket.data(), nullptr, this, nullptr);

	m_streamingSenderPtr.reset();

	delete m_timeoutWatcherPtr;
	m_timeoutWatcherPtr = nullptr;

	// The socket may not change its thread while it is emitting readyRead()
	QMetaObject::invokeMethod(this, &CSocket::HandOverWebSocket, Qt::QueuedConnection);

	return true;
}


void CSocket::HandOverWebSocket()
{
	QTcpSocket* socketPtr = m_socket.data();
	m_socket.clear();

	if (socketPtr != nullptr){
		// Called in the thread owning the socket, the handler moves it to its own thread
		IWebSocketUpgradeHandler* upgradeHandlerPtr = m_rootSocket->GetWebSocketUpgradeHandler();
		if (!upgradeHandlerPtr->HandleWebSocketUpgrade(socketPtr)){
			socketPtr->abort();
			socketPtr->deleteLater();
		}
	}

	// The connection thread is not needed anymore
	Disconnected();
}


} // namespace imtrest


