#pragma once


// ImtCore includes
#include <imtgql/IGqlResponse.h>
#include <imtbase/IUrlParam.h>


namespace imtclientgql
{


/**
	Common interface for a GraphQL client.
*/
class IGqlClient: virtual public istd::IPolymorphic
{
public:
	typedef QSharedPointer<imtgql::IGqlRequest> GqlRequestPtr;
	typedef QSharedPointer<imtgql::IGqlResponse> GqlResponsePtr;

	/**
		Send a request to the server.
	*/
	virtual GqlResponsePtr SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam* = nullptr) const = 0;
};


} // namespace imtgql


