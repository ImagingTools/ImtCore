#include <imtgql/CWebSocketServletComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/ISender.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CWebSocketRequest.h>


namespace imtgql
{


// public methods

// reimplemented (IRequestHandler)

bool CWebSocketServletComp::IsCommandSupported(const QByteArray& /*commandId*/) const
{
	return true;
}


imtrest::ConstResponsePtr CWebSocketServletComp::ProcessRequest(const imtrest::IRequest& request, const QByteArray& subCommandId) const
{
	QByteArray commandId = subCommandId.isEmpty() ? request.GetCommandId() : subCommandId;

	const imtrest::CWebSocketRequest* webSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&request);

	if (webSocketRequest != nullptr){
		switch (webSocketRequest->GetMethodType())
		{
		case imtrest::CWebSocketRequest::MT_CONNECTION_INIT:
			return InitConnection(request);
			break;
		
		case imtrest::CWebSocketRequest::MT_START:
			return RegisterSubscription(request);
			break;

		case imtrest::CWebSocketRequest::MT_STOP:
			return UnregisterSubscription(request);
			break;

		case imtrest::CWebSocketRequest::MT_QUERY:
			return ProcessGqlRequest(request);
			break;

		case imtrest::CWebSocketRequest::MT_ERROR:
			SendErrorMessage(0, "Error request");
			return imtrest::ConstResponsePtr();
			break;

		default:{
			QByteArray errorMessage = QString("Method type not correct: %1").arg(webSocketRequest->GetMethodType()).toUtf8();

			return CreateErrorResponse(errorMessage, request);
			}
			break;
		}
	}
	else{
		QByteArray errorMessage = "Error request";

		return CreateErrorResponse(errorMessage, request);
	}

	return imtrest::ConstResponsePtr();
}


imtrest::ConstResponsePtr CWebSocketServletComp::InitConnection(const imtrest::IRequest& request) const
{
	const imtrest::CWebSocketRequest* webSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&request);

	if (webSocketRequest != nullptr){
	QByteArray data = QString(R""(
{
"type": "connection_ask",
"payload": {
	"connectionTimeoutMs": 300000
}
}
		)"" ).toUtf8();
	
		return CreateDataResponse(data, request);
	}

	return imtrest::ConstResponsePtr();	
}


imtrest::ConstResponsePtr CWebSocketServletComp::ProcessGqlRequest(const imtrest::IRequest& request) const
{
	imtgql::CGqlRequest gqlRequest;

	QByteArray body = request.GetBody();
	QJsonDocument document = QJsonDocument::fromJson(body);
	QJsonObject object = document.object();

	body = object.value("payload").toObject().value("data").toString().toUtf8();

	int errorPosition;
	gqlRequest.ParseQuery(body, errorPosition);

	QByteArray commandId = gqlRequest.GetCommandId();
	if (commandId.isEmpty()){
		QByteArray errorMessage = "Empty command-ID";

		return CreateErrorResponse(errorMessage, request);
	}

	if (m_gqlRequestHandlerCompPtr.IsValid()){
		QString errorMessage;
		imtbase::CTreeItemModel* responseDataModel = m_gqlRequestHandlerCompPtr->CreateResponse(gqlRequest, errorMessage);
		if (responseDataModel == nullptr){
			if (!errorMessage.isEmpty()){
				return CreateErrorResponse(errorMessage.toUtf8(), request);
			}

			return imtrest::ConstResponsePtr();
		}

		QString responseData = responseDataModel->ToJson();

		QByteArray data = QString(R""(
{
"type": "query_data",
"id": "%1",
"payload": %2
}
			)"" ).arg(object.value("id").toString()).arg(responseData).toUtf8();

			return CreateDataResponse(data, request);
	}
	else{
		QByteArray errorMessage = QString("The requested command could not be executed. No servlet was found for the given command: '%1")
					.arg(QString(commandId)).toUtf8();
		return CreateErrorResponse(errorMessage, request);
	}

	return imtrest::ConstResponsePtr();
}


imtrest::ConstResponsePtr CWebSocketServletComp::RegisterSubscription(const imtrest::IRequest& request) const
{
	const imtrest::CWebSocketRequest* webSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&request);
	imtgql::CGqlRequest gqlRequest;
	QByteArray body = request.GetBody();
	QJsonDocument document = QJsonDocument::fromJson(body);
	QJsonObject object = document.object();

	body = object.value("payload").toObject().value("data").toString().toUtf8();

	int errorPosition;
	gqlRequest.ParseQuery(body, errorPosition);

	QByteArray commandId = gqlRequest.GetCommandId();

	if (commandId.isEmpty()){
		QByteArray errorMessage = "Empty command-ID";
	
		return CreateErrorResponse(errorMessage, request);
	}

	IGqlSubscriberController* subscriberControllerPtr = nullptr;
	for (int index = 0; index < m_gqlSubscriberControllersCompPtr.GetCount(); index++){
		IGqlSubscriberController* gqlSubscriberControllerPtr = m_gqlSubscriberControllersCompPtr[index];
		if (gqlSubscriberControllerPtr != nullptr){
			if (gqlSubscriberControllerPtr->IsRequestSupported(gqlRequest)){
				subscriberControllerPtr = m_gqlSubscriberControllersCompPtr[index];

				break;
			}
		}
	}

	if (subscriberControllerPtr != nullptr){
		QString errorMessage;
		if (subscriberControllerPtr->RegisterSubscription(webSocketRequest->GetSubscriptionId(), gqlRequest, request, errorMessage)){
			QByteArray data = QString(R""(
{
"type": "start_ask",
"id": "%1"
}
				)"" ).arg(QString(webSocketRequest->GetSubscriptionId())).toUtf8();
			
				return CreateDataResponse(data, request);
			}		
	}
	else{
		QByteArray errorMessage = QString("The requested command could not be executed. No servlet was found for the given command: '%1")
					.arg(QString(commandId)).toUtf8();
		return CreateErrorResponse(errorMessage, request);
	}

	return imtrest::ConstResponsePtr();
}


imtrest::ConstResponsePtr CWebSocketServletComp::UnregisterSubscription(const imtrest::IRequest& request) const
{
	const imtrest::CWebSocketRequest* webSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&request);
	if (webSocketRequest == nullptr){
		Q_ASSERT(false);

		return imtrest::ConstResponsePtr();
	}

	for (int index = 0; index < m_gqlSubscriberControllersCompPtr.GetCount(); index++){
		IGqlSubscriberController* controlerPtr = m_gqlSubscriberControllersCompPtr[index];
		if ((controlerPtr != nullptr) && m_gqlSubscriberControllersCompPtr[index]->UnRegisterSubscription(webSocketRequest->GetSubscriptionId())){

			QByteArray data = QString(R""(
{
"type": "stop",
"id": "%1"
}
			)"" ).arg(QString(webSocketRequest->GetSubscriptionId())).toUtf8();
			
			return CreateDataResponse(data, request);
			break;
		}
	}

	QByteArray errorMessage = "Subscription Id missing";
	return CreateErrorResponse(errorMessage, request);
}


imtrest::ConstResponsePtr CWebSocketServletComp::CreateDataResponse(QByteArray data, const imtrest::IRequest& request) const
{
	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();
	
	QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

	imtrest::ConstResponsePtr responsePtr(
				engine.CreateResponse(
							request,
							imtrest::IProtocolEngine::SC_OK,
							data,
							reponseTypeId));

	return responsePtr;
}


imtrest::ConstResponsePtr CWebSocketServletComp::CreateErrorResponse(QByteArray errorMessage, const imtrest::IRequest& request) const
{
	qDebug() << "CreateErrorResponse" << errorMessage << request.GetBody();
	qDebug() << "request.GetRequestId()" << request.GetRequestId();

	QByteArray requestBody = request.GetBody();
	QJsonDocument document = QJsonDocument::fromJson(requestBody);
	QJsonObject object = document.object();

	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();

	QString body = QString(R""(
{
	"id": "%1",
	"type": "error",
	"payload": {
		"errors": [
			{
				"errorType": "ProcessRequestError",
				"message": "%2"
			}
		]
	}
}
	)"" ).arg(object["id"].toString()).arg(QString(errorMessage));
	
	QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

	imtrest::ConstResponsePtr responsePtr(
				engine.CreateResponse(
							request,
							imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE,
							body.toUtf8(),
							reponseTypeId));

	SendErrorMessage(0, QString(errorMessage));

	return responsePtr;
}


} // namespace imtgql


