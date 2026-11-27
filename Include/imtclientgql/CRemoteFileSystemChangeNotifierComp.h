// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QSet>

// STL includes
#include <memory>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtfile/IFileSystemChangeNotifier.h>
#include <imtclientgql/IGqlSubscriptionManager.h>


namespace imtclientgql
{


/**
	Changes of the folders of a remote file system, through the subscription OnFileSystemChanged of its server.

	Each watch is a subscription; the watches of the same folder share one. The remote server resolves the path and checks
	the access when it registers the subscription, with the token of the subscription manager (e.g. of a remote session):
	the context of the current request is not sent. AddWatch does not wait for the remote server, so a refused subscription
	is not its error: the watch reports a change, and the listing of the folder then reports the error.

	The listener is called in the thread of the component, which needs an event loop, and not more than once per watch
	within 'NotificationDelayMs'. Besides the changes of the folder, a watch reports a change when the remote server confirms
	the subscription (the first time and after each reconnection) and when it refuses it: the changes meanwhile are not
	known, so the folder is to be read again.
*/
class CRemoteFileSystemChangeNotifierComp:
			public ilog::CLoggerComponentBase,
			virtual public imtfile::IFileSystemChangeNotifier
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRemoteFileSystemChangeNotifierComp);
		I_REGISTER_INTERFACE(imtfile::IFileSystemChangeNotifier);
		I_ASSIGN(m_subscriptionManagerCompPtr, "SubscriptionManager", "Subscription manager connected to the remote file service", true, "SubscriptionManager");
		I_ASSIGN(m_notificationDelayAttrPtr, "NotificationDelayMs", "Time in ms within which the changes of a watch are reported once", true, 300);
	I_END_COMPONENT;

	// reimplemented (imtfile::IFileSystemChangeNotifier)
	virtual bool AddWatch(
				const QString& path,
				imtfile::IFileSystemChangeListener& listener,
				QByteArray& watchId,
				QString& error) override;
	virtual void RemoveWatch(const QByteArray& watchId) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentDestroyed() override;

private:
	class Watch;
	typedef std::shared_ptr<Watch> WatchPtr;

	/**
		Reports a change of the watch after the delay, together with the other changes meanwhile. It may be called from any thread.
	*/
	void ScheduleNotification(const QByteArray& watchId);
	void NotifyScheduledWatches();
	/**
		After it returns, the listener of the watch is not called any more.
	*/
	void StopWatch(Watch& watch);

private:
	I_REF(IGqlSubscriptionManager, m_subscriptionManagerCompPtr);
	I_ATTR(int, m_notificationDelayAttrPtr);

	// Never held across a call of the subscription manager or of a listener: they call back
	QMutex m_mutex;
	QHash<QByteArray, WatchPtr> m_watches;
	QSet<QByteArray> m_scheduledWatchIds;
	bool m_isNotificationScheduled = false;
	// Lives in the thread of the component: the listeners are called there
	QObject m_notificationContext;
};


} // namespace imtclientgql
