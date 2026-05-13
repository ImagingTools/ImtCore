// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>


namespace imtrest
{


/**
	Common interface for dispatching responses and requests to clients.
*/
class IResponseDispatcher: virtual public istd::IPolymorphic
{
public:
	/**
		Send a response to the client identified by the given request ID.
	*/
	virtual bool SendResponse(const QByteArray& requestId, ConstResponsePtr& response) const = 0;

	/**
		Send a request to the client identified by the given request ID.
	*/
	virtual bool SendRequest(const QByteArray& requestId, ConstRequestPtr& request) const = 0;
};


} // namespace imtrest


