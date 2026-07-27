// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CWebSocketServletComp.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QPointer>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtbase/imtbase.h>


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
		case imtrest::CWebSocketRequest::MT_SUBSCRIBE:
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

		case imtrest::CWebSocketRequest::MT_COMPLETE:
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


// reimplemented (imtrest::IResponseDispatcher)

bool CWebSocketServletComp::SendResponse(const QByteArray& /*requestId*/, imtrest::ConstResponsePtr& /*response*/) const
{
	return false;
}


bool CWebSocketServletComp::SendRequest(const QByteArray& /*requestId*/, imtrest::ConstRequestPtr& /*request*/) const
{
	return false;
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
	const auto* webSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&request);
	if (webSocketRequest == nullptr){
		return CreateErrorResponse(QByteArrayLiteral("Invalid WebSocket request"), request);
	}

	// Capture all data from the request immediately - the CWebSocketRequest is parented
	// to QWebSocket via setParent(m_socket) in CWebSocketThread, so it can be
	// cascade-destroyed if the socket disconnects during processing.
	const QByteArray subscriptionId = webSocketRequest->GetRequestId();
	QByteArray body = request.GetBody();
	const QJsonDocument document = QJsonDocument::fromJson(body);
	if (document.isNull() || !document.isObject()) {
		QString errorMessage = QString("Error when parsing JSON request for command Id: '%1'").arg(request.GetCommandId());
		return CreateErrorResponse(errorMessage.toUtf8(), request);
	}

	const QJsonObject rootObject = document.object();
	const QJsonValue payloadValue = rootObject.value(QStringLiteral("payload"));

	if (payloadValue.isObject()) {
		const QJsonObject payloadObject = payloadValue.toObject();

		if (payloadObject.contains(QStringLiteral("data"))) {
			body = payloadObject.value(QStringLiteral("data")).toString().toUtf8();
		}
		else {
			body = QJsonDocument(payloadObject).toJson(QJsonDocument::Compact);
		}
	}
	else if (payloadValue.isString()) {
		body = payloadValue.toString().toUtf8();
	}

	imtgql::CGqlRequest gqlRequest;
	qsizetype errorPosition;
	if (!gqlRequest.ParseQuery(body, errorPosition)){
		QString errorMessage = QString("Error when parsing request: '%1'; Error position: '%2'")
								.arg(qPrintable(body)).arg(errorPosition);
		return CreateErrorResponse(errorMessage.toUtf8(), request);
	}

	// Track request lifetime with QPointer — the request is a QObject
	QPointer<QObject> requestGuard(const_cast<QObject*>(static_cast<const QObject*>(webSocketRequest)));

	imtrest::CWebSocketRequest* webSocketRequestPtr = const_cast<imtrest::CWebSocketRequest*>(webSocketRequest);
	if (webSocketRequestPtr != nullptr){
		webSocketRequestPtr->SetCommandId(gqlRequest.GetCommandId());
	}

	// Extract headers from the message JSON
	imtgql::IGqlContext::Headers gqlHeaders;
	imtgql::IGqlContext* existingContextPtr = const_cast<imtgql::IGqlContext*>(gqlRequest.GetRequestContext());
	if (existingContextPtr != nullptr){
		gqlHeaders = existingContextPtr->GetHeaders();
	}

	QJsonObject headers = rootObject.value("headers").toObject();
	for (const QString& key: headers.keys()){
		gqlHeaders.insert(key.toUtf8().toLower(), headers.value(key).toString().toUtf8());
	}

	if (!m_gqlContextCreatorCompPtr.IsValid()){
		return CreateErrorResponse(QByteArrayLiteral("GraphQL context creator is not configured"), request);
	}

	imtgql::IGqlContextCreator::ContextCreationError contextError;
	imtgql::IGqlContextUniquePtr gqlContextPtr = m_gqlContextCreatorCompPtr->CreateGqlContext(gqlHeaders, contextError);
	if (!gqlContextPtr.IsValid()){
		const QByteArray authToken = gqlHeaders.value(imtbase::s_authenticationTokenHeaderId);
		const QString maskedToken = authToken.isEmpty()
					? QStringLiteral("<empty>")
					: (QStringLiteral("***") + QString::fromUtf8(authToken.right(4)));

		const QString detail = contextError.message.isEmpty()
					? QStringLiteral("Unable to create GraphQL context")
					: contextError.message;

		QString statusLabel;
		QString wireMessage;
		if (contextError.status == imtgql::IGqlContextCreator::CCS_UNAUTHORIZED){
			statusLabel = QStringLiteral("401 Unauthorized");
			wireMessage = QStringLiteral("Unauthorized: %1").arg(detail);
		}
		else if (contextError.status == imtgql::IGqlContextCreator::CCS_FORBIDDEN){
			statusLabel = QStringLiteral("403 Forbidden");
			wireMessage = QStringLiteral("Forbidden: %1").arg(detail);
		}
		else{
			statusLabel = QStringLiteral("internal error");
			wireMessage = detail;
		}

		SendErrorMessage(
					0,
					QStringLiteral("WebSocket subscription auth failed (%1) for command '%2', token %3: %4")
							.arg(statusLabel,
								 QString::fromUtf8(gqlRequest.GetCommandId()),
								 maskedToken,
								 detail),
					QStringLiteral("CWebSocketServletComp"));
		// CreateErrorResponse logs the same wire message once for non-auth paths;
		// auth path already logged above — pass the informative message to the client.
		return CreateErrorResponse(wireMessage.toUtf8(), request, /*logMessage*/ false);
	}

	gqlRequest.SetGqlContext(std::move(gqlContextPtr));

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
		// Final lifetime check before passing request reference to publisher
		if (requestGuard.isNull()){
			SendErrorMessage(0, QStringLiteral("WebSocket request destroyed before subscription registration"), QStringLiteral("CWebSocketServletComp"));
			return imtrest::ConstResponsePtr();
		}

		QString errorMessage;
		if (subscriberControllerPtr->RegisterSubscription(subscriptionId, gqlRequest, request, errorMessage)){
			return imtrest::ConstResponsePtr();
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
				QByteArray data = QString(R"({"type": "complete","id": "%1"})").arg(QString(subscriptionId)).toUtf8();
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


imtrest::ConstResponsePtr CWebSocketServletComp::CreateErrorResponse(
			const QByteArray& errorMessage,
			const imtrest::IRequest& request,
			bool logMessage) const
{
	QByteArray requestBody = request.GetBody();
	QJsonDocument document = QJsonDocument::fromJson(requestBody);
	QJsonObject object = document.object();

	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();

	// Build JSON via QJsonDocument so quotes/newlines in errorMessage cannot break the frame.
	QJsonObject errorObj;
	errorObj.insert(QStringLiteral("message"), QString::fromUtf8(errorMessage));
	QJsonObject extensionsObj;
	extensionsObj.insert(QStringLiteral("type"), QStringLiteral("Warning"));
	errorObj.insert(QStringLiteral("extensions"), extensionsObj);

	QJsonArray payloadArr;
	payloadArr.append(errorObj);

	QJsonObject rootObj;
	rootObj.insert(QStringLiteral("id"), object.value(QStringLiteral("id")).toString());
	rootObj.insert(QStringLiteral("type"), QStringLiteral("error"));
	rootObj.insert(QStringLiteral("payload"), payloadArr);

	const QByteArray body = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);
	const QByteArray reponseTypeId = QByteArrayLiteral("text/html; charset=utf-8");

	imtrest::ConstResponsePtr responsePtr(
		engine.CreateResponse(
			request,
			imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE,
			body,
			reponseTypeId).PopInterfacePtr());

	if (logMessage){
		SendErrorMessage(0, QString::fromUtf8(errorMessage), QStringLiteral("CWebSocketServletComp"));
	}

	return responsePtr;
}


} // namespace imtservergql
