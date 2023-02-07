#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlSubscriptionClient.h>


namespace imtgql
{


class IGqlSubscriptionManager: virtual public istd::IChangeable
{
public:
	virtual QByteArray RegisterSubscription(const imtgql::CGqlRequest& subscriptionRequest, IGqlSubscriptionClient* subscriptionClient) = 0;
	virtual bool UnRegisterSubscription(const QByteArray& subscriptionId) = 0;
};


} // namespace imtgql


