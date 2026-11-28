// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtCore/QUrl>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtassist/CCancellationToken.h>


namespace imtassist
{


/**
	Blocking HTTP transport used by the provider adapters, replaceable in tests.

	\ingroup imtassist
*/
class IAssistantHttpTransport: virtual public istd::IPolymorphic
{
public:
	typedef QList<QPair<QByteArray, QByteArray>> Headers;

	struct Request
	{
		QUrl url;
		Headers headers;
		QByteArray body;
		int timeoutMs = 30000;
	};

	enum TransportStatus
	{
		/**
			An HTTP response was received; see httpStatus.
		*/
		TS_RESPONSE,
		TS_TIMEOUT,
		TS_CANCELED,
		/**
			No HTTP response, e.g. connection refused or TLS failure.
		*/
		TS_NETWORK_ERROR
	};

	struct Response
	{
		TransportStatus status = TS_NETWORK_ERROR;
		int httpStatus = 0;
		QByteArray body;
	};

	/**
		Send a POST request. Blocks until the response, the timeout or the cancellation.
	*/
	virtual Response Post(const Request& request, const CCancellationToken& cancellationToken) = 0;
};


} // namespace imtassist


