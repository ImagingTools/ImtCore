#include <imtclientgql/CSubscriberCompBase.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtclientgql
{


// protected methods

void CSubscriberCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_subscriptionManagerCompPtr.IsValid()){
		imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION, *m_commandAttrPtr);

		imtgql::CGqlObject subscriptionField("data");
		subscriptionField.InsertField("id");
		gqlRequest.AddField(subscriptionField);

		m_subscriptionId = m_subscriptionManagerCompPtr->RegisterSubscription(gqlRequest, this);
	}
}


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


} // namespace gmgaws


