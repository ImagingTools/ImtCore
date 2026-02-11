// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CWebSocketServletComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtgql/CGqlContext.h>


namespace imtservergql
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

		case imtrest::CWebSocketRequest::MT_KEEP_ALIVE:
			return KeepAliveAcknowledge(request);

		case imtrest::CWebSocketRequest::MT_START:
			return RegisterSubscription(request);

		case imtrest::CWebSocketRequest::MT_STOP:
			return UnregisterSubscription(request);

		case imtrest::CWebSocketRequest::MT_QUERY:
			return ProcessGqlRequest(request);

		case imtrest::CWebSocketRequest::MT_ERROR:
			SendErrorMessage(0, "Error request", "CWebSocketServletComp");
			return imtrest::ConstResponsePtr();

		case imtrest::CWebSocketRequest::MT_DATA:
			SendErrorMessage(0, "Data request", "CWebSocketServletComp");
			return imtrest::ConstResponsePtr();

		default:{
			QByteArray errorMessage = QString("Method type not correct: %1").arg(webSocketRequest->GetMethodType()).toUtf8();
			return CreateErrorResponse(errorMessage, request);
		}
		break;
		}
	}
	else{
		return ProcessGqlRequest(request);
	}

	return imtrest::ConstResponsePtr();
}


// reimplemented (imtrest::IRequestManager)

const imtrest::ISender* CWebSocketServletComp::GetSender(const QByteArray& /*requestId*/) const
{
	return nullptr;
}


// protected methods

imtrest::ConstResponsePtr CWebSocketServletComp::InitConnection(const imtrest::IRequest& request) const
{
	const imtrest::CWebSocketRequest* webSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&request);
	if (webSocketRequest != nullptr){
		QByteArray data = QString(R"({"type": "connection_ack","payload": {"connectionTimeoutMs": 300000}})").toUtf8();

		return CreateDataResponse(data, request);
	}

	return imtrest::ConstResponsePtr();
}

imtrest::ConstResponsePtr CWebSocketServletComp::KeepAliveAcknowledge(const imtrest::IRequest& request) const
{
	const imtrest::CWebSocketRequest* webSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&request);
	if (webSocketRequest == nullptr){
		Q_ASSERT(false);

		return imtrest::ConstResponsePtr();
	}

	QByteArray data = QString(R"({"type": "pong"})").toUtf8();

	return CreateDataResponse(data, request);
}


imtrest::ConstResponsePtr CWebSocketServletComp::ProcessGqlRequest(const imtrest::IRequest& request) const
{
	if (m_workerManagerCompPtr.IsValid()){
		m_workerManagerCompPtr->ProcessRequest(request);
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
	if (object.value("payload").toObject().contains("data")){
		body = object.value("payload").toObject().value("data").toString().toUtf8();
	}
	else{
		body = document["payload"].toObject().value("query").toString().toUtf8();
		body = QByteArray("{\"query\": \"") + body + QByteArray("\"}");
	}

	qsizetype errorPosition;
	if (!gqlRequest.ParseQuery(body, errorPosition)){
		QString errorMessage = QString("Error when parsing request: '%1'; Error position: '%2'")
								.arg(qPrintable(body)).arg(errorPosition);
		return CreateErrorResponse(errorMessage.toUtf8(), request);
	}

	imtrest::CWebSocketRequest* webSocketRequestPtr = const_cast<imtrest::CWebSocketRequest*>(dynamic_cast<const imtrest::CWebSocketRequest*>(&request));
	if (webSocketRequestPtr != nullptr){
		webSocketRequestPtr->SetCommandId(gqlRequest.GetCommandId());
	}

	imtgql::IGqlContext* gqlContextPtr = const_cast<imtgql::IGqlContext*>(gqlRequest.GetRequestContext());
	imtgql::IGqlContext::Headers gqlHeaders;
	if (gqlContextPtr != nullptr){
		gqlHeaders = gqlContextPtr->GetHeaders();
	}
	else{
		gqlContextPtr = new imtgql::CGqlContext();
	}
	QJsonObject headers = object.value("headers").toObject();
	for (QString& key: headers.keys()){
		gqlHeaders.insert(key.toUtf8().toLower(), headers.value(key).toString().toUtf8());
	}

	gqlContextPtr->SetHeaders(gqlHeaders);
	gqlRequest.SetGqlContext(gqlContextPtr);

	QByteArray commandId = gqlRequest.GetCommandId();

	if (commandId.isEmpty()){
		return CreateErrorResponse(QByteArrayLiteral("Unable to register subscription with empty command-ID"), request);
	}

	imtgql::IGqlSubscriberController* subscriberControllerPtr = nullptr;
	for (int index = 0; index < m_gqlSubscriberControllersCompPtr.GetCount(); index++){
		imtgql::IGqlSubscriberController* gqlSubscriberControllerPtr = m_gqlSubscriberControllersCompPtr[index];
		if (gqlSubscriberControllerPtr != nullptr){
			if (gqlSubscriberControllerPtr->IsRequestSupported(gqlRequest)){
				subscriberControllerPtr = m_gqlSubscriberControllersCompPtr[index];

				break;
			}
		}
	}

	if (subscriberControllerPtr != nullptr){
		QString errorMessage;
		if (subscriberControllerPtr->RegisterSubscription(webSocketRequest->GetRequestId(), gqlRequest, request, errorMessage)){
			QByteArray data = QString(R"({"type": "start_ack","id": "%1"})")
						.arg(QString(webSocketRequest->GetRequestId())).toUtf8();

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
		imtgql::IGqlSubscriberController* controllerPtr = m_gqlSubscriberControllersCompPtr[index];
		if (controllerPtr != nullptr){
			QByteArray subscriptionId = webSocketRequest->GetRequestId();
			if (controllerPtr->UnregisterSubscription(subscriptionId)){
				QByteArray data = QString(R"({"type": "stop","id": "%1"})").arg(QString(subscriptionId)).toUtf8();
				return CreateDataResponse(data, request);
			}
		}
	}

	QByteArray errorMessage = QByteArray("Unable to unregister subscription'. Error: Subscription is unregistered");
	return CreateErrorResponse(errorMessage, request);
}


imtrest::ConstResponsePtr CWebSocketServletComp::CreateDataResponse(const QByteArray& data, const imtrest::IRequest& request) const
{
	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();

	QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");
	QByteArray commandId = request.GetCommandId();

	imtrest::ConstResponsePtr responsePtr(
				engine.CreateResponse(
							request,
							imtrest::IProtocolEngine::SC_OK,
							data,
							reponseTypeId).PopInterfacePtr());

	return responsePtr;
}


imtrest::ConstResponsePtr CWebSocketServletComp::CreateErrorResponse(const QByteArray& errorMessage, const imtrest::IRequest& request) const
{
	QByteArray requestBody = request.GetBody();
	QJsonDocument document = QJsonDocument::fromJson(requestBody);
	QJsonObject object = document.object();

	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();

	QString body = QString(R"({"id": "%1","type": "error","payload": {"errors": [{"errorType": "ProcessRequestError","message": "%2"}]}})")
							.arg(object["id"].toString())
							.arg(QString(errorMessage));

	QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

	imtrest::ConstResponsePtr responsePtr(
		engine.CreateResponse(
			request,
			imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE,
			body.toUtf8(),
			reponseTypeId).PopInterfacePtr());

	SendErrorMessage(0, QString(errorMessage));

	return responsePtr;
}


} // namespace imtservergql


