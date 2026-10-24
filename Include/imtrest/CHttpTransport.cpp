// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpTransport.h>


#ifdef IMTREST_HTTP_SERVER_AVAILABLE


// Qt includes
#include <QtNetwork/QTcpSocket>

// ImtCore includes
#include <imtrest/CHttpServerShard.h>


namespace imtrest
{


// public methods of the class CHttpListener

CHttpListener::CHttpListener(CHttpTransport& transport)
	:QTcpServer(&transport),
	m_transport(transport)
{
}


// protected methods of the class CHttpListener

// reimplemented (QTcpServer)

void CHttpListener::incomingConnection(qintptr socketDescriptor)
{
	m_transport.DispatchSocketDescriptor(socketDescriptor);
}


// public methods

CHttpTransport::CHttpTransport(IHttpTransportHandler& handler, QObject* parentPtr)
	:QObject(parentPtr),
	m_handler(handler),
	m_listenerPtr(new CHttpListener(*this)),
	m_isSecure(false),
	m_nextIoThreadIndex(0)
{
}


CHttpTransport::~CHttpTransport()
{
	Shutdown();
}


void CHttpTransport::SetConfiguration(const Configuration& configuration)
{
	Q_ASSERT_X(m_ioThreads.isEmpty(), "CHttpTransport::SetConfiguration", "Configuration must be set before the transport is started");

	m_configuration = configuration;
}


const CHttpTransport::Configuration& CHttpTransport::GetConfiguration() const
{
	return m_configuration;
}


bool CHttpTransport::Listen(const QHostAddress& address, quint16 port, const QSslConfiguration* sslConfigurationPtr)
{
	if (!EnsureIoThreadsStarted()){
		return false;
	}

	if (m_listenerPtr->isListening()){
		m_listenerPtr->close();
	}

	m_isSecure = (sslConfigurationPtr != nullptr);
	if (m_isSecure){
		m_sslConfiguration = *sslConfigurationPtr;

		// Only HTTP/1.1 is served by the transport:
		m_sslConfiguration.setAllowedNextProtocols(QList<QByteArray>() << QByteArray(QSslConfiguration::NextProtocolHttp1_1));
	}
	else{
		m_sslConfiguration = QSslConfiguration();
	}

	m_listenerPtr->setListenBacklogSize(m_configuration.listenBacklogSize);

	return m_listenerPtr->listen(address, port);
}


void CHttpTransport::StopListening()
{
	m_listenerPtr->close();
}


bool CHttpTransport::IsListening() const
{
	return m_listenerPtr->isListening();
}


quint16 CHttpTransport::GetServerPort() const
{
	return m_listenerPtr->serverPort();
}


QString CHttpTransport::GetErrorString() const
{
	return m_listenerPtr->errorString();
}


void CHttpTransport::Shutdown()
{
	m_listenerPtr->close();

	{
		QMutexLocker locker(&m_requestRoutesMutex);

		m_requestRoutes.clear();
	}

	for (const IoThread& ioThread : std::as_const(m_ioThreads)){
		CHttpServerShard* shardPtr = ioThread.shardPtr;
		if (shardPtr != nullptr){
			// The shard must be destroyed in its own thread:
			QMetaObject::invokeMethod(
						ioThread.contextPtr,
						[shardPtr](){
							shardPtr->Shutdown();
							delete shardPtr;
						},
						Qt::BlockingQueuedConnection);
		}

		ioThread.contextPtr->deleteLater();

		ioThread.threadPtr->quit();
		ioThread.threadPtr->wait();

		delete ioThread.threadPtr;
	}

	m_ioThreads.clear();
}


bool CHttpTransport::SendResponse(const QByteArray& requestId, const IHttpTransportHandler::ResponseData& response) const
{
	QMutexLocker locker(&m_requestRoutesMutex);

	CHttpServerShard* shardPtr = m_requestRoutes.take(requestId);
	if (shardPtr == nullptr){
		return false;
	}

	// Posting under the lock guarantees that the shard is not destroyed in the meantime (see Shutdown):
	QMetaObject::invokeMethod(
				shardPtr,
				[shardPtr, requestId, response](){
					shardPtr->SendResponse(requestId, response);
				},
				Qt::QueuedConnection);

	return true;
}


int CHttpTransport::GetIoThreadCount() const
{
	return int(m_ioThreads.count());
}


int CHttpTransport::GetActiveConnectionCount() const
{
	int retVal = 0;

	for (const IoThread& ioThread : m_ioThreads){
		retVal += ioThread.shardPtr->GetConnectionCount();
	}

	return retVal;
}


// protected methods

void CHttpTransport::DispatchSocketDescriptor(qintptr socketDescriptor)
{
	const int ioThreadCount = int(m_ioThreads.count());

	// Least loaded I/O thread, the start index is rotated to distribute equally loaded threads:
	CHttpServerShard* targetShardPtr = nullptr;
	int minConnectionCount = 0;
	for (int i = 0; i < ioThreadCount; ++i){
		CHttpServerShard* shardPtr = m_ioThreads[(m_nextIoThreadIndex + i) % ioThreadCount].shardPtr;

		int connectionCount = shardPtr->GetConnectionCount();
		if ((targetShardPtr == nullptr) || (connectionCount < minConnectionCount)){
			targetShardPtr = shardPtr;
			minConnectionCount = connectionCount;
		}
	}

	if (targetShardPtr == nullptr){
		QTcpSocket socket;
		if (socket.setSocketDescriptor(socketDescriptor)){
			socket.abort();
		}

		return;
	}

	m_nextIoThreadIndex = (m_nextIoThreadIndex + 1) % ioThreadCount;

	targetShardPtr->ReserveConnection();

	const bool isSecure = m_isSecure;
	const QSslConfiguration sslConfiguration = m_sslConfiguration;

	QMetaObject::invokeMethod(
				targetShardPtr,
				[targetShardPtr, socketDescriptor, isSecure, sslConfiguration](){
					targetShardPtr->AcceptSocket(socketDescriptor, isSecure, sslConfiguration);
				},
				Qt::QueuedConnection);
}


void CHttpTransport::RegisterRequest(const QByteArray& requestId, CHttpServerShard* shardPtr)
{
	QMutexLocker locker(&m_requestRoutesMutex);

	m_requestRoutes.insert(requestId, shardPtr);
}


void CHttpTransport::UnregisterRequest(const QByteArray& requestId)
{
	QMutexLocker locker(&m_requestRoutesMutex);

	m_requestRoutes.remove(requestId);
}


void CHttpTransport::TransferWebSocket(QTcpSocket* socketPtr)
{
	Q_ASSERT(socketPtr->thread() == thread());

	QMetaObject::invokeMethod(
				this,
				[this, socketPtr](){
					if (!m_handler.OnWebSocketHandshake(socketPtr)){
						socketPtr->abort();
						socketPtr->deleteLater();
					}
				},
				Qt::QueuedConnection);
}


// private methods

bool CHttpTransport::EnsureIoThreadsStarted()
{
	if (!m_ioThreads.isEmpty()){
		return true;
	}

	int ioThreadCount = m_configuration.ioThreadCount;
	if (ioThreadCount <= 0){
		ioThreadCount = QThread::idealThreadCount();
	}

	ioThreadCount = qMax(1, ioThreadCount);

	for (int i = 0; i < ioThreadCount; ++i){
		IoThread ioThread;
		ioThread.threadPtr = new QThread;
		ioThread.threadPtr->setObjectName(QStringLiteral("HttpIoThread-%1").arg(i));

		ioThread.contextPtr = new QObject;
		ioThread.contextPtr->moveToThread(ioThread.threadPtr);

		ioThread.threadPtr->start();

		// The shard (including the internal objects of QtHttpServer) must be created in its own thread:
		CHttpServerShard* shardPtr = nullptr;
		QMetaObject::invokeMethod(
					ioThread.contextPtr,
					[this, &shardPtr](){
						shardPtr = new CHttpServerShard(*this, m_handler, m_configuration);
						if (!shardPtr->Initialize()){
							delete shardPtr;
							shardPtr = nullptr;
						}
					},
					Qt::BlockingQueuedConnection);

		ioThread.shardPtr = shardPtr;

		m_ioThreads.append(ioThread);

		if (shardPtr == nullptr){
			qWarning("CHttpTransport: I/O thread could not be initialized");

			Shutdown();

			return false;
		}
	}

	return true;
}


} // namespace imtrest


#endif // IMTREST_HTTP_SERVER_AVAILABLE


