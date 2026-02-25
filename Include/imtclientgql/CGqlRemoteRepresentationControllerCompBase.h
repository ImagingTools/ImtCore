// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <imtservergql/CGqlRequestHandlerCompBase.h>
#include <imtclientgql/CApiClientComp.h>
#include <imtgql/IGqlResponse.h>


namespace imtclientgql
{


class CGqlRemoteRepresentationControllerCompBase: public imtservergql::CGqlRequestHandlerCompBase
{
public:
	typedef imtservergql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlRemoteRepresentationControllerCompBase)
		I_ASSIGN(m_apiClientCompPtr, "ApiClient", "Api Client", true, "ApiClient");
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

protected:
	imtbase::CTreeItemModel* CreateTreeItemModelFromResponse(const QByteArray& commandId, const imtgql::IGqlResponse& response) const;

protected:
	I_REF(imtclientgql::IGqlClient, m_apiClientCompPtr);
};


} // namespace imtclientgql


