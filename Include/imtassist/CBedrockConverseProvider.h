// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtassist/CHttpAssistantProviderBase.h>


namespace imtassist
{


/**
	Adapter to the Amazon Bedrock Converse API, authenticated by Bedrock API key or AWS Signature Version 4.

	\ingroup imtassist
*/
class CBedrockConverseProvider: public CHttpAssistantProviderBase
{
protected:
	// reimplemented (imtassist::CHttpAssistantProviderBase)
	virtual bool HasCredentials(const Settings& settings) const override;
	virtual bool CreateHttpRequest(
				const Settings& settings,
				const Request& request,
				const ToolNames& toolNames,
				const Turns& turns,
				IAssistantHttpTransport::Request& httpRequest) const override;
	virtual Reply ParseResponseBody(const QByteArray& body, const ToolNames& toolNames) const override;
};


} // namespace imtassist


