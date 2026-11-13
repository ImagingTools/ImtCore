// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtNetwork/QAbstractSocket>

// ImtCore includes
#include <imtrest/CHttpResponseBodyStreamer.h>
#include <imtrest/CSocketTimeoutWatcher.h>
#include <imtrest/IStreamingResponseSender.h>


namespace imtrest
{


/**
	Sender of the streaming responses of HTTP. It builds the head of the response (CHttpSender::BuildStreamingResponseHead()),
	creates the source of the body and sends the body by parts with CHttpResponseBodyStreamer, with the framing that the head
	declares: \c Content-Length, \c chunked or the end of the connection (RFC 9112, section 6.3).
	The responses without a body (\c HEAD, \c 304, a body of the size 0) are written as the head only, and the source is not created.
	The sender lives in the thread of the socket.
*/
class CHttpStreamingResponseSender: public QObject, virtual public IStreamingResponseSender
{
public:
	/**
		\param	socket				Socket of the connection. It should be connected.
		\param	watcher				Watcher of the timeouts of the connection, the responses are written through it.
		\param	writeHighWatermark	See CHttpResponseBodyStreamer.
	*/
	CHttpStreamingResponseSender(QAbstractSocket& socket, CSocketTimeoutWatcher& watcher, qint64 writeHighWatermark, QObject* parentPtr = nullptr);

	// reimplemented (imtrest::IStreamingResponseSender)
	virtual StartResult Start(const ConstResponsePtr& response, const Hints& hints, const FinishedHandler& onFinished) override;

private:
	void OnStreamerFinished();

private:
	QPointer<QAbstractSocket> m_socketPtr;
	CSocketTimeoutWatcher& m_watcher;
	qint64 m_writeHighWatermark;
	FinishedHandler m_onFinished;

	// Streamer of the body of the response that is being sent, it is a child of this object.
	QPointer<CHttpResponseBodyStreamer> m_streamerPtr;
};


} // namespace imtrest


