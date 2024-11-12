
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


bool InitServer(const graphqlserver::ServerSettings& serverSettings, QString& errorMessage)
{
	Cgraphqlserver& instance = GetInstance();
	imtbase::IUrlParam* serverPortUrlParamPtr = instance.GetInterface<imtbase::IUrlParam>("ServerPort");
	if (serverPortUrlParamPtr != nullptr){
		QUrl url = serverPortUrlParamPtr->GetUrl();
		url.setPort(serverSettings.tcpPort);
		serverPortUrlParamPtr->SetUrl(url);

		return true;
	}

	return false;
}


bool StartServer(QString& errorMessage)
{
	Cgraphqlserver& instance = GetInstance();
	imtrest::IServer* serverPtr = instance.GetInterface<imtrest::IServer>();
	if (serverPtr != nullptr){
		return serverPtr->StartServer();
	}

	return false;
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
		if (status == imtrest::IServer::SS_LISTEN){
			return graphqlserver::SS_LISTEN;
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


bool SendSubscription(const QByteArray& subscriptionId, const QJsonObject& subscriptionData)
{
	Cgraphqlserver& instance = GetInstance();
	imtgql::IGqlSubscriberController* gqlPublisherPtr = instance.GetInterface<imtgql::IGqlSubscriberController>();
	graphqlserver::CExternGraphQlPublisherComp* externGraphQlPublisherPtr = dynamic_cast<graphqlserver::CExternGraphQlPublisherComp*>(gqlPublisherPtr);
	if (externGraphQlPublisherPtr != nullptr){
		return externGraphQlPublisherPtr->SendSubscription(subscriptionId, subscriptionData);
	}

	return false;
}


}  // namespace graphqlserver

