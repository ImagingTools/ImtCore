// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imthttp/IRequest.h>
#include <imthttp/IResponse.h>


namespace imthttp
{


/**
	Interface describing the transport layer for sending responses and requests to the client.
*/
class ITransport: virtual public istd::IPolymorphic
{
public:
	/**
		Send a response to the client.
		The transfer should be realized over the socket instance provided by the response object.
	*/
	virtual bool SendResponse(ConstResponsePtr& response) const = 0;
	/*!
		Send a request to the client.
		The transfer should be realized over the socket instance provided by the request object.

	*/
	virtual bool SendRequest(ConstRequestPtr& request) const = 0;
};


} // namespace imthttp


