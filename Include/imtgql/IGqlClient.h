#pragma once


// ImtCore includes
#include <imtgql/IGqlResponseHandler.h>


namespace imtgql
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
	virtual bool SendRequest(const IGqlRequest& request, IGqlResponseHandler& responseHandler) const = 0;
};


} // namespace imtgql


