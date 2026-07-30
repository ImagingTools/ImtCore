// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CSlaveSubscriberControllerComp.h>


// Qt includes
#include <QtCore/QMutexLocker>


namespace imtservergql
{


// protected methods

bool CSlaveSubscriberControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	for (int index = 0; index < m_subscriberControllerListCompPtr.GetCount(); index++){
		imtgql::IGqlSubscriberController* publisherPtr = m_subscriberControllerListCompPtr[index];
		if (publisherPtr != nullptr){
			if (publisherPtr->IsRequestSupported(gqlRequest)){
				return true;
			}
		}
	}

	return false;
}


bool CSlaveSubscriberControllerComp::RegisterSubscription(
			const QByteArray& subscriptionId,
			const imtgql::CGqlRequest& gqlRequest,
			const imtrest::IRequest& networkRequest,
			QString& errorMessage)
{
	for (int index = 0; index < m_subscriberControllerListCompPtr.GetCount(); index++){
		imtgql::IGqlSubscriberController* publisherPtr = m_subscriberControllerListCompPtr[index];
		if (publisherPtr == nullptr){
			continue;
		}
		if (!publisherPtr->IsRequestSupported(gqlRequest)){
			continue;
		}

		{
			QMutexLocker lock(&m_publisherMapMutex);
			if (m_publisherMap.contains(subscriptionId)){
				return true;
			}
		}

		// Nested register without the map lock — publishers may block or re-enter
		// Unregister; holding the mutex across that call risks deadlock.
		if (!publisherPtr->RegisterSubscription(subscriptionId, gqlRequest, networkRequest, errorMessage)){
			continue;
		}

		{
			QMutexLocker lock(&m_publisherMapMutex);
			// Another connection may have raced the same id (unlikely UUID) — keep first.
			if (!m_publisherMap.contains(subscriptionId)){
				m_publisherMap.insert(subscriptionId, publisherPtr);
			}
		}
		return true;
	}

	return false;
}


bool CSlaveSubscriberControllerComp::UnregisterSubscription(const QByteArray& subscriptionId)
{
	imtgql::IGqlSubscriberController* publisherPtr = nullptr;
	{
		QMutexLocker lock(&m_publisherMapMutex);
		auto iter = m_publisherMap.find(subscriptionId);
		if (iter == m_publisherMap.end()){
			return false;
		}
		publisherPtr = iter.value();
	}

	Q_ASSERT(publisherPtr != nullptr);
	if (publisherPtr == nullptr){
		return false;
	}

	const bool res = publisherPtr->UnregisterSubscription(subscriptionId);
	if (res){
		QMutexLocker lock(&m_publisherMapMutex);
		m_publisherMap.remove(subscriptionId);
	}
	return res;
}


} // namespace imtservergql
