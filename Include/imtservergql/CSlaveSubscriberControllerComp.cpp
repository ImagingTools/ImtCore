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
	imtgql::IGqlSubscriberController* stalePublisherPtr = nullptr;
	quint64 generation = 0;
	{
		QMutexLocker lock(&m_publisherMapMutex);
		generation = m_subscriptionGenerationMap.value(subscriptionId) + 1;
		m_subscriptionGenerationMap.insert(subscriptionId, generation);

		auto iter = m_publisherMap.find(subscriptionId);
		if (iter != m_publisherMap.end()){
			stalePublisherPtr = iter.value();
		}
	}

	if (stalePublisherPtr != nullptr){
		stalePublisherPtr->UnregisterSubscription(subscriptionId);
	}

	for (int index = 0; index < m_subscriberControllerListCompPtr.GetCount(); index++){
		imtgql::IGqlSubscriberController* publisherPtr = m_subscriberControllerListCompPtr[index];
		if (publisherPtr == nullptr){
			continue;
		}
		if (!publisherPtr->IsRequestSupported(gqlRequest)){
			continue;
		}

		if (!publisherPtr->RegisterSubscription(subscriptionId, gqlRequest, networkRequest, errorMessage)){
			continue;
		}

		bool isCurrentGeneration = false;
		{
			QMutexLocker lock(&m_publisherMapMutex);
			isCurrentGeneration = (m_subscriptionGenerationMap.value(subscriptionId) == generation);
			if (isCurrentGeneration){
				m_publisherMap.insert(subscriptionId, publisherPtr);
			}
		}
		if (isCurrentGeneration){
			return true;
		}

		publisherPtr->UnregisterSubscription(subscriptionId);
	}

	{
		QMutexLocker lock(&m_publisherMapMutex);
		if (m_subscriptionGenerationMap.value(subscriptionId) == generation){
			m_publisherMap.remove(subscriptionId);
		}
	}

	return false;
}


bool CSlaveSubscriberControllerComp::UnregisterSubscription(const QByteArray& subscriptionId)
{
	imtgql::IGqlSubscriberController* publisherPtr = nullptr;
	quint64 generation = 0;
	{
		QMutexLocker lock(&m_publisherMapMutex);
		generation = m_subscriptionGenerationMap.value(subscriptionId) + 1;
		m_subscriptionGenerationMap.insert(subscriptionId, generation);

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
	{
		QMutexLocker lock(&m_publisherMapMutex);
		if (m_subscriptionGenerationMap.value(subscriptionId) == generation){
			m_publisherMap.remove(subscriptionId);
		}
	}
	return res;
}


} // namespace imtservergql
