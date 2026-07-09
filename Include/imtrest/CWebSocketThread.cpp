// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWebSocketThread.h>


// Qt includes
#include <QtCore/QMutableListIterator>
#include <QtCore/QCoreApplication>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CWebSocketServerComp.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtrest/CHttpRequest.h>


namespace imtrest
{


CWebSocket::CWebSocket(CWebSocketThread *parent)
{
	Q_ASSERT(parent);

	m_parent = parent;
	m_isProcessing = false;
	m_isDisconnectPending = false;
	m_isDisconnected = false;
}


void CWebSocket::OnWebSocketTextMessage(const QString& textMessage)
{
	// Ignore messages delivered after the disconnect handling:
	if (m_isDisconnected){
		return;
	}

	// Serialize the message processing: if the processing pumps a nested event loop
	// (e.g. a remote authorization validation waiting for a reply), the next queued
	// message of this socket could be delivered re-entrantly; defer it instead.
	if (m_isProcessing){
		m_pendingMessages.append(textMessage);
		return;
	}

	m_isProcessing = true;

	m_parent->OnWebSocketTextMessage(textMessage);

	while (!m_pendingMessages.isEmpty() && !m_isDisconnectPending){
		QString pendingMessage = m_pendingMessages.takeFirst();
		m_parent->OnWebSocketTextMessage(pendingMessage);
	}

	m_isProcessing = false;

	if (m_isDisconnectPending){
		m_isDisconnectPending = false;
		HandleDisconnect();
	}
}


void CWebSocket::OnBinaryMessage(const QByteArray& dataMessage)
{
	m_parent->OnWebSocketBinaryMessage(dataMessage);
}


void CWebSocket::OnDisconnected()
{
	// If the disconnect is delivered from a nested event loop while a message is
	// being processed, defer the cleanup until the processing has finished, so that
	// no network request is deleted while it is still in use.
	if (m_isProcessing){
		m_isDisconnectPending = true;
		return;
	}

	HandleDisconnect();
}


void CWebSocket::OnError(QAbstractSocket::SocketError error)
{
	m_parent->OnError(error);
}


// private methods

void CWebSocket::HandleDisconnect()
{
	m_isDisconnected = true;

	// Explicitly clean up subscription requests parented to this handler.
	// Their destructors call OnRequestDestroyed on publishers, cleanly
	// unregistering subscriptions before the QWebSocket is destroyed.
	QList<CWebSocketRequest*> requests = findChildren<CWebSocketRequest*>(QString(), Qt::FindDirectChildrenOnly);
	qDeleteAll(requests);

	m_pendingMessages.clear();

	m_parent->NotifySocketDisconnected();
}


CWebSocketThread::CWebSocketThread(CWebSocketServerComp* parent)
	:QThread(parent),
	m_status(ST_START)
{
	qRegisterMetaType<ConstResponsePtr>("ConstResponsePtr");

	m_server = parent;

	m_enginePtr = m_server->GetProtocolEngine();

	m_httpEnginePtr = m_server->GetHttpProtocolEngine();
	m_requestServerHandlerPtr = m_server->GetRequestServerServlet();
	m_requestClientHandlerPtr = m_server->GetRequestClientServlet();
	m_productId = m_server->GetProductId();
	// Direct connection: the signal is only emitted from the connection thread,
	// where the socket lives; the slot must run there as well.
	connect(this, &CWebSocketThread::SendTextMessage, this, &CWebSocketThread::OnSendTextMessage, Qt::DirectConnection);
}


void CWebSocketThread::SetWebSocket(QWebSocket* webSocketPtr)
{
	{
		QMutexLocker lock(&m_socketMutex);
		m_socket = webSocketPtr;
	}

	// Delete the handler of a previous connection; it was moved back to the main
	// thread in NotifySocketDisconnected, so deleteLater is processed there:
	if (!m_handlerPtr.isNull()){
		m_handlerPtr->deleteLater();
		m_handlerPtr = nullptr;
	}

	if (webSocketPtr != nullptr){
		// The socket and a handler object are moved into this thread, so that the
		// complete message processing (including a possibly blocking authorization
		// validation pumping a nested event loop) runs in the connection thread and
		// cannot stall or re-enter the main event loop.
		// textMessageReceived is connected with Qt::QueuedConnection: the signal is
		// emitted while QWebSocket is still decoding the incoming frame; processing
		// the message directly from that context could destroy objects the frame
		// decoder still uses. Qt copies the QString into the queued event.
		m_handlerPtr = new CWebSocket(this);

		connect(webSocketPtr, &QWebSocket::textMessageReceived, m_handlerPtr.data(), &CWebSocket::OnWebSocketTextMessage, Qt::QueuedConnection);
		connect(webSocketPtr, &QWebSocket::binaryMessageReceived, m_handlerPtr.data(), &CWebSocket::OnBinaryMessage);
		connect(webSocketPtr, &QWebSocket::disconnected, m_handlerPtr.data(), &CWebSocket::OnDisconnected);
#if (QT_VERSION >= 0x060500)
		connect(webSocketPtr, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred), m_handlerPtr.data(), &CWebSocket::OnError);
#endif

		webSocketPtr->setParent(nullptr);
		webSocketPtr->moveToThread(this);
		m_handlerPtr->moveToThread(this);
	}

	start();
}


const QWebSocket* CWebSocketThread::GetWebSocket() const
{
	QMutexLocker lock(&m_socketMutex);

	return m_socket;
}


void CWebSocketThread::SetSocketStatus(Status socketStatus)
{
	QMutexLocker lock(&m_statusMutex);

	m_status = socketStatus;
}


CWebSocketThread::Status CWebSocketThread::GetSocketStatus()
{
	return m_status;
}


QByteArray CWebSocketThread::GetRequestId()
{
	return m_requestId;
}


bool CWebSocketThread::IsSecureConnection() const
{
	return m_isSecureConnection;
}


void CWebSocketThread::EnableSecureConnection(bool isSecureConnection)
{
	m_isSecureConnection = isSecureConnection;
}


void CWebSocketThread::run()
{
	QPointer<QWebSocket> webSocketPtr = GetValidWebSocket();
	if (webSocketPtr.isNull()){
		return;
	}

	// The socket and the handler object were moved into this thread in SetWebSocket;
	// the event loop below delivers their signals in this thread.

	exec();
}


// public slots

void CWebSocketThread::OnWebSocketTextMessage(const QString& textMessage)
{
	if (m_requestServerHandlerPtr == nullptr || m_server == nullptr || textMessage.isEmpty()){
		return;
	}

	QPointer<QWebSocket> webSocketPtr = GetValidWebSocket();
	if (webSocketPtr.isNull()){
		return;
	}

	QString message = QString("Web socket text message received: %1").arg(textMessage);
	m_server->SendVerboseMessage(message, "CWebSocketServerComp");

	imtrest::IRequestUniquePtr newRequestPtr = m_enginePtr->CreateRequest(*m_requestServerHandlerPtr);
	if (newRequestPtr.IsValid()){
		CWebSocketRequest* webSocketRequest = dynamic_cast<CWebSocketRequest*>(newRequestPtr.GetPtr());
		if (webSocketRequest == nullptr){
			return;
		}

		webSocketRequest->SetBody(textMessage.toUtf8());

		imtrest::CWebSocketRequest::MethodType methodType = webSocketRequest->GetMethodType();
		if (methodType == CWebSocketRequest::MT_START || methodType == CWebSocketRequest::MT_SUBSCRIBE){
			newRequestPtr.PopPtr();
			// Parent to the in-thread handler object instead of the QWebSocket to avoid
			// cascade-deletion while the request is still processed. The handler lives in
			// the connection thread (same thread as this code) and deletes the requests
			// explicitly in its disconnect handling, which notifies the publishers.
			webSocketRequest->setParent(m_handlerPtr.data());
			if (m_server != nullptr && !webSocketPtr.isNull()){
				m_server->RegisterSender(webSocketRequest->GetRequestId(), webSocketPtr.data());
			}
		}

		QByteArray clientId = webSocketRequest->GetClientId();
		imtrest::ConstResponsePtr responsePtr;

		if (
			methodType == CWebSocketRequest::MT_CONNECTION_ACK ||
			methodType == CWebSocketRequest::MT_START_ACK ||
			methodType == CWebSocketRequest::MT_DATA ||
			methodType == CWebSocketRequest::MT_ERROR ||
			methodType == CWebSocketRequest::MT_QUERY_DATA){
			if (methodType == CWebSocketRequest::MT_ERROR){
				return;
			}
			if (m_requestClientHandlerPtr != nullptr){
				responsePtr = m_requestClientHandlerPtr->ProcessRequest(*webSocketRequest);
			}
		}
		else if (methodType == CWebSocketRequest::MT_KEEP_ALIVE_ACK){
			// optional ToDo: Map pong to send ping.
			return;
		}
		else {
			if (methodType == CWebSocketRequest::MT_CONNECTION_INIT){
				if (!clientId.isEmpty() && !webSocketPtr.isNull()){
					m_server->RegisterSender(clientId, webSocketPtr.data());
				}
				m_server->SetConnectionStatus(clientId);
			}

			if (methodType == CWebSocketRequest::MT_QUERY){
				if (m_httpEnginePtr != nullptr){
					imtrest::IRequestUniquePtr requestPtr = m_httpEnginePtr->CreateRequest(*m_requestServerHandlerPtr);
					CHttpRequest* newHttpRequestPtr = dynamic_cast<CHttpRequest*>(requestPtr.GetPtr());
					if (newHttpRequestPtr != nullptr){
						if (!clientId.isEmpty() && !webSocketPtr.isNull()){
							m_server->RegisterSender(webSocketRequest->GetRequestId(), webSocketPtr.data());
						}
	
						QJsonDocument document = QJsonDocument::fromJson(textMessage.toUtf8());
						QJsonObject object = document.object();
						QByteArray body = object.value("payload").toObject().value("data").toString().toUtf8();
	
						QJsonObject headers = object.value("headers").toObject();
						for (QString& key: headers.keys()){
							newHttpRequestPtr->SetHeader(key.toUtf8().toLower(), headers.value(key).toString().toUtf8());
						}
						newHttpRequestPtr->SetBody(body);
						newHttpRequestPtr->SetMethodType(CHttpRequest::MT_POST);
						newHttpRequestPtr->SetCommandId("/" + m_productId + "/graphql");
	
						responsePtr = m_requestServerHandlerPtr->ProcessRequest(*requestPtr.PopInterfacePtr());
					}
				}
			}
			else{
				responsePtr = m_requestServerHandlerPtr->ProcessRequest(*webSocketRequest);
			}
		}

		if (responsePtr.IsValid()){
			QByteArray data = responsePtr->GetData();
			emit SendTextMessage(data);
		}
	}
}


// private slots

void CWebSocketThread::NotifySocketDisconnected()
{
	QPointer<QWebSocket> webSocketPtr;

	{
		QMutexLocker lock(&m_socketMutex);
		webSocketPtr = m_socket;
		m_socket = nullptr;
	}

	// Push the socket and the handler back to the main thread, so that pending
	// deleteLater events (posted by CWebSocketServerComp::OnSocketDisconnected)
	// can still be processed after this thread's event loop has finished:
	QThread* mainThreadPtr = QCoreApplication::instance()->thread();
	if (!webSocketPtr.isNull()){
		webSocketPtr->moveToThread(mainThreadPtr);
	}
	if (!m_handlerPtr.isNull()){
		m_handlerPtr->moveToThread(mainThreadPtr);
	}

	exit();
}


void CWebSocketThread::OnWebSocketBinaryMessage(const QByteArray& dataMessage)
{
	if (m_server == nullptr){
		return;
	}

	QString message = QString("Web socket binary message received: '%1'").arg(qPrintable(dataMessage));
	m_server->SendInfoMessage(0, message, "CWebSocketServerComp");
}


void CWebSocketThread::OnError(QAbstractSocket::SocketError error)
{
	if (error == QAbstractSocket::RemoteHostClosedError) {
		// This is a normal disconnect. Let the 'disconnected()' signal handle the cleanup.
		return;
	}

	QPointer<QWebSocket> webSocketPtr = GetValidWebSocket();
	if (!webSocketPtr.isNull() && m_server != nullptr){
		QString errorMessage = QString("Web socket server error: '%1'").arg(webSocketPtr->errorString());

		m_server->SendErrorMessage(0, errorMessage, "CWebSocketServerComp");
	}
}


void CWebSocketThread::OnTimeout()
{
	QPointer<QWebSocket> webSocketPtr = GetValidWebSocket();
	if (webSocketPtr.isNull()){
		return;
	}
	webSocketPtr->sendTextMessage(QString(R"({"type": "ka"})"));
}


QPointer<QWebSocket> CWebSocketThread::GetValidWebSocket() const
{
	QPointer<QWebSocket> webSocketPtr;

	{
		QMutexLocker lock(&m_socketMutex);
		webSocketPtr = m_socket;
	}

	if (webSocketPtr.isNull() || !webSocketPtr->isValid()){
		return {};
	}

	return webSocketPtr;
}


void CWebSocketThread::OnAcceptError(QAbstractSocket::SocketError /*socketError*/)
{
	QWebSocketServer* webSocketServerPtr = qobject_cast<QWebSocketServer*>(sender());
	Q_ASSERT(webSocketServerPtr != nullptr);

	webSocketServerPtr->resumeAccepting();
}


void CWebSocketThread::OnSslErrors(const QList<QSslError> &errors)
{
	if (m_server == nullptr){
		return;
	}

	QString errorMessage;

	for (const QSslError& error: errors){
		if (!errorMessage.isEmpty()){
			errorMessage += " ";
		}
		errorMessage += error.errorString();
	}
	errorMessage.prepend(QStringLiteral("Web socket server SSL-errors: "));

	m_server->SendErrorMessage(0, errorMessage, "CWebSocketServerComp");
	qDebug() << errorMessage << __func__;
}


void CWebSocketThread::OnSendTextMessage(const QByteArray& data) const
{
	QPointer<QWebSocket> webSocketPtr = GetValidWebSocket();
	if (!webSocketPtr.isNull()){
		webSocketPtr->sendTextMessage(data);
	}
}


} // namespace imtrest
