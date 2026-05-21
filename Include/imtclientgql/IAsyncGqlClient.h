// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlResponse.h>
#include <imtbase/IUrlParam.h>
#include <imtclientgql/IAsyncGqlResponseHandler.h>
#include <imtclientgql/IAsyncGqlRequestToken.h>


namespace imtclientgql
{


/**
	Common interface for an asynchronous GraphQL client.

	Asynchronous counterpart of \c IGqlClient. The request is dispatched
	without blocking the calling thread and the result is delivered later
	via the supplied \c IAsyncGqlResponseHandler.

	The returned \c IAsyncGqlRequestTokenPtr can be used to query the state
	of the request, to cancel it, or — for "wait-for-response" adapters
	feeding synchronous high-level interfaces (e.g. \c IObjectCollection) —
	to block until the response arrives.

	The handler is owned by the caller. It must remain alive until the token
	leaves the \c IAsyncGqlRequestToken::S_PENDING state (see the
	"Threading & cancellation contract" notes on \c IAsyncGqlRequestToken).
*/
class IAsyncGqlClient: virtual public istd::IPolymorphic
{
public:
	typedef istd::TSharedInterfacePtr<imtgql::IGqlRequest> GqlRequestPtr;
	typedef istd::TSharedInterfacePtr<imtgql::IGqlResponse> GqlResponsePtr;

	/**
		Dispatch a request to the server asynchronously.

		\param requestPtr The GraphQL request to send.
		\param handlerPtr Callback sink invoked when the response (or an error)
		                  is available. May be \c nullptr for fire-and-forget
		                  call sites that only inspect the returned token.
		\param urlParamPtr Optional URL parameters passed to the protocol engine.
		\return A token tracking the in-flight request. The token is always
		        valid; on synchronous validation failure it is returned in a
		        terminal state (\c S_FAILED) and the handler — if any —
		        is invoked synchronously before \c SendRequest returns.
	*/
	virtual IAsyncGqlRequestTokenPtr SendRequest(
				GqlRequestPtr requestPtr,
				IAsyncGqlResponseHandler* handlerPtr,
				imtbase::IUrlParam* urlParamPtr = nullptr) const = 0;
};


} // namespace imtclientgql

