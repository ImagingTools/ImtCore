// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtservergql/CGqlRequestHandlerCompBase.h>
#include <imtclientgql/IGqlClient.h>


namespace imtclientgql
{


/**
	GraphQL request handler redirecting file system requests
	to a file service running on another machine.
	The requests are forwarded through the assigned API client
	and the remote response is passed back to the caller.
 */
class CRemoteFileSystemControllerComp: public imtservergql::CGqlRequestHandlerCompBase
{
public:
	typedef imtservergql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRemoteFileSystemControllerComp);
		I_ASSIGN(m_apiClientCompPtr, "ApiClient", "API client connected to the remote file service", true, "ApiClient");
	I_END_COMPONENT;

protected:
	// reimplemented (CGqlRequestHandlerCompBase)
	virtual QJsonObject CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtclientgql::IGqlClient, m_apiClientCompPtr);
};


} // namespace imtclientgql
