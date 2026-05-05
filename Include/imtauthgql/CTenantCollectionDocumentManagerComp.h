// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/IDocumentManager.h>
#include <imtbase/IObjectCollection.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantCollectionDocumentManager.h>


namespace imtauthgql
{


class CTenantCollectionDocumentManagerComp:
			public sdl::imtauth::TenantCollectionDocumentManager::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::TenantCollectionDocumentManager::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantCollectionDocumentManagerComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentManager", "Collection document manager", false, "CollectionDocumentManager");
		I_ASSIGN(m_tenantCollectionCompPtr, "TenantCollection", "Tenant collection for refreshing DB-computed fields after save", false, "TenantCollection");
	I_END_COMPONENT

protected:
	// reimplemented (CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Tenants::CTenantData OnGetTenantRepresentation(
				const sdl::imtauth::TenantCollectionDocumentManager::CGetTenantRepresentationGqlRequest& getTenantRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus OnUpdateTenantFromRepresentation(
				const sdl::imtauth::TenantCollectionDocumentManager::CUpdateTenantFromRepresentationGqlRequest& updateTenantFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtdoc::IDocumentManager, m_documentManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_tenantCollectionCompPtr);
};


} // namespace imtauthgql
