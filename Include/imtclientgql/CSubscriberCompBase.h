#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtclientgql/IGqlSubscriptionManager.h>


namespace imtclientgql
{


class CSubscriberCompBase:
			public ilog::CLoggerComponentBase,
			virtual public imtclientgql::IGqlSubscriptionClient
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSubscriberCompBase);
		I_REGISTER_INTERFACE(imtclientgql::IGqlSubscriptionClient);
		I_ASSIGN(m_subscriptionManagerCompPtr, "SubscriptionManager", "Subscription manager", true, "SubscriptionManager");
		I_ASSIGN(m_commandAttrPtr, "Command", "Command for subscription", true, "");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (imtgql::IGqlSubscriptionClient)
	virtual void OnResponseReceived(
				const QByteArray& subscriptionId,
				const QByteArray& subscriptionData) override;
	virtual void OnSubscriptionStatusChanged(
				const QByteArray& subscriptionId,
				const SubscriptionStatus& status,
				const QString& message) override;

protected:
	QByteArray m_subscriptionId;

protected:
	I_REF(imtclientgql::IGqlSubscriptionManager, m_subscriptionManagerCompPtr);
	I_ATTR(QByteArray, m_commandAttrPtr);
};


} // namespace imtclientgql


