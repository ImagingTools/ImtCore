#include <imtrest/CWebSocketServerComp.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/IEnableableParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtrest/CWebSocketRequest.h>
#include <imtrest/CWebSocketThread.h>


namespace imtrest
{


// public methods

IProtocolEngine* CWebSocketServerComp::GetProtocolEngine()
{
	return m_protocolEngineCompPtr.GetPtr();
}


IProtocolEngine* CWebSocketServerComp::GetHttpProtocolEngine()
{
	return m_httpProtocolEngineCompPtr.GetPtr();
}


imtrest::IRequestServlet* CWebSocketServerComp::GetRequestServerServlet()
{
	return m_requestServerHandlerCompPtr.GetPtr();
}


imtrest::IRequestServlet* CWebSocketServerComp::GetRequestClientServlet()
{
	return m_requestClientHandlerCompPtr.GetPtr();
}


void CWebSocketServerComp::RegisterSender(const QByteArray& clientId, QWebSocket* webSocketPtr)
{
	QSharedPointer<CWebSocketSender> socketSender(new CWebSocketSender(webSocketPtr));
	m_senders.insert(clientId, socketSender);
}


void CWebSocketServerComp::SetConnectionStatus(const QByteArray& clientId)
{
	imtcom::IConnectionStatusProvider::ConnectionStatus loginStatus = imtcom::IConnectionStatusProvider::CS_CONNECTED;
	istd::IChangeable::ChangeSet loginChangeSet(loginStatus, QString("Login"));
	loginChangeSet.SetChangeInfo("ClientId", clientId);
	istd::CChangeNotifier notifier(this, &loginChangeSet);

	m_senderLoginStatusMap.insert(clientId, loginStatus);
}


bool CWebSocketServerComp::SendInfoMessage(
			int id,
			const QString& message,
			const QString& messageSource,
			int flags) const
{
	return BaseClass::SendInfoMessage(id, message, messageSource, flags);
}


bool CWebSocketServerComp::SendErrorMessage(
			int id,
			const QString& message,
			const QString& messageSource,
			int flags) const
{
	return BaseClass::SendErrorMessage(id, message, messageSource, flags);
}

void CWebSocketServerComp::SendVerboseMessage(const QString& message, const QString& messageSource) const
{
	BaseClass::SendVerboseMessage(message, messageSource);
}


// reimplemented (icomp::IRequestManager)

const ISender* CWebSocketServerComp::GetSender(const QByteArray& requestId) const
{
	CWebSocketSender* sender = m_senders.value(requestId).data();
	if (sender != nullptr){
		return sender;
	}

	return nullptr;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CWebSocketServerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
	if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr){
		EnsureServerStarted();
	}

	connect(&m_timer, &QTimer::timeout, this, &CWebSocketServerComp::OnTimeout);

	m_timer.start(5000);
}


// reimplemented (imtcom::IConnectionStatusProvider)

imtcom::IConnectionStatusProvider::ConnectionStatus CWebSocketServerComp::GetConnectionStatus() const
{
	return imtcom::IConnectionStatusProvider::CS_UNKNOWN;
}


// reimplemented (imtrest::IServer)

bool CWebSocketServerComp::StartServer()
{
	return EnsureServerStarted();
}

bool CWebSocketServerComp::StopServer()
{
	m_webSocketServerPtr->close();

	return true;
}

IServer::ServerStatus CWebSocketServerComp::GetServerStatus() const
{
	if (m_webSocketServerPtr->isListening()){
		return SS_LISTENING;
	}

	return SS_NOT_STARTED;
}


// private methods

bool CWebSocketServerComp::EnsureServerStarted()
{
	bool retVal = false;

	if (m_protocolEngineCompPtr.IsValid()){
		if (m_webSocketServerPortCompPtr.IsValid()){
			int port = m_webSocketServerPortCompPtr->GetUrl().port();

			retVal = StartListening(QHostAddress::Any, port);
		}
		else{
			retVal = StartListening();
		}
	}

	return retVal;
}


bool CWebSocketServerComp::StartListening(const QHostAddress &address, quint16 port)
{
	if (!m_protocolEngineCompPtr.IsValid()){
		return false;
	}

	if (m_sslConfigurationCompPtr.IsValid() && m_sslConfigurationManagerCompPtr.IsValid()){
		QSslConfiguration sslConfiguration;
		iprm::TParamsPtr<iprm::IEnableableParam> sslEnableParamPtr(
					m_sslConfigurationCompPtr.GetPtr(),
					imtcom::ISslConfigurationManager::ParamKeys::s_enableSslModeParamKey);
		if (sslEnableParamPtr.IsValid() && sslEnableParamPtr->IsEnabled()){
			if (m_sslConfigurationManagerCompPtr->CreateSslConfiguration(*m_sslConfigurationCompPtr, sslConfiguration)){
				m_webSocketServerPtr.SetPtr(new QWebSocketServer("",QWebSocketServer::SecureMode,this));
				m_webSocketServerPtr->setSslConfiguration(sslConfiguration);
			}
			else{
				m_webSocketServerPtr.SetPtr(new QWebSocketServer("",QWebSocketServer::NonSecureMode,this));
			}
		}
		else{
			m_webSocketServerPtr.SetPtr(new QWebSocketServer("",QWebSocketServer::NonSecureMode,this));
		}
	}
	else{
		m_webSocketServerPtr.SetPtr(new QWebSocketServer("",QWebSocketServer::NonSecureMode,this));
	}
	if (m_webSocketServerPtr->listen(address, port)){
		QString message = QString("Web socket server successfully started on port %1").arg(port);
		SendInfoMessage(0, message);
		qDebug() << message;

		connect(m_webSocketServerPtr.GetPtr(), &QWebSocketServer::newConnection, this, &CWebSocketServerComp::HandleNewConnections);
		connect(m_webSocketServerPtr.GetPtr(), &QWebSocketServer::acceptError, this, &CWebSocketServerComp::OnAcceptError);
		connect(m_webSocketServerPtr.GetPtr(), &QWebSocketServer::sslErrors, this, &CWebSocketServerComp::OnSslErrors);

		return true;
	}
	else{
		SendErrorMessage(0, QString("Web socket server could not be started on port %1").arg(port));
	}

	return false;
}


// private slots

void CWebSocketServerComp::HandleNewConnections()
{
	QWebSocketServer* webSocketServerPtr = qobject_cast<QWebSocketServer*>(sender());
	Q_ASSERT(webSocketServerPtr != nullptr);

	while (QWebSocket* webSocketPtr = webSocketServerPtr->nextPendingConnection()){
		QString message = QString("Handle new web socket connection, count threads %1").arg(m_webSocketThreadList.count()) ;
		SendVerboseMessage(message, "CWebSocketServerComp");
		bool find = false;
		for (CWebSocketThread* webSocketThreadPtr: m_webSocketThreadList){
			if (!webSocketThreadPtr->isRunning()){
				webSocketThreadPtr->SetWebSocket(webSocketPtr);
				find = true;

				break;
			}
		}
		if (!find){
			CWebSocketThread* webSocketThreadPtr = new CWebSocketThread(this);
			m_webSocketThreadList.append(webSocketThreadPtr);
			webSocketThreadPtr->SetWebSocket(webSocketPtr);
		}
		connect(webSocketPtr, &QWebSocket::disconnected, this, &CWebSocketServerComp::OnSocketDisconnected);
	}
}


void CWebSocketServerComp::OnSocketDisconnected()
{
	QObject* socketObjectPtr = sender();
	Q_ASSERT(socketObjectPtr != nullptr);

	if (m_subscriberEngineCompPtr.IsValid()){
		m_subscriberEngineCompPtr->UnRegisterSubscriber(socketObjectPtr);
	}

	for (const QByteArray& key: m_senders.keys()){
		if (socketObjectPtr == m_senders[key]->GetSocket()){
			m_senders.remove(key);

			istd::IChangeable::ChangeSet loginChangeSet(imtcom::IConnectionStatusProvider::CS_UNKNOWN, QString("Logout"));
			loginChangeSet.SetChangeInfo("ClientId", key);
			istd::CChangeNotifier notifier(this, &loginChangeSet);
			m_senderLoginStatusMap.remove(key);
		}
	}

	socketObjectPtr->deleteLater();
}


// void CWebSocketServerComp::OnWebSocketTextMessage(const QString& textMessage)
// {
// 	if (!m_requestServerHandlerCompPtr.IsValid()){
// 		return;
// 	}

// 	QWebSocket* webSocketPtr = dynamic_cast<QWebSocket*>(sender());

// 	if (webSocketPtr == nullptr){
// 		return;
// 	}

// 	QString message = QString("Web socket text message received: %1").arg(textMessage);
// 	qDebug() << "OnWebSocketTextMessage" << message;
// 	SendVerboseMessage(message, "CWebSocketServerComp");

// 	istd::TDelPtr<IRequest> newRequestPtr = m_protocolEngineCompPtr->CreateRequest(*m_requestServerHandlerCompPtr.GetPtr());
// 	if (newRequestPtr.IsValid()){
// 		CWebSocketRequest* webSocketRequest = dynamic_cast<CWebSocketRequest*>(newRequestPtr.GetPtr());
// 		if (webSocketRequest == nullptr){
// 			return;
// 		}
// 		webSocketRequest->SetBody(textMessage.toUtf8());

// 		imtrest::CWebSocketRequest::MethodType methodType = webSocketRequest->GetMethodType();
// 		if (methodType == CWebSocketRequest::MT_START){
// 			newRequestPtr.PopPtr();
// 			QSharedPointer<CWebSocketSender> socketSender(new CWebSocketSender(webSocketPtr));
// 			m_senders.insert(webSocketRequest->GetRequestId(), socketSender);
// 			QObject::connect(webSocketPtr, &QWebSocket::disconnected, this, &CWebSocketServerComp::OnSocketDisconnected);
// 		}

// 		imtrest::ConstResponsePtr responsePtr;

// 		if (
// 					methodType == CWebSocketRequest::MT_CONNECTION_ASK ||
// 					methodType == CWebSocketRequest::MT_START_ASK ||
// 					methodType == CWebSocketRequest::MT_DATA ||
// 					methodType == CWebSocketRequest::MT_ERROR ||
// 					methodType == CWebSocketRequest::MT_QUERY_DATA	){
// 			responsePtr = m_requestClientHandlerCompPtr->ProcessRequest(*webSocketRequest);
// 		}
// 		else{
// 			QByteArray clientId = webSocketRequest->GetClientId();
// 			if (methodType == CWebSocketRequest::MT_CONNECTION_INIT){
// 				imtcom::IConnectionStatusProvider::ConnectionStatus loginStatus = imtcom::IConnectionStatusProvider::CS_CONNECTED;
// 				istd::IChangeable::ChangeSet loginChangeSet(loginStatus, QString("Login"));
// 				loginChangeSet.SetChangeInfo("ClientId", clientId);
// 				istd::CChangeNotifier notifier(this, &loginChangeSet);

// 				m_senderLoginStatusMap.insert(clientId, loginStatus);
// 			}

// 			if (!clientId.isEmpty()){
// 				newRequestPtr.PopPtr();
// 				if (!m_senders.contains(clientId)){
// 					QSharedPointer<CWebSocketSender> socketSender(new CWebSocketSender(webSocketPtr));
// 					m_senders.insert(clientId, socketSender);
// 					QObject::connect(webSocketPtr, &QWebSocket::disconnected, this, &CWebSocketServerComp::OnSocketDisconnected);
// 				}
// 			}

// 			responsePtr = m_requestServerHandlerCompPtr->ProcessRequest(*webSocketRequest);
// 		}

// 		if (responsePtr.IsValid()){
// 			QByteArray data = responsePtr->GetData();
// 			webSocketPtr->sendTextMessage(data);
// 		}
// 	}
// }


// void CWebSocketServerComp::OnWebSocketBinaryMessage(const QByteArray& dataMessage)
// {
// 	QString message = QString("Web socket binary message received: '%1'").arg(qPrintable(dataMessage));
// 	SendInfoMessage(0, message, "CWebSocketServerComp");
// }


// void CWebSocketServerComp::OnError(QAbstractSocket::SocketError /*error*/)
// {
// 	QWebSocket* webSocketPtr = dynamic_cast<QWebSocket*>(sender());
// 	if (webSocketPtr != nullptr){
// 		QString errorMessage = QString("Web socket server error: '%1'").arg(webSocketPtr->errorString());

// 		SendErrorMessage(0, errorMessage, "CWebSocketServerComp");
// 	}
// }


void CWebSocketServerComp::OnTimeout()
{
	QList<QWebSocket*> sendedSockets;
	for (const QByteArray& key: m_senders.keys()){
		if (!m_senders[key].isNull()){
			QWebSocket* webSocketPtr = const_cast<QWebSocket*>(m_senders[key]->GetSocket());
			if (webSocketPtr != nullptr && !sendedSockets.contains(webSocketPtr)){
				webSocketPtr->sendTextMessage(QString(R"({"type": "keep_alive"})"));
				sendedSockets.append(webSocketPtr);
			}
		}
	}
}


void CWebSocketServerComp::OnAcceptError(QAbstractSocket::SocketError /*socketError*/)
{
	QWebSocketServer* webSocketServerPtr = qobject_cast<QWebSocketServer*>(sender());
	Q_ASSERT(webSocketServerPtr != nullptr);

	webSocketServerPtr->resumeAccepting();
}


void CWebSocketServerComp::OnSslErrors(const QList<QSslError> &errors)
{
	QString errorMessage;

	for (const QSslError& error: errors){
		if (!errorMessage.isEmpty()){
			errorMessage += " ";
		}
		errorMessage += error.errorString();
	}
	errorMessage.prepend(QStringLiteral("Web socket server SSL-errors: "));

	SendErrorMessage(0, errorMessage, "CWebSocketServerComp");
	qDebug() << errorMessage << __func__;
}


} // namespace imtrest


