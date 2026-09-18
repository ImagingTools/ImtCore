// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlResponse.h>
#include <imtbase/IUrlParam.h>


namespace imtclientgql
{


/**
	Common interface for a GraphQL client.
*/
class IGqlClient: virtual public istd::IPolymorphic
{
public:
	typedef istd::TSharedInterfacePtr<imtgql::IGqlRequest> GqlRequestPtr;
	typedef istd::TSharedInterfacePtr<imtgql::IGqlResponse> GqlResponsePtr;

	/**
		Send a request to the server.
	*/
	virtual GqlResponsePtr SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam* urlParamPtr = nullptr) const = 0;

	/**
		Send a request without waiting for the response (fire-and-forget).

		For callers that only care whether the request was handed to the transport, not
		what the server answered - avoids the SendRequest() nested-event-loop wait entirely.
		Default implementation reports "unsupported"; transports capable of a non-blocking
		send should reimplement this instead of falling back to SendRequest().
	*/
	virtual bool SendRequestNoWait(GqlRequestPtr requestPtr, imtbase::IUrlParam* urlParamPtr = nullptr) const = 0;
};


} // namespace imtgql


