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
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const = 0;

	/**
		Register subscriprion for a GraphQL-based request.
	*/
	virtual bool RegisterSubscription(
				const QByteArray& subscriptionId,
				const imtgql::CGqlRequest& gqlRequest,
				const imtrest::IRequest& networkRequest,
				QString& errorMessage) = 0;
	virtual bool UnRegisterSubscription(const QByteArray& subscriptionId) = 0;
};


} // namespace imtgql


