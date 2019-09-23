#include <imtrest/CTcpServerComp.h>


// Qt includes
#include <QtNetwork/QNetworkInterface>

// ImtCore includes
#include <imtrest/IRequest.h>


namespace imtrest
{


// public methods

// reimplemented (IRequestHandler)

bool CTcpServerComp::ProcessRequest(const IRequest& request) const
{
	if (m_requestHandlerCompPtr.IsValid()){
		return m_requestHandlerCompPtr->ProcessRequest(request);
	}

	return false;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CTcpServerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_protocolEngineCompPtr.IsValid() && m_requestHandlerCompPtr.IsValid()){
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
	if (!m_protocolEngineCompPtr.IsValid()){
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

	while (QTcpSocket* socketPtr = tcpServerPtr->nextPendingConnection()){
		IRequest* newRequestPtr = m_protocolEngineCompPtr->CreateRequest(socketPtr, *this);
		if (newRequestPtr != nullptr){
			m_requests.PushBack(newRequestPtr);

			QObject::connect(socketPtr, &QTcpSocket::disconnected, &QObject::deleteLater);
		}
	}
}


} // namespace imtrest


