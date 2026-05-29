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


} // namespace imtauthgql
