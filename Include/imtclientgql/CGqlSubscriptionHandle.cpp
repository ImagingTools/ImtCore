#include <imtclientgql/CGqlSubscriptionHandle.h>


// ImtCore includes
#include <imtgql/IGqlRequest.h>


namespace imtclientgql
{


CGqlSubscriptionHandle::CGqlSubscriptionHandle(IGqlSubscriptionManager& manager, const imtgql::IGqlRequest& request)
	: m_manager(manager)
{
	m_subscriptionId = m_manager.RegisterSubscription(request, *this);
}


CGqlSubscriptionHandle::~CGqlSubscriptionHandle()
{
	Unregister();
}


void CGqlSubscriptionHandle::ScheduleDestruction()
{
	Unregister();
	deleteLater();
}


// protected methods

// reimplemented (imtclientgql::IGqlSubscriptionClient)

void CGqlSubscriptionHandle::OnResponseReceived(
	const QByteArray& /*subscriptionId*/, const QByteArray& subscriptionData)
{
	Q_EMIT payloadReceived(subscriptionData);
}


void CGqlSubscriptionHandle::OnSubscriptionStatusChanged(
	const QByteArray& /*subscriptionId*/, const SubscriptionStatus& status, const QString& message)
{
	Q_EMIT statusChanged(status, message);
}


// private methods

void CGqlSubscriptionHandle::Unregister()
{
	if (m_subscriptionId.isEmpty()) {
		return;
	}

	m_manager.UnregisterSubscription(m_subscriptionId, *this);
	m_subscriptionId.clear();
}


} // namespace imtclientgql
