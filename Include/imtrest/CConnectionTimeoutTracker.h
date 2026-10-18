// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QtGlobal>


namespace imtrest
{


/**
	Timeouts of a server connection. The value 0 (or a negative one) disables the timeout.
*/
struct CConnectionTimeoutConfig
{
	/**
		Time without any data while a request is being received or a response is being sent, in milliseconds.
		An incomplete request is answered with \c 408 \c Request Timeout (RFC 9110, section 15.5.9),
		a response that the client does not read is aborted.
	*/
	int requestInactivityMs = 30000;

	/**
		Time without any activity on an idle connection (before the first request and between the requests),
		in milliseconds. The server may close an idle connection at any time (RFC 9112, section 9.8).
	*/
	int keepAliveMs = 15000;

	/**
		Time to wait for the closing of the connection by the client after the \c 408 response, in milliseconds.
		The server does not close the connection at once, because the reset of the connection could erase the
		response before the client reads it (RFC 9112, section 9.6). The value 0 closes the connection at once.
	*/
	int lingeringCloseMs = 5000;

	/**
		Time of the processing of a request by the handler, from the end of the request (or of the response to the previous
		request of a pipeline) to the beginning of its response, in milliseconds. The request that is not answered in time
		gets \c 503 \c Service \c Unavailable (RFC 9110, section 15.6.4) and the connection is closed, because the late response
		would be taken for a response to the next request. The value 0 (default) means no limit: a handler can legitimately
		work long, for example a calculation.
	*/
	int requestProcessingMs = 0;
};


/**
	Tracker of the time limits of a server connection.
	It does not use timers, sockets or a system clock: the owner reports the events with the current time and asks
	for the next deadline and for the action that is due. So all rules can be checked without waiting.

	The connection is always in one of the phases:
	- \c P_IDLE: nothing is received, the keep-alive timeout is running;
	- \c P_RECEIVING: a request is partially received, the request inactivity timeout is running;
	- \c P_PROCESSING: a request is passed to the handler. The time of the handler is limited only if the processing timeout is set;
	- \c P_SENDING: a response is being written, the request inactivity timeout is running (progress of the writing);
	- \c P_CLOSING: the connection is going to be closed after the \c 408 response, the lingering timeout is running.
*/
class CConnectionTimeoutTracker
{
public:
	enum Phase
	{
		P_IDLE,
		P_RECEIVING,
		P_PROCESSING,
		P_SENDING,
		P_CLOSING
	};

	enum Action
	{
		/**
			Nothing to do.
		*/
		A_NONE,

		/**
			Close the idle connection without a response.
		*/
		A_CLOSE_IDLE,

		/**
			Answer \c 408 \c Request \c Timeout and wait for the closing of the connection (lingering close).
		*/
		A_REQUEST_TIMEOUT,

		/**
			Abort the connection: the client does not read the response.
		*/
		A_ABORT_STALLED_RESPONSE,

		/**
			Abort the connection: the client did not close it after the \c 408 response.
		*/
		A_ABORT_LINGERING,

		/**
			Answer \c 503 \c Service \c Unavailable and wait for the closing of the connection: the handler did not answer in time.
		*/
		A_PROCESSING_TIMEOUT
	};

	CConnectionTimeoutTracker();
	explicit CConnectionTimeoutTracker(const CConnectionTimeoutConfig& config);

	/**
		The connection is opened. It resets the tracker.
	*/
	void OnConnectionOpened(qint64 nowMs);

	/**
		Data of a request are received.
		The data received while a response is being prepared or sent do not affect the timeouts of that response,
		the next request starts to count when the response is finished.
	*/
	void OnDataReceived(qint64 nowMs);

	/**
		A request is completely received and passed to the handler.
	*/
	void OnRequestCompleted(qint64 nowMs);

	/**
		A response (or a message of the server) starts to be written.
	*/
	void OnResponseStarted(qint64 nowMs);

	/**
		A part of the response is written to the network.
	*/
	void OnBytesWritten(qint64 nowMs);

	/**
		The response is completely written to the network.
	*/
	void OnResponseCompleted(qint64 nowMs);

	/**
		The server sent the \c 408 response and waits for the closing of the connection.
	*/
	void OnClosing(qint64 nowMs);

	Phase GetPhase() const;

	/**
		Get the time when the nearest timeout expires or -1 if no timeout is running.
	*/
	qint64 GetDeadline() const;

	/**
		Get the action that is due at the given time.
	*/
	Action GetAction(qint64 nowMs) const;

private:
	Phase GetPhaseAfterResponse() const;

private:
	CConnectionTimeoutConfig m_config;
	Phase m_phase;
	qint64 m_lastActivityMs;

	// The data of a request that is not completed are received.
	bool m_isRequestPartial;

	// The number of the requests passed to the handler that are not answered.
	int m_outstandingRequests;
};


} // namespace imtrest


