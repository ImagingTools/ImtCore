// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpStreamingResponseSender.h>


// Qt includes
#include <QtCore/QDebug>

// ImtCore includes
#include <imtrest/CHttpSender.h>
#include <imtrest/IStreamingResponse.h>


namespace imtrest
{


// public methods

CHttpStreamingResponseSender::CHttpStreamingResponseSender(QAbstractSocket& socket, CSocketTimeoutWatcher& watcher, qint64 writeHighWatermark, QObject* parentPtr)
	:QObject(parentPtr),
	m_socketPtr(&socket),
	m_watcher(watcher),
	m_writeHighWatermark(writeHighWatermark)
{
}


// reimplemented (imtrest::IStreamingResponseSender)

IStreamingResponseSender::StartResult CHttpStreamingResponseSender::Start(const ConstResponsePtr& response, const Hints& hints, const FinishedHandler& onFinished)
{
	const auto* streamingResponsePtr = dynamic_cast<const IStreamingResponse*>(response.GetPtr());
	if ((streamingResponsePtr == nullptr) || !m_streamerPtr.isNull()){
		qWarning() << "The streaming response cannot be sent: it is not a streaming response or the previous one is not finished";

		return SR_FAILED;
	}

	if (m_socketPtr.isNull() || !m_socketPtr->isOpen() || m_watcher.IsClosing()){
		// The connection is closed or is being closed (for example, the handler was too slow): the response is not needed
		return SR_DROPPED;
	}

	CHttpSender::ResponseOptions options;
	options.isConnectionClosing = hints.isConnectionClosing;
	options.isBodyOmitted = hints.isBodyOmitted;
	options.isChunkedAllowed = hints.isChunkedAllowed;

	const qint64 bodySize = streamingResponsePtr->GetBodySize();

	QByteArray head;
	CHttpSender::BodyFraming framing = CHttpSender::BF_NONE;
	if (!CHttpSender::BuildStreamingResponseHead(*response, bodySize, options, head, framing)){
		qWarning() << "The head of the streaming response cannot be built, the status code is" << response->GetStatusCode();

		return SR_FAILED;
	}

	if (framing == CHttpSender::BF_NONE){
		// No body: HEAD, 304 and so on. The source is not created.
		m_watcher.WriteResponse(head);

		return SR_COMPLETED;
	}

	IResponseBodySourcePtr sourcePtr = streamingResponsePtr->CreateBodySource();
	if (!sourcePtr){
		qWarning() << "The source of the body of the response cannot be created";

		return SR_FAILED;
	}

	if (framing == CHttpSender::BF_UNTIL_CLOSE){
		// The end of the body is the closing of the connection
		m_watcher.CloseAfterResponse();
	}

	if (!m_watcher.BeginStreamingResponse(head)){
		sourcePtr->Abort();

		return SR_DROPPED;
	}

	m_onFinished = onFinished;

	m_streamerPtr = new CHttpResponseBodyStreamer(*m_socketPtr, m_watcher, std::move(sourcePtr), framing, bodySize, m_writeHighWatermark, this);

	connect(m_streamerPtr.data(), &CHttpResponseBodyStreamer::Finished, this, [this](bool /*isSuccess*/){
		OnStreamerFinished();
	});

	m_streamerPtr->Start();

	return SR_STARTED;
}


// private methods

void CHttpStreamingResponseSender::OnStreamerFinished()
{
	// The streamer is in its own call now, so it is deleted later
	if (!m_streamerPtr.isNull()){
		m_streamerPtr->deleteLater();
		m_streamerPtr.clear();
	}

	const FinishedHandler onFinished = m_onFinished;
	m_onFinished = nullptr;

	if (onFinished){
		onFinished();
	}
}


} // namespace imtrest


