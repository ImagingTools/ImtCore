// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantMembershipManagerControllerComp.h>


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

	if (!m_membershipManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'MembershipManager' was not set", "CTenantMembershipManagerControllerComp");
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

	const imtauth::ITenantMembership* membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);

	if (membershipPtr == nullptr){
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

	const imtauth::ITenantMembership* membershipPtr = m_membershipManagerCompPtr->FindMembership(userId, tenantId);

	if (membershipPtr == nullptr){
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


} // namespace imtauthgql
