#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtclientgql/IGqlSubscriptionClient.h>


namespace imtclientgql
{


class IGqlSubscriptionManager: virtual public istd::IChangeable
{
public:
	virtual QByteArray RegisterSubscription(const imtgql::CGqlRequest& subscriptionRequest, IGqlSubscriptionClient* subscriptionClient) = 0;
	virtual bool UnregisterSubscription(const QByteArray& subscriptionId) = 0;
};


} // namespace imtgql


