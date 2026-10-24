// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <istd/TDelPtr.h>

// ImtCore  includes
#include <imtrest/CConnectionTimeoutBehavior.h>
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>
#include <imtrest/IStreamingResponseSender.h>


// Qt includes
#include <QtCore/QPointer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QSslConfiguration>


namespace imtrest
{


class CMultiThreadServer;
class CSocketThread;
class CSocketTimeoutWatcher;


class CSocket: public QObject
{
	Q_OBJECT
public:
	CSocket(CSocketThread* rootSocket, imtrest::IRequest* request, bool secureConnection, const QSslConfiguration& sslConfiguration, qintptr socketDescriptor);
	~CSocket();

public Q_SLOTS:
	void HandleReadyRead();
	void Disconnected();
	void OnSendResponse(QByteArray data);

	/**
		Send a response with a body that is read by parts (see IStreamingResponse): the head is written at once, the body is
		streamed while the client reads it. The next request is parsed after the whole body is sent.
		\param	response			Response, it should be IStreamingResponse.
		\param	isConnectionClosing	The connection is closed after the response (see IRequest::IsConnectionPersistent()).
		\param	isBodyOmitted		The response is the answer to \c HEAD (see IRequest::IsResponseBodyOmitted()).
		\param	isChunkedAllowed	The client supports the chunked encoding (see IRequest::IsChunkedResponseAllowed()).
	*/
	void OnSendStreamingResponse(imtrest::ConstResponsePtr response, bool isConnectionClosing, bool isBodyOmitted, bool isChunkedAllowed);
	void Abort();
	void OnHandleSslErrors(QList<QSslError> errorList);

private:
	/**
		The response to the request that is being processed is finished: the next request can be parsed.
	*/
	void FinishRequest();

	/**
		The response cannot be sent: the client gets the error message of the protocol, if it has one.
	*/
	void SendServerError();

	/**
		The data of the socket are not a valid request: the client gets the message of the protocol (for HTTP \c 400) and
		the connection is closed.
	*/
	void RejectRequest();

	/**
		The request is refused after its head, before its body is received (for HTTP \c 413 or \c 401): the client gets the
		message and the connection is closed. The request is not handled.
	*/
	void RejectRequest(const QByteArray& message);

private:
	CSocketThread* m_rootSocket;
	QPointer<QTcpSocket> m_socket;
	istd::TDelPtr<IRequest> m_requestPtr;

	// Time limits of the connection, it is a child of this object.
	CSocketTimeoutWatcher* m_timeoutWatcherPtr = nullptr;

	CConnectionTimeoutBehavior m_behavior;

	// A request is passed to the handler and its response is not written yet. The next request is not parsed meanwhile,
	// its data wait in the socket (only for the protocols with the cycle of a request and a response).
	bool m_isRequestInProgress = false;

	// Sender of the streaming responses of the protocol of the connection, it is empty if the protocol has no streaming responses.
	IStreamingResponseSenderPtr m_streamingSenderPtr;
};


} // namespace imtrest


