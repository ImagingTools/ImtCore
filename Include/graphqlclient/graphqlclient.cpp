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

GraphQlRequestManager::GraphQlRequestManager(const ConnectionSettings& connectionSettings):
	m_connectionSettings(connectionSettings)
{

}


void GraphQlRequestManager::SendRequest(const QByteArray& commandId,
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
		errorMessage = "Incorrected request type";

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

	QNetworkReply *reply = m_manager.post(request, data);
	QObject::connect(reply, &QIODevice::readyRead, this, &GraphQlRequestManager::OnDataReceived);
	QObject::connect(reply, &QNetworkReply::errorOccurred,
					 this, &GraphQlRequestManager::OnNetworkError);
	QObject::connect(reply, &QNetworkReply::sslErrors,
					 this, &GraphQlRequestManager::OnSslErrors);
}


void GraphQlRequestManager::OnDataReceived()
{
	QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
	if (reply != nullptr){
		QByteArray data = reply->readAll();
		qDebug() << "Reply data: " << data;
		OnResponse(data);
	}
}


void GraphQlRequestManager::OnNetworkError(QNetworkReply::NetworkError error)
{
		qDebug() << "Network error: " << error;
}


void GraphQlRequestManager::OnSslErrors(const QList<QSslError> &errors)
{
	for (const QSslError& sslError: errors){
		qDebug() << "Ssl error: " << sslError.errorString();
	}
}


bool ConnectToServer(const ConnectionSettings& connectionSettings, QString& errorMessage)
{
	bool retVal = false;

	Cgraphqlclient& instance = GetInstance();

	imtbase::IUrlParam* webSocketPortUrlParamPtr = instance.GetInterface<imtbase::IUrlParam>("WebSocketServerPort");
	if (webSocketPortUrlParamPtr != nullptr){
		webSocketPortUrlParamPtr->SetUrl(connectionSettings.webSocketServerUrl);

		retVal = true;
	}
	else{
		retVal = false;
	}

	imtcom::IConnectionController* connectionControllerPtr = instance.GetInterface<imtcom::IConnectionController>("WebSocketClient");
	if (connectionControllerPtr != nullptr){
		connectionControllerPtr->Connect();

		retVal = true;
	}

	return retVal;
}


bool DisconnectClient(QString& errorMessage)
{
	bool retVal = false;

	Cgraphqlclient& instance = GetInstance();

	imtcom::IConnectionController* connectionControllerPtr = instance.GetInterface<imtcom::IConnectionController>("WebSocketClient");
	if (connectionControllerPtr != nullptr){
		connectionControllerPtr->Disconnect();

		retVal = true;
	}

	return retVal;
}


graphqlclient::ClientStatus GetConnectionStatus()
{
	ClientStatus retVal = CS_NOT_CONNECTED;

	Cgraphqlclient& instance = GetInstance();

	imtcom::IConnectionStatusProvider* connectionStatusProviderPtr = instance.GetInterface<imtcom::IConnectionStatusProvider>("ConnectionStatusProvider");
	if (connectionStatusProviderPtr != nullptr){
		imtcom::IConnectionStatusProvider::ConnectionStatus connectionStatus = connectionStatusProviderPtr->GetConnectionStatus();
		if (connectionStatus == imtcom::IConnectionStatusProvider::CS_CONNECTED){
			retVal = CS_CONNECTED;
		}
	}

	return retVal;
}


QByteArray RegisterSubscription(const QByteArray& commantId, const graphqlserver::ResultKeys& resultKeys, ISubscriber& subscriber, QString& errorMessage)
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
	return false;
}


} //namespace graphqlclient
