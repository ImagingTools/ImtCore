// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteTenantMembershipManagerControllerComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtauthgql
{


// reimplemented (sdl::imtauth::TenantMemberships::CGraphQlHandlerCompBase)

sdl::imtauth::TenantMemberships::CGetMembershipsByTenantPayload CRemoteTenantMembershipManagerControllerComp::OnGetMembershipsByTenant(
			const sdl::imtauth::TenantMemberships::CGetMembershipsByTenantGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::TenantMemberships::CGetMembershipsByTenantPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::TenantMemberships::CGetMembershipsByUserPayload CRemoteTenantMembershipManagerControllerComp::OnGetMembershipsByUser(
			const sdl::imtauth::TenantMemberships::CGetMembershipsByUserGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::TenantMemberships::CGetMembershipsByUserPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::TenantMemberships::CGetMembershipPayload CRemoteTenantMembershipManagerControllerComp::OnGetMembership(
			const sdl::imtauth::TenantMemberships::CGetMembershipGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::TenantMemberships::CGetMembershipPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::TenantMemberships::CFindMembershipPayload CRemoteTenantMembershipManagerControllerComp::OnFindMembership(
			const sdl::imtauth::TenantMemberships::CFindMembershipGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::TenantMemberships::CFindMembershipPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::TenantMemberships::CIsMemberPayload CRemoteTenantMembershipManagerControllerComp::OnIsMember(
			const sdl::imtauth::TenantMemberships::CIsMemberGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::TenantMemberships::CIsMemberPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::TenantMemberships::CHasMinimumRolePayload CRemoteTenantMembershipManagerControllerComp::OnHasMinimumRole(
			const sdl::imtauth::TenantMemberships::CHasMinimumRoleGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::TenantMemberships::CHasMinimumRolePayload>(gqlRequest, errorMessage);
}


sdl::imtauth::TenantMemberships::CGetTenantInvitationsPayload CRemoteTenantMembershipManagerControllerComp::OnGetTenantInvitations(
			const sdl::imtauth::TenantMemberships::CGetTenantInvitationsGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::TenantMemberships::CGetTenantInvitationsPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::TenantMemberships::CGetMyTenantInvitationsPayload CRemoteTenantMembershipManagerControllerComp::OnGetMyTenantInvitations(
			const sdl::imtauth::TenantMemberships::CGetMyTenantInvitationsGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::TenantMemberships::CGetMyTenantInvitationsPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::TenantMemberships::CAddMembershipPayload CRemoteTenantMembershipManagerControllerComp::OnAddMembership(
			const sdl::imtauth::TenantMemberships::CAddMembershipGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::TenantMemberships::CAddMembershipPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::TenantMemberships::CCreateTenantInvitationPayload CRemoteTenantMembershipManagerControllerComp::OnCreateTenantInvitation(
			const sdl::imtauth::TenantMemberships::CCreateTenantInvitationGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::TenantMemberships::CCreateTenantInvitationPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::TenantMemberships::CAcceptTenantInvitationPayload CRemoteTenantMembershipManagerControllerComp::OnAcceptTenantInvitation(
			const sdl::imtauth::TenantMemberships::CAcceptTenantInvitationGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::TenantMemberships::CAcceptTenantInvitationPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::TenantMemberships::CRejectTenantInvitationPayload CRemoteTenantMembershipManagerControllerComp::OnRejectTenantInvitation(
			const sdl::imtauth::TenantMemberships::CRejectTenantInvitationGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::TenantMemberships::CRejectTenantInvitationPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::TenantMemberships::CRevokeTenantInvitationPayload CRemoteTenantMembershipManagerControllerComp::OnRevokeTenantInvitation(
			const sdl::imtauth::TenantMemberships::CRevokeTenantInvitationGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::TenantMemberships::CRevokeTenantInvitationPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::TenantMemberships::CResendTenantInvitationPayload CRemoteTenantMembershipManagerControllerComp::OnResendTenantInvitation(
			const sdl::imtauth::TenantMemberships::CResendTenantInvitationGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::TenantMemberships::CResendTenantInvitationPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::TenantMemberships::CRemoveMembershipPayload CRemoteTenantMembershipManagerControllerComp::OnRemoveMembership(
			const sdl::imtauth::TenantMemberships::CRemoveMembershipGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::TenantMemberships::CRemoveMembershipPayload>(gqlRequest, errorMessage);
}


sdl::imtauth::TenantMemberships::CUpdateMembershipRolePayload CRemoteTenantMembershipManagerControllerComp::OnUpdateMembershipRole(
			const sdl::imtauth::TenantMemberships::CUpdateMembershipRoleGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtauth::TenantMemberships::CUpdateMembershipRolePayload>(gqlRequest, errorMessage);
}


} // namespace imtauthgql
