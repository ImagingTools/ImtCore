// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QList>
#include <QtCore/QMutex>

// ImtCore includes
#include <imtcache/ICacheUpdateController.h>
#include <imtservergql/CObjectCollectionChangeNotifierComp.h>


namespace imtcache
{


/**
	Publishes the changes of a source collection that is mirrored into the cache, once the cache has them.

	The collection's changes are not published when they happen: a client that reacted to one would read
	the cache before it had caught up. Each change asks the cache for an update and waits; the messages
	are published when an update that began after the change has finished successfully, and so saw it.
	A failed update publishes nothing, the messages wait for the next one.

	Only the collections attached to this component publish, so a table the cache fills from elsewhere,
	as telemetry may be, never causes a message.
*/
class CCacheCollectionChangeNotifierComp:
			public imtservergql::CObjectCollectionChangeNotifierComp,
			virtual public ICacheUpdateController::IObserver
{
public:
	using BaseClass = imtservergql::CObjectCollectionChangeNotifierComp;

	I_BEGIN_COMPONENT(CCacheCollectionChangeNotifierComp);
		I_ASSIGN(m_cacheUpdateControllerCompPtr, "CacheUpdateController", "Cache that mirrors the collection, asked to update at every change", true, "CacheUpdateController");
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CGqlPublisherCompBase)
	virtual bool PublishData(const QByteArray& commandId, const QByteArray& data) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imtcache::ICacheUpdateController::IObserver)
	virtual void OnCacheUpdated(bool isOk, qint64 startedAtMs) override;

private:
	struct PendingMessage
	{
		QByteArray commandId;
		QByteArray data;
		qint64 changedAtMs = 0;
	};

private:
	I_REF(ICacheUpdateController, m_cacheUpdateControllerCompPtr);

	mutable QMutex m_pendingMutex;
	mutable QList<PendingMessage> m_pendingMessages;
};


} // namespace imtcache
