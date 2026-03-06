#pragma once

// ACF includes
#include <istd/IChangeable.h>


namespace imtservergql
{


class ICommandIdSubscriptionsMonitor: virtual public istd::IChangeable
{
public:
	enum ChangeFlags
	{
		CF_SUBSCRIPTION_ADDED = 0xCEEB12A0,
		CF_SUBSCRIPTIONS_REMOVED_FOR_COMMAND_ID = 0xCEEB1BA1
	};

	inline static const QByteArray CN_SUBSCRIPTION_ADDED =
		QByteArrayLiteral("imtservergql::ICommandIdSubscriptionsMonitor::CN_SUBSCRIPTION_ADDED");
	inline static const QByteArray CN_SUBSCRIPTIONS_REMOVED_FOR_COMMAND_ID =
		QByteArrayLiteral("imtservergql::ICommandIdSubscriptionsMonitor::CN_SUBSCRIPTIONS_REMOVED_FOR_COMMAND_ID");

	struct SubscriptionInfo {
		QByteArray commandId;
		QByteArray subscribedInputId;
	};

	virtual void SubscriptionAdded(
		const QByteArray& subscriptionId, const QByteArray& commandId, const QByteArray& subscribedInputId) = 0;
	virtual void SubscriptionRemoved(const QByteArray& subscriptionId) = 0;
};


} // namespace imtservergql
