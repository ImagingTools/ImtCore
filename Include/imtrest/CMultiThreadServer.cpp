#include <imtrest/CMultiThreadServer.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QMutableListIterator>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslKey>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/ISender.h>
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpResponse.h>
#include <imtrest/CHttpSender.h>
#include <imtrest/CTcpResponse.h>
#include <imtrest/CTcpSender.h>


namespace imtrest
{


CSocket::CSocket(qintptr socketDescriptor, bool secureConnection, const QSslConfiguration& sslConfiguration, CMultiThreadServer *parent)
	:QObject(),
	m_isSecureConnection(secureConnection),
	m_sslConfiguration(sslConfiguration)
{
	if (secureConnection){
		m_socket = new QSslSocket;
	}
	else {
		m_socket = new QTcpSocket;
	}

	if (!m_socket->setSocketDescriptor(socketDescriptor)){
		qDebug() << "Socket error" << m_socket->error();
		return;
	}

	m_server = parent;
	m_enginePtr = m_server->GetProtocolEngine();
	m_requestHandlerPtr = m_server->GetRequestServlet();

	if (m_enginePtr == nullptr){
		return;
	}
	m_requestPtr.SetPtr(m_enginePtr->CreateRequest(*this));
	if (m_requestPtr.IsValid()){
		m_requestId = m_requestPtr->GetRequestId();
	}

	if (secureConnection){
		QEventLoop encryptionLoop;
		QTimer encryptionTimer;
		connect(&encryptionTimer, &QTimer::timeout, &encryptionLoop, &QEventLoop::quit);

		QSslSocket* socketPtr =m_socket.Cast<QSslSocket*>();
		Q_ASSERT_X(socketPtr != nullptr, __func__, "Invalid socket!");
		socketPtr->setLocalCertificate(sslConfiguration.localCertificate());
		socketPtr->setPrivateKey(sslConfiguration.privateKey());
		socketPtr->setProtocol(sslConfiguration.protocol());

		socketPtr->startServerEncryption();
	}

	connect(&m_startTimer, &QTimer::timeout, this, QOverload<>::of(&CSocket::OnTimeOut));
	m_startTimer.setSingleShot(true);
	m_startTimer.start(5000);

	connect(m_socket.GetPtr(), &QTcpSocket::disconnected, this, &CSocket::OnDisconnected, Qt::DirectConnection);
	connect(m_socket.GetPtr(), &QTcpSocket::readyRead, this, &CSocket::OnHandleReadyRead, Qt::DirectConnection);
}


QByteArray CSocket::GetRequestId()
{
	return m_requestId;
}


imtrest::IRequestServlet* CSocket::GetRequestServlet()
{
	return m_requestHandlerPtr;
}


bool CSocket::IsSecureConnection() const
{
	return m_isSecureConnection;
}


void CSocket::EnableSecureConnection(bool isSecureConnection)
{
	m_isSecureConnection = isSecureConnection;
}


IRequest* CSocket::CreateRequest()
{
	if (m_enginePtr == nullptr){
		return nullptr;
	}
	imtrest::IRequest* newRequestPtr = m_enginePtr->CreateRequest(*this);
	if (newRequestPtr != nullptr){
	}

	return newRequestPtr;
}


// reimplemented (IRequestHandler)

bool CSocket::IsCommandSupported(const QByteArray& /*commandId*/) const
{
	return true;
}


ConstResponsePtr CSocket::ProcessRequest(const IRequest& request, const QByteArray& subCommandId) const
{
	Q_EMIT m_server->NewThreadConnection(&request, subCommandId);

	return ConstResponsePtr();
}

// reimplemented (ISender)
bool CSocket::SendResponse(ConstResponsePtr& response) const
{
	CHttpResponse* httpResponsePtr = const_cast<CHttpResponse*>(dynamic_cast<const CHttpResponse*>(response.GetPtr()));
	CTcpResponse* tcpResponsePtr = const_cast<CTcpResponse*>(dynamic_cast<const CTcpResponse*>(response.GetPtr()));

	if(httpResponsePtr != nullptr){
		CHttpSender sender(m_socket.GetPtr());

		sender.SendResponse(response);
	}
	else if(tcpResponsePtr != nullptr){
		CTcpSender sender(m_socket.GetPtr());

		sender.SendResponse(response);
	}

	return true;
}


bool CSocket::SendRequest(ConstRequestPtr& /*reguest*/) const
{
	return false;
}


void CSocket::OnAbort()
{
	if (m_socket->isValid()){
		m_socket->abort();
		m_socket.PopPtr()->deleteLater();
	}
}


void CSocket::OnTimeOut()
{
	if (m_socket != nullptr){
		qDebug() << m_socket->socketDescriptor() << " Time out";
		m_socket->abort();
	}
}


void CSocket::OnHandleReadyRead()
{
	imtrest::IRequestServlet* requestHandlerPtr = GetRequestServlet();
	Q_ASSERT(requestHandlerPtr != nullptr);

	if (requestHandlerPtr == nullptr){
		return;
	}

	if (!m_socket->isOpen()){
		if (m_socket->open(QIODevice::ReadWrite)){
			m_startTimer.stop();
			m_startTimer.setInterval(20000);
			m_startTimer.start();
		}
	}

	if (!m_socket->isTransactionStarted()){
		m_socket->startTransaction();
	}

	if (m_requestPtr->GetState() == IRequest::RS_MESSAGE_COMPLETE){
		m_requestPtr->ResetData();
	}

	// Get state of request data:
	if (!m_requestPtr->ParseDeviceData(*m_socket)){
		qDebug() << "ParseDeviceData error";
		m_socket->rollbackTransaction();

		return;
	}

	// Not all data was read:
	if (m_requestPtr->GetState() != IRequest::RS_MESSAGE_COMPLETE){
		qDebug() << "Not all data was read";

		return;
	}

	// All request data was read:
	m_socket->commitTransaction();

	m_startTimer.stop();

	// Start request handler:
	requestHandlerPtr->ProcessRequest(*m_requestPtr.PopPtr());
}


void CSocket::OnDisconnected()
{
	Q_EMIT SocketDisconnected(m_requestId);
}


CMultiThreadServer::CMultiThreadServer(CTcpServerComp* rootServer)
#if QT_VERSION < 0x060400
	:QTcpServer((QObject*)rootServer),
#else
	:QSslServer((QObject*)rootServer),
#endif
	m_rootServer(*rootServer),
	m_socketListGuard(QReadWriteLock::Recursive),
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


QSslConfiguration& CMultiThreadServer::GetSslConfiguration()
{
	return m_sslConfiguration;
}


// reimplemented (imtrest::IRequestManager)

const ISender* CMultiThreadServer::GetSender(const QByteArray& requestId) const
{
	QReadLocker threadListLock(&m_socketListGuard);

	for (CSocket* socket : m_socketList){
		if (socket->GetRequestId() == requestId){
			return socket;
		}
	}

	return nullptr;
}


void CMultiThreadServer::Disconnected(QByteArray requestId)
{
	QWriteLocker threadListLock(&m_socketListGuard);

	SendLogMessage(
				istd::IInformationProvider::InformationCategory::IC_NONE,
				__LINE__,
				QString("Socket '%1' disconnected").arg(qPrintable(requestId)),
				__func__);

	QMutableListIterator socketIterator(m_socketList);
	while(socketIterator.hasNext()){
		QPointer<CSocket>& socketPtr = socketIterator.next();
		if(socketPtr.isNull()){
			// socket is already removed. Stop tracking and go to next
			socketIterator.remove();

			continue;
		}
		QByteArray socketRequestId = socketPtr->GetRequestId();
		if (requestId.isEmpty() || socketRequestId == requestId){
			socketPtr->deleteLater();
			socketIterator.remove();
		}
	}

	if (m_descriptorList.isEmpty()){
		return;
	}

	qintptr descriptor = GetFirstSocketDescriptor();
	CSocket* threadSocket = new CSocket(descriptor, m_isSecureConnection, GetSslConfiguration(), this);

	connect(threadSocket, &CSocket::SocketDisconnected, this, &CMultiThreadServer::Disconnected, Qt::DirectConnection);
}


void CMultiThreadServer::ShutdownServer()
{
	m_isActive = false;

	QWriteLocker listLock(&m_socketListGuard);

	QMutableListIterator socketIterator(m_socketList);
	while(socketIterator.hasNext()){
		QPointer<CSocket>& socketPtr = socketIterator.next();
		if(!socketPtr.isNull()){
			socketPtr->deleteLater();
		}

		socketIterator.remove();
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
				QString("New connection. Limit: %1").arg(QString::number(m_rootServer.GetThreadsLimit())),
				__func__);

	QWriteLocker listLock(&m_socketListGuard);
	AddSocketDescriptor(socketDescriptor);

	CSocket* socket = nullptr;

	if (socket == nullptr){
		int threadsLimit = m_rootServer.GetThreadsLimit();
		if (threadsLimit < 1){
			threadsLimit = 1;
		}

		if (m_socketList.count() < threadsLimit){
			// Every new connection will be run in a newly created thread
			qintptr descriptor = GetFirstSocketDescriptor();
			socket = new CSocket(descriptor, m_isSecureConnection, GetSslConfiguration(), this);

			m_socketList.append(socket);
			SendLogMessage(
						istd::IInformationProvider::InformationCategory::IC_NONE,
						__LINE__,
						QString("Starting new socket. Active sockets count: %1").arg(QString::number( m_socketList.count())),
						__func__);

			connect(socket, &CSocket::SocketDisconnected, this, &CMultiThreadServer::Disconnected, Qt::QueuedConnection);
		}
	}
}


} // namespace imtrest


