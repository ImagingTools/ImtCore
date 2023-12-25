#include <imtrest/CMultiThreadServer.h>

// Qt includes
#include <QtCore/QCoreApplication>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/ISender.h>
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpSender.h>


namespace imtrest {

CSocket::CSocket(CSocketThread* rootSocket, IRequest* request, qintptr socketDescriptor)
	: QObject(),
	m_rootSocket(rootSocket),
	m_socket(new QTcpSocket()),
	m_requestPtr(request)
{
	if (!m_socket->setSocketDescriptor(socketDescriptor)){
		qDebug() << "Socket error" << m_socket->error();
		// something's wrong, we just emit a signal
		emit m_rootSocket->Error(m_socket->error());
		return;
	}

	qDebug() << "socketDescriptor" << m_socket->socketDescriptor() << " New socket";

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
		m_socket->disconnect();
	}
}


void CSocket::HandleReadyRead()
{
	m_startTimer.stop();
	imtrest::IRequestServlet* requestHandlerPtr = m_rootSocket->GetRequestServlet();
	Q_ASSERT(requestHandlerPtr != nullptr);

	if (!m_requestPtr.IsValid() || requestHandlerPtr == nullptr){
		return;
	}

	if (!m_socket->isOpen()){
		m_socket->open(QIODevice::ReadWrite);
	}

	if (!m_socket->isTransactionStarted()){
		m_socket->startTransaction();
	}

	if (m_requestPtr->GetState() == IRequest::RS_MESSAGE_COMPLETE){
		m_requestPtr->ResetData();
	}

	// Get state of request data:
	if (!m_requestPtr->ParseDeviceData(*m_socket)){
		m_socket->disconnect();

		return;
	}

	// Not all data was read:
	if (m_requestPtr->GetState() != IRequest::RS_MESSAGE_COMPLETE){

		return;
	}

	// All request data was read:
	m_socket->commitTransaction();

	// Start request handler:
	requestHandlerPtr->ProcessRequest(*m_requestPtr.PopPtr());
}


void CSocket::Disconnected()
{
	qDebug() << m_socket->socketDescriptor() << " Disconnected";

	emit m_rootSocket->SocketDisconnected(m_rootSocket->GetRequestId());
}


void CSocket::OnSendResponse(ConstResponsePtr response)
{
	CHttpSender cHttpSender(m_socket.GetPtr());
	cHttpSender.SendResponse(response);
}


CSocketThread::CSocketThread(qintptr ID, CMultiThreadServer* parent) :
	QThread(parent),
	m_status(ST_START)
{
	qRegisterMetaType<ConstResponsePtr>("ConstResponsePtr");
	this->m_socketDescriptor = ID;

	m_server = parent;

	m_enginePtr = m_server->GetProtocolEngine();
	m_requestHandlerPtr = m_server->GetRequestServlet();
}


void CSocketThread::SetSocketDescriptor(qintptr socketDescriptor)
{
	QMutexLocker loc(&m_socketDescriptorMutex);
	m_socketDescriptor = socketDescriptor;
}

qintptr CSocketThread::GetSocketDescriptor()
{
	return m_socketDescriptor;
}


void CSocketThread::SetSocketStatus(Status socketStatus)
{
	QMutexLocker loc(&m_statusMutex);
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
	emit m_server->NewThreadConnection(&request);
	ConstResponsePtr retVal;

	return retVal;
}


QByteArray CSocketThread::GetSupportedCommandId() const
{
	return QByteArray();
}


// reimplemented (ISender)
bool CSocketThread::SendResponse(ConstResponsePtr& response) const
{
	emit OnSendResponse(response);

	return true;
}


bool CSocketThread::SendRequest(ConstRequestPtr& reguest) const
{
	return false;
}


CMultiThreadServer::CMultiThreadServer(CTcpServerComp* rootServer) :
	QTcpServer((QObject*)rootServer),
	m_rootServer(*rootServer),
	m_isActive(true)
{
	connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &CMultiThreadServer::AboutToQuit, Qt::DirectConnection);
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
	for (CSocketThread* socket : m_threadSocketList){
		if (socket->GetRequestId() == requestId){
			return socket;
		}
	}

	return nullptr;
}


void CMultiThreadServer::Disconnected(QByteArray requestId)
{
	for (CSocketThread* socket : m_threadSocketList){
		if (socket->GetRequestId() == requestId){
			if (m_threadSocketList.removeOne(socket)){
				socket->quit();
				socket->wait(1000);
				socket->deleteLater();
			}
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


void CMultiThreadServer::AboutToQuit()
{
	m_isActive = false;

	for (CSocketThread* socket : m_threadSocketList){
		emit socket->Abort();
		socket->wait(1000);
		socket->quit();
		socket->wait(1000);
		socket->deleteLater();
	}
}


void CMultiThreadServer::AddSocketDescriptor(qintptr socketDescriptor)
{
	QMutexLocker loc(&m_descriptorListMutex);

	m_descriptorList << socketDescriptor;
}


qintptr CMultiThreadServer::GetFirstSocketDescriptor()
{
	QMutexLocker loc(&m_descriptorListMutex);

	qintptr retVal = m_descriptorList[0];
	m_descriptorList.removeAt(0);

	return retVal;
}


// This function is called by QTcpServer when a new connection is available.
void CMultiThreadServer::incomingConnection(qintptr socketDescriptor)
{
	if (!m_isActive){
		return;
	}

	// We have a new connection
	qDebug() << socketDescriptor << " Connecting..." << m_rootServer.GetThreadsLimit();

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
