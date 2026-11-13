// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CConnectionTimeoutTracker.h>


namespace imtrest
{


// public methods

CConnectionTimeoutTracker::CConnectionTimeoutTracker()
	:CConnectionTimeoutTracker(CConnectionTimeoutConfig())
{
}


CConnectionTimeoutTracker::CConnectionTimeoutTracker(const CConnectionTimeoutConfig& config)
	:m_config(config),
	m_phase(P_IDLE),
	m_lastActivityMs(0),
	m_isRequestPartial(false),
	m_outstandingRequests(0)
{
	m_config.requestInactivityMs = qMax(0, config.requestInactivityMs);
	m_config.keepAliveMs = qMax(0, config.keepAliveMs);
	m_config.lingeringCloseMs = qMax(0, config.lingeringCloseMs);
	m_config.requestProcessingMs = qMax(0, config.requestProcessingMs);
}


void CConnectionTimeoutTracker::OnConnectionOpened(qint64 nowMs)
{
	m_phase = P_IDLE;
	m_lastActivityMs = nowMs;
	m_isRequestPartial = false;
	m_outstandingRequests = 0;
}


void CConnectionTimeoutTracker::OnDataReceived(qint64 nowMs)
{
	switch (m_phase){
	case P_IDLE:
		m_phase = P_RECEIVING;
		m_isRequestPartial = true;
		m_lastActivityMs = nowMs;
		break;

	case P_RECEIVING:
		m_isRequestPartial = true;
		m_lastActivityMs = nowMs;
		break;

	case P_PROCESSING:
	case P_SENDING:
		m_isRequestPartial = true;
		break;

	case P_CLOSING:
		break;
	}
}


void CConnectionTimeoutTracker::OnRequestCompleted(qint64 nowMs)
{
	if (m_phase == P_CLOSING){
		return;
	}

	m_isRequestPartial = false;
	++m_outstandingRequests;

	if (m_phase == P_IDLE || m_phase == P_RECEIVING){
		m_phase = P_PROCESSING;
		m_lastActivityMs = nowMs;
	}
}


void CConnectionTimeoutTracker::OnResponseStarted(qint64 nowMs)
{
	if (m_phase == P_CLOSING){
		return;
	}

	if (m_outstandingRequests > 0){
		--m_outstandingRequests;
	}

	m_phase = P_SENDING;
	m_lastActivityMs = nowMs;
}


void CConnectionTimeoutTracker::OnBytesWritten(qint64 nowMs)
{
	if (m_phase == P_SENDING){
		m_lastActivityMs = nowMs;
	}
}


void CConnectionTimeoutTracker::OnResponseCompleted(qint64 nowMs)
{
	if (m_phase != P_SENDING){
		return;
	}

	m_phase = GetPhaseAfterResponse();
	m_lastActivityMs = nowMs;
}


void CConnectionTimeoutTracker::OnClosing(qint64 nowMs)
{
	m_phase = P_CLOSING;
	m_lastActivityMs = nowMs;
}


CConnectionTimeoutTracker::Phase CConnectionTimeoutTracker::GetPhase() const
{
	return m_phase;
}


qint64 CConnectionTimeoutTracker::GetDeadline() const
{
	switch (m_phase){
	case P_IDLE:
		return (m_config.keepAliveMs > 0) ? m_lastActivityMs + m_config.keepAliveMs : -1;

	case P_RECEIVING:
	case P_SENDING:
		return (m_config.requestInactivityMs > 0) ? m_lastActivityMs + m_config.requestInactivityMs : -1;

	case P_PROCESSING:
		return (m_config.requestProcessingMs > 0) ? m_lastActivityMs + m_config.requestProcessingMs : -1;

	case P_CLOSING:
		return m_lastActivityMs + m_config.lingeringCloseMs;
	}

	return -1;
}


CConnectionTimeoutTracker::Action CConnectionTimeoutTracker::GetAction(qint64 nowMs) const
{
	const qint64 deadline = GetDeadline();
	if ((deadline < 0) || (nowMs < deadline)){
		return A_NONE;
	}

	switch (m_phase){
	case P_IDLE:
		return A_CLOSE_IDLE;

	case P_RECEIVING:
		return A_REQUEST_TIMEOUT;

	case P_SENDING:
		return A_ABORT_STALLED_RESPONSE;

	case P_CLOSING:
		return A_ABORT_LINGERING;

	case P_PROCESSING:
		return A_PROCESSING_TIMEOUT;
	}

	return A_NONE;
}


// private methods

CConnectionTimeoutTracker::Phase CConnectionTimeoutTracker::GetPhaseAfterResponse() const
{
	if (m_outstandingRequests > 0){
		return P_PROCESSING;
	}

	return m_isRequestPartial ? P_RECEIVING : P_IDLE;
}


} // namespace imtrest


