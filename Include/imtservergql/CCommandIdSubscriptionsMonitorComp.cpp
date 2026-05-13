#include <imtservergql/CCommandIdSubscriptionsMonitorComp.h>



namespace imtservergql
{


// reimplemented (ICommandIdSubscriptionsMonitor)
void CCommandIdSubscriptionsMonitorComp::SubscriptionAdded(
	const QByteArray& subscriptionId, const QByteArray& commandId, const QByteArray& subscribedInputId)
{
	QByteArray subbedId = subscribedInputId.isEmpty() ? commandId : subscribedInputId;

	m_subscribers[commandId][subbedId].append(subscriptionId);

	ICommandIdSubscriptionsMonitor::SubscriptionInfo info = {commandId, subbedId};

	SendNotification(
		ICommandIdSubscriptionsMonitor::ChangeFlags::CF_SUBSCRIPTION_ADDED,
		CN_SUBSCRIPTION_ADDED,
		QVariant::fromValue(info));
}


void CCommandIdSubscriptionsMonitorComp::SubscriptionRemoved(const QByteArray& subscriptionId)
{
	for (const auto& commandId : m_subscribers.keys()) {
		auto& subscribers = m_subscribers[commandId];

		for (const auto& subId : subscribers.keys()) {
			subscribers[subId].removeAll(subscriptionId);
			if (subscribers[subId].isEmpty()) {
				ICommandIdSubscriptionsMonitor::SubscriptionInfo info = {commandId, subId};
				SendNotification(
					ICommandIdSubscriptionsMonitor::ChangeFlags::CF_SUBSCRIPTIONS_REMOVED_FOR_COMMAND_ID,
					CN_SUBSCRIPTIONS_REMOVED_FOR_COMMAND_ID,
					QVariant::fromValue(info));
			}
		}
	}

	// Remove keys with empty values
	m_subscribers.removeIf([](auto it) {
		it.value().removeIf([](auto innerIt) {
			return innerIt.value().isEmpty();
		});

		return it.value().isEmpty();
	});
}


void CCommandIdSubscriptionsMonitorComp::SendNotification(int id, const QByteArray& changeNotifier, QVariant changeInfo)
{
	istd::IChangeable::ChangeSet changeSet(id);
	changeSet.SetChangeInfo(changeNotifier, changeInfo);

	istd::CChangeNotifier notifier(this, &changeSet);
}


} // namespace imtservergql
