// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantManagerControllerComp.h>


// Qt includes
#include <QSet>
#include <QMap>

// ImtCore includes
#include <imtauth/imtauth.h>


namespace imtauthgql
{


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
	for (const auto& id : ids){
		response.Version_1_0->tenantIds->push_back(id);
	}

	return response;
}


sdl::imtauth::Tenants::CGetTenantRelationshipsPayload CTenantManagerControllerComp::OnGetTenantRelationships(
			const sdl::imtauth::Tenants::CGetTenantRelationshipsGqlRequest& /*getTenantRelationshipsRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CGetTenantRelationshipsPayload response;
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
	tenantData.isActive = tenantInfoPtr->IsActive();
	tenantData.createdAt = tenantInfoPtr->GetCreatedAt();
	tenantData.updatedAt = tenantInfoPtr->GetUpdatedAt();
	tenantData.tenantPermissions.Emplace().FromList(tenantInfoPtr->GetTenantPermissions());

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

	QByteArray tenantId = m_tenantManagerCompPtr->CreateTenant(name, description, ownerId);

	if (tenantId.isEmpty()){
		response.Version_1_0->errorMessage = QStringLiteral("Failed to create tenant");
		return response;
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
				m_membershipManagerCompPtr->InviteMembership(uid, tenantId, QByteArray());
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
			const sdl::imtauth::Tenants::CAddTenantRelationshipGqlRequest& /*addTenantRelationshipRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CAddTenantRelationshipPayload response;
	return response;
}


sdl::imtauth::Tenants::CRemoveTenantRelationshipPayload CTenantManagerControllerComp::OnRemoveTenantRelationship(
			const sdl::imtauth::Tenants::CRemoveTenantRelationshipGqlRequest& /*removeTenantRelationshipRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::imtauth::Tenants::CRemoveTenantRelationshipPayload response;
	return response;
}


} // namespace imtauthgql
