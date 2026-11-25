// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantManagerControllerComp.h>
#include <imtauthgql/imtauthgql.h>
#include <imtauthgql/TenantAccess.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


// Qt includes
#include <QtCore/QSet>
#include <QtCore/QMap>

// ImtCore includes
#include <imtauth/imtauth.h>
#include <imtauth/IUserInfo.h>
#include <imtauth/ITenantMembership.h>
#include <iqt/iqt.h>
#include <imtgql/IGqlContext.h>
#include <imtbase/imtbase.h>


namespace
{


QString NormalizeTenantName(const QString& tenantName)
{
	return tenantName.trimmed();
}


bool HasTenantNameConflict(
			const imtauth::ITenantManager& tenantManager,
			const QString& tenantName,
			const QByteArray& excludeTenantId)
{
	const QString normalizedName = NormalizeTenantName(tenantName);
	if (normalizedName.isEmpty()){
		return false;
	}

	for (const QByteArray& id : tenantManager.FindTenantIdsByName(normalizedName)){
		if (excludeTenantId.isEmpty() || id != excludeTenantId){
			return true;
		}
	}

	return false;
}


bool UserExists(const imtbase::IObjectCollection* userCollectionPtr, const QByteArray& userId)
{
	if (userCollectionPtr == nullptr || userId.isEmpty() || !imtbase::isValidUuid(userId)){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	return userCollectionPtr->GetObjectData(userId, dataPtr) && dataPtr.GetPtr() != nullptr;
}


} // anonymous namespace


namespace imtauthgql
{

// protected methods

// reimplemented (sdl::V1_0::imtauth::CTenantsGqlHandlerCompBase)

sdl::V1_0::imtauth::CGetTenantIdsPayload CTenantManagerControllerComp::OnGetTenantIds(
			const sdl::V1_0::imtauth::CGetTenantIdsGqlRequest& /*getTenantIdsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetTenantIdsPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	const QByteArray userId = contextPtr != nullptr ? contextPtr->GetUserId() : QByteArray();
	if (userId.isEmpty()){
		response.errorMessage = QStringLiteral("Access denied: unauthenticated request context");
		return response;
	}

	const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
	const bool isAdmin = userInfoPtr != nullptr && userInfoPtr->IsAdmin();

	// Users other than administrators learn only the organizations they own, created, or belong to.
	QByteArrayList visibleTenantIds;
	if (!isAdmin){
		visibleTenantIds = m_tenantManagerCompPtr->GetOwnedOrCreatedTenantIds(userId);
		if (m_membershipManagerCompPtr.IsValid()){
			for (const QByteArray& membershipId : m_membershipManagerCompPtr->GetMembershipsByUser(userId)){
				imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
				if (membershipPtr.IsValid() && membershipPtr->IsActive() && !visibleTenantIds.contains(membershipPtr->GetTenantId())){
					visibleTenantIds.append(membershipPtr->GetTenantId());
				}
			}
		}
	}

	const QByteArrayList ids = isAdmin ? m_tenantManagerCompPtr->GetTenantIds() : visibleTenantIds;

	response.tenantIds.Emplace().FromList(ids);

	return response;
}


sdl::V1_0::imtauth::CGetTenantPayload CTenantManagerControllerComp::OnGetTenant(
			const sdl::V1_0::imtauth::CGetTenantGqlRequest& getTenantRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::CGetTenantPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	QByteArray tenantId;
	sdl::V1_0::imtauth::GetTenantRequestArguments arguments = getTenantRequest.GetRequestedArguments();
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}

	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}
	if (!imtbase::isValidUuid(tenantId)){
		response.errorMessage = QStringLiteral("Invalid tenant ID format");
		return response;
	}

	QString accessError;
	if (!HasTenantIsolationAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
				accessError)){
		response.errorMessage = accessError;
		return response;
	}

	bool isRead = false;
	imtauth::ITenantInfoUniquePtr tenantInfoPtr = m_tenantManagerCompPtr->GetTenant(tenantId, &isRead);

	if (!tenantInfoPtr.IsValid()){
		// A failed read is an error of the request, so a caller does not take it for an absent organization
		if (!isRead){
			errorMessage = QStringLiteral("The organization could not be read.");
			return response;
		}
		response.errorMessage = QStringLiteral("Organization not found");
		return response;
	}

	sdl::V1_0::imtauth::CTenantData tenantData;
	tenantData.id = tenantInfoPtr->GetTenantId();
	tenantData.name = tenantInfoPtr->GetTenantName();
	tenantData.description = tenantInfoPtr->GetTenantDescription();
	tenantData.ownerId = tenantInfoPtr->GetOwnerId();
	tenantData.creatorId = tenantInfoPtr->GetCreatorId();
	tenantData.isActive = tenantInfoPtr->IsActive();
	tenantData.createdAt = tenantInfoPtr->GetCreatedAt();
	tenantData.updatedAt = tenantInfoPtr->GetUpdatedAt();
	tenantData.tenantPermissions.Emplace().FromList(tenantInfoPtr->GetTenantPermissions());
	tenantData.parentTenantId = tenantInfoPtr->GetParentTenantId();
	tenantData.isSystemTenant = (tenantInfoPtr->GetTenantId() == imtauth::GetSystemTenantId());

	const imtgql::IGqlContext* ctx = gqlRequest.GetRequestContext();
	if (ctx != nullptr){
		tenantData.currentUserId = ctx->GetUserId();
	}

	// Populate the organization permissions available to the current user in this tenant
	if (m_membershipManagerCompPtr.IsValid() && tenantData.currentUserId.has_value()){
		QByteArray currentUid = *tenantData.currentUserId;
		imtauth::ITenantMembershipUniquePtr myMs = m_membershipManagerCompPtr->FindMembership(currentUid, tenantId);
		if (myMs.IsValid()){
			tenantData.currentUserOrganizationPermissions.Emplace().FromList(myMs->GetOrganizationPermissions());
		}
	}

	// Populate members (id + name) from TenantMemberships
	if (m_membershipManagerCompPtr.IsValid()){
		QByteArrayList membershipIds = m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId);
		tenantData.members.Emplace();
		for (const QByteArray& msId : std::as_const(membershipIds)){
			imtauth::ITenantMembershipUniquePtr msPtr = m_membershipManagerCompPtr->GetMembership(msId);
			if (msPtr.IsValid() && msPtr->IsActive()){
				QByteArray userId = msPtr->GetUserId();
				sdl::V1_0::imtauth::CTenantMemberEntry memberEntry;
				memberEntry.id = userId;

				if (m_userCollectionCompPtr.IsValid()){
					memberEntry.name = imtauth::GetUserName(*m_userCollectionCompPtr, userId);
				}

				tenantData.members->push_back(memberEntry);
			}
		}
	}

	response.tenant = tenantData;

	return response;
}


sdl::V1_0::imtauth::CGetTenantPermissionsPayload CTenantManagerControllerComp::OnGetTenantPermissions(
			const sdl::V1_0::imtauth::CGetTenantPermissionsGqlRequest& getTenantPermissionsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetTenantPermissionsPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant manager is not configured");
		return response;
	}

	QByteArray tenantId;
	sdl::V1_0::imtauth::GetTenantPermissionsRequestArguments arguments = getTenantPermissionsRequest.GetRequestedArguments();
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}

	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}
	if (!imtbase::isValidUuid(tenantId)){
		response.errorMessage = QStringLiteral("Invalid tenant ID format");
		return response;
	}

	QString accessError;
	if (!HasTenantManageAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
				{s_viewOrganizationPermissionsPermission, s_viewOrganizationRolesPermission},
				accessError)){
		response.errorMessage = accessError;
		return response;
	}

	QByteArrayList tenantPermissions = m_tenantManagerCompPtr->GetTenantPermissions(tenantId);
	QSet<QByteArray> seenPermissionIds;
	QByteArrayList normalizedPermissionIds;
	for (const QByteArray& permId : tenantPermissions){
		QByteArray normalizedPermId = permId.trimmed();
		if (!normalizedPermId.isEmpty() && !seenPermissionIds.contains(normalizedPermId)){
			seenPermissionIds.insert(normalizedPermId);
			normalizedPermissionIds.append(normalizedPermId);
		}
	}

	response.permissionIds.Emplace().FromList(normalizedPermissionIds);

	return response;
}


sdl::V1_0::imtauth::CCreateTenantPayload CTenantManagerControllerComp::OnCreateTenant(
			const sdl::V1_0::imtauth::CCreateTenantGqlRequest& createTenantRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CCreateTenantPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	QString name;
	QString description;
	// An organization created from inside another one is its sub-organization; the parent is never chosen
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	const QByteArray parentTenantId = gqlContextPtr != nullptr ? gqlContextPtr->GetTenantId() : QByteArray();
	const QByteArray creatorId = gqlContextPtr != nullptr ? gqlContextPtr->GetUserId() : QByteArray();
	QByteArray ownerId = creatorId;

	if (gqlContextPtr == nullptr || !CanCreateOrganization(*gqlContextPtr)){
		response.errorMessage = QStringLiteral("Permission denied: /OrganizationManagement/Add");
		return response;
	}

	sdl::V1_0::imtauth::CreateTenantRequestArguments arguments = createTenantRequest.GetRequestedArguments();
	if (arguments.input->name){
		name = *arguments.input->name;
	}
	if (arguments.input->description){
		description = *arguments.input->description;
	}
	if (arguments.input->ownerId){
		ownerId = *arguments.input->ownerId;
	}
	if (arguments.input->parentTenantId && !arguments.input->parentTenantId->isEmpty() && *arguments.input->parentTenantId != parentTenantId){
		response.errorMessage = QStringLiteral("The parent of a new organization is the organization you work in");
		return response;
	}

	name = NormalizeTenantName(name);
	if (name.isEmpty()){
		response.errorMessage = QStringLiteral("Organization name is required");
		return response;
	}

	if (!m_userCollectionCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("User collection is not configured");
		return response;
	}

	if (!UserExists(m_userCollectionCompPtr.GetPtr(), ownerId)){
		response.errorMessage = QStringLiteral("Owner user not found");
		return response;
	}

	if (HasTenantNameConflict(*m_tenantManagerCompPtr.GetPtr(), name, QByteArray())){
		response.errorMessage = QStringLiteral("An organization with this name already exists");
		return response;
	}

	QByteArray tenantId = m_tenantManagerCompPtr->CreateTenant(name, description, ownerId, creatorId, parentTenantId);

	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Failed to create tenant");
		return response;
	}

	if (m_membershipManagerCompPtr.IsValid() && !ownerId.isEmpty()){
		m_membershipManagerCompPtr->AddMembership(ownerId, tenantId);
	}

	response.tenantId = tenantId;

	return response;
}


sdl::V1_0::imtauth::CRemoveTenantPayload CTenantManagerControllerComp::OnRemoveTenant(
			const sdl::V1_0::imtauth::CRemoveTenantGqlRequest& removeTenantRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRemoveTenantPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	QByteArray tenantId;
	sdl::V1_0::imtauth::RemoveTenantRequestArguments arguments = removeTenantRequest.GetRequestedArguments();
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}

	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		response.success = false;
		return response;
	}
	if (!imtbase::isValidUuid(tenantId)){
		response.errorMessage = QStringLiteral("Invalid tenant ID format");
		response.success = false;
		return response;
	}

	QString accessError;
	const imtauth::ITenantInfoUniquePtr removedTenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
	if (!CheckTenantRemovalAccess(gqlRequest.GetRequestContext(), removedTenantPtr.GetPtr(), accessError)){
		response.errorMessage = accessError;
		response.success = false;
		return response;
	}

	if (!CheckTenantRemoval(*m_tenantManagerCompPtr, {tenantId}, accessError)){
		response.errorMessage = accessError;
		response.success = false;
		return response;
	}

	bool success = m_tenantManagerCompPtr->RemoveTenant(tenantId);

	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to remove tenant");
	}

	return response;
}


sdl::V1_0::imtauth::CUpdateTenantPayload CTenantManagerControllerComp::OnUpdateTenant(
			const sdl::V1_0::imtauth::CUpdateTenantGqlRequest& updateTenantRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CUpdateTenantPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	sdl::V1_0::imtauth::UpdateTenantRequestArguments arguments = updateTenantRequest.GetRequestedArguments();

	QByteArray tenantId;
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		response.success = false;
		return response;
	}
	if (!imtbase::isValidUuid(tenantId)){
		response.errorMessage = QStringLiteral("Invalid tenant ID format");
		response.success = false;
		return response;
	}

	imtauth::ITenantInfoUniquePtr currentTenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
	if (!currentTenantPtr.IsValid()){
		response.errorMessage = QStringLiteral("Organization not found");
		response.success = false;
		return response;
	}

	QString accessError;
	if (!HasTenantManageAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
				GetEditOrganizationPermissions(),
				accessError)){
		response.errorMessage = accessError;
		response.success = false;
		return response;
	}

	QString name = currentTenantPtr->GetTenantName();
	if (arguments.input->name){
		name = *arguments.input->name;
	}
	name = NormalizeTenantName(name);
	if (name.isEmpty()){
		response.errorMessage = QStringLiteral("Organization name is required");
		response.success = false;
		return response;
	}

	if (HasTenantNameConflict(*m_tenantManagerCompPtr.GetPtr(), name, tenantId)){
		response.errorMessage = QStringLiteral("An organization with this name already exists");
		response.success = false;
		return response;
	}


	QString description = currentTenantPtr->GetTenantDescription();
	if (arguments.input->description){
		description = *arguments.input->description;
	}

	QByteArray ownerId = currentTenantPtr->GetOwnerId();
	bool updateOwner = false;

	if (arguments.input->ownerId){
		ownerId = *arguments.input->ownerId;
		updateOwner = true;

		if (!m_userCollectionCompPtr.IsValid()){
			response.errorMessage = QStringLiteral("User collection is not configured");
			response.success = false;
			return response;
		}

		if (!UserExists(m_userCollectionCompPtr.GetPtr(), ownerId)){
			response.errorMessage = QStringLiteral("Owner user not found");
			response.success = false;
			return response;
		}
	}

	const bool hasPermissions = arguments.input->tenantPermissions.HasValue();
	const QByteArrayList newPermissions = hasPermissions ? arguments.input->tenantPermissions->ToList() : QByteArrayList();
	if (hasPermissions){
		const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
		QString permissionsError;
		if (!CheckTenantPermissionsChange(
					*m_tenantManagerCompPtr.GetPtr(),
					currentTenantPtr->GetCreatorId(),
					currentTenantPtr->GetParentTenantId(),
					currentTenantPtr->GetTenantPermissions(),
					newPermissions,
					gqlContextPtr != nullptr ? gqlContextPtr->GetUserId() : QByteArray(),
					permissionsError)){
			response.errorMessage = permissionsError;
			response.success = false;
			return response;
		}
	}

	// The members to sync are read before anything changes
	const bool syncsMembers = m_membershipManagerCompPtr.IsValid() && arguments.input->members;
	QSet<QByteArray> currentUserIds;
	QMap<QByteArray, QByteArray> userIdToMembershipId; // userId -> membershipId
	QSet<QByteArray> newUserIds;
	QSet<QByteArray> keptUserIds;
	if (syncsMembers){
		for (const QByteArray& msId : m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId)){
			imtauth::ITenantMembershipUniquePtr msPtr = m_membershipManagerCompPtr->GetMembership(msId);
			if (msPtr.IsValid()){
				currentUserIds.insert(msPtr->GetUserId());
				userIdToMembershipId[msPtr->GetUserId()] = msId;
			}
		}

		for (const auto& member : *arguments.input->members){
			if (member->id){
				newUserIds << *member->id;
			}
		}

		// The owner and the creator are never removed, as in the organization editor
		keptUserIds = newUserIds;
		keptUserIds << ownerId << currentTenantPtr->GetCreatorId();

		// A removed member's pending invitations go first, as on the other removal paths. A failure stops before the organization
		// and its members change; the invitations already revoked stay revoked, which takes away no member's access.
		const imtgql::IGqlContext* removalContextPtr = gqlRequest.GetRequestContext();
		const QByteArray revokedByUserId = removalContextPtr != nullptr ? removalContextPtr->GetUserId() : QByteArray();
		for (const QByteArray& uid : currentUserIds){
			if (!keptUserIds.contains(uid) && m_invitationManagerCompPtr.IsValid() && !m_invitationManagerCompPtr->RevokeInvitationsSentBy(tenantId, uid, revokedByUserId)){
				response.errorMessage = QStringLiteral("The invitations a removed member sent could not be revoked. Try again.");
				response.success = false;
				return response;
			}
		}
	}

	// The scope goes first: a refused scope leaves the organization untouched
	bool success = !hasPermissions || m_tenantManagerCompPtr->SetTenantPermissions(tenantId, newPermissions);
	if (success){
		success = m_tenantManagerCompPtr->UpdateTenant(tenantId, name, description, ownerId, updateOwner);
	}

	// Sync members with TenantMemberships
	if (success && syncsMembers){
		for (const QByteArray& uid : currentUserIds){
			if (!keptUserIds.contains(uid) && !m_membershipManagerCompPtr->RemoveMembership(userIdToMembershipId[uid])){
				response.errorMessage = QStringLiteral("A member could not be removed. Try again.");
				response.success = false;
				return response;
			}
		}

		// Add memberships for new users
		for (const QByteArray& uid : newUserIds){
			if (!currentUserIds.contains(uid)){
				m_membershipManagerCompPtr->AddMembership(uid, tenantId);
			}
		}
	}

	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to update tenant");
	}

	return response;
}


sdl::V1_0::imtauth::CSetTenantActivePayload CTenantManagerControllerComp::OnSetTenantActive(
			const sdl::V1_0::imtauth::CSetTenantActiveGqlRequest& setTenantActiveRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CSetTenantActivePayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	QByteArray tenantId;
	bool isActive = false;
	sdl::V1_0::imtauth::SetTenantActiveRequestArguments arguments = setTenantActiveRequest.GetRequestedArguments();
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		response.success = false;
		return response;
	}
	if (!imtbase::isValidUuid(tenantId)){
		response.errorMessage = QStringLiteral("Invalid tenant ID format");
		response.success = false;
		return response;
	}
	if (arguments.input->isActive){
		isActive = *arguments.input->isActive;
	}

	QString accessError;
	if (!HasTenantManageAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
				QByteArrayList(),
				accessError)){
		response.errorMessage = accessError;
		response.success = false;
		return response;
	}

	bool success = m_tenantManagerCompPtr->SetTenantActive(tenantId, isActive);

	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to set tenant active state");
	}

	return response;
}


sdl::V1_0::imtauth::CEnsureSystemTenantPayload CTenantManagerControllerComp::OnEnsureSystemTenant(
		const sdl::V1_0::imtauth::CEnsureSystemTenantGqlRequest& /*ensureSystemTenantRequest*/,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CEnsureSystemTenantPayload response;
	
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	const imtauth::IUserInfo* userInfoPtr = contextPtr != nullptr ? contextPtr->GetUserInfo() : nullptr;
	if (userInfoPtr == nullptr || !userInfoPtr->IsAdmin()){
		response.success = false;
		response.errorMessage = QStringLiteral("Access denied: only a system administrator can ensure the System-Tenant");
		return response;
	}
	
	if (!m_tenantManagerCompPtr.IsValid()){
		response.success = false;
		response.errorMessage = QStringLiteral("Tenant manager is not configured");
		return response;
	}
	
	bool success = m_tenantManagerCompPtr->EnsureSystemTenant();
	response.success = success;
	response.systemTenantId = m_tenantManagerCompPtr->GetSystemTenantId();
	if (!success){
		response.errorMessage = QStringLiteral("Failed to ensure System-Tenant");
	}
	
	return response;
}


sdl::V1_0::imtauth::CGetConnectionCodePayload CTenantManagerControllerComp::OnGetConnectionCode(
		const sdl::V1_0::imtauth::CGetConnectionCodeGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetConnectionCodePayload response;
	
	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}
	
	sdl::V1_0::imtauth::GetConnectionCodeRequestArguments arguments = request.GetRequestedArguments();
	QByteArray tenantId;
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}

	if (!m_tenantManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant manager is not configured");
		return response;
	}

	QString accessError;
	if (!HasTenantManageAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
				{s_viewOrganizationConnectionCodePermission},
				accessError)){
		response.errorMessage = accessError;
		return response;
	}
	
	QString connectionCode = m_connectionRequestManagerCompPtr->GetConnectionCode(tenantId);
	bool allowByCode = m_connectionRequestManagerCompPtr->GetAllowConnectionsByCode(tenantId);
	response.connectionCode.Emplace();
	response.connectionCode->connectionCode = connectionCode;
	response.connectionCode->allowConnectionsByCode = allowByCode;
	response.connectionCode->tenantId = tenantId;
	
	return response;
}


sdl::V1_0::imtauth::CRegenerateConnectionCodePayload CTenantManagerControllerComp::OnRegenerateConnectionCode(
		const sdl::V1_0::imtauth::CRegenerateConnectionCodeGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRegenerateConnectionCodePayload response;
	
	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}
	
	sdl::V1_0::imtauth::RegenerateConnectionCodeRequestArguments arguments = request.GetRequestedArguments();
	QByteArray tenantId;
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}

	if (!m_tenantManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant manager is not configured");
		return response;
	}

	QString accessError;
	if (!HasTenantManageAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
				{s_connectOrganizationPermission},
				accessError)){
		response.errorMessage = accessError;
		return response;
	}
	
	QString newCode = m_connectionRequestManagerCompPtr->RegenerateConnectionCode(tenantId);
	if (newCode.isEmpty()){
		response.errorMessage = QStringLiteral("Failed to regenerate connection code");
	} else {
		response.connectionCode = newCode;
	}
	
	return response;
}


sdl::V1_0::imtauth::CSetAllowConnectionsByCodePayload CTenantManagerControllerComp::OnSetAllowConnectionsByCode(
		const sdl::V1_0::imtauth::CSetAllowConnectionsByCodeGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CSetAllowConnectionsByCodePayload response;
	
	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}
	
	sdl::V1_0::imtauth::SetAllowConnectionsByCodeRequestArguments arguments = request.GetRequestedArguments();
	QByteArray tenantId;
	bool allow = true;
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	if (arguments.input->allow){
		allow = *arguments.input->allow;
	}
	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}

	if (!m_tenantManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant manager is not configured");
		return response;
	}

	QString accessError;
	if (!HasTenantManageAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
				{s_connectOrganizationPermission},
				accessError)){
		response.errorMessage = accessError;
		return response;
	}
	
	bool success = m_connectionRequestManagerCompPtr->SetAllowConnectionsByCode(tenantId, allow);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to update allow connections setting");
	}
	
	return response;
}


sdl::V1_0::imtauth::CGetConnectionRequestsPayload CTenantManagerControllerComp::OnGetConnectionRequests(
		const sdl::V1_0::imtauth::CGetConnectionRequestsGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetConnectionRequestsPayload response;
	
	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}
	
	sdl::V1_0::imtauth::GetConnectionRequestsRequestArguments arguments = request.GetRequestedArguments();
	QByteArray tenantId;
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}

	if (!m_tenantManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant manager is not configured");
		return response;
	}

	QString accessError;
	if (!HasTenantManageAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
				{s_viewOrganizationConnectionsPermission},
				accessError)){
		response.errorMessage = accessError;
		return response;
	}
	
	QByteArrayList requestIds = m_connectionRequestManagerCompPtr->GetConnectionRequestIds(tenantId);
	response.requests.Emplace();

	for (const QByteArray& requestId : requestIds){
		imtauth::ITenantConnectionRequestInfoUniquePtr reqPtr = m_connectionRequestManagerCompPtr->GetConnectionRequest(requestId);
		if (reqPtr.IsValid()){
			sdl::V1_0::imtauth::CConnectionRequest data;
			data.id = reqPtr->GetRequestId();
			data.sourceTenantId = reqPtr->GetSourceTenantId();
			data.targetTenantId = reqPtr->GetTargetTenantId();
			data.connectionCode = reqPtr->GetConnectionCode();
			data.message = reqPtr->GetMessage();
			data.status = ToSdlConnectionRequestStatus(reqPtr->GetStatus());
			data.createdAt = reqPtr->GetCreatedAt();
			data.respondedAt = reqPtr->GetRespondedAt();
			data.sourceTenantName = reqPtr->GetSourceTenantName();
			data.targetTenantName = reqPtr->GetTargetTenantName();

			// Resolve tenant names from tenant manager (handles legacy data where names were not stored)
			if (m_tenantManagerCompPtr.IsValid()){
				if (reqPtr->GetSourceTenantName().isEmpty()){
					imtauth::ITenantInfoUniquePtr srcTenantPtr = m_tenantManagerCompPtr->GetTenant(reqPtr->GetSourceTenantId());
					if (srcTenantPtr.IsValid()){
						data.sourceTenantName = srcTenantPtr->GetTenantName();
					}
				}
				if (reqPtr->GetTargetTenantName().isEmpty()){
					imtauth::ITenantInfoUniquePtr tgtTenantPtr = m_tenantManagerCompPtr->GetTenant(reqPtr->GetTargetTenantId());
					if (tgtTenantPtr.IsValid()){
						data.targetTenantName = tgtTenantPtr->GetTenantName();
					}
				}
			}

			response.requests->push_back(data);
		}
	}
	
	return response;
}


sdl::V1_0::imtauth::CCreateConnectionRequestPayload CTenantManagerControllerComp::OnCreateConnectionRequest(
		const sdl::V1_0::imtauth::CCreateConnectionRequestGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CCreateConnectionRequestPayload response;
	
	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}
	
	sdl::V1_0::imtauth::CreateConnectionRequestRequestArguments arguments = request.GetRequestedArguments();
	QByteArray sourceTenantId;
	QString connectionCode;
	QByteArray targetTenantId;
	QString message;
	if (arguments.input->sourceTenantId){
		sourceTenantId = *arguments.input->sourceTenantId;
	}
	if (arguments.input->connectionCode){
		connectionCode = *arguments.input->connectionCode;
	}
	if (arguments.input->targetTenantId){
		targetTenantId = *arguments.input->targetTenantId;
	}
	if (arguments.input->message){
		message = *arguments.input->message;
	}

	if (sourceTenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Source tenant ID is required");
		return response;
	}

	// By the code of the organization, or to an organization found in the directory
	if (connectionCode.isEmpty() == targetTenantId.isEmpty()){
		response.errorMessage = QStringLiteral("A connection is requested by a connection code or to an organization of the directory");
		return response;
	}
	if (!imtbase::isValidUuid(sourceTenantId)){
		response.errorMessage = QStringLiteral("Invalid source tenant ID format");
		return response;
	}
	if (m_tenantManagerCompPtr.IsValid()){
		imtauth::ITenantInfoUniquePtr sourceTenantPtr = m_tenantManagerCompPtr->GetTenant(sourceTenantId);
		if (!sourceTenantPtr.IsValid()){
			response.errorMessage = QStringLiteral("Source tenant not found");
			return response;
		}

		QString accessError;
		if (!HasTenantManageAccess(
					*m_tenantManagerCompPtr.GetPtr(),
					m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
					gqlRequest,
					sourceTenantId,
					{s_connectOrganizationPermission},
					accessError)){
			response.errorMessage = accessError;
			return response;
		}
	}
	
	QString refusalReason;
	const QByteArray requestId = targetTenantId.isEmpty()
				? m_connectionRequestManagerCompPtr->CreateConnectionRequest(sourceTenantId, connectionCode, message, &refusalReason)
				: m_connectionRequestManagerCompPtr->CreateConnectionRequestToTenant(sourceTenantId, targetTenantId, message, &refusalReason);
	if (requestId.isEmpty()){
		response.errorMessage = refusalReason.isEmpty() ? QStringLiteral("Failed to create connection request") : refusalReason;
	} else {
		response.requestId = requestId;
	}
	
	return response;
}


sdl::V1_0::imtauth::CApproveConnectionRequestPayload CTenantManagerControllerComp::OnApproveConnectionRequest(
		const sdl::V1_0::imtauth::CApproveConnectionRequestGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CApproveConnectionRequestPayload response;
	
	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}
	
	sdl::V1_0::imtauth::ApproveConnectionRequestRequestArguments arguments = request.GetRequestedArguments();
	QByteArray requestId;
	QByteArray tenantId;
	if (arguments.input->requestId){
		requestId = *arguments.input->requestId;
	}
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}

	if (!m_tenantManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant manager is not configured");
		return response;
	}

	QString accessError;
	if (!HasTenantManageAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
				{s_connectOrganizationPermission},
				accessError)){
		response.errorMessage = accessError;
		return response;
	}
	
	QByteArray connectionId = m_connectionRequestManagerCompPtr->ApproveConnectionRequest(requestId, tenantId);
	if (connectionId.isEmpty()){
		response.errorMessage = QStringLiteral("Failed to approve connection request");
	} else {
		response.connectionId = connectionId;
	}
	
	return response;
}


sdl::V1_0::imtauth::CRejectConnectionRequestPayload CTenantManagerControllerComp::OnRejectConnectionRequest(
		const sdl::V1_0::imtauth::CRejectConnectionRequestGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRejectConnectionRequestPayload response;
	
	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}
	
	sdl::V1_0::imtauth::RejectConnectionRequestRequestArguments arguments = request.GetRequestedArguments();
	QByteArray requestId;
	QByteArray tenantId;
	if (arguments.input->requestId){
		requestId = *arguments.input->requestId;
	}
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}

	if (!m_tenantManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant manager is not configured");
		return response;
	}

	QString accessError;
	if (!HasTenantManageAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
				{s_connectOrganizationPermission},
				accessError)){
		response.errorMessage = accessError;
		return response;
	}
	
	bool success = m_connectionRequestManagerCompPtr->RejectConnectionRequest(requestId, tenantId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to reject connection request");
	}
	
	return response;
}


sdl::V1_0::imtauth::CCancelConnectionRequestPayload CTenantManagerControllerComp::OnCancelConnectionRequest(
		const sdl::V1_0::imtauth::CCancelConnectionRequestGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CCancelConnectionRequestPayload response;
	
	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}
	
	sdl::V1_0::imtauth::CancelConnectionRequestRequestArguments arguments = request.GetRequestedArguments();
	QByteArray requestId;
	QByteArray tenantId;
	if (arguments.input->requestId){
		requestId = *arguments.input->requestId;
	}
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}

	if (!m_tenantManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant manager is not configured");
		return response;
	}

	QString accessError;
	if (!HasTenantManageAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
				{s_connectOrganizationPermission},
				accessError)){
		response.errorMessage = accessError;
		return response;
	}
	
	bool success = m_connectionRequestManagerCompPtr->CancelConnectionRequest(requestId, tenantId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to cancel connection request");
	}
	
	return response;
}


sdl::V1_0::imtauth::CGetConnectionsPayload CTenantManagerControllerComp::OnGetConnections(
			const sdl::V1_0::imtauth::CGetConnectionsGqlRequest& request,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetConnectionsPayload response;

	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}

	sdl::V1_0::imtauth::GetConnectionsRequestArguments arguments = request.GetRequestedArguments();
	QByteArray tenantId;
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}

	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}
	if (!imtbase::isValidUuid(tenantId)){
		response.errorMessage = QStringLiteral("Invalid tenant ID format");
		return response;
	}

	if (!m_tenantManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant manager is not configured");
		return response;
	}

	QString accessError;
	if (!HasTenantManageAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
				{s_viewOrganizationConnectionsPermission},
				accessError)){
		response.errorMessage = accessError;
		return response;
	}

	response.connections.Emplace();
	QByteArrayList connectionIds = m_connectionRequestManagerCompPtr->GetConnectionIds(tenantId);

	for (const QByteArray& connectionId : connectionIds){
		imtauth::ITenantConnectionInfoUniquePtr connPtr = m_connectionRequestManagerCompPtr->GetConnection(connectionId);
		if (connPtr.IsValid()){
			sdl::V1_0::imtauth::CTenantConnection data;

			data.id = connPtr->GetConnectionId();
			data.tenantAId = connPtr->GetTenantAId();
			data.tenantBId = connPtr->GetTenantBId();
			data.status = ToSdlConnectionStatus(connPtr->GetStatus());
			data.createdAt = connPtr->GetCreatedAt();
			data.updatedAt = connPtr->GetUpdatedAt();

			if (m_tenantManagerCompPtr.IsValid()){
				imtauth::ITenantInfoUniquePtr tenant1Ptr = m_tenantManagerCompPtr->GetTenant(connPtr->GetTenantAId());
				if (tenant1Ptr.IsValid()){
					data.tenantAName = tenant1Ptr->GetTenantName();
					if (m_userCollectionCompPtr.IsValid()){
						QByteArray ownerId = tenant1Ptr->GetOwnerId();
						if (!ownerId.isEmpty()){
							data.tenantAOwnerName = imtauth::GetUserName(*m_userCollectionCompPtr, ownerId);
						}
					}
				}

				imtauth::ITenantInfoUniquePtr tenant2Ptr = m_tenantManagerCompPtr->GetTenant(connPtr->GetTenantBId());
				if (tenant2Ptr.IsValid()){
					data.tenantBName = tenant2Ptr->GetTenantName();
					if (m_userCollectionCompPtr.IsValid()){
						QByteArray ownerId = tenant2Ptr->GetOwnerId();
						if (!ownerId.isEmpty()){
							data.tenantBOwnerName = imtauth::GetUserName(*m_userCollectionCompPtr, ownerId);
						}
					}
				}
			}

			response.connections->push_back(data);
		}
	}

	return response;
}


sdl::V1_0::imtauth::CRemoveConnectionPayload CTenantManagerControllerComp::OnRemoveConnection(
		const sdl::V1_0::imtauth::CRemoveConnectionGqlRequest& request,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRemoveConnectionPayload response;
	
	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}
	
	sdl::V1_0::imtauth::RemoveConnectionRequestArguments arguments = request.GetRequestedArguments();
	QByteArray connectionId;
	QByteArray tenantId;
	if (arguments.input->connectionId){
		connectionId = *arguments.input->connectionId;
	}
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}

	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		response.success = false;
		return response;
	}

	if (m_tenantManagerCompPtr.IsValid()){
		QString accessError;
		if (!HasTenantManageAccess(
					*m_tenantManagerCompPtr.GetPtr(),
					m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
					gqlRequest,
					tenantId,
					{s_removeOrganizationConnectionPermission},
					accessError)){
			response.errorMessage = accessError;
			response.success = false;
			return response;
		}
	}
	
	// The access check above is for the given organization: the connection must be its own and active before anything ends
	imtauth::ITenantConnectionInfoUniquePtr connectionPtr = m_connectionRequestManagerCompPtr->GetConnection(connectionId);
	if (!connectionPtr.IsValid()
				|| (connectionPtr->GetTenantAId() != tenantId && connectionPtr->GetTenantBId() != tenantId)
				|| connectionPtr->GetStatus() != imtauth::ITenantConnectionInfo::CS_ACTIVE){
		response.success = false;
		response.errorMessage = QStringLiteral("Failed to remove connection");
		return response;
	}

	// The contracts do not depend on the address book: they stay
	bool success = m_connectionRequestManagerCompPtr->RemoveConnection(connectionId, tenantId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to remove connection");
	}
	
	return response;
}


} // namespace imtauthgql


namespace
{


sdl::V1_0::imtauth::CCrossTenantMessage MessageInfoToData(const imtauth::CrossTenantMessageInfo& info)
{
	sdl::V1_0::imtauth::CCrossTenantMessage data;
	data.id = info.messageId;
	data.sourceTenantId = info.sourceTenantId;
	data.targetTenantId = info.targetTenantId;
	data.contractId = info.contractId;
	data.linkId = info.linkId;
	data.sourceObjectId = info.sourceObjectId;
	data.targetObjectId = info.targetObjectId;
	data.messageType = imtauthgql::ToSdlMessageType(info.messageType);
	data.customType = info.customType;
	data.payload = QString::fromUtf8(info.payload);
	data.status = imtauthgql::ToSdlMessageStatus(info.status);
	data.errorMessage = info.errorMessage;
	data.createdAt = info.createdAt;
	data.updatedAt = info.updatedAt;
	data.expiresAt = info.expiresAt;
	return data;
}


sdl::V1_0::imtauth::COrderRequest OrderRequestToData(const imtauth::OrderRequestInfo& info)
{
	sdl::V1_0::imtauth::COrderRequest data;
	data.id = info.orderRequestId;
	data.messageId = info.messageId;
	data.sourceTenantId = info.sourceTenantId;
	data.targetTenantId = info.targetTenantId;
	data.contractId = info.contractId;
	data.linkId = info.linkId;
	data.sourceOrderId = info.sourceOrderId;
	data.articleNumber = info.articleNumber;
	data.quantity = info.quantity;
	data.note = info.note;
	data.status = imtauthgql::ToSdlOrderStatus(info.status);
	data.statusNote = info.statusNote;
	data.createdAt = info.createdAt;
	data.updatedAt = info.updatedAt;
	return data;
}


} // anonymous namespace


namespace imtauthgql
{


sdl::V1_0::imtauth::CGetCrossTenantMessagePayload CTenantManagerControllerComp::OnGetCrossTenantMessage(
		const sdl::V1_0::imtauth::CGetCrossTenantMessageGqlRequest& getCrossTenantMessageRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetCrossTenantMessagePayload response;
	
	
	if (!m_messageBrokerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Cross-tenant message broker is not configured");
		return response;
	}
	
	QByteArray messageId;
	sdl::V1_0::imtauth::GetCrossTenantMessageRequestArguments arguments = getCrossTenantMessageRequest.GetRequestedArguments();
	if (arguments.input->messageId){
		messageId = *arguments.input->messageId;
	}
	
	if (messageId.isEmpty()){
		response.errorMessage = QStringLiteral("Message ID is required");
		return response;
	}
	
	imtauth::CrossTenantMessageInfo info = m_messageBrokerCompPtr->GetMessage(messageId);
	if (info.messageId.isEmpty()){
		response.errorMessage = QStringLiteral("Message not found");
		return response;
	}

	QString accessError;
	if (!HasAccessToAnyTenant(gqlRequest, {info.sourceTenantId, info.targetTenantId}, {s_viewOrganizationConnectionsPermission}, accessError)){
		response.errorMessage = accessError;
		return response;
	}
	
	response.message = MessageInfoToData(info);
	
	return response;
}


sdl::V1_0::imtauth::CGetCrossTenantMessagesPayload CTenantManagerControllerComp::OnGetCrossTenantMessages(
		const sdl::V1_0::imtauth::CGetCrossTenantMessagesGqlRequest& getCrossTenantMessagesRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetCrossTenantMessagesPayload response;
	
	if (!m_messageBrokerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Cross-tenant message broker is not configured");
		return response;
	}
	
	QByteArray tenantId;
	bool incomingOnly = false;
	bool outgoingOnly = false;
	sdl::V1_0::imtauth::GetCrossTenantMessagesRequestArguments arguments = getCrossTenantMessagesRequest.GetRequestedArguments();
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	if (arguments.input->direction){
		if (*arguments.input->direction == sdl::V1_0::imtauth::CrossTenantMessageDirection::Incoming){
			incomingOnly = true;
		}
		else if (*arguments.input->direction == sdl::V1_0::imtauth::CrossTenantMessageDirection::Outgoing){
			outgoingOnly = true;
		}
	}
	
	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}

	QString accessError;
	if (!HasAccessToAnyTenant(gqlRequest, {tenantId}, {s_viewOrganizationConnectionsPermission}, accessError)){
		response.errorMessage = accessError;
		return response;
	}
	
	response.messages.Emplace();
	
	QSet<QByteArray> seenIds;
	if (!outgoingOnly){
		const imtauth::CrossTenantMessages incoming = m_messageBrokerCompPtr->GetIncomingMessages(tenantId);
		for (const imtauth::CrossTenantMessageInfo& info : incoming){
			if (!seenIds.contains(info.messageId)){
				seenIds.insert(info.messageId);
				response.messages->push_back(MessageInfoToData(info));
			}
		}
	}
	if (!incomingOnly){
		const imtauth::CrossTenantMessages outgoing = m_messageBrokerCompPtr->GetOutgoingMessages(tenantId);
		for (const imtauth::CrossTenantMessageInfo& info : outgoing){
			if (!seenIds.contains(info.messageId)){
				seenIds.insert(info.messageId);
				response.messages->push_back(MessageInfoToData(info));
			}
		}
	}
	
	return response;
}


sdl::V1_0::imtauth::CSendCrossTenantMessagePayload CTenantManagerControllerComp::OnSendCrossTenantMessage(
		const sdl::V1_0::imtauth::CSendCrossTenantMessageGqlRequest& sendCrossTenantMessageRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CSendCrossTenantMessagePayload response;
	
	
	if (!m_messageBrokerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Cross-tenant message broker is not configured");
		return response;
	}
	
	QByteArray sourceTenantId;
	QByteArray targetTenantId;
	QByteArray contractId;
	QByteArray linkId;
	imtauth::CrossTenantMessageType messageType = imtauth::CTMT_CUSTOM;
	QByteArray payload;
	QByteArray sourceObjectId;
	QString customType;
	QString expiresAt;
	
	sdl::V1_0::imtauth::SendCrossTenantMessageRequestArguments arguments = sendCrossTenantMessageRequest.GetRequestedArguments();
	if (arguments.input->sourceTenantId){
		sourceTenantId = *arguments.input->sourceTenantId;
	}
	if (arguments.input->targetTenantId){
		targetTenantId = *arguments.input->targetTenantId;
	}
	if (arguments.input->contractId){
		contractId = *arguments.input->contractId;
	}
	if (arguments.input->linkId){
		linkId = *arguments.input->linkId;
	}
	if (arguments.input->messageType){
		messageType = FromSdlMessageType(*arguments.input->messageType);
	}
	if (arguments.input->payload){
		payload = (*arguments.input->payload).toUtf8();
	}
	if (arguments.input->sourceObjectId){
		sourceObjectId = *arguments.input->sourceObjectId;
	}
	if (arguments.input->customType){
		customType = *arguments.input->customType;
	}
	if (arguments.input->expiresAt){
		expiresAt = *arguments.input->expiresAt;
	}

	QString accessError;
	if (!HasAccessToAnyTenant(gqlRequest, {sourceTenantId}, {s_connectOrganizationPermission}, accessError)){
		response.errorMessage = accessError;
		return response;
	}

	QString refusalReason;
	QByteArray messageId = m_messageBrokerCompPtr->SendMessage(
				sourceTenantId,
				targetTenantId,
				contractId,
				linkId,
				messageType,
				payload,
				sourceObjectId,
				customType,
				expiresAt,
				&refusalReason);
	if (messageId.isEmpty()){
		response.errorMessage = refusalReason.isEmpty() ? QStringLiteral("Failed to send cross-tenant message") : refusalReason;
		return response;
	}

	response.messageId = messageId;
	
	return response;
}


sdl::V1_0::imtauth::CUpdateCrossTenantMessageStatusPayload CTenantManagerControllerComp::OnUpdateCrossTenantMessageStatus(
		const sdl::V1_0::imtauth::CUpdateCrossTenantMessageStatusGqlRequest& updateCrossTenantMessageStatusRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CUpdateCrossTenantMessageStatusPayload response;
	
	
	if (!m_messageBrokerCompPtr.IsValid()){
		response.success = false;
		response.errorMessage = QStringLiteral("Cross-tenant message broker is not configured");
		return response;
	}
	
	QByteArray messageId;
	imtauth::CrossTenantMessageStatus status = imtauth::CTMS_CREATED;
	QString statusErrorMessage;
	
	sdl::V1_0::imtauth::UpdateCrossTenantMessageStatusRequestArguments arguments = updateCrossTenantMessageStatusRequest.GetRequestedArguments();
	if (arguments.input->messageId){
		messageId = *arguments.input->messageId;
	}
	if (arguments.input->status){
		status = FromSdlMessageStatus(*arguments.input->status);
	}
	if (arguments.input->errorMessage){
		statusErrorMessage = *arguments.input->errorMessage;
	}

	const imtauth::CrossTenantMessageInfo message = m_messageBrokerCompPtr->GetMessage(messageId);
	QString accessError;
	if (!HasAccessToAnyTenant(gqlRequest, {message.sourceTenantId, message.targetTenantId}, {s_connectOrganizationPermission}, accessError)){
		response.success = false;
		response.errorMessage = accessError;
		return response;
	}
	
	bool success = m_messageBrokerCompPtr->UpdateMessageStatus(messageId, status, statusErrorMessage);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to update cross-tenant message status");
	}
	
	return response;
}


sdl::V1_0::imtauth::CGetOrderRequestPayload CTenantManagerControllerComp::OnGetOrderRequest(
		const sdl::V1_0::imtauth::CGetOrderRequestGqlRequest& getOrderRequestRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetOrderRequestPayload response;
	
	
	if (!m_orderRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Order request manager is not configured");
		return response;
	}
	
	QByteArray orderRequestId;
	sdl::V1_0::imtauth::GetOrderRequestRequestArguments arguments = getOrderRequestRequest.GetRequestedArguments();
	if (arguments.input->orderRequestId){
		orderRequestId = *arguments.input->orderRequestId;
	}
	
	if (orderRequestId.isEmpty()){
		response.errorMessage = QStringLiteral("Order request ID is required");
		return response;
	}
	
	imtauth::OrderRequestInfo info = m_orderRequestManagerCompPtr->GetOrderRequest(orderRequestId);
	if (info.orderRequestId.isEmpty()){
		response.errorMessage = QStringLiteral("Order request not found");
		return response;
	}

	QString accessError;
	if (!HasAccessToAnyTenant(gqlRequest, {info.sourceTenantId, info.targetTenantId}, {s_viewOrganizationConnectionsPermission}, accessError)){
		response.errorMessage = accessError;
		return response;
	}
	
	response.orderRequest = OrderRequestToData(info);
	
	return response;
}


sdl::V1_0::imtauth::CGetOrderRequestsPayload CTenantManagerControllerComp::OnGetOrderRequests(
		const sdl::V1_0::imtauth::CGetOrderRequestsGqlRequest& getOrderRequestsRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetOrderRequestsPayload response;
	
	
	if (!m_orderRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Order request manager is not configured");
		return response;
	}
	
	QByteArray tenantId;
	sdl::V1_0::imtauth::GetOrderRequestsRequestArguments arguments = getOrderRequestsRequest.GetRequestedArguments();
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	
	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}

	QString accessError;
	if (!HasAccessToAnyTenant(gqlRequest, {tenantId}, {s_viewOrganizationConnectionsPermission}, accessError)){
		response.errorMessage = accessError;
		return response;
	}
	
	response.orderRequests.Emplace();
	
	const imtauth::OrderRequests orderRequests = m_orderRequestManagerCompPtr->GetOrderRequests(tenantId);
	for (const imtauth::OrderRequestInfo& info : orderRequests){
		response.orderRequests->push_back(OrderRequestToData(info));
	}
	
	return response;
}


sdl::V1_0::imtauth::CConfirmOrderRequestPayload CTenantManagerControllerComp::OnConfirmOrderRequest(
		const sdl::V1_0::imtauth::CConfirmOrderRequestGqlRequest& confirmOrderRequestRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CConfirmOrderRequestPayload response;
	
	
	if (!m_orderRequestManagerCompPtr.IsValid()){
		response.success = false;
		response.errorMessage = QStringLiteral("Order request manager is not configured");
		return response;
	}
	
	QByteArray orderRequestId;
	QString note;
	sdl::V1_0::imtauth::ConfirmOrderRequestRequestArguments arguments = confirmOrderRequestRequest.GetRequestedArguments();
	if (arguments.input->orderRequestId){
		orderRequestId = *arguments.input->orderRequestId;
	}
	if (arguments.input->note){
		note = *arguments.input->note;
	}

	QString accessError;
	if (!HasAccessToAnyTenant(gqlRequest, {m_orderRequestManagerCompPtr->GetOrderRequest(orderRequestId).targetTenantId}, {s_connectOrganizationPermission}, accessError)){
		response.success = false;
		response.errorMessage = accessError;
		return response;
	}
	
	bool success = m_orderRequestManagerCompPtr->ConfirmOrderRequest(orderRequestId, note);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to confirm order request");
	}
	
	return response;
}


sdl::V1_0::imtauth::CRejectOrderRequestPayload CTenantManagerControllerComp::OnRejectOrderRequest(
		const sdl::V1_0::imtauth::CRejectOrderRequestGqlRequest& rejectOrderRequestRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRejectOrderRequestPayload response;
	
	
	if (!m_orderRequestManagerCompPtr.IsValid()){
		response.success = false;
		response.errorMessage = QStringLiteral("Order request manager is not configured");
		return response;
	}
	
	QByteArray orderRequestId;
	QString reason;
	sdl::V1_0::imtauth::RejectOrderRequestRequestArguments arguments = rejectOrderRequestRequest.GetRequestedArguments();
	if (arguments.input->orderRequestId){
		orderRequestId = *arguments.input->orderRequestId;
	}
	if (arguments.input->reason){
		reason = *arguments.input->reason;
	}

	QString accessError;
	if (!HasAccessToAnyTenant(gqlRequest, {m_orderRequestManagerCompPtr->GetOrderRequest(orderRequestId).targetTenantId}, {s_connectOrganizationPermission}, accessError)){
		response.success = false;
		response.errorMessage = accessError;
		return response;
	}
	
	bool success = m_orderRequestManagerCompPtr->RejectOrderRequest(orderRequestId, reason);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to reject order request");
	}
	
	return response;
}


sdl::V1_0::imtauth::CUpdateOrderRequestStatusPayload CTenantManagerControllerComp::OnUpdateOrderRequestStatus(
		const sdl::V1_0::imtauth::CUpdateOrderRequestStatusGqlRequest& updateOrderRequestStatusRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CUpdateOrderRequestStatusPayload response;
	
	
	if (!m_orderRequestManagerCompPtr.IsValid()){
		response.success = false;
		response.errorMessage = QStringLiteral("Order request manager is not configured");
		return response;
	}
	
	QByteArray orderRequestId;
	imtauth::OrderRequestStatus status = imtauth::ORS_RECEIVED;
	QString note;
	sdl::V1_0::imtauth::UpdateOrderRequestStatusRequestArguments arguments = updateOrderRequestStatusRequest.GetRequestedArguments();
	if (arguments.input->orderRequestId){
		orderRequestId = *arguments.input->orderRequestId;
	}
	if (arguments.input->status){
		status = FromSdlOrderStatus(*arguments.input->status);
	}
	if (arguments.input->note){
		note = *arguments.input->note;
	}

	QString accessError;
	if (!HasAccessToAnyTenant(gqlRequest, {m_orderRequestManagerCompPtr->GetOrderRequest(orderRequestId).targetTenantId}, {s_connectOrganizationPermission}, accessError)){
		response.success = false;
		response.errorMessage = accessError;
		return response;
	}
	
	bool success = m_orderRequestManagerCompPtr->UpdateOrderRequestStatus(orderRequestId, status, note);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to update order request status");
	}
	
	return response;
}


// private methods

bool CTenantManagerControllerComp::HasAccessToAnyTenant(
			const ::imtgql::CGqlRequest& gqlRequest,
			const QByteArrayList& tenantIds,
			const QByteArrayList& requiredOrgPermissions,
			QString& errorMessage) const
{
	if (!m_tenantManagerCompPtr.IsValid()){
		errorMessage = QStringLiteral("Tenant manager is not configured");
		return false;
	}

	for (const QByteArray& tenantId : tenantIds){
		if (!tenantId.isEmpty()
					&& HasTenantManageAccess(
								*m_tenantManagerCompPtr.GetPtr(),
								m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
								gqlRequest,
								tenantId,
								requiredOrgPermissions,
								errorMessage)){
			errorMessage.clear();
			return true;
		}
	}

	if (errorMessage.isEmpty()){
		errorMessage = QStringLiteral("Access denied: tenant is outside current user scope");
	}

	return false;
}


} // namespace imtauthgql
