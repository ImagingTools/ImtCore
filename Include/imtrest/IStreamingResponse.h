// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrest/IResponse.h>
#include <imtrest/IResponseBodySource.h>


namespace imtrest
{


/**
	Response with a body that is read and sent by parts (see IResponseBodySource), for example a file or the answer of
	another server. The status, the headers and the data type are the same as of IResponse, the body is not in memory.

	It is an IResponse, so the servlets and the dispatcher of the responses work with it as usual: a servlet returns it
	as any other response. The server sends only the head of the response at once and the body part by part
	while the client reads (see CHttpResponseBodyStreamer).

	The server uses the response from the thread of the connection after the servlet has returned it, and the request
	may be already destroyed. So the response must not refer to the request: everything the source needs should be
	copied to the response.
*/
class IStreamingResponse: virtual public IResponse
{
public:
	/**
		Get the size of the body in bytes or -1 if it is not known.
		The size is sent as \c Content-Length (RFC 9110, section 8.6). Without it the body is sent with
		\c Transfer-Encoding: \c chunked (RFC 9112, section 7.1) or, for a client of \c HTTP/1.0, until the connection is closed.
		For the response to \c HEAD and \c 304 the size is the size of the representation, the body is not sent and the source is
		not created.
		The source should give exactly this number of bytes.
	*/
	virtual qint64 GetBodySize() const = 0;

	/**
		Create the source of the body. It is called at most once, in the thread of the connection, only if the body is sent.
		The response can be destroyed right after the call, so the source must not refer to the response: it should hold
		everything it needs (for example, the open file).
		\return \c nullptr if the source cannot be created (for example, the file cannot be opened): the client gets \c 500.
	*/
	virtual IResponseBodySourcePtr CreateBodySource() const = 0;
};


} // namespace imtrest


