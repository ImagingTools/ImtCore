// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtNetwork/QAbstractSocket>

// ImtCore includes
#include <imtrest/ITransport.h>
#include <imtrest/IResponse.h>

namespace imtrest
{


class CHttpSender: public QObject, virtual public ITransport
{
	Q_OBJECT
public:
	/**
		Properties of the response that depend on the request it answers.
	*/
	struct ResponseOptions
	{
		/**
			The connection is closed after the response, so the response has \c Connection: \c close (RFC 9112, section 9.6).
		*/
		bool isConnectionClosing = false;

		/**
			The response is the answer to \c HEAD: the headers are sent, the content is not (RFC 9110, section 9.3.2).
		*/
		bool isBodyOmitted = false;

		/**
			Time for the \c Date header. The current time is used if the value is not valid.
		*/
		QDateTime dateTime;
	};

	CHttpSender(QAbstractSocket* tcpSocketPtr);
	// reimplemented (ITransport)
	virtual bool SendData(const QByteArray& data) const override;

	/*!
		Assemble the full HTTP/1.1 wire bytes (status line, headers, body) for the given response.
		The result can be passed to SendData().

		The headers of the response are sent as they are, with the following rules of the standard:
		- \c Content-Length is the size of the body. A value set by the response is replaced, because a wrong length breaks
		  the next response on a persistent connection. It is not generated for \c 1xx, \c 204 and \c 304 and with
		  \c Transfer-Encoding (RFC 9110, section 8.6; RFC 9112, section 6.2). A value set for \c 304 and for the response
		  to \c HEAD (the size of the representation) is kept.
		- \c Content-Type is generated if the response has none and the data type is set.
		- \c Date is generated for the responses \c 2xx - \c 5xx if the response has none (RFC 9110, section 6.6.1).
		- The content is not sent for \c 1xx, \c 204, \c 205 and \c 304 (RFC 9110, section 6.4.1) and for \c HEAD.
		- \c Connection is controlled by the server: \c close and \c keep-alive set by the response are removed and
		  \c close is added if the connection is closing. Other values, for example \c Upgrade, are kept.
		- A header with a name that is not a token or with a value that has \c CR, \c LF or \c NUL is skipped: it would
		  allow to inject other headers or responses (RFC 9110, section 5.5).
	*/
	static bool BuildResponseData(const IResponse& response, QByteArray& data, const ResponseOptions& options);

	/*!
		The same with the default options: the connection is persistent, the content is sent, the current time is used.
	*/
	static bool BuildResponseData(const IResponse& response, QByteArray& data);

protected:
	QAbstractSocket* m_tcpSocketPtr;
};


} // namespace imtrest


