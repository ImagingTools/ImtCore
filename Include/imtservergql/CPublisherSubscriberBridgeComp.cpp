// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CPublisherSubscriberBridgeComp.h>


// Qt includes
#include <QJsonDocument>
#include <QJsonObject>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>


namespace imtservergql
{


// public methods

// reimplemented (imtgql::IGqlSubscriberController via CGqlPublisherCompBase)

bool CPublisherSubscriberBridgeComp::RegisterSubscription(
			const QByteArray& subscriptionId,
			const imtgql::CGqlRequest& gqlRequest,
			const imtrest::IRequest& networkRequest,
			QString& errorMessage)
{
	if (!m_subscriptionManagerCompPtr.IsValid()){
		errorMessage = QStringLiteral("Subscription manager is not configured");
		return false;
	}

	// Register the client subscription locally (stores gqlRequest with context and networkRequest)
	if (!BaseClass::RegisterSubscription(subscriptionId, gqlRequest, networkRequest, errorMessage)){
		return false;
	}

	// Build the upstream subscription request using the client's gqlRequest
	// which already contains the user's IGqlContext (token, userId, tenantId)
	imtgql::CGqlRequest upstreamRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION, gqlRequest.GetCommandId());

	imtgql::CGqlFieldObject subscriptionField;
	subscriptionField.InsertField("id");
	upstreamRequest.AddField("data", subscriptionField);

	// Copy the GQL context from the client's request to the upstream request
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr != nullptr){
		istd::IChangeableUniquePtr clonedPtr = contextPtr->CloneMe();
		if (clonedPtr.IsValid()){
			imtgql::IGqlContextUniquePtr castedPtr;
			castedPtr.MoveCastedPtr(std::move(clonedPtr));
			upstreamRequest.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(castedPtr));
		}
	}

	// Register with the upstream subscription manager (forwards to remote server with user context)
	QByteArray upstreamSubscriptionId = m_subscriptionManagerCompPtr->RegisterSubscription(upstreamRequest, this);
	if (upstreamSubscriptionId.isEmpty()){
		BaseClass::UnregisterSubscription(subscriptionId);
		errorMessage = QStringLiteral("Failed to register upstream subscription");
		return false;
	}

	// Store the mapping between client and upstream subscriptions
	QMutexLocker locker(&m_bridgeMutex);
	m_clientToUpstreamMap.insert(subscriptionId, upstreamSubscriptionId);
	m_upstreamToClientsMap[upstreamSubscriptionId].append(subscriptionId);

	return true;
}


bool CPublisherSubscriberBridgeComp::UnregisterSubscription(const QByteArray& subscriptionId)
{
	QByteArray upstreamSubscriptionId;

	{
		QMutexLocker locker(&m_bridgeMutex);

		if (!m_clientToUpstreamMap.contains(subscriptionId)){
			return BaseClass::UnregisterSubscription(subscriptionId);
		}

		upstreamSubscriptionId = m_clientToUpstreamMap.take(subscriptionId);

		if (m_upstreamToClientsMap.contains(upstreamSubscriptionId)){
			m_upstreamToClientsMap[upstreamSubscriptionId].removeAll(subscriptionId);

			// If no more clients are using this upstream subscription, unregister it
			if (m_upstreamToClientsMap[upstreamSubscriptionId].isEmpty()){
				m_upstreamToClientsMap.remove(upstreamSubscriptionId);
			}
			else{
				// Other clients still use this upstream subscription, just remove local
				return BaseClass::UnregisterSubscription(subscriptionId);
			}
		}
	}

	// Unregister from the upstream subscription manager
	if (m_subscriptionManagerCompPtr.IsValid() && !upstreamSubscriptionId.isEmpty()){
		m_subscriptionManagerCompPtr->UnregisterSubscription(upstreamSubscriptionId);
	}

	return BaseClass::UnregisterSubscription(subscriptionId);
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CPublisherSubscriberBridgeComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


void CPublisherSubscriberBridgeComp::OnComponentDestroyed()
{
	if (m_subscriptionManagerCompPtr.IsValid()){
		QMutexLocker locker(&m_bridgeMutex);

		for (const QByteArray& upstreamSubscriptionId : m_upstreamToClientsMap.keys()){
			m_subscriptionManagerCompPtr->UnregisterSubscription(upstreamSubscriptionId);
		}

		m_clientToUpstreamMap.clear();
		m_upstreamToClientsMap.clear();
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imtclientgql::IGqlSubscriptionClient)

void CPublisherSubscriberBridgeComp::OnResponseReceived(
			const QByteArray& subscriptionId,
			const QByteArray& subscriptionData)
{
	QByteArrayList clientSubscriptionIds;

	{
		QMutexLocker locker(&m_bridgeMutex);
		if (!m_upstreamToClientsMap.contains(subscriptionId)){
			return;
		}
		clientSubscriptionIds = m_upstreamToClientsMap.value(subscriptionId);
	}

	if (clientSubscriptionIds.isEmpty()){
		return;
	}

	// Extract the inner payload to avoid double-wrapping
	QByteArray payload = subscriptionData;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(subscriptionData);
	if (jsonDoc.isObject()){
		QJsonObject jsonObject = jsonDoc.object();
		// Find the first object-valued key in the JSON and use it as commandId
		for (const QString& key : jsonObject.keys()){
			QJsonValue innerValue = jsonObject.value(key);
			if (innerValue.isObject()){
				QJsonDocument innerDoc;
				innerDoc.setObject(innerValue.toObject());
				payload = innerDoc.toJson(QJsonDocument::Compact);

				// Use the key as commandId for publishing to specific clients
				QByteArray commandId = key.toUtf8();

				QMutexLocker locker(&m_mutex);
				for (const QByteArray& clientSubscriptionId : clientSubscriptionIds){
					for (const RequestNetworks& entry : m_registeredSubscribers){
						if (entry.networkRequests.contains(clientSubscriptionId)){
							const imtrest::IRequest* networkRequestPtr = entry.networkRequests.value(clientSubscriptionId);
							if (networkRequestPtr != nullptr){
								PushDataToSubscriber(clientSubscriptionId, commandId, payload, *networkRequestPtr);
							}
							break;
						}
					}
				}
				return;
			}
		}
	}

	// Fallback: find commandId from registered subscribers and push raw data
	QMutexLocker locker(&m_mutex);
	for (const QByteArray& clientSubscriptionId : clientSubscriptionIds){
		for (const RequestNetworks& entry : m_registeredSubscribers){
			if (entry.networkRequests.contains(clientSubscriptionId)){
				QByteArray commandId = entry.gqlRequest.GetCommandId();
				const imtrest::IRequest* networkRequestPtr = entry.networkRequests.value(clientSubscriptionId);
				if (networkRequestPtr != nullptr){
					PushDataToSubscriber(clientSubscriptionId, commandId, payload, *networkRequestPtr);
				}
				break;
			}
		}
	}
}


void CPublisherSubscriberBridgeComp::OnSubscriptionStatusChanged(
			const QByteArray& /*subscriptionId*/,
			const SubscriptionStatus& /*status*/,
			const QString& /*message*/)
{
}


} // namespace imtservergql


