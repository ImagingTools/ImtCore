#include <imtclientgql/CSubscriberCompBase.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtclientgql
{


// protected methods

// reimplemented (imtgql::IGqlSubscriptionClient)

void CSubscriberCompBase::OnResponseReceived(
			const QByteArray& subscriptionId,
			const QByteArray& subscriptionData)
{
	qDebug() << "OnResponseReceived" << subscriptionId << subscriptionData;
}


void CSubscriberCompBase::OnSubscriptionStatusChanged(
			const QByteArray& /*subscriptionId*/,
			const SubscriptionStatus& /*status*/,
			const QString& /*message*/)
{
	qDebug() << "OnSubscriptionStatusChanged";
}


// reimplemented (icomp::CComponentBase)

void CSubscriberCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_subscriptionManagerCompPtr.IsValid()){
		imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION, *m_commandAttrPtr);

		imtgql::CGqlObject subscriptionField;
		subscriptionField.InsertField("id");
		gqlRequest.AddField("data", subscriptionField);

		m_subscriptionId = m_subscriptionManagerCompPtr->RegisterSubscription(gqlRequest, this);
	}
}


} // namespace gmgaws


