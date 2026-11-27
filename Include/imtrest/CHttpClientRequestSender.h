// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>

// ImtCore includes
#include <imtrest/IHttpClientRequest.h>


namespace imtrest
{


class CHttpClientRequest;


/**
	Executes an outgoing HTTP request synchronously.

	The request runs in a local event loop with its own network access manager, so it can
	be sent from any thread. It ends with the reply, the timeout or the cancellation; a
	running request is aborted. The reply data is copied into the request.
*/
class CHttpClientRequestSender
{
public:
	enum SendResult
	{
		/**
			A reply was received (it may report an HTTP or network error).
		*/
		SR_FINISHED,
		SR_TIMEOUT,
		SR_CANCELED,
		/**
			The request is invalid or its operation is not supported.
		*/
		SR_INVALID_REQUEST,
		/**
			The reply was larger than the allowed size.
		*/
		SR_TOO_LARGE
	};

	/**
		Returns true if the request should be cancelled. Called repeatedly while the request runs.
	*/
	typedef std::function<bool()> CancellationCheck;

	enum
	{
		DEFAULT_MAX_REPLY_SIZE = 8 * 1024 * 1024,
		CANCELLATION_POLL_INTERVAL_MS = 50
	};

	/**
		Send the request: GET, POST, PUT, DELETE or HEAD, according to its request type.
		\param timeoutMs     Maximum duration of the request.
		\param isCanceled    Optional cancellation check.
		\param maxReplySize  Maximum size of the reply body.
	*/
	static SendResult Send(
				CHttpClientRequest& request,
				int timeoutMs,
				const CancellationCheck& isCanceled = CancellationCheck(),
				qint64 maxReplySize = DEFAULT_MAX_REPLY_SIZE);
};


} // namespace imtrest


