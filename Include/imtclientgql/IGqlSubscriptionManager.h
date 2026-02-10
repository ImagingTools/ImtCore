// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
				IGqlSubscriptionClient* subscriptionClientPtr) = 0;
	virtual bool UnregisterSubscription(const QByteArray& subscriptionId) = 0;
};


} // namespace imtgql


