// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QStringList>

// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtbase/IObjectCollection.h>
#include <imtauth/ICrossOrgGrant.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/CrossOrgGrantCollectionDocumentService_fwd.h>


namespace imtauthgql
{


/**
 * Server-side handler for the CrossOrgGrant collection's per-document representation
 * GraphQL endpoints (analogous to CRoleCollectionDocumentServiceComp).
 *
 * Reads/writes the ICrossOrgGrantData working copy that the server-side
 * CollectionDocumentManager holds for a given documentId.
 */
class CCrossOrgGrantCollectionDocumentServiceComp:
			public sdl::V1_0::imtauth::CCrossOrgGrantCollectionDocumentServiceGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CCrossOrgGrantCollectionDocumentServiceGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCrossOrgGrantCollectionDocumentServiceComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection document manager", true, "CollectionDocumentService");
		I_ASSIGN(m_tenantCollectionCompPtr, "TenantCollection", "Tenant collection for name resolution", false, "TenantCollection");
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Role collection for name resolution", false, "RoleCollection");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager deciding who manages the granting organization", true, "TenantManager");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Membership manager providing the organization permissions of members", true, "MembershipManager");
	I_END_COMPONENT

protected:
	// reimplemented (CCrossOrgGrantCollectionDocumentServiceGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CCrossOrgGrant OnGetGrantRepresentation(
				const sdl::V1_0::imtauth::CGetGrantRepresentationGqlRequest& getGrantRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnUpdateGrantFromRepresentation(
				const sdl::V1_0::imtauth::CUpdateGrantFromRepresentationGqlRequest& updateGrantFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	QString GetTenantName(const QByteArray& tenantId) const;
	QStringList GetRoleNames(const QByteArrayList& roleIds) const;
	QString ComposeDefaultGrantName(const imtauth::CrossOrgGrantInfo& info) const;

	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_tenantCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
};


} // namespace imtauthgql
