#pragma once


// ImtCore includes
#include <imtgql/IGqlRequest.h>


namespace imtgql
{


/**
	Common interface for a GraphQL client.
*/
class IGqlResponseHandler : virtual public istd::IPolymorphic
{
public:
	/**
		Callback on reply for a given request.
	*/
	virtual void OnReply(const IGqlRequest& request, const QByteArray& replyData) = 0;
};


} // namespace imtgql


