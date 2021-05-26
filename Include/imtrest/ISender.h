#pragma once


// ACF includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>


namespace imtrest
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
	virtual bool SendResponse(const IResponse& response) const = 0;
	/*!
		Send a response to the client.
		The transfer should be realized over the socket instance provided by the request object.

	*/
	virtual bool SendRequest(const IRequest& reguest) const = 0;
};


} // namespace imtrest


