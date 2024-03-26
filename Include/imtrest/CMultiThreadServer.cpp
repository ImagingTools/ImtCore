#include <imtrest/CMultiThreadServer.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QMutableListIterator>

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


CSocket::CSocket(CSocketThread* rootSocket, IRequest* request, qintptr socketDescriptor)
	:QObject(),
	m_rootSocket(rootSocket),
	m_socket(new QTcpSocket()),
	m_requestPtr(request)
{
	if (!m_socket->setSocketDescriptor(socketDescriptor)){
		qDebug() << "Socket error" << m_socket->error();

		// something's wrong, we just Q_EMIT a signal
		Q_EMIT m_rootSocket->Error(m_socket->error());

		return;
	}

	connect(&m_startTimer, &QTimer::timeout, this, QOverload<>::of(&CSocket::TimeOut));
	m_startTimer.setSingleShot(true);
	m_startTimer.start(5000);

	connect(m_socket.GetPtr(), &QTcpSocket::disconnected, this, &CSocket::Disconnected, Qt::DirectConnection);
	connect(m_socket.GetPtr(), &QTcpSocket::readyRead, this, &CSocket::HandleReadyRead, Qt::DirectConnection);
}


void CSocket::Abort()
{
	if (m_socket->isValid()){
		m_socket->abort();
		m_socket.PopPtr()->deleteLater();
	}
}


void CSocket::TimeOut()
{
	if (m_socket != nullptr){
		qDebug() << m_socket->socketDescriptor() << " Time out";
		m_socket->abort();
	}
}


void CSocket::HandleReadyRead()
{
	imtrest::IRequestServlet* requestHandlerPtr = m_rootSocket->GetRequestServlet();
	Q_ASSERT(requestHandlerPtr != nullptr);

	if (requestHandlerPtr == nullptr){
		return;
	}

	if (!m_requestPtr.IsValid()){
		m_requestPtr.SetPtr(m_rootSocket->CreateRequest());
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


void CSocket::Disconnected()
{
	Q_EMIT m_rootSocket->SocketDisconnected(m_rootSocket->GetRequestId());
}


void CSocket::OnSendResponse(ConstResponsePtr response)
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
}


CSocketThread::CSocketThread(qintptr socketId, CMultiThreadServer* parent)
	:QThread(parent),
	m_status(ST_START)
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


void CSocketThread::run()
{
	if (m_server == nullptr){
		return;
	}

	imtrest::IRequest* newRequestPtr = m_enginePtr->CreateRequest(*this);
	m_socket.SetPtr(new CSocket(this, newRequestPtr, m_socketDescriptor));

	m_requestId = newRequestPtr->GetRequestId();

	qDebug() << m_socketDescriptor << connect(this, &CSocketThread::OnSendResponse, m_socket.GetPtr(), &CSocket::OnSendResponse, Qt::QueuedConnection);
	qDebug() << m_socketDescriptor << connect(this, &CSocketThread::Abort, m_socket.GetPtr(), &CSocket::Abort, Qt::QueuedConnection);

	exec();
}


// reimplemented (IRequestHandler)

ConstResponsePtr CSocketThread::ProcessRequest(const IRequest& request) const
{
	Q_EMIT m_server->NewThreadConnection(&request);

	return ConstResponsePtr();
}


QByteArray CSocketThread::GetSupportedCommandId() const
{
	return QByteArray();
}


// reimplemented (ISender)

bool CSocketThread::SendResponse(ConstResponsePtr& response) const
{
	Q_EMIT OnSendResponse(response);

	return true;
}


bool CSocketThread::SendRequest(ConstRequestPtr& reguest) const
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
	:QTcpServer((QObject*)rootServer),
	m_rootServer(*rootServer),
	m_threadSocketListGuard(QReadWriteLock::Recursive),
	m_isActive(true)
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


// reimplemented (imtrest::IRequestManager)

const IRequest* CMultiThreadServer::GetRequest(const QByteArray& requestId) const
{
	return nullptr;
}


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
	qDebug() << __FILE__ << __LINE__;
	QWriteLocker threadListLock(&m_threadSocketListGuard);

	qDebug() << __FILE__ << __LINE__ << requestId;

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
	CSocketThread* threadSocket = new CSocketThread(descriptor, this);

	threadSocket->start();

	connect(threadSocket, &CSocketThread::SocketDisconnected, this, &CMultiThreadServer::Disconnected, Qt::DirectConnection);
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

	qDebug() << socketDescriptor << " Connecting..." << m_rootServer.GetThreadsLimit();

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
			threadSocket = new CSocketThread(descriptor, this);

			m_threadSocketList.append(threadSocket);
			qDebug() << "new socket thread" << m_threadSocketList.count();

			threadSocket->start();

			connect(threadSocket, &CSocketThread::SocketDisconnected, this, &CMultiThreadServer::Disconnected, Qt::QueuedConnection);
		}
	}
}


} // namespace imtrest


