// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/TSharedInterfacePtr.h>


namespace imtclientgql
{


/**
	Handle for an in-flight async GraphQL request returned by
	\c IAsyncGqlClient::SendRequest.

	The token is reference-counted via \c istd::TSharedInterfacePtr.
	Dropping the last strong reference does NOT auto-cancel the request —
	explicit \c Cancel must be called to abort it, to avoid surprising
	side-effects in fire-and-forget call sites.

	Threading / cancellation contract:
	- \c Cancel is safe to call from any thread; it transitions the token
	  to \c RS_CANCELLED and triggers exactly one final call to the related
	  \c IAsyncGqlResponseHandler::OnError with \c EC_CANCELLED.
	- \c Wait blocks the calling thread by processing its local event loop
	  (so Qt signals / timers continue to fire) until the token leaves the
	  \c RS_PENDING state or the timeout elapses.
	- \c Wait returning \c false (timeout) does NOT cancel the request;
	  the caller may follow up with \c Cancel and a second \c Wait.
*/
class IAsyncGqlRequestToken: virtual public istd::IPolymorphic
{
public:
	/**
		Lifecycle state of the request.
	*/
	enum RequestState
	{
		/**
			Request is in flight, no terminal callback yet.
		*/
		RS_PENDING = 0,

		/**
			Response received successfully.
		*/
		RS_COMPLETED,

		/**
			Request was cancelled via \c Cancel.
		*/
		RS_CANCELLED,

		/**
			Request failed (network error, timeout, internal error, ...).
		*/
		RS_FAILED
	};

	/**
		Returns the current state of the request. Safe to call from any thread.
	*/
	virtual RequestState GetState() const = 0;

	/**
		Requests cancellation of the in-flight request.
		If the request is already in a terminal state this is a no-op.
		Otherwise the related response handler is invoked exactly once
		with \c EC_CANCELLED before the state transitions to \c RS_CANCELLED.
	*/
	virtual void Cancel() = 0;

	/**
		Blocks the calling thread until the request leaves the
		\c RS_PENDING state or the optional timeout elapses.

		While waiting, the local Qt event loop continues to process events,
		matching the existing synchronous \c CRequestSender behavior.

		\param timeoutMs	Maximum wait in milliseconds. \c -1 means wait forever.
		\return \c true if the request reached a terminal state,
		        \c false if the wait timed out.
	*/
	virtual bool Wait(int timeoutMs = -1) = 0;
};


typedef istd::TSharedInterfacePtr<IAsyncGqlRequestToken> IAsyncGqlRequestTokenPtr;


} // namespace imtclientgql

