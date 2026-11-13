// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtbase/IObjectCollection.h>
#include <imtauth/ITenantInfo.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/ITenantInvitationManager.h>
#include <imtauth/IUserManager.h>
#include <imtdoc/IDocumentServiceEventHandler.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantCollectionDocumentService_fwd.h>


namespace imtauthgql
{


class CTenantCollectionDocumentServiceComp:
			public sdl::V1_0::imtauth::CTenantCollectionDocumentServiceGqlHandlerCompBase,
			virtual public imtdoc::IDocumentServiceEventHandler
{
public:
	typedef sdl::V1_0::imtauth::CTenantCollectionDocumentServiceGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantCollectionDocumentServiceComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentServiceEventHandler)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection document manager", false, "CollectionDocumentService");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager for members", false, "MembershipManager");
		I_ASSIGN(m_invitationManagerCompPtr, "InvitationManager", "Tenant invitation manager for pending invitations", false, "TenantInvitationManager");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Collection of users", true, "UserCollection");
		I_ASSIGN(m_tenantCollectionCompPtr, "TenantCollection", "Stored organizations, the source of the owner of an open organization document", false, "TenantCollection");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager, checks the permission scope against the parent and narrows the sub-organizations", false, "TenantManager");
	I_END_COMPONENT

protected:
	// reimplemented (CTenantCollectionDocumentServiceGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CTenantData OnGetTenantRepresentation(
				const sdl::V1_0::imtauth::CGetTenantRepresentationGqlRequest& getTenantRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnUpdateTenantFromRepresentation(
				const sdl::V1_0::imtauth::CUpdateTenantFromRepresentationGqlRequest& updateTenantFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (imtdoc::IDocumentServiceEventHandler)
	virtual bool ProcessEvent(imtdoc::CEventBase* eventPtr) override;

private:
	// The ownership and the scope change past the open documents, so they take both from the storage.
	void SyncStoredData(imtauth::ITenantInfo& tenant) const;

	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtauth::ITenantInvitationManager, m_invitationManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_tenantCollectionCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
};


} // namespace imtauthgql
