#include <imtrest/CWebSocketThread.h>

// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QMutableListIterator>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/ISender.h>
#include <imtrest/CWebSocketServerComp.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtrest/CHttpRequest.h>


namespace imtrest
{

CWebSocket::CWebSocket(imtrest::CWebSocketServerComp *parent)
{
	m_server = parent;

	m_enginePtr = m_server->GetProtocolEngine();
	m_httpEnginePtr = m_server->GetHttpProtocolEngine();
	m_requestServerHandlerPtr = m_server->GetRequestServerServlet();
	m_requestClientHandlerPtr = m_server->GetRequestClientServlet();
}


void CWebSocket::OnWebSocketTextMessage(const QString& textMessage)
{
	if (m_requestServerHandlerPtr == nullptr || m_server == nullptr){
		return;
	}

	QWebSocket* webSocketPtr = dynamic_cast<QWebSocket*>(sender());

	if (webSocketPtr == nullptr){
		return;
	}

	QString message = QString("Web socket text message received: %1").arg(textMessage);
	qDebug() << "OnWebSocketTextMessage" << message;
	m_server->SendVerboseMessage(message, "CWebSocketServerComp");

	istd::TDelPtr<IRequest> newRequestPtr = m_enginePtr->CreateRequest(*m_requestServerHandlerPtr);
	if (newRequestPtr.IsValid()){
		CWebSocketRequest* webSocketRequest = dynamic_cast<CWebSocketRequest*>(newRequestPtr.GetPtr());
		if (webSocketRequest == nullptr){
			return;
		}
		webSocketRequest->SetBody(textMessage.toUtf8());

		imtrest::CWebSocketRequest::MethodType methodType = webSocketRequest->GetMethodType();
		if (methodType == CWebSocketRequest::MT_START){
			newRequestPtr.PopPtr();
			this->RegisterSender(webSocketRequest->GetRequestId(), webSocketPtr);
		}

		QByteArray clientId = webSocketRequest->GetClientId();
		imtrest::ConstResponsePtr responsePtr;

		if (
			methodType == CWebSocketRequest::MT_CONNECTION_ASK ||
			methodType == CWebSocketRequest::MT_START_ASK ||
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
		else {
			if (methodType == CWebSocketRequest::MT_CONNECTION_INIT){
				m_server->SetConnectionStatus(clientId);
			}

			if (methodType == CWebSocketRequest::MT_QUERY){
				IRequest* requestPtr = m_httpEnginePtr->CreateRequest(*m_requestServerHandlerPtr);
				CHttpRequest* newHttpRequestPtr = dynamic_cast<CHttpRequest*>(requestPtr);
				if (newHttpRequestPtr != nullptr){
					if (!clientId.isEmpty()){
						emit this->RegisterSender(clientId, webSocketPtr);
					}

					QJsonDocument document = QJsonDocument::fromJson(textMessage.toUtf8());
					QJsonObject object = document.object();

					QJsonObject headers = object.value("headers").toObject();
					for (QString& key: headers.keys()){
						newHttpRequestPtr->SetHeader(key.toUtf8(), headers.value(key).toString().toUtf8());
					}
					newHttpRequestPtr->SetBody(textMessage.toUtf8());
					newHttpRequestPtr->SetMethodType(CHttpRequest::MT_POST);

					responsePtr = m_requestServerHandlerPtr->ProcessRequest(*newHttpRequestPtr);
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


CWebSocketThread::CWebSocketThread(CWebSocketServerComp* parent)
	:QThread(parent),
	m_status(ST_START)
{
	qRegisterMetaType<ConstResponsePtr>("ConstResponsePtr");

	m_server = parent;

	m_enginePtr = m_server->GetProtocolEngine();
}

void CWebSocketThread::SetWebSocket(QWebSocket* webSocketPtr)
{
	m_socket = webSocketPtr;
	start();
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
	if (m_server == nullptr || !m_socket->isValid()){
		return;
	}

	QWebSocket* webSocketPtr = m_socket.get();
	if (!m_webSocket.IsValid()){
		m_webSocket.SetPtr(new CWebSocket(m_server));
	}

	connect(webSocketPtr, &QWebSocket::textMessageReceived, m_webSocket.GetPtr(), &CWebSocket::OnWebSocketTextMessage);
	connect(m_webSocket.GetPtr(), &CWebSocket::SendTextMessage, this, &CWebSocketThread::OnSendTextMessage);
	connect(m_webSocket.GetPtr(), &CWebSocket::RegisterSender, m_server, &CWebSocketServerComp::RegisterSender);

	connect(webSocketPtr, &QWebSocket::binaryMessageReceived, this, &CWebSocketThread::OnWebSocketBinaryMessage);
	connect(webSocketPtr, &QWebSocket::disconnected, this, &CWebSocketThread::OnSocketDisconnected);
#if (QT_VERSION >= 0x060500)
	connect(webSocketPtr, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred), this, &CWebSocketThread::OnError);
#else
	connect(webSocketPtr, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &CWebSocketServerComp::OnError);
#endif

	exec();
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


void CWebSocketThread::OnError(QAbstractSocket::SocketError /*error*/)
{
	QWebSocket* webSocketPtr = dynamic_cast<QWebSocket*>(sender());
	if (webSocketPtr != nullptr && m_server != nullptr){
		QString errorMessage = QString("Web socket server error: '%1'").arg(webSocketPtr->errorString());

		m_server->SendErrorMessage(0, errorMessage, "CWebSocketServerComp");
	}
}


void CWebSocketThread::OnTimeout()
{
	if (m_socket->isValid()){
		m_socket->sendTextMessage(QString(R"({"type": "keep_alive"})"));
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
	if (m_socket->isValid()){
		m_socket->sendTextMessage(data);
	}
}


} // namespace imtrest


