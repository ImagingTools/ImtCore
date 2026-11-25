// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantRelationshipProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Contracts_fwd.h>


namespace imtauthgql
{


/**
	The contracts of the organizations on the server of a product: every command goes to the authorization server, which checks it.
*/
class CRemoteContractControllerComp:
			virtual public imtauth::ITenantRelationshipProvider,
			public imtclientgql::TClientRequestManagerCompWrap<sdl::V1_0::imtauth::CContractsGqlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::V1_0::imtauth::CContractsGqlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteContractControllerComp);
		I_REGISTER_INTERFACE(imtauth::ITenantRelationshipProvider);
	I_END_COMPONENT;

	// reimplemented (imtauth::ITenantRelationshipProvider)
	virtual imtauth::RelatedTenantInfos GetRelatedTenants(const QByteArray& tenantId, QString* tenantNamePtr = nullptr, bool* isReadPtr = nullptr) const override;

protected:
	// reimplemented (sdl::V1_0::imtauth::CContractsGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CGetContractsPayload OnGetContracts(
				const sdl::V1_0::imtauth::CGetContractsGqlRequest& getContractsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetContractPayload OnGetContract(
				const sdl::V1_0::imtauth::CGetContractGqlRequest& getContractRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetContractEventsPayload OnGetContractEvents(
				const sdl::V1_0::imtauth::CGetContractEventsGqlRequest& getContractEventsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetContractAttachmentPayload OnGetContractAttachment(
				const sdl::V1_0::imtauth::CGetContractAttachmentGqlRequest& getContractAttachmentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetRelatedTenantsPayload OnGetRelatedTenants(
				const sdl::V1_0::imtauth::CGetRelatedTenantsGqlRequest& getRelatedTenantsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CGetContractPartyCandidatesPayload OnGetContractPartyCandidates(
				const sdl::V1_0::imtauth::CGetContractPartyCandidatesGqlRequest& getContractPartyCandidatesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnCreateContract(
				const sdl::V1_0::imtauth::CCreateContractGqlRequest& createContractRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnUpdateContract(
				const sdl::V1_0::imtauth::CUpdateContractGqlRequest& updateContractRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnAddContractPartyByCode(
				const sdl::V1_0::imtauth::CAddContractPartyByCodeGqlRequest& addContractPartyByCodeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnRemoveContract(
				const sdl::V1_0::imtauth::CRemoveContractGqlRequest& removeContractRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnSubmitContract(
				const sdl::V1_0::imtauth::CSubmitContractGqlRequest& submitContractRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnCancelContract(
				const sdl::V1_0::imtauth::CCancelContractGqlRequest& cancelContractRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnRequestContractChanges(
				const sdl::V1_0::imtauth::CRequestContractChangesGqlRequest& requestContractChangesRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnApproveContract(
				const sdl::V1_0::imtauth::CApproveContractGqlRequest& approveContractRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnSignContract(
				const sdl::V1_0::imtauth::CSignContractGqlRequest& signContractRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnDeclineContract(
				const sdl::V1_0::imtauth::CDeclineContractGqlRequest& declineContractRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnGiveContractNotice(
				const sdl::V1_0::imtauth::CGiveContractNoticeGqlRequest& giveContractNoticeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnRevokeContractNotice(
				const sdl::V1_0::imtauth::CRevokeContractNoticeGqlRequest& revokeContractNoticeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnGiveContractNonRenewalNotice(
				const sdl::V1_0::imtauth::CGiveContractNonRenewalNoticeGqlRequest& giveContractNonRenewalNoticeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnProposeContractTermination(
				const sdl::V1_0::imtauth::CProposeContractTerminationGqlRequest& proposeContractTerminationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnConsentContractTermination(
				const sdl::V1_0::imtauth::CConsentContractTerminationGqlRequest& consentContractTerminationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnCancelContractTerminationProposal(
				const sdl::V1_0::imtauth::CCancelContractTerminationProposalGqlRequest& cancelContractTerminationProposalRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnSetContractAllocationsSuspended(
				const sdl::V1_0::imtauth::CSetContractAllocationsSuspendedGqlRequest& setContractAllocationsSuspendedRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnAddContractAttachment(
				const sdl::V1_0::imtauth::CAddContractAttachmentGqlRequest& addContractAttachmentRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnSetContractPrivateInfo(
				const sdl::V1_0::imtauth::CSetContractPrivateInfoGqlRequest& setContractPrivateInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnSetContractAllocationGroups(
				const sdl::V1_0::imtauth::CSetContractAllocationGroupsGqlRequest& setContractAllocationGroupsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
};


} // namespace imtauthgql


