// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>
#include <memory>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtrest/IResponse.h>


namespace imtrest
{


/**
	Protocol-specific part of sending of a response whose body is read by parts (see IStreamingResponse): how the head of the
	response is made, how the end of the body is shown to the client and how the parts of the body are framed.
	The transport classes (CSocket) do not know the protocols, so they send the streaming responses through this interface,
	that is created for the protocol of the connection (see CConnectionTimeoutBehavior::createStreamingResponseSender).
	The sender lives in the thread of the connection and serves one response at a time.
*/
class IStreamingResponseSender: virtual public istd::IPolymorphic
{
public:
	/**
		Properties of the request that is answered. They are known by the request (see IRequest).
	*/
	struct Hints
	{
		/**
			The connection is closed after the response (see IRequest::IsConnectionPersistent()).
		*/
		bool isConnectionClosing = false;

		/**
			The response is sent without its body, for example the answer to \c HEAD (see IRequest::IsResponseBodyOmitted()).
		*/
		bool isBodyOmitted = false;

		/**
			The client can get a body of an unknown size in the parts that are not limited by the declared length (see
			IRequest::IsChunkedResponseAllowed()).
		*/
		bool isChunkedAllowed = false;
	};

	enum StartResult
	{
		/**
			The body is being sent. The handler is called when the response is finished, successfully or not.
		*/
		SR_STARTED,

		/**
			The whole response is written and there is no body to send: the handler is not called.
		*/
		SR_COMPLETED,

		/**
			The response cannot be sent and nothing is written: the client should get the error message of the protocol. The handler
			is not called.
		*/
		SR_FAILED,

		/**
			The response is not needed, because the connection is closed or is being closed. Nothing is written, the handler is not called.
		*/
		SR_DROPPED
	};

	typedef std::function<void()> FinishedHandler;

	/**
		Start to send the response.
		\param	response	Response, it should be IStreamingResponse. It is not needed after the call.
		\param	hints		Properties of the request that is answered.
		\param	onFinished	Called when the body is sent or the sending is broken, only if the result is \c SR_STARTED.
	*/
	virtual StartResult Start(const ConstResponsePtr& response, const Hints& hints, const FinishedHandler& onFinished) = 0;
};


typedef std::unique_ptr<IStreamingResponseSender> IStreamingResponseSenderPtr;


} // namespace imtrest


