// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>

// ACF includes
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtauth/IContractManager.h>
#include <imtauth/IPartyRoleCatalog.h>
#include <imtauth/ITenantConnectionRequestManager.h>
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/ITenantRelationshipProvider.h>
#include <imtchat/IAttachmentStorage.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Contracts_fwd.h>


namespace imtauthgql
{


/**
	The contracts of the organizations: a command acts on behalf of the organization of the session, which holds
	the permission of the command and is a party of the contract.
*/
class CContractControllerComp: public sdl::V1_0::imtauth::CContractsGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtauth::CContractsGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CContractControllerComp);
		I_ASSIGN(m_contractManagerCompPtr, "ContractManager", "Contracts of the organizations", true, "ContractManager");
		I_ASSIGN(m_contractFactoryCompPtr, "ContractFactory", "Factory of the draft of a contract", true, "ContractInfo");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Organizations", true, "TenantManager");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Members and their permissions in the organizations", true, "TenantMembershipManager");
		I_ASSIGN(m_relationshipProviderCompPtr, "RelationshipProvider", "Organizations linked by the contracts", true, "RelationshipProvider");
		I_ASSIGN(m_connectionManagerCompPtr, "ConnectionManager", "Address books of the organizations", true, "TenantConnectionRequestManager");
		I_ASSIGN(m_roleCatalogCompPtr, "PartyRoleCatalog", "Roles of the parties, the system ones translated", true, "PartyRoleCatalog");
		I_ASSIGN(m_attachmentStorageCompPtr, "AttachmentStorage", "Files of the attachments", true, "AttachmentStorage");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translations of the names of the system roles", false, "TranslationManager");
		I_ASSIGN(m_maxAttachmentSizeAttrPtr, "MaxAttachmentSize", "Largest file of an attachment in bytes", true, 20 * 1024 * 1024);
	I_END_COMPONENT;

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
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnGiveContractPermissions(
				const sdl::V1_0::imtauth::CGiveContractPermissionsGqlRequest& giveContractPermissionsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CContractCommandPayload OnWithdrawContractPermissions(
				const sdl::V1_0::imtauth::CWithdrawContractPermissionsGqlRequest& withdrawContractPermissionsRequest,
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

private:
	/**
		The organization of the session holds the permission of the command, and the user acts for it.
	*/
	bool CheckAccess(
				const ::imtgql::CGqlRequest& gqlRequest,
				const QByteArray& tenantId,
				const QByteArray& permission,
				imtauth::ContractActor& actor,
				QString& errorMessage) const;

	typedef std::function<bool(const imtauth::ContractActor& actor, QString& errorMessage)> ContractCommand;

	/**
		Check the access, run the command and answer with the new revision.
	*/
	sdl::V1_0::imtauth::CContractCommandPayload RunCommand(
				const ::imtgql::CGqlRequest& gqlRequest,
				const QByteArray& tenantId,
				const QByteArray& contractId,
				const QByteArray& permission,
				const ContractCommand& command) const;

	/**
		The contract the organization takes part in, as it sees it; null when it is no party.
	*/
	imtauth::IContractInfoUniquePtr ReadPartyContract(const QByteArray& contractId, const QByteArray& tenantId) const;

	bool CreateDraft(const sdl::V1_0::imtauth::CContractDraftInput& input, imtauth::IContractInfo& draft, QString& errorMessage) const;
	sdl::V1_0::imtauth::CContract CreateContractData(const imtauth::IContractInfo& contract, const QByteArray& languageId) const;
	sdl::V1_0::imtauth::CContractListItem CreateListItem(const imtauth::IContractInfo& contract, const QByteArray& tenantId) const;
	QString GetRoleName(const QByteArray& roleId, const QString& storedName, const QByteArray& languageId) const;

private:
	I_REF(imtauth::IContractManager, m_contractManagerCompPtr);
	I_FACT(imtauth::IContractInfo, m_contractFactoryCompPtr);
	I_REF(imtauth::ITenantManager, m_tenantManagerCompPtr);
	I_REF(imtauth::ITenantMembershipManager, m_membershipManagerCompPtr);
	I_REF(imtauth::ITenantRelationshipProvider, m_relationshipProviderCompPtr);
	I_REF(imtauth::ITenantConnectionRequestManager, m_connectionManagerCompPtr);
	I_REF(imtauth::IPartyRoleCatalog, m_roleCatalogCompPtr);
	I_REF(imtchat::IAttachmentStorage, m_attachmentStorageCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
	I_ATTR(int, m_maxAttachmentSizeAttrPtr);
};


} // namespace imtauthgql


