// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantMembershipManagerControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantMemberships.h>

// ImtCore includes
#include <imtauth/ITenantInfo.h>
#include <imtgql/IGqlContext.h>


namespace
{


QByteArray ContextUserId(const imtgql::CGqlRequest& gqlRequest)
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	return contextPtr != nullptr ? contextPtr->GetUserId() : QByteArray();
}


sdl::V1_0::imtauth::CTenantInvitationData ToTenantInvitationData(
		const imtauth::ITenantInvitation& invitation,
		imtauth::ITenantInvitation::TenantInvitationStatus status)
{
	sdl::V1_0::imtauth::CTenantInvitationData data;
	data.id = invitation.GetInvitationId();
	data.userId = invitation.GetUserId();
	data.tenantId = invitation.GetTenantId();
	data.role = invitation.GetRoleId();
	data.status = static_cast<sdl::V1_0::imtauth::TenantInvitationStatus>(status);
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


/**
	Check if the user is the owner of the tenant.
	Owner bypasses all tenant permission checks.
*/
bool IsOwner(const imtauth::ITenantManager& tenantManager, const QByteArray& userId, const QByteArray& tenantId)
{
	imtauth::ITenantInfoUniquePtr tenantPtr = tenantManager.GetTenant(tenantId);
	return tenantPtr.IsValid() && tenantPtr->GetOwnerId() == userId;
}


/**
	Check if the user has access to perform tenant operations.
	Owner always has access.
	Non-owners must be active members of the tenant.
*/
bool HasTenantAccess(
		const imtauth::ITenantManager& tenantManager,
		const imtauth::ITenantMembershipManager& membershipManager,
		const QByteArray& userId,
		const QByteArray& tenantId)
{
	// Owner bypasses all permission checks
	if (IsOwner(tenantManager, userId, tenantId)){
		return true;
	}

	// Non-owners must be active members of the tenant.
	return membershipManager.HasMinimumRole(userId, tenantId, QByteArray());
}


/**
	Check if the user can manage members (create invitations, remove members, change roles).
	Only Creator, Owner, or Admin can manage members.
*/
bool CanManageMembers(
		const imtauth::ITenantManager& tenantManager,
		const imtauth::ITenantMembershipManager& membershipManager,
		const QByteArray& userId,
		const QByteArray& tenantId)
{
	// Owner and Creator always can manage
	imtauth::ITenantInfoUniquePtr tenantPtr = tenantManager.GetTenant(tenantId);
	if (tenantPtr.IsValid()){
		if (tenantPtr->GetOwnerId() == userId || tenantPtr->GetCreatorId() == userId){
			return true;
		}
	}

	// Check Admin role
	QByteArray adminRoleId = imtauth::TenantEnvironmentRoleToString(imtauth::TER_ADMIN).toUtf8();
	return membershipManager.HasMinimumRole(userId, tenantId, adminRoleId);
}


} // anonymous namespace


namespace imtauthgql
{


// protected methods

// reimplemented (sdl::V1_0::imtauth::CTenantMembershipsGqlHandlerCompBase)

sdl::V1_0::imtauth::CGetMembershipsByTenantPayload CTenantMembershipManagerControllerComp::OnGetMembershipsByTenant(
	const sdl::V1_0::imtauth::CGetMembershipsByTenantGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetMembershipsByTenantPayload response;

	if (!m_invitationManagerCompPtr.IsValid() || !m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'InvitationManager' or 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray tenantId;
	sdl::V1_0::imtauth::GetMembershipsByTenantRequestArguments arguments = request.GetRequestedArguments();
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


sdl::V1_0::imtauth::CGetMembershipsByUserPayload CTenantMembershipManagerControllerComp::OnGetMembershipsByUser(
	const sdl::V1_0::imtauth::CGetMembershipsByUserGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetMembershipsByUserPayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray userId;
	sdl::V1_0::imtauth::GetMembershipsByUserRequestArguments arguments = request.GetRequestedArguments();
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


sdl::V1_0::imtauth::CGetMembershipPayload CTenantMembershipManagerControllerComp::OnGetMembership(
	const sdl::V1_0::imtauth::CGetMembershipGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetMembershipPayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray membershipId;
	sdl::V1_0::imtauth::GetMembershipRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->membershipId){
		membershipId = *arguments.input.Version_1_0->membershipId;
	}

	imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);

	if (!membershipPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Membership not found");
		return response;
	}

	sdl::V1_0::imtauth::CTenantMembershipData membershipData;
	membershipData.id = membershipPtr->GetMembershipId();
	membershipData.userId = membershipPtr->GetUserId();
	membershipData.tenantId = membershipPtr->GetTenantId();
	membershipData.role = membershipPtr->GetRoleId();
	membershipData.isActive = membershipPtr->IsActive();
	membershipData.joinedAt = membershipPtr->GetJoinedAt();
	membershipData.updatedAt = membershipPtr->GetUpdatedAt();

	response.Version_1_0->membership = membershipData;

	return response;
}


sdl::V1_0::imtauth::CFindMembershipPayload CTenantMembershipManagerControllerComp::OnFindMembership(
	const sdl::V1_0::imtauth::CFindMembershipGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CFindMembershipPayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray userId;
	QByteArray tenantId;
	sdl::V1_0::imtauth::FindMembershipRequestArguments arguments = request.GetRequestedArguments();
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

	sdl::V1_0::imtauth::CTenantMembershipData membershipData;
	membershipData.id = membershipPtr->GetMembershipId();
	membershipData.userId = membershipPtr->GetUserId();
	membershipData.tenantId = membershipPtr->GetTenantId();
	membershipData.role = membershipPtr->GetRoleId();
	membershipData.isActive = membershipPtr->IsActive();
	membershipData.joinedAt = membershipPtr->GetJoinedAt();
	membershipData.updatedAt = membershipPtr->GetUpdatedAt();

	response.Version_1_0->membership = membershipData;

	return response;
}


sdl::V1_0::imtauth::CIsMemberPayload CTenantMembershipManagerControllerComp::OnIsMember(
	const sdl::V1_0::imtauth::CIsMemberGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CIsMemberPayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray userId;
	QByteArray tenantId;
	sdl::V1_0::imtauth::IsMemberRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->userId){
		userId = *arguments.input.Version_1_0->userId;
	}
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}

	response.Version_1_0->isMember = m_membershipManagerCompPtr->IsMember(userId, tenantId);

	return response;
}


sdl::V1_0::imtauth::CHasMinimumRolePayload CTenantMembershipManagerControllerComp::OnHasMinimumRole(
	const sdl::V1_0::imtauth::CHasMinimumRoleGqlRequest& request,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CHasMinimumRolePayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray userId;
	QByteArray tenantId;
	sdl::V1_0::imtauth::HasMinimumRoleRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->userId){
		userId = *arguments.input.Version_1_0->userId;
	}
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}

	QByteArray minimumRoleId;
	if (arguments.input.Version_1_0->minimumRole){
		minimumRoleId = (*arguments.input.Version_1_0->minimumRole);
	}

	response.Version_1_0->hasRole = m_membershipManagerCompPtr->HasMinimumRole(userId, tenantId, minimumRoleId);

	return response;
}


sdl::V1_0::imtauth::CGetTenantInvitationsPayload CTenantMembershipManagerControllerComp::OnGetTenantInvitations(
	const sdl::V1_0::imtauth::CGetTenantInvitationsGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetTenantInvitationsPayload response;
	response.Version_1_0.emplace();

	if (!m_invitationManagerCompPtr.IsValid() || !m_membershipManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Invitation manager is not configured");
		return response;
	}

	QByteArray tenantId;
	imtauth::ITenantInvitationManager::Statuses statuses;
	sdl::V1_0::imtauth::GetTenantInvitationsRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}
	if (arguments.input.Version_1_0->statuses){
		for (const auto& status : *arguments.input.Version_1_0->statuses){
			statuses.append(static_cast<imtauth::ITenantInvitation::TenantInvitationStatus>(*status));
		}
	}

	if (!m_tenantManagerCompPtr.IsValid() || !HasTenantAccess(*m_tenantManagerCompPtr.GetPtr(), *m_membershipManagerCompPtr.GetPtr(), ContextUserId(gqlRequest), tenantId)){
		response.Version_1_0->errorMessage = QStringLiteral("Access denied");
		return response;
	}

	response.Version_1_0->invitations.Emplace();
	for (const QByteArray& invitationId : m_invitationManagerCompPtr->GetInvitationsByTenant(tenantId, statuses)){
		imtauth::ITenantInvitationUniquePtr invitationPtr = m_invitationManagerCompPtr->GetInvitation(invitationId);
		if (invitationPtr.IsValid()){
			sdl::V1_0::imtauth::CTenantInvitationData data = ToTenantInvitationData(*invitationPtr, m_invitationManagerCompPtr->GetEffectiveStatus(*invitationPtr));
			if (m_tenantManagerCompPtr.IsValid()){
				imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(invitationPtr->GetTenantId());
				if (tenantPtr.IsValid()){
					data.tenantName = tenantPtr->GetTenantName();
				}
			}
			response.Version_1_0->invitations->push_back(data);
		}
	}

	return response;
}


sdl::V1_0::imtauth::CGetMyTenantInvitationsPayload CTenantMembershipManagerControllerComp::OnGetMyTenantInvitations(
	const sdl::V1_0::imtauth::CGetMyTenantInvitationsGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetMyTenantInvitationsPayload response;
	response.Version_1_0.emplace();

	if (!m_invitationManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Invitation manager is not configured");
		return response;
	}

	imtauth::ITenantInvitationManager::Statuses statuses;
	sdl::V1_0::imtauth::GetMyTenantInvitationsRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->statuses){
		for (const auto& status : *arguments.input.Version_1_0->statuses){
			statuses.append(static_cast<imtauth::ITenantInvitation::TenantInvitationStatus>(*status));
		}
	}

	response.Version_1_0->invitations.Emplace();
	for (const QByteArray& invitationId : m_invitationManagerCompPtr->GetInvitationsByUser(ContextUserId(gqlRequest), statuses)){
		imtauth::ITenantInvitationUniquePtr invitationPtr = m_invitationManagerCompPtr->GetInvitation(invitationId);
		if (invitationPtr.IsValid()){
			sdl::V1_0::imtauth::CTenantInvitationData data = ToTenantInvitationData(*invitationPtr, m_invitationManagerCompPtr->GetEffectiveStatus(*invitationPtr));
			if (m_tenantManagerCompPtr.IsValid()){
				imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(invitationPtr->GetTenantId());
				if (tenantPtr.IsValid()){
					data.tenantName = tenantPtr->GetTenantName();
				}
			}
			response.Version_1_0->invitations->push_back(data);
		}
	}

	return response;
}


sdl::V1_0::imtauth::CAddMembershipPayload CTenantMembershipManagerControllerComp::OnAddMembership(
	const sdl::V1_0::imtauth::CAddMembershipGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CAddMembershipPayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray userId;
	QByteArray tenantId;
	QByteArray roleId;
	sdl::V1_0::imtauth::AddMembershipRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->userId){
		userId = *arguments.input.Version_1_0->userId;
	}
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}
	if (arguments.input.Version_1_0->role){
		roleId = (*arguments.input.Version_1_0->role);
	}

	QByteArray contextUserId = ContextUserId(gqlRequest);
	if (!m_tenantManagerCompPtr.IsValid() || !HasTenantAccess(*m_tenantManagerCompPtr.GetPtr(), *m_membershipManagerCompPtr.GetPtr(), contextUserId, tenantId)){
		response.Version_1_0->errorMessage = QStringLiteral("Access denied");
		return response;
	}

	// Only Creator/Owner/Admin can add members
	if (!CanManageMembers(*m_tenantManagerCompPtr.GetPtr(), *m_membershipManagerCompPtr.GetPtr(), contextUserId, tenantId)){
		response.Version_1_0->errorMessage = QStringLiteral("Only Admin, Owner, or Creator can add members");
		return response;
	}

	QByteArray membershipId = m_membershipManagerCompPtr->AddMembership(userId, tenantId, roleId);

	if (membershipId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to add membership");
		return response;
	}

	response.Version_1_0->membershipId = membershipId;

	return response;
}


sdl::V1_0::imtauth::CRemoveMembershipPayload CTenantMembershipManagerControllerComp::OnRemoveMembership(
	const sdl::V1_0::imtauth::CRemoveMembershipGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRemoveMembershipPayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray membershipId;
	sdl::V1_0::imtauth::RemoveMembershipRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->membershipId){
		membershipId = *arguments.input.Version_1_0->membershipId;
	}

	// Resolve tenant from membership for permission check
	imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
	if (membershipPtr.IsValid() && m_tenantManagerCompPtr.IsValid()){
		QByteArray tenantId = membershipPtr->GetTenantId();
		QByteArray contextUserId = ContextUserId(gqlRequest);
		if (!HasTenantAccess(*m_tenantManagerCompPtr.GetPtr(), *m_membershipManagerCompPtr.GetPtr(), contextUserId, tenantId)){
			response.Version_1_0->success = false;
			response.Version_1_0->errorMessage = QStringLiteral("Access denied");
			return response;
		}
		// Only Creator/Owner/Admin can remove other members; regular members can only remove themselves
		bool isSelfRemoval = (membershipPtr->GetUserId() == contextUserId);
		if (!isSelfRemoval && !CanManageMembers(*m_tenantManagerCompPtr.GetPtr(), *m_membershipManagerCompPtr.GetPtr(), contextUserId, tenantId)){
			response.Version_1_0->success = false;
			response.Version_1_0->errorMessage = QStringLiteral("Only Admin, Owner, or Creator can remove other members");
			return response;
		}
	}

	bool success = m_membershipManagerCompPtr->RemoveMembership(membershipId);

	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to remove membership");
	}

	return response;
}


sdl::V1_0::imtauth::CCreateTenantInvitationPayload CTenantMembershipManagerControllerComp::OnCreateTenantInvitation(
	const sdl::V1_0::imtauth::CCreateTenantInvitationGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CCreateTenantInvitationPayload response;
	response.Version_1_0.emplace();

	if (!m_invitationManagerCompPtr.IsValid() || !m_membershipManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Invitation manager is not configured");
		return response;
	}

	QByteArray userId;
	QByteArray tenantId;
	QByteArray roleId;
	sdl::V1_0::imtauth::CreateTenantInvitationRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->userId){
		userId = *arguments.input.Version_1_0->userId;
	}
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}
	if (arguments.input.Version_1_0->role){
		roleId = (*arguments.input.Version_1_0->role);
	}

	QByteArray contextUserId = ContextUserId(gqlRequest);
	if (!m_tenantManagerCompPtr.IsValid() || !HasTenantAccess(*m_tenantManagerCompPtr.GetPtr(), *m_membershipManagerCompPtr.GetPtr(), contextUserId, tenantId)){
		response.Version_1_0->errorMessage = QStringLiteral("Access denied");
		return response;
	}

	// Only Creator/Owner/Admin can create invitations
	if (!CanManageMembers(*m_tenantManagerCompPtr.GetPtr(), *m_membershipManagerCompPtr.GetPtr(), contextUserId, tenantId)){
		response.Version_1_0->errorMessage = QStringLiteral("Only Admin, Owner, or Creator can invite members");
		return response;
	}

	QByteArray invitationId = m_invitationManagerCompPtr->CreateInvitation(contextUserId, userId, tenantId, roleId);
	if (invitationId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to create tenant invitation");
		return response;
	}

	response.Version_1_0->invitationId = invitationId;
	return response;
}


sdl::V1_0::imtauth::CAcceptTenantInvitationPayload CTenantMembershipManagerControllerComp::OnAcceptTenantInvitation(
	const sdl::V1_0::imtauth::CAcceptTenantInvitationGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CAcceptTenantInvitationPayload response;
	response.Version_1_0.emplace();
	response.Version_1_0->success = false;

	if (!m_invitationManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Invitation manager is not configured");
		return response;
	}

	QByteArray invitationId;
	sdl::V1_0::imtauth::AcceptTenantInvitationRequestArguments arguments = request.GetRequestedArguments();
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


sdl::V1_0::imtauth::CRejectTenantInvitationPayload CTenantMembershipManagerControllerComp::OnRejectTenantInvitation(
	const sdl::V1_0::imtauth::CRejectTenantInvitationGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRejectTenantInvitationPayload response;
	response.Version_1_0.emplace();

	QByteArray invitationId;
	sdl::V1_0::imtauth::RejectTenantInvitationRequestArguments arguments = request.GetRequestedArguments();
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


sdl::V1_0::imtauth::CRevokeTenantInvitationPayload CTenantMembershipManagerControllerComp::OnRevokeTenantInvitation(
	const sdl::V1_0::imtauth::CRevokeTenantInvitationGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRevokeTenantInvitationPayload response;
	response.Version_1_0.emplace();

	if (!m_invitationManagerCompPtr.IsValid() || !m_membershipManagerCompPtr.IsValid()){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Invitation manager is not configured");
		return response;
	}

	QByteArray invitationId;
	sdl::V1_0::imtauth::RevokeTenantInvitationRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->invitationId){
		invitationId = *arguments.input.Version_1_0->invitationId;
	}

	imtauth::ITenantInvitationUniquePtr invitationPtr = m_invitationManagerCompPtr->GetInvitation(invitationId);
	QByteArray contextUserId = ContextUserId(gqlRequest);
	if (!invitationPtr.IsValid() || !m_tenantManagerCompPtr.IsValid() || !HasTenantAccess(*m_tenantManagerCompPtr.GetPtr(), *m_membershipManagerCompPtr.GetPtr(), contextUserId, invitationPtr->GetTenantId())){
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


sdl::V1_0::imtauth::CResendTenantInvitationPayload CTenantMembershipManagerControllerComp::OnResendTenantInvitation(
	const sdl::V1_0::imtauth::CResendTenantInvitationGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CResendTenantInvitationPayload response;
	response.Version_1_0.emplace();

	if (!m_invitationManagerCompPtr.IsValid() || !m_membershipManagerCompPtr.IsValid()){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Invitation manager is not configured");
		return response;
	}

	QByteArray invitationId;
	sdl::V1_0::imtauth::ResendTenantInvitationRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->invitationId){
		invitationId = *arguments.input.Version_1_0->invitationId;
	}

	imtauth::ITenantInvitationUniquePtr invitationPtr = m_invitationManagerCompPtr->GetInvitation(invitationId);
	if (!invitationPtr.IsValid() || !m_tenantManagerCompPtr.IsValid() || !HasTenantAccess(*m_tenantManagerCompPtr.GetPtr(), *m_membershipManagerCompPtr.GetPtr(), ContextUserId(gqlRequest), invitationPtr->GetTenantId())){
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


sdl::V1_0::imtauth::CUpdateMembershipRolePayload CTenantMembershipManagerControllerComp::OnUpdateMembershipRole(
	const sdl::V1_0::imtauth::CUpdateMembershipRoleGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CUpdateMembershipRolePayload response;

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray membershipId;
	QByteArray roleId;
	sdl::V1_0::imtauth::UpdateMembershipRoleRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->membershipId){
		membershipId = *arguments.input.Version_1_0->membershipId;
	}
	if (arguments.input.Version_1_0->role){
		roleId = (*arguments.input.Version_1_0->role);
	}

	// Resolve tenant from membership for permission check and owner guard
	imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
	if (membershipPtr.IsValid()){
		QByteArray tenantId = membershipPtr->GetTenantId();

		// Permission check: ManageMembers
		QByteArray contextUserId = ContextUserId(gqlRequest);
		if (m_tenantManagerCompPtr.IsValid() && !HasTenantAccess(*m_tenantManagerCompPtr.GetPtr(), *m_membershipManagerCompPtr.GetPtr(), contextUserId, tenantId)){
			response.Version_1_0->success = false;
			response.Version_1_0->errorMessage = QStringLiteral("Access denied");
			return response;
		}

		// Only Creator/Owner/Admin can change member roles
		if (m_tenantManagerCompPtr.IsValid() && !CanManageMembers(*m_tenantManagerCompPtr.GetPtr(), *m_membershipManagerCompPtr.GetPtr(), contextUserId, tenantId)){
			response.Version_1_0->success = false;
			response.Version_1_0->errorMessage = QStringLiteral("Only Admin, Owner, or Creator can change member roles");
			return response;
		}

		// Check if membership is the owner — owner's role is managed via TransferOwnership, not UpdateMembershipRole
		if (m_tenantManagerCompPtr.IsValid()){
			imtauth::ITenantInfoUniquePtr tenantInfoPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
			if (tenantInfoPtr.IsValid() && tenantInfoPtr->GetOwnerId() == membershipPtr->GetUserId()){
				response.Version_1_0->success = false;
				response.Version_1_0->errorMessage = QStringLiteral("Cannot change the Owner role");
				return response;
			}
		}
	}

	bool success = m_membershipManagerCompPtr->UpdateMembershipRole(membershipId, roleId);

	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to update membership role");
	}

	return response;
}


sdl::V1_0::imtauth::CTransferTenantOwnershipPayload CTenantMembershipManagerControllerComp::OnTransferTenantOwnership(
	const sdl::V1_0::imtauth::CTransferTenantOwnershipGqlRequest& request,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CTransferTenantOwnershipPayload response;

	if (!m_membershipManagerCompPtr.IsValid() || !m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' or 'TenantManager' was not set", "CTenantMembershipManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray contextUserId = ContextUserId(gqlRequest);

	QByteArray tenantId;
	QByteArray newOwnerId;
	sdl::V1_0::imtauth::TransferTenantOwnershipRequestArguments arguments = request.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}
	if (arguments.input.Version_1_0->newOwnerId){
		newOwnerId = *arguments.input.Version_1_0->newOwnerId;
	}

	if (tenantId.isEmpty() || newOwnerId.isEmpty()){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Missing tenantId or newOwnerId");
		return response;
	}

	// Only the current owner can transfer ownership
	imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
	if (!tenantPtr.IsValid()){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Tenant not found");
		return response;
	}

	QByteArray currentOwnerId = tenantPtr->GetOwnerId();
	if (currentOwnerId != contextUserId){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Only the current owner can transfer ownership");
		return response;
	}

	// Prevent transfer to self
	if (newOwnerId == contextUserId){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Cannot transfer ownership to yourself");
		return response;
	}

	// Verify the new owner is a member of the tenant
	if (!m_membershipManagerCompPtr->IsMember(newOwnerId, tenantId)){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("New owner must be a member of the tenant");
		return response;
	}

	// Update tenant's ownerId — ownership is tracked via ITenantInfo::GetOwnerId(), not via membership role
	bool tenantUpdated = m_tenantManagerCompPtr->UpdateTenant(tenantId, tenantPtr->GetTenantName(), tenantPtr->GetTenantDescription(), newOwnerId, true);
	if (!tenantUpdated){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Failed to update tenant owner");
		return response;
	}

	response.Version_1_0->success = true;
	return response;
}


} // namespace imtauthgql
