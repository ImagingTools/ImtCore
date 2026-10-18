// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>


namespace imtrest
{


/**
	Protocol-specific part of the handling of a connection: what to say to a client before the connection is closed
	and how the connection is used. The transport classes (CSocket, CSocketTimeoutWatcher) do not know the protocols,
	they get this description.
	The protocols are described in one place, CreateForProtocol(): another protocol (FTP, SMTP etc.) is added there
	or the behavior is created by the code that knows the protocol.
*/
struct CConnectionTimeoutBehavior
{
	/**
		Creates the message that is sent to a client that did not complete a request in time, before the closing of the
		connection. If the function is not set or returns an empty message, the connection is just aborted.
	*/
	std::function<QByteArray()> createRequestTimeoutMessage;

	/**
		Creates the message that is sent to a client that sent data that are not a valid request, before the closing of
		the connection. If the function is not set or returns an empty message, the connection is just aborted.
	*/
	std::function<QByteArray()> createBadRequestMessage;

	/**
		Creates the message that is sent to a client whose request is not answered by the handler in time (see
		CConnectionTimeoutConfig::requestProcessingMs), before the closing of the connection. If the function is not set or
		returns an empty message, the connection is just aborted.
	*/
	std::function<QByteArray()> createProcessingTimeoutMessage;

	/**
		Creates the message that is sent to a client instead of a response if the handler failed. The connection stays
		open. If the function is not set, nothing is sent.
	*/
	std::function<QByteArray()> createServerErrorMessage;

	/**
		If \c true, the connection is a persistent channel without a cycle of a request and a response: it is limited
		only before the first message, a connection that sent nothing is closed, the channel is not limited afterwards.
		If \c false, each request has a response, the keep-alive timeout is applied between the requests as well.
	*/
	bool isLimitedBeforeFirstMessageOnly = true;

	/**
		Returns \c true if each request has exactly one response: a request is processed after the response to the
		previous one, so the responses go in the order of the requests (RFC 9112, section 9.3.2).
	*/
	bool IsRequestResponseCycle() const;

	/**
		Get the behavior of the protocol with the given ID (see ProtocolTypeIds, the ID is not case-sensitive).
		The behavior of an unknown protocol is the safest one: no message and the limit only before the first message.
	*/
	static CConnectionTimeoutBehavior CreateForProtocol(const QByteArray& protocolTypeId);

	/**
		Create the \c 408 response of HTTP. It has \c Connection: \c close (RFC 9110, section 15.5.9) and \c Date, that
		is required for the responses of origin servers (RFC 9110, section 6.6.1).
		\param	dateTime	Current time.
	*/
	static QByteArray CreateHttpRequestTimeoutResponse(const QDateTime& dateTime);

	/**
		Create the \c 400 response of HTTP for data that are not a valid request. It has \c Connection: \c close
		(RFC 9112, section 2.2 and 9.6) and \c Date.
		\param	dateTime	Current time.
	*/
	static QByteArray CreateHttpBadRequestResponse(const QDateTime& dateTime);

	/**
		Create the \c 503 response of HTTP for a request that is not answered by the handler in time. It has
		\c Connection: \c close and \c Date (RFC 9110, section 15.6.4).
		\param	dateTime	Current time.
	*/
	static QByteArray CreateHttpProcessingTimeoutResponse(const QDateTime& dateTime);

	/**
		Create the \c 500 response of HTTP for a request whose handler failed (RFC 9110, section 15.6.1). It has no content,
		so a client gets no details of the failure.
		\param	dateTime			Current time.
		\param	isConnectionClosing	The connection is closed after the response, so it has \c Connection: \c close.
	*/
	static QByteArray CreateHttpInternalServerErrorResponse(const QDateTime& dateTime, bool isConnectionClosing = false);
};


} // namespace imtrest


