// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtNetwork/QAbstractSocket>

// ImtCore includes
#include <imtrest/ITransport.h>
#include <imtrest/IResponse.h>

namespace imtrest
{


class CHttpSender: public QObject, virtual public ITransport
{
	Q_OBJECT
public:
	/**
		How the client finds the end of the body of a response (RFC 9112, section 6.3).
	*/
	enum BodyFraming
	{
		/**
			No body is sent: the response to \c HEAD, \c 1xx, \c 204, \c 205, \c 304 or a body of the size 0.
		*/
		BF_NONE,

		/**
			The body has the size of the header \c Content-Length.
		*/
		BF_CONTENT_LENGTH,

		/**
			The body is sent as chunks (\c Transfer-Encoding: \c chunked), see CreateChunk() and CreateLastChunk().
		*/
		BF_CHUNKED,

		/**
			The body ends when the connection is closed. The response has \c Connection: \c close. It is used for a body
			of an unknown size when the client does not support the chunked encoding (\c HTTP/1.0).
		*/
		BF_UNTIL_CLOSE
	};

	/**
		Properties of the response that depend on the request it answers.
	*/
	struct ResponseOptions
	{
		/**
			The connection is closed after the response, so the response has \c Connection: \c close (RFC 9112, section 9.6).
		*/
		bool isConnectionClosing = false;

		/**
			The response is the answer to \c HEAD: the headers are sent, the content is not (RFC 9110, section 9.3.2).
		*/
		bool isBodyOmitted = false;

		/**
			The client supports \c Transfer-Encoding: \c chunked, it is a client of \c HTTP/1.1 or later (RFC 9112, section 6.1).
			It is used only for the response with a body of an unknown size, see BuildStreamingResponseHead().
		*/
		bool isChunkedAllowed = true;

		/**
			Time for the \c Date header. The current time is used if the value is not valid.
		*/
		QDateTime dateTime;
	};

	CHttpSender(QAbstractSocket* tcpSocketPtr);
	// reimplemented (ITransport)
	virtual bool SendData(const QByteArray& data) const override;

	/*!
		Assemble the full HTTP/1.1 wire bytes (status line, headers, body) for the given response.
		The result can be passed to SendData().

		The headers of the response are sent as they are, with the following rules of the standard:
		- \c Content-Length is the size of the body. A value set by the response is replaced, because a wrong length breaks
		  the next response on a persistent connection. It is not generated for \c 1xx, \c 204 and \c 304 and with
		  \c Transfer-Encoding (RFC 9110, section 8.6; RFC 9112, section 6.2). A value set for \c 304 and for the response
		  to \c HEAD (the size of the representation) is kept.
		- \c Content-Type is generated if the response has none and the data type is set.
		- \c Date is generated for the responses \c 2xx - \c 5xx if the response has none (RFC 9110, section 6.6.1).
		- The content is not sent for \c 1xx, \c 204, \c 205 and \c 304 (RFC 9110, section 6.4.1) and for \c HEAD.
		- \c Connection is controlled by the server: \c close and \c keep-alive set by the response are removed and
		  \c close is added if the connection is closing. Other values, for example \c Upgrade, are kept.
		- A header with a name that is not a token or with a value that has \c CR, \c LF or \c NUL is skipped: it would
		  allow to inject other headers or responses (RFC 9110, section 5.5).
	*/
	static bool BuildResponseData(const IResponse& response, QByteArray& data, const ResponseOptions& options);

	/*!
		The same with the default options: the connection is persistent, the content is sent, the current time is used.
	*/
	static bool BuildResponseData(const IResponse& response, QByteArray& data);

	/*!
		Assemble the head of the response with a body that is sent later by parts (see IStreamingResponse): the status line,
		the headers and the empty line. The rules of the headers are the same as of BuildResponseData() with the following
		differences. The headers \c Content-Length and \c Transfer-Encoding set by the response are not used, the framing of
		the body is decided here:
		- the size is known: \c Content-Length is the size (\c BF_CONTENT_LENGTH), a body of the size 0 is \c BF_NONE;
		- the size is not known: \c Transfer-Encoding: \c chunked (\c BF_CHUNKED) if the client supports it, otherwise the body
		  ends with the connection and the head has \c Connection: \c close (\c BF_UNTIL_CLOSE, RFC 9112, section 6.3);
		- the response to \c HEAD, \c 1xx, \c 204, \c 205 and \c 304 has no body (\c BF_NONE). For \c HEAD and \c 304 the
		  \c Content-Length is the size of the representation if it is known, otherwise the value set by the response is kept.
		\param	response	Response with the status, the headers and the data type. Its data are not used.
		\param	bodySize	Size of the body or a negative value if it is not known.
		\param	head		The head of the response.
		\param	framing		How the body is sent after the head. For \c BF_CHUNKED the parts of the body should be sent with
							CreateChunk() and finished with CreateLastChunk().
		\return \c false if the response cannot be built, for example the engine does not know its status.
	*/
	static bool BuildStreamingResponseHead(const IResponse& response, qint64 bodySize, const ResponseOptions& options, QByteArray& head, BodyFraming& framing);

	/*!
		Create a chunk of the chunked body: the size in hexadecimal, the data, each followed by CRLF (RFC 9112, section 7.1).
		\return An empty array for empty data: a chunk of the size 0 would finish the body, see CreateLastChunk().
	*/
	static QByteArray CreateChunk(const QByteArray& data);

	/*!
		Create the end of the chunked body: the chunk of the size 0 without trailers.
	*/
	static QByteArray CreateLastChunk();

private:
	struct StreamedBody
	{
		bool isStreamed = false;
		qint64 size = -1;
	};

	static bool BuildData(
				const IResponse& response,
				const QByteArray& contentData,
				const ResponseOptions& options,
				const StreamedBody& streamedBody,
				QByteArray& data,
				BodyFraming& framing);

protected:
	QAbstractSocket* m_tcpSocketPtr;
};


} // namespace imtrest


