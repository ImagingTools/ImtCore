// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CSubscriberCompBase.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtclientgql
{


// protected methods

// reimplemented (imtgql::IGqlSubscriptionClient)

void CSubscriberCompBase::OnResponseReceived(
			const QByteArray& /*subscriptionId*/,
			const QByteArray& /*subscriptionData*/)
{
}


void CSubscriberCompBase::OnSubscriptionStatusChanged(
			const QByteArray& /*subscriptionId*/,
			const SubscriptionStatus& /*status*/,
			const QString& /*message*/)
{
}


QByteArray CSubscriberCompBase::GetCommandId(const QByteArray& subscriptionId) const
{
	int index = m_subscriptionIds.indexOf(subscriptionId);
	if (index < 0){
		return QByteArray();
	}

	if (index >= m_commandAttrPtr.GetCount()){
		return QByteArray();
	}

	return m_commandAttrPtr[index];
}


// reimplemented (icomp::CComponentBase)

void CSubscriberCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_subscriptionManagerCompPtr.IsValid()){
		for (int i = 0; i < m_commandAttrPtr.GetCount(); i++){
			imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION, m_commandAttrPtr[i]);

			imtgql::CGqlFieldObject subscriptionField;
			subscriptionField.InsertField("id");
			gqlRequest.AddField("data", subscriptionField);

			m_subscriptionIds << m_subscriptionManagerCompPtr->RegisterSubscription(gqlRequest, this);
		}
	}
}


} // namespace imtclientgql


