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

	{
		// Socket threads insert here while the main thread removes entries in OnSocketDisconnected.
		// Scoped tighter than the notifier: its observers must not run while this lock is held.
		QWriteLocker locker(&m_sendersLock);

		m_senderLoginStatusMap.insert(clientId, loginStatus);
	}
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
		QByteArray data = response->GetData();

		return sender->SendData(data);
	}

	return false;
}


bool CWebSocketServerComp::SendRequest(const QByteArray& requestId, ConstRequestPtr& request) const
{
	QReadLocker locker(&m_sendersLock);

	CWebSocketSender* sender = m_senders.value(requestId).data();
	if (sender != nullptr){
		QByteArray data = request->GetBody();

		return sender->SendData(data);
	}

	return false;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CWebSocketServerComp::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (!m_isInitialized){
		return;
	}

	// The server and its sockets are changed in their own thread: the settings can be changed in the thread of a request
	if (modelId == MI_SERVER_CONNECTION){
		QMetaObject::invokeMethod(this, [this](){ MoveToConfiguredPort(); }, Qt::AutoConnection);
	}
	else{
		QMetaObject::invokeMethod(this, [this](){ RestartForSslConfiguration(); }, Qt::AutoConnection);
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
		BaseClass2::RegisterModel(m_sslConfigurationModelCompPtr.GetPtr(), MI_SSL_CONFIGURATION);
	}

	if (m_webServerInterfaceModelCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_webServerInterfaceModelCompPtr.GetPtr(), MI_SERVER_CONNECTION);
	}

	if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr){
		m_isStartRequested = true;

		EnsureServerStarted();
	}

	connect(&m_timer, &QTimer::timeout, this, &CWebSocketServerComp::OnTimeout);

	m_timer.start(5000);

	m_isInitialized = true;
}


void CWebSocketServerComp::OnComponentDestroyed()
{
	// First: a notification from another thread does not start the server again
	BaseClass2::UnregisterAllModels();

	// Not virtual: in the destructor of the wrapper a virtual call gets a wrong this
	CWebSocketServerComp::StopServer();

	m_timer.stop();

	// The threads of the connections use the references of the component: they are stopped while it has them
	StopConnectionThreads();

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
	m_isStartRequested = true;

	return EnsureServerStarted();
}


bool CWebSocketServerComp::StopServer()
{
	m_isStartRequested = false;

	StopListening();

	return true;
}


IServer::ServerStatus CWebSocketServerComp::GetServerStatus() const
{
	if (m_webSocketServerPtr.IsValid() && m_webSocketServerPtr->isListening()){
		return SS_LISTENING;
	}

	return SS_NOT_STARTED;
}


// private methods

void CWebSocketServerComp::StopListening()
{
	// Nothing to close before the server has listened once
	if (m_webSocketServerPtr.IsValid()){
		m_webSocketServerPtr->close();
	}
}


bool CWebSocketServerComp::GetSecureConnection(bool& isSecure, QSslConfiguration& sslConfiguration) const
{
	iprm::TParamsPtr<iprm::IEnableableParam> sslEnableParamPtr(m_sslConfigurationCompPtr.GetPtr(), imtcom::ISslConfigurationManager::ParamKeys::s_enableSslModeParamKey);

	isSecure = sslEnableParamPtr.IsValid() && sslEnableParamPtr->IsEnabled();
	if (!isSecure){
		return true;
	}

	return m_sslConfigurationManagerCompPtr.IsValid() && m_sslConfigurationManagerCompPtr->CreateSslConfiguration(*m_sslConfigurationCompPtr, sslConfiguration);
}


bool CWebSocketServerComp::RestartServer()
{
	for (CWebSocketThread* webSocketThreadPtr : std::as_const(m_webSocketThreadList)){
		QWebSocket* webSocketPtr = const_cast<QWebSocket*>(webSocketThreadPtr->GetWebSocket());
		if (webSocketPtr != nullptr){
			webSocketPtr->close(QWebSocketProtocol::CloseCodeGoingAway);
		}
	}

	StopListening();

	return EnsureServerStarted();
}


void CWebSocketServerComp::RestartForSslConfiguration()
{
	// The server that is stopped (StopServer) stays stopped. Checked when the queued call runs: the server can be stopped meanwhile
	if (!m_isStartRequested){
		return;
	}

	// The settings loaded again unchanged keep the connections, also of the server that does not listen: its port is tried again only
	// when it is set again. A refused encryption leaves no server object, the next settings start it anew
	bool isSecure = false;
	QSslConfiguration sslConfiguration;
	if (		m_webSocketServerPtr.IsValid() &&
				GetSecureConnection(isSecure, sslConfiguration) &&
				(isSecure == (m_webSocketServerPtr->secureMode() == QWebSocketServer::SecureMode)) &&
				(!isSecure || (sslConfiguration == m_webSocketServerPtr->sslConfiguration()))){
		return;
	}

	RestartServer();
}


void CWebSocketServerComp::MoveToConfiguredPort()
{
	// Checked when the queued call runs: the server can be stopped meanwhile
	if (!m_isStartRequested || !m_webServerInterfaceCompPtr.IsValid()){
		return;
	}

	// The notification can be of the host, of another protocol or of the same settings loaded again: only another port moves the server.
	// The port of the system (0) would be another one each time, a protocol without a port has -1
	const int port = m_webServerInterfaceCompPtr->GetPort(imtcom::IServerConnectionInterface::PT_WEBSOCKET);
	if (port <= 0){
		return;
	}

	const bool isListening = m_webSocketServerPtr.IsValid() && m_webSocketServerPtr->isListening();
	if (isListening && (m_webSocketServerPtr->serverPort() == port)){
		m_requestedPort = port;

		return;
	}

	// The port that could not be listened on is tried again only when it is set again after another one
	if (port == m_requestedPort){
		return;
	}

	// Without a server object (it could not be secure as it is asked) the server is started anew, with the check of the encryption
	if (!m_webSocketServerPtr.IsValid()){
		EnsureServerStarted();

		return;
	}

	m_requestedPort = port;

	const quint16 previousPort = isListening ? m_webSocketServerPtr->serverPort() : 0;
	if (ListenAgain(quint16(port)) || (previousPort == 0)){
		return;
	}

	if (ListenAgain(previousPort)){
		SendErrorMessage(0, QStringLiteral("Web socket server stays on port %1").arg(previousPort));
	}
	else{
		SendErrorMessage(0, QStringLiteral("Web socket server could not return to port %1, it does not listen").arg(previousPort));
	}
}


void CWebSocketServerComp::StopConnectionThreads()
{
	for (CWebSocketThread* webSocketThreadPtr : std::as_const(m_webSocketThreadList)){
		webSocketThreadPtr->quit();
		webSocketThreadPtr->wait();
	}
}


bool CWebSocketServerComp::ListenAgain(quint16 port)
{
	m_webSocketServerPtr->close();

	if (m_webSocketServerPtr->listen(QHostAddress::Any, port)){
		SendInfoMessage(0, QStringLiteral("Web socket server successfully started on port %1").arg(port));

		return true;
	}

	SendErrorMessage(0, QStringLiteral("Web socket server could not be started on port %1").arg(port));

	return false;
}


bool CWebSocketServerComp::EnsureServerStarted()
{
	bool retVal = false;

	if (m_protocolEngineCompPtr.IsValid()){
		if (m_webServerInterfaceCompPtr.IsValid()){
			int port = m_webServerInterfaceCompPtr->GetPort(imtcom::IServerConnectionInterface::PT_WEBSOCKET);

			m_requestedPort = port;

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
	QSslConfiguration sslConfiguration;
	if (!GetSecureConnection(isSecureConnection, sslConfiguration)){
		// The server that is asked to be secure must not work without the encryption: the clients would send the data as plain text.
		// Without a server object a move to another port starts it anew, with this check
		m_webSocketServerPtr.Reset();

		SendErrorMessage(0, QStringLiteral("Could not enable secure connection (SSL) on web socket server, the server is not started"));

		return false;
	}

	if (isSecureConnection){
		m_webSocketServerPtr.SetPtr(new QWebSocketServer("", QWebSocketServer::SecureMode, this));
		m_webSocketServerPtr->setSslConfiguration(sslConfiguration);

		SendInfoMessage(0, QStringLiteral("Secure connection (SSL) enabled on web socket server"));
	}
	else{
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

	// The server that could not listen now listens later on another port (ListenAgain): it is connected already
	connect(m_webSocketServerPtr.GetPtr(), &QWebSocketServer::newConnection, this, &CWebSocketServerComp::HandleNewConnections);
	connect(m_webSocketServerPtr.GetPtr(), &QWebSocketServer::acceptError, this, &CWebSocketServerComp::OnAcceptError);
	connect(m_webSocketServerPtr.GetPtr(), &QWebSocketServer::sslErrors, this, &CWebSocketServerComp::OnSslErrors);

	if (m_webSocketServerPtr->listen(address, port)){
		SendInfoMessage(0, QStringLiteral("Web socket server successfully started on port %1").arg(port));

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
		QString message = QStringLiteral("Handle new web socket connection, (Subprotocol: '%1', Threads: %2)").arg(subprotocol).arg(m_webSocketThreadList.count()) ;
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

