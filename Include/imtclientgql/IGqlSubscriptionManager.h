#pragma once


// ImtCore includes
#include <imtgql/IGqlRequest.h>
#include <imtclientgql/IGqlSubscriptionClient.h>


namespace imtclientgql
{


class IGqlSubscriptionManager: virtual public istd::IChangeable
{
public:
	virtual QByteArray RegisterSubscription(
				const imtgql::IGqlRequest& subscriptionRequest,
				imtclientgql::IGqlSubscriptionClient* subscriptionClientPtr) = 0;
	virtual bool UnregisterSubscription(const QByteArray& subscriptionId) = 0;
};


} // namespace imtgql


