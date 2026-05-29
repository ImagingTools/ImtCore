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


class CTenantManagerControllerComp: public sdl::imtauth::Tenants::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::Tenants::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantManagerControllerComp);
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager providing CRUD operations", true, "TenantManager");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager for member operations", true, "MembershipManager");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "Collection of users", true, "UserCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::Tenants::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Tenants::CGetTenantIdsPayload OnGetTenantIds(
				const sdl::imtauth::Tenants::CGetTenantIdsGqlRequest& getTenantIdsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CGetTenantRelationshipsPayload OnGetTenantRelationships(
				const sdl::imtauth::Tenants::CGetTenantRelationshipsGqlRequest& getTenantRelationshipsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	void extracted(sdl::imtauth::Tenants::CTenantData::V1_0 &tenantData,
				   QByteArrayList &membershipIds) const;
	virtual sdl::imtauth::Tenants::CGetTenantPayload OnGetTenant(
			const sdl::imtauth::Tenants::CGetTenantGqlRequest &getTenantRequest,
			const ::imtgql::CGqlRequest &gqlRequest,
			QString &errorMessage) const override;
	virtual sdl::imtauth::Tenants::CCreateTenantPayload OnCreateTenant(
				const sdl::imtauth::Tenants::CCreateTenantGqlRequest& createTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CRemoveTenantPayload OnRemoveTenant(
				const sdl::imtauth::Tenants::CRemoveTenantGqlRequest& removeTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CUpdateTenantPayload OnUpdateTenant(
				const sdl::imtauth::Tenants::CUpdateTenantGqlRequest& updateTenantRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CSetTenantActivePayload OnSetTenantActive(
				const sdl::imtauth::Tenants::CSetTenantActiveGqlRequest& setTenantActiveRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CAddTenantRelationshipPayload OnAddTenantRelationship(
				const sdl::imtauth::Tenants::CAddTenantRelationshipGqlRequest& addTenantRelationshipRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Tenants::CRemoveTenantRelationshipPayload OnRemoveTenantRelationship(
				const sdl::imtauth::Tenants::CRemoveTenantRelationshipGqlRequest& removeTenantRelationshipRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
};


} // namespace imtauthgql
