#pragma once


// ImtCore includes
#include <imtgql/IGqlResponseHandler.h>


namespace imtclientgql
{


/**
	Common interface for a GraphQL client.
*/
class IGqlClient: virtual public istd::IPolymorphic
{
public:
	/**
		Send a request to the server.
	*/
	virtual bool SendRequest(const imtgql::IGqlRequest& request, imtgql::IGqlResponseHandler& responseHandler) const = 0;
};


} // namespace imtgql


