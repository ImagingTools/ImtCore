#include <imtrest/CWebSocketServerComp.h>


// ACF includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtrest/CWebSocketRequest.h>
#include <imtauth/ILoginStatusProvider.h>


namespace imtrest
{


// public methods


const IRequest* CWebSocketServerComp::GetRequest(const QByteArray& /*requestId*/) const
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

	if (m_protocolEngineCompPtr.IsValid()){
		if (m_startServerOnCreateAttrPtr.IsValid() && *m_startServerOnCreateAttrPtr){
			if (m_webSocketServerPortCompPtr.IsValid()){
				int port = m_webSocketServerPortCompPtr->GetUrl().port();

				StartListening(QHostAddress::Any, port);
			}
			else{
				StartListening();
			}
		}
	}

	connect(&m_timer, &QTimer::timeout, this, &CWebSocketServerComp::OnTimeout);

	m_timer.start(10000);
}


int CWebSocketServerComp::GetLoginStatus(const QByteArray& clientId) const
{
	return m_senderLoginStatusMap.value(clientId, imtauth::ILoginStatusProvider::LSF_CACHED);
}


// private methods

bool CWebSocketServerComp::StartListening(const QHostAddress &address, quint16 port)
{
	if (!m_protocolEngineCompPtr.IsValid()){
		return false;
	}

	istd::TDelPtr<QWebSocketServer> webSocketServerPtr(new QWebSocketServer("",QWebSocketServer::NonSecureMode,this));
	if (webSocketServerPtr->listen(address, port)){
		SendInfoMessage(0, QString("Web socket server successfully started on port %1").arg(port));
		qDebug() << QString("Web server successfully started on port %1").arg(port);

		connect(webSocketServerPtr.GetPtr(), &QWebSocketServer::newConnection, this, &CWebSocketServerComp::HandleNewConnections, Qt::UniqueConnection);

		m_servers.push_back(webSocketServerPtr.PopPtr());

		return true;
	}
	else{
		SendErrorMessage(0, QString("Server could not be started on port %1").arg(port));
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
			m_subscriberEngineCompPtr->RegisterSubscriber(webSocketPtr, *m_requestServerHandlerCompPtr, *m_protocolEngineCompPtr);
		}

		QString message = QString("Handle new web socket connection");
		SendVerboseMessage(message, "CWebSocketServerComp");

		connect(webSocketPtr, &QWebSocket::textMessageReceived, this, &CWebSocketServerComp::OnWebSocketTextMessage);
		connect(webSocketPtr, &QWebSocket::binaryMessageReceived, this, &CWebSocketServerComp::OnWebSocketBinaryMessage);
		connect(webSocketPtr, &QWebSocket::disconnected, this, &CWebSocketServerComp::OnSocketDisconnected);

#if (QT_VERSION >= 0x060500)
		connect(webSocketPtr, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred), this, &CWebSocketServerComp::OnError);
#else
		connect(webSocketPtr, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &CWebSocketServerComp::OnError);
#endif
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

			int loginStatus = 0;
			istd::IChangeable::ChangeSet loginChangeSet(loginStatus, QObject::tr("Logout"));
			loginChangeSet.SetChangeInfo("ClientId", key);
			istd::CChangeNotifier notifier(this, &loginChangeSet);
			m_senderLoginStatusMap.remove(key);

			QString message = QString("Web socket sender %1 removed").arg(qPrintable(key));
			SendInfoMessage(0, message, "CWebSocketServerComp");

			break;
		}
	}

	QString message = QString("Web socket disconnected");
	SendInfoMessage(0, message, "CWebSocketServerComp");

	socketObjectPtr->deleteLater();
}


void CWebSocketServerComp::OnWebSocketTextMessage(const QString& textMessage)
{
	if (!m_requestServerHandlerCompPtr.IsValid()){
		return;
	}

	QWebSocket* webSocketPtr = dynamic_cast<QWebSocket*>(sender());

	if (webSocketPtr == nullptr){
		return;
	}

	QString message = QString("Web socket text message received: %1").arg(textMessage);
	SendVerboseMessage(message, "CWebSocketServerComp");

	qDebug() << message;

	istd::TDelPtr<IRequest> newRequestPtr = m_protocolEngineCompPtr->CreateRequest(*m_requestServerHandlerCompPtr.GetPtr());
	if (newRequestPtr.IsValid()){
		CWebSocketRequest* webSocketRequest = dynamic_cast<CWebSocketRequest*>(newRequestPtr.GetPtr());
		if (webSocketRequest == nullptr){
			return;
		}
		webSocketRequest->SetBody(textMessage.toUtf8());

		imtrest::CWebSocketRequest::MethodType methodType = webSocketRequest->GetMethodType();
		if (methodType == CWebSocketRequest::MT_START){
			newRequestPtr.PopPtr();
			QSharedPointer<CWebSocketSender> socketSender(new CWebSocketSender(webSocketPtr));
			m_senders.insert(webSocketRequest->GetRequestId(), socketSender);
			QObject::connect(webSocketPtr, &QWebSocket::disconnected, this, &CWebSocketServerComp::OnSocketDisconnected);
		}

		imtrest::ConstResponsePtr responsePtr;

		if (
					methodType == CWebSocketRequest::MT_CONNECTION_ASK ||
					methodType == CWebSocketRequest::MT_START_ASK ||
					methodType == CWebSocketRequest::MT_DATA ||
					methodType == CWebSocketRequest::MT_ERROR ||
					methodType == CWebSocketRequest::MT_QUERY_DATA	){
			responsePtr = m_requestClientHandlerCompPtr->ProcessRequest(*webSocketRequest);
		}
		else{
			if (methodType == CWebSocketRequest::MT_CONNECTION_INIT){
				int loginStatus = imtauth::ILoginStatusProvider::LSF_LOGGED_IN;
				QByteArray clientId = webSocketRequest->GetClientId();
				istd::IChangeable::ChangeSet loginChangeSet(loginStatus, QObject::tr("Login"));
				loginChangeSet.SetChangeInfo("ClientId", clientId);
				istd::CChangeNotifier notifier(this, &loginChangeSet);
				m_senderLoginStatusMap.insert(clientId, imtauth::ILoginStatusProvider::LSF_LOGGED_IN);
				if (!clientId.isEmpty()){
					newRequestPtr.PopPtr();
					QSharedPointer<CWebSocketSender> socketSender(new CWebSocketSender(webSocketPtr));
					m_senders.insert(clientId, socketSender);
					QObject::connect(webSocketPtr, &QWebSocket::disconnected, this, &CWebSocketServerComp::OnSocketDisconnected);

				}
			}
			responsePtr = m_requestServerHandlerCompPtr->ProcessRequest(*webSocketRequest);
		}
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
}


void CWebSocketServerComp::OnError(QAbstractSocket::SocketError /*error*/)
{
	QWebSocket* webSocketPtr = dynamic_cast<QWebSocket*>(sender());
	if (webSocketPtr != nullptr){
		QString errorMessage = QString("Web socket server error: %1").arg(webSocketPtr->errorString());

		SendErrorMessage(0, errorMessage, "CWebSocketServerComp");
	}
}


void CWebSocketServerComp::OnTimeout()
{
	for (const QByteArray& key: m_senders.keys()){
		if (!m_senders[key].isNull()){
			QWebSocket* webSocketPtr = const_cast<QWebSocket*>(m_senders[key]->GetSocket());
			if (webSocketPtr != nullptr){
				webSocketPtr->sendTextMessage(QString(R"({"type": "keep_alive"})"));
			}
		}
	}
}


} // namespace imtrest


