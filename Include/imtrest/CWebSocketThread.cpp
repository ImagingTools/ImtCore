// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CWebSocketThread.h>


// Qt includes
#include <QtCore/QMutableListIterator>

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
}


void CWebSocket::OnWebSocketTextMessage(const QString& textMessage)
{
	m_parent->OnWebSocketTextMessage(textMessage);
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

	// Always send through the event loop so pending disconnect events can invalidate m_socket first.
	connect(this, &CWebSocketThread::SendTextMessage, this, &CWebSocketThread::OnSendTextMessage, Qt::QueuedConnection);
}


void CWebSocketThread::SetWebSocket(QWebSocket* webSocketPtr)
{
	m_socket = webSocketPtr;

	if (webSocketPtr != nullptr){
		// Queue socket signals and avoid duplicate delivery if the same socket is wired twice.
		const Qt::ConnectionType connectionType = static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection);
		connect(webSocketPtr, &QWebSocket::textMessageReceived, this, &CWebSocketThread::OnWebSocketTextMessage, connectionType);
		connect(webSocketPtr, &QWebSocket::binaryMessageReceived, this, &CWebSocketThread::OnWebSocketBinaryMessage, connectionType);
		connect(webSocketPtr, &QWebSocket::disconnected, this, &CWebSocketThread::OnSocketDisconnected, connectionType);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
		connect(webSocketPtr, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred), this, &CWebSocketThread::OnError, connectionType);
#else
		// Older Qt builds still use disconnected() for cleanup; error logging is optional here.
#endif
	}

	start();
}


const QWebSocket* CWebSocketThread::GetWebSocket() const
{
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
	exec();
}


// public slots

void CWebSocketThread::OnWebSocketTextMessage(const QString& textMessage)
{
	if (m_requestServerHandlerPtr == nullptr || m_server == nullptr || textMessage.isEmpty()){
		return;
	}

	QWebSocket* webSocketPtr = m_socket.data();
	if (webSocketPtr == nullptr || !webSocketPtr->isValid()){
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
			webSocketRequest->setParent(this);
			if (m_server != nullptr){
				m_server->RegisterSender(webSocketRequest->GetRequestId(), webSocketPtr);
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
				if (!clientId.isEmpty()){
					m_server->RegisterSender(clientId, webSocketPtr);
				}
				m_server->SetConnectionStatus(clientId);
			}

			if (methodType == CWebSocketRequest::MT_QUERY){
				if (m_httpEnginePtr != nullptr){
					imtrest::IRequestUniquePtr requestPtr = m_httpEnginePtr->CreateRequest(*m_requestServerHandlerPtr);
					CHttpRequest* newHttpRequestPtr = dynamic_cast<CHttpRequest*>(requestPtr.GetPtr());
					if (newHttpRequestPtr != nullptr){
						if (!clientId.isEmpty()){
							m_server->RegisterSender(webSocketRequest->GetRequestId(), webSocketPtr);
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

void CWebSocketThread::OnSocketDisconnected()
{
	m_socket = nullptr;
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

	QWebSocket* webSocketPtr = dynamic_cast<QWebSocket*>(sender());
	if (webSocketPtr != nullptr && m_server != nullptr){
		QString errorMessage = QString("Web socket server error: '%1'").arg(webSocketPtr->errorString());

		m_server->SendErrorMessage(0, errorMessage, "CWebSocketServerComp");
	}
}


void CWebSocketThread::OnTimeout()
{
	if (!m_socket.isNull() && m_socket->isValid()){
		m_socket->sendTextMessage(QString(R"({"type": "ka"})"));
	}
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
	if (!m_socket.isNull() && m_socket->isValid()){
		m_socket->sendTextMessage(data);
	}
}


} // namespace imtrest
