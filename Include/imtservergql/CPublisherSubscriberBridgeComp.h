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

	Uses a private inner class to implement IGqlSubscriptionClient without
	multiple inheritance conflicts. Subscribes to a remote server's notifications
	and re-publishes them to local WebSocket clients.
*/
class CPublisherSubscriberBridgeComp: public CGqlPublisherCompBase
{
public:
	typedef CGqlPublisherCompBase BaseClass;

	I_BEGIN_COMPONENT(CPublisherSubscriberBridgeComp);
		I_ASSIGN(m_subscriptionManagerCompPtr, "SubscriptionManager", "Subscription manager for registering subscriptions", true, "SubscriptionManager");
		I_ASSIGN_MULTI_0(m_subscriptionCommandAttrPtr, "SubscriptionCommand", "Commands to subscribe to on the remote server", true);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	/**
		Private inner class implementing IGqlSubscriptionClient.
		Delegates received data back to the owning bridge component.
	*/
	class SubscriptionClient: virtual public imtclientgql::IGqlSubscriptionClient
	{
	public:
		explicit SubscriptionClient(CPublisherSubscriberBridgeComp* ownerPtr);

		// reimplemented (imtclientgql::IGqlSubscriptionClient)
		virtual void OnResponseReceived(
					const QByteArray& subscriptionId,
					const QByteArray& subscriptionData) override;
		virtual void OnSubscriptionStatusChanged(
					const QByteArray& subscriptionId,
					const SubscriptionStatus& status,
					const QString& message) override;

	private:
		CPublisherSubscriberBridgeComp* m_ownerPtr;
	};

	void HandleSubscriptionData(const QByteArray& subscriptionId, const QByteArray& subscriptionData);
	QByteArray GetCommandForSubscription(const QByteArray& subscriptionId) const;

private:
	I_REF(imtclientgql::IGqlSubscriptionManager, m_subscriptionManagerCompPtr);
	I_MULTIATTR(QByteArray, m_subscriptionCommandAttrPtr);

	SubscriptionClient m_subscriptionClient{this};
	QByteArrayList m_subscriptionIds;
};


} // namespace imtservergql


