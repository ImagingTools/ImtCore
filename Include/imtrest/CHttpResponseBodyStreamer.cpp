// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpResponseBodyStreamer.h>


// Qt includes
#include <QtCore/QDebug>


namespace imtrest
{


// public methods

CHttpResponseBodyStreamer::CHttpResponseBodyStreamer(
			QAbstractSocket& socket,
			CSocketTimeoutWatcher& watcher,
			IResponseBodySourcePtr&& sourcePtr,
			CHttpSender::BodyFraming framing,
			qint64 bodySize,
			qint64 writeHighWatermark,
			QObject* parentPtr)
	:QObject(parentPtr),
	m_socketPtr(&socket),
	m_watcher(watcher),
	m_sourcePtr(std::move(sourcePtr)),
	m_framing(framing),
	m_bodySize((bodySize >= 0) ? bodySize : -1),
	m_writeHighWatermark((writeHighWatermark > 0) ? writeHighWatermark : s_defaultWriteHighWatermark),
	m_sentSize(0),
	m_isFinished(false),
	m_isTransferring(false)
{
	connect(&socket, &QAbstractSocket::bytesWritten, this, &CHttpResponseBodyStreamer::StreamBody);
	connect(&socket, &QAbstractSocket::disconnected, this, &CHttpResponseBodyStreamer::OnSocketDisconnected);

	if (m_sourcePtr){
		// The source tells that it has the data after it gave RR_WAIT. The streamer is started by the event loop, not inside the call of the source.
		QPointer<CHttpResponseBodyStreamer> streamerPtr(this);
		m_sourcePtr->SetDataAvailableHandler([streamerPtr](){
			if (!streamerPtr.isNull()){
				QMetaObject::invokeMethod(streamerPtr.data(), &CHttpResponseBodyStreamer::StreamBody, Qt::QueuedConnection);
			}
		});
	}
}


CHttpResponseBodyStreamer::~CHttpResponseBodyStreamer()
{
	if (!m_isFinished && m_sourcePtr){
		m_isFinished = true;

		m_sourcePtr->Abort();
	}
}


void CHttpResponseBodyStreamer::Start()
{
	if (m_isFinished){
		return;
	}

	if (!m_sourcePtr){
		Fail("The source is not set");

		return;
	}

	if (m_socketPtr.isNull() || !m_socketPtr->isOpen()){
		// The disconnection is already past, no signal will tell about it
		m_isFinished = true;
		m_sourcePtr->Abort();

		Q_EMIT Finished(false);

		return;
	}

	StreamBody();
}


bool CHttpResponseBodyStreamer::IsFinished() const
{
	return m_isFinished;
}


qint64 CHttpResponseBodyStreamer::GetSentSize() const
{
	return m_sentSize;
}


// private slots

void CHttpResponseBodyStreamer::StreamBody()
{
	// Writing to the socket can emit bytesWritten() at once: the running loop continues instead of the nested one
	if (m_isFinished || m_isTransferring){
		return;
	}

	m_isTransferring = true;

	while (!m_isFinished && HasRoomInSocket()){
		if (!TransferNext()){
			break;
		}
	}

	m_isTransferring = false;
}


void CHttpResponseBodyStreamer::OnSocketDisconnected()
{
	if (m_isFinished){
		return;
	}

	m_isFinished = true;

	if (m_sourcePtr){
		m_sourcePtr->Abort();
	}

	Q_EMIT Finished(false);
}


// private methods

bool CHttpResponseBodyStreamer::HasRoomInSocket() const
{
	return !m_socketPtr.isNull() && m_socketPtr->isOpen() && (m_socketPtr->bytesToWrite() < m_writeHighWatermark);
}


bool CHttpResponseBodyStreamer::TransferNext()
{
	// The declared size is reached: the body is sent, the source is not asked for more
	if ((m_bodySize >= 0) && (m_sentSize >= m_bodySize)){
		Complete();

		return false;
	}

	qint64 maxSize = s_maxPartSize;
	if (m_bodySize >= 0){
		maxSize = qMin(maxSize, m_bodySize - m_sentSize);
	}

	QByteArray part;

	switch (m_sourcePtr->Read(part, maxSize)){
	case IResponseBodySource::RR_DATA:
		// An empty chunk would finish the chunked body
		if (part.isEmpty() || (part.size() > maxSize)){
			Fail("The source gave no data or more data than requested");

			return false;
		}

		m_watcher.WriteStreamingData((m_framing == CHttpSender::BF_CHUNKED) ? CHttpSender::CreateChunk(part) : part);
		m_sentSize += part.size();

		return true;

	case IResponseBodySource::RR_END:
		if ((m_bodySize >= 0) && (m_sentSize < m_bodySize)){
			Fail("The source ended before the declared size of the body");

			return false;
		}

		Complete();

		return false;

	case IResponseBodySource::RR_WAIT:
		// The handler of the source will start the streamer again
		return false;

	case IResponseBodySource::RR_ERROR:
		Fail("The source cannot read the body");

		return false;
	}

	return false;
}


void CHttpResponseBodyStreamer::Complete()
{
	m_isFinished = true;

	if (m_framing == CHttpSender::BF_CHUNKED){
		m_watcher.WriteStreamingData(CHttpSender::CreateLastChunk());
	}

	m_watcher.EndStreamingResponse();

	Q_EMIT Finished(true);
}


void CHttpResponseBodyStreamer::Fail(const char* reason)
{
	if (m_isFinished){
		return;
	}

	qWarning() << "The response body cannot be sent:" << reason;

	m_isFinished = true;

	if (m_sourcePtr){
		m_sourcePtr->Abort();
	}

	// The head of the response is already sent, so the only way to tell the client that the body is not complete
	// is to break the connection.
	if (!m_socketPtr.isNull()){
		m_socketPtr->abort();
	}

	Q_EMIT Finished(false);
}


} // namespace imtrest


