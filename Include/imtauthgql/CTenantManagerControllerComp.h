// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/IUserManager.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants>


namespace imtauthgql
{


class CTenantManagerControllerComp: public sdl::V1_0::imtauth::CTenantsGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CTenantsGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantManagerControllerComp);
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager providing CRUD operations", true, "TenantManager");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager for member operations", true, "MembershipManager");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Collection of users", true, "UserCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CTenantsGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CGetTenantIdsPayload OnGetTenantIds(
				const sdl::V1_0::imtauth::CGetTenantIdsGqlRequest& getTenantIdsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetTenantRelationshipsPayload OnGetTenantRelationships(
				const sdl::V1_0::imtauth::CGetTenantRelationshipsGqlRequest& getTenantRelationshipsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	void extracted(sdl::V1_0::imtauth::CTenantData &tenantData,
				   QByteArrayList &membershipIds) const;
	virtual sdl::V1_0::imtauth::CGetTenantPayload OnGetTenant(
			const sdl::V1_0::imtauth::CGetTenantGqlRequest &getTenantRequest,
			const ::imtgql::CGqlRequest &gqlRequest,
			QString &errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCreateTenantPayload OnCreateTenant(
				const sdl::V1_0::imtauth::CCreateTenantGqlRequest& createTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRemoveTenantPayload OnRemoveTenant(
				const sdl::V1_0::imtauth::CRemoveTenantGqlRequest& removeTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CUpdateTenantPayload OnUpdateTenant(
				const sdl::V1_0::imtauth::CUpdateTenantGqlRequest& updateTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CSetTenantActivePayload OnSetTenantActive(
				const sdl::V1_0::imtauth::CSetTenantActiveGqlRequest& setTenantActiveRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CAddTenantRelationshipPayload OnAddTenantRelationship(
				const sdl::V1_0::imtauth::CAddTenantRelationshipGqlRequest& addTenantRelationshipRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRemoveTenantRelationshipPayload OnRemoveTenantRelationship(
				const sdl::V1_0::imtauth::CRemoveTenantRelationshipGqlRequest& removeTenantRelationshipRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtauthgql
