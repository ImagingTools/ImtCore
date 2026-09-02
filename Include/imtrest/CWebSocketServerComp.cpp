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

	istd::IChangeable::ChangeSet loginChangeSet(loginStatus, QStringLiteral("Login"));
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


// reimplemented (imtrest::IResponseDispatcher)

bool CWebSocketServerComp::SendResponse(const QByteArray& requestId, ConstResponsePtr& response) const
{
	QReadLocker locker(&m_sendersLock);

	CWebSocketSender* sender = m_senders.value(requestId).data();
	if (sender != nullptr){
		return sender->SendResponse(response);
	}

	return false;
}


bool CWebSocketServerComp::SendRequest(const QByteArray& requestId, ConstRequestPtr& request) const
{
	QReadLocker locker(&m_sendersLock);

	CWebSocketSender* sender = m_senders.value(requestId).data();
	if (sender != nullptr){
		return sender->SendRequest(request);
	}

	return false;
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

				SendInfoMessage(0, QStringLiteral("Secure connection (SSL) enabled on web socket server"));

				isSecureConnection = true;
			}
			else{
				QString message = QStringLiteral("Could not enable secure connection (SSL) on web socket server");
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
		SendInfoMessage(0, QStringLiteral("Web socket server successfully started on port %1").arg(port));

		connect(m_webSocketServerPtr.GetPtr(), &QWebSocketServer::newConnection, this, &CWebSocketServerComp::HandleNewConnections);
		connect(m_webSocketServerPtr.GetPtr(), &QWebSocketServer::acceptError, this, &CWebSocketServerComp::OnAcceptError);
		connect(m_webSocketServerPtr.GetPtr(), &QWebSocketServer::sslErrors, this, &CWebSocketServerComp::OnSslErrors);

		return true;
	}
	else{
		SendErrorMessage(0, QStringLiteral("Web socket server could not be started on port %1").arg(port));
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
		QString message = QStringLiteral("Handle new web socket connection, (Subprotocol: '%1', Threads: %2)").arg(subprotocol, m_webSocketThreadList.count()) ;
		SendVerboseMessage(message, "CWebSocketServerComp");

		if (!subprotocol.isEmpty() && !supportedSubprotocols.contains(subprotocol)) {
			SendVerboseMessage("Unsupported subprotocol: " + subprotocol, "CWebSocketServerComp");
		}
#endif
		connect(webSocketPtr, &QWebSocket::disconnected, this, &CWebSocketServerComp::OnSocketDisconnected);

		// bool find = false;
		CWebSocketThread* webSocketThreadPtr = nullptr;
		for (CWebSocketThread* webSocketThreadItemPtr: m_webSocketThreadList){
			if (!webSocketThreadItemPtr->isRunning()){
				webSocketThreadItemPtr->SetWebSocket(webSocketPtr);
				webSocketThreadPtr = webSocketThreadItemPtr;

				break;
			}
		}

		if (webSocketThreadPtr == nullptr){
			webSocketThreadPtr = new CWebSocketThread(this);
			m_webSocketThreadList.append(webSocketThreadPtr);
			webSocketThreadPtr->SetWebSocket(webSocketPtr);
		}

		connect(webSocketPtr, &QWebSocket::textMessageReceived, webSocketThreadPtr, &CWebSocketThread::TextMessageReceived);
		connect(webSocketPtr, &QWebSocket::disconnected, webSocketThreadPtr, &CWebSocketThread::SocketDisconnected);
		connect(webSocketPtr, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred), webSocketThreadPtr, &CWebSocketThread::SocketError);
	}
}


void CWebSocketServerComp::OnSocketDisconnected()
{
	QObject* socketObjectPtr = sender();
	Q_ASSERT(socketObjectPtr != nullptr);

	if (m_subscriberEngineCompPtr.IsValid()){
		m_subscriberEngineCompPtr->UnRegisterSubscriber(socketObjectPtr);
	}

	// m_senders is read concurrently by worker threads in SendResponse()/SendRequest() (QReadLocker)
	// and written by RegisterSender() (QWriteLocker). This handler runs on the main thread when a
	// socket disconnects and previously mutated m_senders / m_senderLoginStatusMap with NO lock, so a
	// disconnect racing a worker's publish did a concurrent QMap read+write on the same shared map:
	// that corrupts the map's shared nodes / QByteArray keys / the QSharedPointer<CWebSocketSender>
	// control block, giving a use-after-free refcount fault (crash reproduced live under workers:10 -
	// faulting instruction `lock xadd [rax]` with rax = 0xdddddddd..., the MSVC freed-heap fill).
	// Take the write lock (exclusive against both readers and RegisterSender) while removing the
	// disconnecting socket's entries, matching how every other m_senders accessor is guarded.
	QList<QByteArray> removedKeys;
	{
		QWriteLocker locker(&m_sendersLock);
		for (const QByteArray& key: m_senders.keys()){
			if (socketObjectPtr == m_senders[key]->GetSocket().data()){
				m_senders.remove(key);
				m_senderLoginStatusMap.remove(key);
				removedKeys.append(key);
			}
		}
	}

	// Announce the logout status change AFTER releasing the lock: CChangeNotifier fires observer
	// callbacks synchronously, and an observer that calls back into SendResponse()/RegisterSender()
	// would re-enter the (non-recursive) m_sendersLock and deadlock.
	for (const QByteArray& key: removedKeys){
		istd::IChangeable::ChangeSet loginChangeSet(imtcom::IConnectionStatusProvider::CS_UNKNOWN, QStringLiteral("Logout"));
		loginChangeSet.SetChangeInfo("ClientId", key);
		istd::CChangeNotifier notifier(this, &loginChangeSet);
	}

	socketObjectPtr->deleteLater();
}


void CWebSocketServerComp::OnTimeout()
{
	QList<QWebSocket*> sendedSockets;
	for (CWebSocketThread* webSocketThread: m_webSocketThreadList) {
		QWebSocket* webSocketPtr = const_cast<QWebSocket*>(webSocketThread->GetWebSocket());
		if (webSocketPtr != nullptr && !sendedSockets.contains(webSocketPtr)){
			QString subProtocolId;

		#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
			subProtocolId = webSocketPtr->subprotocol();
		#endif

			if (subProtocolId == "graphql-transport-ws"){
				//optional ToDo: Remember send ping and disconnect websocket if no pong is received
				webSocketPtr->sendTextMessage(QStringLiteral(R"({"type": "ping"})"));
			}
			else{
				webSocketPtr->sendTextMessage(QStringLiteral(R"({"type": "ka"})"));
			}

			sendedSockets.append(webSocketPtr);
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

