#pragma once


// ImtCore includes
#include <graphqlserver/graphqlserver.h>
#include <graphqlserver/CGqlRequest.h>
#include <imtgql/CGqlRequest.h>


namespace graphqlserver
{


class CExternGraphQl
{
protected:
	graphqlserver::CGqlRequest ConvertRequest(const imtgql::CGqlRequest& gqlRequest) const
	{
		graphqlserver::CGqlRequest retVal;
		retVal.SetCommandId(gqlRequest.GetCommandId());
		retVal.SetRequestType((graphqlserver::CGqlRequest::RequestType)gqlRequest.GetRequestType());
		retVal.SetQuery(gqlRequest.GetQuery());

		return retVal;
	}
};


} // namespace graphqlserver


