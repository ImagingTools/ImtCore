#include <imtclientgql/CWebSocketClientComp.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QMessageAuthenticationCode>
#include <QtCore/QUrl>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtbase/CTreeItemModel.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtrest/CWebSocketSender.h>


namespace imtclientgql
{


// public methods

CWebSocketClientComp::CWebSocketClientComp()
	: m_loginStatus(imtauth::ILoginStatusProvider::LSF_CACHED)
{
}


bool CWebSocketClientComp::SendResponse(imtrest::ConstResponsePtr& response) const
{
	m_webSocket.sendTextMessage(response->GetData());

	return true;
}


bool CWebSocketClientComp::SendRequest(imtrest::ConstRequestPtr& request) const
{
	m_webSocket.sendTextMessage(request->GetBody());

	return true;
}


int CWebSocketClientComp::GetLoginStatus(const QByteArray& /*clientId*/) const
{
	return m_loginStatus;
}


// reimplemented (imtrest::IRequestManager)

const imtrest::IRequest* CWebSocketClientComp::GetRequest(const QByteArray& /*requestId*/) const
{
	return nullptr;
}


const imtrest::ISender* CWebSocketClientComp::GetSender(const QByteArray& /*requestId*/) const
{
	return this;
}


bool CWebSocketClientComp::SendRequest(const imtgql::IGqlRequest& request, imtgql::IGqlResponseHandler& responseHandler) const
{
	QString key = QUuid::createUuid().toString(QUuid::WithoutBraces);
	//	QString key = request.GetFactoryId();

	QJsonObject dataObject;
	dataObject["type"] = "query";
	dataObject["id"] = key;
	QJsonObject payloadObject;
	payloadObject["data"] = QString(request.GetQuery());
	dataObject["payload"] = payloadObject;
	QByteArray data = QJsonDocument(dataObject).toJson(QJsonDocument::Compact);

	m_webSocket.sendTextMessage(data);
	NetworkOperation networkOperation(10000, this);

	while(1){
		int resultCode = networkOperation.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);
		if(resultCode == 1){
			if(m_queryDataMap.contains(key)){
				responseHandler.OnReply(request, m_queryDataMap.value(key));
				m_queryDataMap.remove(key);

				return true;
			}
			continue;
		}
		else{
			break;
		}
	}

	return false;
}


// protected methods

QByteArray CWebSocketClientComp::Sign(const QByteArray& message, const QByteArray& key) const
{
	if (!key.isEmpty()){
		return QMessageAuthenticationCode::hash(message, key, QCryptographicHash::Sha256);
	}

	return QCryptographicHash::hash(message, QCryptographicHash::Sha256);
}



void CWebSocketClientComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	connect(&m_webSocket, &QWebSocket::connected, this, &CWebSocketClientComp::OnWebSocketConnected);
	connect(&m_webSocket, &QWebSocket::disconnected, this, &CWebSocketClientComp::OnWebSocketDisConnected);
	connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &CWebSocketClientComp::OnWebSocketTextMessageReceived);
	connect(&m_webSocket, &QWebSocket::binaryMessageReceived, this, &CWebSocketClientComp::OnWebSocketBinaryMessageReceived);
	connect(&m_webSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnWebSocketError(QAbstractSocket::SocketError)));

	connect(&m_refreshTimer, &QTimer::timeout, this, &CWebSocketClientComp::OnConnectedTimer);

	m_refreshTimer.setSingleShot(true);
	m_refreshTimer.setInterval(5000);
	Connect();
}


void CWebSocketClientComp::OnComponentDestroyed()
{
	disconnect(&m_webSocket, &QWebSocket::connected, this, &CWebSocketClientComp::OnWebSocketConnected);
	m_refreshTimer.stop();
	m_webSocket.disconnect();

	BaseClass::OnComponentDestroyed();
}


void CWebSocketClientComp::OnConnectedTimer()
{
	Connect();
}

void CWebSocketClientComp::OnWebSocketConnected()
{
	m_loginStatus = imtauth::ILoginStatusProvider::LSF_LOGGED_IN;
	istd::IChangeable::ChangeSet loginChangeSet(m_loginStatus, QObject::tr("Login"));
	istd::CChangeNotifier notifier(this, &loginChangeSet);

	QString clientId;
	if (m_clientIdAttrPtr.IsValid()){
		clientId = *m_clientIdAttrPtr;
	}

	if (m_clientIdCompPtr.IsValid()){
		clientId = m_clientIdCompPtr->GetText();
	}

	QString body = "{ \"type\": \"connection_init\" %1}";
	if (clientId.isEmpty()){
		body = body.arg("");
	}
	else{
		body = body.arg(", \"clientId\": \"" + clientId + "\"");
	}

	m_webSocket.sendTextMessage(body);
}


void CWebSocketClientComp::OnWebSocketDisConnected()
{
	m_loginStatus = imtauth::ILoginStatusProvider::LSF_CACHED;
	istd::IChangeable::ChangeSet loginChangeSet(m_loginStatus, QObject::tr("Logout"));
	istd::CChangeNotifier notifier(this, &loginChangeSet);

	m_startQueries.Reset();

	m_refreshTimer.start();
}


void CWebSocketClientComp::OnWebSocketError(QAbstractSocket::SocketError /*error*/)
{
	QString errorText = m_webSocket.errorString();

	qDebug() << errorText;
}


void CWebSocketClientComp::OnWebSocketTextMessageReceived(const QString& message)
{
	QWebSocket* webSocketPtr = dynamic_cast<QWebSocket*>(sender());

	if (webSocketPtr == nullptr){
		return;
	}

	if (!m_protocolEngineCompPtr.IsValid()){
		return;
	}

	istd::TDelPtr<imtrest::CWebSocketRequest> webSocketRequest(new imtrest::CWebSocketRequest(*m_protocolEngineCompPtr.GetPtr()));
	webSocketRequest->SetBody(message.toUtf8());

	imtrest::ConstResponsePtr responsePtr;
	imtrest::CWebSocketRequest::MethodType methodType = webSocketRequest->GetMethodType();
	if (
			methodType == imtrest::CWebSocketRequest::MT_CONNECTION_ASK ||
			methodType == imtrest::CWebSocketRequest::MT_KEEP_ALIVE
			){
	}
	else if (methodType == imtrest::CWebSocketRequest::MT_QUERY_DATA){
		m_queryDataMap.insert(webSocketRequest->GetRequestId(), webSocketRequest->GetBody());

		emit OnQueryDataReceived(1);
	}
	else if (methodType == imtrest::CWebSocketRequest::MT_ERROR
			 || methodType == imtrest::CWebSocketRequest::MT_START_ASK
			 || methodType == imtrest::CWebSocketRequest::MT_DATA){
		responsePtr = m_clientRequestHandlerCompPtr->ProcessRequest(*webSocketRequest);
	}
	else{
		responsePtr = m_serverRequestHandlerCompPtr->ProcessRequest(*webSocketRequest);
	}

	if (responsePtr.IsValid()){
		QByteArray data = responsePtr->GetData();
		webSocketPtr->sendTextMessage(data);
		if (methodType == imtrest::CWebSocketRequest::MT_START){
			m_startQueries.PushBack(webSocketRequest.PopPtr());
		}
	}
}


void CWebSocketClientComp::OnWebSocketBinaryMessageReceived(const QByteArray& message)
{
	//	qDebug() << "CWebSocketClientComp::OnWebSocketBinaryMessageReceived: " << message;
}


void CWebSocketClientComp::Connect()
{
	qDebug() << "CWebSocketClientComp Connect";

	QString host = *m_serverAddressAttrPtr;
	QString port = QString::number(*m_serverPortAttrPtr);
	QString login = *m_serverLoginAttrPtr;
	QString password = *m_serverPasswordAttrPtr;
	QJsonObject authorization;

	if (m_webSocketServerAddressCompPtr.IsValid()){
		host = m_webSocketServerPortCompPtr->GetText();
	}
	if (m_webSocketServerPortCompPtr.IsValid()){
		port = m_webSocketServerPortCompPtr->GetText();
	}
	if (m_webSocketServerLoginCompPtr.IsValid()){
		login = m_webSocketServerLoginCompPtr->GetText();
	}
	if (m_webSocketServerPasswordCompPtr.IsValid()){
		port = m_webSocketServerPasswordCompPtr->GetText();
	}
	authorization["host"] = host;
	authorization["port"] = port;
	authorization["login"] = login;
	authorization["password"] = password;
	QByteArray authHeader = QJsonDocument(authorization).toJson(QJsonDocument::Compact);

	QUrl url;
	url.setHost(host);
	url.setPath("/realtime");
	url.setQuery("header=" + authHeader.toBase64() + "&payload=e30=");
	url.setScheme("ws");
	url.setPort(port.toInt());
	qDebug() << "url" << url;
	m_webSocket.open(url);
}

// public methods of the embedded class NetworkOperation

CWebSocketClientComp::NetworkOperation::NetworkOperation(int timeout, const CWebSocketClientComp* parent)
{
	Q_ASSERT(parent != nullptr);

	timerFlag = false;

	// If the network reply is finished, the internal event loop will be finished:
	QObject::connect(parent, &CWebSocketClientComp::OnQueryDataReceived, &connectionLoop, &QEventLoop::exit, Qt::DirectConnection);

	// If the application will be finished, the internal event loop will be also finished:
	QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, &connectionLoop, &QEventLoop::quit, Qt::DirectConnection);

	// If a timeout for the request was defined, start the timer:
	if (timeout > 0){
		timer.setSingleShot(true);

		// If the timer is running out, the internal event loop will be finished:
		QObject::connect(&timer, &QTimer::timeout, &connectionLoop, &QEventLoop::quit, Qt::DirectConnection);

		timer.start(timeout);
	}
}


CWebSocketClientComp::NetworkOperation::~NetworkOperation()
{
	timer.stop();
}


} // namespace imtclientgql


