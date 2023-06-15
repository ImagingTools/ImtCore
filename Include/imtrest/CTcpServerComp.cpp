#include <imtrest/CTcpServerComp.h>


// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>
#include <imtrest/ISender.h>
#include <imtrest/CMultiThreadServer.h>


namespace imtrest
{


// public methods

// reimplemented (IRequestHandler)

IRequestServlet::ConstResponsePtr CTcpServerComp::ProcessRequest(const IRequest& request) const
{
	ConstResponsePtr retVal;

	if (m_requestHandlerCompPtr.IsValid()){
		retVal = m_requestHandlerCompPtr->ProcessRequest(request);
		if (retVal.IsValid()){
			request.GetProtocolEngine().GetSender().SendResponse(*retVal);
		}
	}

	return retVal;
}


// protected methods

// reimplemented (ibase::TRuntimeStatusHanderCompWrap)

void CTcpServerComp::OnSystemShutdown()
{
}


// reimplemented (icomp::CComponentBase)

void CTcpServerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_protocolEngineCompPtr.IsValid() && m_requestHandlerCompPtr.IsValid()){
		if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr){
			if (m_serverAddressAttrPtr.IsValid() && m_serverPortCompPtr.IsValid()){
				QString port = m_serverPortCompPtr->GetText();
				StartListening(QHostAddress((*m_serverAddressAttrPtr).toStdString().c_str()), port.toInt());
			}
			else if (m_serverAddressAttrPtr.IsValid() && m_serverPortAttrPtr.IsValid()){
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
	if ((m_isMultiThreadingAttrPtr.IsValid() && *m_isMultiThreadingAttrPtr)){
		istd::TDelPtr<CMultiThreadServer> tcpServerPtr(new CMultiThreadServer(this));
		if (tcpServerPtr->listen(address, port)){
			SendInfoMessage(0, QString("Server successfully started on %1:%2").arg(address.toString()).arg(port));

			connect(tcpServerPtr.GetPtr(), &CMultiThreadServer::newThreadConnection, this, &CTcpServerComp::HandleNewThreadConnections, Qt::DirectConnection);

			m_servers.push_back(tcpServerPtr.PopPtr());

			return true;
		}
		else{
			SendErrorMessage(0, QString("Server could not be started on %1:%2").arg(address.toString()).arg(port));
		}
	}
	else{
		istd::TDelPtr<QTcpServer> tcpServerPtr(new QTcpServer(this));
		if (tcpServerPtr->listen(address, port)){
			SendInfoMessage(0, QString("Server successfully started on %1:%2").arg(address.toString()).arg(port));

			connect(tcpServerPtr.GetPtr(), &QTcpServer::newConnection, this, &CTcpServerComp::HandleNewConnections, Qt::UniqueConnection);

			m_servers.push_back(tcpServerPtr.PopPtr());

			return true;
		}
		else{
			SendErrorMessage(0, QString("Server could not be started on %1:%2").arg(address.toString()).arg(port));
		}
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

			QObject::connect(socketPtr, &QTcpSocket::disconnected, this, &CTcpServerComp::OnSocketDisconnected);
		}
	}
}


void CTcpServerComp::HandleNewThreadConnections(QTcpSocket* socketPtr)
{
	if (socketPtr == nullptr){
		return;
	}

	IRequest* newRequestPtr = m_protocolEngineCompPtr->CreateRequest(socketPtr, *this);
	if (newRequestPtr != nullptr){
		m_requests.PushBack(newRequestPtr);

		QObject::connect(socketPtr, &QTcpSocket::disconnected, this, &CTcpServerComp::OnSocketDisconnected);
	}
}


void CTcpServerComp::OnSocketDisconnected()
{
	QObject* socketObjectPtr = sender();
	if (socketObjectPtr == nullptr){
		//Q_ASSERT(false);

		return;
	}

	for (int i = 0; i < m_requests.GetCount(); ++i){
		IRequest* requestPtr = m_requests.GetAt(i);
		Q_ASSERT(requestPtr != nullptr);

		if (&requestPtr->GetSocketObject() == socketObjectPtr){
			m_requests.RemoveAt(i);

			qDebug("Request deleted");

			break;
		}
	}

	socketObjectPtr->deleteLater();
}


QByteArray CTcpServerComp::GetSupportedCommandId() const
{
	return QByteArray();
}


} // namespace imtrest


