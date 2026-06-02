// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>
#include <imtauthgql/CTenantManagerControllerComp.h>


// Qt includes
#include <QSet>
#include <QMap>

// ImtCore includes
#include <imtauth/imtauth.h>


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
	for (const auto& id : ids){
		response.tenantIds->push_back(id);
	}

	return response;
}


sdl::V1_0::imtauth::CGetTenantRelationshipsPayload CTenantManagerControllerComp::OnGetTenantRelationships(
			const sdl::V1_0::imtauth::CGetTenantRelationshipsGqlRequest& /*getTenantRelationshipsRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CGetTenantRelationshipsPayload response;
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

	QByteArray tenantId = m_tenantManagerCompPtr->CreateTenant(name, description, ownerId);

	if (tenantId.isEmpty()){
		response.errorMessage = QStringLiteral("Failed to create tenant");
		return response;
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
			const sdl::V1_0::imtauth::CAddTenantRelationshipGqlRequest& /*addTenantRelationshipRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CAddTenantRelationshipPayload response;
	return response;
}


sdl::V1_0::imtauth::CRemoveTenantRelationshipPayload CTenantManagerControllerComp::OnRemoveTenantRelationship(
			const sdl::V1_0::imtauth::CRemoveTenantRelationshipGqlRequest& /*removeTenantRelationshipRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtauth::CRemoveTenantRelationshipPayload response;
	return response;
}


} // namespace imtauthgql
