// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CPublisherSubscriberBridgeComp.h>


// Qt includes
#include <QJsonDocument>
#include <QJsonObject>

// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtservergql
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CPublisherSubscriberBridgeComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_subscriptionManagerCompPtr.IsValid()){
		for (int i = 0; i < m_commandIdsAttrPtr.GetCount(); i++){
			imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION, m_commandIdsAttrPtr[i]);

			imtgql::CGqlFieldObject subscriptionField;
			subscriptionField.InsertField("id");
			gqlRequest.AddField("data", subscriptionField);

			m_subscriptionIds << m_subscriptionManagerCompPtr->RegisterSubscription(gqlRequest, this);
		}
	}
}


void CPublisherSubscriberBridgeComp::OnComponentDestroyed()
{
	if (m_subscriptionManagerCompPtr.IsValid()){
		for (const QByteArray& subscriptionId : m_subscriptionIds){
			m_subscriptionManagerCompPtr->UnregisterSubscription(subscriptionId);
		}
	}
	m_subscriptionIds.clear();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imtclientgql::IGqlSubscriptionClient)

void CPublisherSubscriberBridgeComp::OnResponseReceived(
			const QByteArray& subscriptionId,
			const QByteArray& subscriptionData)
{
	QByteArray commandId = GetCommandForSubscription(subscriptionId);
	if (commandId.isEmpty()){
		return;
	}

	// The subscriptionData arrives as {"commandId": <innerPayload>} from the subscription manager.
	// PublishData wraps data with commandId again, so we need to extract the inner payload
	// to avoid double-wrapping like {"commandId": {"commandId": {...}}}.
	QJsonDocument jsonDoc = QJsonDocument::fromJson(subscriptionData);
	if (jsonDoc.isObject()){
		QJsonObject jsonObject = jsonDoc.object();
		QJsonValue innerValue = jsonObject.value(QString::fromUtf8(commandId));
		if (!innerValue.isUndefined()){
			QJsonDocument innerDoc;
			if (innerValue.isObject()){
				innerDoc.setObject(innerValue.toObject());
			}
			else{
				innerDoc.setObject(jsonObject);
			}
			PublishData(commandId, innerDoc.toJson(QJsonDocument::Compact));
			return;
		}
	}

	PublishData(commandId, subscriptionData);
}


void CPublisherSubscriberBridgeComp::OnSubscriptionStatusChanged(
			const QByteArray& /*subscriptionId*/,
			const SubscriptionStatus& /*status*/,
			const QString& /*message*/)
{
}


// private methods

QByteArray CPublisherSubscriberBridgeComp::GetCommandForSubscription(const QByteArray& subscriptionId) const
{
	int index = m_subscriptionIds.indexOf(subscriptionId);
	if (index < 0){
		return QByteArray();
	}

	if (index >= m_commandIdsAttrPtr.GetCount()){
		return QByteArray();
	}

	return m_commandIdsAttrPtr[index];
}


} // namespace imtservergql


