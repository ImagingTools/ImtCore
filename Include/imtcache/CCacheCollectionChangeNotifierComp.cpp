// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcache/CCacheCollectionChangeNotifierComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QMutexLocker>


namespace imtcache
{


namespace
{


/// A cache that stays behind for this long has bigger problems than lost messages.
constexpr int MAX_PENDING_MESSAGES = 5000;


} // anonymous namespace


// protected methods

// reimplemented (imtservergql::CGqlPublisherCompBase)

bool CCacheCollectionChangeNotifierComp::PublishData(const QByteArray& commandId, const QByteArray& data) const
{
	PendingMessage message;
	message.commandId = commandId;
	message.data = data;
	message.changedAtMs = QDateTime::currentMSecsSinceEpoch();

	{
		QMutexLocker locker(&m_pendingMutex);

		m_pendingMessages.append(message);

		if (m_pendingMessages.size() > MAX_PENDING_MESSAGES){
			m_pendingMessages.removeFirst();

			SendWarningMessage(0, QStringLiteral("The cache update is not catching up with the collection changes. The oldest change was not published"), __func__);
		}
	}

	if (m_cacheUpdateControllerCompPtr.IsValid()){
		m_cacheUpdateControllerCompPtr->RequestUpdate(ICacheUpdateController::UM_INCREMENTAL);
	}

	return true;
}


// reimplemented (icomp::CComponentBase)

void CCacheCollectionChangeNotifierComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_cacheUpdateControllerCompPtr.IsValid()){
		m_cacheUpdateControllerCompPtr->AttachObserver(this);
	}
}


void CCacheCollectionChangeNotifierComp::OnComponentDestroyed()
{
	if (m_cacheUpdateControllerCompPtr.IsValid()){
		m_cacheUpdateControllerCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imtcache::ICacheUpdateController::IObserver)

void CCacheCollectionChangeNotifierComp::OnCacheUpdated(bool isOk, qint64 startedAtMs)
{
	if (!isOk){
		return;
	}

	QList<PendingMessage> readyMessages;

	{
		QMutexLocker locker(&m_pendingMutex);

		for (auto it = m_pendingMessages.begin(); it != m_pendingMessages.end(); ){
			if (it->changedAtMs <= startedAtMs){
				readyMessages.append(*it);
				it = m_pendingMessages.erase(it);
			}
			else{
				++it;
			}
		}
	}

	for (const PendingMessage& message : std::as_const(readyMessages)){
		BaseClass::PublishData(message.commandId, message.data);
	}
}


} // namespace imtcache
