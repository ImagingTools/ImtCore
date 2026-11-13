// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>
#include <memory>

// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtrest
{


/**
	Source of the body of a response that is read and sent by parts, so the body is never completely in memory.
	It is the counterpart of IHttpRequestBodySink: the sink receives the body of a request, the source gives the body of a response.

	The source lives in the thread of the connection: it is created there (see IStreamingResponse::CreateBodySource()),
	and all its methods are called from that thread only, so a source can use Qt objects of that thread
	(files, network replies) without locks.

	The streamer of the connection (CHttpResponseBodyStreamer) reads the source only while the socket has room for the data, so a slow
	client slows the source down: the data are not read ahead of the network.
*/
class IResponseBodySource: virtual public istd::IPolymorphic
{
public:
	enum ReadResult
	{
		/**
			The data are read, at least one byte.
		*/
		RR_DATA,

		/**
			The body is completely read.
		*/
		RR_END,

		/**
			No data are available now (for example, they are not yet received from another server), but the body is not
			finished. The source calls the handler set by SetDataAvailableHandler() when the next Read() can give the data.
		*/
		RR_WAIT,

		/**
			The body cannot be read (for example, a read error). The response cannot be completed: it is already started,
			so the connection is aborted.
		*/
		RR_ERROR
	};

	typedef std::function<void()> DataAvailableHandler;

	/**
		Read the next part of the body.
		\param	data	The part of the body, it is replaced. It should not be longer than \c maxSize and should not be empty if
						the result is \c RR_DATA.
		\param	maxSize	Maximal number of bytes to read, always positive. If the size of the body is declared (the header
						\c Content-Length), \c maxSize does not go beyond the declared end.
	*/
	virtual ReadResult Read(QByteArray& data, qint64 maxSize) = 0;

	/**
		Set the function that tells that the data are available after \c RR_WAIT. It is set once, before the first Read().
		It may be called later than \c RR_WAIT was returned (then it does no harm: the streamer just checks the source) and
		should be called from the thread of the connection.
	*/
	virtual void SetDataAvailableHandler(const DataAvailableHandler& handler) = 0;

	/**
		The response cannot be completed: the client has disconnected, the connection is aborted, the source gave an error
		or the declared size does not match the data. The source should release everything it holds (close the file,
		cancel the request to another server). It is called at most once and never after the body is completely read.
		The destructor of the source should release the resources too, as for a source that is not aborted.
	*/
	virtual void Abort() = 0;
};


typedef std::unique_ptr<IResponseBodySource> IResponseBodySourcePtr;


} // namespace imtrest


