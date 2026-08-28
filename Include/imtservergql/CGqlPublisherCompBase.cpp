// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlPublisherCompBase.h>


// ACF includes
#include<istd/TDelPtr.h>

// ImtCore includes
#include<imtrest/IProtocolEngine.h>
#include<imtrest/ITransport.h>
#include<imtrest/CWebSocketRequest.h>


namespace imtservergql
{


// public methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CGqlPublisherCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	const imtgql::CGqlFieldObject& fieldsPtr = gqlRequest.GetFields();
	if (fieldsPtr.GetFieldIds().isEmpty()){
		return false;
	}

	QByteArray commandId = gqlRequest.GetCommandId();

	return m_commandIdsAttrPtr.FindValue(commandId) != -1;
}


bool CGqlPublisherCompBase::RegisterSubscription(
			const QByteArray& subscriptionId,
			const imtgql::CGqlRequest& gqlRequest,
			const imtrest::IRequest& networkRequest,
			QString& errorMessage)
{
	Q_ASSERT(IsRequestSupported(gqlRequest));

	if (!IsRequestSupported(gqlRequest)){
		errorMessage = QStringLiteral("Request with command-ID: '%1 'is not supported").arg(gqlRequest.GetCommandId());
		SendErrorMessage(0, errorMessage, "CGqlPublisherCompBase");

		return false;
	}

	const imtrest::CWebSocketRequest* constWebSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&networkRequest);
	imtrest::CWebSocketRequest* webSocketRequest = dynamic_cast<imtrest::CWebSocketRequest*>(const_cast<imtrest::CWebSocketRequest*>(constWebSocketRequest));
	if (webSocketRequest == nullptr){
		errorMessage = QStringLiteral("Internal error");
		SendErrorMessage(0, errorMessage, "CGqlPublisherCompBase");

		return false;
	}

	QMutexLocker locker(&m_mutex);

	// Duplicate check
	for (const RequestNetworks& entry : m_registeredSubscribers) {
		if (entry.networkRequests.contains(subscriptionId) && entry.networkRequests.value(subscriptionId) == &networkRequest) {
			errorMessage = QStringLiteral("Subscription ID already in use.");
			return false;
		}
	}


	// To support unique variables per user, every subscription MUST have its own RequestNetworks entry.
	RequestNetworks newEntry;
	newEntry.gqlRequest.CopyFrom(gqlRequest);
	newEntry.networkRequests.insert(subscriptionId, &networkRequest);

	m_registeredSubscribers.append(newEntry);

	webSocketRequest->RegisterRequestEventHandler(this);
	return true;
}


bool CGqlPublisherCompBase::UnregisterSubscription(const QByteArray& subscriptionId)
{
	QMutexLocker locker(&m_mutex);

	for (int i = 0; i < m_registeredSubscribers.size(); ++i) {
		if (m_registeredSubscribers[i].networkRequests.contains(subscriptionId)) {
			m_registeredSubscribers.removeAt(i);
			return true;
		}
	}
	return false;
}


// reimplemented (imtrest::IRequestEventHandler)

void CGqlPublisherCompBase::OnRequestDestroyed(imtrest::IRequest* request)
{
	imtrest::CWebSocketRequest* webSocketRequestPtr = dynamic_cast<imtrest::CWebSocketRequest*>(request);
	if (webSocketRequestPtr != nullptr){
		UnregisterSubscription(webSocketRequestPtr->GetQueryId());
	}
}


// protected methods

bool CGqlPublisherCompBase::PushDataToSubscriber(
			const QByteArray& subscriptionId,
			const QByteArray& commandId,
			const QByteArray& data,
			const imtrest::IRequest& networkRequest,
			const bool useAwsStyle) const
{
	if (!m_requestManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'RequestManager' was not set", "CGqlPublisherCompBase");
		return false;
	}

	const auto* wsRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&networkRequest);
	const bool isSubscribe = wsRequest && (wsRequest->GetMethodType() == imtrest::CWebSocketRequest::MT_SUBSCRIBE);

	QString typeId = "data";
	if (!useAwsStyle || isSubscribe){
		typeId = "next";
	}

	QByteArray body = QString(R"({"type": "%1","id": "%2","payload": {"data": {"%3": %4}}})")
			.arg(typeId)
			.arg(subscriptionId)
			.arg(commandId)
			.arg(data).toUtf8();

	QByteArray reponseTypeId = QByteArray("application/json; charset=utf-8");
	const imtrest::IProtocolEngine& engine = networkRequest.GetProtocolEngine();

	imtrest::ConstResponsePtr responsePtr(engine.CreateResponse(networkRequest, imtrest::IProtocolEngine::SC_OK, body, reponseTypeId).PopInterfacePtr());
	if (!responsePtr.IsValid()){
		SendErrorMessage(0, QStringLiteral("Unable to send response to subscriber. Error: Response is invalid"), "CGqlPublisherCompBase");
		return false;
	}

	bool retVal = m_requestManagerCompPtr->SendResponse(networkRequest.GetRequestId(), responsePtr);
	if (!retVal){
		QString message = QStringLiteral("Unable to send response to subscriber. Data: '%1'").arg(data);
		SendErrorMessage(0, message, "CGqlPublisherCompBase");
	}

	return true;
}


bool CGqlPublisherCompBase::PublishData(const QByteArray& commandId, const QByteArray& data) const
{
	return PublishDataFiltered(commandId, data, nullptr);
}


bool CGqlPublisherCompBase::PublishDataFiltered(
	const QByteArray& commandId,
	const QByteArray& data,
	std::function<bool(const imtgql::CGqlRequest&)> predicate) const
{
	// The push MUST run while holding m_mutex, not after releasing it. Each stored
	// const imtrest::IRequest* is a raw pointer to a CWebSocketRequest that is parented to its
	// QWebSocket and cascade-destroyed the instant that socket disconnects; CWebSocketRequest's dtor
	// calls OnRequestDestroyed() -> UnregisterSubscription(), which removes the entry under THIS same
	// m_mutex before the object's memory is freed. A previous version copied the raw pointers into a
	// local list, released the lock "to keep the server responsive", and dereferenced them afterwards -
	// a socket disconnect on another thread could then free a request in that gap, so *req became a
	// use-after-free (reproduced live: server crash under concurrent load / many workers, with the last
	// flushed log line being the WebSocket lifetime guard in CWebSocketServletComp). Publishing under
	// the lock closes that window: the dtor's UnregisterSubscription blocks on m_mutex until this push
	// completes, so every request stays alive for the whole call. This is deadlock-free - the actual
	// send in PushDataToSubscriber is a non-blocking emit() over a Qt::QueuedConnection to the socket
	// thread (CWebSocketSender), never a blocking cross-thread wait - and it matches how the sibling
	// CPublisherSubscriberBridgeComp already publishes (push while holding the same mutex).
	QMutexLocker locker(&m_mutex);

	for (const auto& entry : m_registeredSubscribers) {
		if (entry.gqlRequest.GetCommandId() != commandId) {
			continue;
		}

		// Apply filtering (predicate) to this subscriber's unique variables
		if (predicate && !predicate(entry.gqlRequest)) {
			continue;
		}

		for (auto it = entry.networkRequests.constBegin(); it != entry.networkRequests.constEnd(); ++it) {
			const imtrest::IRequest* networkRequestPtr = it.value();
			if (networkRequestPtr == nullptr) {
				continue;
			}

			bool retVal = PushDataToSubscriber(it.key(), commandId, data, *networkRequestPtr);
			if (!retVal){
				QString message = QStringLiteral("Unable to notify subscriber about the changes. Subscription-ID: '%1', '%2'").arg(commandId, data);
				SendErrorMessage(0, message, "CGqlPublisherCompBase");
			}
		}
	}

	return true;
}


} // namespace imtservergql


