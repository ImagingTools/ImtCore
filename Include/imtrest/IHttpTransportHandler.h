// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtCore/QUrl>
#include <QtNetwork/QHostAddress>

// ACF includes
#include <istd/IPolymorphic.h>


class QTcpSocket;


namespace imtrest
{


/**
	Callback interface of the HTTP transport (CHttpTransport).
	The transport is responsible only for the network I/O (TCP/TLS, HTTP/1.1 framing, keep-alive,
	WebSocket upgrade detection). Everything related to the domain model (requests, servlets,
	protocol engines) is done by the implementation of this interface.

	Threading contract:
	- \c OnHttpRequest and \c OnHttpRequestAborted are called in one of the transport's I/O threads.
	  The implementation must not block: it has to hand over the request to its own processing
	  and return immediately. The answer is delivered later through CHttpTransport::SendResponse
	  from any thread.
	- \c OnWebSocketHandshake is called in the thread of the CHttpTransport object.
*/
class IHttpTransportHandler: virtual public istd::IPolymorphic
{
public:
	typedef QList<QPair<QByteArray, QByteArray>> HeaderList;

	/**
		Transport-level HTTP request data. All data is copied out of the I/O layer,
		so the object can be freely passed between threads.
	*/
	struct RequestData
	{
		QByteArray requestId;
		QByteArray method;
		QUrl url;
		HeaderList headers;
		QByteArray body;
		QHostAddress remoteAddress;
		quint16 remotePort = 0;
		bool isSecure = false;
	};

	/**
		Transport-level HTTP response data.
		\note Content-Length is always calculated by the transport and must not be set.
	*/
	struct ResponseData
	{
		int statusCode = 200;
		HeaderList headers;
		QByteArray body;
	};

	/**
		New HTTP request was received.
		\return \c false if the request cannot be accepted (the transport answers with 503).
	*/
	virtual bool OnHttpRequest(const RequestData& request) = 0;

	/**
		The client has disconnected before the response for the given request was sent.
	*/
	virtual void OnHttpRequestAborted(const QByteArray& requestId) = 0;

	/**
		The first request on the connection is a WebSocket upgrade request.
		The socket (plain or already encrypted TLS socket) contains the complete, still unread
		handshake request and lives in the thread of the transport. The ownership is transferred
		to the implementation.
		\return \c false if the socket was not taken over (the transport closes it).
	*/
	virtual bool OnWebSocketHandshake(QTcpSocket* socketPtr) = 0;
};


} // namespace imtrest


