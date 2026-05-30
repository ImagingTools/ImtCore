// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantManagerControllerComp.h>


// Qt includes
#include <QSet>
#include <QMap>

// ImtCore includes
#include <imtauth/imtauth.h>


namespace imtauthgql
{


namespace
{


imtauth::ITenantInfo::TenantRelationshipRole FromSdlRelationshipRole(sdl::imtauth::Tenants::TenantRelationshipRole role)
{
	switch (role){
	case sdl::imtauth::Tenants::TenantRelationshipRole::Parent:
		return imtauth::ITenantInfo::Parent;
	case sdl::imtauth::Tenants::TenantRelationshipRole::Child:
		return imtauth::ITenantInfo::Child;
	case sdl::imtauth::Tenants::TenantRelationshipRole::Supplier:
		return imtauth::ITenantInfo::Supplier;
	case sdl::imtauth::Tenants::TenantRelationshipRole::Customer:
		return imtauth::ITenantInfo::Customer;
	case sdl::imtauth::Tenants::TenantRelationshipRole::Affiliate:
		return imtauth::ITenantInfo::Affiliate;
	default:
		return imtauth::ITenantInfo::Partner;
	}
}


sdl::imtauth::Tenants::TenantRelationshipRole ToSdlRelationshipRole(imtauth::ITenantInfo::TenantRelationshipRole role)
{
	switch (role){
	case imtauth::ITenantInfo::Parent:
		return sdl::imtauth::Tenants::TenantRelationshipRole::Parent;
	case imtauth::ITenantInfo::Child:
		return sdl::imtauth::Tenants::TenantRelationshipRole::Child;
	case imtauth::ITenantInfo::Supplier:
		return sdl::imtauth::Tenants::TenantRelationshipRole::Supplier;
	case imtauth::ITenantInfo::Customer:
		return sdl::imtauth::Tenants::TenantRelationshipRole::Customer;
	case imtauth::ITenantInfo::Affiliate:
		return sdl::imtauth::Tenants::TenantRelationshipRole::Affiliate;
	default:
		return sdl::imtauth::Tenants::TenantRelationshipRole::Partner;
	}
}


sdl::imtauth::Tenants::CTenantRelationship::V1_0 RelationshipToData(
		const QByteArray& sourceTenantId,
		const imtauth::ITenantInfo::TenantRelationship& relationship)
{
	sdl::imtauth::Tenants::CTenantRelationship::V1_0 data;
	data.id = relationship.relationshipId;
	data.sourceTenantId = sourceTenantId;
	data.targetTenantId = relationship.targetTenantId;
	data.role = ToSdlRelationshipRole(relationship.role);
	data.sourceRole = ToSdlRelationshipRole(relationship.sourceRole);
	data.targetRole = ToSdlRelationshipRole(relationship.targetRole);
	data.scope = relationship.scope;
	data.validFrom = relationship.validFrom;
	data.validUntil = relationship.validUntil;
	data.isActive = relationship.isActive;
	data.description = relationship.description;
	data.createdAt = relationship.createdAt;
	return data;
}


sdl::imtauth::Tenants::TenantConnectionStatus ToSdlConnectionStatus(imtauth::TenantConnectionStatus status)
{
	switch (status){
	case imtauth::TCS_ACCEPTED:
		return sdl::imtauth::Tenants::TenantConnectionStatus::Accepted;
	case imtauth::TCS_REJECTED:
		return sdl::imtauth::Tenants::TenantConnectionStatus::Rejected;
	case imtauth::TCS_EXPIRED:
		return sdl::imtauth::Tenants::TenantConnectionStatus::Expired;
	case imtauth::TCS_REVOKED:
		return sdl::imtauth::Tenants::TenantConnectionStatus::Revoked;
	default:
		return sdl::imtauth::Tenants::TenantConnectionStatus::Pending;
	}
}


sdl::imtauth::Tenants::CTenantConnectionRequest::V1_0 ConnectionRequestToData(const imtauth::TenantConnectionRequestInfo& info)
{
	sdl::imtauth::Tenants::CTenantConnectionRequest::V1_0 data;
	data.id = info.requestId;
	data.sourceTenantId = info.sourceTenantId;
	data.targetTenantId = info.targetTenantId;
	data.targetIdentifier = info.targetIdentifier;
	data.connectCode = info.connectCode;
	data.proposedSourceRole = ToSdlRelationshipRole(info.proposedSourceRole);
	data.proposedTargetRole = ToSdlRelationshipRole(info.proposedTargetRole);
	data.message = info.message;
	data.status = ToSdlConnectionStatus(info.status);
	data.createdAt = info.createdAt;
	data.expiresAt = info.expiresAt;
	data.respondedAt = info.respondedAt;
	return data;
}


} // anonymous namespace


// protected methods

// reimplemented (sdl::imtauth::Tenants::CGraphQlHandlerCompBase)

sdl::imtauth::Tenants::CGetTenantIdsPayload CTenantManagerControllerComp::OnGetTenantIds(
			const sdl::imtauth::Tenants::CGetTenantIdsGqlRequest& /*getTenantIdsRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CGetTenantIdsPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArrayList ids = m_tenantManagerCompPtr->GetTenantIds();

	response.Version_1_0->tenantIds.Emplace().FromList(ids);

	return response;
}


sdl::imtauth::Tenants::CGetTenantRelationshipsPayload CTenantManagerControllerComp::OnGetTenantRelationships(
			const sdl::imtauth::Tenants::CGetTenantRelationshipsGqlRequest& getTenantRelationshipsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CGetTenantRelationshipsPayload response;

	response.Version_1_0.emplace();

	if (!m_tenantManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Tenant manager is not configured");
		return response;
	}

	QByteArray tenantId;
	sdl::imtauth::Tenants::GetTenantRelationshipsRequestArguments arguments = getTenantRelationshipsRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}

	if (tenantId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}

	imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
	if (!tenantPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Tenant not found");
		return response;
	}

	response.Version_1_0->relationships.Emplace();

	const imtauth::ITenantInfo::TenantRelationships relationships = tenantPtr->GetRelationships();
	for (const imtauth::ITenantInfo::TenantRelationship& relationship : relationships){
		response.Version_1_0->relationships->push_back(RelationshipToData(tenantId, relationship));
	}

	return response;
}


sdl::imtauth::Tenants::CGetTenantPayload CTenantManagerControllerComp::OnGetTenant(
			const sdl::imtauth::Tenants::CGetTenantGqlRequest& getTenantRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CGetTenantPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray tenantId;
	sdl::imtauth::Tenants::GetTenantRequestArguments arguments = getTenantRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}

	imtauth::ITenantInfoUniquePtr tenantInfoPtr = m_tenantManagerCompPtr->GetTenant(tenantId);

	if (!tenantInfoPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Tenant not found");
		return response;
	}

	sdl::imtauth::Tenants::CTenantData::V1_0 tenantData;
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

	// Populate members (id + name) from TenantMemberships
	if (m_membershipManagerCompPtr.IsValid()){
		QByteArrayList membershipIds = m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId);
		tenantData.members.Emplace();
		for (const QByteArray& msId : std::as_const(membershipIds)){
			imtauth::ITenantMembershipUniquePtr msPtr = m_membershipManagerCompPtr->GetMembership(msId);
			if (msPtr.IsValid() && msPtr->IsActive()){
				QByteArray userId = msPtr->GetUserId();
				sdl::imtauth::Tenants::CTenantMemberEntry::V1_0 memberEntry;
				memberEntry.id = userId;

				if (m_userCollectionCompPtr.IsValid()){
					memberEntry.name = imtauth::GetUserName(*m_userCollectionCompPtr, userId);
				}

				tenantData.members->push_back(memberEntry);
			}
		}
	}

	response.Version_1_0->tenant = tenantData;

	return response;
}


sdl::imtauth::Tenants::CCreateTenantPayload CTenantManagerControllerComp::OnCreateTenant(
			const sdl::imtauth::Tenants::CCreateTenantGqlRequest& createTenantRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CCreateTenantPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QString name;
	QString description;
	QByteArray ownerId = GetUserId(gqlRequest);
	QByteArray parentTenantId;
	sdl::imtauth::Tenants::CreateTenantRequestArguments arguments = createTenantRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->name){
		name = *arguments.input.Version_1_0->name;
	}
	if (arguments.input.Version_1_0->description){
		description = *arguments.input.Version_1_0->description;
	}
	if (arguments.input.Version_1_0->ownerId){
		ownerId = *arguments.input.Version_1_0->ownerId;
	}
	if (arguments.input.Version_1_0->parentTenantId){
		parentTenantId = *arguments.input.Version_1_0->parentTenantId;
	}

	QByteArray tenantId = m_tenantManagerCompPtr->CreateTenant(name, description, ownerId);

	if (tenantId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to create tenant");
		return response;
	}

	// Set hierarchy fields if parentTenantId was provided
	if (!parentTenantId.isEmpty()){
		m_tenantManagerCompPtr->SetTenantHierarchy(tenantId, parentTenantId);
	}

	if (m_membershipManagerCompPtr.IsValid() && !ownerId.isEmpty()){
		m_membershipManagerCompPtr->AddMembership(ownerId, tenantId, QByteArray());
	}

	response.Version_1_0->tenantId = tenantId;

	return response;
}


sdl::imtauth::Tenants::CRemoveTenantPayload CTenantManagerControllerComp::OnRemoveTenant(
	const sdl::imtauth::Tenants::CRemoveTenantGqlRequest& removeTenantRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CRemoveTenantPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray tenantId;
	sdl::imtauth::Tenants::RemoveTenantRequestArguments arguments = removeTenantRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}

	bool success = m_tenantManagerCompPtr->RemoveTenant(tenantId);

	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to remove tenant");
	}

	return response;
}


sdl::imtauth::Tenants::CUpdateTenantPayload CTenantManagerControllerComp::OnUpdateTenant(
	const sdl::imtauth::Tenants::CUpdateTenantGqlRequest& updateTenantRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CUpdateTenantPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray tenantId;
	QString name;
	QString description;
	QByteArray ownerId;
	sdl::imtauth::Tenants::UpdateTenantRequestArguments arguments = updateTenantRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}
	if (arguments.input.Version_1_0->name){
		name = *arguments.input.Version_1_0->name;
	}
	if (arguments.input.Version_1_0->description){
		description = *arguments.input.Version_1_0->description;
	}
	if (arguments.input.Version_1_0->ownerId){
		ownerId = *arguments.input.Version_1_0->ownerId;
	}

	bool success = m_tenantManagerCompPtr->UpdateTenant(tenantId, name, description, ownerId, bool(arguments.input.Version_1_0->ownerId));

	// Sync members with TenantMemberships
	if (success && m_membershipManagerCompPtr.IsValid() && arguments.input.Version_1_0->members){
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
		for (const auto& member : *arguments.input.Version_1_0->members){
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
				m_membershipManagerCompPtr->AddMembership(uid, tenantId, QByteArray());
			}
		}
	}

	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to update tenant");
	}

	return response;
}


sdl::imtauth::Tenants::CSetTenantActivePayload CTenantManagerControllerComp::OnSetTenantActive(
	const sdl::imtauth::Tenants::CSetTenantActiveGqlRequest& setTenantActiveRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CSetTenantActivePayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	response.Version_1_0.emplace();

	QByteArray tenantId;
	bool isActive = false;
	sdl::imtauth::Tenants::SetTenantActiveRequestArguments arguments = setTenantActiveRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}
	if (arguments.input.Version_1_0->isActive){
		isActive = *arguments.input.Version_1_0->isActive;
	}

	bool success = m_tenantManagerCompPtr->SetTenantActive(tenantId, isActive);

	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to set tenant active state");
	}

	return response;
}


sdl::imtauth::Tenants::CAddTenantRelationshipPayload CTenantManagerControllerComp::OnAddTenantRelationship(
			const sdl::imtauth::Tenants::CAddTenantRelationshipGqlRequest& addTenantRelationshipRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CAddTenantRelationshipPayload response;

	response.Version_1_0.emplace();

	if (!m_tenantManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Tenant manager is not configured");
		return response;
	}

	QByteArray sourceTenantId;
	QByteArray targetTenantId;
	imtauth::ITenantInfo::TenantRelationshipRole role = imtauth::ITenantInfo::Partner;
	imtauth::ITenantInfo::TenantRelationshipRole sourceRole = imtauth::ITenantInfo::Partner;
	imtauth::ITenantInfo::TenantRelationshipRole targetRole = imtauth::ITenantInfo::Partner;
	QString scope;
	QString validFrom;
	QString validUntil;
	QString description;

	sdl::imtauth::Tenants::AddTenantRelationshipRequestArguments arguments = addTenantRelationshipRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->sourceTenantId){
		sourceTenantId = *arguments.input.Version_1_0->sourceTenantId;
	}
	if (arguments.input.Version_1_0->targetTenantId){
		targetTenantId = *arguments.input.Version_1_0->targetTenantId;
	}
	if (arguments.input.Version_1_0->role){
		role = FromSdlRelationshipRole(*arguments.input.Version_1_0->role);
		targetRole = role;
	}
	if (arguments.input.Version_1_0->sourceRole){
		sourceRole = FromSdlRelationshipRole(*arguments.input.Version_1_0->sourceRole);
	}
	if (arguments.input.Version_1_0->targetRole){
		targetRole = FromSdlRelationshipRole(*arguments.input.Version_1_0->targetRole);
	}
	if (arguments.input.Version_1_0->scope){
		scope = *arguments.input.Version_1_0->scope;
	}
	if (arguments.input.Version_1_0->validFrom){
		validFrom = *arguments.input.Version_1_0->validFrom;
	}
	if (arguments.input.Version_1_0->validUntil){
		validUntil = *arguments.input.Version_1_0->validUntil;
	}
	if (arguments.input.Version_1_0->description){
		description = *arguments.input.Version_1_0->description;
	}

	QByteArray relationshipId = m_tenantManagerCompPtr->AddTenantRelationship(
				sourceTenantId,
				targetTenantId,
				role,
				sourceRole,
				targetRole,
				scope,
				validFrom,
				validUntil,
				description);

	if (relationshipId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to add tenant relationship");
		return response;
	}

	response.Version_1_0->relationshipId = relationshipId;

	return response;
}


sdl::imtauth::Tenants::CRemoveTenantRelationshipPayload CTenantManagerControllerComp::OnRemoveTenantRelationship(
			const sdl::imtauth::Tenants::CRemoveTenantRelationshipGqlRequest& removeTenantRelationshipRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CRemoveTenantRelationshipPayload response;

	response.Version_1_0.emplace();

	if (!m_tenantManagerCompPtr.IsValid()){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Tenant manager is not configured");
		return response;
	}

	QByteArray tenantId;
	QByteArray relationshipId;
	sdl::imtauth::Tenants::RemoveTenantRelationshipRequestArguments arguments = removeTenantRelationshipRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}
	if (arguments.input.Version_1_0->relationshipId){
		relationshipId = *arguments.input.Version_1_0->relationshipId;
	}

	bool success = m_tenantManagerCompPtr->RemoveTenantRelationship(tenantId, relationshipId);
	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to remove tenant relationship");
	}

	return response;
}


namespace
{


imtauth::CrossOrgAccessLevel FromSdlAccessLevel(sdl::imtauth::Tenants::CrossOrgAccessLevel level)
{
	switch (level){
	case sdl::imtauth::Tenants::CrossOrgAccessLevel::Read:
		return imtauth::COAL_READ;
	case sdl::imtauth::Tenants::CrossOrgAccessLevel::Write:
		return imtauth::COAL_WRITE;
	case sdl::imtauth::Tenants::CrossOrgAccessLevel::Admin:
		return imtauth::COAL_ADMIN;
	default:
		return imtauth::COAL_NONE;
	}
}


sdl::imtauth::Tenants::CrossOrgAccessLevel ToSdlAccessLevel(imtauth::CrossOrgAccessLevel level)
{
	switch (level){
	case imtauth::COAL_READ:
		return sdl::imtauth::Tenants::CrossOrgAccessLevel::Read;
	case imtauth::COAL_WRITE:
		return sdl::imtauth::Tenants::CrossOrgAccessLevel::Write;
	case imtauth::COAL_ADMIN:
		return sdl::imtauth::Tenants::CrossOrgAccessLevel::Admin;
	default:
		return sdl::imtauth::Tenants::CrossOrgAccessLevel::None;
	}
}


sdl::imtauth::Tenants::CCrossOrgGrant::V1_0 GrantInfoToData(const imtauth::CrossOrgGrantInfo& info)
{
	sdl::imtauth::Tenants::CCrossOrgGrant::V1_0 data;
	data.id = info.grantId;
	data.sourceTenantId = info.sourceTenantId;
	data.targetTenantId = info.targetTenantId;
	data.relationshipId = info.relationshipId;
	data.targetTeamId = info.targetTeamId;
	data.accessLevel = ToSdlAccessLevel(info.accessLevel);
	data.resourceScope = info.resourceScope;
	data.description = info.description;
	data.createdAt = info.createdAt;
	data.expiresAt = info.expiresAt;
	data.isActive = info.isActive;
	return data;
}


} // anonymous namespace


sdl::imtauth::Tenants::CGetCrossOrgGrantsPayload CTenantManagerControllerComp::OnGetCrossOrgGrants(
			const sdl::imtauth::Tenants::CGetCrossOrgGrantsGqlRequest& getCrossOrgGrantsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CGetCrossOrgGrantsPayload response;

	response.Version_1_0.emplace();

	if (!m_grantManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Cross-org grant manager is not configured");
		return response;
	}

	QByteArray tenantId;
	sdl::imtauth::Tenants::GetCrossOrgGrantsRequestArguments arguments = getCrossOrgGrantsRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}

	if (tenantId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}

	response.Version_1_0->grants.Emplace();

	QSet<QByteArray> seenIds;
	const imtauth::CrossOrgGrants outgoing = m_grantManagerCompPtr->GetGrantsBySourceTenant(tenantId);
	for (const imtauth::CrossOrgGrantInfo& info : outgoing){
		if (!seenIds.contains(info.grantId)){
			seenIds.insert(info.grantId);
			response.Version_1_0->grants->push_back(GrantInfoToData(info));
		}
	}

	const imtauth::CrossOrgGrants incoming = m_grantManagerCompPtr->GetGrantsByTargetTenant(tenantId);
	for (const imtauth::CrossOrgGrantInfo& info : incoming){
		if (!seenIds.contains(info.grantId)){
			seenIds.insert(info.grantId);
			response.Version_1_0->grants->push_back(GrantInfoToData(info));
		}
	}

	return response;
}


sdl::imtauth::Tenants::CCreateCrossOrgGrantPayload CTenantManagerControllerComp::OnCreateCrossOrgGrant(
			const sdl::imtauth::Tenants::CCreateCrossOrgGrantGqlRequest& createCrossOrgGrantRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CCreateCrossOrgGrantPayload response;

	response.Version_1_0.emplace();

	if (!m_grantManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Cross-org grant manager is not configured");
		return response;
	}

	QByteArray sourceTenantId;
	QByteArray targetTenantId;
	QByteArray relationshipId;
	QByteArray targetTeamId;
	imtauth::CrossOrgAccessLevel accessLevel = imtauth::COAL_NONE;
	QString resourceScope;
	QString description;
	QString expiresAt;

	sdl::imtauth::Tenants::CreateCrossOrgGrantRequestArguments arguments = createCrossOrgGrantRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->sourceTenantId){
		sourceTenantId = *arguments.input.Version_1_0->sourceTenantId;
	}
	if (arguments.input.Version_1_0->targetTenantId){
		targetTenantId = *arguments.input.Version_1_0->targetTenantId;
	}
	if (arguments.input.Version_1_0->relationshipId){
		relationshipId = *arguments.input.Version_1_0->relationshipId;
	}
	if (arguments.input.Version_1_0->accessLevel){
		accessLevel = FromSdlAccessLevel(*arguments.input.Version_1_0->accessLevel);
	}
	if (arguments.input.Version_1_0->resourceScope){
		resourceScope = *arguments.input.Version_1_0->resourceScope;
	}
	if (arguments.input.Version_1_0->targetTeamId){
		targetTeamId = *arguments.input.Version_1_0->targetTeamId;
	}
	if (arguments.input.Version_1_0->description){
		description = *arguments.input.Version_1_0->description;
	}
	if (arguments.input.Version_1_0->expiresAt){
		expiresAt = *arguments.input.Version_1_0->expiresAt;
	}

	QByteArray grantId = m_grantManagerCompPtr->CreateGrant(
				sourceTenantId,
				targetTenantId,
				relationshipId,
				accessLevel,
				resourceScope,
				targetTeamId,
				description,
				expiresAt);

	if (grantId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to create cross-org grant");
		return response;
	}

	response.Version_1_0->grantId = grantId;

	return response;
}


sdl::imtauth::Tenants::CRevokeCrossOrgGrantPayload CTenantManagerControllerComp::OnRevokeCrossOrgGrant(
			const sdl::imtauth::Tenants::CRevokeCrossOrgGrantGqlRequest& revokeCrossOrgGrantRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CRevokeCrossOrgGrantPayload response;

	response.Version_1_0.emplace();

	if (!m_grantManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Cross-org grant manager is not configured");
		response.Version_1_0->success = false;
		return response;
	}

	QByteArray grantId;
	sdl::imtauth::Tenants::RevokeCrossOrgGrantRequestArguments arguments = revokeCrossOrgGrantRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->grantId){
		grantId = *arguments.input.Version_1_0->grantId;
	}

	bool success = m_grantManagerCompPtr->RevokeGrant(grantId);
	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to revoke cross-org grant");
	}

	return response;
}


sdl::imtauth::Tenants::CEnsureSystemTenantPayload CTenantManagerControllerComp::OnEnsureSystemTenant(
			const sdl::imtauth::Tenants::CEnsureSystemTenantGqlRequest& /*ensureSystemTenantRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CEnsureSystemTenantPayload response;

	response.Version_1_0.emplace();

	if (!m_tenantManagerCompPtr.IsValid()){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Tenant manager is not configured");
		return response;
	}

	bool success = m_tenantManagerCompPtr->EnsureSystemTenant();
	response.Version_1_0->success = success;
	response.Version_1_0->systemTenantId = m_tenantManagerCompPtr->GetSystemTenantId();
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to ensure System-Tenant");
	}

	return response;
}


sdl::imtauth::Tenants::CGetTenantConnectionRequestsPayload CTenantManagerControllerComp::OnGetTenantConnectionRequests(
			const sdl::imtauth::Tenants::CGetTenantConnectionRequestsGqlRequest& getTenantConnectionRequestsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CGetTenantConnectionRequestsPayload response;

	response.Version_1_0.emplace();

	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Tenant connection request manager is not configured");
		return response;
	}

	QByteArray tenantId;
	sdl::imtauth::Tenants::GetTenantConnectionRequestsRequestArguments arguments = getTenantConnectionRequestsRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}

	if (tenantId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}

	response.Version_1_0->requests.Emplace();

	QSet<QByteArray> seenIds;
	const imtauth::TenantConnectionRequests outgoing = m_connectionRequestManagerCompPtr->GetOutgoingRequests(tenantId);
	for (const imtauth::TenantConnectionRequestInfo& info : outgoing){
		if (!seenIds.contains(info.requestId)){
			seenIds.insert(info.requestId);
			response.Version_1_0->requests->push_back(ConnectionRequestToData(info));
		}
	}

	const imtauth::TenantConnectionRequests incoming = m_connectionRequestManagerCompPtr->GetIncomingRequests(tenantId);
	for (const imtauth::TenantConnectionRequestInfo& info : incoming){
		if (!seenIds.contains(info.requestId)){
			seenIds.insert(info.requestId);
			response.Version_1_0->requests->push_back(ConnectionRequestToData(info));
		}
	}

	return response;
}


sdl::imtauth::Tenants::CCreateTenantConnectionRequestPayload CTenantManagerControllerComp::OnCreateTenantConnectionRequest(
			const sdl::imtauth::Tenants::CCreateTenantConnectionRequestGqlRequest& createTenantConnectionRequestRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CCreateTenantConnectionRequestPayload response;

	response.Version_1_0.emplace();

	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Tenant connection request manager is not configured");
		return response;
	}

	QByteArray sourceTenantId;
	QString targetIdentifier;
	imtauth::ITenantInfo::TenantRelationshipRole proposedSourceRole = imtauth::ITenantInfo::Partner;
	imtauth::ITenantInfo::TenantRelationshipRole proposedTargetRole = imtauth::ITenantInfo::Partner;
	QString message;
	QString expiresAt;

	sdl::imtauth::Tenants::CreateTenantConnectionRequestRequestArguments arguments = createTenantConnectionRequestRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->sourceTenantId){
		sourceTenantId = *arguments.input.Version_1_0->sourceTenantId;
	}
	if (arguments.input.Version_1_0->targetIdentifier){
		targetIdentifier = *arguments.input.Version_1_0->targetIdentifier;
	}
	if (arguments.input.Version_1_0->proposedSourceRole){
		proposedSourceRole = FromSdlRelationshipRole(*arguments.input.Version_1_0->proposedSourceRole);
	}
	if (arguments.input.Version_1_0->proposedTargetRole){
		proposedTargetRole = FromSdlRelationshipRole(*arguments.input.Version_1_0->proposedTargetRole);
	}
	if (arguments.input.Version_1_0->message){
		message = *arguments.input.Version_1_0->message;
	}
	if (arguments.input.Version_1_0->expiresAt){
		expiresAt = *arguments.input.Version_1_0->expiresAt;
	}

	QByteArray requestId = m_connectionRequestManagerCompPtr->CreateConnectionRequest(
				sourceTenantId,
				targetIdentifier,
				proposedSourceRole,
				proposedTargetRole,
				message,
				expiresAt);

	if (requestId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to create tenant connection request");
		return response;
	}

	response.Version_1_0->requestId = requestId;

	return response;
}


sdl::imtauth::Tenants::CCreateTenantConnectCodePayload CTenantManagerControllerComp::OnCreateTenantConnectCode(
			const sdl::imtauth::Tenants::CCreateTenantConnectCodeGqlRequest& createTenantConnectCodeRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CCreateTenantConnectCodePayload response;

	response.Version_1_0.emplace();

	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Tenant connection request manager is not configured");
		return response;
	}

	QByteArray sourceTenantId;
	imtauth::ITenantInfo::TenantRelationshipRole proposedSourceRole = imtauth::ITenantInfo::Partner;
	imtauth::ITenantInfo::TenantRelationshipRole proposedTargetRole = imtauth::ITenantInfo::Partner;
	QString message;
	QString expiresAt;

	sdl::imtauth::Tenants::CreateTenantConnectCodeRequestArguments arguments = createTenantConnectCodeRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->sourceTenantId){
		sourceTenantId = *arguments.input.Version_1_0->sourceTenantId;
	}
	if (arguments.input.Version_1_0->proposedSourceRole){
		proposedSourceRole = FromSdlRelationshipRole(*arguments.input.Version_1_0->proposedSourceRole);
	}
	if (arguments.input.Version_1_0->proposedTargetRole){
		proposedTargetRole = FromSdlRelationshipRole(*arguments.input.Version_1_0->proposedTargetRole);
	}
	if (arguments.input.Version_1_0->message){
		message = *arguments.input.Version_1_0->message;
	}
	if (arguments.input.Version_1_0->expiresAt){
		expiresAt = *arguments.input.Version_1_0->expiresAt;
	}

	QString generatedCode;
	QByteArray requestId = m_connectionRequestManagerCompPtr->CreateConnectCode(
				sourceTenantId,
				proposedSourceRole,
				proposedTargetRole,
				message,
				expiresAt,
				generatedCode);

	if (requestId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to create tenant connect code");
		return response;
	}

	response.Version_1_0->requestId = requestId;
	response.Version_1_0->connectCode = generatedCode;

	return response;
}


sdl::imtauth::Tenants::CAcceptTenantConnectionRequestPayload CTenantManagerControllerComp::OnAcceptTenantConnectionRequest(
			const sdl::imtauth::Tenants::CAcceptTenantConnectionRequestGqlRequest& acceptTenantConnectionRequestRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CAcceptTenantConnectionRequestPayload response;

	response.Version_1_0.emplace();

	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Tenant connection request manager is not configured");
		return response;
	}

	QByteArray requestId;
	QByteArray acceptingTenantId;
	sdl::imtauth::Tenants::AcceptTenantConnectionRequestRequestArguments arguments = acceptTenantConnectionRequestRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->requestId){
		requestId = *arguments.input.Version_1_0->requestId;
	}
	if (arguments.input.Version_1_0->acceptingTenantId){
		acceptingTenantId = *arguments.input.Version_1_0->acceptingTenantId;
	}

	bool success = m_connectionRequestManagerCompPtr->AcceptConnectionRequest(requestId, acceptingTenantId);
	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to accept tenant connection request");
	}

	return response;
}


sdl::imtauth::Tenants::CAcceptTenantConnectCodePayload CTenantManagerControllerComp::OnAcceptTenantConnectCode(
			const sdl::imtauth::Tenants::CAcceptTenantConnectCodeGqlRequest& acceptTenantConnectCodeRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CAcceptTenantConnectCodePayload response;

	response.Version_1_0.emplace();

	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Tenant connection request manager is not configured");
		return response;
	}

	QString connectCode;
	QByteArray acceptingTenantId;
	sdl::imtauth::Tenants::AcceptTenantConnectCodeRequestArguments arguments = acceptTenantConnectCodeRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->connectCode){
		connectCode = *arguments.input.Version_1_0->connectCode;
	}
	if (arguments.input.Version_1_0->acceptingTenantId){
		acceptingTenantId = *arguments.input.Version_1_0->acceptingTenantId;
	}

	QByteArray requestId = m_connectionRequestManagerCompPtr->AcceptConnectCode(connectCode, acceptingTenantId);
	if (requestId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to accept tenant connect code");
		return response;
	}

	response.Version_1_0->requestId = requestId;

	return response;
}


sdl::imtauth::Tenants::CRejectTenantConnectionRequestPayload CTenantManagerControllerComp::OnRejectTenantConnectionRequest(
			const sdl::imtauth::Tenants::CRejectTenantConnectionRequestGqlRequest& rejectTenantConnectionRequestRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CRejectTenantConnectionRequestPayload response;

	response.Version_1_0.emplace();

	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Tenant connection request manager is not configured");
		return response;
	}

	QByteArray requestId;
	sdl::imtauth::Tenants::RejectTenantConnectionRequestRequestArguments arguments = rejectTenantConnectionRequestRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->requestId){
		requestId = *arguments.input.Version_1_0->requestId;
	}

	bool success = m_connectionRequestManagerCompPtr->RejectConnectionRequest(requestId);
	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to reject tenant connection request");
	}

	return response;
}


sdl::imtauth::Tenants::CRevokeTenantConnectionRequestPayload CTenantManagerControllerComp::OnRevokeTenantConnectionRequest(
			const sdl::imtauth::Tenants::CRevokeTenantConnectionRequestGqlRequest& revokeTenantConnectionRequestRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CRevokeTenantConnectionRequestPayload response;

	response.Version_1_0.emplace();

	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Tenant connection request manager is not configured");
		return response;
	}

	QByteArray requestId;
	sdl::imtauth::Tenants::RevokeTenantConnectionRequestRequestArguments arguments = revokeTenantConnectionRequestRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->requestId){
		requestId = *arguments.input.Version_1_0->requestId;
	}

	bool success = m_connectionRequestManagerCompPtr->RevokeConnectionRequest(requestId);
	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to revoke tenant connection request");
	}

	return response;
}


namespace
{


sdl::imtauth::Tenants::CrossTenantMessageType ToSdlMessageType(imtauth::CrossTenantMessageType type)
{
	switch (type){
	case imtauth::CTMT_ORDER_REQUEST:
		return sdl::imtauth::Tenants::CrossTenantMessageType::OrderRequest;
	case imtauth::CTMT_ORDER_CONFIRMATION:
		return sdl::imtauth::Tenants::CrossTenantMessageType::OrderConfirmation;
	case imtauth::CTMT_ORDER_REJECTION:
		return sdl::imtauth::Tenants::CrossTenantMessageType::OrderRejection;
	case imtauth::CTMT_ORDER_STATUS_UPDATE:
		return sdl::imtauth::Tenants::CrossTenantMessageType::OrderStatusUpdate;
	case imtauth::CTMT_ORDER_CANCELLATION:
		return sdl::imtauth::Tenants::CrossTenantMessageType::OrderCancellation;
	case imtauth::CTMT_DOCUMENT_SHARE:
		return sdl::imtauth::Tenants::CrossTenantMessageType::DocumentShare;
	default:
		return sdl::imtauth::Tenants::CrossTenantMessageType::Custom;
	}
}


imtauth::CrossTenantMessageType FromSdlMessageType(sdl::imtauth::Tenants::CrossTenantMessageType type)
{
	switch (type){
	case sdl::imtauth::Tenants::CrossTenantMessageType::OrderRequest:
		return imtauth::CTMT_ORDER_REQUEST;
	case sdl::imtauth::Tenants::CrossTenantMessageType::OrderConfirmation:
		return imtauth::CTMT_ORDER_CONFIRMATION;
	case sdl::imtauth::Tenants::CrossTenantMessageType::OrderRejection:
		return imtauth::CTMT_ORDER_REJECTION;
	case sdl::imtauth::Tenants::CrossTenantMessageType::OrderStatusUpdate:
		return imtauth::CTMT_ORDER_STATUS_UPDATE;
	case sdl::imtauth::Tenants::CrossTenantMessageType::OrderCancellation:
		return imtauth::CTMT_ORDER_CANCELLATION;
	case sdl::imtauth::Tenants::CrossTenantMessageType::DocumentShare:
		return imtauth::CTMT_DOCUMENT_SHARE;
	default:
		return imtauth::CTMT_CUSTOM;
	}
}


sdl::imtauth::Tenants::CrossTenantMessageStatus ToSdlMessageStatus(imtauth::CrossTenantMessageStatus status)
{
	switch (status){
	case imtauth::CTMS_VALIDATED:
		return sdl::imtauth::Tenants::CrossTenantMessageStatus::Validated;
	case imtauth::CTMS_DELIVERED:
		return sdl::imtauth::Tenants::CrossTenantMessageStatus::Delivered;
	case imtauth::CTMS_ACKNOWLEDGED:
		return sdl::imtauth::Tenants::CrossTenantMessageStatus::Acknowledged;
	case imtauth::CTMS_PROCESSED:
		return sdl::imtauth::Tenants::CrossTenantMessageStatus::Processed;
	case imtauth::CTMS_FAILED:
		return sdl::imtauth::Tenants::CrossTenantMessageStatus::Failed;
	case imtauth::CTMS_EXPIRED:
		return sdl::imtauth::Tenants::CrossTenantMessageStatus::Expired;
	default:
		return sdl::imtauth::Tenants::CrossTenantMessageStatus::Created;
	}
}


imtauth::CrossTenantMessageStatus FromSdlMessageStatus(sdl::imtauth::Tenants::CrossTenantMessageStatus status)
{
	switch (status){
	case sdl::imtauth::Tenants::CrossTenantMessageStatus::Validated:
		return imtauth::CTMS_VALIDATED;
	case sdl::imtauth::Tenants::CrossTenantMessageStatus::Delivered:
		return imtauth::CTMS_DELIVERED;
	case sdl::imtauth::Tenants::CrossTenantMessageStatus::Acknowledged:
		return imtauth::CTMS_ACKNOWLEDGED;
	case sdl::imtauth::Tenants::CrossTenantMessageStatus::Processed:
		return imtauth::CTMS_PROCESSED;
	case sdl::imtauth::Tenants::CrossTenantMessageStatus::Failed:
		return imtauth::CTMS_FAILED;
	case sdl::imtauth::Tenants::CrossTenantMessageStatus::Expired:
		return imtauth::CTMS_EXPIRED;
	default:
		return imtauth::CTMS_CREATED;
	}
}


sdl::imtauth::Tenants::CCrossTenantMessage::V1_0 MessageInfoToData(const imtauth::CrossTenantMessageInfo& info)
{
	sdl::imtauth::Tenants::CCrossTenantMessage::V1_0 data;
	data.id = info.messageId;
	data.sourceTenantId = info.sourceTenantId;
	data.targetTenantId = info.targetTenantId;
	data.relationshipId = info.relationshipId;
	data.sourceObjectId = info.sourceObjectId;
	data.targetObjectId = info.targetObjectId;
	data.messageType = ToSdlMessageType(info.messageType);
	data.customType = info.customType;
	data.payload = QString::fromUtf8(info.payload);
	data.status = ToSdlMessageStatus(info.status);
	data.errorMessage = info.errorMessage;
	data.createdAt = info.createdAt;
	data.updatedAt = info.updatedAt;
	data.expiresAt = info.expiresAt;
	return data;
}


sdl::imtauth::Tenants::OrderRequestStatus ToSdlOrderStatus(imtauth::OrderRequestStatus status)
{
	switch (status){
	case imtauth::ORS_CONFIRMED:
		return sdl::imtauth::Tenants::OrderRequestStatus::Confirmed;
	case imtauth::ORS_REJECTED:
		return sdl::imtauth::Tenants::OrderRequestStatus::Rejected;
	case imtauth::ORS_IN_PROGRESS:
		return sdl::imtauth::Tenants::OrderRequestStatus::InProgress;
	case imtauth::ORS_COMPLETED:
		return sdl::imtauth::Tenants::OrderRequestStatus::Completed;
	case imtauth::ORS_CANCELLED:
		return sdl::imtauth::Tenants::OrderRequestStatus::Cancelled;
	default:
		return sdl::imtauth::Tenants::OrderRequestStatus::Received;
	}
}


imtauth::OrderRequestStatus FromSdlOrderStatus(sdl::imtauth::Tenants::OrderRequestStatus status)
{
	switch (status){
	case sdl::imtauth::Tenants::OrderRequestStatus::Confirmed:
		return imtauth::ORS_CONFIRMED;
	case sdl::imtauth::Tenants::OrderRequestStatus::Rejected:
		return imtauth::ORS_REJECTED;
	case sdl::imtauth::Tenants::OrderRequestStatus::InProgress:
		return imtauth::ORS_IN_PROGRESS;
	case sdl::imtauth::Tenants::OrderRequestStatus::Completed:
		return imtauth::ORS_COMPLETED;
	case sdl::imtauth::Tenants::OrderRequestStatus::Cancelled:
		return imtauth::ORS_CANCELLED;
	default:
		return imtauth::ORS_RECEIVED;
	}
}


sdl::imtauth::Tenants::COrderRequest::V1_0 OrderRequestToData(const imtauth::OrderRequestInfo& info)
{
	sdl::imtauth::Tenants::COrderRequest::V1_0 data;
	data.id = info.orderRequestId;
	data.messageId = info.messageId;
	data.sourceTenantId = info.sourceTenantId;
	data.targetTenantId = info.targetTenantId;
	data.relationshipId = info.relationshipId;
	data.sourceOrderId = info.sourceOrderId;
	data.articleNumber = info.articleNumber;
	data.quantity = info.quantity;
	data.note = info.note;
	data.status = ToSdlOrderStatus(info.status);
	data.statusNote = info.statusNote;
	data.createdAt = info.createdAt;
	data.updatedAt = info.updatedAt;
	return data;
}


} // anonymous namespace


sdl::imtauth::Tenants::CGetCrossTenantMessagePayload CTenantManagerControllerComp::OnGetCrossTenantMessage(
			const sdl::imtauth::Tenants::CGetCrossTenantMessageGqlRequest& getCrossTenantMessageRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CGetCrossTenantMessagePayload response;

	response.Version_1_0.emplace();

	if (!m_messageBrokerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Cross-tenant message broker is not configured");
		return response;
	}

	QByteArray messageId;
	sdl::imtauth::Tenants::GetCrossTenantMessageRequestArguments arguments = getCrossTenantMessageRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->messageId){
		messageId = *arguments.input.Version_1_0->messageId;
	}

	if (messageId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Message ID is required");
		return response;
	}

	imtauth::CrossTenantMessageInfo info = m_messageBrokerCompPtr->GetMessage(messageId);
	if (info.messageId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Message not found");
		return response;
	}

	response.Version_1_0->message = MessageInfoToData(info);

	return response;
}


sdl::imtauth::Tenants::CGetCrossTenantMessagesPayload CTenantManagerControllerComp::OnGetCrossTenantMessages(
			const sdl::imtauth::Tenants::CGetCrossTenantMessagesGqlRequest& getCrossTenantMessagesRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CGetCrossTenantMessagesPayload response;

	response.Version_1_0.emplace();

	if (!m_messageBrokerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Cross-tenant message broker is not configured");
		return response;
	}

	QByteArray tenantId;
	bool incomingOnly = false;
	bool outgoingOnly = false;
	sdl::imtauth::Tenants::GetCrossTenantMessagesRequestArguments arguments = getCrossTenantMessagesRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}
	if (arguments.input.Version_1_0->direction){
		if (*arguments.input.Version_1_0->direction == sdl::imtauth::Tenants::CrossTenantMessageDirection::Incoming){
			incomingOnly = true;
		}
		else if (*arguments.input.Version_1_0->direction == sdl::imtauth::Tenants::CrossTenantMessageDirection::Outgoing){
			outgoingOnly = true;
		}
	}

	if (tenantId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}

	response.Version_1_0->messages.Emplace();

	QSet<QByteArray> seenIds;
	if (!outgoingOnly){
		const imtauth::CrossTenantMessages incoming = m_messageBrokerCompPtr->GetIncomingMessages(tenantId);
		for (const imtauth::CrossTenantMessageInfo& info : incoming){
			if (!seenIds.contains(info.messageId)){
				seenIds.insert(info.messageId);
				response.Version_1_0->messages->push_back(MessageInfoToData(info));
			}
		}
	}
	if (!incomingOnly){
		const imtauth::CrossTenantMessages outgoing = m_messageBrokerCompPtr->GetOutgoingMessages(tenantId);
		for (const imtauth::CrossTenantMessageInfo& info : outgoing){
			if (!seenIds.contains(info.messageId)){
				seenIds.insert(info.messageId);
				response.Version_1_0->messages->push_back(MessageInfoToData(info));
			}
		}
	}

	return response;
}


sdl::imtauth::Tenants::CSendCrossTenantMessagePayload CTenantManagerControllerComp::OnSendCrossTenantMessage(
			const sdl::imtauth::Tenants::CSendCrossTenantMessageGqlRequest& sendCrossTenantMessageRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CSendCrossTenantMessagePayload response;

	response.Version_1_0.emplace();

	if (!m_messageBrokerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Cross-tenant message broker is not configured");
		return response;
	}

	QByteArray sourceTenantId;
	QByteArray targetTenantId;
	QByteArray relationshipId;
	imtauth::CrossTenantMessageType messageType = imtauth::CTMT_CUSTOM;
	QByteArray payload;
	QByteArray sourceObjectId;
	QString customType;
	QString expiresAt;

	sdl::imtauth::Tenants::SendCrossTenantMessageRequestArguments arguments = sendCrossTenantMessageRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->sourceTenantId){
		sourceTenantId = *arguments.input.Version_1_0->sourceTenantId;
	}
	if (arguments.input.Version_1_0->targetTenantId){
		targetTenantId = *arguments.input.Version_1_0->targetTenantId;
	}
	if (arguments.input.Version_1_0->relationshipId){
		relationshipId = *arguments.input.Version_1_0->relationshipId;
	}
	if (arguments.input.Version_1_0->messageType){
		messageType = FromSdlMessageType(*arguments.input.Version_1_0->messageType);
	}
	if (arguments.input.Version_1_0->payload){
		payload = (*arguments.input.Version_1_0->payload).toUtf8();
	}
	if (arguments.input.Version_1_0->sourceObjectId){
		sourceObjectId = *arguments.input.Version_1_0->sourceObjectId;
	}
	if (arguments.input.Version_1_0->customType){
		customType = *arguments.input.Version_1_0->customType;
	}
	if (arguments.input.Version_1_0->expiresAt){
		expiresAt = *arguments.input.Version_1_0->expiresAt;
	}

	QByteArray messageId = m_messageBrokerCompPtr->SendMessage(
				sourceTenantId,
				targetTenantId,
				relationshipId,
				messageType,
				payload,
				sourceObjectId,
				customType,
				expiresAt);

	if (messageId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to send cross-tenant message");
		return response;
	}

	response.Version_1_0->messageId = messageId;

	return response;
}


sdl::imtauth::Tenants::CUpdateCrossTenantMessageStatusPayload CTenantManagerControllerComp::OnUpdateCrossTenantMessageStatus(
			const sdl::imtauth::Tenants::CUpdateCrossTenantMessageStatusGqlRequest& updateCrossTenantMessageStatusRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CUpdateCrossTenantMessageStatusPayload response;

	response.Version_1_0.emplace();

	if (!m_messageBrokerCompPtr.IsValid()){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Cross-tenant message broker is not configured");
		return response;
	}

	QByteArray messageId;
	imtauth::CrossTenantMessageStatus status = imtauth::CTMS_CREATED;
	QString statusErrorMessage;

	sdl::imtauth::Tenants::UpdateCrossTenantMessageStatusRequestArguments arguments = updateCrossTenantMessageStatusRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->messageId){
		messageId = *arguments.input.Version_1_0->messageId;
	}
	if (arguments.input.Version_1_0->status){
		status = FromSdlMessageStatus(*arguments.input.Version_1_0->status);
	}
	if (arguments.input.Version_1_0->errorMessage){
		statusErrorMessage = *arguments.input.Version_1_0->errorMessage;
	}

	bool success = m_messageBrokerCompPtr->UpdateMessageStatus(messageId, status, statusErrorMessage);
	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to update cross-tenant message status");
	}

	return response;
}


sdl::imtauth::Tenants::CGetOrderRequestPayload CTenantManagerControllerComp::OnGetOrderRequest(
			const sdl::imtauth::Tenants::CGetOrderRequestGqlRequest& getOrderRequestRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CGetOrderRequestPayload response;

	response.Version_1_0.emplace();

	if (!m_orderRequestManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Order request manager is not configured");
		return response;
	}

	QByteArray orderRequestId;
	sdl::imtauth::Tenants::GetOrderRequestRequestArguments arguments = getOrderRequestRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->orderRequestId){
		orderRequestId = *arguments.input.Version_1_0->orderRequestId;
	}

	if (orderRequestId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Order request ID is required");
		return response;
	}

	imtauth::OrderRequestInfo info = m_orderRequestManagerCompPtr->GetOrderRequest(orderRequestId);
	if (info.orderRequestId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Order request not found");
		return response;
	}

	response.Version_1_0->orderRequest = OrderRequestToData(info);

	return response;
}


sdl::imtauth::Tenants::CGetOrderRequestsPayload CTenantManagerControllerComp::OnGetOrderRequests(
			const sdl::imtauth::Tenants::CGetOrderRequestsGqlRequest& getOrderRequestsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CGetOrderRequestsPayload response;

	response.Version_1_0.emplace();

	if (!m_orderRequestManagerCompPtr.IsValid()){
		response.Version_1_0->errorMessage = QStringLiteral("Order request manager is not configured");
		return response;
	}

	QByteArray tenantId;
	sdl::imtauth::Tenants::GetOrderRequestsRequestArguments arguments = getOrderRequestsRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->tenantId){
		tenantId = *arguments.input.Version_1_0->tenantId;
	}

	if (tenantId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}

	response.Version_1_0->orderRequests.Emplace();

	const imtauth::OrderRequests orderRequests = m_orderRequestManagerCompPtr->GetOrderRequests(tenantId);
	for (const imtauth::OrderRequestInfo& info : orderRequests){
		response.Version_1_0->orderRequests->push_back(OrderRequestToData(info));
	}

	return response;
}


sdl::imtauth::Tenants::CConfirmOrderRequestPayload CTenantManagerControllerComp::OnConfirmOrderRequest(
			const sdl::imtauth::Tenants::CConfirmOrderRequestGqlRequest& confirmOrderRequestRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CConfirmOrderRequestPayload response;

	response.Version_1_0.emplace();

	if (!m_orderRequestManagerCompPtr.IsValid()){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Order request manager is not configured");
		return response;
	}

	QByteArray orderRequestId;
	QString note;
	sdl::imtauth::Tenants::ConfirmOrderRequestRequestArguments arguments = confirmOrderRequestRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->orderRequestId){
		orderRequestId = *arguments.input.Version_1_0->orderRequestId;
	}
	if (arguments.input.Version_1_0->note){
		note = *arguments.input.Version_1_0->note;
	}

	bool success = m_orderRequestManagerCompPtr->ConfirmOrderRequest(orderRequestId, note);
	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to confirm order request");
	}

	return response;
}


sdl::imtauth::Tenants::CRejectOrderRequestPayload CTenantManagerControllerComp::OnRejectOrderRequest(
			const sdl::imtauth::Tenants::CRejectOrderRequestGqlRequest& rejectOrderRequestRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CRejectOrderRequestPayload response;

	response.Version_1_0.emplace();

	if (!m_orderRequestManagerCompPtr.IsValid()){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Order request manager is not configured");
		return response;
	}

	QByteArray orderRequestId;
	QString reason;
	sdl::imtauth::Tenants::RejectOrderRequestRequestArguments arguments = rejectOrderRequestRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->orderRequestId){
		orderRequestId = *arguments.input.Version_1_0->orderRequestId;
	}
	if (arguments.input.Version_1_0->reason){
		reason = *arguments.input.Version_1_0->reason;
	}

	bool success = m_orderRequestManagerCompPtr->RejectOrderRequest(orderRequestId, reason);
	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to reject order request");
	}

	return response;
}


sdl::imtauth::Tenants::CUpdateOrderRequestStatusPayload CTenantManagerControllerComp::OnUpdateOrderRequestStatus(
			const sdl::imtauth::Tenants::CUpdateOrderRequestStatusGqlRequest& updateOrderRequestStatusRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CUpdateOrderRequestStatusPayload response;

	response.Version_1_0.emplace();

	if (!m_orderRequestManagerCompPtr.IsValid()){
		response.Version_1_0->success = false;
		response.Version_1_0->errorMessage = QStringLiteral("Order request manager is not configured");
		return response;
	}

	QByteArray orderRequestId;
	imtauth::OrderRequestStatus status = imtauth::ORS_RECEIVED;
	QString note;
	sdl::imtauth::Tenants::UpdateOrderRequestStatusRequestArguments arguments = updateOrderRequestStatusRequest.GetRequestedArguments();
	if (arguments.input.Version_1_0->orderRequestId){
		orderRequestId = *arguments.input.Version_1_0->orderRequestId;
	}
	if (arguments.input.Version_1_0->status){
		status = FromSdlOrderStatus(*arguments.input.Version_1_0->status);
	}
	if (arguments.input.Version_1_0->note){
		note = *arguments.input.Version_1_0->note;
	}

	bool success = m_orderRequestManagerCompPtr->UpdateOrderRequestStatus(orderRequestId, status, note);
	response.Version_1_0->success = success;
	if (!success){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to update order request status");
	}

	return response;
}


} // namespace imtauthgql