#include <imtrest/CWebSocketServerComp.h>


// Acf includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>
#include <imtrest/ISender.h>
#include <imtrest/CWebSocketRequest.h>


namespace imtrest
{


// public methods

// reimplemented (IRequestHandler)

ConstResponsePtr CWebSocketServerComp::ProcessRequest(const IRequest& request) const
{
	ConstResponsePtr retVal;

//	if (m_requestHandlerCompPtr.IsValid()){
//		retVal = m_requestHandlerCompPtr->ProcessRequest(request);
//		if (retVal.IsValid()){
//			request.GetProtocolEngine().GetSender().SendResponse(*retVal);
//		}
//	}

	return retVal;
}


QByteArray CWebSocketServerComp::GetSupportedCommandId() const
{
	return QByteArray();
}


const IRequest* CWebSocketServerComp::GetRequest(const QByteArray& requestId) const
{
	return nullptr;
}


const ISender* CWebSocketServerComp::GetSender(const QByteArray& requestId) const
{
	return nullptr;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CWebSocketServerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_protocolEngineCompPtr.IsValid() && m_requestHandlerCompPtr.IsValid()){
		if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr){
			if (m_serverAddressAttrPtr.IsValid() && m_webSocketServerPortCompPtr.IsValid()){
				QString port = m_webSocketServerPortCompPtr->GetText();
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

	while (QWebSocket* webSocketPtr = webSocketServerPtr->nextPendingConnection()){
		if (m_subscriberEngineCompPtr.IsValid()){
			m_subscriberEngineCompPtr->RegisterSubscriber(webSocketPtr, *m_requestHandlerCompPtr, *m_protocolEngineCompPtr);
		}
		connect(webSocketPtr, &QWebSocket::textMessageReceived, this, &CWebSocketServerComp::OnWebSocketTextMessage);
		connect(webSocketPtr, &QWebSocket::binaryMessageReceived, this, &CWebSocketServerComp::OnWebSocketBinaryMessage);

	}
}


void CWebSocketServerComp::OnSocketDisconnected()
{
	QObject* socketObjectPtr = sender();
	Q_ASSERT(socketObjectPtr != nullptr);

	if (m_subscriberEngineCompPtr.IsValid()){
		m_subscriberEngineCompPtr->UnRegisterSubscriber(socketObjectPtr);
	}

//	for (int i = 0; i < m_requests.GetCount(); ++i){
//		IRequest* requestPtr = m_requests.GetAt(i);
//		Q_ASSERT(requestPtr != nullptr);

//		if (&requestPtr->GetSocketObject() == socketObjectPtr){
//			m_requests.RemoveAt(i);

//			break;
//		}
//	}

	socketObjectPtr->deleteLater();
}


void CWebSocketServerComp::OnWebSocketTextMessage(const QString& textMessage)
{
	QWebSocket* webSocketPtr = dynamic_cast<QWebSocket*>(sender());

	if (webSocketPtr == nullptr){
		return;
	}

	istd::TDelPtr<IRequest> newRequestPtr = m_protocolEngineCompPtr->CreateRequest(*this);
	if (newRequestPtr.IsValid()){
		CWebSocketRequest* webSocketRequest = dynamic_cast<CWebSocketRequest*>(newRequestPtr.GetPtr());
		if (webSocketRequest == nullptr){
			return;
		}
		webSocketRequest->SetBody(textMessage.toUtf8());
		ProcessRequest(*webSocketRequest);

		if (webSocketRequest->GetMethodType() == CWebSocketRequest::MT_START){
			newRequestPtr.PopPtr();
			m_requests.PushBack(webSocketRequest);
			QObject::connect(webSocketPtr, &QWebSocket::disconnected, this, &CWebSocketServerComp::OnSocketDisconnected);
		}
	}
}


void CWebSocketServerComp::OnWebSocketBinaryMessage(const QByteArray& dataMessage)
{
}



} // namespace imtrest


