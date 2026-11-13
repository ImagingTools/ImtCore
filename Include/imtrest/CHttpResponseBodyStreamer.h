// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtNetwork/QAbstractSocket>

// ImtCore includes
#include <imtrest/CHttpSender.h>
#include <imtrest/CSocketTimeoutWatcher.h>
#include <imtrest/IResponseBodySource.h>


namespace imtrest
{


/**
	Streamer of the body of a response: it reads the body from the source by parts and writes it to the socket of the connection
	(see IStreamingResponse). The head of the response is already written by CSocketTimeoutWatcher::BeginStreamingResponse().

	The streamer reads the source only while the socket has not too much unsent data (\c writeHighWatermark), and goes on when the
	data are written to the network (\c bytesWritten). So a slow client slows the source down and the memory does not depend
	on the size of the body: at most the high watermark and one part of the source are in memory.

	The streamer finishes when:
	- the body is completely sent (\c Finished(true)). The watcher is told, the response completes as soon as the data are sent
	  to the network;
	- the source gives an error, ends before the declared size or gives more data than declared: the response cannot be
	  completed correctly and it is already started, so the connection is aborted and the source is aborted;
	- the connection is disconnected (the client has gone, the watcher has aborted a stalled response): the source is aborted.

	The streamer lives in the thread of the socket.
*/
class CHttpResponseBodyStreamer: public QObject
{
	Q_OBJECT
public:
	/**
		Maximal size of a part that is read from the source at a time, in bytes.
	*/
	static constexpr qint64 s_maxPartSize = 256LL * 1024;

	/**
		Limit of the unsent data of the socket that is used if the given one is not positive, in bytes.
	*/
	static constexpr qint64 s_defaultWriteHighWatermark = 1024LL * 1024;

	/**
		\param	socket				Socket of the connection. It should be connected.
		\param	watcher				Watcher of the timeouts of the connection, it should have the started streaming response.
		\param	sourcePtr			Source of the body, the streamer owns it.
		\param	framing				How the body is sent: \c BF_CONTENT_LENGTH, \c BF_CHUNKED or \c BF_UNTIL_CLOSE.
		\param	bodySize			The declared size of the body or a negative value if it is not known. It is the size from
									the header \c Content-Length: the source should give exactly so many bytes.
		\param	writeHighWatermark	The streamer does not read the source while the socket has at least so many unsent bytes.
	*/
	CHttpResponseBodyStreamer(
				QAbstractSocket& socket,
				CSocketTimeoutWatcher& watcher,
				IResponseBodySourcePtr&& sourcePtr,
				CHttpSender::BodyFraming framing,
				qint64 bodySize,
				qint64 writeHighWatermark,
				QObject* parentPtr = nullptr);

	/**
		The source is aborted if the body is not finished.
	*/
	virtual ~CHttpResponseBodyStreamer();

	/**
		Start to send the body.
	*/
	void Start();

	/**
		Returns \c true if the streamer has finished successfully or not.
	*/
	bool IsFinished() const;

	/**
		Get the number of the bytes of the body that are given by the source and written to the socket.
	*/
	qint64 GetSentSize() const;

Q_SIGNALS:
	/**
		The streamer has finished.
		\param	isSuccess	\c true if the whole body is written to the socket, \c false if the response is broken.
	*/
	void Finished(bool isSuccess);

private Q_SLOTS:
	void StreamBody();
	void OnSocketDisconnected();

private:
	bool HasRoomInSocket() const;

	/**
		Read one part of the source and write it. Returns \c true if the next part can be read at once.
	*/
	bool TransferNext();

	void Complete();
	void Fail(const char* reason);

private:
	QPointer<QAbstractSocket> m_socketPtr;
	CSocketTimeoutWatcher& m_watcher;
	IResponseBodySourcePtr m_sourcePtr;
	CHttpSender::BodyFraming m_framing;
	qint64 m_bodySize;
	qint64 m_writeHighWatermark;
	qint64 m_sentSize;
	bool m_isFinished;
	bool m_isTransferring;
};


} // namespace imtrest


