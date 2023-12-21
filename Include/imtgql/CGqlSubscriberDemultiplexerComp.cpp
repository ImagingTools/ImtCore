#include <imtgql/CGqlSubscriberDemultiplexerComp.h>


namespace imtgql
{


// reimplemented (imtgql::IGqlSubscriberController)

bool CGqlSubscriberDemultiplexerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	for (int i = 0; i < m_gqlSubscriberControllersCompPtr.GetCount(); i++){
		const imtgql::IGqlSubscriberController* gqlSubscriberControllerPtr = m_gqlSubscriberControllersCompPtr[i];
		if (gqlSubscriberControllerPtr != nullptr){
			if (gqlSubscriberControllerPtr->IsRequestSupported(gqlRequest)){
				return true;
			}
		}
	}

	return false;
}


bool CGqlSubscriberDemultiplexerComp::RegisterSubscription(
			const QByteArray& subscriptionId,
			const imtgql::CGqlRequest& gqlRequest,
			const imtrest::IRequest& networkRequest,
			QString& errorMessage)
{
	for (int i = 0; i < m_gqlSubscriberControllersCompPtr.GetCount(); i++){
		imtgql::IGqlSubscriberController* gqlSubscriberControllerPtr = m_gqlSubscriberControllersCompPtr[i];
		if (gqlSubscriberControllerPtr != nullptr){
			if (gqlSubscriberControllerPtr->IsRequestSupported(gqlRequest)){
				return gqlSubscriberControllerPtr->RegisterSubscription(subscriptionId, gqlRequest, networkRequest, errorMessage);
			}
		}
	}

	return false;
}


bool CGqlSubscriberDemultiplexerComp::UnRegisterSubscription(const QByteArray& subscriptionId)
{
	return false;
}


} // namespace imtgql


