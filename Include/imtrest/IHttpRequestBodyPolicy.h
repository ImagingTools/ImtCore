// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// STL includes
#include <memory>

// ImtCore includes
#include <imtrest/IHttpRequestBodySink.h>
#include <imtrest/IResponse.h>


namespace imtrest
{


class CHttpRequest;


/**
	Decision about the body of a request. It is made after the headers are received and before the body is read,
	so a request can be refused without receiving its body (for example \c 413 or \c 401).
	The policy can be called from the threads of the connections at the same time, it should not change its state.
*/
class IHttpRequestBodyPolicy: virtual public istd::IPolymorphic
{
public:
	enum BodyMode
	{
		/**
			The body is received to the memory, see BodyDecision::maxBodySize.
		*/
		BM_BUFFER,

		/**
			The body is not kept in the request, its parts are passed to BodyDecision::sinkPtr while the request is received.
			The request has an empty body. The limit BodyDecision::maxBodySize applies to the whole body.
		*/
		BM_STREAM,

		/**
			The request is answered at once with BodyDecision::rejectStatusCode without receiving of the body.
			The connection is closed after the response.
		*/
		BM_REJECT
	};

	struct BodyDecision
	{
		BodyMode mode = BM_BUFFER;

		/**
			Maximal size of the body in bytes. Less than 0 - the limit of the server, 0 - no limit.
			The request with a bigger body is refused with \c 413.
		*/
		qint64 maxBodySize = -1;

		/**
			Status of the response for BM_REJECT, a value of IProtocolEngine::StatusCode.
		*/
		int rejectStatusCode = 0;

		/**
			Receiver of the body for BM_STREAM, it is created for the request. It is shared: the request and the owner of the
			receiver (the policy or the handler that reads the result) keep it. Without it the request is refused with \c 500.
		*/
		std::shared_ptr<IHttpRequestBodySink> sinkPtr;

		/**
			Additional headers of the response for BM_REJECT, for example \c WWW-Authenticate for \c 401.
		*/
		IResponse::Headers rejectHeaders;
	};

	/**
		Decide how to receive the body of the request.
		\param	requestHead	Request with the received headers, the body is not available yet.
	*/
	virtual BodyDecision DecideBody(const CHttpRequest& requestHead) const = 0;
};


} // namespace imtrest


