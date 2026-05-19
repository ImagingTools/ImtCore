// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CSlaveSubscriberControllerComp.h>


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
		if (publisherPtr != nullptr){
			if (publisherPtr->IsRequestSupported(gqlRequest)){
				{
					QReadLocker readLocker(&m_lock);
					if (m_publisherMap.contains(subscriptionId)){
						qWarning("Subscription already registered");

						return true;
					}
				}
				if (publisherPtr->RegisterSubscription(subscriptionId, gqlRequest, networkRequest, errorMessage)){
					QWriteLocker writeLocker(&m_lock);
					// Re-check under write lock to prevent duplicate registration (TOCTOU)
					if (!m_publisherMap.contains(subscriptionId)){
						m_publisherMap[subscriptionId] = publisherPtr;
					}

					return true;
				}
			}
		}
	}

	return false;
}


bool CSlaveSubscriberControllerComp::UnregisterSubscription(const QByteArray& subscriptionId)
{
	imtgql::IGqlSubscriberController* publisherPtr = nullptr;
	{
		QReadLocker readLocker(&m_lock);
		auto it = m_publisherMap.constFind(subscriptionId);
		if (it != m_publisherMap.constEnd()){
			publisherPtr = it.value();
		}
	}

	if (publisherPtr == nullptr){
		return false;
	}

	Q_ASSERT(publisherPtr != nullptr);

	bool res = publisherPtr->UnregisterSubscription(subscriptionId);
	if (res){
		QWriteLocker writeLocker(&m_lock);
		m_publisherMap.remove(subscriptionId);
	}
	return res;
}


} // namespace imtservergql


