// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantMemberships.h>


namespace imtauthgql
{


class CRemoteTenantMembershipManagerControllerComp:
			public imtclientgql::TClientRequestManagerCompWrap<
							sdl::imtauth::TenantMemberships::CGraphQlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::imtauth::TenantMemberships::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteTenantMembershipManagerControllerComp);
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
	virtual sdl::imtauth::TenantMemberships::CGetTenantInvitationsPayload OnGetTenantInvitations(
				const sdl::imtauth::TenantMemberships::CGetTenantInvitationsGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::TenantMemberships::CGetMyTenantInvitationsPayload OnGetMyTenantInvitations(
				const sdl::imtauth::TenantMemberships::CGetMyTenantInvitationsGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::TenantMemberships::CAddMembershipPayload OnAddMembership(
				const sdl::imtauth::TenantMemberships::CAddMembershipGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::TenantMemberships::CCreateTenantInvitationPayload OnCreateTenantInvitation(
				const sdl::imtauth::TenantMemberships::CCreateTenantInvitationGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::TenantMemberships::CAcceptTenantInvitationPayload OnAcceptTenantInvitation(
				const sdl::imtauth::TenantMemberships::CAcceptTenantInvitationGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::TenantMemberships::CRejectTenantInvitationPayload OnRejectTenantInvitation(
				const sdl::imtauth::TenantMemberships::CRejectTenantInvitationGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::TenantMemberships::CRevokeTenantInvitationPayload OnRevokeTenantInvitation(
				const sdl::imtauth::TenantMemberships::CRevokeTenantInvitationGqlRequest& request,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::TenantMemberships::CResendTenantInvitationPayload OnResendTenantInvitation(
				const sdl::imtauth::TenantMemberships::CResendTenantInvitationGqlRequest& request,
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
	virtual sdl::imtauth::TenantMemberships::CTransferTenantOwnershipPayload OnTransferTenantOwnership(
				const sdl::imtauth::TenantMemberships::CTransferTenantOwnershipGqlRequest& transferTenantOwnershipRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
};


} // namespace imtauthgql
