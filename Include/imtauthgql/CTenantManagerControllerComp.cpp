// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantManagerControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


// Qt includes
#include <QSet>
#include <QMap>

// ImtCore includes
#include <imtauth/imtauth.h>


namespace imtauthgql
{


namespace
{


imtauth::ITenantInfo::TenantRelationshipRole FromSdlRelationshipRole(sdl::V1_0::imtauth::TenantRelationshipRole role)
{
	switch (role){
	case sdl::V1_0::imtauth::TenantRelationshipRole::Parent:
		return imtauth::ITenantInfo::Parent;
	case sdl::V1_0::imtauth::TenantRelationshipRole::Child:
		return imtauth::ITenantInfo::Child;
	case sdl::V1_0::imtauth::TenantRelationshipRole::Supplier:
		return imtauth::ITenantInfo::Supplier;
	case sdl::V1_0::imtauth::TenantRelationshipRole::Customer:
		return imtauth::ITenantInfo::Customer;
	case sdl::V1_0::imtauth::TenantRelationshipRole::Affiliate:
		return imtauth::ITenantInfo::Affiliate;
	default:
		return imtauth::ITenantInfo::Partner;
	}
}


sdl::V1_0::imtauth::TenantRelationshipRole ToSdlRelationshipRole(imtauth::ITenantInfo::TenantRelationshipRole role)
{
	switch (role){
	case imtauth::ITenantInfo::Parent:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Parent;
	case imtauth::ITenantInfo::Child:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Child;
	case imtauth::ITenantInfo::Supplier:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Supplier;
	case imtauth::ITenantInfo::Customer:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Customer;
	case imtauth::ITenantInfo::Affiliate:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Affiliate;
	default:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Partner;
	}
}


sdl::V1_0::imtauth::CTenantRelationship RelationshipToData(
		const QByteArray& sourceTenantId,
		const imtauth::ITenantInfo::TenantRelationship& relationship)
{
	sdl::V1_0::imtauth::CTenantRelationship data;
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


sdl::V1_0::imtauth::TenantConnectionStatus ToSdlConnectionStatus(imtauth::TenantConnectionStatus status)
{
	switch (status){
	case imtauth::TCS_ACCEPTED:
		return sdl::V1_0::imtauth::TenantConnectionStatus::Accepted;
	case imtauth::TCS_REJECTED:
		return sdl::V1_0::imtauth::TenantConnectionStatus::Rejected;
	case imtauth::TCS_EXPIRED:
		return sdl::V1_0::imtauth::TenantConnectionStatus::Expired;
	case imtauth::TCS_REVOKED:
		return sdl::V1_0::imtauth::TenantConnectionStatus::Revoked;
	default:
		return sdl::V1_0::imtauth::TenantConnectionStatus::Pending;
	}
}


sdl::V1_0::imtauth::CTenantConnectionRequest ConnectionRequestToData(const imtauth::TenantConnectionRequestInfo& info)
{
	sdl::V1_0::imtauth::CTenantConnectionRequest data;
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
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetTenantRelationshipsPayload response;


	if (!m_tenantManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant manager is not configured");
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

	imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
	if (!tenantPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant not found");
		return response;
	}

	response.relationships.Emplace();

	const imtauth::ITenantInfo::TenantRelationships relationships = tenantPtr->GetRelationships();
	for (const imtauth::ITenantInfo::TenantRelationship& relationship : relationships){
		response.relationships->push_back(RelationshipToData(tenantId, relationship));
	}

	return response;
}


sdl::V1_0::imtauth::CGetTenantPayload CTenantManagerControllerComp::OnGetTenant(
			const sdl::V1_0::imtauth::CGetTenantGqlRequest& getTenantRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
		m_membershipManagerCompPtr->AddMembership(ownerId, tenantId, QByteArray());
	}

	response.tenantId = tenantId;

	return response;
}


sdl::V1_0::imtauth::CRemoveTenantPayload CTenantManagerControllerComp::OnRemoveTenant(
	const sdl::V1_0::imtauth::CRemoveTenantGqlRequest& removeTenantRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
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

	bool success = m_tenantManagerCompPtr->RemoveTenant(tenantId);

	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to remove tenant");
	}

	return response;
}


sdl::V1_0::imtauth::CUpdateTenantPayload CTenantManagerControllerComp::OnUpdateTenant(
	const sdl::V1_0::imtauth::CUpdateTenantGqlRequest& updateTenantRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CUpdateTenantPayload response;

	if (!m_tenantManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'TenantManager' was not set", "CTenantManagerControllerComp");
		return response;
	}

	QByteArray tenantId;
	QString name;
	QString description;
	QByteArray ownerId;
	sdl::V1_0::imtauth::UpdateTenantRequestArguments arguments = updateTenantRequest.GetRequestedArguments();
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	if (arguments.input->name){
		name = *arguments.input->name;
	}
	if (arguments.input->description){
		description = *arguments.input->description;
	}
	if (arguments.input->ownerId){
		ownerId = *arguments.input->ownerId;
	}

	bool success = m_tenantManagerCompPtr->UpdateTenant(tenantId, name, description, ownerId, bool(arguments.input->ownerId));

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
				m_membershipManagerCompPtr->AddMembership(uid, tenantId, QByteArray());
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
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
	if (arguments.input->isActive){
		isActive = *arguments.input->isActive;
	}

	bool success = m_tenantManagerCompPtr->SetTenantActive(tenantId, isActive);

	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to set tenant active state");
	}

	return response;
}


sdl::V1_0::imtauth::CAddTenantRelationshipPayload CTenantManagerControllerComp::OnAddTenantRelationship(
			const sdl::V1_0::imtauth::CAddTenantRelationshipGqlRequest& addTenantRelationshipRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CAddTenantRelationshipPayload response;


	if (!m_tenantManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant manager is not configured");
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

	sdl::V1_0::imtauth::AddTenantRelationshipRequestArguments arguments = addTenantRelationshipRequest.GetRequestedArguments();
	if (arguments.input->sourceTenantId){
		sourceTenantId = *arguments.input->sourceTenantId;
	}
	if (arguments.input->targetTenantId){
		targetTenantId = *arguments.input->targetTenantId;
	}
	if (arguments.input->role){
		role = FromSdlRelationshipRole(*arguments.input->role);
		targetRole = role;
	}
	if (arguments.input->sourceRole){
		sourceRole = FromSdlRelationshipRole(*arguments.input->sourceRole);
	}
	if (arguments.input->targetRole){
		targetRole = FromSdlRelationshipRole(*arguments.input->targetRole);
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
		response.errorMessage = QStringLiteral("Failed to add tenant relationship");
		return response;
	}

	response.relationshipId = relationshipId;

	return response;
}


sdl::V1_0::imtauth::CRemoveTenantRelationshipPayload CTenantManagerControllerComp::OnRemoveTenantRelationship(
			const sdl::V1_0::imtauth::CRemoveTenantRelationshipGqlRequest& removeTenantRelationshipRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRemoveTenantRelationshipPayload response;


	if (!m_tenantManagerCompPtr.IsValid()){
		response.success = false;
		response.errorMessage = QStringLiteral("Tenant manager is not configured");
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

	bool success = m_tenantManagerCompPtr->RemoveTenantRelationship(tenantId, relationshipId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to remove tenant relationship");
	}

	return response;
}


namespace
{


imtauth::CrossOrgAccessLevel FromSdlAccessLevel(sdl::V1_0::imtauth::CrossOrgAccessLevel level)
{
	switch (level){
	case sdl::V1_0::imtauth::CrossOrgAccessLevel::Read:
		return imtauth::COAL_READ;
	case sdl::V1_0::imtauth::CrossOrgAccessLevel::Write:
		return imtauth::COAL_WRITE;
	case sdl::V1_0::imtauth::CrossOrgAccessLevel::Admin:
		return imtauth::COAL_ADMIN;
	default:
		return imtauth::COAL_NONE;
	}
}


sdl::V1_0::imtauth::CrossOrgAccessLevel ToSdlAccessLevel(imtauth::CrossOrgAccessLevel level)
{
	switch (level){
	case imtauth::COAL_READ:
		return sdl::V1_0::imtauth::CrossOrgAccessLevel::Read;
	case imtauth::COAL_WRITE:
		return sdl::V1_0::imtauth::CrossOrgAccessLevel::Write;
	case imtauth::COAL_ADMIN:
		return sdl::V1_0::imtauth::CrossOrgAccessLevel::Admin;
	default:
		return sdl::V1_0::imtauth::CrossOrgAccessLevel::None;
	}
}


sdl::V1_0::imtauth::CCrossOrgGrant GrantInfoToData(const imtauth::CrossOrgGrantInfo& info)
{
	sdl::V1_0::imtauth::CCrossOrgGrant data;
	data.id = info.grantId;
	data.sourceTenantId = info.sourceTenantId;
	data.targetTenantId = info.targetTenantId;
	data.relationshipId = info.relationshipId;
	data.contractId = info.contractId;
	data.targetTeamId = info.targetTeamId;
	data.accessLevel = ToSdlAccessLevel(info.accessLevel);
	data.resourceScope = info.resourceScope;
	data.description = info.description;
	data.createdAt = info.createdAt;
	data.expiresAt = info.expiresAt;
	data.isActive = info.isActive;
	return data;
}


imtauth::ContractStatus FromSdlContractStatus(sdl::V1_0::imtauth::ContractStatus status)
{
	switch (status){
	case sdl::V1_0::imtauth::ContractStatus::Active:
		return imtauth::CTS_ACTIVE;
	case sdl::V1_0::imtauth::ContractStatus::Expired:
		return imtauth::CTS_EXPIRED;
	case sdl::V1_0::imtauth::ContractStatus::Terminated:
		return imtauth::CTS_TERMINATED;
	case sdl::V1_0::imtauth::ContractStatus::Renewed:
		return imtauth::CTS_RENEWED;
	default:
		return imtauth::CTS_DRAFT;
	}
}


sdl::V1_0::imtauth::ContractStatus ToSdlContractStatus(imtauth::ContractStatus status)
{
	switch (status){
	case imtauth::CTS_ACTIVE:
		return sdl::V1_0::imtauth::ContractStatus::Active;
	case imtauth::CTS_EXPIRED:
		return sdl::V1_0::imtauth::ContractStatus::Expired;
	case imtauth::CTS_TERMINATED:
		return sdl::V1_0::imtauth::ContractStatus::Terminated;
	case imtauth::CTS_RENEWED:
		return sdl::V1_0::imtauth::ContractStatus::Renewed;
	default:
		return sdl::V1_0::imtauth::ContractStatus::Draft;
	}
}


sdl::V1_0::imtauth::CContract ContractInfoToData(const imtauth::ContractInfo& info)
{
	sdl::V1_0::imtauth::CContract data;
	data.id = info.contractId;
	data.relationshipId = info.relationshipId;
	data.sourceTenantId = info.sourceTenantId;
	data.targetTenantId = info.targetTenantId;
	data.status = ToSdlContractStatus(info.status);
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


sdl::V1_0::imtauth::CGetCrossOrgGrantsPayload CTenantManagerControllerComp::OnGetCrossOrgGrants(
			const sdl::V1_0::imtauth::CGetCrossOrgGrantsGqlRequest& getCrossOrgGrantsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
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

	response.grants.Emplace();

	QSet<QByteArray> seenIds;
	const imtauth::CrossOrgGrants outgoing = m_grantManagerCompPtr->GetGrantsBySourceTenant(tenantId);
	for (const imtauth::CrossOrgGrantInfo& info : outgoing){
		if (!seenIds.contains(info.grantId)){
			seenIds.insert(info.grantId);
			response.grants->push_back(GrantInfoToData(info));
		}
	}

	const imtauth::CrossOrgGrants incoming = m_grantManagerCompPtr->GetGrantsByTargetTenant(tenantId);
	for (const imtauth::CrossOrgGrantInfo& info : incoming){
		if (!seenIds.contains(info.grantId)){
			seenIds.insert(info.grantId);
			response.grants->push_back(GrantInfoToData(info));
		}
	}

	return response;
}


sdl::V1_0::imtauth::CCreateCrossOrgGrantPayload CTenantManagerControllerComp::OnCreateCrossOrgGrant(
			const sdl::V1_0::imtauth::CCreateCrossOrgGrantGqlRequest& createCrossOrgGrantRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CCreateCrossOrgGrantPayload response;


	if (!m_grantManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Cross-org grant manager is not configured");
		return response;
	}

	QByteArray sourceTenantId;
	QByteArray targetTenantId;
	QByteArray relationshipId;
	QByteArray contractId;
	QByteArray targetTeamId;
	imtauth::CrossOrgAccessLevel accessLevel = imtauth::COAL_NONE;
	QString resourceScope;
	QString description;
	QString expiresAt;

	sdl::V1_0::imtauth::CreateCrossOrgGrantRequestArguments arguments = createCrossOrgGrantRequest.GetRequestedArguments();
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
	if (arguments.input->accessLevel){
		accessLevel = FromSdlAccessLevel(*arguments.input->accessLevel);
	}
	if (arguments.input->resourceScope){
		resourceScope = *arguments.input->resourceScope;
	}
	if (arguments.input->targetTeamId){
		targetTeamId = *arguments.input->targetTeamId;
	}
	if (arguments.input->description){
		description = *arguments.input->description;
	}
	if (arguments.input->expiresAt){
		expiresAt = *arguments.input->expiresAt;
	}

	QByteArray grantId = m_grantManagerCompPtr->CreateGrant(
				sourceTenantId,
				targetTenantId,
				relationshipId,
				accessLevel,
				resourceScope,
				targetTeamId,
				description,
				expiresAt,
				contractId);

	if (grantId.isEmpty()){
		response.errorMessage = QStringLiteral("Failed to create cross-org grant");
		return response;
	}

	response.grantId = grantId;

	return response;
}


sdl::V1_0::imtauth::CRevokeCrossOrgGrantPayload CTenantManagerControllerComp::OnRevokeCrossOrgGrant(
			const sdl::V1_0::imtauth::CRevokeCrossOrgGrantGqlRequest& revokeCrossOrgGrantRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
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

	bool success = m_grantManagerCompPtr->RevokeGrant(grantId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to revoke cross-org grant");
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


sdl::V1_0::imtauth::CGetTenantConnectionRequestsPayload CTenantManagerControllerComp::OnGetTenantConnectionRequests(
			const sdl::V1_0::imtauth::CGetTenantConnectionRequestsGqlRequest& getTenantConnectionRequestsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetTenantConnectionRequestsPayload response;


	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant connection request manager is not configured");
		return response;
	}

	QByteArray tenantId;
	sdl::V1_0::imtauth::GetTenantConnectionRequestsRequestArguments arguments = getTenantConnectionRequestsRequest.GetRequestedArguments();
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}

	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Tenant ID is required");
		return response;
	}

	response.requests.Emplace();

	QSet<QByteArray> seenIds;
	const imtauth::TenantConnectionRequests outgoing = m_connectionRequestManagerCompPtr->GetOutgoingRequests(tenantId);
	for (const imtauth::TenantConnectionRequestInfo& info : outgoing){
		if (!seenIds.contains(info.requestId)){
			seenIds.insert(info.requestId);
			response.requests->push_back(ConnectionRequestToData(info));
		}
	}

	const imtauth::TenantConnectionRequests incoming = m_connectionRequestManagerCompPtr->GetIncomingRequests(tenantId);
	for (const imtauth::TenantConnectionRequestInfo& info : incoming){
		if (!seenIds.contains(info.requestId)){
			seenIds.insert(info.requestId);
			response.requests->push_back(ConnectionRequestToData(info));
		}
	}

	return response;
}


sdl::V1_0::imtauth::CCreateTenantConnectionRequestPayload CTenantManagerControllerComp::OnCreateTenantConnectionRequest(
			const sdl::V1_0::imtauth::CCreateTenantConnectionRequestGqlRequest& createTenantConnectionRequestRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CCreateTenantConnectionRequestPayload response;


	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant connection request manager is not configured");
		return response;
	}

	QByteArray sourceTenantId;
	QString targetIdentifier;
	imtauth::ITenantInfo::TenantRelationshipRole proposedSourceRole = imtauth::ITenantInfo::Partner;
	imtauth::ITenantInfo::TenantRelationshipRole proposedTargetRole = imtauth::ITenantInfo::Partner;
	QString message;
	QString expiresAt;

	sdl::V1_0::imtauth::CreateTenantConnectionRequestRequestArguments arguments = createTenantConnectionRequestRequest.GetRequestedArguments();
	if (arguments.input->sourceTenantId){
		sourceTenantId = *arguments.input->sourceTenantId;
	}
	if (arguments.input->targetIdentifier){
		targetIdentifier = *arguments.input->targetIdentifier;
	}
	if (arguments.input->proposedSourceRole){
		proposedSourceRole = FromSdlRelationshipRole(*arguments.input->proposedSourceRole);
	}
	if (arguments.input->proposedTargetRole){
		proposedTargetRole = FromSdlRelationshipRole(*arguments.input->proposedTargetRole);
	}
	if (arguments.input->message){
		message = *arguments.input->message;
	}
	if (arguments.input->expiresAt){
		expiresAt = *arguments.input->expiresAt;
	}

	QByteArray requestId = m_connectionRequestManagerCompPtr->CreateConnectionRequest(
				sourceTenantId,
				targetIdentifier,
				proposedSourceRole,
				proposedTargetRole,
				message,
				expiresAt);

	if (requestId.isEmpty()){
		response.errorMessage = QStringLiteral("Failed to create tenant connection request");
		return response;
	}

	response.requestId = requestId;

	return response;
}


sdl::V1_0::imtauth::CCreateTenantConnectCodePayload CTenantManagerControllerComp::OnCreateTenantConnectCode(
			const sdl::V1_0::imtauth::CCreateTenantConnectCodeGqlRequest& createTenantConnectCodeRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CCreateTenantConnectCodePayload response;


	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant connection request manager is not configured");
		return response;
	}

	QByteArray sourceTenantId;
	imtauth::ITenantInfo::TenantRelationshipRole proposedSourceRole = imtauth::ITenantInfo::Partner;
	imtauth::ITenantInfo::TenantRelationshipRole proposedTargetRole = imtauth::ITenantInfo::Partner;
	QString message;
	QString expiresAt;

	sdl::V1_0::imtauth::CreateTenantConnectCodeRequestArguments arguments = createTenantConnectCodeRequest.GetRequestedArguments();
	if (arguments.input->sourceTenantId){
		sourceTenantId = *arguments.input->sourceTenantId;
	}
	if (arguments.input->proposedSourceRole){
		proposedSourceRole = FromSdlRelationshipRole(*arguments.input->proposedSourceRole);
	}
	if (arguments.input->proposedTargetRole){
		proposedTargetRole = FromSdlRelationshipRole(*arguments.input->proposedTargetRole);
	}
	if (arguments.input->message){
		message = *arguments.input->message;
	}
	if (arguments.input->expiresAt){
		expiresAt = *arguments.input->expiresAt;
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
		response.errorMessage = QStringLiteral("Failed to create tenant connect code");
		return response;
	}

	response.requestId = requestId;
	response.connectCode = generatedCode;

	return response;
}


sdl::V1_0::imtauth::CAcceptTenantConnectionRequestPayload CTenantManagerControllerComp::OnAcceptTenantConnectionRequest(
			const sdl::V1_0::imtauth::CAcceptTenantConnectionRequestGqlRequest& acceptTenantConnectionRequestRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CAcceptTenantConnectionRequestPayload response;


	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.success = false;
		response.errorMessage = QStringLiteral("Tenant connection request manager is not configured");
		return response;
	}

	QByteArray requestId;
	QByteArray acceptingTenantId;
	sdl::V1_0::imtauth::AcceptTenantConnectionRequestRequestArguments arguments = acceptTenantConnectionRequestRequest.GetRequestedArguments();
	if (arguments.input->requestId){
		requestId = *arguments.input->requestId;
	}
	if (arguments.input->acceptingTenantId){
		acceptingTenantId = *arguments.input->acceptingTenantId;
	}

	bool success = m_connectionRequestManagerCompPtr->AcceptConnectionRequest(requestId, acceptingTenantId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to accept tenant connection request");
	}

	return response;
}


sdl::V1_0::imtauth::CAcceptTenantConnectCodePayload CTenantManagerControllerComp::OnAcceptTenantConnectCode(
			const sdl::V1_0::imtauth::CAcceptTenantConnectCodeGqlRequest& acceptTenantConnectCodeRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CAcceptTenantConnectCodePayload response;


	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.errorMessage = QStringLiteral("Tenant connection request manager is not configured");
		return response;
	}

	QString connectCode;
	QByteArray acceptingTenantId;
	sdl::V1_0::imtauth::AcceptTenantConnectCodeRequestArguments arguments = acceptTenantConnectCodeRequest.GetRequestedArguments();
	if (arguments.input->connectCode){
		connectCode = *arguments.input->connectCode;
	}
	if (arguments.input->acceptingTenantId){
		acceptingTenantId = *arguments.input->acceptingTenantId;
	}

	QByteArray requestId = m_connectionRequestManagerCompPtr->AcceptConnectCode(connectCode, acceptingTenantId);
	if (requestId.isEmpty()){
		response.errorMessage = QStringLiteral("Failed to accept tenant connect code");
		return response;
	}

	response.requestId = requestId;

	return response;
}


sdl::V1_0::imtauth::CRejectTenantConnectionRequestPayload CTenantManagerControllerComp::OnRejectTenantConnectionRequest(
			const sdl::V1_0::imtauth::CRejectTenantConnectionRequestGqlRequest& rejectTenantConnectionRequestRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRejectTenantConnectionRequestPayload response;


	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.success = false;
		response.errorMessage = QStringLiteral("Tenant connection request manager is not configured");
		return response;
	}

	QByteArray requestId;
	sdl::V1_0::imtauth::RejectTenantConnectionRequestRequestArguments arguments = rejectTenantConnectionRequestRequest.GetRequestedArguments();
	if (arguments.input->requestId){
		requestId = *arguments.input->requestId;
	}

	bool success = m_connectionRequestManagerCompPtr->RejectConnectionRequest(requestId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to reject tenant connection request");
	}

	return response;
}


sdl::V1_0::imtauth::CRevokeTenantConnectionRequestPayload CTenantManagerControllerComp::OnRevokeTenantConnectionRequest(
			const sdl::V1_0::imtauth::CRevokeTenantConnectionRequestGqlRequest& revokeTenantConnectionRequestRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRevokeTenantConnectionRequestPayload response;


	if (!m_connectionRequestManagerCompPtr.IsValid()){
		response.success = false;
		response.errorMessage = QStringLiteral("Tenant connection request manager is not configured");
		return response;
	}

	QByteArray requestId;
	sdl::V1_0::imtauth::RevokeTenantConnectionRequestRequestArguments arguments = revokeTenantConnectionRequestRequest.GetRequestedArguments();
	if (arguments.input->requestId){
		requestId = *arguments.input->requestId;
	}

	bool success = m_connectionRequestManagerCompPtr->RevokeConnectionRequest(requestId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to revoke tenant connection request");
	}

	return response;
}


namespace
{


sdl::V1_0::imtauth::CrossTenantMessageType ToSdlMessageType(imtauth::CrossTenantMessageType type)
{
	switch (type){
	case imtauth::CTMT_ORDER_REQUEST:
		return sdl::V1_0::imtauth::CrossTenantMessageType::OrderRequest;
	case imtauth::CTMT_ORDER_CONFIRMATION:
		return sdl::V1_0::imtauth::CrossTenantMessageType::OrderConfirmation;
	case imtauth::CTMT_ORDER_REJECTION:
		return sdl::V1_0::imtauth::CrossTenantMessageType::OrderRejection;
	case imtauth::CTMT_ORDER_STATUS_UPDATE:
		return sdl::V1_0::imtauth::CrossTenantMessageType::OrderStatusUpdate;
	case imtauth::CTMT_ORDER_CANCELLATION:
		return sdl::V1_0::imtauth::CrossTenantMessageType::OrderCancellation;
	case imtauth::CTMT_DOCUMENT_SHARE:
		return sdl::V1_0::imtauth::CrossTenantMessageType::DocumentShare;
	default:
		return sdl::V1_0::imtauth::CrossTenantMessageType::Custom;
	}
}


imtauth::CrossTenantMessageType FromSdlMessageType(sdl::V1_0::imtauth::CrossTenantMessageType type)
{
	switch (type){
	case sdl::V1_0::imtauth::CrossTenantMessageType::OrderRequest:
		return imtauth::CTMT_ORDER_REQUEST;
	case sdl::V1_0::imtauth::CrossTenantMessageType::OrderConfirmation:
		return imtauth::CTMT_ORDER_CONFIRMATION;
	case sdl::V1_0::imtauth::CrossTenantMessageType::OrderRejection:
		return imtauth::CTMT_ORDER_REJECTION;
	case sdl::V1_0::imtauth::CrossTenantMessageType::OrderStatusUpdate:
		return imtauth::CTMT_ORDER_STATUS_UPDATE;
	case sdl::V1_0::imtauth::CrossTenantMessageType::OrderCancellation:
		return imtauth::CTMT_ORDER_CANCELLATION;
	case sdl::V1_0::imtauth::CrossTenantMessageType::DocumentShare:
		return imtauth::CTMT_DOCUMENT_SHARE;
	default:
		return imtauth::CTMT_CUSTOM;
	}
}


sdl::V1_0::imtauth::CrossTenantMessageStatus ToSdlMessageStatus(imtauth::CrossTenantMessageStatus status)
{
	switch (status){
	case imtauth::CTMS_VALIDATED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Validated;
	case imtauth::CTMS_DELIVERED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Delivered;
	case imtauth::CTMS_ACKNOWLEDGED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Acknowledged;
	case imtauth::CTMS_PROCESSED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Processed;
	case imtauth::CTMS_FAILED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Failed;
	case imtauth::CTMS_EXPIRED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Expired;
	default:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Created;
	}
}


imtauth::CrossTenantMessageStatus FromSdlMessageStatus(sdl::V1_0::imtauth::CrossTenantMessageStatus status)
{
	switch (status){
	case sdl::V1_0::imtauth::CrossTenantMessageStatus::Validated:
		return imtauth::CTMS_VALIDATED;
	case sdl::V1_0::imtauth::CrossTenantMessageStatus::Delivered:
		return imtauth::CTMS_DELIVERED;
	case sdl::V1_0::imtauth::CrossTenantMessageStatus::Acknowledged:
		return imtauth::CTMS_ACKNOWLEDGED;
	case sdl::V1_0::imtauth::CrossTenantMessageStatus::Processed:
		return imtauth::CTMS_PROCESSED;
	case sdl::V1_0::imtauth::CrossTenantMessageStatus::Failed:
		return imtauth::CTMS_FAILED;
	case sdl::V1_0::imtauth::CrossTenantMessageStatus::Expired:
		return imtauth::CTMS_EXPIRED;
	default:
		return imtauth::CTMS_CREATED;
	}
}


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


sdl::V1_0::imtauth::OrderRequestStatus ToSdlOrderStatus(imtauth::OrderRequestStatus status)
{
	switch (status){
	case imtauth::ORS_CONFIRMED:
		return sdl::V1_0::imtauth::OrderRequestStatus::Confirmed;
	case imtauth::ORS_REJECTED:
		return sdl::V1_0::imtauth::OrderRequestStatus::Rejected;
	case imtauth::ORS_IN_PROGRESS:
		return sdl::V1_0::imtauth::OrderRequestStatus::InProgress;
	case imtauth::ORS_COMPLETED:
		return sdl::V1_0::imtauth::OrderRequestStatus::Completed;
	case imtauth::ORS_CANCELLED:
		return sdl::V1_0::imtauth::OrderRequestStatus::Cancelled;
	default:
		return sdl::V1_0::imtauth::OrderRequestStatus::Received;
	}
}


imtauth::OrderRequestStatus FromSdlOrderStatus(sdl::V1_0::imtauth::OrderRequestStatus status)
{
	switch (status){
	case sdl::V1_0::imtauth::OrderRequestStatus::Confirmed:
		return imtauth::ORS_CONFIRMED;
	case sdl::V1_0::imtauth::OrderRequestStatus::Rejected:
		return imtauth::ORS_REJECTED;
	case sdl::V1_0::imtauth::OrderRequestStatus::InProgress:
		return imtauth::ORS_IN_PROGRESS;
	case sdl::V1_0::imtauth::OrderRequestStatus::Completed:
		return imtauth::ORS_COMPLETED;
	case sdl::V1_0::imtauth::OrderRequestStatus::Cancelled:
		return imtauth::ORS_CANCELLED;
	default:
		return imtauth::ORS_RECEIVED;
	}
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
	data.status = ToSdlOrderStatus(info.status);
	data.statusNote = info.statusNote;
	data.createdAt = info.createdAt;
	data.updatedAt = info.updatedAt;
	return data;
}


} // anonymous namespace


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