// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CClientRequestTenantInfoProviderComp.h>


// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantCollectionDocumentManager.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtauth::ITenantInfoProvider)

const imtbase::ICollectionInfo& CClientRequestTenantInfoProviderComp::GetTenantList() const
{
	static imtbase::CCollectionInfo emptyCollection;

	return emptyCollection;
}


imtauth::ITenantInfoSharedPtr CClientRequestTenantInfoProviderComp::GetTenant(const QByteArray& tenantId, const iprm::IParamsSet* /*paramsPtr*/) const
{
	return nullptr;
}


} // namespace imtauthgql


