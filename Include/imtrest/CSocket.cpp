// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CMultiThreadServer.h>
#include <imtrest/CSocketTimeoutWatcher.h>

// Qt includes
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslKey>


namespace imtrest
{


CSocket::CSocket(CSocketThread* rootSocket, IRequest* request, bool secureConnection, const QSslConfiguration& sslConfiguration, qintptr socketDescriptor)
	:QObject(),
	m_rootSocket(rootSocket),
	m_requestPtr(request)
{
	if (secureConnection){
		m_socket = new QSslSocket();

#if 0
		qDebug() << "SSL Build Version:" << QSslSocket::sslLibraryBuildVersionString();
		qDebug() << "Supports SSL:" << QSslSocket::supportsSsl();
		qDebug() << "SSL Library Version:" << QSslSocket::sslLibraryVersionString();
#endif
		Q_ASSERT(QSslSocket::supportsSsl());
	}
	else {
		m_socket = new QTcpSocket();
	}

	if (!m_socket->setSocketDescriptor(socketDescriptor)){
		qDebug() << "Socket error" << m_socket->error();

		// something's wrong, we just Q_EMIT a signal
		Q_EMIT m_rootSocket->Error(m_socket->error());

		return;
	}

	if (secureConnection){
		QSslSocket* socketPtr = dynamic_cast<QSslSocket*>(m_socket.data());
		connect(socketPtr, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(OnHandleSslErrors(QList<QSslError>)));

		Q_ASSERT_X(socketPtr != nullptr, __func__, "Invalid socket!");
		socketPtr->setLocalCertificate(sslConfiguration.localCertificate());
		socketPtr->setPrivateKey(sslConfiguration.privateKey());
		socketPtr->setProtocol(sslConfiguration.protocol());

		socketPtr->startServerEncryption();
	}

	m_timeoutWatcherPtr = new CSocketTimeoutWatcher(
				*m_socket,
				m_rootSocket->GetConnectionTimeoutConfig(),
				CConnectionTimeoutBehavior::CreateForProtocol(m_rootSocket->GetProtocolTypeId()),
				this);

	connect(m_socket.data(), &QTcpSocket::disconnected, this, &CSocket::Disconnected, Qt::DirectConnection);
	connect(m_socket.data(), &QTcpSocket::readyRead, this, &CSocket::HandleReadyRead, Qt::DirectConnection);
}


CSocket::~CSocket()
{
	if (!m_socket.isNull()){
		m_socket->abort();
		m_socket->deleteLater();
	}
}


void CSocket::Abort()
{
	if (!m_socket.isNull() && m_socket->isValid()){
		m_socket->abort();
		m_socket->deleteLater();
	}
}


void CSocket::OnHandleSslErrors(QList<QSslError> errorList)
{
	for (const QSslError& error : errorList){
		qDebug() << error.errorString();
	}
}


void CSocket::HandleReadyRead()
{
	if (m_timeoutWatcherPtr != nullptr){
		if (m_timeoutWatcherPtr->IsClosing() || m_timeoutWatcherPtr->IsCloseRequested()){
			// The client should only close the connection after the response of the timeout (RFC 9112, section 9.6)
			// and should not send requests after a request that asked to close it (RFC 9112, section 9.6).
			m_socket->readAll();

			return;
		}

		m_timeoutWatcherPtr->OnDataReceived();
	}

	imtrest::IRequestServlet* requestHandlerPtr = m_rootSocket->GetRequestServlet();
	Q_ASSERT(requestHandlerPtr != nullptr);

	if (requestHandlerPtr == nullptr){
		return;
	}

	if (!m_requestPtr.IsValid()){
		m_requestPtr.SetPtr(m_rootSocket->CreateRequest().PopInterfacePtr());
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
		qDebug() << "Data parsing failed";

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

	const bool isConnectionClosing = !m_requestPtr->IsConnectionPersistent();
	m_rootSocket->SetResponseHints(isConnectionClosing, m_requestPtr->IsResponseBodyOmitted());

	if (m_timeoutWatcherPtr != nullptr){
		m_timeoutWatcherPtr->OnRequestCompleted();

		if (isConnectionClosing){
			m_timeoutWatcherPtr->CloseAfterResponse();
		}
	}

	// Start request handler:
	requestHandlerPtr->ProcessRequest(*m_requestPtr.PopPtr());
}


void CSocket::Disconnected()
{
	Q_EMIT m_rootSocket->SocketDisconnected(m_rootSocket->GetRequestId());
}


void CSocket::OnSendResponse(QByteArray data)
{
	if (!m_socket.isNull() && m_socket->isOpen()){
		if (m_timeoutWatcherPtr != nullptr){
			m_timeoutWatcherPtr->WriteResponse(data);
		}
		else{
			m_socket->write(data);
		}
	}
}


} // namespace imtrest


