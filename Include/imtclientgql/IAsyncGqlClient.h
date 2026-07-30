// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QFuture>

// ImtCore includes
#include <imtgql/IGqlResponse.h>
#include <imtbase/IUrlParam.h>
#include <imtclientgql/IAsyncGqlResponseHandler.h>


namespace imtclientgql
{


/**
	Common interface for an asynchronous GraphQL client.

	Asynchronous counterpart of \c IGqlClient. The request is dispatched
	without blocking the calling thread and the result is delivered later
	via the supplied \c IAsyncGqlResponseHandler.

	The returned \c QFuture can be used to query the state of the request
	(\c QFuture::isFinished / \c QFuture::isCanceled), to cancel it
	(\c QFuture::cancel), or — for "wait-for-response" adapters feeding
	synchronous high-level interfaces (e.g. \c IObjectCollection) —
	to block until the response arrives (\c QFuture::waitForFinished).

	Threading / cancellation contract:
	- \c QFuture::cancel is safe to call from any thread; it triggers exactly
	  one final call to the related \c IAsyncGqlResponseHandler::OnError with
	  \c EC_CANCELLED before the future is finished.
	- A finished, non-cancelled future always carries exactly one result:
	  the received response on success, or an invalid (null) response on
	  failure (network error, timeout, internal error, ...).

	The handler is owned by the caller. It must remain alive until the
	returned future is finished.
*/
class IAsyncGqlClient: virtual public istd::IPolymorphic
{
public:
	typedef istd::TSharedInterfacePtr<imtgql::IGqlRequest> GqlRequestPtr;
	typedef istd::TSharedInterfacePtr<imtgql::IGqlResponse> GqlResponsePtr;

	/**
		Dispatch a request to the server asynchronously.

		\param requestPtr	The GraphQL request to send.
		\param handlerPtr	Callback sink invoked when the response (or an error)
		                    is available. May be \c nullptr for fire-and-forget
		                    call sites that only inspect the returned token.
		\param urlParamPtr	Optional URL parameters passed to the protocol engine.
		\return A future tracking the in-flight request. On synchronous
		        validation failure the future is returned already finished
		        with an invalid (null) response and the handler — if any —
		        is invoked synchronously before \c SendRequest returns.
	*/
	virtual QFuture<GqlResponsePtr> SendRequest(
				GqlRequestPtr requestPtr,
				IAsyncGqlResponseHandler* handlerPtr,
				imtbase::IUrlParam* urlParamPtr = nullptr) const = 0;
};


} // namespace imtclientgql

