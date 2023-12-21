#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtclientgql/IGqlSubscriptionManager.h>


namespace imtclientgql
{


template <class Base>
class TSubscriberCompWrap:
			public Base,
			virtual public imtclientgql::IGqlSubscriptionClient
{
public:
	typedef Base BaseClass;

	I_BEGIN_COMPONENT(TSubscriberCompWrap);
		I_REGISTER_INTERFACE(imtclientgql::IGqlSubscriptionClient);
		I_ASSIGN(m_subscriptionManagerCompPtr, "SubscriptionManager", "Subscription manager", true, "SubscriptionManager");
		I_ASSIGN(m_commandAttrPtr, "Command", "Command for subscription", true, "");
	I_END_COMPONENT;

protected:
	virtual void RegisterSubscription();

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


// protected methods

template<class Base>
void TSubscriberCompWrap<Base>::RegisterSubscription()
{
	if (m_subscriptionManagerCompPtr.IsValid()){
		imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION, *m_commandAttrPtr);

		imtgql::CGqlObject subscriptionField("data");
		subscriptionField.InsertField("id");
		gqlRequest.AddField(subscriptionField);

		m_subscriptionId = m_subscriptionManagerCompPtr->RegisterSubscription(gqlRequest, this);
	}
}


// reimplemented (imtgql::IGqlSubscriptionClient)

template<class Base>
void TSubscriberCompWrap<Base>::OnResponseReceived(
			const QByteArray& subscriptionId,
			const QByteArray& subscriptionData)
{
}


template<class Base>
void TSubscriberCompWrap<Base>::OnSubscriptionStatusChanged(
			const QByteArray& subscriptionId,
			const SubscriptionStatus& status,
			const QString& message)
{
}


} // namespace imtclientgql


