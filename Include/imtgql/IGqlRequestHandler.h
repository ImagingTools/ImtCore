#pragma once


// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


class IGqlRequestHandler: virtual public istd::IPolymorphic
{
public:
	/**
		Get IDs of the supported commands.
	*/
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const = 0;

	/**
		Create response for a GraphQL-based request.
	*/
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
};


} // namespace imtgql


