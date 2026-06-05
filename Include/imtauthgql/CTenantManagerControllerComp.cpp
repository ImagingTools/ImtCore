// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantManagerControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


// Qt includes
#include <QtCore/QSet>
#include <QtCore/QMap>

// ImtCore includes
#include <imtauth/imtauth.h>
#include <imtauth/ITenantRelationshipInfo.h>
#include <imtauth/ITenantRelationshipProposalInfo.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


namespace imtauthgql
{


namespace
{


imtauth::ITenantRelationshipInfo::TenantRelationshipRole FromSdlRelationshipRole(sdl::V1_0::imtauth::TenantRelationshipRole role)
{
	switch (role){
	case sdl::V1_0::imtauth::TenantRelationshipRole::Parent:
		return imtauth::ITenantRelationshipInfo::TRR_PARENT;
	case sdl::V1_0::imtauth::TenantRelationshipRole::Child:
		return imtauth::ITenantRelationshipInfo::TRR_CHILD;
	case sdl::V1_0::imtauth::TenantRelationshipRole::Supplier:
		return imtauth::ITenantRelationshipInfo::TRR_SUPPLIER;
	case sdl::V1_0::imtauth::TenantRelationshipRole::Customer:
		return imtauth::ITenantRelationshipInfo::TRR_CUSTOMER;
	case sdl::V1_0::imtauth::TenantRelationshipRole::Affiliate:
		return imtauth::ITenantRelationshipInfo::TRR_AFFILIATE;
	default:
		return imtauth::ITenantRelationshipInfo::TRR_PARTNER;
	}
}


sdl::V1_0::imtauth::TenantRelationshipRole ToSdlRelationshipRole(imtauth::ITenantRelationshipInfo::TenantRelationshipRole role)
{
	switch (role){
	case imtauth::ITenantRelationshipInfo::TRR_PARENT:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Parent;
	case imtauth::ITenantRelationshipInfo::TRR_CHILD:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Child;
	case imtauth::ITenantRelationshipInfo::TRR_SUPPLIER:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Supplier;
	case imtauth::ITenantRelationshipInfo::TRR_CUSTOMER:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Customer;
	case imtauth::ITenantRelationshipInfo::TRR_AFFILIATE:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Affiliate;
	default:
		return sdl::V1_0::imtauth::TenantRelationshipRole::Partner;
	}
}


sdl::V1_0::imtauth::RelationshipStatus ToSdlRelationshipStatus(imtauth::ITenantRelationshipInfo::TenantRelationshipStatus status)
{
	switch (status){
	case imtauth::ITenantRelationshipInfo::TRS_ARCHIVED:
		return sdl::V1_0::imtauth::RelationshipStatus::Archived;
	case imtauth::ITenantRelationshipInfo::TRS_PENDING_APPROVED:
		return sdl::V1_0::imtauth::RelationshipStatus::PendingApproval;
	default:
		return sdl::V1_0::imtauth::RelationshipStatus::Active;
	}
}


sdl::V1_0::imtauth::ConnectionRequestStatus ToSdlConnectionRequestStatus(imtauth::ITenantConnectionRequestInfo::ConnectionRequestStatus status)
{
	switch (status){
	case imtauth::ITenantConnectionRequestInfo::CRS_APPROVED:
		return sdl::V1_0::imtauth::ConnectionRequestStatus::Approved;
	case imtauth::ITenantConnectionRequestInfo::CRS_REJECTED:
		return sdl::V1_0::imtauth::ConnectionRequestStatus::Rejected;
	case imtauth::ITenantConnectionRequestInfo::CRS_CANCELED:
		return sdl::V1_0::imtauth::ConnectionRequestStatus::Canceled;
	default:
		return sdl::V1_0::imtauth::ConnectionRequestStatus::Pending;
	}
}


sdl::V1_0::imtauth::ConnectionStatus ToSdlConnectionStatus(imtauth::ITenantConnectionInfo::ConnectionStatus status)
{
	switch (status){
	case imtauth::ITenantConnectionInfo::CS_REMOVED:
		return sdl::V1_0::imtauth::ConnectionStatus::Removed;
	case imtauth::ITenantConnectionInfo::CS_SUSPENDED:
		return sdl::V1_0::imtauth::ConnectionStatus::Suspended;
	default:
		return sdl::V1_0::imtauth::ConnectionStatus::Active;
	}
}


sdl::V1_0::imtauth::TenantRelationshipProposalInfo::RelationshipProposalType ToSdlProposalType(imtauth::ITenantRelationshipProposalInfo::RelationshipProposalType type)
{
	switch (type){
	case imtauth::ITenantRelationshipProposalInfo::RPT_UPDATE:
		return sdl::V1_0::imtauth::ITenantRelationshipProposalInfo::RelationshipProposalType::Update;
	case imtauth::ITenantRelationshipProposalInfo::RPT_DELETE:
		return sdl::V1_0::imtauth::ITenantRelationshipProposalInfo::RelationshipProposalType::Delete;
	default:
		return sdl::V1_0::imtauth::ITenantRelationshipProposalInfo::RelationshipProposalType::Create;
	}
}


sdl::V1_0::imtauth::ITenantRelationshipProposalInfo::RelationshipProposalStatus ToSdlProposalStatus(imtauth::ITenantRelationshipProposalInfo::RelationshipProposalStatus status)
{
	switch (status){
	case imtauth::ITenantRelationshipProposalInfo::RPS_APPROVED_BY_INITIATOR:
		return sdl::V1_0::imtauth::ITenantRelationshipProposalInfo::RelationshipProposalStatus::ApprovedByInitiator;
	case imtauth::ITenantRelationshipProposalInfo::RPS_APPROVED_BY_COUNTERPARTY:
		return sdl::V1_0::imtauth::ITenantRelationshipProposalInfo::RelationshipProposalStatus::ApprovedByCounterparty;
	case imtauth::ITenantRelationshipProposalInfo::RPS_REJECTED:
		return sdl::V1_0::imtauth::ITenantRelationshipProposalInfo::RelationshipProposalStatus::Rejected;
	case imtauth::ITenantRelationshipProposalInfo::RPS_CANCELED:
		return sdl::V1_0::imtauth::ITenantRelationshipProposalInfo::RelationshipProposalStatus::Canceled;
	case imtauth::ITenantRelationshipProposalInfo::RPS_EXPIRED:
		return sdl::V1_0::imtauth::ITenantRelationshipProposalInfo::RelationshipProposalStatus::Expired;
	case imtauth::ITenantRelationshipProposalInfo::RPS_APPLIED:
		return sdl::V1_0::imtauth::ITenantRelationshipProposalInfo::RelationshipProposalStatus::Applied;
	default:
		return sdl::V1_0::imtauth::ITenantRelationshipProposalInfo::RelationshipProposalStatus::Pending;
	}
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

	response.relationships.Emplace();

	QByteArrayList relIds = m_connectionRequestManagerCompPtr->GetTenantRelationshipIds(tenantId);
	for (const QByteArray& relId : relIds){
		// Load relationship details from collection via the connection request manager interface
		// The relationship data is stored in the relationship collection managed by the manager
		sdl::V1_0::imtauth::CTenantRelationship data;
		data.id = relId;
		response.relationships->push_back(data);
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

	sdl::V1_0::imtauth::UpdateTenantRequestArguments arguments = updateTenantRequest.GetRequestedArguments();

	QByteArray tenantId;
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}

	QString name;
	if (arguments.input->name){
		name = *arguments.input->name;
	}


	QString description;
	if (arguments.input->description){
		description = *arguments.input->description;
	}

	QByteArray ownerId;

	if (arguments.input->ownerId){
		ownerId = *arguments.input->ownerId;
	}

	bool success = m_tenantManagerCompPtr->UpdateTenant(tenantId, name, description, ownerId, false);

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


sdl::V1_0::imtauth::CGetConnectionCodePayload CTenantManagerControllerComp::OnGetConnectionCode(
		const sdl::V1_0::imtauth::CGetConnectionCodeGqlRequest& request,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
	
	bool success = m_connectionRequestManagerCompPtr->SetAllowConnectionsByCode(tenantId, allow);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to update allow connections setting");
	}
	
	return response;
}


sdl::V1_0::imtauth::CGetConnectionRequestsPayload CTenantManagerControllerComp::OnGetConnectionRequests(
		const sdl::V1_0::imtauth::CGetConnectionRequestsGqlRequest& request,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
	
	QByteArrayList requestIds = m_connectionRequestManagerCompPtr->GetConnectionRequestIds(tenantId);
	response.requests.Emplace();

	if (m_requestCollectionCompPtr.IsValid()){
		for (const QByteArray& requestId : requestIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_requestCollectionCompPtr->GetObjectData(requestId, dataPtr)){
				const imtauth::ITenantConnectionRequestInfo* reqPtr =
					dynamic_cast<const imtauth::ITenantConnectionRequestInfo*>(dataPtr.GetPtr());
				if (reqPtr != nullptr){
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
					response.requests->push_back(data);
				}
			}
		}
	}
	
	return response;
}


sdl::V1_0::imtauth::CCreateConnectionRequestPayload CTenantManagerControllerComp::OnCreateConnectionRequest(
		const sdl::V1_0::imtauth::CCreateConnectionRequestGqlRequest& request,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
	
	bool success = m_connectionRequestManagerCompPtr->RejectConnectionRequest(requestId, tenantId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to reject connection request");
	}
	
	return response;
}


sdl::V1_0::imtauth::CCancelConnectionRequestPayload CTenantManagerControllerComp::OnCancelConnectionRequest(
		const sdl::V1_0::imtauth::CCancelConnectionRequestGqlRequest& request,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
	
	bool success = m_connectionRequestManagerCompPtr->CancelConnectionRequest(requestId, tenantId);
	response.success = success;
	if (!success){
		response.errorMessage = QStringLiteral("Failed to cancel connection request");
	}
	
	return response;
}


sdl::V1_0::imtauth::CGetConnectionsPayload CTenantManagerControllerComp::OnGetConnections(
			const sdl::V1_0::imtauth::CGetConnectionsGqlRequest& request,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
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

	response.connections.Emplace();
	QByteArrayList connectionIds = m_connectionRequestManagerCompPtr->GetConnectionIds(tenantId);

	if (m_connectionCollectionCompPtr.IsValid()){
		for (const QByteArray& connectionId : connectionIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_connectionCollectionCompPtr->GetObjectData(connectionId, dataPtr)){
				const imtauth::ITenantConnectionInfo* connPtr =
					dynamic_cast<const imtauth::ITenantConnectionInfo*>(dataPtr.GetPtr());
				if (connPtr != nullptr){
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
						}

						imtauth::ITenantInfoUniquePtr tenant2Ptr = m_tenantManagerCompPtr->GetTenant(connPtr->GetTenantBId());
						if (tenant2Ptr.IsValid()){
							data.tenantBName = tenant2Ptr->GetTenantName();
						}
					}

					response.connections->push_back(data);
				}
			}
		}
	}

	return response;
}


sdl::V1_0::imtauth::CRemoveConnectionPayload CTenantManagerControllerComp::OnRemoveConnection(
		const sdl::V1_0::imtauth::CRemoveConnectionGqlRequest& request,
		const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
	
	QByteArray connectionId;
	QByteArray initiatorTenantId;
	QByteArray counterpartyTenantId;
	if (arguments.input->connectionId){
		connectionId = *arguments.input->connectionId;
	}
	if (arguments.input->initiatorTenantId){
		initiatorTenantId = *arguments.input->initiatorTenantId;
	}
	if (arguments.input->counterpartyTenantId){
		counterpartyTenantId = *arguments.input->counterpartyTenantId;
	}
	
	QByteArray proposalId = m_connectionRequestManagerCompPtr->CreateRelationshipProposal(
		QByteArray(), connectionId, initiatorTenantId, counterpartyTenantId);
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
