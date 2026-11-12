// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CWebSocketServletComp.h>


// STL includes
#include <algorithm>

// Qt includes
#include <QtCore/QMultiHash>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QPointer>
#include <QtCore/QDeadlineTimer>
#include <QtCore/QScopeGuard>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtbase/imtbase.h>
#include <imtbase/IObjectCollection.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <imtservergql/imtservergql.h>


namespace imtservergql
{


static const int s_maxParallelChecks = 4;


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
			QByteArray errorMessage = QStringLiteral("Method type not correct: %1").arg(webSocketRequest->GetMethodType()).toUtf8();
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


// reimplemented (imtrest::IRequestEventHandler)

void CWebSocketServletComp::OnRequestDestroyed(imtrest::IRequest* request)
{
	RemoveTokenSubscriptions([request](const TokenSubscription& subscription){ return subscription.requestPtr == request; });
}


// protected methods

imtrest::ConstResponsePtr CWebSocketServletComp::InitConnection(const imtrest::IRequest& request) const
{
	const imtrest::CWebSocketRequest* webSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&request);
	if (webSocketRequest != nullptr){
		QByteArray data = QStringLiteral(R"({"type": "connection_ack","payload": {"connectionTimeoutMs": 300000}})").toUtf8();

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

	QByteArray data = QStringLiteral(R"({"type": "pong"})").toUtf8();

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

	// CreateGqlContext sets the thread context; it must not outlive this registration.
	auto cleanup = qScopeGuard([]() {
		imtgql::CGqlRequestContextManager::Clear();
	});

	// Capture all data from the request immediately - the CWebSocketRequest is parented
	// to QWebSocket via setParent(m_socket) in CWebSocketThread, so it can be
	// cascade-destroyed if the socket disconnects during processing.
	const QByteArray subscriptionId = webSocketRequest->GetRequestId();

	if (subscriptionId.isEmpty()){
		return CreateErrorResponse(QByteArrayLiteral("Unable to register subscription with empty subscription-ID"), request);
	}

	QByteArray body = request.GetBody();
	const QJsonDocument document = QJsonDocument::fromJson(body);
	if (document.isNull() || !document.isObject()) {
		QString errorMessage = QStringLiteral("Error when parsing JSON request for command Id: '%1'").arg(request.GetCommandId());
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
		// Chained, not arg(body, errorPosition): that binds the position to the
		// arg(text, fieldWidth) overload, which pads the message with as many
		// spaces as the position - hundreds of megabytes of them for a failure
		// late in a query, built in memory and then written out one message at a
		// time until the server stops answering anything else.
		QString errorMessage = QStringLiteral("Error when parsing request: '%1'; Error position: '%2'")
								.arg(QString::fromUtf8(body))
								.arg(errorPosition);
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

	// A subscription with credentials is registered with the access read now: revoked credentials never become a subscription
	const bool hasCredentials = !gqlHeaders.value(imtbase::s_authenticationTokenHeaderId).isEmpty() || !gqlHeaders.value(imtbase::s_serverTokenHeaderId).isEmpty();
	quint64 accessGeneration;
	{
		QMutexLocker locker(&m_tokenSubscriptionsMutex);
		accessGeneration = m_accessChangeGeneration;
	}
	const qint64 authorizationGeneration = m_gqlContextCreatorCompPtr->GetAuthorizationGeneration();
	imtgql::IGqlContextCreator::ContextCreationError contextError;
	QDateTime sessionExpiresAt;
	imtgql::IGqlContextUniquePtr gqlContextPtr = hasCredentials
				? CreateCurrentContext(gqlHeaders, false, contextError, &sessionExpiresAt)
				: m_gqlContextCreatorCompPtr->CreateGqlContext(gqlHeaders, contextError);
	if (requestGuard.isNull()){
		return imtrest::ConstResponsePtr();
	}
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

		// The client tells a refusal from an access it may confirm later by the code
		return CreateErrorResponse(wireMessage.toUtf8(), request, /*logMessage*/ false, GetRefusalCode(contextError));
	}

	const AuthorizationData registeredAccess = GetAuthorizationData(*gqlContextPtr);
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

		if (!IsSubscriberAllowed(*subscriberControllerPtr, gqlRequest)){
			return CreateErrorResponse(QStringLiteral("The subscription '%1' is not allowed").arg(QString::fromUtf8(commandId)).toUtf8(), request, true, s_subscriptionForbiddenCode);
		}

		const bool isRecipientChannel = subscriberControllerPtr->IsOpenFor(imtgql::IGqlSubscriberController::SK_RECIPIENT, gqlRequest);

		QString errorMessage;
		if (subscriberControllerPtr->RegisterSubscription(subscriptionId, gqlRequest, request, errorMessage)){
			bool accessChanged;
			{
				QMutexLocker locker(&m_tokenSubscriptionsMutex);
				accessChanged = accessGeneration != m_accessChangeGeneration;
				accessGeneration = m_accessChangeGeneration;
			}

			QString authorizationError;
			QByteArray reasonCode;
			const bool isCurrent = !hasCredentials
						|| (!accessChanged && m_gqlContextCreatorCompPtr->GetAuthorizationGeneration() == authorizationGeneration)
						|| IsAuthorizationCurrent(gqlHeaders, registeredAccess, isRecipientChannel, authorizationError, reasonCode);
			if (requestGuard.isNull()){
				return imtrest::ConstResponsePtr();
			}
			if (!isCurrent){
				subscriberControllerPtr->UnregisterSubscription(subscriptionId);
				return CreateErrorResponse(authorizationError.toUtf8(), request, true, reasonCode);
			}

			if (hasCredentials){
				TokenSubscription subscription;
				subscription.subscriptionId = subscriptionId;
				subscription.controllerPtr = subscriberControllerPtr;
				subscription.headers = gqlHeaders;
				subscription.registeredAccess = registeredAccess;
				subscription.isRecipientChannel = isRecipientChannel;
				subscription.requestPtr = &request;
				subscription.connectionPtr = webSocketRequest->parent();

				AddTokenSubscription(subscription, sessionExpiresAt, accessGeneration);
				webSocketRequestPtr->RegisterRequestEventHandler(const_cast<CWebSocketServletComp*>(this));
			}

			if (webSocketRequest->GetMethodType() == imtrest::CWebSocketRequest::MT_START){
				return CreateDataResponse(QStringLiteral(R"({"type": "start_ack","id": "%1"})").arg(QString::fromUtf8(subscriptionId)).toUtf8(), request);
			}

			return imtrest::ConstResponsePtr();
		}

		return CreateErrorResponse(errorMessage.isEmpty()
					? QByteArrayLiteral("Failed to register subscription")
					: errorMessage.toUtf8(), request);
	}
	else{
		QByteArray errorMessage = QStringLiteral("The requested command could not be executed. No servlet was found for the given command: '%1")
		.arg(commandId).toUtf8();
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

	const QByteArray subscriptionId = webSocketRequest->GetRequestId();

	// The stop comes in a request of its own over the same connection
	RemoveTokenSubscriptions([&subscriptionId, webSocketRequest](const TokenSubscription& subscription){
		return subscription.subscriptionId == subscriptionId && subscription.connectionPtr == webSocketRequest->parent();
	});

	for (int index = 0; index < m_gqlSubscriberControllersCompPtr.GetCount(); index++){
		imtgql::IGqlSubscriberController* controllerPtr = m_gqlSubscriberControllersCompPtr[index];
		if (controllerPtr != nullptr){
			if (controllerPtr->UnregisterSubscription(subscriptionId)){
				QByteArray data = QStringLiteral(R"({"type": "complete","id": "%1"})").arg(subscriptionId).toUtf8();
				return CreateDataResponse(data, request);
			}
		}
	}

	SendWarningMessage(
		0,
		QStringLiteral("Unregister requested for subscription '%1', which is not registered.").arg(subscriptionId),
		QStringLiteral("CWebSocketServletComp"));

QJsonObject completeObject;
	completeObject.insert(QStringLiteral("type"), QStringLiteral("complete"));
	completeObject.insert(QStringLiteral("id"), QString(subscriptionId));
	const QByteArray data = QJsonDocument(completeObject).toJson(QJsonDocument::Compact);
	return CreateDataResponse(data, request);
}


imtrest::ConstResponsePtr CWebSocketServletComp::CreateDataResponse(const QByteArray& data, const imtrest::IRequest& request) const
{
	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();

	QByteArray reponseTypeId = QByteArrayLiteral("text/html; charset=utf-8");
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
			bool logMessage,
			const QByteArray& reasonCode) const
{
	QByteArray requestBody = request.GetBody();
	QJsonDocument document = QJsonDocument::fromJson(requestBody);
	QJsonObject object = document.object();

	QString subscriptionId = object.value(QStringLiteral("id")).toString();
	if (subscriptionId.isEmpty()){
		auto webSocketRequestPtr = dynamic_cast<const imtrest::CWebSocketRequest*>(&request);
		if (webSocketRequestPtr != nullptr){
			subscriptionId = QString::fromUtf8(webSocketRequestPtr->GetRequestId());
		}
	}

	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();

	// Build JSON via QJsonDocument so quotes/newlines in errorMessage cannot break the frame.
	QJsonObject errorObj;
	errorObj.insert(QStringLiteral("message"), QString::fromUtf8(errorMessage));
	QJsonObject extensionsObj;
	extensionsObj.insert(QStringLiteral("type"), QStringLiteral("Warning"));
	if (!reasonCode.isEmpty()){
		extensionsObj.insert(QStringLiteral("code"), QString::fromUtf8(reasonCode));
	}
	errorObj.insert(QStringLiteral("extensions"), extensionsObj);

	QJsonArray payloadArr;
	payloadArr.append(errorObj);

	QJsonObject rootObj;
	rootObj.insert(QStringLiteral("id"), subscriptionId);
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


// reimplemented (icomp::CComponentBase)

void CWebSocketServletComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_checkSchedule = CSubscriptionCheckSchedule(qint64(*m_maxCheckIntervalAttrPtr) * 1000, qint64(*m_gracePeriodAttrPtr) * 1000);
	m_clock.start();
	m_checkPool.setMaxThreadCount(s_maxParallelChecks);

	for (int index = 0; index < m_accessModelsCompPtr.GetCount(); index++){
		ObserveAccessModel(m_accessModelsCompPtr[index]);
	}

	// The manager of the server tokens of this server tells a revoked or changed token
	if (m_serverTokenReceiverCompPtr.IsValid()){
		ObserveAccessModel(dynamic_cast<imod::IModel*>(m_serverTokenReceiverCompPtr->GetTokenManager()));
	}
}


void CWebSocketServletComp::OnComponentDestroyed()
{
	EnsureModelsDetached();

	QThread* threadPtr = nullptr;
	{
		QMutexLocker locker(&m_tokenSubscriptionsMutex);
		m_isTokenCheckStopped = true;
		m_tokenCheckCondition.wakeAll();
		threadPtr = m_tokenCheckThreadPtr;
		m_tokenCheckThreadPtr = nullptr;
	}

	if (threadPtr != nullptr){
		threadPtr->wait();
		delete threadPtr;
	}

	// A running check ends with its request: its result is not used
	m_checkPool.waitForDone();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelObserverBase)

void CWebSocketServletComp::OnUpdate(imod::IModel* /*modelPtr*/, const istd::IChangeable::ChangeSet& changeSet)
{
	QMutexLocker locker(&m_tokenSubscriptionsMutex);

	++m_accessChangeGeneration;

	// The changed data of a subscribed user concern its subscriptions only; any other change may concern everybody
	const QByteArray objectId = changeSet.GetChangeInfo(imtbase::IObjectCollection::CN_OBJECT_DATA_CHANGED).toByteArray();
	const qint64 nowMs = m_clock.elapsed();
	bool isUserChange = false;
	if (!objectId.isEmpty()){
		for (auto it = m_tokenSubscriptions.cbegin(); it != m_tokenSubscriptions.cend(); ++it){
			if (!it->isEmpty() && it->first().registeredAccess.userId == objectId){
				m_checkSchedule.OnAccessChanged(nowMs, it.key());
				isUserChange = true;
			}
		}
	}
	if (!isUserChange){
		m_checkSchedule.OnAccessChanged(nowMs);
	}

	m_tokenCheckCondition.wakeAll();
}


// private methods

imtgql::IGqlContextUniquePtr CWebSocketServletComp::CreateCurrentContext(
			const imtgql::IGqlContext::Headers& headers,
			bool isEstablished,
			imtgql::IGqlContextCreator::ContextCreationError& error,
			QDateTime* sessionExpiresAtPtr) const
{
	// A check that met a change of the access is repeated; a refusal or a failed read is not
	for (int attempt = 0; attempt < 3; attempt++){
		imtgql::IGqlContextCreator::AuthorizationState state = imtgql::IGqlContextCreator::AS_UNAVAILABLE;
		imtgql::IGqlContextUniquePtr gqlContextPtr = m_gqlContextCreatorCompPtr->RenewGqlContext(headers, isEstablished, state, error, sessionExpiresAtPtr);
		if (state != imtgql::IGqlContextCreator::AS_STALE){
			return state == imtgql::IGqlContextCreator::AS_CONFIRMED ? std::move(gqlContextPtr) : imtgql::IGqlContextUniquePtr();
		}
	}

	error.status = imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR;
	error.message = QStringLiteral("The access kept changing during the check");

	return imtgql::IGqlContextUniquePtr();
}


bool CWebSocketServletComp::IsAuthorizationCurrent(
			const imtgql::IGqlContext::Headers& headers,
			const AuthorizationData& registeredAccess,
			bool isRecipientChannel,
			QString& errorMessage,
			QByteArray& reasonCode) const
{
	imtgql::IGqlContextCreator::ContextCreationError error;
	const imtgql::IGqlContextUniquePtr gqlContextPtr = CreateCurrentContext(headers, false, error);
	if (!gqlContextPtr.IsValid()){
		errorMessage = error.message;
		reasonCode = GetRefusalCode(error);

		return false;
	}

	if (!IsServedBy(registeredAccess, isRecipientChannel, GetAuthorizationData(*gqlContextPtr))){
		errorMessage = QStringLiteral("The permissions changed during subscription registration");
		reasonCode = s_subscriptionPermissionsChangedCode;

		return false;
	}

	return true;
}


bool CWebSocketServletComp::IsServedBy(const AuthorizationData& registeredAccess, bool isRecipientChannel, const AuthorizationData& currentAccess)
{
	// Compared with the access of the registration, not with the last check: more access serves it, the same recipient serves its own channel
	return IsAccessWithin(registeredAccess, currentAccess) || (isRecipientChannel && IsSameRecipient(registeredAccess, currentAccess));
}


QByteArray CWebSocketServletComp::GetRefusalCode(const imtgql::IGqlContextCreator::ContextCreationError& error)
{
	switch (error.status){
	case imtgql::IGqlContextCreator::CCS_UNAUTHORIZED:
		return s_subscriptionUnauthorizedCode;
	case imtgql::IGqlContextCreator::CCS_FORBIDDEN:
		return s_subscriptionForbiddenCode;
	default:
		return s_subscriptionAuthorizationUnavailableCode;
	}
}


QByteArray CWebSocketServletComp::GetCredentialKey(const imtgql::IGqlContext::Headers& headers)
{
	return headers.value(imtbase::s_authenticationTokenHeaderId) + '\n'
				+ headers.value(imtbase::s_serverTokenHeaderId) + '\n'
				+ headers.value(imtbase::s_productIdHeaderId);
}


qint64 CWebSocketServletComp::ToScheduleTime(const QDateTime& time, qint64 nowMs) const
{
	return time.isValid() ? std::max(qint64(0), nowMs + QDateTime::currentDateTimeUtc().msecsTo(time)) : -1;
}

bool CWebSocketServletComp::IsSubscriberAllowed(
			const imtgql::IGqlSubscriberController& controller,
			const imtgql::CGqlRequest& gqlRequest) const
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr == nullptr){
		return false;
	}

	// A server acting on its own is a user holding the permissions of its token
	const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		return !*m_isAnonymousDeniedAttrPtr || controller.IsOpenFor(imtgql::IGqlSubscriberController::SK_ANONYMOUS, gqlRequest);
	}

	return !*m_isUndeclaredUserChannelDeniedAttrPtr || userInfoPtr->IsAdmin() || controller.IsOpenFor(imtgql::IGqlSubscriberController::SK_USER, gqlRequest);
}


void CWebSocketServletComp::AddTokenSubscription(const TokenSubscription& subscription, const QDateTime& sessionExpiresAt, quint64 accessGeneration) const
{
	QMutexLocker locker(&m_tokenSubscriptionsMutex);

	const QByteArray credentialKey = GetCredentialKey(subscription.headers);
	m_tokenSubscriptions[credentialKey].append(subscription);

	// Just checked: next at the end of its session or at the control check
	const qint64 nowMs = m_clock.elapsed();
	m_checkSchedule.Add(credentialKey, nowMs, ToScheduleTime(sessionExpiresAt, nowMs));
	// Include events after the last registration check, even when this key did not exist yet.
	if (accessGeneration != m_accessChangeGeneration){
		m_checkSchedule.OnAccessChanged(nowMs, credentialKey);
	}

	if (m_tokenCheckThreadPtr == nullptr && !m_isTokenCheckStopped){
		m_tokenCheckThreadPtr = QThread::create([this](){ RunTokenChecks(); });
		m_tokenCheckThreadPtr->setObjectName(QStringLiteral("WebSocketTokenCheck"));
		m_tokenCheckThreadPtr->start();
	}

	m_tokenCheckCondition.wakeAll();
}


void CWebSocketServletComp::RunTokenChecks() const
{
	QMutexLocker locker(&m_tokenSubscriptionsMutex);

	while (!m_isTokenCheckStopped){
		const qint64 nowMs = m_clock.elapsed();

		// Not confirmed within the grace period: ended as temporarily unavailable, not as refused
		for (const QByteArray& credentialKey : m_checkSchedule.TakeExpired(nowMs)){
			DropCredentialsLocked(
						credentialKey,
						s_subscriptionAuthorizationUnavailableCode,
						QStringLiteral("The access could not be confirmed: the authorization is temporarily unavailable"));
		}

		// A check may wait for the network: it holds back neither the other checks nor the end of a grace period
		for (const QByteArray& credentialKey : m_checkSchedule.TakeDue(nowMs)){
			const auto found = m_tokenSubscriptions.constFind(credentialKey);
			if (found == m_tokenSubscriptions.cend() || found->isEmpty()){
				m_checkSchedule.Remove(credentialKey);
				continue;
			}

			const imtgql::IGqlContext::Headers headers = found->first().headers;
			const qint64 attemptId = m_checkSchedule.GetAttemptId(credentialKey);
			m_checkPool.start([this, credentialKey, attemptId, headers](){ CheckCredentials(credentialKey, attemptId, headers); });
		}

		const qint64 wakeMs = m_checkSchedule.GetNextWakeTime();
		const qint64 waitMs = wakeMs - m_clock.elapsed();
		if (wakeMs < 0){
			m_tokenCheckCondition.wait(&m_tokenSubscriptionsMutex);
		}
		else if (waitMs > 0){
			m_tokenCheckCondition.wait(&m_tokenSubscriptionsMutex, QDeadlineTimer(waitMs));
		}
	}
}


void CWebSocketServletComp::RemoveTokenSubscriptions(const std::function<bool(const TokenSubscription&)>& predicate) const
{
	QMutexLocker locker(&m_tokenSubscriptionsMutex);
	for (auto it = m_tokenSubscriptions.begin(); it != m_tokenSubscriptions.end(); ){
		it->removeIf(predicate);
		if (it->isEmpty()){
			m_checkSchedule.Remove(it.key());
			it = m_tokenSubscriptions.erase(it);
		}
		else{
			++it;
		}
	}
}


void CWebSocketServletComp::CheckCredentials(const QByteArray& credentialKey, qint64 attemptId, const imtgql::IGqlContext::Headers& headers) const
{
	{
		QMutexLocker locker(&m_tokenSubscriptionsMutex);
		if (m_isTokenCheckStopped || !m_checkSchedule.IsCurrentAttempt(credentialKey, attemptId)){
			return;
		}
	}

	// An established subscription is checked as one: an expired access token may hold there while its session lives
	imtgql::IGqlContextCreator::ContextCreationError error;
	imtgql::IGqlContextCreator::AuthorizationState state = imtgql::IGqlContextCreator::AS_UNAVAILABLE;
	QDateTime sessionExpiresAt;
	const imtgql::IGqlContextUniquePtr gqlContextPtr = m_gqlContextCreatorCompPtr->RenewGqlContext(headers, true, state, error, &sessionExpiresAt);
	const bool isConfirmed = state == imtgql::IGqlContextCreator::AS_CONFIRMED && gqlContextPtr.IsValid();
	const AuthorizationData currentAccess = isConfirmed ? GetAuthorizationData(*gqlContextPtr) : AuthorizationData();

	QMutexLocker locker(&m_tokenSubscriptionsMutex);

	// Ended meanwhile, by its client or by its grace period, perhaps subscribed again: the result is not used
	if (m_isTokenCheckStopped || !m_checkSchedule.IsCurrentAttempt(credentialKey, attemptId)){
		return;
	}

	const qint64 nowMs = m_clock.elapsed();
	if (state == imtgql::IGqlContextCreator::AS_DENIED){
		DropCredentialsLocked(credentialKey, GetRefusalCode(error), error.message);
	}
	else if (isConfirmed){
		auto found = m_tokenSubscriptions.find(credentialKey);
		if (found == m_tokenSubscriptions.end()){
			m_checkSchedule.Remove(credentialKey);
			return;
		}
		QList<TokenSubscription>& subscriptions = *found;
		for (int index = subscriptions.size() - 1; index >= 0; --index){
			const TokenSubscription& subscription = subscriptions[index];
			if (!IsServedBy(subscription.registeredAccess, subscription.isRecipientChannel, currentAccess)){
				DropTokenSubscriptionLocked(subscriptions.takeAt(index), s_subscriptionPermissionsChangedCode, QStringLiteral("The permissions have changed"));
			}
		}

		if (!subscriptions.isEmpty()){
			m_checkSchedule.OnChecked(credentialKey, CSubscriptionCheckSchedule::CR_CONFIRMED, nowMs, ToScheduleTime(sessionExpiresAt, nowMs));
		}
		else{
			m_tokenSubscriptions.erase(found);
			m_checkSchedule.Remove(credentialKey);
		}
	}
	else{
		// Nothing is known: the subscriptions are kept within the grace period, a stale check is repeated
		const CSubscriptionCheckSchedule::CheckResult result = (state == imtgql::IGqlContextCreator::AS_STALE)
					? CSubscriptionCheckSchedule::CR_STALE
					: CSubscriptionCheckSchedule::CR_UNAVAILABLE;
		m_checkSchedule.OnChecked(credentialKey, result, nowMs);
	}

	m_tokenCheckCondition.wakeAll();
}


void CWebSocketServletComp::DropTokenSubscriptionLocked(const TokenSubscription& subscription, const QByteArray& reasonCode, const QString& reason) const
{
	// Under the lock the request is alive: its destructor waits for it in OnRequestDestroyed
	subscription.controllerPtr->UnregisterSubscription(subscription.subscriptionId);

	SendWarningMessage(0, QStringLiteral("Subscription '%1' dropped: %2").arg(QString::fromUtf8(subscription.requestPtr->GetCommandId()), reason), QStringLiteral("CWebSocketServletComp"));

	if (m_requestManagerCompPtr.IsValid()){
		imtrest::ConstResponsePtr responsePtr = CreateErrorResponse(QStringLiteral("Forbidden: %1").arg(reason).toUtf8(), *subscription.requestPtr, false, reasonCode);
		if (responsePtr.IsValid()){
			m_requestManagerCompPtr->SendResponse(subscription.requestPtr->GetRequestId(), responsePtr);
		}
	}
}


void CWebSocketServletComp::DropCredentialsLocked(const QByteArray& credentialKey, const QByteArray& reasonCode, const QString& reason) const
{
	const QList<TokenSubscription> subscriptions = m_tokenSubscriptions.take(credentialKey);
	for (auto it = subscriptions.crbegin(); it != subscriptions.crend(); ++it){
		DropTokenSubscriptionLocked(*it, reasonCode, reason);
	}

	m_checkSchedule.Remove(credentialKey);
}


void CWebSocketServletComp::ObserveAccessModel(imod::IModel* modelPtr)
{
	if (modelPtr != nullptr && !modelPtr->IsAttached(this)){
		modelPtr->AttachObserver(this);
	}
}


} // namespace imtservergql
