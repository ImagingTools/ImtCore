// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemoteContractControllerComp.h>


// ImtCore includes
#include <imtauthgql/ContractGqlUtils.h>
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Contracts.h>


namespace imtauthgql
{


// public methods

// reimplemented (imtauth::ITenantRelationshipProvider)

imtauth::RelatedTenantInfos CRemoteContractControllerComp::GetRelatedTenants(const QByteArray& tenantId, QString* tenantNamePtr, bool* isReadPtr) const
{
	namespace sdlauth = sdl::V1_0::imtauth;

	imtauth::RelatedTenantInfos retVal;
	if (isReadPtr != nullptr){
		*isReadPtr = false;
	}

	if (tenantId.isEmpty()){
		return retVal;
	}

	sdlauth::GetRelatedTenantsRequestArguments arguments;
	arguments.input.emplace();
	arguments.input->tenantId = tenantId;

	imtgql::CGqlRequest gqlRequest;
	if (!sdlauth::CGetRelatedTenantsGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return retVal;
	}

	QString errorMessage;
	const sdlauth::CGetRelatedTenantsPayload payload = SendModelRequest<sdlauth::CGetRelatedTenantsPayload>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty() || !payload.relatedTenants || !payload.errorMessage.value_or(QString()).isEmpty()){
		return retVal;
	}

	if (tenantNamePtr != nullptr){
		*tenantNamePtr = payload.tenantName.value_or(QString());
	}

	for (const istd::TNullableValue<sdlauth::CRelatedTenant>& item : *payload.relatedTenants){
		if (!item){
			continue;
		}

		imtauth::RelatedTenantInfo relatedTenant;
		relatedTenant.tenantId = item->tenantId.value_or(QByteArray());
		relatedTenant.tenantName = item->tenantName.value_or(QString());
		relatedTenant.isEffective = item->isEffective.value_or(false);
		relatedTenant.relatedContractIds = FromElementList(item->relatedContractIds);
		relatedTenant.sharingContractIds = FromElementList(item->sharingContractIds);
		relatedTenant.relatedSince = FromSdlTime(item->relatedSince);
		if (!relatedTenant.tenantId.isEmpty()){
			retVal.append(relatedTenant);
		}
	}

	if (isReadPtr != nullptr){
		*isReadPtr = true;
	}

	return retVal;
}


// protected methods

// reimplemented (sdl::V1_0::imtauth::CContractsGqlHandlerCompBase)

sdl::V1_0::imtauth::CGetContractsPayload CRemoteContractControllerComp::OnGetContracts(
			const sdl::V1_0::imtauth::CGetContractsGqlRequest& /*getContractsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetContractsPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetContractPayload CRemoteContractControllerComp::OnGetContract(
			const sdl::V1_0::imtauth::CGetContractGqlRequest& /*getContractRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetContractPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetContractEventsPayload CRemoteContractControllerComp::OnGetContractEvents(
			const sdl::V1_0::imtauth::CGetContractEventsGqlRequest& /*getContractEventsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetContractEventsPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetContractAttachmentPayload CRemoteContractControllerComp::OnGetContractAttachment(
			const sdl::V1_0::imtauth::CGetContractAttachmentGqlRequest& /*getContractAttachmentRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetContractAttachmentPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetRelatedTenantsPayload CRemoteContractControllerComp::OnGetRelatedTenants(
			const sdl::V1_0::imtauth::CGetRelatedTenantsGqlRequest& /*getRelatedTenantsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetRelatedTenantsPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CGetContractPartyCandidatesPayload CRemoteContractControllerComp::OnGetContractPartyCandidates(
			const sdl::V1_0::imtauth::CGetContractPartyCandidatesGqlRequest& /*getContractPartyCandidatesRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CGetContractPartyCandidatesPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnCreateContract(
			const sdl::V1_0::imtauth::CCreateContractGqlRequest& /*createContractRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnUpdateContract(
			const sdl::V1_0::imtauth::CUpdateContractGqlRequest& /*updateContractRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnAddContractPartyByCode(
			const sdl::V1_0::imtauth::CAddContractPartyByCodeGqlRequest& /*addContractPartyByCodeRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnRemoveContract(
			const sdl::V1_0::imtauth::CRemoveContractGqlRequest& /*removeContractRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnSubmitContract(
			const sdl::V1_0::imtauth::CSubmitContractGqlRequest& /*submitContractRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnCancelContract(
			const sdl::V1_0::imtauth::CCancelContractGqlRequest& /*cancelContractRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnRequestContractChanges(
			const sdl::V1_0::imtauth::CRequestContractChangesGqlRequest& /*requestContractChangesRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnApproveContract(
			const sdl::V1_0::imtauth::CApproveContractGqlRequest& /*approveContractRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnSignContract(
			const sdl::V1_0::imtauth::CSignContractGqlRequest& /*signContractRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnDeclineContract(
			const sdl::V1_0::imtauth::CDeclineContractGqlRequest& /*declineContractRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnGiveContractNotice(
			const sdl::V1_0::imtauth::CGiveContractNoticeGqlRequest& /*giveContractNoticeRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnRevokeContractNotice(
			const sdl::V1_0::imtauth::CRevokeContractNoticeGqlRequest& /*revokeContractNoticeRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnGiveContractNonRenewalNotice(
			const sdl::V1_0::imtauth::CGiveContractNonRenewalNoticeGqlRequest& /*giveContractNonRenewalNoticeRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnSetContractAllocationsSuspended(
			const sdl::V1_0::imtauth::CSetContractAllocationsSuspendedGqlRequest& /*setContractAllocationsSuspendedRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnAddContractAttachment(
			const sdl::V1_0::imtauth::CAddContractAttachmentGqlRequest& /*addContractAttachmentRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnSetContractPrivateInfo(
			const sdl::V1_0::imtauth::CSetContractPrivateInfoGqlRequest& /*setContractPrivateInfoRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


sdl::V1_0::imtauth::CContractCommandPayload CRemoteContractControllerComp::OnSetContractAllocationGroups(
			const sdl::V1_0::imtauth::CSetContractAllocationGroupsGqlRequest& /*setContractAllocationGroupsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::V1_0::imtauth::CContractCommandPayload>(gqlRequest, errorMessage);
}


} // namespace imtauthgql


