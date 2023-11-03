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


const IRequest* CWebSocketServerComp::GetRequest(const QByteArray& requestId) const
{
	return nullptr;
}


const ISender* CWebSocketServerComp::GetSender(const QByteArray& requestId) const
{
	CWebSocketSender* sender = m_senders.value(requestId).data();
	if (sender != nullptr){
		return sender;
	}

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
		SendInfoMessage(0, QString("Web server successfully started on %1:%2").arg(address.toString()).arg(port));
		qDebug() << QString("Web server successfully started on %1:%2").arg(address.toString()).arg(port);

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

		QString message = QString("Handle new web socket connection");
		SendInfoMessage(0, message, "CWebSocketServerComp");
		qDebug() << message;

		connect(webSocketPtr, &QWebSocket::textMessageReceived, this, &CWebSocketServerComp::OnWebSocketTextMessage);
		connect(webSocketPtr, &QWebSocket::binaryMessageReceived, this, &CWebSocketServerComp::OnWebSocketBinaryMessage);
		connect(webSocketPtr, &QWebSocket::disconnected, this, &CWebSocketServerComp::OnSocketDisconnected);
		connect(webSocketPtr, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &CWebSocketServerComp::OnError);
	}
}


void CWebSocketServerComp::OnSocketDisconnected()
{
	QObject* socketObjectPtr = sender();
	Q_ASSERT(socketObjectPtr != nullptr);

	if (m_subscriberEngineCompPtr.IsValid()){
		m_subscriberEngineCompPtr->UnRegisterSubscriber(socketObjectPtr);
	}

	for (QByteArray key: m_senders.keys()){
		if (socketObjectPtr == m_senders[key]->GetSocket()){
			m_senders.remove(key);

			QString message = QString("Web socket sender %1 removed").arg(qPrintable(key));
			SendInfoMessage(0, message, "CWebSocketServerComp");
			qDebug() << message;

			break;
		}
	}

	QString message = QString("Web socket disconnected");
	SendInfoMessage(0, message, "CWebSocketServerComp");
	qDebug() << message;

	socketObjectPtr->deleteLater();
}


void CWebSocketServerComp::OnWebSocketTextMessage(const QString& textMessage)
{
	if (!m_requestHandlerCompPtr.IsValid()){
		return;
	}

	QWebSocket* webSocketPtr = dynamic_cast<QWebSocket*>(sender());

	if (webSocketPtr == nullptr){
		return;
	}

	QString message = QString("Web socket text message received: %1").arg(textMessage);
	SendInfoMessage(0, message, "CWebSocketServerComp");
	qDebug() << message;

	istd::TDelPtr<IRequest> newRequestPtr = m_protocolEngineCompPtr->CreateRequest(*m_requestHandlerCompPtr.GetPtr());
	if (newRequestPtr.IsValid()){
		CWebSocketRequest* webSocketRequest = dynamic_cast<CWebSocketRequest*>(newRequestPtr.GetPtr());
		if (webSocketRequest == nullptr){
			return;
		}
		webSocketRequest->SetBody(textMessage.toUtf8());

		if (webSocketRequest->GetMethodType() == CWebSocketRequest::MT_START){
			newRequestPtr.PopPtr();
			QSharedPointer<CWebSocketSender> socketSender(new CWebSocketSender(webSocketPtr));
			m_senders.insert(webSocketRequest->GetRequestId(), socketSender);
			QObject::connect(webSocketPtr, &QWebSocket::disconnected, this, &CWebSocketServerComp::OnSocketDisconnected);
		}

		imtrest::ConstResponsePtr responsePtr = m_requestHandlerCompPtr->ProcessRequest(*webSocketRequest);
		if (responsePtr.IsValid()){
			QByteArray data = responsePtr->GetData();
			webSocketPtr->sendTextMessage(data);
		}
	}
}


void CWebSocketServerComp::OnWebSocketBinaryMessage(const QByteArray& dataMessage)
{
	QString message = QString("Web socket binary message received: %1").arg(qPrintable(dataMessage));
	SendInfoMessage(0, message, "CWebSocketServerComp");
	qDebug() << message;
}


void CWebSocketServerComp::OnError(QAbstractSocket::SocketError error)
{
	QWebSocket* webSocketPtr = dynamic_cast<QWebSocket*>(sender());
	if (webSocketPtr != nullptr){
		QString errorMessage = QString("Web socket server error: %1").arg(webSocketPtr->errorString());

		SendErrorMessage(0, errorMessage, "CWebSocketServerComp");
		qDebug() << errorMessage;
	}
}


} // namespace imtrest


