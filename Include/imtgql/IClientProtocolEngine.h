#pragma once


// ImtCore includes
#include <imtgql/IGqlRequest.h>


class QNetworkRequest;


namespace imtgql
{


class IGqlRequest;


/**
	Common interface for the protocol-related communication on the client side.
	This interface represents the transport layer in the communication.
*/
class IClientProtocolEngine: virtual public istd::IPolymorphic
{
public:
	/**
		Create network (transport) request based on the GraphQL-request.
	*/
	virtual QNetworkRequest* CreateNetworkRequest(const IGqlRequest& request) const = 0;
};


} // namespace imtgql


