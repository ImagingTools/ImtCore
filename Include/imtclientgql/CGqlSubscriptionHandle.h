// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>

// ImtCore includes
#include <imtclientgql/IGqlSubscriptionManager.h>


namespace imtgql
{
class IGqlRequest;
}


namespace imtclientgql
{


/**
	IGqlSubscriptionClient as a QObject: registers on construction, unregisters on
	destruction, and re-emits the manager's callbacks as signals.

	Shall live on the subscriber's thread. The signals are emitted on whatever thread the
	manager dispatches on, so connect with the subscriber as context object and Qt
	delivers on the subscriber's thread.

	Destroying the handle revokes the subscription.
*/
class CGqlSubscriptionHandle: public QObject, public IGqlSubscriptionClient
{
	Q_OBJECT

public:
	/**
		Constructs a subscription handle, registering it with the given manager and request.
		Check \c IsRegistered() to see if the subscription was successfully registered.
	*/
	CGqlSubscriptionHandle(IGqlSubscriptionManager& manager, const imtgql::IGqlRequest& request);

	CGqlSubscriptionHandle(const CGqlSubscriptionHandle&) = delete;
	CGqlSubscriptionHandle& operator=(const CGqlSubscriptionHandle&) = delete;

	~CGqlSubscriptionHandle() override;

	bool IsRegistered() const
	{
		return !m_subscriptionId.isEmpty();
	}

	/**
		Revokes the subscription. No signal is emitted after this returns.
	*/
	void Unregister();

Q_SIGNALS:
	void payloadReceived(const QByteArray& subscriptionData);
	void statusChanged(imtclientgql::IGqlSubscriptionClient::SubscriptionStatus status, const QString& message);

protected:
	// reimplemented (imtclientgql::IGqlSubscriptionClient)
	void OnResponseReceived(const QByteArray& subscriptionId, const QByteArray& subscriptionData) final;
	void OnSubscriptionStatusChanged(
		const QByteArray& subscriptionId, const SubscriptionStatus& status, const QString& message) final;

private:
	IGqlSubscriptionManager& m_manager;
	QByteArray m_subscriptionId;
};


} // namespace imtclientgql
