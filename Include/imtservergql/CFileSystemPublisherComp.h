// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QPair>

// ImtCore includes
#include <imtfile/IFileSystemChangeNotifier.h>
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtservergql
{


/**
	Publishes OnFileSystemChanged of FileSystem.sdl.
	Each subscription watches its folder through IFileSystemChangeNotifier, in the context of the subscription request,
	and a change of the folder goes to that subscriber only. The subscription ends the watch when the subscriber
	unsubscribes or its connection drops. A subscriber reads the folder after the subscription is registered,
	so it misses no change. With a remote provider as the source it relays the subscription of another server.
*/
class CFileSystemPublisherComp:
			public CGqlPublisherCompBase,
			virtual public imtfile::IFileSystemChangeListener
{
public:
	typedef CGqlPublisherCompBase BaseClass;

	I_BEGIN_COMPONENT(CFileSystemPublisherComp);
		I_ASSIGN(m_changeNotifierCompPtr, "ChangeNotifier", "Source of the changes of the watched folders", true, "ChangeNotifier");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool RegisterSubscription(
				const QByteArray& subscriptionId,
				const imtgql::CGqlRequest& gqlRequest,
				const imtrest::IRequest& networkRequest,
				QString& errorMessage) override;
	virtual bool UnregisterSubscription(const QByteArray& connectionId, const QByteArray& subscriptionId) override;

	// reimplemented (imtfile::IFileSystemChangeListener)
	virtual void OnFileSystemChanged(const QByteArray& watchId, const QString& path) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentDestroyed() override;

private:
	// Connection ID and subscription ID
	typedef QPair<QByteArray, QByteArray> SubscriptionKey;

	I_REF(imtfile::IFileSystemChangeNotifier, m_changeNotifierCompPtr);

	QMutex m_watchMutex;
	QMap<QByteArray, SubscriptionKey> m_subscriptionsOfWatches;
	QMap<SubscriptionKey, QByteArray> m_watchesOfSubscriptions;
};


} // namespace imtservergql
