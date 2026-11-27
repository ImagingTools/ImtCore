// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CRemoteFileSystemChangeNotifierComp.h>


// Qt includes
#include <QtCore/QRecursiveMutex>
#include <QtCore/QTimer>
#include <QtCore/QUuid>

// STL includes
#include <utility>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/FileSystem.h>


namespace imtclientgql
{


/**
	A watch as the client of its subscription.
*/
class CRemoteFileSystemChangeNotifierComp::Watch: public IGqlSubscriptionClient
{
public:
	Watch(
				CRemoteFileSystemChangeNotifierComp& notifier,
				const QByteArray& watchId,
				const QString& path,
				imtfile::IFileSystemChangeListener& listener)
		:m_notifier(notifier),
		m_watchId(watchId),
		m_path(path),
		m_listener(listener)
	{
	}

	const QByteArray& GetSubscriptionId() const
	{
		return m_subscriptionId;
	}

	void SetSubscriptionId(const QByteArray& subscriptionId)
	{
		m_subscriptionId = subscriptionId;
	}

	/**
		Calls the listener, unless the watch is stopped.
	*/
	void Notify()
	{
		QMutexLocker listenerLocker(&m_listenerMutex);
		if (!m_isStopped){
			m_listener.OnFileSystemChanged(m_watchId, m_path);
		}
	}

	/**
		After it returns, the listener is not called any more: a call in another thread is waited for, a call in this
		thread (the listener stops its own watch) is not.
	*/
	void Stop()
	{
		QMutexLocker listenerLocker(&m_listenerMutex);
		m_isStopped = true;
	}

	// reimplemented (imtclientgql::IGqlSubscriptionClient)
	virtual void OnResponseReceived(const QByteArray& /*subscriptionId*/, const QByteArray& /*subscriptionData*/) override
	{
		// The event names the folder of the subscription only
		m_notifier.ScheduleNotification(m_watchId);
	}

	virtual void OnSubscriptionStatusChanged(
				const QByteArray& /*subscriptionId*/,
				const SubscriptionStatus& status,
				const QString& /*message*/) override
	{
		// The changes before a confirmation or a refusal are not known: the folder is to be read again.
		// A refusal is reported once, not again after each retry of the subscription manager.
		bool isReported = false;
		{
			QMutexLocker statusLocker(&m_statusMutex);
			isReported = (status == SS_REGISTERED) || (status == SS_REFUSED && m_status != SS_REFUSED);
			m_status = status;
		}

		if (isReported){
			m_notifier.ScheduleNotification(m_watchId);
		}
	}

private:
	CRemoteFileSystemChangeNotifierComp& m_notifier;
	const QByteArray m_watchId;
	const QString m_path;
	imtfile::IFileSystemChangeListener& m_listener;
	// Set before the watch is added, read after it is removed (under the mutex of the notifier)
	QByteArray m_subscriptionId;

	QMutex m_statusMutex;
	SubscriptionStatus m_status = SS_UNKNOWN;

	// Held across the call of the listener; recursive, so that the listener can stop its own watch
	QRecursiveMutex m_listenerMutex;
	bool m_isStopped = false;
};


// public methods

// reimplemented (imtfile::IFileSystemChangeNotifier)

bool CRemoteFileSystemChangeNotifierComp::AddWatch(
			const QString& path,
			imtfile::IFileSystemChangeListener& listener,
			QByteArray& watchId,
			QString& error)
{
	namespace filesdl = sdl::V1_0::imtbase;

	filesdl::OnFileSystemChangedRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->path = path;

	// Without a context: the subscription goes with the token of the subscription manager, not with that of the current request
	imtgql::CGqlRequest subscriptionRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION);
	if (!filesdl::COnFileSystemChangedGqlRequest::SetupGqlRequest(subscriptionRequest, arguments)){
		error = QStringLiteral("Unable to subscribe to the changes of '%1'").arg(path);
		SendErrorMessage(0, QStringLiteral("Unable to build the subscription request for '%1'").arg(path), "CRemoteFileSystemChangeNotifierComp");

		return false;
	}

	subscriptionRequest.AddSimpleField(filesdl::CFileSystemChangedEvent::FileSystemChangedEventFields::Path.toUtf8());

	const QByteArray newWatchId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	const WatchPtr watchPtr = std::make_shared<Watch>(*this, newWatchId, path, listener);

	// The manager reports the status of the subscription from inside this call
	const QByteArray subscriptionId = m_subscriptionManagerCompPtr->RegisterSubscription(subscriptionRequest, *watchPtr);
	if (subscriptionId.isEmpty()){
		error = QStringLiteral("Unable to subscribe to the changes of '%1'").arg(path);
		SendErrorMessage(0, QStringLiteral("The subscription manager did not register the subscription for '%1'").arg(path), "CRemoteFileSystemChangeNotifierComp");

		return false;
	}

	watchPtr->SetSubscriptionId(subscriptionId);
	{
		QMutexLocker locker(&m_mutex);
		m_watches.insert(newWatchId, watchPtr);
	}

	watchId = newWatchId;

	return true;
}


void CRemoteFileSystemChangeNotifierComp::RemoveWatch(const QByteArray& watchId)
{
	WatchPtr watchPtr;
	{
		QMutexLocker locker(&m_mutex);
		watchPtr = m_watches.take(watchId);
	}

	if (watchPtr){
		StopWatch(*watchPtr);
	}
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CRemoteFileSystemChangeNotifierComp::OnComponentDestroyed()
{
	QHash<QByteArray, WatchPtr> watches;
	{
		QMutexLocker locker(&m_mutex);
		watches.swap(m_watches);
	}

	for (const WatchPtr& watchPtr : std::as_const(watches)){
		StopWatch(*watchPtr);
	}

	BaseClass::OnComponentDestroyed();
}


// private methods

void CRemoteFileSystemChangeNotifierComp::ScheduleNotification(const QByteArray& watchId)
{
	{
		QMutexLocker locker(&m_mutex);
		m_scheduledWatchIds.insert(watchId);
		if (m_isNotificationScheduled){
			return;
		}

		m_isNotificationScheduled = true;
	}

	// The timer is started in the thread of the component
	const int delay = *m_notificationDelayAttrPtr;
	QMetaObject::invokeMethod(&m_notificationContext, [this, delay](){
		QTimer::singleShot(delay, &m_notificationContext, [this](){
			NotifyScheduledWatches();
		});
	}, Qt::QueuedConnection);
}


void CRemoteFileSystemChangeNotifierComp::NotifyScheduledWatches()
{
	QList<WatchPtr> watches;
	{
		QMutexLocker locker(&m_mutex);
		for (const QByteArray& watchId : std::as_const(m_scheduledWatchIds)){
			// A removed watch is not reported
			const WatchPtr watchPtr = m_watches.value(watchId);
			if (watchPtr){
				watches.append(watchPtr);
			}
		}

		m_scheduledWatchIds.clear();
		m_isNotificationScheduled = false;
	}

	for (const WatchPtr& watchPtr : std::as_const(watches)){
		watchPtr->Notify();
	}
}


void CRemoteFileSystemChangeNotifierComp::StopWatch(Watch& watch)
{
	// The manager waits for its running calls of the watch, and they take the mutex: it is not held here
	m_subscriptionManagerCompPtr->UnregisterSubscription(watch.GetSubscriptionId(), watch);
	watch.Stop();
}


} // namespace imtclientgql
