#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtrest
{


class IResponse;


/**
	Interface describing transfer of the server response to the client.
*/
class IResponder: virtual public istd::IPolymorphic
{
public:
	/**
		Send a response to the client.
		The transfer should be realized over the socket instance provided by the response object.
	*/
	virtual bool SendResponse(const IResponse& response) const = 0;
};


} // namespace imtrest


