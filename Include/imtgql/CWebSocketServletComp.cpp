#include <imtgql/CWebSocketServletComp.h>


// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/ISender.h>
#include <imtrest/IResponse.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CWebSocketRequest.h>

// Qt includes
#include <QtCore/QJsonDocument>


namespace imtgql
{


// public methods

// reimplemented (IRequestHandler)

imtrest::IRequestServlet::ConstResponsePtr CWebSocketServletComp::ProcessRequest(const imtrest::IRequest& request) const
{
	QByteArray commandId = request.GetCommandId();

	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();

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
			return UnRegisterSubscription(request);		
			break;

		default:{
			QByteArray errorMessage = QString("Method type not corrected: %1").arg(webSocketRequest->GetMethodType()).toUtf8();

			return CreateErrorResponse(errorMessage, request);
			}
			break;
		}
	}
	else{
		QByteArray errorMessage = "Error request";

		return CreateErrorResponse(errorMessage, request);
	}

	return imtrest::IRequestServlet::ConstResponsePtr();
}


QByteArray CWebSocketServletComp::GetSupportedCommandId() const
{
	return "";
}


imtrest::IRequestServlet::ConstResponsePtr CWebSocketServletComp::InitConnection(const imtrest::IRequest& request) const
{
	const imtrest::CWebSocketRequest* webSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&request);

	if (webSocketRequest != nullptr){
	QByteArray data = QString(R""(
{
"type": "connection_ack",
"payload": {
	"connectionTimeoutMs": 300000
}
}
		)"" ).toUtf8();
	
		return CreateDataResponse(data, request);
	}

	return imtrest::IRequestServlet::ConstResponsePtr();	
}


imtrest::IRequestServlet::ConstResponsePtr CWebSocketServletComp::RegisterSubscription(const imtrest::IRequest& request) const
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
		if (m_gqlSubscriberControllersCompPtr[index]->IsRequestSupported(gqlRequest)){
			subscriberControllerPtr = m_gqlSubscriberControllersCompPtr[index];
		}
	}

	if (subscriberControllerPtr != nullptr){
		QString errorMessage;
		if (subscriberControllerPtr->RegisterSubscribtion(webSocketRequest->GetSubscriptionId(), gqlRequest, request, errorMessage)){
			QByteArray data = QString(R""(
{
"type": "start_ack",
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

	imtrest::IRequestServlet::ConstResponsePtr();
}


imtrest::IRequestServlet::ConstResponsePtr CWebSocketServletComp::UnRegisterSubscription(const imtrest::IRequest& request) const
{
	IGqlSubscriberController* subscriberControllerPtr = nullptr;
	const imtrest::CWebSocketRequest* webSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&request);
	if (webSocketRequest == nullptr){
		Q_ASSERT(false);

		return imtrest::IRequestServlet::ConstResponsePtr();
	}

	for (int index = 0; index < m_gqlSubscriberControllersCompPtr.GetCount(); index++){
		if (m_gqlSubscriberControllersCompPtr[index]->UnRegisterSubscribtion(webSocketRequest->GetSubscriptionId())){

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


imtrest::IRequestServlet::ConstResponsePtr CWebSocketServletComp::CreateDataResponse(QByteArray data, const imtrest::IRequest& request) const
{
	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();
	
	QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

	imtrest::IRequestServlet::ConstResponsePtr responsePtr(engine.CreateResponse(request, imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, data, reponseTypeId));


	return responsePtr;
}


imtrest::IRequestServlet::ConstResponsePtr CWebSocketServletComp::CreateErrorResponse(QByteArray errorMessage, const imtrest::IRequest& request) const
{
	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();

	QString body = QString(R""(
{
	"type": "error",
	"payload": {
		"errors": [
			{
				"errorType": "ProcessRequestError",
				"message": "%1"
			}
		]
	}
}
	)"" ).arg(QString(errorMessage));
	
	QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

	imtrest::IRequestServlet::ConstResponsePtr responsePtr(engine.CreateResponse(request, imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body.toUtf8(), reponseTypeId));

	SendErrorMessage(0, QString(errorMessage));

	return responsePtr;
}


void CWebSocketServletComp::OnComponentCreated() 
{
	BaseClass::OnComponentCreated();
}




} // namespace imtrest


