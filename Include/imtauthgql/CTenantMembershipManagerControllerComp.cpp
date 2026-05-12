// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantMembershipManagerControllerComp.h>

// ImtCore includes
#include <imtgql/IGqlContext.h>


namespace
{


QByteArray ContextUserId(const imtgql::CGqlRequest& gqlRequest)
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	return contextPtr != nullptr ? contextPtr->GetUserId() : QByteArray();
}


sdl::imtauth::TenantMemberships::CTenantInvitationData::V1_0 ToTenantInvitationData(
		const imtauth::ITenantInvitation& invitation,
		imtauth::ITenantInvitation::TenantInvitationStatus status)
{
	sdl::imtauth::TenantMemberships::CTenantInvitationData::V1_0 data;
	data.id = invitation.GetInvitationId();
	data.userId = invitation.GetUserId();
	data.tenantId = invitation.GetTenantId();
	data.role = static_cast<sdl::imtauth::TenantMemberships::TenantMemberRole>(invitation.GetRole());
	data.status = static_cast<sdl::imtauth::TenantMemberships::TenantInvitationStatus>(status);
	data.invitedByUserId = invitation.GetInvitedByUserId();
	data.createdAt = invitation.GetCreatedAt();
	data.updatedAt = invitation.GetUpdatedAt();
	data.expiresAt = invitation.GetExpiresAt();
	data.acceptedAt = invitation.GetAcceptedAt();
	data.rejectedAt = invitation.GetRejectedAt();
	data.revokedAt = invitation.GetRevokedAt();
	data.revokedByUserId = invitation.GetRevokedByUserId();
	data.lastSentAt = invitation.GetLastSentAt();
	return data;
}


bool CanManageTenant(const imtauth::ITenantMembershipManager& membershipManager, const QByteArray& userId, const QByteArray& tenantId)
{
	return membershipManager.HasMinimumRole(userId, tenantId, imtauth::ITenantMembership::TMR_ADMIN) ||
			membershipManager.HasMinimumRole(userId, tenantId, imtauth::ITenantMembership::TMR_OWNER);
}


} // anonymous namespace


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::imtauth::TenantMemberships::CGraphQlHandlerCompBase)

sdl::imtauth::TenantMemberships::CGetMembershipsByTenantPayload CTenantMembershipManagerControllerComp::OnGetMembershipsByTenant(
	const sdl::imtauth::TenantMemberships::CGetMembershipsByTenantGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CGetMembershipsByTenantPayload response;

	if (!m_invitationManagerCompPtr.IsValid() || !m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'InvitationManager' or 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray tenantId;
	sdl::imtauth::TenantMemberships::GetMembershipsByTenantRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}

	QByteArrayList ids = m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId);
	response.Version_1_0->membershipIds.Emplace();
	for (const auto& id : ids){
		response.Version_1_0->membershipIds->push_back(id);
	}

	return response;
}


sdl::imtauth::TenantMemberships::CGetMembershipsByUserPayload CTenantMembershipManagerControllerComp::OnGetMembershipsByUser(
	const sdl::imtauth::TenantMemberships::CGetMembershipsByUserGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CGetMembershipsByUserPayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray userId;
	sdl::imtauth::TenantMemberships::GetMembershipsByUserRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->userId){
		userId = *arguments.input.Version_1_0->userId;
	}

	QByteArrayList ids = m_membershipManagerCompPtr->GetMembershipsByUser(userId);
	response.Version_1_0->membershipIds.Emplace();
	for (const auto& id : ids){
		response.Version_1_0->membershipIds->push_back(id);
	}

	return response;
}


sdl::imtauth::TenantMemberships::CGetMembershipPayload CTenantMembershipManagerControllerComp::OnGetMembership(
	const sdl::imtauth::TenantMemberships::CGetMembershipGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CGetMembershipPayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray membershipId;
	sdl::imtauth::TenantMemberships::GetMembershipRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->membershipId){
		membershipId = *arguments.input.Version_1_0->membershipId;
	}

	imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);

	if (!membershipPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Membership not found");
		return response;
	}

	sdl::imtauth::TenantMemberships::CTenantMembershipData::V1_0 membershipData;
	membershipData.id = membershipPtr->GetMembershipId();
	membershipData.userId = membershipPtr->GetUserId();
	membershipData.tenantId = membershipPtr->GetTenantId();
	membershipData.role = static_cast<sdl::imtauth::TenantMemberships::TenantMemberRole>(membershipPtr->GetRole());
	membershipData.isActive = membershipPtr->IsActive();
	membershipData.joinedAt = membershipPtr->GetJoinedAt();
	membershipData.updatedAt = membershipPtr->GetUpdatedAt();

	response.Version_1_0->membership = membershipData;

	return response;
}


sdl::imtauth::TenantMemberships::CFindMembershipPayload CTenantMembershipManagerControllerComp::OnFindMembership(
	const sdl::imtauth::TenantMemberships::CFindMembershipGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CFindMembershipPayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray userId;
	QByteArray tenantId;
	sdl::imtauth::TenantMemberships::FindMembershipRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->userId){
		userId = *arguments.input.Version_1_0->userId;
	}
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}

	imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->FindMembership(userId, tenantId);

	if (!membershipPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Membership not found");
		return response;
	}

	sdl::imtauth::TenantMemberships::CTenantMembershipData::V1_0 membershipData;
	membershipData.id = membershipPtr->GetMembershipId();
	membershipData.userId = membershipPtr->GetUserId();
	membershipData.tenantId = membershipPtr->GetTenantId();
	membershipData.role = static_cast<sdl::imtauth::TenantMemberships::TenantMemberRole>(membershipPtr->GetRole());
	membershipData.isActive = membershipPtr->IsActive();
	membershipData.joinedAt = membershipPtr->GetJoinedAt();
	membershipData.updatedAt = membershipPtr->GetUpdatedAt();

	response.Version_1_0->membership = membershipData;

	return response;
}


sdl::imtauth::TenantMemberships::CIsMemberPayload CTenantMembershipManagerControllerComp::OnIsMember(
	const sdl::imtauth::TenantMemberships::CIsMemberGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CIsMemberPayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray userId;
	QByteArray tenantId;
	sdl::imtauth::TenantMemberships::IsMemberRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->userId){
		userId = *arguments.input.Version_1_0->userId;
	}
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}

	response.Version_1_0->isMember = m_membershipManagerCompPtr->IsMember(userId, tenantId);

	return response;
}


sdl::imtauth::TenantMemberships::CHasMinimumRolePayload CTenantMembershipManagerControllerComp::OnHasMinimumRole(
	const sdl::imtauth::TenantMemberships::CHasMinimumRoleGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CHasMinimumRolePayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray userId;
	QByteArray tenantId;
	sdl::imtauth::TenantMemberships::HasMinimumRoleRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->userId){
		userId = *arguments.input.Version_1_0->userId;
	}
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}

	imtauth::ITenantMembership::TenantMemberRole minimumRole = imtauth::ITenantMembership::TMR_MEMBER;
	if (arguments.input.Version_1_0->minimumRole){
		minimumRole = static_cast<imtauth::ITenantMembership::TenantMemberRole>(*arguments.input.Version_1_0->minimumRole);
	}

	response.Version_1_0->hasRole = m_membershipManagerCompPtr->HasMinimumRole(userId, tenantId, minimumRole);

	return response;
}


sdl::imtauth::TenantMemberships::CGetTenantInvitationsPayload CTenantMembershipManagerControllerComp::OnGetTenantInvitations(
	const sdl::imtauth::TenantMemberships::CGetTenantInvitationsGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CGetTenantInvitationsPayload response;
	response.Version_1_0.emplace();

	if (!m_invitationManagerCompPtr.IsValid() || !m_membershipManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Invitation manager is not configured");
		return response;
	}

	QByteArray tenantId;
	imtauth::ITenantInvitationManager::Statuses statuses;
	sdl::imtauth::TenantMemberships::GetTenantInvitationsRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}
	if (arguments.input.Version_1_0->statuses){
		for (const auto& status : *arguments.input.Version_1_0->statuses){
			statuses.append(static_cast<imtauth::ITenantInvitation::TenantInvitationStatus>(*status));
		}
	}

	if (!CanManageTenant(*m_membershipManagerCompPtr.GetPtr(), ContextUserId(gqlRequest), tenantId)){
		response.Version_1_0->errorMessage = QStringLiteral("Access denied");
		return response;
	}

	response.Version_1_0->invitations.Emplace();
	for (const QByteArray& invitationId : m_invitationManagerCompPtr->GetInvitationsByTenant(tenantId, statuses)){
		imtauth::ITenantInvitationUniquePtr invitationPtr = m_invitationManagerCompPtr->GetInvitation(invitationId);
		if (invitationPtr.IsValid()){
			response.Version_1_0->invitations->push_back(ToTenantInvitationData(*invitationPtr, m_invitationManagerCompPtr->GetEffectiveStatus(*invitationPtr)));
		}
	}

	return response;
}


sdl::imtauth::TenantMemberships::CGetMyTenantInvitationsPayload CTenantMembershipManagerControllerComp::OnGetMyTenantInvitations(
	const sdl::imtauth::TenantMemberships::CGetMyTenantInvitationsGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CGetMyTenantInvitationsPayload response;
	response.Version_1_0.emplace();

	if (!m_invitationManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Invitation manager is not configured");
		return response;
	}

	imtauth::ITenantInvitationManager::Statuses statuses;
	sdl::imtauth::TenantMemberships::GetMyTenantInvitationsRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->statuses){
		for (const auto& status : *arguments.input.Version_1_0->statuses){
			statuses.append(static_cast<imtauth::ITenantInvitation::TenantInvitationStatus>(*status));
		}
	}

	response.Version_1_0->invitations.Emplace();
	for (const QByteArray& invitationId : m_invitationManagerCompPtr->GetInvitationsByUser(ContextUserId(gqlRequest), statuses)){
		imtauth::ITenantInvitationUniquePtr invitationPtr = m_invitationManagerCompPtr->GetInvitation(invitationId);
		if (invitationPtr.IsValid()){
			response.Version_1_0->invitations->push_back(ToTenantInvitationData(*invitationPtr, m_invitationManagerCompPtr->GetEffectiveStatus(*invitationPtr)));
		}
	}

	return response;
}


sdl::imtauth::TenantMemberships::CAddMembershipPayload CTenantMembershipManagerControllerComp::OnAddMembership(
	const sdl::imtauth::TenantMemberships::CAddMembershipGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CAddMembershipPayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray userId;
	QByteArray tenantId;
	imtauth::ITenantMembership::TenantMemberRole role = imtauth::ITenantMembership::TMR_MEMBER;
	sdl::imtauth::TenantMemberships::AddMembershipRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->userId){
		userId = *arguments.input.Version_1_0->userId;
	}
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}
	if (arguments.input.Version_1_0->role){
		role = static_cast<imtauth::ITenantMembership::TenantMemberRole>(*arguments.input.Version_1_0->role);
	}

	QByteArray membershipId = m_membershipManagerCompPtr->AddMembership(userId, tenantId, role);

	if (membershipId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to add membership");
		return response;
	}

	response.Version_1_0->membershipId = membershipId;

	return response;
}


sdl::imtauth::TenantMemberships::CRemoveMembershipPayload CTenantMembershipManagerControllerComp::OnRemoveMembership(
	const sdl::imtauth::TenantMemberships::CRemoveMembershipGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CRemoveMembershipPayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray membershipId;
	sdl::imtauth::TenantMemberships::RemoveMembershipRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->membershipId){
		membershipId = *arguments.input.Version_1_0->membershipId;
	}

	bool success = m_membershipManagerCompPtr->RemoveMembership(membershipId);

	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to remove membership");
	}

	return response;
}


sdl::imtauth::TenantMemberships::CInviteTenantMemberPayload CTenantMembershipManagerControllerComp::OnInviteTenantMember(
	const sdl::imtauth::TenantMemberships::CInviteTenantMemberGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CInviteTenantMemberPayload response;

	if (!m_invitationManagerCompPtr.IsValid() || !m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'InvitationManager' or 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray userId;
	QByteArray tenantId;
	imtauth::ITenantMembership::TenantMemberRole role = imtauth::ITenantMembership::TMR_MEMBER;
	sdl::imtauth::TenantMemberships::InviteTenantMemberRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->userId){
		userId = *arguments.input.Version_1_0->userId;
	}
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}
	if (arguments.input.Version_1_0->role){
		role = static_cast<imtauth::ITenantMembership::TenantMemberRole>(*arguments.input.Version_1_0->role);
	}

	QByteArray contextUserId = ContextUserId(gqlRequest);
	if (!CanManageTenant(*m_membershipManagerCompPtr.GetPtr(), contextUserId, tenantId)){
		response.Version_1_0->errorMessage = QStringLiteral("Access denied");
		return response;
	}

	QByteArray invitationId = m_invitationManagerCompPtr->CreateInvitation(contextUserId, userId, tenantId, role);
	if (invitationId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to invite tenant member");
		return response;
	}

	response.Version_1_0->membershipId = invitationId;

	return response;
}


sdl::imtauth::TenantMemberships::CCreateTenantInvitationPayload CTenantMembershipManagerControllerComp::OnCreateTenantInvitation(
	const sdl::imtauth::TenantMemberships::CCreateTenantInvitationGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CCreateTenantInvitationPayload response;
	response.Version_1_0.emplace();

	if (!m_invitationManagerCompPtr.IsValid() || !m_membershipManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Invitation manager is not configured");
		return response;
	}

	QByteArray userId;
	QByteArray tenantId;
	imtauth::ITenantMembership::TenantMemberRole role = imtauth::ITenantMembership::TMR_MEMBER;
	sdl::imtauth::TenantMemberships::CreateTenantInvitationRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->userId){
		userId = *arguments.input.Version_1_0->userId;
	}
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}
	if (arguments.input.Version_1_0->role){
		role = static_cast<imtauth::ITenantMembership::TenantMemberRole>(*arguments.input.Version_1_0->role);
	}

	QByteArray contextUserId = ContextUserId(gqlRequest);
	if (!CanManageTenant(*m_membershipManagerCompPtr.GetPtr(), contextUserId, tenantId)){
		response.Version_1_0->errorMessage = QStringLiteral("Access denied");
		return response;
	}

	QByteArray invitationId = m_invitationManagerCompPtr->CreateInvitation(contextUserId, userId, tenantId, role);
	if (invitationId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to create tenant invitation");
		return response;
	}

	response.Version_1_0->invitationId = invitationId;
	return response;
}


sdl::imtauth::TenantMemberships::CAcceptTenantInvitationPayload CTenantMembershipManagerControllerComp::OnAcceptTenantInvitation(
	const sdl::imtauth::TenantMemberships::CAcceptTenantInvitationGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CAcceptTenantInvitationPayload response;
	response.Version_1_0.emplace();
	response.Version_1_0->success = false;

	if (!m_invitationManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Invitation manager is not configured");
		return response;
	}

	QByteArray invitationId;
	sdl::imtauth::TenantMemberships::AcceptTenantInvitationRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->invitationId){
		invitationId = *arguments.input.Version_1_0->invitationId;
	}

	QByteArray membershipId = m_invitationManagerCompPtr->AcceptInvitation(invitationId, ContextUserId(gqlRequest));
	if (membershipId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to accept tenant invitation");
		return response;
	}

	response.Version_1_0->membershipId = membershipId;
	response.Version_1_0->success = true;
	return response;
}


sdl::imtauth::TenantMemberships::CRejectTenantInvitationPayload CTenantMembershipManagerControllerComp::OnRejectTenantInvitation(
	const sdl::imtauth::TenantMemberships::CRejectTenantInvitationGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CRejectTenantInvitationPayload response;
	response.Version_1_0.emplace();

	QByteArray invitationId;
	sdl::imtauth::TenantMemberships::RejectTenantInvitationRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->invitationId){
		invitationId = *arguments.input.Version_1_0->invitationId;
	}

	bool success = m_invitationManagerCompPtr.IsValid() && m_invitationManagerCompPtr->RejectInvitation(invitationId, ContextUserId(gqlRequest));
	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to reject tenant invitation");
	}
	return response;
}


sdl::imtauth::TenantMemberships::CRevokeTenantInvitationPayload CTenantMembershipManagerControllerComp::OnRevokeTenantInvitation(
	const sdl::imtauth::TenantMemberships::CRevokeTenantInvitationGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CRevokeTenantInvitationPayload response;
	response.Version_1_0.emplace();

	if (!m_invitationManagerCompPtr.IsValid() || !m_membershipManagerCompPtr.IsValid()){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Invitation manager is not configured");
		return response;
	}

	QByteArray invitationId;
	sdl::imtauth::TenantMemberships::RevokeTenantInvitationRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->invitationId){
		invitationId = *arguments.input.Version_1_0->invitationId;
	}

	imtauth::ITenantInvitationUniquePtr invitationPtr = m_invitationManagerCompPtr->GetInvitation(invitationId);
	QByteArray contextUserId = ContextUserId(gqlRequest);
	if (!invitationPtr.IsValid() || !CanManageTenant(*m_membershipManagerCompPtr.GetPtr(), contextUserId, invitationPtr->GetTenantId())){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Access denied");
		return response;
	}

	bool success = m_invitationManagerCompPtr->RevokeInvitation(invitationId, contextUserId);
	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to revoke tenant invitation");
	}
	return response;
}


sdl::imtauth::TenantMemberships::CResendTenantInvitationPayload CTenantMembershipManagerControllerComp::OnResendTenantInvitation(
	const sdl::imtauth::TenantMemberships::CResendTenantInvitationGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CResendTenantInvitationPayload response;
	response.Version_1_0.emplace();

	if (!m_invitationManagerCompPtr.IsValid() || !m_membershipManagerCompPtr.IsValid()){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Invitation manager is not configured");
		return response;
	}

	QByteArray invitationId;
	sdl::imtauth::TenantMemberships::ResendTenantInvitationRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->invitationId){
		invitationId = *arguments.input.Version_1_0->invitationId;
	}

	imtauth::ITenantInvitationUniquePtr invitationPtr = m_invitationManagerCompPtr->GetInvitation(invitationId);
	if (!invitationPtr.IsValid() || !CanManageTenant(*m_membershipManagerCompPtr.GetPtr(), ContextUserId(gqlRequest), invitationPtr->GetTenantId())){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Access denied");
		return response;
	}

	bool success = m_invitationManagerCompPtr->ResendInvitation(invitationId);
	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to resend tenant invitation");
	}
	return response;
}


sdl::imtauth::TenantMemberships::CUpdateMembershipRolePayload CTenantMembershipManagerControllerComp::OnUpdateMembershipRole(
	const sdl::imtauth::TenantMemberships::CUpdateMembershipRoleGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CUpdateMembershipRolePayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray membershipId;
	imtauth::ITenantMembership::TenantMemberRole role = imtauth::ITenantMembership::TMR_MEMBER;
	sdl::imtauth::TenantMemberships::UpdateMembershipRoleRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->membershipId){
		membershipId = *arguments.input.Version_1_0->membershipId;
	}
	if (arguments.input.Version_1_0->role){
		role = static_cast<imtauth::ITenantMembership::TenantMemberRole>(*arguments.input.Version_1_0->role);
	}

	bool success = m_membershipManagerCompPtr->UpdateMembershipRole(membershipId, role);

	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to update membership role");
	}

	return response;
}


sdl::imtauth::TenantMemberships::CAcceptTenantMembershipInvitationPayload CTenantMembershipManagerControllerComp::OnAcceptTenantMembershipInvitation(
	const sdl::imtauth::TenantMemberships::CAcceptTenantMembershipInvitationGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CAcceptTenantMembershipInvitationPayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray membershipId;
	sdl::imtauth::TenantMemberships::AcceptTenantMembershipInvitationRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->membershipId){
		membershipId = *arguments.input.Version_1_0->membershipId;
	}

	bool success = m_membershipManagerCompPtr->AcceptMembershipInvitation(membershipId);
	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to accept membership invitation");
	}

	return response;
}


sdl::imtauth::TenantMemberships::CRejectTenantMembershipInvitationPayload CTenantMembershipManagerControllerComp::OnRejectTenantMembershipInvitation(
	const sdl::imtauth::TenantMemberships::CRejectTenantMembershipInvitationGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::TenantMemberships::CRejectTenantMembershipInvitationPayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray membershipId;
	sdl::imtauth::TenantMemberships::RejectTenantMembershipInvitationRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->membershipId){
		membershipId = *arguments.input.Version_1_0->membershipId;
	}

	bool success = m_membershipManagerCompPtr->RejectMembershipInvitation(membershipId);
	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to reject membership invitation");
	}

	return response;
}


} // namespace imtauthgql
