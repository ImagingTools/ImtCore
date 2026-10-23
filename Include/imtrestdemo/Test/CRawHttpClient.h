// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtNetwork/QTcpSocket>


/**
	Minimal HTTP client on a blocking socket for the tests of the live server. It sends exactly the bytes that the test gives, so
	the test controls the splitting of the request into parts, the pauses and the wrong requests that a real client library
	does not produce, and it reads what the server sends back: the interim responses (\c 100), the body by \c Content-Length, by \c chunked
	and until the connection is closed.
	The server must work in another thread (see imtrestdemo::CDemoServerThread).
*/
class CRawHttpClient
{
public:
	struct Response
	{
		int statusCode = 0;
		QByteArray version;
		QByteArray reasonPhrase;

		/**
			Headers with the names in the lower case, the repeated headers are joined with ", ".
		*/
		QMap<QByteArray, QByteArray> headers;
		QByteArray body;

		/**
			Head of the response as the server has sent it.
		*/
		QByteArray rawHead;

		/**
			Statuses of the interim responses (\c 100) that came before.
		*/
		QList<int> interimStatuses;

		bool isChunked = false;

		QByteArray GetHeader(const QByteArray& name) const
		{
			return headers.value(name.toLower());
		}

		bool HasHeader(const QByteArray& name) const
		{
			return headers.contains(name.toLower());
		}
	};

	typedef QList<QPair<QByteArray, QByteArray>> Headers;

	CRawHttpClient();
	~CRawHttpClient();

	bool Connect(int port, int timeoutMs = 5000);
	void Close();
	bool IsOpen() const;

	/**
		Send the bytes and wait until they are written to the socket.
	*/
	bool Send(const QByteArray& data, int timeoutMs = 10000);

	/**
		Read one response (after the interim ones).
		\param	isHeadRequest	The response is to \c HEAD: it has no body, whatever its headers say.
		\return \c false if the response is not received completely in time or the connection is closed before the end.
	*/
	bool ReadResponse(Response& response, bool isHeadRequest = false, int timeoutMs = 10000);

	/**
		Wait for an interim response (\c 100) that the server sends before the body of the request is sent. If the final response
		comes instead, it stays in the buffer for ReadResponse().
		\return \c true and the status if an interim response is received.
	*/
	bool ReadInterimResponse(int& statusCode, int timeoutMs = 5000);

	/**
		Wait until the server closes the connection. The data that come meanwhile stay in the buffer.
	*/
	bool WaitForClose(int timeoutMs = 5000);

	/**
		Number of the bytes that were received and not used by the responses.
	*/
	int GetBufferedSize() const;

	/**
		Create a request. \c Host is added if the headers do not have it, \c Content-Length is added if there is a body and the headers
		do not have it.
	*/
	static QByteArray CreateRequest(
				const QByteArray& method,
				const QByteArray& target,
				const Headers& headers = Headers(),
				const QByteArray& body = QByteArray(),
				const QByteArray& version = "HTTP/1.1");

	/**
		Encode the data as the parts of \c Transfer-Encoding: chunked.
	*/
	static QByteArray CreateChunk(const QByteArray& data);
	static QByteArray CreateLastChunk();

private:
	bool FillBuffer(int timeoutMs);
	bool ReadHead(QByteArray& head, int timeoutMs);
	bool ReadBodyByLength(qint64 length, QByteArray& body, int timeoutMs);
	bool ReadChunkedBody(QByteArray& body, int timeoutMs);
	bool ReadBodyUntilClose(QByteArray& body, int timeoutMs);
	bool ReadLine(QByteArray& line, int timeoutMs);

private:
	QTcpSocket m_socket;
	QByteArray m_buffer;
};


