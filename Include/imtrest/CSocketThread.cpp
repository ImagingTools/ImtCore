// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	if (m_server == nullptr){
		return;
	}

	imtrest::IRequestUniquePtr newRequestPtr = m_enginePtr->CreateRequest(*this);
	Q_ASSERT(newRequestPtr.IsValid());
	m_requestId = newRequestPtr->GetRequestId();

	m_socket.SetPtr(new CSocket(this, newRequestPtr.PopInterfacePtr(), m_isSecureConnection, m_sslConfiguration, m_socketDescriptor));

	bool retVal = connect(this, &CSocketThread::OnSendResponse, m_socket.GetPtr(), &CSocket::OnSendResponse, Qt::QueuedConnection);
	Q_ASSERT(retVal);

	retVal = connect(this, &CSocketThread::Abort, m_socket.GetPtr(), &CSocket::Abort, Qt::QueuedConnection);
	Q_ASSERT(retVal);

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


} // namespace imtrest


