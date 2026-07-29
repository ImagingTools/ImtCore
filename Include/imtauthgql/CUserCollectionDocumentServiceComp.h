// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QMap>
#include <QMutex>

// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtdoc/IDocumentServiceEventHandler.h>
#include <imtcrypt/IHashGenerator.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/ITenantEntityBindingManager.h>
#include <imtauth/IDelegatedAccess.h>
#include <imtauth/IRoleInfoProvider.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/UserCollectionDocumentService_fwd.h>


namespace imtauthgql
{


/**
 * Server-side handler for the User collection's per-document representation
 * GraphQL endpoints (analogous to CTenantCollectionDocumentServiceComp).
 *
 * Reads/writes the IUserInfo working copy that the server-side
 * CollectionDocumentManager holds for a given documentId.
 *
 * Also implements IDocumentServiceEventHandler to react to CDocumentSavedEvent
 * and auto-add the newly created user as a Member of the originating tenant.
 */
class CUserCollectionDocumentServiceComp:
			public sdl::V1_0::imtauth::CUserCollectionDocumentServiceGqlHandlerCompBase,
			virtual public imtdoc::IDocumentServiceEventHandler
{
public:
	typedef sdl::V1_0::imtauth::CUserCollectionDocumentServiceGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserCollectionDocumentServiceComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentServiceEventHandler)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection document manager", false, "CollectionDocumentService");
		I_ASSIGN(m_hashCalculatorCompPtr, "HashCalculator", "Hash calculator for password hashing", true, "HashCalculator");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager for auto-adding membership on user save", false, "MembershipManager");
		I_ASSIGN(m_bindingManagerCompPtr, "BindingManager", "Tenant entity binding manager for tenant-scoped adaptation of user roles/groups/permissions", false, "TenantEntityBindingManager");
		I_ASSIGN(m_delegatedAccessCompPtr, "DelegatedAccess", "Delegated access resolver used to enrich the user with roles delegated via cross-org grants", false, "DelegatedAccessResolver");
		I_ASSIGN(m_roleInfoProviderCompPtr, "RoleInfoProvider", "Role info provider for delegated role product validation", false, "RoleInfoProvider");
	I_END_COMPONENT

protected:
	// reimplemented (CUserCollectionDocumentServiceGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CUserData OnGetUserRepresentation(
				const sdl::V1_0::imtauth::CGetUserRepresentationGqlRequest& getUserRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnUpdateUserFromRepresentation(
				const sdl::V1_0::imtauth::CUpdateUserFromRepresentationGqlRequest& updateUserFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (imtdoc::IDocumentServiceEventHandler)
	virtual bool ProcessEvent(imtdoc::CEventBase* eventPtr) override;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
	I_REF(imtcrypt::IHashGenerator, m_hashCalculatorCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtauth::ITenantEntityBindingManager, m_bindingManagerCompPtr);
	I_REF(imtauth::IDelegatedAccess, m_delegatedAccessCompPtr);
	I_REF(imtauth::IRoleInfoProvider, m_roleInfoProviderCompPtr);

	mutable QMutex m_pendingTenantIdsMutex;
	mutable QMap<QByteArray, QByteArray> m_pendingTenantIds; ///< documentId -> tenantId, stored during OnUpdateUserFromRepresentation
};


} // namespace imtauthgql
