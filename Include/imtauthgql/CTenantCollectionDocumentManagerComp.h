// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/IDocumentManager.h>
#include <imtbase/IObjectCollection.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/IUserManager.h>
#include <imtdoc/IDocumentManagerEventHandler.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantCollectionDocumentManager.h>


namespace imtauthgql
{


class CTenantCollectionDocumentManagerComp:
			public sdl::imtauth::TenantCollectionDocumentManager::CGraphQlHandlerCompBase,
			virtual public imtdoc::IDocumentManagerEventHandler
{
public:
	typedef sdl::imtauth::TenantCollectionDocumentManager::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantCollectionDocumentManagerComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentManagerEventHandler)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentManager", "Collection document manager", false, "CollectionDocumentManager");
		I_ASSIGN(m_tenantCollectionCompPtr, "TenantCollection", "Tenant collection for refreshing DB-computed fields after save", false, "TenantCollection");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager for memberIds", false, "MembershipManager");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Collection of users", true, "UserCollection");
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

	// reimplemented (imtdoc::IDocumentManagerEventHandler)
	virtual bool ProcessEvent(imtdoc::CEventBase* eventPtr) override;

private:
	I_REF(imtdoc::IDocumentManager, m_documentManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_tenantCollectionCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtauthgql


