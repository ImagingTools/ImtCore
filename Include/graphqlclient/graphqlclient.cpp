// QtCore includes
#include <imtbase/IUrlParam.h>
#include <imtcom/IConnectionController.h>
#include <imtcom/IConnectionStatusProvider.h>

// graphqlserver includes
// #include <graphqlserver/CGqlRequest.h>
#include <graphqlserver/CExternGraphQl.h>

// graphqlclient includes
#include <graphqlclient/graphqlclient.h>
#include <graphqlclient/CExternSubscriberComp.h>
#include <GeneratedFiles/graphqlclient/Cgraphqlclient.h>

// Qt includes
#include <Qtcore/QJsonObject>



namespace graphqlclient {


Cgraphqlclient& GetInstance()
{
	static Cgraphqlclient instance;

	return instance;
}


void CreateParamsData(const QJsonObject& queryObject, QByteArray& result)
{
	QStringList keys = queryObject.keys();
	for (const QString& key: keys){
		result += key.toUtf8();
		result += ": ";

		if (queryObject.value(key).isObject()){
			result += "{";
			CreateParamsData(queryObject.value(key).toObject(), result);
			result += "}";
		}
		else{
			result += "\"";
			result += queryObject.value(key).toString().toUtf8();
			result += "\"";
		}
	}
}

// GraphQlRequestManager implemented

// public methods

GraphQlRequestManager::GraphQlRequestManager(const ConnectionSettings& connectionSettings)
	:m_connectionSettings(connectionSettings)
{
}


void GraphQlRequestManager::SendRequest(
			const QByteArray& commandId,
			const graphqlserver::RequestType requestType,
			const QJsonObject& inputParams,
			const graphqlserver::ResultKeys& resultKeys,
			QString &errorMessage)
{
	QNetworkRequest request;
	QUrl url = m_connectionSettings.serverUrl;
	url.setPath("/" + m_connectionSettings.productId + "/graphql");
	request.setUrl(url);

	QByteArray data = "{\"query\":\"";
	if (requestType == graphqlserver::RequestType::RT_QUERY){
		data += "query";
	}
	else if (requestType == graphqlserver::RequestType::RT_MUTATION){
		data += "mutation";
	}
	else {
		errorMessage = "Incorrect request type";

		Q_ASSERT_X(false, "GraphQlRequestManager::SendRequest", "GraphQL Client: Invalid request type");

		return;
	}

	data += " " + commandId + " {" + commandId + " ";

	if (!inputParams.isEmpty()){
		QByteArray paramsData;
		CreateParamsData(inputParams, paramsData);
		data += "(";
		data += paramsData;
		data += ")";
	}

	QByteArray fieldsData;
	graphqlserver::CreateResultData(&resultKeys, fieldsData);

	data += fieldsData;
	data += "}\"}";

	QNetworkReply* networkReplyPtr = m_manager.post(request, data);

	QObject::connect(networkReplyPtr, &QIODevice::readyRead, this, &GraphQlRequestManager::OnDataReceived);

	QObject::connect(
				networkReplyPtr,
				&QNetworkReply::errorOccurred,
				this,
				&GraphQlRequestManager::OnNetworkError);

	QObject::connect(
				networkReplyPtr,
				&QNetworkReply::sslErrors,
				this,
				&GraphQlRequestManager::OnSslErrors);
}


void GraphQlRequestManager::OnDataReceived()
{
	QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
	if (reply != nullptr){
		QByteArray data = reply->readAll();

		OnResponse(data);
	}
}


void GraphQlRequestManager::OnNetworkError(QNetworkReply::NetworkError error)
{
	qDebug() << "[GraphQL-Client] Network error: " << error;
}


void GraphQlRequestManager::OnSslErrors(const QList<QSslError> &errors)
{
	for (const QSslError& sslError: errors){
		qDebug() << "[GraphQL-Client] SSL-error: " << sslError.errorString();
	}
}


bool ConnectToServer(const ConnectionSettings& connectionSettings, QString& errorMessage)
{
	Cgraphqlclient& instance = GetInstance();

	imtbase::IUrlParam* webSocketPortUrlParamPtr = instance.GetInterface<imtbase::IUrlParam>("WebSocketServerPort");
	if (webSocketPortUrlParamPtr != nullptr){
		bool retVal = webSocketPortUrlParamPtr->SetUrl(connectionSettings.webSocketServerUrl);
		if (!retVal){
			errorMessage = QString("Server URL could not be set");

			return false;
		}
	}
	else{
		errorMessage = QString("Internal error: wrong component configuration");

		Q_ASSERT(false);

		return false;
	}

	imtcom::IConnectionController* connectionControllerPtr = instance.GetInterface<imtcom::IConnectionController>("WebSocketClient");
	if (connectionControllerPtr != nullptr){
		bool retVal = connectionControllerPtr->Connect();
		if (!retVal) {
			errorMessage = QString("Server could not be connected. Server-URL: '%1'").arg(webSocketPortUrlParamPtr->GetUrl().toString());

			return false;
		}
	}

	return true;
}


bool DisconnectFromServer(QString& errorMessage)
{
	bool retVal = false;

	Cgraphqlclient& instance = GetInstance();

	imtcom::IConnectionController* connectionControllerPtr = instance.GetInterface<imtcom::IConnectionController>("WebSocketClient");
	if (connectionControllerPtr != nullptr){
		connectionControllerPtr->Disconnect();

		retVal = true;
	}
	else {
		errorMessage = QString("Internal error: wrong component configuration");

		Q_ASSERT(false);

		return false;
	}

	return retVal;
}


graphqlclient::ClientStatus GetConnectionStatus()
{
	ClientStatus retVal = CS_UNKNOWN;

	Cgraphqlclient& instance = GetInstance();

	imtcom::IConnectionStatusProvider* connectionStatusProviderPtr = instance.GetInterface<imtcom::IConnectionStatusProvider>("ConnectionStatusProvider");
	if (connectionStatusProviderPtr != nullptr){
		imtcom::IConnectionStatusProvider::ConnectionStatus connectionStatus = connectionStatusProviderPtr->GetConnectionStatus();
		if (connectionStatus == imtcom::IConnectionStatusProvider::CS_CONNECTED){
			retVal = CS_CONNECTED;
		}
		else if (connectionStatus == imtcom::IConnectionStatusProvider::CS_DISCONNECTED){
			retVal = CS_NOT_CONNECTED;
		}
		else if (connectionStatus == imtcom::IConnectionStatusProvider::CS_UNKNOWN) {
			retVal = CS_UNKNOWN;
		}
	}

	return retVal;
}


QByteArray RegisterSubscription(
			const QByteArray& commantId,
			const graphqlserver::ResultKeys& resultKeys,
			ISubscriber& subscriber,
			QString& errorMessage)
{
	QByteArray fieldData;
	graphqlserver::CreateResultData(&resultKeys, fieldData);

	QByteArray queryData = "{\"query\": \"subscription " + commantId + " {" + commantId + " " + fieldData + "}\"}";

	Cgraphqlclient& instance = GetInstance();
	imtclientgql::IGqlSubscriptionClient* gqlSubscriptionClientPtr = instance.GetInterface<imtclientgql::IGqlSubscriptionClient>();
	graphqlclient::CExternSubscriberComp* externSubscriberPtr = dynamic_cast<graphqlclient::CExternSubscriberComp*>(gqlSubscriptionClientPtr);
	if (externSubscriberPtr != nullptr){
		return externSubscriberPtr->RegisterSubscribtion(queryData, subscriber, errorMessage);
	}
	else{
		errorMessage = "Internal error";
	}

	return QByteArray();
}



bool UnregisterSubscription(const QByteArray& subscriptionId, QString& errorMessage)
{
	Cgraphqlclient& instance = GetInstance();
	imtclientgql::IGqlSubscriptionClient* gqlSubscriptionClientPtr = instance.GetInterface<imtclientgql::IGqlSubscriptionClient>();
	graphqlclient::CExternSubscriberComp* externSubscriberPtr = dynamic_cast<graphqlclient::CExternSubscriberComp*>(gqlSubscriptionClientPtr);
	if (externSubscriberPtr != nullptr) {
		return externSubscriberPtr->UnregisterSubscription(subscriptionId, errorMessage);
	}
	else{
		errorMessage = "Internal error";

		Q_ASSERT(false);
	}

	return false;
}


} //namespace graphqlclient


