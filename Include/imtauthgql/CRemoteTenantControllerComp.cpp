// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteTenantControllerComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtauthgql
{


// reimplemented (imtauth::ITenantManager)

QByteArrayList CRemoteTenantControllerComp::GetTenantIds() const
{
	namespace tenantsdl = sdl::imtauth::Tenants;

	imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_QUERY, tenantsdl::CGetTenantIdsGqlRequest::GetCommandId());
	tenantsdl::CGetTenantIdsGqlRequest getTenantIdsRequest(gqlRequest, false);

	QString errorMessage;
	tenantsdl::CGetTenantIdsPayload payload = OnGetTenantIds(getTenantIdsRequest, gqlRequest, errorMessage);
	if (!payload.Version_1_0.has_value() || !payload.Version_1_0->tenantIds.has_value()){
		return QByteArrayList();
	}

	QByteArrayList result;
	for (const auto& id : *payload.Version_1_0->tenantIds){
		result.append(*id);
	}

	return result;
}


imtauth::ITenantInfoUniquePtr CRemoteTenantControllerComp::GetTenant(const QByteArray& tenantId) const
{
	namespace tenantsdl = sdl::imtauth::Tenants;

	if (!m_tenantFactoryCompPtr.IsValid()){
		return nullptr;
	}

	tenantsdl::GetTenantRequestArguments arguments;
	arguments.input.Version_1_0 = tenantsdl::CGetTenantInput::V1_0();
	arguments.input.Version_1_0->tenantId = tenantId;

	imtgql::CGqlRequest gqlRequest;
	if (!tenantsdl::CGetTenantGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return nullptr;
	}

	tenantsdl::CGetTenantGqlRequest getTenantRequest(gqlRequest, false);

	QString errorMessage;
	tenantsdl::CGetTenantPayload payload = OnGetTenant(getTenantRequest, gqlRequest, errorMessage);
	if (!payload.Version_1_0.has_value() || !payload.Version_1_0->tenant.has_value()){
		return nullptr;
	}

	const auto& tenantData = *payload.Version_1_0->tenant;

	imtauth::ITenantInfoUniquePtr tenantInfoPtr = m_tenantFactoryCompPtr.CreateInstance();
	if (!tenantInfoPtr.IsValid()){
		return nullptr;
	}

	if (tenantData.id){
		tenantInfoPtr->SetTenantId(*tenantData.id);
	}
	if (tenantData.name){
		tenantInfoPtr->SetTenantName(*tenantData.name);
	}
	if (tenantData.description){
		tenantInfoPtr->SetTenantDescription(*tenantData.description);
	}
	if (tenantData.isActive){
		tenantInfoPtr->SetActive(*tenantData.isActive);
	}
	if (tenantData.tenantPermissions){
		tenantInfoPtr->SetTenantPermissions(tenantData.tenantPermissions->ToList());
	}
	if (tenantData.createdAt){
		tenantInfoPtr->SetCreatedAt(*tenantData.createdAt);
	}
	if (tenantData.updatedAt){
		tenantInfoPtr->SetUpdatedAt(*tenantData.updatedAt);
	}
	if (tenantData.ownerId){
		tenantInfoPtr->SetOwnerId(*tenantData.ownerId);
	}

	return tenantInfoPtr;
}


QByteArray CRemoteTenantControllerComp::CreateTenant(const QString& tenantName, const QString& description, const QByteArray& ownerId)
{
	namespace tenantsdl = sdl::imtauth::Tenants;

	tenantsdl::CreateTenantRequestArguments arguments;
	arguments.input.Version_1_0 = tenantsdl::CCreateTenantInput::V1_0();
	arguments.input.Version_1_0->name = tenantName;
	arguments.input.Version_1_0->description = description;
	arguments.input.Version_1_0->ownerId = ownerId;

	imtgql::CGqlRequest gqlRequest;
	if (!tenantsdl::CCreateTenantGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return QByteArray();
	}

	tenantsdl::CCreateTenantGqlRequest createTenantRequest(gqlRequest, false);

	QString errorMessage;
	tenantsdl::CCreateTenantPayload payload = OnCreateTenant(createTenantRequest, gqlRequest, errorMessage);
	if (!payload.Version_1_0.has_value() || !payload.Version_1_0->tenantId.has_value()){
		return QByteArray();
	}

	return *payload.Version_1_0->tenantId;
}


bool CRemoteTenantControllerComp::RemoveTenant(const QByteArray& tenantId)
{
	namespace tenantsdl = sdl::imtauth::Tenants;

	tenantsdl::RemoveTenantRequestArguments arguments;
	arguments.input.Version_1_0 = tenantsdl::CRemoveTenantInput::V1_0();
	arguments.input.Version_1_0->tenantId = tenantId;

	imtgql::CGqlRequest gqlRequest;
	if (!tenantsdl::CRemoveTenantGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	tenantsdl::CRemoveTenantGqlRequest removeTenantRequest(gqlRequest, false);

	QString errorMessage;
	tenantsdl::CRemoveTenantPayload payload = OnRemoveTenant(removeTenantRequest, gqlRequest, errorMessage);
	if (!payload.Version_1_0.has_value() || !payload.Version_1_0->success.has_value()){
		return false;
	}

	return *payload.Version_1_0->success;
}


bool CRemoteTenantControllerComp::UpdateTenant(const QByteArray& tenantId, const QString& tenantName, const QString& description, const QByteArray& ownerId, bool updateOwner)
{
	namespace tenantsdl = sdl::imtauth::Tenants;

	tenantsdl::UpdateTenantRequestArguments arguments;
	arguments.input.Version_1_0 = tenantsdl::CUpdateTenantInput::V1_0();
	arguments.input.Version_1_0->tenantId = tenantId;
	arguments.input.Version_1_0->name = tenantName;
	arguments.input.Version_1_0->description = description;
	if (updateOwner){
		arguments.input.Version_1_0->ownerId = ownerId;
	}

	imtgql::CGqlRequest gqlRequest;
	if (!tenantsdl::CUpdateTenantGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	tenantsdl::CUpdateTenantGqlRequest updateTenantRequest(gqlRequest, false);

	QString errorMessage;
	tenantsdl::CUpdateTenantPayload payload = OnUpdateTenant(updateTenantRequest, gqlRequest, errorMessage);
	if (!payload.Version_1_0.has_value() || !payload.Version_1_0->success.has_value()){
		return false;
	}

	return *payload.Version_1_0->success;
}


bool CRemoteTenantControllerComp::SetTenantActive(const QByteArray& tenantId, bool isActive)
{
	namespace tenantsdl = sdl::imtauth::Tenants;

	tenantsdl::SetTenantActiveRequestArguments arguments;
	arguments.input.Version_1_0 = tenantsdl::CSetTenantActiveInput::V1_0();
	arguments.input.Version_1_0->tenantId = tenantId;
	arguments.input.Version_1_0->isActive = isActive;

	imtgql::CGqlRequest gqlRequest;
	if (!tenantsdl::CSetTenantActiveGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	tenantsdl::CSetTenantActiveGqlRequest setTenantActiveRequest(gqlRequest, false);

	QString errorMessage;
	tenantsdl::CSetTenantActivePayload payload = OnSetTenantActive(setTenantActiveRequest, gqlRequest, errorMessage);
	if (!payload.Version_1_0.has_value() || !payload.Version_1_0->success.has_value()){
		return false;
	}

	return *payload.Version_1_0->success;
}


// reimplemented (sdl::V1_0::imtauth::CGraphQlHandlerCompBase)

sdl::V1_0::imtauth::CGetTenantIdsPayload CRemoteTenantControllerComp::OnGetTenantIds(
			const sdl::V1_0::imtauth::CGetTenantIdsGqlRequest& /*getTenantIdsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetTenantIdsPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetTenantRelationshipsPayload CRemoteTenantControllerComp::OnGetTenantRelationships(
			const sdl::V1_0::imtauth::CGetTenantRelationshipsGqlRequest& /*getTenantRelationshipsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetTenantRelationshipsPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetTenantPayload CRemoteTenantControllerComp::OnGetTenant(
			const sdl::V1_0::imtauth::CGetTenantGqlRequest& /*getTenantRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetTenantPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CCreateTenantPayload CRemoteTenantControllerComp::OnCreateTenant(
			const sdl::V1_0::imtauth::CCreateTenantGqlRequest& /*createTenantRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CCreateTenantPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CRemoveTenantPayload CRemoteTenantControllerComp::OnRemoveTenant(
			const sdl::V1_0::imtauth::CRemoveTenantGqlRequest& /*removeTenantRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CRemoveTenantPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CUpdateTenantPayload CRemoteTenantControllerComp::OnUpdateTenant(
			const sdl::V1_0::imtauth::CUpdateTenantGqlRequest& /*updateTenantRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CUpdateTenantPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CSetTenantActivePayload CRemoteTenantControllerComp::OnSetTenantActive(
			const sdl::V1_0::imtauth::CSetTenantActiveGqlRequest& /*setTenantActiveRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CSetTenantActivePayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CAddTenantRelationshipPayload CRemoteTenantControllerComp::OnAddTenantRelationship(
			const sdl::V1_0::imtauth::CAddTenantRelationshipGqlRequest& /*addTenantRelationshipRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CAddTenantRelationshipPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CRemoveTenantRelationshipPayload CRemoteTenantControllerComp::OnRemoveTenantRelationship(
			const sdl::V1_0::imtauth::CRemoveTenantRelationshipGqlRequest& /*removeTenantRelationshipRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CRemoveTenantRelationshipPayload>(gqlRequest, errorMessage);
}


} // namespace imtauthgql
