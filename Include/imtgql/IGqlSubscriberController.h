#pragma once


// ImtCore includes
#include <imtbase/CTreeItemModel.h>
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
	virtual bool RegisterSubscribtion(
				const QByteArray& subscriptionId, 
				const imtgql::CGqlRequest& gqlRequest,  
				const imtrest::IRequest& networkRequest,
				QString& errorMessage) = 0;
	virtual bool UnRegisterSubscribtion(const QByteArray& subscriptionId) = 0;
};


} // namespace imtgql


