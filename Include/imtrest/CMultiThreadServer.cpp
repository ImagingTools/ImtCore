// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CMultiThreadServer.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QMutableListIterator>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/ISender.h>


namespace imtrest
{


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

	qintptr descriptor = PopSocketDescriptor();
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


qintptr CMultiThreadServer::PopSocketDescriptor()
{
	QMutexLocker lock(&m_descriptorListMutex);

	Q_ASSERT(!m_descriptorList.isEmpty());

	qintptr retVal = m_descriptorList.front();

	m_descriptorList.pop_front();

	return retVal;
}


void CMultiThreadServer::incomingConnection(qintptr socketDescriptor)
{
	if (!m_isActive){
		return;
	}

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
			qintptr descriptor = PopSocketDescriptor();

			threadSocket = new CSocketThread(descriptor, m_isSecureConnection, m_sslConfiguration, this);
			connect(threadSocket, &CSocketThread::SocketDisconnected, this, &CMultiThreadServer::Disconnected, Qt::QueuedConnection);

			m_threadSocketList.append(threadSocket);

			threadSocket->start();
		}
	}
}


} // namespace imtrest


