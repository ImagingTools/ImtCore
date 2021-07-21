#include <imtrest/CWebSocketServerComp.h>


// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>
#include <imtrest/ISender.h>


namespace imtrest
{


// public methods

// reimplemented (IRequestHandler)

IRequestServlet::ConstResponsePtr CWebSocketServerComp::ProcessRequest(const IRequest& request) const
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

// reimplemented (icomp::CComponentBase)

void CWebSocketServerComp::OnComponentCreated()
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

bool CWebSocketServerComp::StartListening(const QHostAddress &address, quint16 port)
{
    if (!m_protocolEngineCompPtr.IsValid()){
	return false;
    }

	istd::TDelPtr<QWebSocketServer> webSocketServerPtr(new QWebSocketServer("",QWebSocketServer::NonSecureMode,this));
	if (webSocketServerPtr->listen(address, port)){
	connect(webSocketServerPtr.GetPtr(), &QWebSocketServer::newConnection, this, &CWebSocketServerComp::HandleNewConnections, Qt::UniqueConnection);

	m_servers.push_back(webSocketServerPtr.PopPtr());

	return true;
    }
    else{
	SendErrorMessage(0, QString("Server could not be started on %1:%2").arg(address.toString()).arg(port));
    }

    return false;
}


// private slots

void CWebSocketServerComp::HandleNewConnections()
{
	QWebSocketServer* webSocketServerPtr = qobject_cast<QWebSocketServer*>(sender());
	Q_ASSERT(webSocketServerPtr != nullptr);

	while (QWebSocket* socketPtr = webSocketServerPtr->nextPendingConnection()){
		if (m_subscriberEngineCompPtr.IsValid()){
			m_subscriberEngineCompPtr->RegisterSubscriber(socketPtr, *m_requestHandlerCompPtr, *m_protocolEngineCompPtr);
		}

		IRequest* newRequestPtr = m_protocolEngineCompPtr->CreateRequest(socketPtr, *this);
		if (newRequestPtr != nullptr){
			m_requests.PushBack(newRequestPtr);

			QObject::connect(socketPtr, &QWebSocket::disconnected, this, &CWebSocketServerComp::OnSocketDisconnected);
		}
    }
}


void CWebSocketServerComp::OnSocketDisconnected()
{
    QObject* socketObjectPtr = sender();
    Q_ASSERT(socketObjectPtr != nullptr);

	if (m_subscriberEngineCompPtr.IsValid()){
		m_subscriberEngineCompPtr->UnRegisterSubscriber(socketObjectPtr);
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


QByteArray CWebSocketServerComp::GetSupportedCommandId() const
{
    return QByteArray();
}


} // namespace imtrest


