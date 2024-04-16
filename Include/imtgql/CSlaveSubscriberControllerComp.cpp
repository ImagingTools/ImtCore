#include <imtgql/CSlaveSubscriberControllerComp.h>


// ImtCore includes
#include<imtbase/IOperationContext.h>
#include<imtrest/IProtocolEngine.h>


namespace imtgql
{


// protected methods

bool CSlaveSubscriberControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	if (!m_subscriberControllerListCompPtr.IsValid()){
		return false;
	}

	for (int index = 0; index < m_subscriberControllerListCompPtr.GetCount(); index++){
		if (m_subscriberControllerListCompPtr[index]->IsRequestSupported(gqlRequest)){
			return true;
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
	if (!m_subscriberControllerListCompPtr.IsValid()){
		return false;
	}

	for (int index = 0; index < m_subscriberControllerListCompPtr.GetCount(); index++){
		if (m_subscriberControllerListCompPtr[index]->IsRequestSupported(gqlRequest)
			&& m_subscriberControllerListCompPtr[index]->RegisterSubscription(subscriptionId, gqlRequest, networkRequest, errorMessage)){
			return true;
		}
	}

	return false;
}


bool CSlaveSubscriberControllerComp::UnRegisterSubscription(const QByteArray& subscriptionId)
{
	if (!m_subscriberControllerListCompPtr.IsValid()){
		return false;
	}

	for (int index = 0; index < m_subscriberControllerListCompPtr.GetCount(); index++){
		if (m_subscriberControllerListCompPtr[index]->UnRegisterSubscription(subscriptionId)){
			return true;
		}
	}

	return false;
}


} // namespace imtgql


