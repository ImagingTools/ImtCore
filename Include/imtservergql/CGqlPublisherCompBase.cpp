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
		errorMessage = QString("Request with command-ID: '%1 'is not supported").arg(qPrintable(gqlRequest.GetCommandId()));
		SendErrorMessage(0, errorMessage, "CGqlPublisherCompBase");

		return false;
	}

	const imtrest::CWebSocketRequest* constWebSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&networkRequest);
	imtrest::CWebSocketRequest* webSocketRequest = dynamic_cast<imtrest::CWebSocketRequest*>(const_cast<imtrest::CWebSocketRequest*>(constWebSocketRequest));
	if (webSocketRequest == nullptr){
		errorMessage = QString("Internal error");
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
	if (request == nullptr){
		return;
	}

	// Remove all subscription entries that still reference the destroyed network request.
	// Matching by pointer (instead of only by query-ID) guarantees that no dangling
	// request pointers remain registered after a connection loss.
	QMutexLocker locker(&m_mutex);

	for (int i = m_registeredSubscribers.size() - 1; i >= 0; --i){
		QMap<QByteArray, const imtrest::IRequest*>& networkRequests = m_registeredSubscribers[i].networkRequests;

		for (auto it = networkRequests.begin(); it != networkRequests.end();){
			if (it.value() == request){
				it = networkRequests.erase(it);
			}
			else{
				++it;
			}
		}

		if (networkRequests.isEmpty()){
			m_registeredSubscribers.removeAt(i);
		}
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
			.arg(qPrintable(subscriptionId))
			.arg(qPrintable(commandId))
			.arg(qPrintable(data)).toUtf8();

	QByteArray reponseTypeId = QByteArray("application/json; charset=utf-8");
	const imtrest::IProtocolEngine& engine = networkRequest.GetProtocolEngine();

	imtrest::ConstResponsePtr responsePtr(engine.CreateResponse(networkRequest, imtrest::IProtocolEngine::SC_OK, body, reponseTypeId).PopInterfacePtr());
	if (!responsePtr.IsValid()){
		SendErrorMessage(0, QString("Unable to send response to subscriber. Error: Response is invalid"), "CGqlPublisherCompBase");
		return false;
	}

	bool retVal = m_requestManagerCompPtr->SendResponse(networkRequest.GetRequestId(), responsePtr);
	if (!retVal){
		QString message = QString("Unable to send response to subscriber. Data: '%1'").arg(qPrintable(data));
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
	struct Target { QByteArray id; const imtrest::IRequest* req; };
	QList<Target> targets;

	// The mutex must stay locked while pushing the data: OnRequestDestroyed synchronizes
	// on the same mutex, so a network request cannot be destroyed (e.g. on connection loss
	// in another thread) while it is dereferenced here. Sending is non-blocking (the
	// dispatcher only queues the data), so holding the lock is cheap.
	QMutexLocker locker(&m_mutex);

	for (const auto& entry : m_registeredSubscribers) {
		if (entry.gqlRequest.GetCommandId() == commandId) {

			// Apply filtering (predicate) to this subscriber's unique variables
			if (!predicate || predicate(entry.gqlRequest)) {
				for (auto it = entry.networkRequests.constBegin(); it != entry.networkRequests.constEnd(); ++it) {
					targets.append({it.key(), it.value()});
				}
			}
		}
	}

	for (const auto& target : targets) {
		bool retVal = PushDataToSubscriber(target.id, commandId, data, *target.req);

		if (!retVal){
			QString message = QString("Unable to notify subscriber about the changes. Subscription-ID: '%1', '%2'").arg(qPrintable(commandId), qPrintable(data));
			SendErrorMessage(0, message, "CGqlPublisherCompBase");
		}
	}

	return true;
}


} // namespace imtservergql


