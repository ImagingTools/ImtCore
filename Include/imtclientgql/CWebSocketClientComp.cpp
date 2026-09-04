// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CWebSocketClientComp.h>


// Qt includes
#include <QtCore/QMessageAuthenticationCode>
#include <QtCore/QUrl>
#include <QtCore/QFile>

#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
#include <QtWebSockets/QWebSocketHandshakeOptions>
#endif

// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/IEnableableParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtgql/CGqlResponse.h>
#include <imtgql/CGqlRequest.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtrest/CHttpRequest.h>
#include <imtrest/CHttpResponse.h>



namespace imtclientgql
{


// public methods

CWebSocketClientComp::CWebSocketClientComp()
{
	m_lastSocketError = QAbstractSocket::SocketError::UnknownSocketError;
}


// reimplemented (imtclientgql::IGqlClient)

QByteArray CWebSocketClientComp::BuildRequestEnvelope(const GqlRequestPtr& requestPtr, QString& key) const
{
	key = QUuid::createUuid().toString(QUuid::WithoutBraces);

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

		const QByteArray languageId = contextPtr->GetLanguageId();
		if (!languageId.isEmpty()){
			headersObject[QString::fromUtf8(imtbase::s_languageIdHeaderId)] = QString::fromUtf8(languageId);
		}
	}
	dataObject["headers"] = headersObject;

	QString clientId;
	if (m_clientIdCompPtr.IsValid()){
		clientId = m_clientIdCompPtr->GetText();
	}
	if (!clientId.isEmpty()){
		dataObject["clientid"] = clientId;
	}

	return QJsonDocument(dataObject).toJson(QJsonDocument::Compact);
}


IGqlClient::GqlResponsePtr CWebSocketClientComp::SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam* /*urlParamPtr*/) const
{
	QString key;
	QByteArray data = BuildRequestEnvelope(requestPtr, key);

	m_webSocket.sendTextMessage(data);
	NetworkOperation networkOperation(100, this);

	GqlResponsePtr retVal;

	// 300 * 100ms = 30s. The server may legitimately take a few seconds to answer a
	// request that arrives while it is mid-processing another exchange on the same
	// connection (e.g. a deferred server->client sync push queued right after this
	// client's own request) — 10s used to be tight enough to time out under that kind
	// of transient, non-error latency.
	for (int i = 0; i < 300; i++){
		networkOperation.timer.start();
		networkOperation.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);
		QCoreApplication::processEvents();
		QWriteLocker writeLock(&m_queryDataMapLock);
		if (m_queryDataMap.contains(key)){
			imtgql::CGqlResponse* responsePtr = new imtgql::CGqlResponse(requestPtr);
			responsePtr->SetResponseData(m_queryDataMap.value(key));
			retVal.SetPtr(responsePtr);

			m_queryDataMap.remove(key);

			return retVal;
		}
	}

	return retVal;
}


bool CWebSocketClientComp::SendRequestNoWait(GqlRequestPtr requestPtr, imtbase::IUrlParam* /*urlParamPtr*/) const
{
	QString key;
	const QByteArray data = BuildRequestEnvelope(requestPtr, key);

	// No NetworkOperation / wait loop: the caller does not need a response, only that the
	// message was handed to the socket - avoids blocking this component's thread for up to
	// SendRequest()'s 30s budget for requests whose result is never actually consumed.
	m_webSocket.sendTextMessage(data);

	return true;
}


// reimplemented (imtrest::ITransport)

bool CWebSocketClientComp::SendResponse(imtrest::ConstResponsePtr& response) const
{
	QByteArray data = response->GetData();
	imtrest::CHttpResponse* httpResponsePtr = dynamic_cast<imtrest::CHttpResponse*>(const_cast<imtrest::IResponse*>(response.GetPtr()));
	if (httpResponsePtr != nullptr && !data.isEmpty()){
		QByteArray body = data;
		imtrest::IResponse::Headers headers = response->GetHeaders();
		data = QStringLiteral(R"({"type": "query_data","id": "%1","payload": %2})")
				   .arg(headers.value("id"), body).toUtf8();
	}

	EmitSendTextMessage(data);

	return true;
}


bool CWebSocketClientComp::SendRequest(imtrest::ConstRequestPtr& request) const
{
	QByteArray message = request->GetBody();
	EmitSendTextMessage(message);

	return true;
}


// reimplemented (imtrest::IResponseDispatcher)

bool CWebSocketClientComp::SendResponse(const QByteArray& /*requestId*/, imtrest::ConstResponsePtr& response) const
{
	return SendResponse(response);
}


bool CWebSocketClientComp::SendRequest(const QByteArray& /*requestId*/, imtrest::ConstRequestPtr& request) const
{
	return SendRequest(request);
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


// reimplemented (imod::CMultiModelDispatcherBase)

void CWebSocketClientComp::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet)
{
	Q_ASSERT_X(m_sslConfigurationCompPtr.IsValid() && m_sslConfigurationManagerCompPtr.IsValid(), "Update server's SSL configuration", "SSL configuration or manager is not set!");

	if (m_connectOnCreateAttrPtr.IsValid() && *m_connectOnCreateAttrPtr && m_isInitialized){
		EnsureWebSocketConnection();
	}
}


// reimplemented (ibase::TRuntimeStatusHanderCompWrap)

void CWebSocketClientComp::OnSystemShutdown()
{
	disconnect(&m_webSocket, &QWebSocket::connected, this, &CWebSocketClientComp::OnWebSocketConnected);

	emit EmitStopTimer();

	m_webSocket.disconnect();

	emit EmitWebSocketClose();

	BaseClass2::UnregisterAllModels();
}


// reimplemented (icomp::CComponentBase)

void CWebSocketClientComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	connect(&m_webSocket, &QWebSocket::connected, this, &CWebSocketClientComp::OnWebSocketConnected, Qt::QueuedConnection);
	connect(&m_webSocket, &QWebSocket::disconnected, this, &CWebSocketClientComp::OnWebSocketDisconnected, Qt::QueuedConnection);
	connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &CWebSocketClientComp::OnWebSocketTextMessageReceived);
	connect(&m_webSocket, &QWebSocket::binaryMessageReceived, this, &CWebSocketClientComp::OnWebSocketBinaryMessageReceived);
	connect(this, &CWebSocketClientComp::EmitSendTextMessage, this, &CWebSocketClientComp::OnSendTextMessage);
	connect(this, &CWebSocketClientComp::EmitWebSocketClose, &m_webSocket, &QWebSocket::close);
	connect(this, SIGNAL(EmitStartTimer()), &m_refreshTimer, SLOT(start()), Qt::QueuedConnection);
	connect(this, SIGNAL(EmitStopTimer()), &m_refreshTimer, SLOT(stop()), Qt::QueuedConnection);
	connect(&m_webSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnWebSocketError(QAbstractSocket::SocketError)));
	connect(&m_webSocket, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(OnSslErrors(const QList<QSslError>&)));

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

	if (m_sslConfigurationModelCompPtr.IsValid() && m_sslConfigurationManagerCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_sslConfigurationModelCompPtr.GetPtr());
	}

	if (m_connectOnCreateAttrPtr.IsValid() && *m_connectOnCreateAttrPtr){
		EnsureWebSocketConnection();
	}

	m_isInitialized = true;
}


void CWebSocketClientComp::OnComponentDestroyed()
{
	// Fallback (will not work in someb situations)
	if (!m_runtimeStatusCompPtr.IsValid()){
		OnSystemShutdown();
	}

	BaseClass::OnComponentDestroyed();
}


// private slots

void CWebSocketClientComp::OnTimeout()
{
	EnsureWebSocketConnection();
}


void CWebSocketClientComp::OnWebSocketConnected()
{
	SendInfoMessage(0, "WebSocket connected", "CWebSocketClientComp");

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
		body = body.arg(", \"clientid\": \"" + clientId + "\"");
	}

	m_webSocket.sendTextMessage(body);
	m_connectionStatusProvider.SetConnectionStatus(imtcom::IConnectionStatusProvider::CS_CONNECTED);
}


void CWebSocketClientComp::OnWebSocketDisconnected()
{
	SendInfoMessage(0, "WebSocket disconnected", "CWebSocketClientComp");

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


void CWebSocketClientComp::OnSslErrors(const QList<QSslError>& sslErrors)
{
	for (const QSslError& error : sslErrors){
		SendErrorMessage(0, QStringLiteral("SSL client error: %1").arg(error.errorString()));
	}

	m_webSocket.ignoreSslErrors(sslErrors);
}


void CWebSocketClientComp::OnWebSocketTextMessageReceived(const QString& message)
{
	if (!message.contains("keep_alive")){
		SendVerboseMessage(message, "OnWebSocketTextMessageReceived");
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
			methodType == imtrest::CWebSocketRequest::MT_CONNECTION_ACK ||
			methodType == imtrest::CWebSocketRequest::MT_KEEP_ALIVE ||
			methodType == imtrest::CWebSocketRequest::MT_KEEP_ALIVE_ACK
			){
	}
	else if (methodType == imtrest::CWebSocketRequest::MT_QUERY_DATA){
		// Envelope is {"type":"query_data","id":"...","payload":{...GQL JSON...}}.
		// Callers of SendRequest expect the GraphQL body only.
		QByteArray gqlBody = webSocketRequest->GetBody();
		{
			const QJsonDocument envelope = QJsonDocument::fromJson(gqlBody);
			if (envelope.isObject()){
				const QJsonValue payloadVal = envelope.object().value(QStringLiteral("payload"));
				if (payloadVal.isObject()){
					gqlBody = QJsonDocument(payloadVal.toObject()).toJson(QJsonDocument::Compact);
				}
				else if (payloadVal.isString()){
					gqlBody = payloadVal.toString().toUtf8();
				}
			}
		}
		QWriteLocker writeLock(&m_queryDataMapLock);
		m_queryDataMap.insert(webSocketRequest->GetRequestId(), gqlBody);
		writeLock.unlock();

		emit EmitQueryDataReceived(1);
	}
	else if (	methodType == imtrest::CWebSocketRequest::MT_ERROR ||
				methodType == imtrest::CWebSocketRequest::MT_START_ACK ||
				methodType == imtrest::CWebSocketRequest::MT_START ||
				methodType == imtrest::CWebSocketRequest::MT_DATA){
		responsePtr = m_clientRequestHandlerCompPtr->ProcessRequest(*webSocketRequest);
		if (methodType == imtrest::CWebSocketRequest::MT_START){
			m_startQueries.PushBack(webSocketRequest.PopPtr());
		}
	}
	else{
		if (methodType == imtrest::CWebSocketRequest::MT_QUERY && m_httpProtocolEngineCompPtr.IsValid() && m_serverRequestHandlerCompPtr.IsValid()){
			imtrest::IRequestUniquePtr requestPtr = m_httpProtocolEngineCompPtr->CreateRequest(*m_serverRequestHandlerCompPtr);
			imtrest::CHttpRequest* newHttpRequestPtr = dynamic_cast<imtrest::CHttpRequest*>(requestPtr.GetPtr());
			if (newHttpRequestPtr != nullptr){
				QByteArray clientId = webSocketRequest->GetClientId();

				QJsonDocument document = QJsonDocument::fromJson(message.toUtf8());
				QJsonObject object = document.object();
				QByteArray body = object.value("payload").toObject().value("data").toString().toUtf8();
				QJsonObject headers = object.value("headers").toObject();
				for (QString& key: headers.keys()){
					newHttpRequestPtr->SetHeader(key.toUtf8().toLower(), headers.value(key).toString().toUtf8());
				}
				newHttpRequestPtr->SetHeader("id", webSocketRequest->GetRequestId());
				newHttpRequestPtr->SetBody(body);
				newHttpRequestPtr->SetMethodType(imtrest::CHttpRequest::MT_POST);
				newHttpRequestPtr->SetCommandId("/" + *m_productId + "/graphql");

				responsePtr = m_serverRequestHandlerCompPtr->ProcessRequest(*requestPtr.PopInterfacePtr());
			}
		}
	}

	if (responsePtr.IsValid()){
		QByteArray data = responsePtr->GetData();
		// Server CSubscriptionManagerComp sends type=query with a correlation id and waits
		// for type=query_data. Reply with the same envelope (same as SendResponse path).
		if (methodType == imtrest::CWebSocketRequest::MT_QUERY){
			const QByteArray queryRequestId = webSocketRequest->GetRequestId();
			if (!queryRequestId.isEmpty()){
				const QByteArray payload = data.isEmpty() ? QByteArrayLiteral("{}") : data;
				data = QByteArrayLiteral("{\"type\":\"query_data\",\"id\":\"")
							+ queryRequestId
							+ QByteArrayLiteral("\",\"payload\":")
							+ payload
							+ QByteArrayLiteral("}");
			}
		}
		webSocketPtr->sendTextMessage(data);
	}
}


void CWebSocketClientComp::OnWebSocketBinaryMessageReceived(const QByteArray& /*message*/)
{
}


void CWebSocketClientComp::OnSendTextMessage(const QByteArray& message)
{
	m_webSocket.sendTextMessage(message);
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

	if (m_serverConnectionCompPtr.IsValid()){
		QUrl url;
		if (m_serverConnectionCompPtr->GetUrl(imtcom::IServerConnectionInterface::PT_WEBSOCKET, url)){
			host = url.host();
			port = url.port();
			path = url.path();
		}
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

	if (m_sslConfigurationCompPtr.IsValid() && m_sslConfigurationManagerCompPtr.IsValid()){
		QSslConfiguration sslConfiguration;
		iprm::TParamsPtr<iprm::IEnableableParam> sslEnableParamPtr(
					m_sslConfigurationCompPtr.GetPtr(),
					imtcom::ISslConfigurationManager::ParamKeys::s_enableSslModeParamKey);
		if (sslEnableParamPtr.IsValid() && sslEnableParamPtr->IsEnabled()){
			if (m_sslConfigurationManagerCompPtr->CreateSslConfiguration(*m_sslConfigurationCompPtr, sslConfiguration)){
				url.setScheme("wss");
				m_webSocket.setSslConfiguration(sslConfiguration);
			}
		}
		else{
			m_webSocket.setSslConfiguration(QSslConfiguration());
		}
	}

	SendInfoMessage(0, QStringLiteral("Try connect to the WebSocket-server: Host: %1; Port: %2, Protocol: %3").arg(url.host(), QString::number(url.port()), url.scheme()));

#if QT_VERSION >= QT_VERSION_CHECK(6,4,0)
	QWebSocketHandshakeOptions handshakeOptions;
	if (m_subprotocolListCompPtr.IsValid()){
		QStringList protocols;

		int count = m_subprotocolListCompPtr->GetOptionsCount();
		for (int i = 0; i < count; i++){
			protocols << m_subprotocolListCompPtr->GetOptionId(i);
		}

		handshakeOptions.setSubprotocols(protocols);
	}
	m_webSocket.open(url, handshakeOptions);
#else
	m_webSocket.open(url);
#endif
}


// public methods of the embedded class NetworkOperation

CWebSocketClientComp::NetworkOperation::NetworkOperation(int timeout, const CWebSocketClientComp* parent)
{
	Q_ASSERT(parent != nullptr);

	timerFlag = false;

	// If the network reply is finished, the internal event loop will be finished:
	connect(parent, &CWebSocketClientComp::EmitQueryDataReceived, &connectionLoop, &QEventLoop::exit);

	// If the application will be finished, the internal event loop will be also finished:
	connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, &connectionLoop, &QEventLoop::quit);

	// If a timeout for the request was defined, start the timer:
	if (timeout > 0){
		timer.setSingleShot(true);

		// If the timer is running out, the internal event loop will be finished:
		connect(&timer, &QTimer::timeout, &connectionLoop, &QEventLoop::quit);

		timer.start(timeout);
	}
}


CWebSocketClientComp::NetworkOperation::~NetworkOperation()
{
	timer.stop();
}


// public methods of embedded class ConnectionStatusProvider

CWebSocketClientComp::ConnectionStatusProvider::ConnectionStatusProvider()
	:m_connectionStatus(CS_UNKNOWN)
{
}


void CWebSocketClientComp::ConnectionStatusProvider::SetConnectionStatus(ConnectionStatus status)
{
	if (m_connectionStatus != status){
		istd::IChangeable::ChangeSet changeSet(status == CS_CONNECTED ? CF_CONNECTED : CF_DISCONNECTED);
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
