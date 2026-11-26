// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CContractControllerComp.h>


// Qt includes
#include <QtCore/QCryptographicHash>

// ImtCore includes
#include <imtauth/ContractState.h>
#include <imtauth/IUserInfo.h>
#include <imtauthgql/ContractGqlUtils.h>
#include <imtauthgql/TenantAccess.h>
#include <imtgql/IGqlContext.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Contracts.h>


namespace imtauthgql
{


namespace sdlauth = sdl::V1_0::imtauth;


// protected methods

// reimplemented (sdl::V1_0::imtauth::CContractsGqlHandlerCompBase)

sdlauth::CGetContractsPayload CContractControllerComp::OnGetContracts(
			const sdlauth::CGetContractsGqlRequest& getContractsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdlauth::CGetContractsPayload response;

	const sdlauth::GetContractsRequestArguments arguments = getContractsRequest.GetRequestedArguments();
	const QByteArray tenantId = arguments.input ? arguments.input->tenantId.value_or(QByteArray()) : QByteArray();

	imtauth::ContractActor actor;
	QString accessError;
	if (!CheckAccess(gqlRequest, tenantId, s_viewContractsPermission, actor, accessError)){
		response.errorMessage = accessError;

		return response;
	}

	bool isRead = false;
	const imtauth::ContractInfoList contracts = m_contractManagerCompPtr->GetContractsOfParty(tenantId, &isRead);
	if (!isRead){
		response.errorMessage = QStringLiteral("The contracts of the organization could not be read");

		return response;
	}

	response.contracts.Emplace();
	for (const imtauth::IContractInfoSharedPtr& contractPtr : contracts){
		response.contracts->push_back(CreateListItem(*contractPtr, tenantId));
	}

	return response;
}


sdlauth::CGetContractPayload CContractControllerComp::OnGetContract(
			const sdlauth::CGetContractGqlRequest& getContractRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdlauth::CGetContractPayload response;

	const sdlauth::GetContractRequestArguments arguments = getContractRequest.GetRequestedArguments();
	const QByteArray tenantId = arguments.input ? arguments.input->tenantId.value_or(QByteArray()) : QByteArray();
	const QByteArray contractId = arguments.input ? arguments.input->contractId.value_or(QByteArray()) : QByteArray();

	imtauth::ContractActor actor;
	QString accessError;
	if (!CheckAccess(gqlRequest, tenantId, s_viewContractsPermission, actor, accessError)){
		response.errorMessage = accessError;

		return response;
	}

	imtauth::IContractInfoUniquePtr contractPtr = ReadPartyContract(contractId, tenantId);
	if (!contractPtr.IsValid()){
		response.errorMessage = QStringLiteral("The organization is not a party of the contract");

		return response;
	}

	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	const QByteArray languageId = (contextPtr != nullptr) ? contextPtr->GetLanguageId() : QByteArray();
	response.contract = CreateContractData(*contractPtr, languageId);

	imtauth::IContractPartyPrivateInfoUniquePtr privateInfoPtr = m_contractManagerCompPtr->GetPrivateInfo(contractId, tenantId);
	if (!privateInfoPtr.IsValid()){
		response.errorMessage = QStringLiteral("The records of the organization could not be read");

		return response;
	}

	sdlauth::CContractPrivateInfo privateData;
	privateData.internalNumber = privateInfoPtr->GetInternalNumber();
	privateData.responsibleUserId = privateInfoPtr->GetResponsibleUserId();
	privateData.notes = privateInfoPtr->GetNotes();
	privateData.reminderDays = privateInfoPtr->GetReminderDays();
	privateData.updatedAt = ToSdlTime(privateInfoPtr->GetUpdatedAt());
	privateData.attachments.Emplace();
	for (const imtauth::IContractInfo::Attachment& attachment : privateInfoPtr->GetAttachments()){
		sdlauth::CContractAttachment attachmentData;
		attachmentData.attachmentId = attachment.attachmentId;
		attachmentData.fileName = attachment.fileName;
		attachmentData.mimeType = attachment.mimeType;
		attachmentData.fileSize = int(attachment.size);
		attachmentData.contentHash = QString::fromUtf8(attachment.contentHash);
		attachmentData.addedByTenantId = attachment.addedByTenantId;
		attachmentData.addedByUserId = attachment.addedByUserId;
		attachmentData.addedAt = ToSdlTime(attachment.addedAt);
		attachmentData.replacesAttachmentId = attachment.replacesAttachmentId;
		attachmentData.isReplaced = attachment.isReplaced;
		attachmentData.isInternal = true;
		privateData.attachments->push_back(attachmentData);
	}

	privateData.allocationGroups.Emplace();
	for (const imtauth::IContractPartyPrivateInfo::AllocationGroups& entry : privateInfoPtr->GetAllocationGroups()){
		sdlauth::CContractAllocationGroups groupsData;
		groupsData.allocationId = entry.allocationId;
		groupsData.groupIds = ToElementList(entry.groupIds);
		privateData.allocationGroups->push_back(groupsData);
	}

	response.privateInfo = privateData;

	return response;
}


sdlauth::CGetContractEventsPayload CContractControllerComp::OnGetContractEvents(
			const sdlauth::CGetContractEventsGqlRequest& getContractEventsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdlauth::CGetContractEventsPayload response;

	const sdlauth::GetContractEventsRequestArguments arguments = getContractEventsRequest.GetRequestedArguments();
	const QByteArray tenantId = arguments.input ? arguments.input->tenantId.value_or(QByteArray()) : QByteArray();
	const QByteArray contractId = arguments.input ? arguments.input->contractId.value_or(QByteArray()) : QByteArray();

	imtauth::ContractActor actor;
	QString accessError;
	if (!CheckAccess(gqlRequest, tenantId, s_viewContractsPermission, actor, accessError)){
		response.errorMessage = accessError;

		return response;
	}

	if (!ReadPartyContract(contractId, tenantId).IsValid()){
		response.errorMessage = QStringLiteral("The organization is not a party of the contract");

		return response;
	}

	bool isRead = false;
	const imtauth::ContractEventInfoList events = m_contractManagerCompPtr->GetEvents(contractId, tenantId, &isRead);
	if (!isRead){
		response.errorMessage = QStringLiteral("The journal of the contract could not be read");

		return response;
	}

	response.events.Emplace();
	for (const imtauth::IContractEventInfoSharedPtr& eventPtr : events){
		sdlauth::CContractEvent eventData;
		eventData.id = eventPtr->GetEventId();
		eventData.eventType = ConvertEnum<sdlauth::ContractEventType>(eventPtr->GetEventType());
		eventData.tenantId = eventPtr->GetTenantId();
		imtauth::ITenantInfoUniquePtr tenantPtr = eventPtr->GetTenantId().isEmpty() ? nullptr : m_tenantManagerCompPtr->GetTenant(eventPtr->GetTenantId());
		eventData.tenantName = tenantPtr.IsValid() ? tenantPtr->GetTenantName() : QString();
		eventData.userId = eventPtr->GetUserId();
		eventData.userName = eventPtr->GetUserName();
		eventData.time = ToSdlTime(eventPtr->GetTime());
		eventData.revision = eventPtr->GetRevision();
		eventData.comment = eventPtr->GetComment();
		eventData.subjectId = eventPtr->GetSubjectId();
		eventData.isPrivate = eventPtr->IsPrivate();
		response.events->push_back(eventData);
	}

	return response;
}


sdlauth::CGetContractAttachmentPayload CContractControllerComp::OnGetContractAttachment(
			const sdlauth::CGetContractAttachmentGqlRequest& getContractAttachmentRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdlauth::CGetContractAttachmentPayload response;

	const sdlauth::GetContractAttachmentRequestArguments arguments = getContractAttachmentRequest.GetRequestedArguments();
	const QByteArray tenantId = arguments.input ? arguments.input->tenantId.value_or(QByteArray()) : QByteArray();
	const QByteArray contractId = arguments.input ? arguments.input->contractId.value_or(QByteArray()) : QByteArray();
	const QByteArray attachmentId = arguments.input ? arguments.input->attachmentId.value_or(QByteArray()) : QByteArray();

	imtauth::ContractActor actor;
	QString accessError;
	if (!CheckAccess(gqlRequest, tenantId, s_viewContractsPermission, actor, accessError)){
		response.errorMessage = accessError;

		return response;
	}

	imtauth::IContractInfoUniquePtr contractPtr = ReadPartyContract(contractId, tenantId);
	if (!contractPtr.IsValid()){
		response.errorMessage = QStringLiteral("The organization is not a party of the contract");

		return response;
	}

	// A shared attachment of the contract, or an internal one of the organization
	bool isKnown = imtauth::FindContractAttachment(contractPtr->GetAttachments(), attachmentId) >= 0;
	if (!isKnown){
		imtauth::IContractPartyPrivateInfoUniquePtr privateInfoPtr = m_contractManagerCompPtr->GetPrivateInfo(contractId, tenantId);
		isKnown = privateInfoPtr.IsValid() && imtauth::FindContractAttachment(privateInfoPtr->GetAttachments(), attachmentId) >= 0;
	}

	QByteArray data;
	QString fileName;
	QString mimeType;
	if (!isKnown || !m_attachmentStorageCompPtr->GetAttachment(attachmentId, data, fileName, mimeType)){
		response.errorMessage = QStringLiteral("The attachment is not available");

		return response;
	}

	response.fileName = fileName;
	response.mimeType = mimeType;
	response.data = QString::fromLatin1(data.toBase64());

	return response;
}


sdlauth::CGetRelatedTenantsPayload CContractControllerComp::OnGetRelatedTenants(
			const sdlauth::CGetRelatedTenantsGqlRequest& getRelatedTenantsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdlauth::CGetRelatedTenantsPayload response;

	const sdlauth::GetRelatedTenantsRequestArguments arguments = getRelatedTenantsRequest.GetRequestedArguments();
	const QByteArray tenantId = arguments.input ? arguments.input->tenantId.value_or(QByteArray()) : QByteArray();

	// Any member of an organization knows whom it is linked to; the terms need the permission to view the contracts
	QString accessError;
	if (!HasTenantIsolationAccess(*m_tenantManagerCompPtr, m_membershipManagerCompPtr.GetPtr(), gqlRequest, tenantId, accessError)){
		response.errorMessage = accessError;

		return response;
	}

	QString tenantName;
	bool isRead = false;
	const imtauth::RelatedTenantInfos relatedTenants = m_relationshipProviderCompPtr->GetRelatedTenants(tenantId, &tenantName, &isRead);
	if (!isRead){
		response.errorMessage = QStringLiteral("The contracts of the organization could not be read");

		return response;
	}

	response.tenantName = tenantName;
	response.relatedTenants.Emplace();
	for (const imtauth::RelatedTenantInfo& relatedTenant : relatedTenants){
		sdlauth::CRelatedTenant relatedData;
		relatedData.tenantId = relatedTenant.tenantId;
		relatedData.tenantName = relatedTenant.tenantName;
		relatedData.isEffective = relatedTenant.isEffective;
		relatedData.relatedContractIds = ToElementList(relatedTenant.relatedContractIds);
		relatedData.sharingContractIds = ToElementList(relatedTenant.sharingContractIds);
		relatedData.relatedSince = ToSdlTime(relatedTenant.relatedSince);
		response.relatedTenants->push_back(relatedData);
	}

	return response;
}


sdlauth::CGetContractPartyCandidatesPayload CContractControllerComp::OnGetContractPartyCandidates(
			const sdlauth::CGetContractPartyCandidatesGqlRequest& getContractPartyCandidatesRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdlauth::CGetContractPartyCandidatesPayload response;

	const sdlauth::GetContractPartyCandidatesRequestArguments arguments = getContractPartyCandidatesRequest.GetRequestedArguments();
	const QByteArray tenantId = arguments.input ? arguments.input->tenantId.value_or(QByteArray()) : QByteArray();

	imtauth::ContractActor actor;
	QString accessError;
	if (!CheckAccess(gqlRequest, tenantId, s_editContractsPermission, actor, accessError)){
		response.errorMessage = accessError;

		return response;
	}

	QList<QPair<QByteArray, sdlauth::ContractPartyChannel>> candidates;
	auto addCandidate = [&candidates, &tenantId](const QByteArray& candidateId, sdlauth::ContractPartyChannel channel){
		if (candidateId.isEmpty() || candidateId == tenantId){
			return;
		}

		for (const QPair<QByteArray, sdlauth::ContractPartyChannel>& candidate : std::as_const(candidates)){
			if (candidate.first == candidateId){
				return;
			}
		}

		candidates.append(qMakePair(candidateId, channel));
	};

	for (const QByteArray& connectionId : m_connectionManagerCompPtr->GetConnectionIds(tenantId)){
		imtauth::ITenantConnectionInfoUniquePtr connectionPtr = m_connectionManagerCompPtr->GetConnection(connectionId);
		if (connectionPtr.IsValid()){
			addCandidate((connectionPtr->GetTenantAId() == tenantId) ? connectionPtr->GetTenantBId() : connectionPtr->GetTenantAId(), sdlauth::ContractPartyChannel::AddressBook);
		}
	}

	const QDateTime now = QDateTime::currentDateTimeUtc();
	for (const imtauth::IContractInfoSharedPtr& contractPtr : m_contractManagerCompPtr->GetContractsOfParty(tenantId)){
		const imtauth::ContractState state = imtauth::ComputeContractState(*contractPtr, now).state;
		if (state == imtauth::CS_SIGNED || state == imtauth::CS_ACTIVE){
			for (const QByteArray& partyId : imtauth::GetContractPartyTenantIds(*contractPtr)){
				addCandidate(partyId, sdlauth::ContractPartyChannel::CoParty);
			}
		}
	}

	response.candidates.Emplace();
	for (const QPair<QByteArray, sdlauth::ContractPartyChannel>& candidate : std::as_const(candidates)){
		imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(candidate.first);
		if (!tenantPtr.IsValid() || !tenantPtr->IsActive()){
			continue;
		}

		sdlauth::CContractPartyCandidate candidateData;
		candidateData.tenantId = candidate.first;
		candidateData.tenantName = tenantPtr->GetTenantName();
		candidateData.channel = candidate.second;
		response.candidates->push_back(candidateData);
	}

	return response;
}


sdlauth::CContractCommandPayload CContractControllerComp::OnCreateContract(
			const sdlauth::CCreateContractGqlRequest& createContractRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdlauth::CContractCommandPayload response;
	response.success = false;

	const sdlauth::CreateContractRequestArguments arguments = createContractRequest.GetRequestedArguments();
	const QByteArray tenantId = arguments.input ? arguments.input->tenantId.value_or(QByteArray()) : QByteArray();

	imtauth::ContractActor actor;
	QString error;
	if (!CheckAccess(gqlRequest, tenantId, s_editContractsPermission, actor, error)){
		response.errorMessage = error;

		return response;
	}

	imtauth::IContractInfoUniquePtr draftPtr = m_contractFactoryCompPtr.CreateInstance();
	if (!draftPtr.IsValid() || !arguments.input->draft || !CreateDraft(*arguments.input->draft, *draftPtr, error)){
		response.errorMessage = error.isEmpty() ? QStringLiteral("The draft is missing") : error;

		return response;
	}

	const QByteArray contractId = m_contractManagerCompPtr->CreateContract(actor, *draftPtr, error);
	if (contractId.isEmpty()){
		response.errorMessage = error;

		return response;
	}

	imtauth::IContractInfoUniquePtr contractPtr = m_contractManagerCompPtr->GetContract(contractId);
	response.success = true;
	response.contractId = contractId;
	response.revision = contractPtr.IsValid() ? contractPtr->GetRevision() : 0;

	return response;
}


sdlauth::CContractCommandPayload CContractControllerComp::OnUpdateContract(
			const sdlauth::CUpdateContractGqlRequest& updateContractRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	const sdlauth::UpdateContractRequestArguments arguments = updateContractRequest.GetRequestedArguments();
	if (!arguments.input || !arguments.input->draft){
		sdlauth::CContractCommandPayload response;
		response.success = false;
		response.errorMessage = QStringLiteral("The draft is missing");

		return response;
	}

	const sdlauth::CUpdateContractInput& input = *arguments.input;
	const QByteArray contractId = input.contractId.value_or(QByteArray());

	return RunCommand(gqlRequest, input.tenantId.value_or(QByteArray()), contractId, s_editContractsPermission,
				[&](const imtauth::ContractActor& actor, QString& error){
		imtauth::IContractInfoUniquePtr draftPtr = m_contractFactoryCompPtr.CreateInstance();
		if (!draftPtr.IsValid() || !CreateDraft(*input.draft, *draftPtr, error)){
			return false;
		}

		draftPtr->SetContractId(contractId);

		return m_contractManagerCompPtr->UpdateContract(actor, *draftPtr, input.revision.value_or(0), error);
	});
}


sdlauth::CContractCommandPayload CContractControllerComp::OnAddContractPartyByCode(
			const sdlauth::CAddContractPartyByCodeGqlRequest& addContractPartyByCodeRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	const sdlauth::AddContractPartyByCodeRequestArguments arguments = addContractPartyByCodeRequest.GetRequestedArguments();
	const sdlauth::CAddContractPartyByCodeInput input = arguments.input ? *arguments.input : sdlauth::CAddContractPartyByCodeInput();
	const QByteArray contractId = input.contractId.value_or(QByteArray());

	return RunCommand(gqlRequest, input.tenantId.value_or(QByteArray()), contractId, s_editContractsPermission,
				[&](const imtauth::ContractActor& actor, QString& error){
		return m_contractManagerCompPtr->AddPartyByCode(
					actor,
					contractId,
					input.revision.value_or(0),
					input.connectionCode.value_or(QString()),
					input.roleId.value_or(QByteArray()),
					error);
	});
}


#define IMT_CONTRACT_COMMAND_INPUT(RequestClass, ArgumentsClass, request) \
	const sdlauth::ArgumentsClass arguments = request.GetRequestedArguments(); \
	const sdlauth::CContractCommandInput input = arguments.input ? *arguments.input : sdlauth::CContractCommandInput(); \
	const QByteArray tenantId = input.tenantId.value_or(QByteArray()); \
	const QByteArray contractId = input.contractId.value_or(QByteArray()); \
	const int revision = input.revision.value_or(0); \
	const QString comment = input.comment.value_or(QString()); \
	Q_UNUSED(revision); \
	Q_UNUSED(comment);


sdlauth::CContractCommandPayload CContractControllerComp::OnRemoveContract(
			const sdlauth::CRemoveContractGqlRequest& removeContractRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	IMT_CONTRACT_COMMAND_INPUT(CRemoveContractGqlRequest, RemoveContractRequestArguments, removeContractRequest)

	return RunCommand(gqlRequest, tenantId, QByteArray(), s_editContractsPermission, [&](const imtauth::ContractActor& actor, QString& error){
		return m_contractManagerCompPtr->RemoveContract(actor, contractId, error);
	});
}


sdlauth::CContractCommandPayload CContractControllerComp::OnSubmitContract(
			const sdlauth::CSubmitContractGqlRequest& submitContractRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	IMT_CONTRACT_COMMAND_INPUT(CSubmitContractGqlRequest, SubmitContractRequestArguments, submitContractRequest)

	return RunCommand(gqlRequest, tenantId, contractId, s_editContractsPermission, [&](const imtauth::ContractActor& actor, QString& error){
		return m_contractManagerCompPtr->SubmitContract(actor, contractId, revision, error);
	});
}


sdlauth::CContractCommandPayload CContractControllerComp::OnCancelContract(
			const sdlauth::CCancelContractGqlRequest& cancelContractRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	IMT_CONTRACT_COMMAND_INPUT(CCancelContractGqlRequest, CancelContractRequestArguments, cancelContractRequest)

	return RunCommand(gqlRequest, tenantId, contractId, s_editContractsPermission, [&](const imtauth::ContractActor& actor, QString& error){
		return m_contractManagerCompPtr->CancelContract(actor, contractId, comment, error);
	});
}


sdlauth::CContractCommandPayload CContractControllerComp::OnRequestContractChanges(
			const sdlauth::CRequestContractChangesGqlRequest& requestContractChangesRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	IMT_CONTRACT_COMMAND_INPUT(CRequestContractChangesGqlRequest, RequestContractChangesRequestArguments, requestContractChangesRequest)

	return RunCommand(gqlRequest, tenantId, contractId, s_editContractsPermission, [&](const imtauth::ContractActor& actor, QString& error){
		return m_contractManagerCompPtr->RequestChanges(actor, contractId, revision, comment, error);
	});
}


sdlauth::CContractCommandPayload CContractControllerComp::OnApproveContract(
			const sdlauth::CApproveContractGqlRequest& approveContractRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	IMT_CONTRACT_COMMAND_INPUT(CApproveContractGqlRequest, ApproveContractRequestArguments, approveContractRequest)

	return RunCommand(gqlRequest, tenantId, contractId, s_approveContractsPermission, [&](const imtauth::ContractActor& actor, QString& error){
		return m_contractManagerCompPtr->ApproveContract(actor, contractId, revision, error);
	});
}


sdlauth::CContractCommandPayload CContractControllerComp::OnSignContract(
			const sdlauth::CSignContractGqlRequest& signContractRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	const sdlauth::SignContractRequestArguments arguments = signContractRequest.GetRequestedArguments();
	const sdlauth::CSignContractInput input = arguments.input ? *arguments.input : sdlauth::CSignContractInput();
	const QByteArray contractId = input.contractId.value_or(QByteArray());

	imtauth::ContractSignatureRequest signature;
	signature.position = input.position.value_or(QString());

	return RunCommand(gqlRequest, input.tenantId.value_or(QByteArray()), contractId, s_signContractsPermission, [&](const imtauth::ContractActor& actor, QString& error){
		return m_contractManagerCompPtr->SignContract(actor, contractId, input.revision.value_or(0), signature, error);
	});
}


sdlauth::CContractCommandPayload CContractControllerComp::OnDeclineContract(
			const sdlauth::CDeclineContractGqlRequest& declineContractRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	IMT_CONTRACT_COMMAND_INPUT(CDeclineContractGqlRequest, DeclineContractRequestArguments, declineContractRequest)

	return RunCommand(gqlRequest, tenantId, contractId, s_signContractsPermission, [&](const imtauth::ContractActor& actor, QString& error){
		return m_contractManagerCompPtr->DeclineContract(actor, contractId, comment, error);
	});
}


sdlauth::CContractCommandPayload CContractControllerComp::OnGiveContractNotice(
			const sdlauth::CGiveContractNoticeGqlRequest& giveContractNoticeRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	const sdlauth::GiveContractNoticeRequestArguments arguments = giveContractNoticeRequest.GetRequestedArguments();
	const sdlauth::CGiveContractNoticeInput input = arguments.input ? *arguments.input : sdlauth::CGiveContractNoticeInput();
	const QByteArray contractId = input.contractId.value_or(QByteArray());

	return RunCommand(gqlRequest, input.tenantId.value_or(QByteArray()), contractId, s_signContractsPermission, [&](const imtauth::ContractActor& actor, QString& error){
		return m_contractManagerCompPtr->GiveNotice(actor, contractId, input.reason.value_or(QString()), input.isImmediate.value_or(false), error);
	});
}


sdlauth::CContractCommandPayload CContractControllerComp::OnRevokeContractNotice(
			const sdlauth::CRevokeContractNoticeGqlRequest& revokeContractNoticeRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	IMT_CONTRACT_COMMAND_INPUT(CRevokeContractNoticeGqlRequest, RevokeContractNoticeRequestArguments, revokeContractNoticeRequest)

	return RunCommand(gqlRequest, tenantId, contractId, s_signContractsPermission, [&](const imtauth::ContractActor& actor, QString& error){
		return m_contractManagerCompPtr->RevokeNotice(actor, contractId, error);
	});
}


sdlauth::CContractCommandPayload CContractControllerComp::OnGiveContractNonRenewalNotice(
			const sdlauth::CGiveContractNonRenewalNoticeGqlRequest& giveContractNonRenewalNoticeRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	IMT_CONTRACT_COMMAND_INPUT(CGiveContractNonRenewalNoticeGqlRequest, GiveContractNonRenewalNoticeRequestArguments, giveContractNonRenewalNoticeRequest)

	return RunCommand(gqlRequest, tenantId, contractId, s_signContractsPermission, [&](const imtauth::ContractActor& actor, QString& error){
		return m_contractManagerCompPtr->GiveNonRenewalNotice(actor, contractId, error);
	});
}


sdlauth::CContractCommandPayload CContractControllerComp::OnSetContractAllocationsSuspended(
			const sdlauth::CSetContractAllocationsSuspendedGqlRequest& setContractAllocationsSuspendedRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	const sdlauth::SetContractAllocationsSuspendedRequestArguments arguments = setContractAllocationsSuspendedRequest.GetRequestedArguments();
	const sdlauth::CSetContractAllocationsSuspendedInput input = arguments.input ? *arguments.input : sdlauth::CSetContractAllocationsSuspendedInput();
	const QByteArray contractId = input.contractId.value_or(QByteArray());

	return RunCommand(gqlRequest, input.tenantId.value_or(QByteArray()), contractId, s_suspendContractPermissionsPermission, [&](const imtauth::ContractActor& actor, QString& error){
		return m_contractManagerCompPtr->SetAllocationsSuspended(actor, contractId, input.isSuspended.value_or(false), error);
	});
}


sdlauth::CContractCommandPayload CContractControllerComp::OnAddContractAttachment(
			const sdlauth::CAddContractAttachmentGqlRequest& addContractAttachmentRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	const sdlauth::AddContractAttachmentRequestArguments arguments = addContractAttachmentRequest.GetRequestedArguments();
	const sdlauth::CAddContractAttachmentInput input = arguments.input ? *arguments.input : sdlauth::CAddContractAttachmentInput();
	const QByteArray contractId = input.contractId.value_or(QByteArray());

	return RunCommand(gqlRequest, input.tenantId.value_or(QByteArray()), contractId, s_editContractsPermission, [&](const imtauth::ContractActor& actor, QString& error){
		const QByteArray data = QByteArray::fromBase64(input.data.value_or(QString()).toLatin1());
		if (data.isEmpty() || data.size() > *m_maxAttachmentSizeAttrPtr){
			error = QStringLiteral("An attachment is a file of at most %1 bytes").arg(*m_maxAttachmentSizeAttrPtr);

			return false;
		}

		// The file is checked as a party first, so no file of an outsider is stored
		if (!ReadPartyContract(contractId, actor.tenantId).IsValid()){
			error = QStringLiteral("The organization is not a party of the contract");

			return false;
		}

		imtauth::IContractInfo::Attachment attachment;
		attachment.fileName = input.fileName.value_or(QString()).trimmed();
		attachment.mimeType = input.mimeType.value_or(QString());
		attachment.size = data.size();
		attachment.contentHash = QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
		attachment.replacesAttachmentId = input.replacesAttachmentId.value_or(QByteArray());
		attachment.attachmentId = m_attachmentStorageCompPtr->StoreAttachment(data, attachment.fileName, attachment.mimeType);
		if (attachment.attachmentId.isEmpty()){
			error = QStringLiteral("The file could not be stored");

			return false;
		}

		if (!m_contractManagerCompPtr->AddAttachment(actor, contractId, attachment, input.isInternal.value_or(false), error)){
			m_attachmentStorageCompPtr->DeleteAttachment(attachment.attachmentId);

			return false;
		}

		return true;
	});
}


sdlauth::CContractCommandPayload CContractControllerComp::OnSetContractPrivateInfo(
			const sdlauth::CSetContractPrivateInfoGqlRequest& setContractPrivateInfoRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	const sdlauth::SetContractPrivateInfoRequestArguments arguments = setContractPrivateInfoRequest.GetRequestedArguments();
	const sdlauth::CSetContractPrivateInfoInput input = arguments.input ? *arguments.input : sdlauth::CSetContractPrivateInfoInput();
	const QByteArray contractId = input.contractId.value_or(QByteArray());

	return RunCommand(gqlRequest, input.tenantId.value_or(QByteArray()), contractId, s_editContractsPermission, [&](const imtauth::ContractActor& actor, QString& error){
		imtauth::IContractPartyPrivateInfoUniquePtr privateInfoPtr = m_privateInfoFactoryCompPtr.CreateInstance();
		if (!privateInfoPtr.IsValid()){
			error = QStringLiteral("The records could not be created");

			return false;
		}

		privateInfoPtr->SetContractId(contractId);
		privateInfoPtr->SetTenantId(actor.tenantId);
		privateInfoPtr->SetInternalNumber(input.internalNumber.value_or(QString()));
		privateInfoPtr->SetResponsibleUserId(input.responsibleUserId.value_or(QByteArray()));
		privateInfoPtr->SetNotes(input.notes.value_or(QString()));
		privateInfoPtr->SetReminderDays(input.reminderDays.value_or(0));

		return m_contractManagerCompPtr->SetPrivateInfo(actor, *privateInfoPtr, error);
	});
}


sdlauth::CContractCommandPayload CContractControllerComp::OnSetContractAllocationGroups(
			const sdlauth::CSetContractAllocationGroupsGqlRequest& setContractAllocationGroupsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	const sdlauth::SetContractAllocationGroupsRequestArguments arguments = setContractAllocationGroupsRequest.GetRequestedArguments();
	const sdlauth::CSetContractAllocationGroupsInput input = arguments.input ? *arguments.input : sdlauth::CSetContractAllocationGroupsInput();
	const QByteArray contractId = input.contractId.value_or(QByteArray());

	return RunCommand(gqlRequest, input.tenantId.value_or(QByteArray()), contractId, s_distributeContractPermissionsPermission, [&](const imtauth::ContractActor& actor, QString& error){
		return m_contractManagerCompPtr->SetAllocationGroups(actor, contractId, input.allocationId.value_or(QByteArray()), FromElementList(input.groupIds), error);
	});
}


// private methods

bool CContractControllerComp::CheckAccess(
			const ::imtgql::CGqlRequest& gqlRequest,
			const QByteArray& tenantId,
			const QByteArray& permission,
			imtauth::ContractActor& actor,
			QString& errorMessage) const
{
	if (tenantId.isEmpty()){
		errorMessage = QStringLiteral("A contract is handled on behalf of an organization");

		return false;
	}

	if (!HasTenantManageAccess(*m_tenantManagerCompPtr, m_membershipManagerCompPtr.GetPtr(), gqlRequest, tenantId, {permission}, errorMessage)){
		return false;
	}

	const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
	actor.tenantId = tenantId;
	actor.userId = (contextPtr != nullptr) ? contextPtr->GetUserId() : QByteArray();
	const imtauth::IUserInfo* userInfoPtr = (contextPtr != nullptr) ? contextPtr->GetUserInfo() : nullptr;
	actor.userName = (userInfoPtr != nullptr) ? userInfoPtr->GetName() : QString();

	return true;
}


sdlauth::CContractCommandPayload CContractControllerComp::RunCommand(
			const ::imtgql::CGqlRequest& gqlRequest,
			const QByteArray& tenantId,
			const QByteArray& contractId,
			const QByteArray& permission,
			const ContractCommand& command) const
{
	sdlauth::CContractCommandPayload response;
	response.success = false;

	imtauth::ContractActor actor;
	QString error;
	if (!CheckAccess(gqlRequest, tenantId, permission, actor, error) || !command(actor, error)){
		response.errorMessage = error;

		return response;
	}

	response.success = true;
	if (!contractId.isEmpty()){
		response.contractId = contractId;
		imtauth::IContractInfoUniquePtr contractPtr = m_contractManagerCompPtr->GetContract(contractId);
		if (contractPtr.IsValid()){
			response.revision = contractPtr->GetRevision();
		}
	}

	return response;
}


imtauth::IContractInfoUniquePtr CContractControllerComp::ReadPartyContract(const QByteArray& contractId, const QByteArray& tenantId) const
{
	imtauth::IContractInfoUniquePtr contractPtr = m_contractManagerCompPtr->GetContract(contractId);
	if (!contractPtr.IsValid() || imtauth::FindContractParty(contractPtr->GetParties(), tenantId) < 0){
		return nullptr;
	}

	// The draft is the business of its initiator
	if (!contractPtr->GetSubmittedAt().isValid() && contractPtr->GetInitiatorTenantId() != tenantId){
		return nullptr;
	}

	return contractPtr;
}


bool CContractControllerComp::CreateDraft(const sdlauth::CContractDraftInput& input, imtauth::IContractInfo& draft, QString& /*errorMessage*/) const
{
	draft.SetName(input.name.value_or(QString()));
	draft.SetDescription(input.description.value_or(QString()));
	draft.SetTerms(input.terms.value_or(QString()));
	draft.SetTypeId(input.typeId.value_or(QByteArray()));
	draft.SetEffectiveFrom(FromSdlTime(input.effectiveFrom));
	draft.SetTermDays(input.termDays.value_or(0));
	draft.SetValidUntil(FromSdlTime(input.validUntil));
	draft.SetAutoRenew(input.autoRenew.value_or(false));
	draft.SetRenewalPeriodDays(input.renewalPeriodDays.value_or(0));
	draft.SetRenewalNoticeDays(input.renewalNoticeDays.value_or(0));
	draft.SetTerminationNoticeDays(input.terminationNoticeDays.value_or(0));

	imtauth::IContractInfo::Parties parties;
	if (input.parties){
		for (const istd::TNullableValue<sdlauth::CContractPartyInput>& item : *input.parties){
			if (!item){
				continue;
			}

			imtauth::IContractInfo::Party party;
			party.tenantId = item->tenantId.value_or(QByteArray());
			party.roleId = item->roleId.value_or(QByteArray());
			parties.append(party);
		}
	}

	draft.SetParties(parties);

	imtauth::IContractInfo::Links links;
	if (input.links){
		for (const istd::TNullableValue<sdlauth::CContractLinkInput>& item : *input.links){
			if (!item){
				continue;
			}

			imtauth::IContractInfo::Link link;
			link.linkId = item->linkId.value_or(QByteArray());
			link.tenantAId = item->tenantAId.value_or(QByteArray());
			link.tenantBId = item->tenantBId.value_or(QByteArray());
			link.roleAId = item->roleAId.value_or(QByteArray());
			link.roleBId = item->roleBId.value_or(QByteArray());
			link.validFrom = FromSdlTime(item->validFrom);
			link.validUntil = FromSdlTime(item->validUntil);
			link.isMessagingAllowed = item->isMessagingAllowed.value_or(true);
			link.isSharingAllowed = item->isSharingAllowed.value_or(true);
			links.append(link);
		}
	}

	draft.SetLinks(links);

	imtauth::IContractInfo::Allocations allocations;
	if (input.allocations){
		for (const istd::TNullableValue<sdlauth::CContractAllocationInput>& item : *input.allocations){
			if (!item){
				continue;
			}

			imtauth::IContractInfo::Allocation allocation;
			allocation.allocationId = item->allocationId.value_or(QByteArray());
			allocation.linkId = item->linkId.value_or(QByteArray());
			allocation.grantorTenantId = item->grantorTenantId.value_or(QByteArray());
			allocation.granteeTenantId = item->granteeTenantId.value_or(QByteArray());
			allocation.productId = item->productId.value_or(QByteArray());
			allocation.permissionIds = FromElementList(item->permissionIds);
			allocation.validFrom = FromSdlTime(item->validFrom);
			allocation.validUntil = FromSdlTime(item->validUntil);
			allocation.comment = item->comment.value_or(QString());
			allocations.append(allocation);
		}
	}

	draft.SetAllocations(allocations);

	imtauth::IContractInfo::References references;
	if (input.references){
		for (const istd::TNullableValue<sdlauth::CContractReferenceInput>& item : *input.references){
			if (!item){
				continue;
			}

			imtauth::IContractInfo::Reference reference;
			reference.contractId = item->contractId.value_or(QByteArray());
			reference.type = item->referenceType ? ConvertEnum<imtauth::IContractInfo::ReferenceType>(*item->referenceType) : imtauth::IContractInfo::RT_RELATED;
			references.append(reference);
		}
	}

	draft.SetReferences(references);

	return true;
}


sdlauth::CContract CContractControllerComp::CreateContractData(const imtauth::IContractInfo& contract, const QByteArray& languageId) const
{
	const QDateTime now = QDateTime::currentDateTimeUtc();
	const imtauth::ContractStateInfo stateInfo = imtauth::ComputeContractState(contract, now);

	sdlauth::CContract retVal;
	retVal.id = contract.GetContractId();
	retVal.number = contract.GetNumber();
	retVal.name = contract.GetName();
	retVal.description = contract.GetDescription();
	retVal.terms = contract.GetTerms();
	retVal.typeId = contract.GetTypeId();
	retVal.typeName = contract.GetTypeName();
	retVal.initiatorTenantId = contract.GetInitiatorTenantId();
	retVal.createdByUserId = contract.GetCreatedByUserId();
	retVal.effectiveFrom = ToSdlTime(contract.GetEffectiveFrom());
	retVal.termDays = contract.GetTermDays();
	retVal.validUntil = ToSdlTime(contract.GetValidUntil());
	retVal.autoRenew = contract.IsAutoRenew();
	retVal.renewalPeriodDays = contract.GetRenewalPeriodDays();
	retVal.renewalNoticeDays = contract.GetRenewalNoticeDays();
	retVal.terminationNoticeDays = contract.GetTerminationNoticeDays();

	retVal.parties.Emplace();
	for (const imtauth::IContractInfo::Party& party : contract.GetParties()){
		sdlauth::CContractParty partyData;
		partyData.tenantId = party.tenantId;
		partyData.tenantName = party.tenantName;
		partyData.roleId = party.roleId;
		partyData.roleName = GetRoleName(party.roleId, party.roleName, languageId);
		partyData.channel = ConvertEnum<sdlauth::ContractPartyChannel>(party.channel);
		partyData.state = ConvertEnum<sdlauth::ContractPartyState>(party.state);
		partyData.stateComment = party.stateComment;
		partyData.stateChangedAt = ToSdlTime(party.stateChangedAt);
		partyData.withdrawAt = ToSdlTime(party.withdrawAt);
		partyData.withdrawReason = party.withdrawReason;
		partyData.nonRenewalNoticeAt = ToSdlTime(party.nonRenewalNoticeAt);
		partyData.allocationsSuspended = party.allocationsSuspended;
		partyData.isBound = imtauth::IsPartyBoundAt(contract, party, now);
		partyData.signatures.Emplace();
		for (const imtauth::IContractInfo::Signature& signature : party.signatures){
			sdlauth::CContractSignature signatureData;
			signatureData.userId = signature.userId;
			signatureData.userName = signature.userName;
			signatureData.position = signature.position;
			signatureData.signedAt = ToSdlTime(signature.signedAt);
			signatureData.revision = signature.revision;
			signatureData.contentHash = QString::fromUtf8(signature.contentHash);
			partyData.signatures->push_back(signatureData);
		}

		retVal.parties->push_back(partyData);
	}

	retVal.links.Emplace();
	for (const imtauth::IContractInfo::Link& link : contract.GetLinks()){
		sdlauth::CContractLink linkData;
		linkData.linkId = link.linkId;
		linkData.tenantAId = link.tenantAId;
		linkData.tenantBId = link.tenantBId;
		linkData.roleAId = link.roleAId;
		linkData.roleAName = GetRoleName(link.roleAId, link.roleAName, languageId);
		linkData.roleBId = link.roleBId;
		linkData.roleBName = GetRoleName(link.roleBId, link.roleBName, languageId);
		linkData.validFrom = ToSdlTime(link.validFrom);
		linkData.validUntil = ToSdlTime(link.validUntil);
		linkData.isMessagingAllowed = link.isMessagingAllowed;
		linkData.isSharingAllowed = link.isSharingAllowed;
		linkData.isInForce = imtauth::IsLinkInForce(contract, link, now);
		retVal.links->push_back(linkData);
	}

	retVal.allocations.Emplace();
	for (const imtauth::IContractInfo::Allocation& allocation : contract.GetAllocations()){
		sdlauth::CContractAllocation allocationData;
		allocationData.allocationId = allocation.allocationId;
		allocationData.linkId = allocation.linkId;
		allocationData.grantorTenantId = allocation.grantorTenantId;
		allocationData.granteeTenantId = allocation.granteeTenantId;
		allocationData.productId = allocation.productId;
		allocationData.permissionIds = ToElementList(allocation.permissionIds);
		allocationData.validFrom = ToSdlTime(allocation.validFrom);
		allocationData.validUntil = ToSdlTime(allocation.validUntil);
		allocationData.comment = allocation.comment;
		allocationData.isInForce = imtauth::IsAllocationInForce(contract, allocation, now);
		retVal.allocations->push_back(allocationData);
	}

	retVal.references.Emplace();
	for (const imtauth::IContractInfo::Reference& reference : contract.GetReferences()){
		sdlauth::CContractReference referenceData;
		referenceData.contractId = reference.contractId;
		referenceData.referenceType = ConvertEnum<sdlauth::ContractReferenceType>(reference.type);
		referenceData.contractNumber = reference.contractNumber;
		referenceData.contractName = reference.contractName;
		retVal.references->push_back(referenceData);
	}

	retVal.attachments.Emplace();
	for (const imtauth::IContractInfo::Attachment& attachment : contract.GetAttachments()){
		sdlauth::CContractAttachment attachmentData;
		attachmentData.attachmentId = attachment.attachmentId;
		attachmentData.fileName = attachment.fileName;
		attachmentData.mimeType = attachment.mimeType;
		attachmentData.fileSize = int(attachment.size);
		attachmentData.contentHash = QString::fromUtf8(attachment.contentHash);
		attachmentData.addedByTenantId = attachment.addedByTenantId;
		attachmentData.addedByUserId = attachment.addedByUserId;
		attachmentData.addedAt = ToSdlTime(attachment.addedAt);
		attachmentData.replacesAttachmentId = attachment.replacesAttachmentId;
		attachmentData.isReplaced = attachment.isReplaced;
		attachmentData.isInternal = false;
		retVal.attachments->push_back(attachmentData);
	}

	retVal.roleSlots.Emplace();
	for (const imtauth::IContractInfo::RoleSlot& slot : contract.GetRoleSlots()){
		sdlauth::CContractRoleSlot slotData;
		slotData.roleId = slot.roleId;
		slotData.minCount = slot.minCount;
		slotData.maxCount = slot.maxCount;
		retVal.roleSlots->push_back(slotData);
	}

	retVal.revision = contract.GetRevision();
	retVal.contentHash = QString::fromUtf8(contract.GetContentHash());
	retVal.submittedAt = ToSdlTime(contract.GetSubmittedAt());
	retVal.cancelledAt = ToSdlTime(contract.GetCancelledAt());
	retVal.signedAt = ToSdlTime(contract.GetSignedAt());
	retVal.terminatedAt = ToSdlTime(contract.GetTerminatedAt());
	retVal.terminationReason = contract.GetTerminationReason();

	retVal.supersededBy = contract.GetSupersededBy();
	retVal.supersededAt = ToSdlTime(contract.GetSupersededAt());
	retVal.createdAt = ToSdlTime(contract.GetCreatedAt());
	retVal.updatedAt = ToSdlTime(contract.GetUpdatedAt());

	retVal.state = ConvertEnum<sdlauth::ContractState>(stateInfo.state);
	retVal.isChangesRequested = (stateInfo.flags & imtauth::CSF_CHANGES_REQUESTED) != 0;
	retVal.isTerminating = (stateInfo.flags & imtauth::CSF_TERMINATING) != 0;
	retVal.isExpiring = (stateInfo.flags & imtauth::CSF_EXPIRING) != 0;
	retVal.isPartiallySuspended = (stateInfo.flags & imtauth::CSF_PARTIALLY_SUSPENDED) != 0;
	retVal.effectiveStart = ToSdlTime(stateInfo.effectiveStart);
	retVal.periodEnd = ToSdlTime(stateInfo.periodEnd);
	retVal.endsAt = ToSdlTime(stateInfo.endsAt);
	retVal.endedAt = ToSdlTime(stateInfo.endedAt);

	return retVal;
}


sdlauth::CContractListItem CContractControllerComp::CreateListItem(const imtauth::IContractInfo& contract, const QByteArray& tenantId) const
{
	const imtauth::ContractStateInfo stateInfo = imtauth::ComputeContractState(contract, QDateTime::currentDateTimeUtc());
	const imtauth::IContractInfo::Parties parties = contract.GetParties();

	sdlauth::CContractListItem retVal;
	retVal.id = contract.GetContractId();
	retVal.number = contract.GetNumber();
	retVal.name = contract.GetName();
	retVal.typeName = contract.GetTypeName();
	retVal.initiatorTenantId = contract.GetInitiatorTenantId();
	retVal.state = ConvertEnum<sdlauth::ContractState>(stateInfo.state);
	retVal.isChangesRequested = (stateInfo.flags & imtauth::CSF_CHANGES_REQUESTED) != 0;
	retVal.isTerminating = (stateInfo.flags & imtauth::CSF_TERMINATING) != 0;
	retVal.isExpiring = (stateInfo.flags & imtauth::CSF_EXPIRING) != 0;
	retVal.effectiveStart = ToSdlTime(stateInfo.effectiveStart);
	retVal.periodEnd = ToSdlTime(stateInfo.periodEnd);
	retVal.endedAt = ToSdlTime(stateInfo.endedAt);
	retVal.updatedAt = ToSdlTime(contract.GetUpdatedAt());

	QStringList partyNames;
	for (const imtauth::IContractInfo::Party& party : parties){
		if (party.tenantId != tenantId){
			partyNames.append(party.tenantName);
		}
	}

	retVal.partyNames = ToElementList(QList<QString>(partyNames));

	const int partyIndex = imtauth::FindContractParty(parties, tenantId);
	bool isMyTurn = false;
	if (partyIndex >= 0){
		const imtauth::IContractInfo::Party& party = parties[partyIndex];
		retVal.partyState = ConvertEnum<sdlauth::ContractPartyState>(party.state);

		const bool isNegotiated = stateInfo.state == imtauth::CS_NEGOTIATION || stateInfo.state == imtauth::CS_SIGNING;
		isMyTurn = isNegotiated && (party.state == imtauth::IContractInfo::PS_REVIEWING || party.state == imtauth::IContractInfo::PS_APPROVED);
	}

	retVal.isMyTurnToSign = isMyTurn;

	return retVal;
}


QString CContractControllerComp::GetRoleName(const QByteArray& roleId, const QString& storedName, const QByteArray& languageId) const
{
	// The names of the system roles are texts of the .acc files
	if (roleId.isEmpty() || !m_translationManagerCompPtr.IsValid()){
		return storedName;
	}

	imtauth::IPartyRoleInfoUniquePtr rolePtr = m_roleCatalogCompPtr->GetRole(roleId);
	if (!rolePtr.IsValid() || !rolePtr->GetOwnerTenantId().isEmpty()){
		return storedName;
	}

	return TranslateSystemText(m_translationManagerCompPtr.GetPtr(), rolePtr->GetName(), languageId);
}


} // namespace imtauthgql


