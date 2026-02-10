// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtrest/IRequest.h>


namespace imtgql
{


class IGqlSubscriberController: virtual public istd::IPolymorphic
{
public:
	/**
		Get IDs of the supported commands.
	*/
	virtual bool IsRequestSupported(const CGqlRequest& gqlRequest) const = 0;

	/**
		Register subscriprion for a GraphQL-based request.
	*/
	virtual bool RegisterSubscription(
				const QByteArray& subscriptionId,
				const CGqlRequest& gqlRequest,
				const imtrest::IRequest& networkRequest,
				QString& errorMessage) = 0;
	virtual bool UnregisterSubscription(const QByteArray& subscriptionId) = 0;
};


} // namespace imtgql


