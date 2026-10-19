// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtNetwork/QAbstractSocket>

// ImtCore includes
#include <imtrest/CConnectionTimeoutBehavior.h>
#include <imtrest/CConnectionTimeoutTracker.h>


namespace imtrest
{


/**
	Watcher of the time limits of a server connection (see CConnectionTimeoutTracker).
	It does not know any protocol: the protocol-specific part is given as CConnectionTimeoutBehavior.
	The owner of the connection reports the received data, the completed requests and writes the responses
	through the watcher. The watcher closes the connection when a timeout expires:
	- an idle connection is closed without a message (for example, RFC 9112, section 9.8 for HTTP);
	- for an incomplete request the message of the behavior is sent (for HTTP it is \c 408 \c Request \c Timeout, see
	  CConnectionTimeoutBehavior). The connection is not closed at once, the watcher waits for the closing by the client
	  and discards the received data, to avoid the reset of the connection that could erase the message
	  (RFC 9112, section 9.6). If the client does not close the connection, it is aborted after the lingering timeout.
	  Without a message the connection is just aborted;
	- a response that the client does not read is aborted.
	A persistent channel (see CConnectionTimeoutBehavior::isLimitedBeforeFirstMessageOnly) is limited only before the
	first message.
*/
class CSocketTimeoutWatcher: public QObject
{
	Q_OBJECT
public:
	/**
		\param	socket		Connected socket of the connection. It should live in the thread of the watcher.
		\param	config		Time limits.
		\param	behavior	Protocol-specific part of the timeouts.
	*/
	CSocketTimeoutWatcher(QAbstractSocket& socket, const CConnectionTimeoutConfig& config, const CConnectionTimeoutBehavior& behavior, QObject* parentPtr = nullptr);

	/**
		Returns \c true if the connection is being closed by the watcher or is closed.
		The data received by a closing connection should be discarded.
	*/
	bool IsClosing() const;

	CConnectionTimeoutTracker::Phase GetPhase() const;

	/**
		The request that is being processed asks to close the connection after the response (for HTTP \c Connection: \c close
		or \c HTTP/1.0, RFC 9112, section 9.3). It should be called after OnRequestCompleted() of the request.
		The connection is closed when the response is written and no other response is waited. The data received
		afterwards are not a request (the client should not send them) and should be discarded, see IsCloseRequested().
	*/
	void CloseAfterResponse();

	/**
		Returns \c true if the connection will be closed after the response, see CloseAfterResponse().
	*/
	bool IsCloseRequested() const;

	/**
		The received data are not a valid request. The message of the behavior is sent (for HTTP it is \c 400 \c Bad
		\c Request, RFC 9112, section 2.2) and the connection is closed as after a timeout of an incomplete request:
		it waits for the closing by the client, the data received meanwhile are discarded, see IsClosing().
		Without a message the connection is just aborted.
	*/
	void RejectRequest();

	/**
		The request is refused after its head (for HTTP the policy of the body, for example \c 413). The same as RejectRequest(),
		but the given message is sent instead of the message of the behavior. Without a message the connection is just aborted.
	*/
	void RejectRequest(const QByteArray& message);

	/**
		Data of a request are received from the socket.
	*/
	void OnDataReceived();

	/**
		A request is completely received and passed to the handler.
	*/
	void OnRequestCompleted();

	/**
		Write a response to the socket. It should be used for each response instead of the writing to the socket.
		Data are ignored if the connection is closing.
	*/
	void WriteResponse(const QByteArray& data);

	/**
		Write an interim message that is not a response to the request (for HTTP \c 100 \c Continue). The request is still
		received, so the phase of the connection does not change. Data are ignored if the connection is closing.
	*/
	void WriteInterim(const QByteArray& data);

Q_SIGNALS:
	/**
		A timeout has expired and the action is performed.
		\param	action	Value of CConnectionTimeoutTracker::Action.
	*/
	void TimedOut(int action);

private Q_SLOTS:
	void OnTimer();
	void OnBytesWritten();
	void OnSocketDisconnected();

private:
	qint64 GetNow() const;
	void UpdateTimer();
	void CompleteResponseIfWritten();
	void CloseConnection();
	void CloseWithMessage(const QByteArray& message);
	void PerformAction(CConnectionTimeoutTracker::Action action);

private:
	QPointer<QAbstractSocket> m_socketPtr;
	CConnectionTimeoutTracker m_tracker;
	CConnectionTimeoutBehavior m_behavior;
	bool m_isFirstRequestCompleted;
	bool m_isCloseRequested;
	bool m_isStopped;
	QElapsedTimer m_clock;
	QTimer m_timer;
};


} // namespace imtrest


