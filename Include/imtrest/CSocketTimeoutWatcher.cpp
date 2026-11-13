// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CSocketTimeoutWatcher.h>


// STL includes
#include <limits>


namespace imtrest
{


// public methods

CSocketTimeoutWatcher::CSocketTimeoutWatcher(QAbstractSocket& socket, const CConnectionTimeoutConfig& config, const CConnectionTimeoutBehavior& behavior, QObject* parentPtr)
	:QObject(parentPtr),
	m_socketPtr(&socket),
	m_tracker(config),
	m_behavior(behavior),
	m_isFirstRequestCompleted(false),
	m_isCloseRequested(false),
	m_isStopped(false),
	m_isStreaming(false)
{
	m_clock.start();

	m_timer.setSingleShot(true);
	m_timer.setTimerType(Qt::PreciseTimer);

	connect(&m_timer, &QTimer::timeout, this, &CSocketTimeoutWatcher::OnTimer);
	connect(&socket, &QAbstractSocket::bytesWritten, this, &CSocketTimeoutWatcher::OnBytesWritten);
	connect(&socket, &QAbstractSocket::disconnected, this, &CSocketTimeoutWatcher::OnSocketDisconnected);

	m_tracker.OnConnectionOpened(GetNow());

	UpdateTimer();
}


bool CSocketTimeoutWatcher::IsClosing() const
{
	return m_isStopped || (m_tracker.GetPhase() == CConnectionTimeoutTracker::P_CLOSING);
}


CConnectionTimeoutTracker::Phase CSocketTimeoutWatcher::GetPhase() const
{
	return m_tracker.GetPhase();
}


void CSocketTimeoutWatcher::CloseAfterResponse()
{
	m_isCloseRequested = true;
}


bool CSocketTimeoutWatcher::IsCloseRequested() const
{
	return m_isCloseRequested;
}


void CSocketTimeoutWatcher::RejectRequest()
{
	if (IsClosing() || m_socketPtr.isNull()){
		return;
	}

	CloseWithMessage(m_behavior.createBadRequestMessage ? m_behavior.createBadRequestMessage() : QByteArray());
}


void CSocketTimeoutWatcher::RejectRequest(const QByteArray& message)
{
	if (IsClosing() || m_socketPtr.isNull()){
		return;
	}

	CloseWithMessage(message);
}


void CSocketTimeoutWatcher::OnDataReceived()
{
	if (IsClosing() || m_isCloseRequested){
		return;
	}

	m_tracker.OnDataReceived(GetNow());

	UpdateTimer();
}


void CSocketTimeoutWatcher::OnRequestCompleted()
{
	if (IsClosing() || m_isCloseRequested){
		return;
	}

	if (m_behavior.isLimitedBeforeFirstMessageOnly && !m_isFirstRequestCompleted){
		// The persistent channel is not limited after its first message.
		CConnectionTimeoutConfig unlimitedConfig;
		unlimitedConfig.requestInactivityMs = 0;
		unlimitedConfig.keepAliveMs = 0;

		m_tracker = CConnectionTimeoutTracker(unlimitedConfig);
		m_tracker.OnConnectionOpened(GetNow());
	}

	m_isFirstRequestCompleted = true;

	m_tracker.OnRequestCompleted(GetNow());

	UpdateTimer();
}


void CSocketTimeoutWatcher::WriteResponse(const QByteArray& data)
{
	if (m_socketPtr.isNull() || IsClosing()){
		return;
	}

	// The writing can emit bytesWritten() at once, so the phase is changed before it.
	m_tracker.OnResponseStarted(GetNow());

	m_socketPtr->write(data);

	CompleteResponseIfWritten();

	UpdateTimer();
}


bool CSocketTimeoutWatcher::BeginStreamingResponse(const QByteArray& head)
{
	if (m_socketPtr.isNull() || IsClosing()){
		return false;
	}

	// The flag is set before the writing: it can emit bytesWritten() at once and the response must not complete with its head.
	m_isStreaming = true;

	m_tracker.OnResponseStarted(GetNow());

	m_socketPtr->write(head);

	UpdateTimer();

	return true;
}


void CSocketTimeoutWatcher::WriteStreamingData(const QByteArray& data)
{
	if (m_socketPtr.isNull() || IsClosing() || !m_isStreaming){
		return;
	}

	m_socketPtr->write(data);
}


void CSocketTimeoutWatcher::EndStreamingResponse()
{
	if (!m_isStreaming){
		return;
	}

	m_isStreaming = false;

	CompleteResponseIfWritten();

	UpdateTimer();
}


void CSocketTimeoutWatcher::WriteInterim(const QByteArray& data)
{
	if (m_socketPtr.isNull() || IsClosing()){
		return;
	}

	m_socketPtr->write(data);
}


// private slots

void CSocketTimeoutWatcher::OnTimer()
{
	const CConnectionTimeoutTracker::Action action = m_tracker.GetAction(GetNow());
	if (action == CConnectionTimeoutTracker::A_NONE){
		// The timer can fire a little earlier than the deadline.
		UpdateTimer();

		return;
	}

	PerformAction(action);

	Q_EMIT TimedOut(int(action));
}


void CSocketTimeoutWatcher::OnBytesWritten()
{
	if (m_isStopped){
		return;
	}

	m_tracker.OnBytesWritten(GetNow());

	CompleteResponseIfWritten();

	UpdateTimer();
}


void CSocketTimeoutWatcher::OnSocketDisconnected()
{
	m_isStopped = true;

	m_timer.stop();
}


// private methods

qint64 CSocketTimeoutWatcher::GetNow() const
{
	return m_clock.elapsed();
}


void CSocketTimeoutWatcher::UpdateTimer()
{
	const qint64 deadline = m_tracker.GetDeadline();
	if (m_isStopped || (deadline < 0)){
		m_timer.stop();

		return;
	}

	const qint64 remaining = qMax<qint64>(0, qMin<qint64>(deadline - GetNow(), std::numeric_limits<int>::max()));

	m_timer.start(int(remaining));
}


void CSocketTimeoutWatcher::CompleteResponseIfWritten()
{
	if (!m_socketPtr.isNull() && !m_isStreaming && (m_tracker.GetPhase() == CConnectionTimeoutTracker::P_SENDING) && (m_socketPtr->bytesToWrite() == 0)){
		m_tracker.OnResponseCompleted(GetNow());

		if (m_isCloseRequested && (m_tracker.GetPhase() != CConnectionTimeoutTracker::P_PROCESSING)){
			CloseConnection();
		}
	}
}


void CSocketTimeoutWatcher::CloseWithMessage(const QByteArray& message)
{
	if (!message.isEmpty()){
		m_tracker.OnClosing(GetNow());
		m_socketPtr->write(message);
		UpdateTimer();
	}
	else{
		m_isStopped = true;
		m_timer.stop();
		m_socketPtr->abort();
	}
}


void CSocketTimeoutWatcher::CloseConnection()
{
	m_isStopped = true;
	m_timer.stop();

	if (!m_socketPtr.isNull()){
		m_socketPtr->disconnectFromHost();
	}
}


void CSocketTimeoutWatcher::PerformAction(CConnectionTimeoutTracker::Action action)
{
	if (m_socketPtr.isNull()){
		m_isStopped = true;
		m_timer.stop();

		return;
	}

	switch (action){
	case CConnectionTimeoutTracker::A_CLOSE_IDLE:
		CloseConnection();
		break;

	case CConnectionTimeoutTracker::A_REQUEST_TIMEOUT:
		CloseWithMessage(m_behavior.createRequestTimeoutMessage ? m_behavior.createRequestTimeoutMessage() : QByteArray());
		break;

	case CConnectionTimeoutTracker::A_PROCESSING_TIMEOUT:
		// The late response of the handler would be taken for the response to the next request, so the connection is closed
		CloseWithMessage(m_behavior.createProcessingTimeoutMessage ? m_behavior.createProcessingTimeoutMessage() : QByteArray());
		break;

	case CConnectionTimeoutTracker::A_ABORT_STALLED_RESPONSE:
	case CConnectionTimeoutTracker::A_ABORT_LINGERING:
		m_isStopped = true;
		m_timer.stop();
		m_socketPtr->abort();
		break;

	case CConnectionTimeoutTracker::A_NONE:
		break;
	}
}


} // namespace imtrest


