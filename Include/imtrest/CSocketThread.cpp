// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CSocketThread.h>

// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QMutableListIterator>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/ITransport.h>
#include <imtrest/CMultiThreadServer.h>


namespace imtrest
{


CSocketThread::CSocketThread(qintptr socketId, bool secureConnection, const QSslConfiguration& sslConfiguration, CMultiThreadServer* parent)
	:QThread(parent),
	m_status(ST_START),
	m_isSecureConnection(secureConnection),
	m_isConnectionClosing(false),
	m_isResponseBodyOmitted(false),
	m_isChunkedResponseAllowed(false),
	m_sslConfiguration(sslConfiguration)
{
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


CConnectionTimeoutConfig CSocketThread::GetConnectionTimeoutConfig() const
{
	return m_server->GetConnectionTimeoutConfig();
}


qint64 CSocketThread::GetWriteHighWatermark() const
{
	return m_server->GetWriteHighWatermark();
}


QByteArray CSocketThread::GetProtocolTypeId() const
{
	return (m_enginePtr != nullptr) ? m_enginePtr->GetProtocolTypeId() : QByteArray();
}


void CSocketThread::SetResponseHints(bool isConnectionClosing, bool isBodyOmitted, bool isChunkedAllowed)
{
	m_isConnectionClosing = isConnectionClosing;
	m_isResponseBodyOmitted = isBodyOmitted;
	m_isChunkedResponseAllowed = isChunkedAllowed;
}


bool CSocketThread::IsConnectionClosing() const
{
	return m_isConnectionClosing;
}


bool CSocketThread::IsResponseBodyOmitted() const
{
	return m_isResponseBodyOmitted;
}


bool CSocketThread::IsChunkedResponseAllowed() const
{
	return m_isChunkedResponseAllowed;
}


bool CSocketThread::SendStreamingResponse(const ConstResponsePtr& response, bool isConnectionClosing, bool isBodyOmitted, bool isChunkedAllowed) const
{
	if (!response.IsValid()){
		return false;
	}

	Q_EMIT OnSendStreamingResponse(response, isConnectionClosing, isBodyOmitted, isChunkedAllowed);

	return true;
}


imtrest::IRequestUniquePtr CSocketThread::CreateRequest() const
{
	if (m_enginePtr == nullptr){
		return nullptr;
	}

	imtrest::IRequestUniquePtr newRequestPtr = m_enginePtr->CreateRequest(*this);
	if (newRequestPtr.IsValid()){
		m_requestId = newRequestPtr->GetRequestId();
	}

	return newRequestPtr;
}


// reimplemented (QThread)

void CSocketThread::run()
{
	if ((m_server == nullptr) || (m_enginePtr == nullptr)){
		return;
	}

	imtrest::IRequestUniquePtr newRequestPtr = m_enginePtr->CreateRequest(*this);
	if (newRequestPtr.IsValid()){
		m_requestId = newRequestPtr->GetRequestId();
	}

	m_socket.SetPtr(new CSocket(this, newRequestPtr.PopInterfacePtr(), m_isSecureConnection, m_sslConfiguration, m_socketDescriptor));

	bool retVal = connect(this, &CSocketThread::OnSendResponse, m_socket.GetPtr(), &CSocket::OnSendResponse, Qt::QueuedConnection);
	Q_ASSERT(retVal);

	retVal = connect(this, &CSocketThread::OnSendStreamingResponse, m_socket.GetPtr(), &CSocket::OnSendStreamingResponse, Qt::QueuedConnection);
	Q_ASSERT(retVal);

	retVal = connect(this, &CSocketThread::Abort, m_socket.GetPtr(), &CSocket::Abort, Qt::QueuedConnection);
	Q_ASSERT(retVal);

	exec();
	m_socket.Reset();
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


// reimplemented (ITransport)

bool CSocketThread::SendData(const QByteArray& data) const
{
	Q_EMIT OnSendResponse(data);

	return true;
}


} // namespace imtrest


