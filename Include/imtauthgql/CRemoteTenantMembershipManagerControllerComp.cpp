// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteTenantMembershipManagerControllerComp.h>
#include <imtauthsdl/SDL/1.0/CPP/TenantMemberships.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtauthgql
{


// reimplemented (sdl::V1_0::imtauth::CTenantMembershipsGqlHandlerCompBase)

sdl::V1_0::imtauth::CGetMembershipsByTenantPayload CRemoteTenantMembershipManagerControllerComp::OnGetMembershipsByTenant(
			const sdl::V1_0::imtauth::CGetMembershipsByTenantGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetMembershipsByTenantPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetMembershipsByUserPayload CRemoteTenantMembershipManagerControllerComp::OnGetMembershipsByUser(
			const sdl::V1_0::imtauth::CGetMembershipsByUserGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetMembershipsByUserPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetMembershipPayload CRemoteTenantMembershipManagerControllerComp::OnGetMembership(
			const sdl::V1_0::imtauth::CGetMembershipGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetMembershipPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CFindMembershipPayload CRemoteTenantMembershipManagerControllerComp::OnFindMembership(
			const sdl::V1_0::imtauth::CFindMembershipGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CFindMembershipPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CIsMemberPayload CRemoteTenantMembershipManagerControllerComp::OnIsMember(
			const sdl::V1_0::imtauth::CIsMemberGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CIsMemberPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CHasMinimumRolePayload CRemoteTenantMembershipManagerControllerComp::OnHasMinimumRole(
			const sdl::V1_0::imtauth::CHasMinimumRoleGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CHasMinimumRolePayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetTenantInvitationsPayload CRemoteTenantMembershipManagerControllerComp::OnGetTenantInvitations(
			const sdl::V1_0::imtauth::CGetTenantInvitationsGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetTenantInvitationsPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetMyTenantInvitationsPayload CRemoteTenantMembershipManagerControllerComp::OnGetMyTenantInvitations(
			const sdl::V1_0::imtauth::CGetMyTenantInvitationsGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetMyTenantInvitationsPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CAddMembershipPayload CRemoteTenantMembershipManagerControllerComp::OnAddMembership(
			const sdl::V1_0::imtauth::CAddMembershipGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CAddMembershipPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CCreateTenantInvitationPayload CRemoteTenantMembershipManagerControllerComp::OnCreateTenantInvitation(
			const sdl::V1_0::imtauth::CCreateTenantInvitationGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CCreateTenantInvitationPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CAcceptTenantInvitationPayload CRemoteTenantMembershipManagerControllerComp::OnAcceptTenantInvitation(
			const sdl::V1_0::imtauth::CAcceptTenantInvitationGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CAcceptTenantInvitationPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CRejectTenantInvitationPayload CRemoteTenantMembershipManagerControllerComp::OnRejectTenantInvitation(
			const sdl::V1_0::imtauth::CRejectTenantInvitationGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CRejectTenantInvitationPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CRevokeTenantInvitationPayload CRemoteTenantMembershipManagerControllerComp::OnRevokeTenantInvitation(
			const sdl::V1_0::imtauth::CRevokeTenantInvitationGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CRevokeTenantInvitationPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CResendTenantInvitationPayload CRemoteTenantMembershipManagerControllerComp::OnResendTenantInvitation(
			const sdl::V1_0::imtauth::CResendTenantInvitationGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CResendTenantInvitationPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CRemoveMembershipPayload CRemoteTenantMembershipManagerControllerComp::OnRemoveMembership(
			const sdl::V1_0::imtauth::CRemoveMembershipGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CRemoveMembershipPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CUpdateMembershipRolePayload CRemoteTenantMembershipManagerControllerComp::OnUpdateMembershipRole(
			const sdl::V1_0::imtauth::CUpdateMembershipRoleGqlRequest& /*request*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CUpdateMembershipRolePayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CTransferTenantOwnershipPayload CRemoteTenantMembershipManagerControllerComp::OnTransferTenantOwnership(
			const sdl::V1_0::imtauth::CTransferTenantOwnershipGqlRequest& /*transferTenantOwnershipRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CTransferTenantOwnershipPayload>(gqlRequest, errorMessage);
}


} // namespace imtauthgql


