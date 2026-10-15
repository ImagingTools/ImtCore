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
	void PerformAction(CConnectionTimeoutTracker::Action action);

private:
	QPointer<QAbstractSocket> m_socketPtr;
	CConnectionTimeoutTracker m_tracker;
	CConnectionTimeoutBehavior m_behavior;
	bool m_isFirstRequestCompleted;
	bool m_isStopped;
	QElapsedTimer m_clock;
	QTimer m_timer;
};


} // namespace imtrest


