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
	QObject-based IGqlSubscriptionClient: registers itself with the given manager on
	construction, re-emits incoming callbacks as signals instead of requiring the
	caller to implement IGqlSubscriptionClient.

	Lifetime: prefer ScheduleDestruction() over a bare `delete`/unique_ptr reset
	whenever dispatch for this subscription could concurrently be in flight on
	another thread. Unregistering is safe to do synchronously (it only removes a
	pointer from the manager's dispatch list, under the manager's own mutex); it is
	only the deletion of this QObject itself that must be deferred, since a virtual
	call onto it could still be executing on another thread. ScheduleDestruction()
	unregisters immediately (so no further dispatch is possible) and defers the
	actual C++ object destruction via deleteLater().
*/
class CGqlSubscriptionHandle: public QObject, public IGqlSubscriptionClient
{
	Q_OBJECT

public:
	CGqlSubscriptionHandle(IGqlSubscriptionManager& manager, const imtgql::IGqlRequest& request);

	CGqlSubscriptionHandle(const CGqlSubscriptionHandle&) = delete;
	CGqlSubscriptionHandle& operator=(const CGqlSubscriptionHandle&) = delete;

	~CGqlSubscriptionHandle() override;

	bool IsRegistered() const
	{
		return !m_subscriptionId.isEmpty();
	}

	/**
		Unregisters (if not already unregistered) and schedules this object's
		deletion via deleteLater(). Use this instead of `delete`/unique_ptr::reset()
		for normal teardown of a handle.
	*/
	void ScheduleDestruction();

Q_SIGNALS:
	void payloadReceived(const QByteArray& subscriptionData);
	void statusChanged(imtclientgql::IGqlSubscriptionClient::SubscriptionStatus status, const QString& message);

protected:
	// reimplemented (imtclientgql::IGqlSubscriptionClient)
	void OnResponseReceived(const QByteArray& subscriptionId, const QByteArray& subscriptionData) override;
	void OnSubscriptionStatusChanged(
		const QByteArray& subscriptionId, const SubscriptionStatus& status, const QString& message) override;

private:
	void Unregister();

private:
	IGqlSubscriptionManager& m_manager;
	QByteArray m_subscriptionId;
};


} // namespace imtclientgql
