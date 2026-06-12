// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/ITenantInvitationManager.h>
#include <imtauth/IDelegatedAccess.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantMemberships_fwd.h>


namespace imtauthgql
{


class CTenantMembershipManagerControllerComp: public sdl::V1_0::imtauth::CTenantMembershipsGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CTenantMembershipsGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantMembershipManagerControllerComp);
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Tenant membership manager providing CRUD operations", true, "TenantMembershipManager");
		I_ASSIGN(m_invitationManagerCompPtr, "InvitationManager", "Tenant invitation manager providing invitation lifecycle operations", true, "TenantInvitationManager");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Tenant manager for resolving tenant names", false, "TenantManager");
		I_ASSIGN(m_delegatedAccessCompPtr, "DelegatedAccess", "Optional delegated access resolver for cross-org grant read access", false, "DelegatedAccessResolver");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtauth::CTenantMembershipsGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CGetMembershipsByTenantPayload OnGetMembershipsByTenant(
				const sdl::V1_0::imtauth::CGetMembershipsByTenantGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetMembershipsByUserPayload OnGetMembershipsByUser(
				const sdl::V1_0::imtauth::CGetMembershipsByUserGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetMembershipPayload OnGetMembership(
				const sdl::V1_0::imtauth::CGetMembershipGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CFindMembershipPayload OnFindMembership(
				const sdl::V1_0::imtauth::CFindMembershipGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CIsMemberPayload OnIsMember(
				const sdl::V1_0::imtauth::CIsMemberGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CHasMinimumRolePayload OnHasMinimumRole(
				const sdl::V1_0::imtauth::CHasMinimumRoleGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetTenantInvitationsPayload OnGetTenantInvitations(
				const sdl::V1_0::imtauth::CGetTenantInvitationsGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetMyTenantInvitationsPayload OnGetMyTenantInvitations(
				const sdl::V1_0::imtauth::CGetMyTenantInvitationsGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CAddMembershipPayload OnAddMembership(
				const sdl::V1_0::imtauth::CAddMembershipGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCreateTenantInvitationPayload OnCreateTenantInvitation(
				const sdl::V1_0::imtauth::CCreateTenantInvitationGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CAcceptTenantInvitationPayload OnAcceptTenantInvitation(
				const sdl::V1_0::imtauth::CAcceptTenantInvitationGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRejectTenantInvitationPayload OnRejectTenantInvitation(
				const sdl::V1_0::imtauth::CRejectTenantInvitationGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRevokeTenantInvitationPayload OnRevokeTenantInvitation(
				const sdl::V1_0::imtauth::CRevokeTenantInvitationGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CResendTenantInvitationPayload OnResendTenantInvitation(
				const sdl::V1_0::imtauth::CResendTenantInvitationGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRemoveMembershipPayload OnRemoveMembership(
				const sdl::V1_0::imtauth::CRemoveMembershipGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CUpdateMembershipRolePayload OnUpdateMembershipRole(
				const sdl::V1_0::imtauth::CUpdateMembershipRoleGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CTransferTenantOwnershipPayload OnTransferTenantOwnership(
				const sdl::V1_0::imtauth::CTransferTenantOwnershipGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtauth::ITenantInvitationManager, m_invitationManagerCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::IDelegatedAccess, m_delegatedAccessCompPtr);
};


} // namespace imtauthgql
