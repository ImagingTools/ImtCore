#include <imtrest/CTcpServerComp.h>


// Qt includes
#include <QtNetwork/QNetworkInterface>

// ImtCore includes
#include <imtrest/IRequest.h>


namespace imtrest
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CTcpServerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_protocolManagerCompPtr.IsValid() && m_requestHandlerCompPtr.IsValid()){
		if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr){
			if (m_serverAddressAttrPtr.IsValid() && m_serverPortAttrPtr.IsValid()){
				StartListening(QHostAddress((*m_serverAddressAttrPtr).toStdString().c_str()), *m_serverPortAttrPtr);
			}
			else{
				StartListening();
			}
		}
	}
}


// private methods

bool CTcpServerComp::StartListening(const QHostAddress &address, quint16 port)
{
	if (!m_protocolManagerCompPtr.IsValid()){
		return false;
	}

	istd::TDelPtr<QTcpServer> tcpServerPtr(new QTcpServer(this));
	if (tcpServerPtr->listen(address, port)){
		connect(tcpServerPtr.GetPtr(), &QTcpServer::newConnection, this, &CTcpServerComp::HandleNewConnections, Qt::UniqueConnection);

		m_servers.push_back(tcpServerPtr.PopPtr());

		return true;
	}
	else{
		SendErrorMessage(0, QString("Server could not be started on %1:%2").arg(address.toString()).arg(port));
	}

	return false;
}


// private slots

void CTcpServerComp::HandleNewConnections()
{
	QTcpServer* tcpServerPtr = qobject_cast<QTcpServer*>(sender());
	Q_ASSERT(tcpServerPtr != nullptr);

	while (QTcpSocket* socket = tcpServerPtr->nextPendingConnection()){
		Request newRequest(socket);

		m_requests.push_back(newRequest);

		QObject::connect(socket, &QTcpSocket::readyRead, this, &CTcpServerComp::HandleReadyRead);

		QObject::connect(socket, &QTcpSocket::disconnected, &QObject::deleteLater);
	}
}


void CTcpServerComp::HandleReadyRead()
{
	QTcpSocket* socketPtr = qobject_cast<QTcpSocket*>(sender());
	Q_ASSERT(socketPtr != nullptr);

	if (!socketPtr->isTransactionStarted()){
		socketPtr->startTransaction();
	}

	// Read all data:
	QByteArray data = socketPtr->readAll();

	SendVerboseMessage(QString("Incomming data: %1").arg(qPrintable(data)));

	// Get state of request data:
	IProtocolEngine::RequestState requestState = IProtocolEngine::RS_NON_STARTED;
	if (!m_protocolManagerCompPtr->GetRequestState(data, requestState)){
		socketPtr->disconnect();

		return;
	}

	// Not all data was read:
	if (requestState != IProtocolEngine::RS_MESSAGE_COMPLETE){
		return;
	}

	istd::TDelPtr<IRequest> requestPtr = m_protocolManagerCompPtr->CreateRequest(data);
	if (!requestPtr.IsValid()){
		socketPtr->disconnect();

		return;
	}

	// All request data was read:
	socketPtr->commitTransaction();

	// Start request handler:
	HandleRequest(*requestPtr);
}


void CTcpServerComp::HandleRequest(const IRequest& request)
{
	if (m_requestHandlerCompPtr.IsValid()){
		m_requestHandlerCompPtr->ProcessRequest(request, *m_protocolManagerCompPtr);
	}
}


} // namespace imtrest


