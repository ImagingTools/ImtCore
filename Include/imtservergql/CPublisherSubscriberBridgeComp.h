// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtclientgql/IGqlSubscriptionClient.h>
#include <imtclientgql/IGqlSubscriptionManager.h>
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtservergql
{


/**
	A bridge component that combines CGqlPublisherCompBase (server-side publish)
	with IGqlSubscriptionClient (client-side subscribe) in a single component.

	Directly inherits IGqlSubscriptionClient to receive subscription data
	from the remote server and re-publishes it to local WebSocket clients.
*/
class CPublisherSubscriberBridgeComp: public CGqlPublisherCompBase,
                                      virtual public imtclientgql::IGqlSubscriptionClient
{
public:
	typedef CGqlPublisherCompBase BaseClass;

	I_BEGIN_COMPONENT(CPublisherSubscriberBridgeComp);
		I_ASSIGN(m_subscriptionManagerCompPtr, "SubscriptionManager", "Subscription manager for registering subscriptions", true, "SubscriptionManager");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imtclientgql::IGqlSubscriptionClient)
	virtual void OnResponseReceived(
				const QByteArray& subscriptionId,
				const QByteArray& subscriptionData) override;
	virtual void OnSubscriptionStatusChanged(
				const QByteArray& subscriptionId,
				const SubscriptionStatus& status,
				const QString& message) override;

private:
	QByteArray GetCommandForSubscription(const QByteArray& subscriptionId) const;

private:
	I_REF(imtclientgql::IGqlSubscriptionManager, m_subscriptionManagerCompPtr);

	QByteArrayList m_subscriptionIds;
};


} // namespace imtservergql


