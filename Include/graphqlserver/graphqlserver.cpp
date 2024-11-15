
// ImtCore includes
#include <imtbase/Init.h>
#include <imtbase/IUrlParam.h>
#include <imtgql/IGqlRequestHandler.h>
#include <imtgql/IGqlSubscriberController.h>
#include <imtrest/IServer.h>
#include <graphqlserver/graphqlserver.h>

// GraphQlServer includes
#include <graphqlserver/CExternGraphQlControllerComp.h>
#include <graphqlserver/CExternGraphQlPublisherComp.h>
#include <GeneratedFiles/graphqlserver/Cgraphqlserver.h>


namespace graphqlserver {



Cgraphqlserver& GetInstance()
{
	static Cgraphqlserver instance;

	return instance;
}

void CreateResultData(const ResultKeys* resultKeys, QByteArray& resultData)
{
	if (resultKeys == nullptr){
		return;
	}

	resultData += "{";
	QByteArrayList keys = resultKeys->GetKeys();
	for (int index = 0; index < keys.count(); index++){
		if (index > 0){
			resultData += " ";
		}
		const QByteArray key = keys[index];
		resultData += key;
		if (resultKeys->IsChild(key)){
			ResultKeys* resultKeysNoConst = const_cast<ResultKeys*>(resultKeys);
			if (resultKeysNoConst != nullptr){
				CreateResultData(resultKeysNoConst->GetChild(key), resultData);
			}
		}
	}
	resultData += "}";
}


bool StartServer(const graphqlserver::ServerSettings& serverSettings, QString& errorMessage)
{
	bool retVal = false;

	Cgraphqlserver& instance = GetInstance();
	imtbase::IUrlParam* serverPortUrlParamPtr = instance.GetInterface<imtbase::IUrlParam>("ServerPort");
	if (serverPortUrlParamPtr != nullptr){
		QUrl url = serverPortUrlParamPtr->GetUrl();
		url.setPort(serverSettings.tcpPort);
		serverPortUrlParamPtr->SetUrl(url);

		retVal = true;
	}
	else{
		retVal = false;
	}

	imtbase::IUrlParam* webSocketPortUrlParamPtr = instance.GetInterface<imtbase::IUrlParam>("WebSocketServerPort");
	if (webSocketPortUrlParamPtr != nullptr){
		QUrl url = webSocketPortUrlParamPtr->GetUrl();
		url.setPort(serverSettings.webSocketPort);
		webSocketPortUrlParamPtr->SetUrl(url);

		retVal = true;
	}
	else{
		retVal = false;
	}

	if (!retVal){
		return false;
	}

	imtrest::IServer* serverPtr = instance.GetInterface<imtrest::IServer>("HttpServerFramework");
	if (serverPtr != nullptr){
		serverPtr->StartServer();
		retVal = true;
	}
	else{
		retVal = false;
	}

	serverPtr = instance.GetInterface<imtrest::IServer>("WebSocketServerFramework");
	if (serverPtr != nullptr){
		serverPtr->StartServer();
		retVal = true;
	}
	else{
		retVal = false;
	}

	return retVal;
}


bool StopServer(QString& errorMessage)
{
	Cgraphqlserver& instance = GetInstance();
	imtrest::IServer* serverPtr = instance.GetInterface<imtrest::IServer>();
	if (serverPtr != nullptr){
		return serverPtr->StopServer();
	}

	return false;
}


graphqlserver::ServerStatus GetServerStatus()
{
	Cgraphqlserver& instance = GetInstance();
	imtrest::IServer* serverPtr = instance.GetInterface<imtrest::IServer>();
	if (serverPtr != nullptr){
		imtrest::IServer::ServerStatus status = serverPtr->GetServerStatus();
		if (status == imtrest::IServer::SS_LISTENING){
			return graphqlserver::SS_LISTENING;
		}
	}

	return graphqlserver::SS_NOT_STARTED;
}


bool RegisterGqlHandler(const QByteArray& commandId, graphqlserver::IGqlRequestHandler& gqlRequestHandler, QString& errorMessage)
{
	Cgraphqlserver& instance = GetInstance();
	imtgql::IGqlRequestHandler* gqlRequestHandlerPtr = instance.GetInterface<imtgql::IGqlRequestHandler>();
	graphqlserver::CExternGraphQlControllerComp* externGraphQlControllerPtr = dynamic_cast<graphqlserver::CExternGraphQlControllerComp*>(gqlRequestHandlerPtr);
	if (externGraphQlControllerPtr != nullptr){
		return externGraphQlControllerPtr->RegisterGqlHandler(commandId, gqlRequestHandler, errorMessage);
	}


	return false;
}


bool RegisterGqlPublisher(const QByteArray& commandId, IGqlPublisher& gqlPublisher, QString& errorMessage)
{
	Cgraphqlserver& instance = GetInstance();
	imtgql::IGqlSubscriberController* gqlPublisherPtr = instance.GetInterface<imtgql::IGqlSubscriberController>();
	graphqlserver::CExternGraphQlPublisherComp* externGraphQlPublisherPtr = dynamic_cast<graphqlserver::CExternGraphQlPublisherComp*>(gqlPublisherPtr);
	if (externGraphQlPublisherPtr != nullptr){
		return externGraphQlPublisherPtr->RegisterGqlPublisher(commandId, gqlPublisher, errorMessage);
	}

	return false;
}


bool PublishData(const QByteArray& subscriptionId, const QJsonObject& subscriptionData)
{
	Cgraphqlserver& instance = GetInstance();
	imtgql::IGqlSubscriberController* gqlPublisherPtr = instance.GetInterface<imtgql::IGqlSubscriberController>();
	graphqlserver::CExternGraphQlPublisherComp* externGraphQlPublisherPtr = dynamic_cast<graphqlserver::CExternGraphQlPublisherComp*>(gqlPublisherPtr);
	if (externGraphQlPublisherPtr != nullptr){
		return externGraphQlPublisherPtr->PublishData(subscriptionId, subscriptionData);
	}

	return false;
}


}  // namespace graphqlserver

