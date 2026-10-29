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


void CGqlSubscriptionHandle::Unregister()
{
	if (m_subscriptionId.isEmpty()) {
		return;
	}
	m_manager.UnregisterSubscription(m_subscriptionId, *this);
	m_subscriptionId.clear();
}


// protected methods

// reimplemented (imtclientgql::IGqlSubscriptionClient)

// Pure relays: they run on whichever thread the manager dispatches on and touch no member
// state, so the only cross-thread operation is the emission itself. Everything the handle
// owns is read and written on its own thread.

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


} // namespace imtclientgql
