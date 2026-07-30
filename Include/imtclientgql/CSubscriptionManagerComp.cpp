// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CSubscriptionManagerComp.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QFutureWatcher>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QMessageAuthenticationCode>
#include <QtCore/QMetaObject>
#include <QtCore/QThread>
#include <QtCore/QUuid>
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QUrl>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtrest/CWebSocketRequest.h>
#include <imtgql/CGqlResponse.h>


namespace imtclientgql
{


// public methods

// reimplemented (imtgql::IGqlSubscriptionManager)

QByteArray CSubscriptionManagerComp::RegisterSubscription(
			const imtgql::IGqlRequest& subscriptionRequest,
			IGqlSubscriptionClient* subscriptionClient)
{
	if (!m_connectionStatusProviderCompPtr.IsValid()){
		return QByteArray();
	}

	if (!subscriptionClient){
		return QByteArray();
	}

	auto requestImplPtr = dynamic_cast<const imtgql::CGqlRequest*>(&subscriptionRequest);
	if (requestImplPtr == nullptr){
		SendWarningMessage(0, "Unexpected subscription request");

		return QByteArray();
	}

	QByteArray clientId = requestImplPtr->GetHeader("clientid");

	QMutexLocker locker(&m_registeredClientsMutex);

	for (QByteArray subscriptionId : m_registeredClients.keys()){
		if (m_registeredClients[subscriptionId].m_request.IsEqual(subscriptionRequest) && m_registeredClients[subscriptionId].m_clientId == clientId){
			m_registeredClients[subscriptionId].m_clients.append(subscriptionClient);

			return subscriptionId;
		}
	}

	QByteArray subscriptionId = QUuid::createUuid().toString(QUuid::WithoutBraces).toLocal8Bit();

	SubscriptionHelper subscriptionHelper;
	subscriptionHelper.m_request = *requestImplPtr;
	subscriptionHelper.m_clientId = clientId;
	subscriptionHelper.m_status = IGqlSubscriptionClient::SS_IN_REGISTRATION;
	subscriptionHelper.m_clients.append(subscriptionClient);
	m_registeredClients.insert(subscriptionId, subscriptionHelper);

	locker.unlock();

	if (!SubscriptionRegister(*requestImplPtr, subscriptionId)){
		QMutexLocker failedRegistrationLocker(&m_registeredClientsMutex);
		m_registeredClients.remove(subscriptionId);

		return QByteArray();
	}

	return subscriptionId;
}


bool CSubscriptionManagerComp::UnregisterSubscription(const QByteArray& subscriptionId)
{
	QMutexLocker locker(&m_registeredClientsMutex);

	if (m_registeredClients.contains(subscriptionId)){
		m_registeredClients.remove(subscriptionId);

		return true;
	}

	return false;
}


// reimplemented (imod::CSingleModelObserverBase)

void CSubscriptionManagerComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!m_connectionStatusProviderCompPtr.IsValid()){
		return;
	}

	QMutexLocker locker(&m_registeredClientsMutex);

	QByteArray clientId = changeSet.GetChangeInfo("ClientId").toByteArray();

	for (const QByteArray& subscriptionId : m_registeredClients.keys()){
		if (changeSet.Contains(imtcom::IConnectionStatusProvider::CF_CONNECTED)){
			if (m_registeredClients[subscriptionId].m_clientId == clientId){
				SubscriptionRegister(m_registeredClients[subscriptionId].m_request, subscriptionId);
				m_registeredClients[subscriptionId].m_status = IGqlSubscriptionClient::SS_REGISTERED;
			}
		}
		else{
			if (m_registeredClients[subscriptionId].m_clientId == clientId){
				m_registeredClients[subscriptionId].m_status = IGqlSubscriptionClient::SS_IN_REGISTRATION;
			}
		}

		UpdateCustomerSubscriptionStatuses(subscriptionId);
	}
}


// reimplemented (imtrest::IRequestServlet)

bool CSubscriptionManagerComp::IsCommandSupported(const QByteArray& /*commandId*/) const
{
	return true;
}


imtrest::ConstResponsePtr CSubscriptionManagerComp::ProcessRequest(const imtrest::IRequest& request, const QByteArray& subCommandId) const
{
	QMutexLocker locker(&m_registeredClientsMutex);

	QByteArray commandId = subCommandId.isEmpty() ? request.GetCommandId() : subCommandId;

	auto webSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&request);
	if (webSocketRequest != nullptr){
		QByteArray message = webSocketRequest->GetBody();

		QJsonParseError jsonError;
		QJsonDocument jsonDocument = QJsonDocument::fromJson(message, &jsonError);
		if (jsonDocument.isNull()){
			QByteArray errorMessage = QString("Unable to convert message to JSON: '%1'").arg(qPrintable(jsonError.errorString())).toUtf8();
			qDebug() << errorMessage;

			locker.unlock();
			return CreateErrorResponse(errorMessage, request);
		}

		QJsonObject rootObject = jsonDocument.object();

		switch (webSocketRequest->GetMethodType())
		{
		case imtrest::CWebSocketRequest::MT_CONNECTION_ACK:
			for (const QByteArray& subscriptionId : m_registeredClients.keys()){
				if (m_registeredClients[subscriptionId].m_status == IGqlSubscriptionClient::SS_IN_REGISTRATION){
					istd::IChangeableUniquePtr objectPtr(m_registeredClients[subscriptionId].m_request.CloneMe());
					locker.unlock();
					auto requestPtr = dynamic_cast<imtgql::CGqlRequest*>(objectPtr.GetPtr());
					if (requestPtr != nullptr){
						SubscriptionRegister(*requestPtr, subscriptionId);
					}
					locker.relock();
				}
			}
		break;

		case imtrest::CWebSocketRequest::MT_START_ACK:{
			QByteArray subscriptionId = rootObject.value("id").toString().toLocal8Bit();
			if (m_registeredClients.contains(subscriptionId)){
				m_registeredClients[subscriptionId].m_status = IGqlSubscriptionClient::SS_REGISTERED;

				UpdateCustomerSubscriptionStatuses(subscriptionId, message);
			}
		}
		break;

		case imtrest::CWebSocketRequest::MT_DATA:{
			QByteArray subscriptionId = rootObject.value("id").toString().toLocal8Bit();
			if (m_registeredClients.contains(subscriptionId)){
				for (IGqlSubscriptionClient* subscriptionClientPtr : m_registeredClients[subscriptionId].m_clients){
					if (subscriptionClientPtr != nullptr){
						if (!rootObject.contains("payload")){
							break;
						}

						QJsonObject payloadObject = rootObject.value("payload").toObject().value("data").toObject();

						QJsonDocument document;
						document.setObject(payloadObject);

						QByteArray payload = document.toJson(QJsonDocument::Compact);

						locker.unlock();
						subscriptionClientPtr->OnResponseReceived(subscriptionId, payload);
						locker.relock();
					}
				}
			}
		}
		break;

		case imtrest::CWebSocketRequest::MT_QUERY_DATA:{
			const QString key = QString::fromUtf8(webSocketRequest->GetQueryId());
			// Envelope is {"type":"query_data","id":"...","payload":{...GQL JSON...}}.
			// ParseModelResponse expects the GraphQL body only.
			QByteArray body = webSocketRequest->GetBody();
			{
				const QJsonDocument envelope = QJsonDocument::fromJson(body);
				if (envelope.isObject()){
					const QJsonValue payloadVal = envelope.object().value(QStringLiteral("payload"));
					if (payloadVal.isObject()){
						body = QJsonDocument(payloadVal.toObject()).toJson(QJsonDocument::Compact);
					}
					else if (payloadVal.isString()){
						body = payloadVal.toString().toUtf8();
					}
				}
			}
			locker.unlock();
			CompletePending(key, body, false);
		}
		break;

		case imtrest::CWebSocketRequest::MT_ERROR:
		{
			const QString key = QString::fromUtf8(webSocketRequest->GetQueryId());
			QByteArray body = webSocketRequest->GetBody();
			{
				const QJsonDocument envelope = QJsonDocument::fromJson(body);
				if (envelope.isObject()){
					const QJsonValue payloadVal = envelope.object().value(QStringLiteral("payload"));
					if (payloadVal.isObject()){
						body = QJsonDocument(payloadVal.toObject()).toJson(QJsonDocument::Compact);
					}
					else if (payloadVal.isString()){
						body = payloadVal.toString().toUtf8();
					}
					else if (payloadVal.isArray()){
						body = QJsonDocument(payloadVal.toArray()).toJson(QJsonDocument::Compact);
					}
				}
			}
			locker.unlock();
			// Deliver the body so async callers can parse GraphQL errors.
			CompletePending(key, body, true);
		}
			break;

		default:
			{
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


// reimplemented (IAsyncGqlClient)

QFuture<CSubscriptionManagerComp::GqlResult> CSubscriptionManagerComp::SendRequest(
			GqlRequestPtr requestPtr,
			imtbase::IUrlParam* /*urlParamPtr*/) const
{
	auto promisePtr = std::make_shared<QPromise<GqlResult>>();
	promisePtr->start();
	QFuture<GqlResult> future = promisePtr->future();

	auto FailFast = [promisePtr](ErrorCategory category, const QString& message) {
		promisePtr->addResult(GqlResult{GqlResponsePtr(), category, message});
		promisePtr->finish();
	};

	if (!requestPtr.IsValid()){
		FailFast(EC_INVALID_REQUEST, "Invalid request");
		return future;
	}

	if (!m_engineCompPtr.IsValid()){
		FailFast(EC_INTERNAL, "Protocol engine is not available");
		return future;
	}

	const QString key = QUuid::createUuid().toString(QUuid::WithoutBraces);

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
			if (headerId != "accept-encoding" && headerId != imtbase::s_authenticationTokenHeaderId){
				headersObject[headerId] = QString(headers.value(headerId));
			}
		}
	}
	dataObject["headers"] = headersObject;

	const QByteArray queryData = QJsonDocument(dataObject).toJson(QJsonDocument::Compact);
	imtrest::ConstRequestPtr constRequestPtr(
				m_engineCompPtr->CreateRequestForSend(*this, 0, queryData, "").PopInterfacePtr());

	{
		QMutexLocker lock(&m_pendingAsyncMutex);
		PendingAsync pending;
		pending.promisePtr = promisePtr;
		pending.requestPtr = requestPtr;
		m_pendingAsync.insert(key, pending);
	}

	// QFuture::cancel drops the pending entry so a late response is ignored.
	// The watcher must live on this component's thread (needs its event loop);
	// SendRequest may be called from a worker thread, so arm it via the same
	// invokeMethod pattern as the timeout below.
	{
		auto* self = const_cast<CSubscriptionManagerComp*>(this);
		const auto armCancelWatcher = [self, key, future]() {
			auto* watcherPtr = new QFutureWatcher<GqlResult>(self);
			QObject::connect(watcherPtr, &QFutureWatcherBase::canceled, self, [self, key]() {
				self->FailPending(key, EC_NONE, QString());
			});
			QObject::connect(watcherPtr, &QFutureWatcherBase::finished, watcherPtr, &QObject::deleteLater);
			watcherPtr->setFuture(future);
		};
		if (QThread::currentThread() == self->thread()){
			armCancelWatcher();
		}
		else{
			QMetaObject::invokeMethod(self, armCancelWatcher, Qt::QueuedConnection);
		}
	}

	if (!SendRequestInternal(*requestPtr, constRequestPtr)){
		FailPending(key, EC_NETWORK,
					QString("Request could not be sent: '%1'").arg(QString(requestPtr->GetCommandId())));
		return future;
	}

	// Timeout without nested loop for async callers. Arm the QTimer on this
	// component's thread only — QTimer::singleShot(ctx) constructs a temporary
	// QObject parented to ctx; doing that from a CWorkerThread while ctx lives
	// on the app thread triggers:
	//   QObject: Cannot create children for a parent that is in a different thread
	if (m_requestTimeoutMs > 0){
		const int timeoutMs = m_requestTimeoutMs;
		auto* self = const_cast<CSubscriptionManagerComp*>(this);
		const auto armTimeout = [self, key, timeoutMs]() {
			QTimer::singleShot(timeoutMs, self, [self, key]() {
				QMutexLocker lock(&self->m_pendingAsyncMutex);
				if (!self->m_pendingAsync.contains(key)){
					return;
				}
				lock.unlock();
				self->FailPending(key, EC_TIMEOUT, "Request timed out");
			});
		};
		if (QThread::currentThread() == self->thread()){
			armTimeout();
		}
		else{
			QMetaObject::invokeMethod(self, armTimeout, Qt::QueuedConnection);
		}
	}

	return future;
}


// protected methods

bool CSubscriptionManagerComp::SubscriptionRegister(const imtgql::CGqlRequest& subscriptionRequest, const QByteArray& subscriptionId) const
{
	if (!m_engineCompPtr.IsValid()){
		Q_ASSERT(0);

		return false;
	}

	QString authToken;

	QByteArray endpoint;
	QUrl url(endpoint);
	QString host = url.host();

	QJsonObject authorization;
	authorization["Authorization"] = authToken;
	authorization["host"] = host;

	QJsonObject extensions;
	extensions["authorization"] = authorization;

	QJsonObject payload;
	payload["data"] = QString(subscriptionRequest.GetQuery());
	payload["extensions"] = extensions;

	QJsonObject registerSubscription;
	registerSubscription["id"] = QString(subscriptionId);
	registerSubscription["type"] = "start";
	registerSubscription["payload"] = payload;

	QJsonObject headersObject;
	const imtgql::IGqlContext* contextPtr = subscriptionRequest.GetRequestContext();
	if (contextPtr != nullptr){
		imtgql::IGqlContext::Headers headers = contextPtr->GetHeaders();
		for (const QByteArray& headerId : headers.keys()){
			if (headerId != "accept-encoding"){
				headersObject[headerId] = QString(headers.value(headerId));
			}
		}
	}
	registerSubscription["headers"] = headersObject;

	QByteArray queryData = QJsonDocument(registerSubscription).toJson(QJsonDocument::Compact);

	imtrest::ConstRequestPtr requestPtr(m_engineCompPtr->CreateRequestForSend(*this, 0, queryData, "").PopInterfacePtr());

	return SendRequestInternal(subscriptionRequest, requestPtr);
}


bool CSubscriptionManagerComp::SendRequestInternal(const imtgql::IGqlRequest& request, imtrest::ConstRequestPtr& requestPtr) const
{
	bool retVal = false;
	QByteArray clientId;

	auto requestImplPtr = dynamic_cast<const imtgql::CGqlRequest*>(&request);
	if (requestImplPtr != nullptr){
		clientId = requestImplPtr->GetHeader("clientid");
	}

	if (m_subscriptionSenderCompPtr.IsValid()){
		retVal = m_subscriptionSenderCompPtr->SendRequest(requestPtr);
	}
	else if (m_requestManagerCompPtr.IsValid()){
		if (clientId.isEmpty()){
			SendErrorMessage(
				0,
				QStringLiteral("Outbound WebSocket request has empty clientid — cannot route to recipient"),
				"SubscriptionManager");
			return false;
		}
		retVal = m_requestManagerCompPtr->SendRequest(clientId, requestPtr);
		if (!retVal){
			SendErrorMessage(
				0,
				QStringLiteral("No WebSocket sender registered for clientid '%1' (client offline or id mismatch)")
				.arg(QString::fromUtf8(clientId)),
				"SubscriptionManager");
		}
	}

	return retVal;
}


// reimplemented (icomp::CComponentBase)

void CSubscriptionManagerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_requestTimeoutMsAttrPtr.IsValid() && *m_requestTimeoutMsAttrPtr > 0){
		m_requestTimeoutMs = *m_requestTimeoutMsAttrPtr;
	}

	if (m_connectionStatusProviderModelCompPtr.IsValid()){
		m_connectionStatusProviderModelCompPtr->AttachObserver(this);
	}
}


// private methods

void CSubscriptionManagerComp::UpdateCustomerSubscriptionStatuses(const QByteArray& subscriptionId, const QString& message) const
{
	if (m_registeredClients.contains(subscriptionId)){
		for (IGqlSubscriptionClient* subscriptionClientPtr : m_registeredClients[subscriptionId].m_clients){
			if (subscriptionClientPtr != nullptr){
				subscriptionClientPtr->OnSubscriptionStatusChanged(subscriptionId, m_registeredClients[subscriptionId].m_status, message);
			}
		}
	}
}


imtrest::ConstResponsePtr CSubscriptionManagerComp::CreateErrorResponse(const QByteArray& errorMessage, const imtrest::IRequest& request) const
{
	QByteArray requestBody = request.GetBody();
	QJsonDocument document = QJsonDocument::fromJson(requestBody);
	QJsonObject object = document.object();

	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();

	QString body = QString(R"({"id": "%1","type": "error","payload": [ {"message": "%2", "extensions": { "type": "Warning" }} ]})")
		.arg(object["id"].toString())
		.arg(errorMessage);

	QByteArray responseTypeId("text/html; charset=utf-8");
	imtrest::ConstResponsePtr responsePtr(
		engine.CreateResponse(
			request,
			imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE,
			body.toUtf8(),
			responseTypeId).PopInterfacePtr());

	SendErrorMessage(0, QString(errorMessage));

	return responsePtr;
}


void CSubscriptionManagerComp::CompletePending(const QString& key, const QByteArray& body, bool isError) const
{
	PendingAsync pending;
	{
		QMutexLocker lock(&m_pendingAsyncMutex);
		if (!m_pendingAsync.contains(key)){
			return;
		}
		pending = m_pendingAsync.take(key);
	}

	if (pending.promisePtr->future().isFinished()){
		return;
	}

	auto* responseRawPtr = new imtgql::CGqlResponse(pending.requestPtr);
	responseRawPtr->SetResponseData(body);
	IAsyncGqlClient::GqlResponsePtr responsePtr;
	responsePtr.SetPtr(responseRawPtr);

	// query_data is delivered on the WebSocket socket thread (CWebSocketThread).
	// Completing the promise there could run continuations which re-enter the
	// same socket while m_isProcessingMessage is true. Always hop to this
	// component's thread first.
	//
	// Taking the map entry above is the single atomic terminalization point:
	// a racing FailPending (cancel watcher / timeout) finds no entry and does
	// nothing, so this path must always finish the future. Cancellation may
	// also occur after take() but before this lambda runs, so it re-checks the
	// canceled flag and finishes without a result — otherwise
	// cancel(); waitForFinished() callers would block forever.
	auto deliver = [pending, responsePtr, isError]() mutable {
		if (pending.promisePtr->future().isFinished()){
			return;
		}
		if (pending.promisePtr->isCanceled()){
			pending.promisePtr->finish();
			return;
		}
		// isError: still deliver body (may contain GraphQL errors).
		Q_UNUSED(isError);
		pending.promisePtr->addResult(GqlResult{responsePtr, EC_NONE, QString()});
		pending.promisePtr->finish();
	};

	CSubscriptionManagerComp* self = const_cast<CSubscriptionManagerComp*>(this);
	const bool queued = QMetaObject::invokeMethod(self, deliver, Qt::QueuedConnection);

	if (!queued){
		deliver();
	}
}


void CSubscriptionManagerComp::FailPending(
			const QString& key,
			ErrorCategory category,
			const QString& message) const
{
	PendingAsync pending;
	{
		QMutexLocker lock(&m_pendingAsyncMutex);
		if (!m_pendingAsync.contains(key)){
			return;
		}
		pending = m_pendingAsync.take(key);
	}

	if (pending.promisePtr->future().isFinished()){
		return;
	}

	// Same atomic terminalization as CompletePending: the entry was taken above,
	// so this path must finish the future. Re-check cancellation in the queued
	// lambda so a cancel racing with e.g. a timeout still yields EC_CANCELLED.
	auto deliver = [pending, category, message]() mutable {
		if (pending.promisePtr->future().isFinished()){
			return;
		}
		const bool cancelled = pending.promisePtr->isCanceled();
		if (!cancelled){
			pending.promisePtr->addResult(GqlResult{GqlResponsePtr(), category, message});
		}
		pending.promisePtr->finish();
	};

	CSubscriptionManagerComp* self = const_cast<CSubscriptionManagerComp*>(this);
	const bool queued = QMetaObject::invokeMethod(self, deliver, Qt::QueuedConnection);

	if (!queued){
		deliver();
	}
}


} // namespace imtclientgql
