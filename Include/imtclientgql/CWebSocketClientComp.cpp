#include <imtclientgql/CWebSocketClientComp.h>


// Qt includes
#include <QtCore/QMessageAuthenticationCode>
#include <QtCore/QUrl>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>

// ImtCore includes
#include <imtgql/CGqlResponse.h>
#include <imtgql/CGqlRequest.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtrest/CHttpRequest.h>


namespace imtclientgql
{


// public methods

CWebSocketClientComp::CWebSocketClientComp()
{
	m_lastSocketError = QAbstractSocket::SocketError::UnknownSocketError;
}


// reimplemented (imtclientgql::IGqlClient)

IGqlClient::GqlResponsePtr CWebSocketClientComp::SendRequest(IGqlClient::GqlRequestPtr requestPtr, imtbase::IUrlParam* /*urlParamPtr*/) const
{
	QString key = QUuid::createUuid().toString(QUuid::WithoutBraces);

	QJsonObject dataObject;
	dataObject["type"] = "query";
	dataObject["id"] = key;
	QJsonObject payloadObject;
	payloadObject["data"] = QString(requestPtr->GetQuery());
	dataObject["payload"] = payloadObject;
	QJsonObject headersObject;
	const imtgql::IGqlContext* contextPtr = requestPtr->GetRequestContext();
	if (contextPtr != nullptr){
		imtgql::IGqlContext::Headers headers = contextPtr->GetHeaders();
		for (const QByteArray& headerId: headers.keys()){
			headersObject[headerId] = QString(headers.value(headerId));
		}
	}
	dataObject["headers"] = headersObject;

	QString clientId;
	if (m_clientIdCompPtr.IsValid()){
		clientId = m_clientIdCompPtr->GetText();
	}
	if (!clientId.isEmpty()){
		dataObject["clientId"] = clientId;
	}

	QByteArray data = QJsonDocument(dataObject).toJson(QJsonDocument::Compact);

	m_webSocket.sendTextMessage(data);
	NetworkOperation networkOperation(100, this);

	GqlResponsePtr retVal;

	int resultCode = 0;
	for (int i = 0; i < 100; i++){
		networkOperation.timer.start();
		resultCode = networkOperation.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);
		QCoreApplication::processEvents();
		if (resultCode == 1 && m_queryDataMap.contains(key)){
			imtgql::CGqlResponse* responsePtr = new imtgql::CGqlResponse(requestPtr);
			responsePtr->SetResponseData(m_queryDataMap.value(key));
			retVal.reset(responsePtr);

			m_queryDataMap.remove(key);

			return retVal;
		}
	}

	return retVal;
}


// reimplemented (imtrest::ISender)

bool CWebSocketClientComp::SendResponse(imtrest::ConstResponsePtr& response) const
{
	QByteArray data = response->GetData();
	// QJsonDocument document = QJsonDocument::fromJson(data);
	// QJsonObject object = document.object();
	// QByteArray body = object.value("payload").toObject().value("data").toString().toUtf8();

	// 	// QString responseData = responseDataModelPtr->ToJson();
	// if (!body.isEmpty() && object.value("type").toString() != "data"){
	// 	data = QString(R"({"type": "query_data","id": "%1","payload": %2})")
	// 	.arg(object.value("id").toString()).arg(body).toUtf8();
	// }

	m_webSocket.sendTextMessage(data);

	return true;
}


bool CWebSocketClientComp::SendRequest(imtrest::ConstRequestPtr& request) const
{
	m_webSocket.sendTextMessage(request->GetBody());

	return true;
}


// reimplemented (imtrest::IRequestManager)

const imtrest::ISender* CWebSocketClientComp::GetSender(const QByteArray& /*requestId*/) const
{
	return this;
}


// reimplemented (imtcom::IConnectionController)

bool CWebSocketClientComp::Connect()
{
	EmitStartTimer();

	return true;
}


bool CWebSocketClientComp::Disconnect()
{
	EmitStopTimer();
	EmitWebSocketClose();

	return true;
}


// protected methods

QByteArray CWebSocketClientComp::Sign(const QByteArray& message, const QByteArray& key) const
{
	if (!key.isEmpty()){
		return QMessageAuthenticationCode::hash(message, key, QCryptographicHash::Sha256);
	}

	return QCryptographicHash::hash(message, QCryptographicHash::Sha256);
}


// reimplemented (icomp::CComponentBase)

void CWebSocketClientComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	connect(&m_webSocket, &QWebSocket::connected, this, &CWebSocketClientComp::OnWebSocketConnected, Qt::QueuedConnection);
	connect(&m_webSocket, &QWebSocket::disconnected, this, &CWebSocketClientComp::OnWebSocketDisconnected, Qt::QueuedConnection);
	connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &CWebSocketClientComp::OnWebSocketTextMessageReceived);
	connect(&m_webSocket, &QWebSocket::binaryMessageReceived, this, &CWebSocketClientComp::OnWebSocketBinaryMessageReceived);
	connect(this, &CWebSocketClientComp::EmitWebSocketClose, &m_webSocket, &QWebSocket::close);
	connect(this, SIGNAL(EmitStartTimer()), &m_refreshTimer, SLOT(start()), Qt::QueuedConnection);
	connect(this, SIGNAL(EmitStopTimer()), &m_refreshTimer, SLOT(stop()), Qt::QueuedConnection);
	connect(&m_webSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnWebSocketError(QAbstractSocket::SocketError)));

	connect(&m_refreshTimer, &QTimer::timeout, this, &CWebSocketClientComp::OnTimeout);

	m_refreshTimer.setSingleShot(true);
	m_refreshTimer.setInterval(4000);

	if (m_clientIdCompPtr.IsValid()){
		QString clientId = m_clientIdCompPtr->GetText();
		if (clientId.isEmpty()){
			clientId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
			m_clientIdCompPtr->SetText(clientId);
		}
	}

	if (m_connectOnCreateAttrPtr.IsValid() && *m_connectOnCreateAttrPtr){
		EnsureWebSocketConnection();
	}
}


void CWebSocketClientComp::OnComponentDestroyed()
{
	disconnect(&m_webSocket, &QWebSocket::connected, this, &CWebSocketClientComp::OnWebSocketConnected);
	m_refreshTimer.stop();
	m_webSocket.disconnect();

	BaseClass::OnComponentDestroyed();
}


// private slots

void CWebSocketClientComp::OnTimeout()
{
	EnsureWebSocketConnection();
}


void CWebSocketClientComp::OnWebSocketConnected()
{

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
	m_connectionStatusProvider.SetConnectionStatus(imtcom::IConnectionStatusProvider::CS_CONNECTED);
}


void CWebSocketClientComp::OnWebSocketDisconnected()
{
	m_connectionStatusProvider.SetConnectionStatus(imtcom::IConnectionStatusProvider::CS_DISCONNECTED);
	m_startQueries.Reset();
	m_refreshTimer.start();
}


void CWebSocketClientComp::OnWebSocketError(QAbstractSocket::SocketError error)
{
	if (m_lastSocketError != error){
		QString errorText = m_webSocket.errorString();

		SendErrorMessage(0, errorText, "CWebSocketClientComp");

		m_lastSocketError = error;
	}
}


void CWebSocketClientComp::OnWebSocketTextMessageReceived(const QString& message)
{
	if (!message.contains("keep_alive")){
		SendInfoMessage(0, message, "OnWebSocketTextMessageReceived");
	}

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

		emit EmitQueryDataReceived(1);
	}
	else if (	methodType == imtrest::CWebSocketRequest::MT_ERROR ||
				methodType == imtrest::CWebSocketRequest::MT_START_ASK ||
				methodType == imtrest::CWebSocketRequest::MT_DATA){
		responsePtr = m_clientRequestHandlerCompPtr->ProcessRequest(*webSocketRequest);
	}
	else{
		if (methodType == imtrest::CWebSocketRequest::MT_QUERY){
			// webSocketRequest.PopPtr();
			// QJsonDocument document = QJsonDocument::fromJson(webSocketRequest->GetBody());
			// QJsonObject object = document.object();
			// QByteArray body = object.value("payload").toObject().value("data").toString().toUtf8();
			// QJsonObject headers = object.value("headers").toObject();
			// // QString productId = headers.value("ProductId").toString();
			// for (QString& key: headers.keys()){
			// 	webSocketRequest->SetHeader(key.toUtf8(), headers.value(key).toString().toUtf8());
			// }
			// webSocketRequest->SetCommandId("Agent/graphql");
			// webSocketRequest->SetBody(body);
			// webSocketRequest->SetMethodType(imtrest::CWebSocketRequest::MT_QUERY);

			// responsePtr = m_serverRequestHandlerCompPtr->ProcessRequest(*webSocketRequest);

			imtrest::IRequest* requestPtr = m_httpProtocolEngineCompPtr->CreateRequest(*m_serverRequestHandlerCompPtr);
			imtrest::CHttpRequest* newHttpRequestPtr = dynamic_cast<imtrest::CHttpRequest*>(requestPtr);
			if (newHttpRequestPtr != nullptr){
				QByteArray clientId = webSocketRequest->GetClientId();

				QJsonDocument document = QJsonDocument::fromJson(message.toUtf8());
				QJsonObject object = document.object();
				QByteArray body = object.value("payload").toObject().value("data").toString().toUtf8();
				QJsonObject headers = object.value("headers").toObject();
				for (QString& key: headers.keys()){
					newHttpRequestPtr->SetHeader(key.toUtf8(), headers.value(key).toString().toUtf8());
				}
				newHttpRequestPtr->SetBody(body);
				newHttpRequestPtr->SetMethodType(imtrest::CHttpRequest::MT_POST);

				responsePtr = m_serverRequestHandlerCompPtr->ProcessRequest(*newHttpRequestPtr);
			}
		}

	}

	if (responsePtr.IsValid()){
		QByteArray data = responsePtr->GetData();

		webSocketPtr->sendTextMessage(data);

		if (methodType == imtrest::CWebSocketRequest::MT_START){
			m_startQueries.PushBack(webSocketRequest.PopPtr());
		}
	}
}


void CWebSocketClientComp::OnWebSocketBinaryMessageReceived(const QByteArray& /*message*/)
{
}


// private methods

void CWebSocketClientComp::EnsureWebSocketConnection()
{
	QString login;
	if (m_serverLoginAttrPtr.IsValid()){
		login = *m_serverLoginAttrPtr;
	}

	if (m_webSocketServerLoginCompPtr.IsValid()){
		login = m_webSocketServerLoginCompPtr->GetText();
	}

	QString password;
	if (m_serverPasswordAttrPtr.IsValid()){
		password = *m_serverPasswordAttrPtr;
	}

	if (m_webSocketServerPasswordCompPtr.IsValid()){
		password = m_webSocketServerPasswordCompPtr->GetText();
	}

	QString host;
	QString path;
	int port = 0;

	if (m_webSocketServerAddressCompPtr.IsValid()){
		host = m_webSocketServerAddressCompPtr->GetUrl().host();
		port = m_webSocketServerAddressCompPtr->GetUrl().port();
		path = m_webSocketServerAddressCompPtr->GetUrl().path();
	}

	QJsonObject authorization;

	authorization["host"] = host;
	authorization["port"] = QString::number(port);
	authorization["login"] = login;
	authorization["password"] = password;

	QByteArray authHeader = QJsonDocument(authorization).toJson(QJsonDocument::Compact);

	QUrl url;
	url.setHost(host);
	url.setPath(path);
	url.setQuery("header=" + authHeader.toBase64() + "&payload=e30=");
	url.setScheme("ws");
	url.setPort(port);

	m_webSocket.open(url);
}


// public methods of the embedded class NetworkOperation

CWebSocketClientComp::NetworkOperation::NetworkOperation(int timeout, const CWebSocketClientComp* parent)
{
	Q_ASSERT(parent != nullptr);

	timerFlag = false;

	// If the network reply is finished, the internal event loop will be finished:
	QObject::connect(parent, &CWebSocketClientComp::EmitQueryDataReceived, &connectionLoop, &QEventLoop::exit, Qt::DirectConnection);

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


// public methods of embedded class ConnectionStatusProvider

CWebSocketClientComp::ConnectionStatusProvider::ConnectionStatusProvider()
	:m_connectionStatus(imtcom::IConnectionStatusProvider::CS_UNKNOWN)
{
}


void CWebSocketClientComp::ConnectionStatusProvider::SetConnectionStatus(
			imtcom::IConnectionStatusProvider::ConnectionStatus status)
{
	if (m_connectionStatus != status){
		istd::IChangeable::ChangeSet changeSet(status);
		istd::CChangeNotifier notifier(this, &changeSet);

		m_connectionStatus = status;
	}
}


// reimplemented (imtcom::IConnectionStatusProvider)

imtcom::IConnectionStatusProvider::ConnectionStatus CWebSocketClientComp::ConnectionStatusProvider::GetConnectionStatus() const
{
	return m_connectionStatus;
}


} // namespace imtclientgql


