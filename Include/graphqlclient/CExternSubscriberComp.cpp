#include <graphqlclient/CExternSubscriberComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace graphqlclient
{


// public methods

QByteArray CExternSubscriberComp::RegisterSubscribtion(const QByteArray& queryData,  ISubscriber& subscriber, QString& errorMessage)
{
	QByteArray retVal;
	if (m_subscriptionManagerCompPtr.IsValid()){
		imtgql::CGqlRequest gqlRequest;
		int errorPosition;

		if (gqlRequest.ParseQuery(queryData, errorPosition)){
			retVal = m_subscriptionManagerCompPtr->RegisterSubscription(gqlRequest, this);
			m_subscribtions.insert(retVal, &subscriber);
		}
	}

	return retVal;
}


bool CExternSubscriberComp::UnregisterSubscription(const QByteArray& subscriptionId, QString& errorMessage)
{
	if (m_subscribtions.contains(subscriptionId)){
		m_subscribtions.remove(subscriptionId);
		m_subscriptionManagerCompPtr->UnregisterSubscription(subscriptionId);

		return true;
	}

	return false;
}


// protected methods

// reimplemented (imtgql::IGqlSubscriptionClient)

void CExternSubscriberComp::OnResponseReceived(
			const QByteArray& subscriptionId,
			const QByteArray& subscriptionData)
{
	if (m_subscribtions.contains(subscriptionId)){
		m_subscribtions.value(subscriptionId)->OnSubscriptionReceived(subscriptionId, subscriptionData);
	}
}


void CExternSubscriberComp::OnSubscriptionStatusChanged(
			const QByteArray& subscriptionId,
			const SubscriptionStatus& status,
			const QString& message)
{
	if (m_subscribtions.contains(subscriptionId)){
		graphqlclient::ISubscriber::SubscriptionStatus externStatus = static_cast<graphqlclient::ISubscriber::SubscriptionStatus>(status);
		m_subscribtions.value(subscriptionId)->OnSubscriptionStatusChanged(subscriptionId, externStatus, message);
	}
}



} // namespace graphqlclient


