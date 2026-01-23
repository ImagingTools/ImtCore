#pragma once


// ImtCore includes
#include <imthttp/IRequest.h>
#include <imthttp/IResponse.h>


namespace imthttp
{


/**
	Interface describing transfer of the server response to the client.
*/
class ISender: virtual public istd::IPolymorphic
{
public:
	/**
		Send a response to the client.
		The transfer should be realized over the socket instance provided by the response object.
	*/
	virtual bool SendResponse(ConstResponsePtr& response) const = 0;
	/*!
		Send a response to the client.
		The transfer should be realized over the socket instance provided by the request object.

	*/
	virtual bool SendRequest(ConstRequestPtr& reguest) const = 0;
};


} // namespace imthttp


