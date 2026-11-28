// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtassist/IAssistantHttpTransport.h>


namespace imtassist
{


/**
	HTTP transport based on imtrest::CHttpClientRequest and imtrest::CHttpClientRequestSender.
	It can be used from any thread; the cancellation token is checked while the request runs.

	\ingroup imtassist
*/
class CRestHttpTransport: virtual public IAssistantHttpTransport
{
public:
	enum
	{
		/**
			Maximum size of a response body; larger responses are treated as network errors.
		*/
		MAX_RESPONSE_SIZE = 8 * 1024 * 1024
	};

	// reimplemented (imtassist::IAssistantHttpTransport)
	virtual Response Post(const Request& request, const CCancellationToken& cancellationToken) override;
};


} // namespace imtassist


