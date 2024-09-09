#include <imtrest/CMultiThreadServer.h>

// Qt includes
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslKey>

// ImtCore includes
#include <imtrest/CHttpResponse.h>
#include <imtrest/CHttpSender.h>
#include <imtrest/CTcpResponse.h>
#include <imtrest/CTcpSender.h>


namespace imtrest
{


CSocket::CSocket(CSocketThread* rootSocket, IRequest* request, bool secureConnection, const QSslConfiguration& sslConfiguration, qintptr socketDescriptor)
	:QObject(),
	m_rootSocket(rootSocket),
	m_requestPtr(request)
{
	if (secureConnection){
		m_socket = new QSslSocket();
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
		Q_ASSERT_X(socketPtr != nullptr, __func__, "Invalid socket!");
		socketPtr->setLocalCertificate(sslConfiguration.localCertificate());
		socketPtr->setPrivateKey(sslConfiguration.privateKey());
		socketPtr->setProtocol(sslConfiguration.protocol());

		socketPtr->startServerEncryption();
	}

	connect(&m_startTimer, &QTimer::timeout, this, QOverload<>::of(&CSocket::TimeOut));
	m_startTimer.setSingleShot(true);
	m_startTimer.start(5000);

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


void CSocket::TimeOut()
{
	if (m_socket != nullptr){
		qDebug() << "Connection time out: " << m_socket->socketDescriptor();

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
		CHttpSender sender(m_socket.data());

		sender.SendResponse(response);
	}
	else if(tcpResponsePtr != nullptr){
		CTcpSender sender(m_socket.data());

		sender.SendResponse(response);
	}
}


} // namespace imtrest


