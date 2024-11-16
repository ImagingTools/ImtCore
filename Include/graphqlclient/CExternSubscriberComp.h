#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtclientgql/IGqlSubscriptionManager.h>
#include <graphqlclient/graphqlclient.h>


namespace graphqlclient
{


class CExternSubscriberComp:
			public ilog::CLoggerComponentBase,
			virtual public imtclientgql::IGqlSubscriptionClient
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CExternSubscriberComp);
		I_REGISTER_INTERFACE(imtclientgql::IGqlSubscriptionClient);
		I_ASSIGN(m_subscriptionManagerCompPtr, "SubscriptionManager", "Subscription manager", true, "SubscriptionManager");
	I_END_COMPONENT;

public:
	QByteArray RegisterSubscribtion(const QByteArray& queryData, ISubscriber& subscriber,QString& errorMessage);
	bool UnregisterSubscription(const QByteArray& subscriptionId, QString& errorMessage);
protected:
	// reimplemented (imtgql::IGqlSubscriptionClient)
	virtual void OnResponseReceived(
				const QByteArray& subscriptionId,
				const QByteArray& subscriptionData) override;
	virtual void OnSubscriptionStatusChanged(
				const QByteArray& subscriptionId,
				const SubscriptionStatus& status,
				const QString& message) override;

protected:
	QMap<QByteArray, graphqlclient::ISubscriber*> m_subscriptions; // <SubscriptionId, ISubscriberPtr>

protected:
	I_REF(imtclientgql::IGqlSubscriptionManager, m_subscriptionManagerCompPtr);
};


} // namespace graphqlclient


