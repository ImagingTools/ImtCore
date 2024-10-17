#include <imtgql/CSlaveSubscriberControllerComp.h>


// ImtCore includes
#include<imtbase/IOperationContext.h>
#include<imtrest/IProtocolEngine.h>


namespace imtgql
{


// protected methods

bool CSlaveSubscriberControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	for (int index = 0; index < m_subscriberControllerListCompPtr.GetCount(); index++){
		imtgql::IGqlSubscriberController* subscriberControllerPtr = m_subscriberControllerListCompPtr[index];
		if (subscriberControllerPtr != nullptr){
			if (subscriberControllerPtr->IsRequestSupported(gqlRequest)){
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
		imtgql::IGqlSubscriberController* subscriberControllerPtr = m_subscriberControllerListCompPtr[index];
		if (subscriberControllerPtr != nullptr){
			if (subscriberControllerPtr->IsRequestSupported(gqlRequest)
				&& subscriberControllerPtr->RegisterSubscription(subscriptionId, gqlRequest, networkRequest, errorMessage)){
				return true;
			}
		}

	}

	return false;
}


bool CSlaveSubscriberControllerComp::UnRegisterSubscription(const QByteArray& subscriptionId)
{
	for (int index = 0; index < m_subscriberControllerListCompPtr.GetCount(); index++){
		imtgql::IGqlSubscriberController* subscriberControllerPtr = m_subscriberControllerListCompPtr[index];
		if (subscriberControllerPtr != nullptr){
			if (subscriberControllerPtr->UnRegisterSubscription(subscriptionId)){
				return true;
			}
		}
	}

	return false;
}


} // namespace imtgql


