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
	namespace tenantsdl = sdl::V1_0::imtauth;

	imtgql::CGqlRequest gqlRequest(imtgql::IGqlRequest::RT_QUERY, tenantsdl::CGetTenantIdsGqlRequest::GetCommandId());
	tenantsdl::CGetTenantIdsGqlRequest getTenantIdsRequest(gqlRequest, false);

	QString errorMessage;
	tenantsdl::CGetTenantIdsPayload payload = OnGetTenantIds(getTenantIdsRequest, gqlRequest, errorMessage);
	if (!payload.tenantIds.has_value()){
		return QByteArrayList();
	}

	QByteArrayList result;
	for (const auto& id : *payload.tenantIds){
		result.append(*id);
	}

	return result;
}


imtauth::ITenantInfoUniquePtr CRemoteTenantControllerComp::GetTenant(const QByteArray& tenantId) const
{
	namespace tenantsdl = sdl::V1_0::imtauth;

	if (!m_tenantFactoryCompPtr.IsValid()){
		return nullptr;
	}

	tenantsdl::GetTenantRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->tenantId = tenantId;

	imtgql::CGqlRequest gqlRequest;
	if (!tenantsdl::CGetTenantGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return nullptr;
	}

	tenantsdl::CGetTenantGqlRequest getTenantRequest(gqlRequest, false);

	QString errorMessage;
	tenantsdl::CGetTenantPayload payload = OnGetTenant(getTenantRequest, gqlRequest, errorMessage);
	if (!payload.tenant.has_value()){
		return nullptr;
	}

	const auto& tenantData = *payload.tenant;

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
	namespace tenantsdl = sdl::V1_0::imtauth;

	tenantsdl::CreateTenantRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->name = tenantName;
	arguments.input->description = description;
	arguments.input->ownerId = ownerId;

	imtgql::CGqlRequest gqlRequest;
	if (!tenantsdl::CCreateTenantGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return QByteArray();
	}

	tenantsdl::CCreateTenantGqlRequest createTenantRequest(gqlRequest, false);

	QString errorMessage;
	tenantsdl::CCreateTenantPayload payload = OnCreateTenant(createTenantRequest, gqlRequest, errorMessage);
	if (!payload.tenantId.has_value()){
		return QByteArray();
	}

	return *payload.tenantId;
}


bool CRemoteTenantControllerComp::RemoveTenant(const QByteArray& tenantId)
{
	namespace tenantsdl = sdl::V1_0::imtauth;

	tenantsdl::RemoveTenantRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->tenantId = tenantId;

	imtgql::CGqlRequest gqlRequest;
	if (!tenantsdl::CRemoveTenantGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	tenantsdl::CRemoveTenantGqlRequest removeTenantRequest(gqlRequest, false);

	QString errorMessage;
	tenantsdl::CRemoveTenantPayload payload = OnRemoveTenant(removeTenantRequest, gqlRequest, errorMessage);
	if (!payload.success.has_value()){
		return false;
	}

	return *payload.success;
}


bool CRemoteTenantControllerComp::UpdateTenant(const QByteArray& tenantId, const QString& tenantName, const QString& description, const QByteArray& ownerId, bool updateOwner)
{
	namespace tenantsdl = sdl::V1_0::imtauth;

	tenantsdl::UpdateTenantRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->tenantId = tenantId;
	arguments.input->name = tenantName;
	arguments.input->description = description;
	if (updateOwner){
		arguments.input->ownerId = ownerId;
	}

	imtgql::CGqlRequest gqlRequest;
	if (!tenantsdl::CUpdateTenantGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	tenantsdl::CUpdateTenantGqlRequest updateTenantRequest(gqlRequest, false);

	QString errorMessage;
	tenantsdl::CUpdateTenantPayload payload = OnUpdateTenant(updateTenantRequest, gqlRequest, errorMessage);
	if (!payload.success.has_value()){
		return false;
	}

	return *payload.success;
}


bool CRemoteTenantControllerComp::SetTenantActive(const QByteArray& tenantId, bool isActive)
{
	namespace tenantsdl = sdl::V1_0::imtauth;

	tenantsdl::SetTenantActiveRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->tenantId = tenantId;
	arguments.input->isActive = isActive;

	imtgql::CGqlRequest gqlRequest;
	if (!tenantsdl::CSetTenantActiveGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	tenantsdl::CSetTenantActiveGqlRequest setTenantActiveRequest(gqlRequest, false);

	QString errorMessage;
	tenantsdl::CSetTenantActivePayload payload = OnSetTenantActive(setTenantActiveRequest, gqlRequest, errorMessage);
	if (!payload.success.has_value()){
		return false;
	}

	return *payload.success;
}


// reimplemented (sdl::V1_0::imtauth::CTenantsGqlHandlerCompBase)

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


sdl::V1_0::imtauth::CGetCrossOrgGrantsPayload CRemoteTenantControllerComp::OnGetCrossOrgGrants(
			const sdl::V1_0::imtauth::CGetCrossOrgGrantsGqlRequest& /*getCrossOrgGrantsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetCrossOrgGrantsPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CCreateCrossOrgGrantPayload CRemoteTenantControllerComp::OnCreateCrossOrgGrant(
			const sdl::V1_0::imtauth::CCreateCrossOrgGrantGqlRequest& /*createCrossOrgGrantRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CCreateCrossOrgGrantPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CRevokeCrossOrgGrantPayload CRemoteTenantControllerComp::OnRevokeCrossOrgGrant(
			const sdl::V1_0::imtauth::CRevokeCrossOrgGrantGqlRequest& /*revokeCrossOrgGrantRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CRevokeCrossOrgGrantPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetContractsPayload CRemoteTenantControllerComp::OnGetContracts(
			const sdl::V1_0::imtauth::CGetContractsGqlRequest& /*getContractsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetContractsPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CCreateContractPayload CRemoteTenantControllerComp::OnCreateContract(
			const sdl::V1_0::imtauth::CCreateContractGqlRequest& /*createContractRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CCreateContractPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CUpdateContractStatusPayload CRemoteTenantControllerComp::OnUpdateContractStatus(
			const sdl::V1_0::imtauth::CUpdateContractStatusGqlRequest& /*updateContractStatusRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CUpdateContractStatusPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CTerminateContractPayload CRemoteTenantControllerComp::OnTerminateContract(
			const sdl::V1_0::imtauth::CTerminateContractGqlRequest& /*terminateContractRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CTerminateContractPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CEnsureSystemTenantPayload CRemoteTenantControllerComp::OnEnsureSystemTenant(
			const sdl::V1_0::imtauth::CEnsureSystemTenantGqlRequest& /*ensureSystemTenantRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CEnsureSystemTenantPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetTenantConnectionRequestsPayload CRemoteTenantControllerComp::OnGetTenantConnectionRequests(
			const sdl::V1_0::imtauth::CGetTenantConnectionRequestsGqlRequest& /*getTenantConnectionRequestsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetTenantConnectionRequestsPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetTenantConnectCodeDetailsPayload CRemoteTenantControllerComp::OnGetTenantConnectCodeDetails(
			const sdl::V1_0::imtauth::CGetTenantConnectCodeDetailsGqlRequest& /*getTenantConnectCodeDetailsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetTenantConnectCodeDetailsPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CCreateTenantConnectionRequestPayload CRemoteTenantControllerComp::OnCreateTenantConnectionRequest(
			const sdl::V1_0::imtauth::CCreateTenantConnectionRequestGqlRequest& /*createTenantConnectionRequestRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CCreateTenantConnectionRequestPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CCreateTenantConnectCodePayload CRemoteTenantControllerComp::OnCreateTenantConnectCode(
			const sdl::V1_0::imtauth::CCreateTenantConnectCodeGqlRequest& /*createTenantConnectCodeRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CCreateTenantConnectCodePayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CAcceptTenantConnectionRequestPayload CRemoteTenantControllerComp::OnAcceptTenantConnectionRequest(
			const sdl::V1_0::imtauth::CAcceptTenantConnectionRequestGqlRequest& /*acceptTenantConnectionRequestRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CAcceptTenantConnectionRequestPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CAcceptTenantConnectCodePayload CRemoteTenantControllerComp::OnAcceptTenantConnectCode(
			const sdl::V1_0::imtauth::CAcceptTenantConnectCodeGqlRequest& /*acceptTenantConnectCodeRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CAcceptTenantConnectCodePayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CRejectTenantConnectionRequestPayload CRemoteTenantControllerComp::OnRejectTenantConnectionRequest(
			const sdl::V1_0::imtauth::CRejectTenantConnectionRequestGqlRequest& /*rejectTenantConnectionRequestRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CRejectTenantConnectionRequestPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CRevokeTenantConnectionRequestPayload CRemoteTenantControllerComp::OnRevokeTenantConnectionRequest(
			const sdl::V1_0::imtauth::CRevokeTenantConnectionRequestGqlRequest& /*revokeTenantConnectionRequestRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CRevokeTenantConnectionRequestPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetCrossTenantMessagePayload CRemoteTenantControllerComp::OnGetCrossTenantMessage(
			const sdl::V1_0::imtauth::CGetCrossTenantMessageGqlRequest& /*getCrossTenantMessageRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetCrossTenantMessagePayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetCrossTenantMessagesPayload CRemoteTenantControllerComp::OnGetCrossTenantMessages(
			const sdl::V1_0::imtauth::CGetCrossTenantMessagesGqlRequest& /*getCrossTenantMessagesRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetCrossTenantMessagesPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CSendCrossTenantMessagePayload CRemoteTenantControllerComp::OnSendCrossTenantMessage(
			const sdl::V1_0::imtauth::CSendCrossTenantMessageGqlRequest& /*sendCrossTenantMessageRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CSendCrossTenantMessagePayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CUpdateCrossTenantMessageStatusPayload CRemoteTenantControllerComp::OnUpdateCrossTenantMessageStatus(
			const sdl::V1_0::imtauth::CUpdateCrossTenantMessageStatusGqlRequest& /*updateCrossTenantMessageStatusRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CUpdateCrossTenantMessageStatusPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetOrderRequestPayload CRemoteTenantControllerComp::OnGetOrderRequest(
			const sdl::V1_0::imtauth::CGetOrderRequestGqlRequest& /*getOrderRequestRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetOrderRequestPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetOrderRequestsPayload CRemoteTenantControllerComp::OnGetOrderRequests(
			const sdl::V1_0::imtauth::CGetOrderRequestsGqlRequest& /*getOrderRequestsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetOrderRequestsPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CConfirmOrderRequestPayload CRemoteTenantControllerComp::OnConfirmOrderRequest(
			const sdl::V1_0::imtauth::CConfirmOrderRequestGqlRequest& /*confirmOrderRequestRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CConfirmOrderRequestPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CRejectOrderRequestPayload CRemoteTenantControllerComp::OnRejectOrderRequest(
			const sdl::V1_0::imtauth::CRejectOrderRequestGqlRequest& /*rejectOrderRequestRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CRejectOrderRequestPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CUpdateOrderRequestStatusPayload CRemoteTenantControllerComp::OnUpdateOrderRequestStatus(
			const sdl::V1_0::imtauth::CUpdateOrderRequestStatusGqlRequest& /*updateOrderRequestStatusRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CUpdateOrderRequestStatusPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CDeleteTenantConnectCodePayload CRemoteTenantControllerComp::OnDeleteTenantConnectCode(
			const sdl::V1_0::imtauth::CDeleteTenantConnectCodeGqlRequest& /*deleteTenantConnectCodeRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CDeleteTenantConnectCodePayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CPurgeExpiredConnectCodesPayload CRemoteTenantControllerComp::OnPurgeExpiredConnectCodes(
			const sdl::V1_0::imtauth::CPurgeExpiredConnectCodesGqlRequest& /*purgeExpiredConnectCodesRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CPurgeExpiredConnectCodesPayload>(gqlRequest, errorMessage);
}


} // namespace imtauthgql


