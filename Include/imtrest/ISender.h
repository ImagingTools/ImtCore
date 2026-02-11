// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
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
	virtual bool SendResponse(ConstResponsePtr& response) const = 0;
	/*!
		Send a response to the client.
		The transfer should be realized over the socket instance provided by the request object.

	*/
	virtual bool SendRequest(ConstRequestPtr& reguest) const = 0;
};


} // namespace imtrest


