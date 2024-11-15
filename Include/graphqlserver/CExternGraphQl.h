#pragma once


// ImtCore includes
#include <graphqlserver/graphqlserver.h>
#include <imtgql/CGqlRequest.h>


namespace graphqlserver
{


class CExternGraphQl
{
protected:
	void CreateParams(const imtgql::CGqlObject& gqlObject, QJsonObject& result) const;
	void CreateResultKeys(const imtgql::CGqlObject& gqlObject, graphqlserver::ResultKeys& resultKeys) const;
};


} // namespace graphqlserver


