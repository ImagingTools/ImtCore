// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>

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

	Uses lazy registration: upstream subscriptions are created only when a real
	client subscribes, forwarding the client's GQL context (user/token) to the
	remote server. This ensures the remote server can identify the user.
*/
class CPublisherSubscriberBridgeComp: public CGqlPublisherCompBase,
                                      virtual public imtclientgql::IGqlSubscriptionClient
{
public:
	typedef CGqlPublisherCompBase BaseClass;

	I_BEGIN_COMPONENT(CPublisherSubscriberBridgeComp);
		I_ASSIGN(m_subscriptionManagerCompPtr, "SubscriptionManager", "Subscription manager for registering subscriptions", true, "SubscriptionManager");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlSubscriberController via CGqlPublisherCompBase)
	virtual bool RegisterSubscription(
				const QByteArray& subscriptionId,
				const imtgql::CGqlRequest& gqlRequest,
				const imtrest::IRequest& networkRequest,
				QString& errorMessage) override;
	virtual bool UnregisterSubscription(const QByteArray& subscriptionId) override;

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
	I_REF(imtclientgql::IGqlSubscriptionManager, m_subscriptionManagerCompPtr);

	// Mapping: client subscriptionId -> upstream subscriptionId
	QMap<QByteArray, QByteArray> m_clientToUpstreamMap;
	// Mapping: upstream subscriptionId -> list of client subscriptionIds
	QMap<QByteArray, QByteArrayList> m_upstreamToClientsMap;

	mutable QMutex m_bridgeMutex;
};


} // namespace imtservergql


