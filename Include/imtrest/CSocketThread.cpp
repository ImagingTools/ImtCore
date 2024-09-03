#include <imtrest/CSocketThread.h>

// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QMutableListIterator>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/ISender.h>
#include <imtrest/CMultiThreadServer.h>


namespace imtrest
{


CSocketThread::CSocketThread(qintptr socketId, bool secureConnection, const QSslConfiguration& sslConfiguration, CMultiThreadServer* parent)
	:QThread(parent),
	m_status(ST_START),
	m_isSecureConnection(secureConnection),
	m_sslConfiguration(sslConfiguration)
{
	qRegisterMetaType<ConstResponsePtr>("ConstResponsePtr");
	this->m_socketDescriptor = socketId;

	m_server = parent;

	m_enginePtr = m_server->GetProtocolEngine();
	m_requestHandlerPtr = m_server->GetRequestServlet();
}


void CSocketThread::SetSocketDescriptor(qintptr socketDescriptor)
{
	QMutexLocker lock(&m_socketDescriptorMutex);

	m_socketDescriptor = socketDescriptor;
}

qintptr CSocketThread::GetSocketDescriptor()
{
	return m_socketDescriptor;
}


void CSocketThread::SetSocketStatus(Status socketStatus)
{
	QMutexLocker lock(&m_statusMutex);

	m_status = socketStatus;
}


CSocketThread::Status CSocketThread::GetSocketStatus()
{
	return m_status;
}


QByteArray CSocketThread::GetRequestId()
{
	return m_requestId;
}


imtrest::IRequestServlet* CSocketThread::GetRequestServlet()
{
	return m_requestHandlerPtr;
}


bool CSocketThread::IsSecureConnection() const
{
	return m_isSecureConnection;
}


void CSocketThread::EnableSecureConnection(bool isSecureConnection)
{
	m_isSecureConnection = isSecureConnection;
}


void CSocketThread::run()
{
	if (m_server == nullptr){
		return;
	}

	imtrest::IRequest* newRequestPtr = m_enginePtr->CreateRequest(*this);
	m_socket.SetPtr(new CSocket(this, newRequestPtr, m_isSecureConnection, m_sslConfiguration, m_socketDescriptor));


	m_requestId = newRequestPtr->GetRequestId();

	qDebug() << m_socketDescriptor << connect(this, &CSocketThread::OnSendResponse, m_socket.GetPtr(), &CSocket::OnSendResponse, Qt::QueuedConnection);
	qDebug() << m_socketDescriptor << connect(this, &CSocketThread::Abort, m_socket.GetPtr(), &CSocket::Abort, Qt::QueuedConnection);

	exec();
}


// reimplemented (IRequestHandler)

bool CSocketThread::IsCommandSupported(const QByteArray& /*commandId*/) const
{
	return true;
}


ConstResponsePtr CSocketThread::ProcessRequest(const IRequest& request, const QByteArray& subCommandId) const
{
	Q_EMIT m_server->NewThreadConnection(&request, subCommandId);

	return ConstResponsePtr();
}


// reimplemented (ISender)

bool CSocketThread::SendResponse(ConstResponsePtr& response) const
{
	Q_EMIT OnSendResponse(response);

	return true;
}


bool CSocketThread::SendRequest(ConstRequestPtr& /*reguest*/) const
{
	return false;
}


IRequest* CSocketThread::CreateRequest()
{
	if (m_enginePtr == nullptr){
		return nullptr;
	}
	imtrest::IRequest* newRequestPtr = m_enginePtr->CreateRequest(*this);
	if (newRequestPtr != nullptr){
		m_requestId = newRequestPtr->GetRequestId();
	}

	return newRequestPtr;
}


CMultiThreadServer::CMultiThreadServer(CTcpServerComp* rootServer)
#if QT_VERSION >= 0x060400
	:QSslServer(rootServer),
#else
	:QTcpServer(rootServer),
#endif
	m_rootServer(*rootServer),
	m_threadSocketListGuard(QReadWriteLock::Recursive),
	m_isActive(true),
	m_isSecureConnection(false)
{
	connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &CMultiThreadServer::ShutdownServer, Qt::DirectConnection);
}


CMultiThreadServer::~CMultiThreadServer()
{
	ShutdownServer();
}


imtrest::IRequestServlet* CMultiThreadServer::GetRequestServlet()
{
	return m_rootServer.GetRequestServlet();
}


imtrest::IProtocolEngine* CMultiThreadServer::GetProtocolEngine()
{
	return m_rootServer.GetProtocolEngine();
}


bool CMultiThreadServer::IsSecureConnection() const
{
	return m_isSecureConnection;
}


void CMultiThreadServer::EnableSecureConnection(bool isSecureConnection)
{
	m_isSecureConnection = isSecureConnection;
}


void CMultiThreadServer::SetSslConfiguration(const QSslConfiguration& sslConfiguration)
{
	if (!m_isSecureConnection){
		SendErrorMessage(0, "Unable to set sslconfiguration. Server configured in non secure mode!");
		I_CRITICAL();

		return;
	}

	if (BaseClass::isListening()){
		SendCriticalMessage(0, "Unable to set sslconfiguration. Server is running!");
		I_CRITICAL();

		return;
	}

	m_sslConfiguration = sslConfiguration;
}


// reimplemented (imtrest::IRequestManager)

const ISender* CMultiThreadServer::GetSender(const QByteArray& requestId) const
{
	QReadLocker threadListLock(&m_threadSocketListGuard);

	for (CSocketThread* socket : m_threadSocketList){
		if (socket->GetRequestId() == requestId){
			return socket;
		}
	}

	return nullptr;
}


void CMultiThreadServer::Disconnected(QByteArray requestId)
{
	QWriteLocker threadListLock(&m_threadSocketListGuard);

	SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_NONE,
				__LINE__,
				QString("Socket '%1' disconnected").arg(requestId),
				__func__);

	QMutableListIterator threadSocketIterator(m_threadSocketList);
	while(threadSocketIterator.hasNext()){
		QPointer<CSocketThread>& socketPtr = threadSocketIterator.next();
		if(socketPtr.isNull()){
			// socket is already removed. Stop tracking and go to next
			threadSocketIterator.remove();

			continue;
		}
		if (socketPtr->GetRequestId() == requestId){
			socketPtr->quit();
			socketPtr->wait();
			socketPtr->deleteLater();
			threadSocketIterator.remove();
		}
	}

	if (m_descriptorList.isEmpty()){
		return;
	}

	qintptr descriptor = GetFirstSocketDescriptor();
	CSocketThread* threadSocket = new CSocketThread(descriptor, m_isSecureConnection, m_sslConfiguration, this);
	m_threadSocketList.append(threadSocket);
	connect(threadSocket, &CSocketThread::SocketDisconnected, this, &CMultiThreadServer::Disconnected, Qt::DirectConnection);
	threadSocket->start();
}


void CMultiThreadServer::ShutdownServer()
{
	m_isActive = false;

	QWriteLocker threadListLock(&m_threadSocketListGuard);

	QMutableListIterator threadSocketIterator(m_threadSocketList);
	while(threadSocketIterator.hasNext()){
		QPointer<CSocketThread>& socketPtr = threadSocketIterator.next();
		if(!socketPtr.isNull()){
			socketPtr->quit();
			socketPtr->wait();
			socketPtr->deleteLater();
		}

		threadSocketIterator.remove();
	}
}


void CMultiThreadServer::AddSocketDescriptor(qintptr socketDescriptor)
{
	QMutexLocker lock(&m_descriptorListMutex);

	m_descriptorList << socketDescriptor;
}


qintptr CMultiThreadServer::GetFirstSocketDescriptor()
{
	QMutexLocker lock(&m_descriptorListMutex);

	qintptr retVal = m_descriptorList[0];
	m_descriptorList.removeAt(0);

	return retVal;
}


void CMultiThreadServer::incomingConnection(qintptr socketDescriptor)
{
	if (!m_isActive){
		return;
	}

	SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_NONE,
				__LINE__,
				QString("New connection. Limit: %0").arg(QString::number(m_rootServer.GetThreadsLimit())),
				__func__);

	QWriteLocker threadListLock(&m_threadSocketListGuard);
	AddSocketDescriptor(socketDescriptor);

	CSocketThread* threadSocket = nullptr;

	if (threadSocket == nullptr){
		int threadsLimit = m_rootServer.GetThreadsLimit();
		if (threadsLimit < 1){
			threadsLimit = 1;
		}

		if (m_threadSocketList.count() < threadsLimit){
			// Every new connection will be run in a newly created thread
			qintptr descriptor = GetFirstSocketDescriptor();
			threadSocket = new CSocketThread(descriptor, m_isSecureConnection, m_sslConfiguration, this);

			m_threadSocketList.append(threadSocket);
			SendLogMessage(
						istd::IInformationProvider::InformationCategory::IC_NONE,
						__LINE__,
						QString("Starting new socket. Active sockets count: %0").arg(QString::number( m_threadSocketList.count())),
						__func__);
			connect(threadSocket, &CSocketThread::SocketDisconnected, this, &CMultiThreadServer::Disconnected, Qt::QueuedConnection);
			threadSocket->start();
		}
	}
}


} // namespace imtrest


