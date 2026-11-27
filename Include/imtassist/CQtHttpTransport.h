// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtassist/IAssistantHttpTransport.h>


namespace imtassist
{


/**
	HTTP transport based on QNetworkAccessManager.
	Each call uses its own network access manager and a local event loop, so it can be
	used from any thread. The cancellation token is checked while the request is running.

	\ingroup imtassist
*/
class CQtHttpTransport: virtual public IAssistantHttpTransport
{
public:
	enum
	{
		/**
			Maximum size of a response body; larger responses are treated as network errors.
		*/
		MAX_RESPONSE_SIZE = 8 * 1024 * 1024,
		CANCELLATION_POLL_INTERVAL_MS = 50
	};

	// reimplemented (imtassist::IAssistantHttpTransport)
	virtual Response Post(const Request& request, const CCancellationToken& cancellationToken) override;
};


} // namespace imtassist


