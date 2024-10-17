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


// reimplemented (icomp::CComponentBase)

void CSubscriberCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_subscriptionManagerCompPtr.IsValid()){
		if (m_commandAttrPtr.GetCount() > 0){
			imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION, m_commandAttrPtr[0]);

			imtgql::CGqlObject subscriptionField;
			subscriptionField.InsertField("id");
			gqlRequest.AddField("data", subscriptionField);

			m_subscriptionId = m_subscriptionManagerCompPtr->RegisterSubscription(gqlRequest, this);
		}
	}
}


} // namespace gmgaws


