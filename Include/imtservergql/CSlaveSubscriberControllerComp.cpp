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
				if (!m_publisherMap.contains(subscriptionId)){
					if (publisherPtr->RegisterSubscription(subscriptionId, gqlRequest, networkRequest, errorMessage)){
						m_publisherMap[subscriptionId] = publisherPtr;

						return true;
					}
				}
				else{
					qWarning("Subscription already registered");

					return true;
				}
			}
		}
	}

	return false;
}


bool CSlaveSubscriberControllerComp::UnregisterSubscription(const QByteArray& subscriptionId)
{
	if (m_publisherMap.contains(subscriptionId)){
		imtgql::IGqlSubscriberController* publisherPtr = m_publisherMap[subscriptionId];
		Q_ASSERT(publisherPtr != nullptr);
		bool res = publisherPtr->UnregisterSubscription(subscriptionId);
		if(res){
			m_publisherMap.remove(subscriptionId);
		}
		return res;
	}

	return false;
}


} // namespace imtservergql


