#pragma once


// ImtCore includes
#include <imtgql/IGqlRequest.h>


namespace imtgql
{


/**
	Common interface for a GraphQL client.
*/
class IGqlClient: virtual public istd::IPolymorphic
{
public:
	/**
		Interface to handle server response on the given client request.
	*/
	class ResponseHandler: virtual public istd::IPolymorphic
	{
	public:
		/**
			Callback on reply for a given request.
		*/
		virtual void OnReply(const IGqlRequest& request, const QByteArray& replyData) = 0;
	};

	/**
		Send a request to the server.
	*/
	virtual bool SendRequest(const IGqlRequest& request, ResponseHandler& responseHandler) const = 0;
};


} // namespace imtgql


