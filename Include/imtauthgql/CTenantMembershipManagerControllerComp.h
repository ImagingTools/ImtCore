// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/ITenantMembershipManager.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantMemberships.h>


namespace imtauthgql
{


class CTenantMembershipManagerControllerComp: public sdl::imtauth::TenantMemberships::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::TenantMemberships::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantMembershipManagerControllerComp);
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager providing CRUD operations", true, "TenantMembershipManager");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::TenantMemberships::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::TenantMemberships::CGetMembershipsByTenantPayload OnGetMembershipsByTenant(
				const sdl::imtauth::TenantMemberships::CGetMembershipsByTenantGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::TenantMemberships::CGetMembershipsByUserPayload OnGetMembershipsByUser(
				const sdl::imtauth::TenantMemberships::CGetMembershipsByUserGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::TenantMemberships::CGetMembershipPayload OnGetMembership(
				const sdl::imtauth::TenantMemberships::CGetMembershipGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::TenantMemberships::CFindMembershipPayload OnFindMembership(
				const sdl::imtauth::TenantMemberships::CFindMembershipGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::TenantMemberships::CIsMemberPayload OnIsMember(
				const sdl::imtauth::TenantMemberships::CIsMemberGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::TenantMemberships::CHasMinimumRolePayload OnHasMinimumRole(
				const sdl::imtauth::TenantMemberships::CHasMinimumRoleGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::TenantMemberships::CAddMembershipPayload OnAddMembership(
				const sdl::imtauth::TenantMemberships::CAddMembershipGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::TenantMemberships::CRemoveMembershipPayload OnRemoveMembership(
				const sdl::imtauth::TenantMemberships::CRemoveMembershipGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::TenantMemberships::CUpdateMembershipRolePayload OnUpdateMembershipRole(
				const sdl::imtauth::TenantMemberships::CUpdateMembershipRoleGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
};


} // namespace imtauthgql
