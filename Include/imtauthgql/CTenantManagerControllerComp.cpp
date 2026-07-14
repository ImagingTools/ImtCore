// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantManagerControllerComp.h>
#include <imtauthgql/imtauthgql.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


// Qt includes
#include <QtCore/QSet>
#include <QtCore/QMap>

// ImtCore includes
#include <imtauth/imtauth.h>
#include <imtauth/ITenantMembership.h>
#include <imtauth/ITenantRelationshipInfo.h>
#include <imtauth/ITenantRelationshipProposalInfo.h>
#include <iqt/iqt.h>
#include <imtgql/IGqlContext.h>
#include <imtbase/imtbase.h>


namespace
{


bool HasTenantIsolationAccess(
			const imtauth::ITenantManager& tenantManager,
			const imtauth::ITenantMembershipManager* membershipManagerPtr,
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArray& tenantId,
			QString& errorMessage)
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr == nullptr){
		errorMessage = QStringLiteral("Access denied: request context is missing");
		return false;
	}

	const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
	if (userInfoPtr != nullptr){
		if (userInfoPtr->IsAdmin()){
			return true;
		}
	}

	const QByteArray contextTenantId = contextPtr->GetTenantId();
	if (!contextTenantId.isEmpty()){
		if (contextTenantId == tenantId){
			return true;
		}

		errorMessage = QStringLiteral("Access denied: requested tenant does not match current tenant context");
		return false;
	}

	const QByteArray userId = contextPtr->GetUserId();
	if (userId.isEmpty()){
		errorMessage = QStringLiteral("Access denied: unauthenticated request context");
		return false;
	}

	imtauth::ITenantInfoUniquePtr tenantPtr = tenantManager.GetTenant(tenantId);
	if (tenantPtr.IsValid() && tenantPtr->GetOwnerId() == userId){
		return true;
	}

	if (membershipManagerPtr != nullptr && membershipManagerPtr->IsMember(userId, tenantId)){
		return true;
	}

	errorMessage = QStringLiteral("Access denied: tenant is outside current user scope");
	return false;
}


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

	const QByteArrayList tenantIds = tenantManager.GetTenantIds();
	for (const QByteArray& id : tenantIds){
		if (!excludeTenantId.isEmpty() && id == excludeTenantId){
			continue;
		}

		imtauth::ITenantInfoUniquePtr tenantPtr = tenantManager.GetTenant(id);
		if (!tenantPtr.IsValid()){
			continue;
		}

		if (NormalizeTenantName(tenantPtr->GetTenantName()).compare(normalizedName, Qt::CaseInsensitive) == 0){
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


bool HasTenantManageAccess(
			const imtauth::ITenantManager& tenantManager,
			const imtauth::ITenantMembershipManager* membershipManagerPtr,
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArray& tenantId,
			const QByteArray& requiredOrgPermission,
			QString& errorMessage)
{
	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	if (contextPtr == nullptr){
		errorMessage = QStringLiteral("Access denied: request context is missing");
		return false;
	}

	const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
	if (userInfoPtr != nullptr && userInfoPtr->IsAdmin()){
		return true;
	}

	const QByteArray contextTenantId = contextPtr->GetTenantId();
	if (!contextTenantId.isEmpty() && contextTenantId != tenantId){
		errorMessage = QStringLiteral("Access denied: requested tenant does not match current tenant context");
		return false;
	}

	const QByteArray userId = contextPtr->GetUserId();
	if (userId.isEmpty()){
		errorMessage = QStringLiteral("Access denied: unauthenticated request context");
		return false;
	}

	imtauth::ITenantInfoUniquePtr tenantPtr = tenantManager.GetTenant(tenantId);
	if (!tenantPtr.IsValid()){
		errorMessage = QStringLiteral("Tenant not found");
		return false;
	}

	if (tenantPtr->GetOwnerId() == userId || tenantPtr->GetCreatorId() == userId){
		return true;
	}

	if (!requiredOrgPermission.isEmpty() && membershipManagerPtr != nullptr){
		imtauth::ITenantMembershipUniquePtr msPtr = membershipManagerPtr->FindMembership(userId, tenantId);
		if (msPtr.IsValid() && msPtr->IsActive() && msPtr->GetOrganizationPermissions().contains(requiredOrgPermission)){
			return true;
		}
	}

	errorMessage = QStringLiteral("Access denied: insufficient permissions to manage tenant");
	return false;
}


} // anonymous namespace


namespace imtauthgql
{

// protected methods

// reimplemented (sdl::V1_0::imtauth::CTenantsGqlHandlerCompBase)

sdl::V1_0::imtauth::CGetTenantIdsPayload CTenantManagerControllerComp::OnGetTenantIds(
			const sdl::V1_0::imtauth::CGetTenantIdsGqlRequest& /*getTenantIdsRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetTenantIdsPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	QByteArrayList ids = m_tenantManagerCompPtr->GetTenantIds();

	response.tenantIds.Emplace().FromList(ids);

	return response;
}


sdl::V1_0::imtauth::CGetTenantRelationshipsPayload CTenantManagerControllerComp::OnGetTenantRelationships(
			const sdl::V1_0::imtauth::CGetTenantRelationshipsGqlRequest& getTenantRelationshipsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetTenantRelationshipsPayload response;

	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}

	QByteArray tenantId;
	sdl::V1_0::imtauth::GetTenantRelationshipsRequestArguments arguments = getTenantRelationshipsRequest.GetRequestedArguments();
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
	if (!HasTenantIsolationAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
				accessError)){
		response.errorMessage = accessError;
		return response;
	}

	response.relationships.Emplace();

	QByteArrayList relIds = m_connectionRequestManagerCompPtr->GetTenantRelationshipIds(tenantId);
	for (const QByteArray& relId : relIds){
		imtauth::ITenantRelationshipInfoUniquePtr relPtr = m_connectionRequestManagerCompPtr->GetTenantRelationship(relId);
		if (!relPtr.IsValid()){
			continue;
		}

		sdl::V1_0::imtauth::CTenantRelationship data;
		data.id = relPtr->GetRelationshipId();
		data.connectionId = relPtr->GetConnectionId();
		data.sourceTenantId = relPtr->GetSourceTenantId();
		data.targetTenantId = relPtr->GetTargetTenantId();
		data.sourceRole = ToSdlRelationshipRole(relPtr->GetSourceRole());
		data.targetRole = ToSdlRelationshipRole(relPtr->GetTargetRole());
		data.scope = relPtr->GetScope();
		data.validFrom = relPtr->GetValidFrom();
		data.validUntil = relPtr->GetValidUntil();
		data.status = ToSdlRelationshipStatus(relPtr->GetStatus());
		data.description = relPtr->GetDescription();
		data.createdAt = relPtr->GetCreatedAt();
		data.updatedAt = relPtr->GetUpdatedAt();

		if (m_tenantManagerCompPtr.IsValid()){
			imtauth::ITenantInfoUniquePtr sourceTenantPtr = m_tenantManagerCompPtr->GetTenant(relPtr->GetSourceTenantId());
			if (sourceTenantPtr.IsValid()){
				data.sourceTenantName = sourceTenantPtr->GetTenantName();
			}

			imtauth::ITenantInfoUniquePtr targetTenantPtr = m_tenantManagerCompPtr->GetTenant(relPtr->GetTargetTenantId());
			if (targetTenantPtr.IsValid()){
				data.targetTenantName = targetTenantPtr->GetTenantName();
			}
		}

		response.relationships->push_back(data);
	}

	return response;
}


sdl::V1_0::imtauth::CGetTenantPayload CTenantManagerControllerComp::OnGetTenant(
			const sdl::V1_0::imtauth::CGetTenantGqlRequest& getTenantRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
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

	imtauth::ITenantInfoUniquePtr tenantInfoPtr = m_tenantManagerCompPtr->GetTenant(tenantId);

	if (!tenantInfoPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant not found");
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
	if (!HasTenantIsolationAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
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
	QByteArray ownerId = GetUserId(gqlRequest);
	QByteArray parentTenantId;
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
	if (arguments.input->parentTenantId){
		parentTenantId = *arguments.input->parentTenantId;
	}

	name = NormalizeTenantName(name);
	if (name.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant name is required");
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
		response.errorMessage = QStringLiteral("Tenant with this name already exists");
		return response;
	}

	if (!parentTenantId.isEmpty()){
		if (!imtbase::isValidUuid(parentTenantId)){
			response.errorMessage = QStringLiteral("Invalid parent tenant ID format");
			return response;
		}

		if (!m_tenantManagerCompPtr->GetTenant(parentTenantId).IsValid()){
			response.errorMessage = QStringLiteral("Parent tenant not found");
			return response;
		}
	}

	QByteArray tenantId = m_tenantManagerCompPtr->CreateTenant(name, description, ownerId);

	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Failed to create tenant");
		return response;
	}

	// Set hierarchy fields if parentTenantId was provided
	if (!parentTenantId.isEmpty()){
		m_tenantManagerCompPtr->SetTenantHierarchy(tenantId, parentTenantId);
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
	if (!HasTenantManageAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
				QByteArray(),
				accessError)){
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
		response.errorMessage = QStringLiteral("Tenant not found");
		response.success = false;
		return response;
	}

	QString accessError;
	if (!HasTenantManageAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
				QByteArrayLiteral("EditOrganization"),
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
		response.errorMessage = QStringLiteral("Tenant name is required");
		response.success = false;
		return response;
	}

	if (HasTenantNameConflict(*m_tenantManagerCompPtr.GetPtr(), name, tenantId)){
		response.errorMessage = QStringLiteral("Tenant with this name already exists");
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

	bool success = m_tenantManagerCompPtr->UpdateTenant(tenantId, name, description, ownerId, updateOwner);

	// Sync members with TenantMemberships
	if (success && m_membershipManagerCompPtr.IsValid() && arguments.input->members){
		// Get current member user IDs
		QByteArrayList currentMembershipIds = m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId);
		QSet<QByteArray> currentUserIds;
		QMap<QByteArray, QByteArray> userIdToMembershipId; // userId -> membershipId
		for (const QByteArray& msId : currentMembershipIds){
			imtauth::ITenantMembershipUniquePtr msPtr = m_membershipManagerCompPtr->GetMembership(msId);
			if (msPtr.IsValid()){
				currentUserIds.insert(msPtr->GetUserId());
				userIdToMembershipId[msPtr->GetUserId()] = msId;
			}
		}

		// Build new set
		QSet<QByteArray> newUserIds;
		for (const auto& member : *arguments.input->members){
			if (member->id){
				newUserIds << *member->id;
			}
		}

		// Remove memberships for users no longer in the list
		for (const QByteArray& uid : currentUserIds){
			if (!newUserIds.contains(uid)){
				m_membershipManagerCompPtr->RemoveMembership(userIdToMembershipId[uid]);
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
				QByteArray(),
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


sdl::V1_0::imtauth::CRemoveTenantRelationshipPayload CTenantManagerControllerComp::OnRemoveTenantRelationship(
			const sdl::V1_0::imtauth::CRemoveTenantRelationshipGqlRequest& removeTenantRelationshipRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRemoveTenantRelationshipPayload response;

	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.success = false;
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}

	QByteArray tenantId;
	QByteArray relationshipId;
	sdl::V1_0::imtauth::RemoveTenantRelationshipRequestArguments arguments = removeTenantRelationshipRequest.GetRequestedArguments();
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	if (arguments.input->relationshipId){
		relationshipId = *arguments.input->relationshipId;
	}

	bool success = m_connectionRequestManagerCompPtr->RemoveTenantRelationship(tenantId, relationshipId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to remove tenant relationship");
	}

	return response;
}


} // namespace imtauthgql


namespace
{


sdl::V1_0::imtauth::CCrossOrgGrant GrantInfoToData(const imtauth::CrossOrgGrantInfo& info)
{
	sdl::V1_0::imtauth::CCrossOrgGrant data;
	data.id = info.grantId;
	data.sourceTenantId = info.sourceTenantId;
	data.targetTenantId = info.targetTenantId;
	data.roleIds.Emplace().FromList(info.roleIds);
	data.description = info.description;
	data.createdAt = info.createdAt;
	data.expiresAt = info.expiresAt;
	data.isActive = info.isActive;
	return data;
}


sdl::V1_0::imtauth::CContract ContractInfoToData(const imtauth::ContractInfo& info)
{
	sdl::V1_0::imtauth::CContract data;
	data.id = info.contractId;
	data.relationshipId = info.relationshipId;
	data.sourceTenantId = info.sourceTenantId;
	data.targetTenantId = info.targetTenantId;
	data.status = imtauthgql::ToSdlContractStatus(info.status);
	data.scope = info.scope;
	data.validFrom = info.validFrom;
	data.validUntil = info.validUntil;
	data.description = info.description;
	data.terms = info.terms;
	data.createdAt = info.createdAt;
	data.updatedAt = info.updatedAt;
	return data;
}


} // anonymous namespace


namespace imtauthgql
{


sdl::V1_0::imtauth::CGetCrossOrgGrantsPayload CTenantManagerControllerComp::OnGetCrossOrgGrants(
			const sdl::V1_0::imtauth::CGetCrossOrgGrantsGqlRequest& getCrossOrgGrantsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetCrossOrgGrantsPayload response;

	if (!m_grantManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Cross-org grant manager is not configured");
		return response;
	}

	QByteArray tenantId;
	sdl::V1_0::imtauth::GetCrossOrgGrantsRequestArguments arguments = getCrossOrgGrantsRequest.GetRequestedArguments();
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
	if (!HasTenantIsolationAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
				accessError)){
		response.errorMessage = accessError;
		return response;
	}

	response.grants.Emplace();

	QSet<QByteArray> seenIds;
	const imtauth::CrossOrgGrants outgoing = m_grantManagerCompPtr->GetGrantsBySourceTenant(tenantId);
	for (const imtauth::CrossOrgGrantInfo& info : outgoing){
		// Revoked (soft-deleted) grants are inactive and must not appear in the
		// delegation listing.
		if (!info.isActive){
			continue;
		}
		if (!seenIds.contains(info.grantId)){
			seenIds.insert(info.grantId);
			response.grants->push_back(GrantInfoToData(info));
		}
	}

	const imtauth::CrossOrgGrants incoming = m_grantManagerCompPtr->GetGrantsByTargetTenant(tenantId);
	for (const imtauth::CrossOrgGrantInfo& info : incoming){
		if (!info.isActive){
			continue;
		}
		if (!seenIds.contains(info.grantId)){
			seenIds.insert(info.grantId);
			response.grants->push_back(GrantInfoToData(info));
		}
	}

	return response;
}


sdl::V1_0::imtauth::CCreateCrossOrgGrantPayload CTenantManagerControllerComp::OnCreateCrossOrgGrant(
			const sdl::V1_0::imtauth::CCreateCrossOrgGrantGqlRequest& createCrossOrgGrantRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CCreateCrossOrgGrantPayload response;

	if (!m_grantManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Cross-org grant manager is not configured");
		return response;
	}

	QByteArray sourceTenantId;
	QByteArray targetTenantId;
	QByteArrayList roleIds;
	QString description;
	QString expiresAt;

	sdl::V1_0::imtauth::CreateCrossOrgGrantRequestArguments arguments = createCrossOrgGrantRequest.GetRequestedArguments();
	if (arguments.input->sourceTenantId){
		sourceTenantId = *arguments.input->sourceTenantId;
	}
	if (arguments.input->targetTenantId){
		targetTenantId = *arguments.input->targetTenantId;
	}
	if (arguments.input->roleIds){
		roleIds = arguments.input->roleIds->ToList();
	}
	if (arguments.input->description){
		description = *arguments.input->description;
	}
	if (arguments.input->expiresAt){
		expiresAt = *arguments.input->expiresAt;
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
				sourceTenantId,
				QByteArrayLiteral("ConnectOrganization"),
				accessError)){
		response.errorMessage = accessError;
		return response;
	}

	QByteArray grantId = m_grantManagerCompPtr->CreateGrant(
							 sourceTenantId,
							 targetTenantId,
							 roleIds,
							 description,
							 expiresAt);
	
	if (grantId.isEmpty()){
		response.errorMessage = QStringLiteral("Failed to create cross-org grant");
		return response;
	}

	response.grantId = grantId;

	return response;
}


sdl::V1_0::imtauth::CRevokeCrossOrgGrantPayload CTenantManagerControllerComp::OnRevokeCrossOrgGrant(
		const sdl::V1_0::imtauth::CRevokeCrossOrgGrantGqlRequest& revokeCrossOrgGrantRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRevokeCrossOrgGrantPayload response;
	
	
	if (!m_grantManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Cross-org grant manager is not configured");
		response.success = false;
		return response;
	}
	
	QByteArray grantId;
	sdl::V1_0::imtauth::RevokeCrossOrgGrantRequestArguments arguments = revokeCrossOrgGrantRequest.GetRequestedArguments();
	if (arguments.input->grantId){
		grantId = *arguments.input->grantId;
	}

	// Enforce tenant isolation: the grant must belong to the context tenant
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	QByteArray contextTenantId;
	if (gqlContextPtr != nullptr){
		contextTenantId = gqlContextPtr->GetTenantId();
	}
	if (!contextTenantId.isEmpty()){
		imtauth::CrossOrgGrantInfo grantInfo = m_grantManagerCompPtr->GetGrant(grantId);
		if (grantInfo.sourceTenantId != contextTenantId && grantInfo.targetTenantId != contextTenantId){
			response.errorMessage = QStringLiteral("Access denied: grant does not belong to the current tenant");
			response.success = false;
			return response;
		}
	}
	
	bool success = m_grantManagerCompPtr->RevokeGrant(grantId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to revoke cross-org grant");
	}
	
	return response;
}


sdl::V1_0::imtauth::CRemoveCrossOrgGrantsPayload CTenantManagerControllerComp::OnRemoveCrossOrgGrants(
		const sdl::V1_0::imtauth::CRemoveCrossOrgGrantsGqlRequest& removeCrossOrgGrantsRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRemoveCrossOrgGrantsPayload response;

	if (!m_grantManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Cross-org grant manager is not configured");
		response.success = false;
		return response;
	}

	QByteArrayList grantIds;
	sdl::V1_0::imtauth::RemoveCrossOrgGrantsRequestArguments arguments = removeCrossOrgGrantsRequest.GetRequestedArguments();
	if (arguments.input->grantIds){
		grantIds = arguments.input->grantIds->ToList();
	}

	// Enforce tenant isolation: all grants must belong to the context tenant
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	QByteArray contextTenantId;
	if (gqlContextPtr != nullptr){
		contextTenantId = gqlContextPtr->GetTenantId();
	}
	if (!contextTenantId.isEmpty()){
		for (const QByteArray& grantId : grantIds){
			if (grantId.isEmpty()){
				continue;
			}
			imtauth::CrossOrgGrantInfo grantInfo = m_grantManagerCompPtr->GetGrant(grantId);
			if (grantInfo.sourceTenantId != contextTenantId && grantInfo.targetTenantId != contextTenantId){
				response.errorMessage = QStringLiteral("Access denied: one or more grants do not belong to the current tenant");
				response.success = false;
				return response;
			}
		}
	}

	bool success = m_grantManagerCompPtr->RemoveGrants(grantIds);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to remove one or more cross-org grants");
	}

	return response;
}


sdl::V1_0::imtauth::CGetContractsPayload CTenantManagerControllerComp::OnGetContracts(
		const sdl::V1_0::imtauth::CGetContractsGqlRequest& getContractsRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetContractsPayload response;
	
	
	if (!m_contractManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Contract manager is not configured");
		return response;
	}
	
	QByteArray tenantId;
	sdl::V1_0::imtauth::GetContractsRequestArguments arguments = getContractsRequest.GetRequestedArguments();
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	
	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}
	
	response.contracts.Emplace();
	
	QSet<QByteArray> seenIds;
	const imtauth::Contracts outgoing = m_contractManagerCompPtr->GetContractsBySourceTenant(tenantId);
	for (const imtauth::ContractInfo& info : outgoing){
		if (!seenIds.contains(info.contractId)){
			seenIds.insert(info.contractId);
			response.contracts->push_back(ContractInfoToData(info));
		}
	}
	
	const imtauth::Contracts incoming = m_contractManagerCompPtr->GetContractsByTargetTenant(tenantId);
	for (const imtauth::ContractInfo& info : incoming){
		if (!seenIds.contains(info.contractId)){
			seenIds.insert(info.contractId);
			response.contracts->push_back(ContractInfoToData(info));
		}
	}
	
	return response;
}


sdl::V1_0::imtauth::CCreateContractPayload CTenantManagerControllerComp::OnCreateContract(
		const sdl::V1_0::imtauth::CCreateContractGqlRequest& createContractRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CCreateContractPayload response;
	
	
	if (!m_contractManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Contract manager is not configured");
		return response;
	}
	
	QByteArray relationshipId;
	QByteArray sourceTenantId;
	QByteArray targetTenantId;
	QString scope;
	QString validFrom;
	QString validUntil;
	QString description;
	QString terms;
	
	sdl::V1_0::imtauth::CreateContractRequestArguments arguments = createContractRequest.GetRequestedArguments();
	if (arguments.input->relationshipId){
		relationshipId = *arguments.input->relationshipId;
	}
	if (arguments.input->sourceTenantId){
		sourceTenantId = *arguments.input->sourceTenantId;
	}
	if (arguments.input->targetTenantId){
		targetTenantId = *arguments.input->targetTenantId;
	}
	if (arguments.input->scope){
		scope = *arguments.input->scope;
	}
	if (arguments.input->validFrom){
		validFrom = *arguments.input->validFrom;
	}
	if (arguments.input->validUntil){
		validUntil = *arguments.input->validUntil;
	}
	if (arguments.input->description){
		description = *arguments.input->description;
	}
	if (arguments.input->terms){
		terms = *arguments.input->terms;
	}

	QByteArray contractId = m_contractManagerCompPtr->CreateContract(
								relationshipId,
								sourceTenantId,
								targetTenantId,
								scope,
								validFrom,
								validUntil,
								description,
								terms);
	
	if (contractId.isEmpty()){
		response.errorMessage = QStringLiteral("Failed to create contract");
		return response;
	}
	
	response.contractId = contractId;
	
	return response;
}


sdl::V1_0::imtauth::CUpdateContractStatusPayload CTenantManagerControllerComp::OnUpdateContractStatus(
		const sdl::V1_0::imtauth::CUpdateContractStatusGqlRequest& updateContractStatusRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CUpdateContractStatusPayload response;
	
	
	if (!m_contractManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Contract manager is not configured");
		response.success = false;
		return response;
	}
	
	QByteArray contractId;
	imtauth::ContractStatus status = imtauth::CTS_DRAFT;
	
	sdl::V1_0::imtauth::UpdateContractStatusRequestArguments arguments = updateContractStatusRequest.GetRequestedArguments();
	if (arguments.input->contractId){
		contractId = *arguments.input->contractId;
	}
	if (arguments.input->status){
		status = FromSdlContractStatus(*arguments.input->status);
	}
	
	bool success = m_contractManagerCompPtr->UpdateContractStatus(contractId, status);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to update contract status");
	}
	
	return response;
}


sdl::V1_0::imtauth::CTerminateContractPayload CTenantManagerControllerComp::OnTerminateContract(
		const sdl::V1_0::imtauth::CTerminateContractGqlRequest& terminateContractRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CTerminateContractPayload response;
	
	
	if (!m_contractManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Contract manager is not configured");
		response.success = false;
		return response;
	}
	
	QByteArray contractId;
	sdl::V1_0::imtauth::TerminateContractRequestArguments arguments = terminateContractRequest.GetRequestedArguments();
	if (arguments.input->contractId){
		contractId = *arguments.input->contractId;
	}
	
	bool success = m_contractManagerCompPtr->TerminateContract(contractId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to terminate contract");
	}
	
	return response;
}


sdl::V1_0::imtauth::CEnsureSystemTenantPayload CTenantManagerControllerComp::OnEnsureSystemTenant(
		const sdl::V1_0::imtauth::CEnsureSystemTenantGqlRequest& /*ensureSystemTenantRequest*/,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CEnsureSystemTenantPayload response;
	
	
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
	if (!HasTenantIsolationAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
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
	if (!HasTenantIsolationAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
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
	if (!HasTenantIsolationAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
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
	if (!HasTenantIsolationAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
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
	QString message;
	if (arguments.input->sourceTenantId){
		sourceTenantId = *arguments.input->sourceTenantId;
	}
	if (arguments.input->connectionCode){
		connectionCode = *arguments.input->connectionCode;
	}
	if (arguments.input->message){
		message = *arguments.input->message;
	}

	if (sourceTenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Source tenant ID is required");
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
		if (!HasTenantIsolationAccess(
					*m_tenantManagerCompPtr.GetPtr(),
					m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
					gqlRequest,
					sourceTenantId,
					accessError)){
			response.errorMessage = accessError;
			return response;
		}
	}
	
	QByteArray requestId = m_connectionRequestManagerCompPtr->CreateConnectionRequest(sourceTenantId, connectionCode, message);
	if (requestId.isEmpty()){
		response.errorMessage = QStringLiteral("Failed to create connection request. The target organization may have disabled connections by code, or a connection already exists.");
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
	if (!HasTenantIsolationAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
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
	if (!HasTenantIsolationAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
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
	if (!HasTenantIsolationAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
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
	if (!HasTenantIsolationAccess(
				*m_tenantManagerCompPtr.GetPtr(),
				m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
				gqlRequest,
				tenantId,
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
		if (!HasTenantIsolationAccess(
					*m_tenantManagerCompPtr.GetPtr(),
					m_membershipManagerCompPtr.IsValid() ? m_membershipManagerCompPtr.GetPtr() : nullptr,
					gqlRequest,
					tenantId,
					accessError)){
			response.errorMessage = accessError;
			response.success = false;
			return response;
		}
	}
	
	bool success = m_connectionRequestManagerCompPtr->RemoveConnection(connectionId, tenantId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to remove connection");
	}
	
	return response;
}


sdl::V1_0::imtauth::CGetRelationshipProposalsPayload CTenantManagerControllerComp::OnGetRelationshipProposals(
		const sdl::V1_0::imtauth::CGetRelationshipProposalsGqlRequest& request,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetRelationshipProposalsPayload response;
	
	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}
	
	sdl::V1_0::imtauth::GetRelationshipProposalsRequestArguments arguments = request.GetRequestedArguments();
	QByteArray tenantId;
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}
	
	QByteArrayList proposalIds = m_connectionRequestManagerCompPtr->GetRelationshipProposalIds(tenantId);
	response.proposals.Emplace();
	for (const QByteArray& pId : proposalIds){
		sdl::V1_0::imtauth::CRelationshipProposal data;
		data.id = pId;
		response.proposals->push_back(data);
	}
	
	return response;
}


sdl::V1_0::imtauth::CCreateRelationshipProposalPayload CTenantManagerControllerComp::OnCreateRelationshipProposal(
		const sdl::V1_0::imtauth::CCreateRelationshipProposalGqlRequest& request,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CCreateRelationshipProposalPayload response;
	
	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}
	
	sdl::V1_0::imtauth::CreateRelationshipProposalRequestArguments arguments = request.GetRequestedArguments();
	
	if (!arguments.input->proposal){
		response.errorMessage = QStringLiteral("Proposal data is required");
		return response;
	}
	
	auto& proposal = *arguments.input->proposal;
	
	// Build a proposal info object from the SDL data
	imtauth::ITenantRelationshipProposalInfoUniquePtr proposalInfo = m_proposalFactoryCompPtr.CreateInstance();
	if (!proposalInfo.IsValid()){
		response.errorMessage = QStringLiteral("Failed to create proposal info instance");
		return response;
	}
	
	if (proposal.connectionId){
		proposalInfo->SetConnectionId(*proposal.connectionId);
	}
	if (proposal.initiatorTenantId){
		proposalInfo->SetInitiatorTenantId(*proposal.initiatorTenantId);
	}
	if (proposal.counterpartyTenantId){
		proposalInfo->SetCounterpartyTenantId(*proposal.counterpartyTenantId);
	}
	if (proposal.proposalType){
		proposalInfo->SetProposalType(imtauthgql::FromSdlProposalType(*proposal.proposalType));
	}
	if (proposal.existingRelationshipId){
		proposalInfo->SetExistingRelationshipId(*proposal.existingRelationshipId);
	}
	if (proposal.proposedSourceRole){
		proposalInfo->SetProposedSourceRole(imtauthgql::FromSdlRelationshipRole(*proposal.proposedSourceRole));
	}
	if (proposal.proposedTargetRole){
		proposalInfo->SetProposedTargetRole(imtauthgql::FromSdlRelationshipRole(*proposal.proposedTargetRole));
	}
	if (proposal.proposedScope){
		proposalInfo->SetProposedScope(*proposal.proposedScope);
	}
	if (proposal.proposedDescription){
		proposalInfo->SetProposedDescription(*proposal.proposedDescription);
	}
	if (proposal.proposedValidFrom){
		proposalInfo->SetProposedValidFrom(*proposal.proposedValidFrom);
	}
	if (proposal.proposedValidUntil){
		proposalInfo->SetProposedValidUntil(*proposal.proposedValidUntil);
	}
	if (proposal.message){
		proposalInfo->SetMessage(*proposal.message);
	}
	
	QByteArray proposalId = m_connectionRequestManagerCompPtr->CreateRelationshipProposal(*proposalInfo);
	if (proposalId.isEmpty()){
		response.errorMessage = QStringLiteral("Failed to create relationship proposal. A valid connection is required.");
	} else {
		response.proposalId = proposalId;
	}
	
	return response;
}


sdl::V1_0::imtauth::CApproveRelationshipProposalPayload CTenantManagerControllerComp::OnApproveRelationshipProposal(
		const sdl::V1_0::imtauth::CApproveRelationshipProposalGqlRequest& request,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CApproveRelationshipProposalPayload response;
	
	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}
	
	sdl::V1_0::imtauth::ApproveRelationshipProposalRequestArguments arguments = request.GetRequestedArguments();
	QByteArray proposalId;
	QByteArray tenantId;
	if (arguments.input->proposalId){
		proposalId = *arguments.input->proposalId;
	}
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	
	QByteArray relationshipId = m_connectionRequestManagerCompPtr->ApproveRelationshipProposal(proposalId, tenantId);
	response.success = true;
	if (!relationshipId.isEmpty()){
		response.relationshipId = relationshipId;
	}
	
	return response;
}


sdl::V1_0::imtauth::CRejectRelationshipProposalPayload CTenantManagerControllerComp::OnRejectRelationshipProposal(
		const sdl::V1_0::imtauth::CRejectRelationshipProposalGqlRequest& request,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRejectRelationshipProposalPayload response;
	
	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}
	
	sdl::V1_0::imtauth::RejectRelationshipProposalRequestArguments arguments = request.GetRequestedArguments();
	QByteArray proposalId;
	QByteArray tenantId;
	if (arguments.input->proposalId){
		proposalId = *arguments.input->proposalId;
	}
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	
	bool success = m_connectionRequestManagerCompPtr->RejectRelationshipProposal(proposalId, tenantId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to reject relationship proposal");
	}
	
	return response;
}


sdl::V1_0::imtauth::CCancelRelationshipProposalPayload CTenantManagerControllerComp::OnCancelRelationshipProposal(
		const sdl::V1_0::imtauth::CCancelRelationshipProposalGqlRequest& request,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CCancelRelationshipProposalPayload response;
	
	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Connection manager is not configured");
		return response;
	}
	
	sdl::V1_0::imtauth::CancelRelationshipProposalRequestArguments arguments = request.GetRequestedArguments();
	QByteArray proposalId;
	QByteArray tenantId;
	if (arguments.input->proposalId){
		proposalId = *arguments.input->proposalId;
	}
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	
	bool success = m_connectionRequestManagerCompPtr->CancelRelationshipProposal(proposalId, tenantId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to cancel relationship proposal");
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
	data.relationshipId = info.relationshipId;
	data.contractId = info.contractId;
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
	data.relationshipId = info.relationshipId;
	data.contractId = info.contractId;
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
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
	
	response.message = MessageInfoToData(info);
	
	return response;
}


sdl::V1_0::imtauth::CGetCrossTenantMessagesPayload CTenantManagerControllerComp::OnGetCrossTenantMessages(
		const sdl::V1_0::imtauth::CGetCrossTenantMessagesGqlRequest& getCrossTenantMessagesRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CSendCrossTenantMessagePayload response;
	
	
	if (!m_messageBrokerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Cross-tenant message broker is not configured");
		return response;
	}
	
	QByteArray sourceTenantId;
	QByteArray targetTenantId;
	QByteArray relationshipId;
	QByteArray contractId;
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
	if (arguments.input->relationshipId){
		relationshipId = *arguments.input->relationshipId;
	}
	if (arguments.input->contractId){
		contractId = *arguments.input->contractId;
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
	
	QByteArray messageId = m_messageBrokerCompPtr->SendMessage(
							   sourceTenantId,
							   targetTenantId,
							   relationshipId,
							   messageType,
							   payload,
							   sourceObjectId,
							   customType,
							   expiresAt,
							   contractId);
	
	if (messageId.isEmpty()){
		response.errorMessage = QStringLiteral("Failed to send cross-tenant message");
		return response;
	}
	
	response.messageId = messageId;
	
	return response;
}


sdl::V1_0::imtauth::CUpdateCrossTenantMessageStatusPayload CTenantManagerControllerComp::OnUpdateCrossTenantMessageStatus(
		const sdl::V1_0::imtauth::CUpdateCrossTenantMessageStatusGqlRequest& updateCrossTenantMessageStatusRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
	
	bool success = m_messageBrokerCompPtr->UpdateMessageStatus(messageId, status, statusErrorMessage);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to update cross-tenant message status");
	}
	
	return response;
}


sdl::V1_0::imtauth::CGetOrderRequestPayload CTenantManagerControllerComp::OnGetOrderRequest(
		const sdl::V1_0::imtauth::CGetOrderRequestGqlRequest& getOrderRequestRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
	
	response.orderRequest = OrderRequestToData(info);
	
	return response;
}


sdl::V1_0::imtauth::CGetOrderRequestsPayload CTenantManagerControllerComp::OnGetOrderRequests(
		const sdl::V1_0::imtauth::CGetOrderRequestsGqlRequest& getOrderRequestsRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
	
	response.orderRequests.Emplace();
	
	const imtauth::OrderRequests orderRequests = m_orderRequestManagerCompPtr->GetOrderRequests(tenantId);
	for (const imtauth::OrderRequestInfo& info : orderRequests){
		response.orderRequests->push_back(OrderRequestToData(info));
	}
	
	return response;
}


sdl::V1_0::imtauth::CConfirmOrderRequestPayload CTenantManagerControllerComp::OnConfirmOrderRequest(
		const sdl::V1_0::imtauth::CConfirmOrderRequestGqlRequest& confirmOrderRequestRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
	
	bool success = m_orderRequestManagerCompPtr->ConfirmOrderRequest(orderRequestId, note);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to confirm order request");
	}
	
	return response;
}


sdl::V1_0::imtauth::CRejectOrderRequestPayload CTenantManagerControllerComp::OnRejectOrderRequest(
		const sdl::V1_0::imtauth::CRejectOrderRequestGqlRequest& rejectOrderRequestRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
	
	bool success = m_orderRequestManagerCompPtr->RejectOrderRequest(orderRequestId, reason);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to reject order request");
	}
	
	return response;
}


sdl::V1_0::imtauth::CUpdateOrderRequestStatusPayload CTenantManagerControllerComp::OnUpdateOrderRequestStatus(
		const sdl::V1_0::imtauth::CUpdateOrderRequestStatusGqlRequest& updateOrderRequestStatusRequest,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
	
	bool success = m_orderRequestManagerCompPtr->UpdateOrderRequestStatus(orderRequestId, status, note);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to update order request status");
	}
	
	return response;
}



} // namespace imtauthgql
