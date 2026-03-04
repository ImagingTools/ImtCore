// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


QByteArray CWebSocketServerComp::GetProductId()
{
	if (m_productId.IsValid()){
		return *m_productId;
	}

	return QByteArray();
}


void CWebSocketServerComp::RegisterSender(const QByteArray& clientId, QWebSocket* webSocketPtr)
{
	QWriteLocker locker(&m_sendersLock);

	QSharedPointer<CWebSocketSender> socketSender(new CWebSocketSender(webSocketPtr));
	QThread* mainThread = QCoreApplication::instance()->thread();
	if (mainThread == nullptr){
		Q_ASSERT(false);
	}
	socketSender->moveToThread(mainThread);

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
	QReadLocker locker(&m_sendersLock);

	CWebSocketSender* sender = m_senders.value(requestId).data();
	if (sender != nullptr){
		return sender;
	}

	return nullptr;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CWebSocketServerComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	Q_ASSERT_X(m_sslConfigurationCompPtr.IsValid() && m_sslConfigurationManagerCompPtr.IsValid(), "Update server's SSL configuration", "SSL configuration or manager is not set!");

	if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr && m_isInitialized){
		while (m_webSocketThreadList.count() > 0){
			imtrest::CWebSocketThread* webSocketThread = m_webSocketThreadList.back();
			webSocketThread->disconnect();
			m_webSocketThreadList.pop_back();
		}
		StopServer();
		EnsureServerStarted();
	}
}


// reimplemented (ibase::TRuntimeStatusHanderCompWrap)

void CWebSocketServerComp::OnSystemShutdown()
{
	BaseClass2::UnregisterAllModels();
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CWebSocketServerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_sslConfigurationModelCompPtr.IsValid() && m_sslConfigurationManagerCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_sslConfigurationModelCompPtr.GetPtr());
	}

	if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr){
		EnsureServerStarted();
	}

	connect(&m_timer, &QTimer::timeout, this, &CWebSocketServerComp::OnTimeout);

	m_timer.start(5000);

	m_isInitialized = true;
}


void CWebSocketServerComp::OnComponentDestroyed()
{
	StopServer();

	m_timer.stop();

	m_webSocketServerPtr.Reset();

	BaseClass::OnComponentDestroyed();
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
		if (m_webServerInterfaceCompPtr.IsValid()){
			int port = m_webServerInterfaceCompPtr->GetPort(imtcom::IServerConnectionInterface::PT_WEBSOCKET);

			retVal = StartListening(QHostAddress::Any, port);
		}
		else{
			retVal = StartListening();
		}
	}

	return retVal;
}


bool CWebSocketServerComp::StartListening(const QHostAddress& address, quint16 port)
{
	if (!m_protocolEngineCompPtr.IsValid()){
		return false;
	}

	bool isSecureConnection = false;
	if (m_sslConfigurationCompPtr.IsValid() && m_sslConfigurationManagerCompPtr.IsValid()){
		QSslConfiguration sslConfiguration;
		iprm::TParamsPtr<iprm::IEnableableParam> sslEnableParamPtr(
					m_sslConfigurationCompPtr.GetPtr(),
					imtcom::ISslConfigurationManager::ParamKeys::s_enableSslModeParamKey);
		if (sslEnableParamPtr.IsValid() && sslEnableParamPtr->IsEnabled()){
			if (m_sslConfigurationManagerCompPtr->CreateSslConfiguration(*m_sslConfigurationCompPtr, sslConfiguration)){
				m_webSocketServerPtr.SetPtr(new QWebSocketServer("", QWebSocketServer::SecureMode, this));
				m_webSocketServerPtr->setSslConfiguration(sslConfiguration);

				SendInfoMessage(0, QString("Secure connection (SSL) enabled on web socket server"));

				isSecureConnection = true;
			}
			else{
				QString message = QString("Could not enable secure connection (SSL) on web socket server");
				SendErrorMessage(0, message);
			}
		}
	}

	if (!isSecureConnection){
		m_webSocketServerPtr.SetPtr(new QWebSocketServer("", QWebSocketServer::NonSecureMode, this));
	}

#if QT_VERSION >= QT_VERSION_CHECK(6,4,0)
	if (m_subprotocolListCompPtr.IsValid()){
		QStringList supportedSubprotocolList;

		int count = m_subprotocolListCompPtr->GetOptionsCount();
		for (int i = 0; i < count; i++){
			supportedSubprotocolList << m_subprotocolListCompPtr->GetOptionId(i);
		}

		m_webSocketServerPtr->setSupportedSubprotocols(supportedSubprotocolList);
	}
#endif

	if (m_webSocketServerPtr->listen(address, port)){
		SendInfoMessage(0, QString("Web socket server successfully started on port %1").arg(port));

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

#if QT_VERSION >= QT_VERSION_CHECK(6,4,0)
	QStringList supportedSubprotocols = webSocketServerPtr->supportedSubprotocols();
#endif

	while (QWebSocket* webSocketPtr = webSocketServerPtr->nextPendingConnection()){
#if QT_VERSION >= QT_VERSION_CHECK(6,4,0)
		QString subprotocol = webSocketPtr->subprotocol();
		QString message = QString("Handle new web socket connection, (Subprotocol: '%1', Threads: %2)").arg(subprotocol, m_webSocketThreadList.count()) ;
		SendVerboseMessage(message, "CWebSocketServerComp");

		if (!subprotocol.isEmpty() && !supportedSubprotocols.contains(subprotocol)) {
			SendVerboseMessage("Unsupported subprotocol: " + subprotocol, "CWebSocketServerComp");
		}
#endif
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


void CWebSocketServerComp::OnTimeout()
{
	QList<QWebSocket*> sendedSockets;
	for (const QByteArray& key: m_senders.keys()){
		if (!m_senders[key].isNull()){
			QWebSocket* webSocketPtr = const_cast<QWebSocket*>(m_senders[key]->GetSocket());
			if (webSocketPtr != nullptr && !sendedSockets.contains(webSocketPtr)){
				QString subProtocolId;

			#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
				subProtocolId = webSocketPtr->subprotocol();
			#endif

				if (subProtocolId == "graphql-transport-ws"){
					//optional ToDo: Remember send ping and disconnect websocket if no pong is received
					webSocketPtr->sendTextMessage(QString(R"({"type": "ping"})"));
				}
				else{
					webSocketPtr->sendTextMessage(QString(R"({"type": "ka"})"));
				}

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


void CWebSocketServerComp::OnSslErrors(const QList<QSslError>& errors)
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


