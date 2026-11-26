// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CContractManagerComp.h>


// Qt includes
#include <QtCore/QSet>
#include <QtCore/QUuid>

// STL includes
#include <algorithm>
#include <utility>

// ACF includes
#include <ilog/CMessageContainer.h>
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtauth/CollectionSelection.h>
#include <imtauth/ContractState.h>
#include <imtauth/TenantEvents.h>


namespace imtauth
{


namespace
{


static const QByteArray s_groupEntityType = QByteArrayLiteral("UserGroups");
static const QByteArray s_partyFilterParamId = QByteArrayLiteral("PartyTenantId");
static const QByteArray s_statesFilterParamId = QByteArrayLiteral("ContractStates");


QByteArray CreateId()
{
	return QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
}


QByteArray GetPrivateInfoKey(const QByteArray& contractId, const QByteArray& tenantId)
{
	return contractId + '.' + tenantId;
}


bool IsConcludedState(ContractState state)
{
	return state == CS_SIGNED || state == CS_ACTIVE;
}


bool IsNegotiatedState(ContractState state)
{
	return state == CS_NEGOTIATION || state == CS_SIGNING;
}


bool IsUnknownChannel(IContractInfo::PartyChannel channel)
{
	return channel == IContractInfo::PC_CONNECTION_CODE || channel == IContractInfo::PC_DIRECTORY;
}


QByteArrayList GetGranteeTenantIds(const IContractInfo& contract, const QByteArray& grantorTenantId)
{
	QByteArrayList retVal;
	for (const IContractInfo::Allocation& allocation : contract.GetAllocations()){
		if ((grantorTenantId.isEmpty() || allocation.grantorTenantId == grantorTenantId) && !retVal.contains(allocation.granteeTenantId)){
			retVal.append(allocation.granteeTenantId);
		}
	}

	return retVal;
}


} // anonymous namespace


// reimplemented (imtauth::IContractProvider)

IContractInfoUniquePtr CContractManagerComp::GetContract(const QByteArray& contractId) const
{
	return ReadContract(contractId);
}


ContractInfoList CContractManagerComp::GetContractsOfParty(const QByteArray& tenantId, bool* isReadPtr) const
{
	ContractInfoList retVal;
	if (isReadPtr != nullptr){
		*isReadPtr = false;
	}

	if (tenantId.isEmpty()){
		return retVal;
	}

	QByteArrayList contractIds;
	bool isCached = false;
	quint64 generation = 0;
	{
		QMutexLocker locker(&m_cacheMutex);
		generation = m_cacheGeneration;
		auto cacheIter = m_partyContractIdsCache.constFind(tenantId);
		if (cacheIter != m_partyContractIdsCache.constEnd()){
			contractIds = *cacheIter;
			isCached = true;
		}
	}

	bool isRead = true;
	if (!isCached){
		iprm::CTextParam partyParam;
		partyParam.SetText(QString::fromUtf8(tenantId));
		iprm::CParamsSet filterParams;
		filterParams.SetEditableParameter(s_partyFilterParamId, &partyParam);

		ilog::CMessageContainer readErrors;
		contractIds = m_contractCollectionCompPtr->GetElementIds(0, -1, &filterParams, &readErrors);
		isRead = readErrors.GetWorstCategory() < istd::IInformationProvider::IC_ERROR;
	}

	QByteArrayList partyContractIds;
	for (const QByteArray& contractId : std::as_const(contractIds)){
		IContractInfoUniquePtr contractPtr = ReadContract(contractId);
		if (!contractPtr.IsValid()){
			isRead = false;

			continue;
		}

		// A collection that does not know the filter gives every contract
		if (FindContractParty(contractPtr->GetParties(), tenantId) < 0){
			continue;
		}

		partyContractIds.append(contractId);

		// The draft is the business of its initiator
		if (contractPtr->GetSubmittedAt().isValid() || contractPtr->GetInitiatorTenantId() == tenantId){
			retVal.append(IContractInfoSharedPtr::CreateFromUnique(std::move(contractPtr)));
		}
	}

	if (isRead && !isCached){
		QMutexLocker locker(&m_cacheMutex);
		if (m_cacheGeneration == generation){
			m_partyContractIdsCache.insert(tenantId, partyContractIds);
		}
	}

	if (isReadPtr != nullptr){
		*isReadPtr = isRead;
	}

	return retVal;
}


ContractInfoList CContractManagerComp::GetOpenContracts(bool* isReadPtr) const
{
	const QList<int> openStates = {CS_NEGOTIATION, CS_SIGNING, CS_SIGNED, CS_ACTIVE};
	QStringList stateTexts;
	for (int state : openStates){
		stateTexts.append(QString::number(state));
	}

	iprm::CTextParam statesParam;
	statesParam.SetText(stateTexts.join(QLatin1Char(',')));
	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter(s_statesFilterParamId, &statesParam);

	ilog::CMessageContainer readErrors;
	const QByteArrayList contractIds = m_contractCollectionCompPtr->GetElementIds(0, -1, &filterParams, &readErrors);
	bool isRead = readErrors.GetWorstCategory() < istd::IInformationProvider::IC_ERROR;

	const QDateTime now = QDateTime::currentDateTimeUtc();
	ContractInfoList retVal;
	for (const QByteArray& contractId : contractIds){
		IContractInfoUniquePtr contractPtr = ReadContract(contractId);
		if (!contractPtr.IsValid()){
			isRead = false;

			continue;
		}

		// The stored state is the one of the last change: time may have ended the contract since
		if (openStates.contains(ComputeContractState(*contractPtr, now).state)){
			retVal.append(IContractInfoSharedPtr::CreateFromUnique(std::move(contractPtr)));
		}
	}

	if (isReadPtr != nullptr){
		*isReadPtr = isRead;
	}

	return retVal;
}


ContractInfoList CContractManagerComp::GetContractsOfPair(const QByteArray& tenantId, const QByteArray& otherTenantId, bool* isReadPtr) const
{
	ContractInfoList retVal;
	for (const IContractInfoSharedPtr& contractPtr : GetContractsOfParty(tenantId, isReadPtr)){
		if (contractPtr->GetSubmittedAt().isValid() && FindContractLinkOfPair(contractPtr->GetLinks(), tenantId, otherTenantId) >= 0){
			retVal.append(contractPtr);
		}
	}

	return retVal;
}


IContractPartyPrivateInfoUniquePtr CContractManagerComp::GetPrivateInfo(const QByteArray& contractId, const QByteArray& tenantId) const
{
	return ReadPrivateInfo(contractId, tenantId);
}


ContractEventInfoList CContractManagerComp::GetEvents(const QByteArray& contractId, const QByteArray& tenantId, bool* isReadPtr) const
{
	ContractEventInfoList retVal;
	bool isRead = true;
	for (const QByteArray& id : CollectionSelection::SelectIds(*m_eventCollectionCompPtr, CollectionSelection::ColumnEquals("ContractId", contractId), QByteArrayList(), &isRead)){
		imtbase::IObjectCollection::DataPtr dataPtr;
		const IContractEventInfo* storedEventPtr = m_eventCollectionCompPtr->GetObjectData(id, dataPtr) ? dynamic_cast<const IContractEventInfo*>(dataPtr.GetPtr()) : nullptr;
		IContractEventInfoUniquePtr eventPtr = m_eventFactoryCompPtr.CreateInstance();
		if (storedEventPtr == nullptr || !eventPtr.IsValid() || !eventPtr->CopyFrom(*storedEventPtr)){
			isRead = false;

			continue;
		}

		if (eventPtr->GetContractId() == contractId && (!eventPtr->IsPrivate() || eventPtr->GetTenantId() == tenantId)){
			retVal.append(IContractEventInfoSharedPtr::CreateFromUnique(std::move(eventPtr)));
		}
	}

	std::sort(retVal.begin(), retVal.end(), [](const IContractEventInfoSharedPtr& first, const IContractEventInfoSharedPtr& second){
		return first->GetTime() < second->GetTime();
	});

	if (isReadPtr != nullptr){
		*isReadPtr = isRead;
	}

	return retVal;
}


// reimplemented (imtauth::IContractManager)

QByteArray CContractManagerComp::CreateContract(const ContractActor& actor, const IContractInfo& draft, QString& errorMessage)
{
	QMutexLocker locker(&m_changeMutex);

	ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(actor.tenantId);
	if (actor.tenantId.isEmpty() || !tenantPtr.IsValid()){
		errorMessage = QStringLiteral("A contract is concluded by an organization");

		return QByteArray();
	}

	IContractInfoUniquePtr contractPtr = m_contractFactoryCompPtr.CreateInstance();
	if (!contractPtr.IsValid()){
		errorMessage = QStringLiteral("The contract could not be created");

		return QByteArray();
	}

	const QDateTime now = QDateTime::currentDateTimeUtc();
	contractPtr->SetContractId(CreateId());
	contractPtr->SetInitiatorTenantId(actor.tenantId);
	contractPtr->SetCreatedByUserId(actor.userId);
	contractPtr->SetCreatedAt(now);
	contractPtr->SetUpdatedAt(now);
	contractPtr->SetRevision(1);

	if (!ApplyDraft(draft, *contractPtr, errorMessage)){
		return QByteArray();
	}

	contractPtr->SetContentHash(ComputeContractContentHash(*contractPtr));

	if (!StoreContract(*contractPtr, true)){
		errorMessage = QStringLiteral("The contract could not be stored");

		return QByteArray();
	}

	ChangeRecord record;
	record.journal.append(CreateEntry(actor, IContractEventInfo::ET_CREATED));
	Announce(*contractPtr, record);

	return contractPtr->GetContractId();
}


bool CContractManagerComp::UpdateContract(const ContractActor& actor, const IContractInfo& draft, int expectedRevision, QString& errorMessage)
{
	return ChangeContract(actor, draft.GetContractId(), [&](IContractInfo& contract, ChangeRecord& record, QString& error){
		if (actor.tenantId != contract.GetInitiatorTenantId()){
			error = QStringLiteral("Only the initiator changes the terms");

			return false;
		}

		if (contract.GetSignedAt().isValid() || contract.GetCancelledAt().isValid()){
			error = QStringLiteral("The terms of a concluded or cancelled contract stay; a new contract supersedes it");

			return false;
		}

		if (!CheckRevision(contract, expectedRevision, error)){
			return false;
		}

		const QByteArrayList previousParties = GetContractPartyTenantIds(contract);
		if (!ApplyDraft(draft, contract, error)){
			return false;
		}

		int newUnknownCount = 0;
		QByteArrayList addedParties;
		for (const IContractInfo::Party& party : contract.GetParties()){
			if (!previousParties.contains(party.tenantId)){
				addedParties.append(party.tenantId);
				if (IsUnknownChannel(party.channel)){
					++newUnknownCount;
				}
			}
		}

		const bool isSubmitted = contract.GetSubmittedAt().isValid();
		if (isSubmitted && !CheckInvitationLimit(contract.GetInitiatorTenantId(), newUnknownCount, error)){
			return false;
		}

		record.unknownInvitations = isSubmitted ? newUnknownCount : 0;

		RestartReview(contract);

		record.journal.append(CreateEntry(actor, IContractEventInfo::ET_UPDATED));
		if (isSubmitted){
			const QByteArrayList currentParties = GetContractPartyTenantIds(contract);
			QByteArrayList keptParties;
			QByteArrayList removedParties;
			for (const QByteArray& tenantId : previousParties){
				(currentParties.contains(tenantId) ? keptParties : removedParties).append(tenantId);
			}

			record.events.append(CreateTenantEvent(TenantEvent::s_contractTermsChanged, contract, actor.tenantId, keptParties));
			if (!removedParties.isEmpty()){
				record.events.append(CreateTenantEvent(TenantEvent::s_contractPartyRemoved, contract, actor.tenantId, removedParties));
			}

			if (!addedParties.isEmpty()){
				record.events.append(CreateTenantEvent(TenantEvent::s_contractInvitationReceived, contract, actor.tenantId, addedParties));
			}
		}

		return true;
	}, errorMessage);
}


bool CContractManagerComp::AddPartyByCode(
			const ContractActor& actor,
			const QByteArray& contractId,
			int expectedRevision,
			const QString& connectionCode,
			const QByteArray& roleId,
			QString& errorMessage)
{
	return ChangeContract(actor, contractId, [&](IContractInfo& contract, ChangeRecord& record, QString& error){
		if (actor.tenantId != contract.GetInitiatorTenantId()){
			error = QStringLiteral("Only the initiator adds parties");

			return false;
		}

		if (contract.GetSignedAt().isValid() || contract.GetCancelledAt().isValid()){
			error = QStringLiteral("A concluded or cancelled contract gets no new parties; a new contract supersedes it");

			return false;
		}

		if (!CheckRevision(contract, expectedRevision, error)){
			return false;
		}

		const QByteArray tenantId = m_connectionManagerCompPtr->FindTenantByConnectionCode(connectionCode.trimmed());
		if (tenantId.isEmpty()){
			error = QStringLiteral("No organization has this connection code");

			return false;
		}

		IContractInfo::Parties parties = contract.GetParties();
		if (FindContractParty(parties, tenantId) >= 0){
			error = QStringLiteral("The organization is a party already");

			return false;
		}

		IContractInfo::Party party;
		if (!FindChannel(contract.GetInitiatorTenantId(), tenantId, party.channel) || IsUnknownChannel(party.channel)){
			ITenantDirectoryProfileUniquePtr profilePtr = m_directoryCompPtr->GetProfile(tenantId);
			if (!m_connectionManagerCompPtr->GetAllowConnectionsByCode(tenantId) || !profilePtr.IsValid() || !profilePtr->IsAcceptingInvitationsFromUnknown()){
				error = QStringLiteral("The organization does not accept contracts by its code");

				return false;
			}

			party.channel = IContractInfo::PC_CONNECTION_CODE;
		}

		if (!roleId.isEmpty()){
			party.roleName = FindRoleName(roleId, contract.GetInitiatorTenantId(), IPartyRoleInfo::RA_PARTY);
			if (party.roleName.isEmpty()){
				error = QStringLiteral("The role '%1' is not available for a party").arg(QString::fromUtf8(roleId));

				return false;
			}

			party.roleId = roleId;
		}

		const bool isSubmitted = contract.GetSubmittedAt().isValid();
		if (isSubmitted && !CheckInvitationLimit(contract.GetInitiatorTenantId(), IsUnknownChannel(party.channel) ? 1 : 0, error)){
			return false;
		}

		record.unknownInvitations = (isSubmitted && IsUnknownChannel(party.channel)) ? 1 : 0;

		party.tenantId = tenantId;
		party.tenantName = FindTenantName(tenantId);
		party.state = isSubmitted ? IContractInfo::PS_REVIEWING : IContractInfo::PS_INVITED;
		party.stateChangedAt = QDateTime::currentDateTimeUtc();

		const QByteArrayList previousParties = GetContractPartyTenantIds(contract);
		parties.append(party);
		contract.SetParties(parties);
		AddSchemeLinks(contract, tenantId);

		RestartReview(contract);

		JournalEntry entry = CreateEntry(actor, IContractEventInfo::ET_PARTY_ADDED, party.tenantName);
		entry.subjectId = tenantId;
		record.journal.append(entry);
		if (isSubmitted){
			record.events.append(CreateTenantEvent(TenantEvent::s_contractTermsChanged, contract, actor.tenantId, previousParties));
			record.events.append(CreateTenantEvent(TenantEvent::s_contractInvitationReceived, contract, actor.tenantId, {tenantId}));
		}

		return true;
	}, errorMessage);
}


bool CContractManagerComp::RemoveContract(const ContractActor& actor, const QByteArray& contractId, QString& errorMessage)
{
	QMutexLocker locker(&m_changeMutex);

	IContractInfoUniquePtr contractPtr = ReadContract(contractId);
	if (!contractPtr.IsValid()){
		errorMessage = QStringLiteral("The contract could not be read");

		return false;
	}

	if (actor.tenantId != contractPtr->GetInitiatorTenantId() || contractPtr->GetSubmittedAt().isValid()){
		errorMessage = QStringLiteral("Only the initiator removes a draft that was never sent");

		return false;
	}

	bool isRead = true;
	const QByteArrayList eventIds = CollectionSelection::SelectIds(*m_eventCollectionCompPtr, CollectionSelection::ColumnEquals("ContractId", contractId), QByteArrayList(), &isRead);
	bool isPrivateRead = true;
	const QByteArrayList privateIds = CollectionSelection::SelectIds(*m_privateCollectionCompPtr, CollectionSelection::ColumnEquals("ContractId", contractId), QByteArrayList(), &isPrivateRead);
	if (!isRead || !isPrivateRead){
		errorMessage = QStringLiteral("The records of the contract could not be read");

		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	// The records go first: a failure leaves the draft to remove again
	if ((!eventIds.isEmpty() && !m_eventCollectionCompPtr->RemoveElements(eventIds))
				|| (!privateIds.isEmpty() && !m_privateCollectionCompPtr->RemoveElements(privateIds))
				|| !m_contractCollectionCompPtr->RemoveElements({contractId})){
		errorMessage = QStringLiteral("The draft could not be removed");

		return false;
	}

	const QByteArrayList partyTenantIds = GetContractPartyTenantIds(*contractPtr);
	{
		QMutexLocker cacheLocker(&m_cacheMutex);
		++m_cacheGeneration;
		m_contractCache.remove(contractId);
		for (const QByteArray& tenantId : partyTenantIds){
			m_partyContractIdsCache.remove(tenantId);
			m_privateInfoCache.remove(GetPrivateInfoKey(contractId, tenantId));
		}
	}

	TenantEvent::Publish(this, CreateTenantEvent(TenantEvent::s_contractChanged, *contractPtr, QByteArray(), {contractPtr->GetInitiatorTenantId()}));

	return true;
}


bool CContractManagerComp::SubmitContract(const ContractActor& actor, const QByteArray& contractId, int expectedRevision, QString& errorMessage)
{
	return ChangeContract(actor, contractId, [&](IContractInfo& contract, ChangeRecord& record, QString& error){
		if (actor.tenantId != contract.GetInitiatorTenantId() || contract.GetSubmittedAt().isValid()){
			error = QStringLiteral("Only the initiator sends its draft, once");

			return false;
		}

		IContractInfo::Parties parties = contract.GetParties();
		int unknownCount = 0;
		for (const IContractInfo::Party& party : std::as_const(parties)){
			if (IsUnknownChannel(party.channel)){
				++unknownCount;
			}
		}

		if (!CheckRevision(contract, expectedRevision, error) || !CheckSendable(contract, error)
					|| !CheckInvitationLimit(contract.GetInitiatorTenantId(), unknownCount, error)){
			return false;
		}

		record.unknownInvitations = unknownCount;

		const QDateTime now = QDateTime::currentDateTimeUtc();
		contract.SetNumber(CreateNumber(contract));
		contract.SetSubmittedAt(now);
		for (IContractInfo::Party& party : parties){
			party.state = IContractInfo::PS_REVIEWING;
			party.stateChangedAt = now;
		}

		contract.SetParties(parties);

		record.journal.append(CreateEntry(actor, IContractEventInfo::ET_SUBMITTED));
		record.events.append(CreateTenantEvent(TenantEvent::s_contractInvitationReceived, contract, actor.tenantId, GetContractPartyTenantIds(contract)));

		return true;
	}, errorMessage);
}


bool CContractManagerComp::CancelContract(const ContractActor& actor, const QByteArray& contractId, const QString& reason, QString& errorMessage)
{
	return ChangeContract(actor, contractId, [&](IContractInfo& contract, ChangeRecord& record, QString& error){
		if (actor.tenantId != contract.GetInitiatorTenantId() || !contract.GetSubmittedAt().isValid()
					|| contract.GetSignedAt().isValid() || contract.GetCancelledAt().isValid()){
			error = QStringLiteral("Only the initiator withdraws a sent offer before all parties signed");

			return false;
		}

		contract.SetCancelledAt(QDateTime::currentDateTimeUtc());

		record.journal.append(CreateEntry(actor, IContractEventInfo::ET_CANCELLED, reason));
		record.events.append(CreateTenantEvent(TenantEvent::s_contractCancelled, contract, actor.tenantId, GetContractPartyTenantIds(contract)));

		return true;
	}, errorMessage);
}


bool CContractManagerComp::RequestChanges(const ContractActor& actor, const QByteArray& contractId, int expectedRevision, const QString& comment, QString& errorMessage)
{
	return ChangeContract(actor, contractId, [&](IContractInfo& contract, ChangeRecord& record, QString& error){
		if (!IsNegotiatedState(ComputeContractState(contract, QDateTime::currentDateTimeUtc()).state)){
			error = QStringLiteral("Changes are asked for while the contract is negotiated");

			return false;
		}

		if (actor.tenantId == contract.GetInitiatorTenantId() || comment.trimmed().isEmpty()){
			error = QStringLiteral("A party other than the initiator asks for changes, and says which");

			return false;
		}

		if (!CheckRevision(contract, expectedRevision, error)){
			return false;
		}

		IContractInfo::Parties parties = contract.GetParties();
		IContractInfo::Party& party = parties[FindContractParty(parties, actor.tenantId)];
		party.state = IContractInfo::PS_CHANGES_REQUESTED;
		party.stateComment = comment.trimmed();
		party.stateChangedAt = QDateTime::currentDateTimeUtc();
		party.signatures.clear();
		contract.SetParties(parties);

		record.journal.append(CreateEntry(actor, IContractEventInfo::ET_CHANGES_REQUESTED, comment.trimmed()));
		record.events.append(CreateTenantEvent(TenantEvent::s_contractChangesRequested, contract, actor.tenantId, {contract.GetInitiatorTenantId()}));

		return true;
	}, errorMessage);
}


bool CContractManagerComp::ApproveContract(const ContractActor& actor, const QByteArray& contractId, int expectedRevision, QString& errorMessage)
{
	return ChangeContract(actor, contractId, [&](IContractInfo& contract, ChangeRecord& record, QString& error){
		IContractInfo::Parties parties = contract.GetParties();
		IContractInfo::Party& party = parties[FindContractParty(parties, actor.tenantId)];
		if (!IsNegotiatedState(ComputeContractState(contract, QDateTime::currentDateTimeUtc()).state) || party.state != IContractInfo::PS_REVIEWING){
			error = QStringLiteral("A party approves the terms it reviews");

			return false;
		}

		if (!CheckRevision(contract, expectedRevision, error)){
			return false;
		}

		party.state = IContractInfo::PS_APPROVED;
		party.stateComment.clear();
		party.stateChangedAt = QDateTime::currentDateTimeUtc();
		contract.SetParties(parties);

		record.journal.append(CreateEntry(actor, IContractEventInfo::ET_APPROVED));

		return true;
	}, errorMessage);
}


bool CContractManagerComp::SignContract(
			const ContractActor& actor,
			const QByteArray& contractId,
			int expectedRevision,
			const ContractSignatureRequest& signature,
			QString& errorMessage)
{
	return ChangeContract(actor, contractId, [&](IContractInfo& contract, ChangeRecord& record, QString& error){
		const QDateTime now = QDateTime::currentDateTimeUtc();
		IContractInfo::Parties parties = contract.GetParties();
		const int partyIndex = FindContractParty(parties, actor.tenantId);
		const IContractInfo::PartyState partyState = parties[partyIndex].state;
		if (!IsNegotiatedState(ComputeContractState(contract, now).state) || (partyState != IContractInfo::PS_REVIEWING && partyState != IContractInfo::PS_APPROVED)){
			error = QStringLiteral("A party signs the terms it reviews, while the offer stands");

			return false;
		}

		if (!CheckRevision(contract, expectedRevision, error)){
			return false;
		}

		if (actor.userId.isEmpty()){
			error = QStringLiteral("A user signs");

			return false;
		}

		IContractInfo::Signature givenSignature;
		givenSignature.userId = actor.userId;
		givenSignature.userName = actor.userName;
		givenSignature.position = signature.position;
		givenSignature.signedAt = now;
		givenSignature.revision = contract.GetRevision();
		givenSignature.contentHash = ComputeContractContentHash(contract);

		// One signature of a user with the right to sign binds the party
		IContractInfo::Party& party = parties[partyIndex];
		party.signatures = IContractInfo::Signatures() << givenSignature;
		party.state = IContractInfo::PS_SIGNED;
		party.stateComment.clear();
		party.stateChangedAt = now;
		contract.SetParties(parties);

		record.journal.append(CreateEntry(actor, IContractEventInfo::ET_SIGNED, signature.position));

		bool areAllSigned = true;
		for (const IContractInfo::Party& otherParty : std::as_const(parties)){
			areAllSigned = areAllSigned && otherParty.state == IContractInfo::PS_SIGNED;
		}

		if (areAllSigned){
			return Conclude(contract, record, error);
		}

		// The initiator follows the signatures; the others hear when the contract is concluded
		record.events.append(CreateTenantEvent(TenantEvent::s_contractSigned, contract, actor.tenantId, {contract.GetInitiatorTenantId()}));

		return true;
	}, errorMessage);
}


bool CContractManagerComp::DeclineContract(const ContractActor& actor, const QByteArray& contractId, const QString& comment, QString& errorMessage)
{
	return ChangeContract(actor, contractId, [&](IContractInfo& contract, ChangeRecord& record, QString& error){
		if (!IsNegotiatedState(ComputeContractState(contract, QDateTime::currentDateTimeUtc()).state) || actor.tenantId == contract.GetInitiatorTenantId()){
			error = QStringLiteral("A party other than the initiator declines a contract while it is negotiated");

			return false;
		}

		if (comment.trimmed().isEmpty()){
			error = QStringLiteral("A refusal gives its reason");

			return false;
		}

		IContractInfo::Parties parties = contract.GetParties();
		IContractInfo::Party& party = parties[FindContractParty(parties, actor.tenantId)];
		party.state = IContractInfo::PS_DECLINED;
		party.stateComment = comment.trimmed();
		party.stateChangedAt = QDateTime::currentDateTimeUtc();
		party.signatures.clear();
		contract.SetParties(parties);

		record.journal.append(CreateEntry(actor, IContractEventInfo::ET_DECLINED, comment.trimmed()));
		record.events.append(CreateTenantEvent(TenantEvent::s_contractDeclined, contract, actor.tenantId, GetContractPartyTenantIds(contract)));

		return true;
	}, errorMessage);
}


bool CContractManagerComp::GiveNotice(const ContractActor& actor, const QByteArray& contractId, const QString& reason, bool isImmediate, QString& errorMessage)
{
	return ChangeContract(actor, contractId, [&](IContractInfo& contract, ChangeRecord& record, QString& error){
		const QDateTime now = QDateTime::currentDateTimeUtc();
		IContractInfo::Parties parties = contract.GetParties();
		IContractInfo::Party& party = parties[FindContractParty(parties, actor.tenantId)];
		if (!IsConcludedState(ComputeContractState(contract, now).state) || party.state != IContractInfo::PS_SIGNED){
			error = QStringLiteral("A party bound by a concluded contract leaves it");

			return false;
		}

		if (isImmediate && reason.trimmed().isEmpty()){
			error = QStringLiteral("Leaving at once gives the breach as the reason");

			return false;
		}

		party.withdrawReason = reason.trimmed();
		party.stateChangedAt = now;
		if (isImmediate){
			party.state = IContractInfo::PS_WITHDRAWN;
			party.withdrawAt = now;
		}
		else{
			party.state = IContractInfo::PS_NOTICE_GIVEN;
			party.withdrawAt = now.addDays(contract.GetTerminationNoticeDays());
		}

		contract.SetParties(parties);

		record.journal.append(CreateEntry(actor, isImmediate ? IContractEventInfo::ET_WITHDRAWN : IContractEventInfo::ET_NOTICE_GIVEN, reason.trimmed()));
		record.events.append(CreateTenantEvent(
					isImmediate ? TenantEvent::s_contractWithdrawn : TenantEvent::s_contractNoticeGiven,
					contract,
					actor.tenantId,
					GetContractPartyTenantIds(contract)));

		return true;
	}, errorMessage);
}


bool CContractManagerComp::RevokeNotice(const ContractActor& actor, const QByteArray& contractId, QString& errorMessage)
{
	return ChangeContract(actor, contractId, [&](IContractInfo& contract, ChangeRecord& record, QString& error){
		const QDateTime now = QDateTime::currentDateTimeUtc();
		IContractInfo::Parties parties = contract.GetParties();
		IContractInfo::Party& party = parties[FindContractParty(parties, actor.tenantId)];
		if (party.state != IContractInfo::PS_NOTICE_GIVEN || party.withdrawAt <= now || !IsConcludedState(ComputeContractState(contract, now).state)){
			error = QStringLiteral("A notice is revoked before the party left");

			return false;
		}

		party.state = IContractInfo::PS_SIGNED;
		party.withdrawAt = QDateTime();
		party.withdrawReason.clear();
		party.stateChangedAt = now;
		contract.SetParties(parties);

		record.journal.append(CreateEntry(actor, IContractEventInfo::ET_NOTICE_REVOKED));
		record.events.append(CreateTenantEvent(TenantEvent::s_contractNoticeRevoked, contract, actor.tenantId, GetContractPartyTenantIds(contract)));

		return true;
	}, errorMessage);
}


bool CContractManagerComp::GiveNonRenewalNotice(const ContractActor& actor, const QByteArray& contractId, QString& errorMessage)
{
	return ChangeContract(actor, contractId, [&](IContractInfo& contract, ChangeRecord& record, QString& error){
		const QDateTime now = QDateTime::currentDateTimeUtc();
		IContractInfo::Parties parties = contract.GetParties();
		IContractInfo::Party& party = parties[FindContractParty(parties, actor.tenantId)];
		if (!contract.IsAutoRenew() || !IsConcludedState(ComputeContractState(contract, now).state) || !IsPartyBoundAt(contract, party, now)){
			error = QStringLiteral("A party bound by a renewing contract stops its renewal");

			return false;
		}

		if (party.nonRenewalNoticeAt.isValid()){
			return true;
		}

		party.nonRenewalNoticeAt = now;
		contract.SetParties(parties);

		record.journal.append(CreateEntry(actor, IContractEventInfo::ET_NON_RENEWAL_NOTICE));
		record.events.append(CreateTenantEvent(TenantEvent::s_contractRenewalStopped, contract, actor.tenantId, GetContractPartyTenantIds(contract)));

		return true;
	}, errorMessage);
}


bool CContractManagerComp::GiveAllocation(const ContractActor& actor, const QByteArray& contractId, const IContractInfo::Allocation& allocation, QString& errorMessage)
{
	IContractInfo::Allocation newAllocation = allocation;
	newAllocation.allocationId = CreateId();
	newAllocation.grantorTenantId = actor.tenantId;
	newAllocation.permissionIds.removeAll(QByteArray());
	std::sort(newAllocation.permissionIds.begin(), newAllocation.permissionIds.end());
	newAllocation.permissionIds.erase(std::unique(newAllocation.permissionIds.begin(), newAllocation.permissionIds.end()), newAllocation.permissionIds.end());

	if (newAllocation.productId.isEmpty() || newAllocation.permissionIds.isEmpty()){
		errorMessage = QStringLiteral("Choose the permissions to give");

		return false;
	}

	if (newAllocation.validFrom.isValid() && newAllocation.validUntil.isValid() && newAllocation.validUntil <= newAllocation.validFrom){
		errorMessage = QStringLiteral("The permissions end after they start");

		return false;
	}

	return ChangeContract(actor, contractId, [&](IContractInfo& contract, ChangeRecord& record, QString& error){
		if (!CanPartyChangeAllocations(contract, actor.tenantId, error)){
			return false;
		}

		const IContractInfo::Links links = contract.GetLinks();
		const int linkIndex = FindContractLinkOfPair(links, actor.tenantId, newAllocation.granteeTenantId);
		if (linkIndex < 0 || newAllocation.granteeTenantId == actor.tenantId){
			error = QStringLiteral("Your organization gives permissions to a party it is linked with (page Links)");

			return false;
		}

		newAllocation.linkId = links[linkIndex].linkId;
		if (!CheckAllocationScope(contract, newAllocation, error)){
			return false;
		}

		IContractInfo::Allocations allocations = contract.GetAllocations();
		allocations.append(newAllocation);
		contract.SetAllocations(allocations);

		JournalEntry entry = CreateEntry(actor, IContractEventInfo::ET_PERMISSIONS_GIVEN, FindTenantName(newAllocation.granteeTenantId));
		entry.subjectId = newAllocation.allocationId;
		record.journal.append(entry);
		if (contract.GetSignedAt().isValid()){
			record.events.append(CreateTenantEvent(TenantEvent::s_contractPermissionsReceived, contract, actor.tenantId, {newAllocation.granteeTenantId}));
		}

		return true;
	}, errorMessage);
}


bool CContractManagerComp::WithdrawAllocation(const ContractActor& actor, const QByteArray& contractId, const QByteArray& allocationId, QString& errorMessage)
{
	return ChangeContract(actor, contractId, [&](IContractInfo& contract, ChangeRecord& record, QString& error){
		if (!CanPartyChangeAllocations(contract, actor.tenantId, error)){
			return false;
		}

		IContractInfo::Allocations allocations = contract.GetAllocations();
		const int allocationIndex = FindContractAllocation(allocations, allocationId);
		if (allocationIndex < 0 || allocations[allocationIndex].grantorTenantId != actor.tenantId){
			error = QStringLiteral("An organization takes back only the permissions it gave");

			return false;
		}

		const QByteArray granteeTenantId = allocations[allocationIndex].granteeTenantId;
		allocations.removeAt(allocationIndex);
		contract.SetAllocations(allocations);

		JournalEntry entry = CreateEntry(actor, IContractEventInfo::ET_PERMISSIONS_WITHDRAWN, FindTenantName(granteeTenantId));
		entry.subjectId = allocationId;
		record.journal.append(entry);
		if (contract.GetSignedAt().isValid()){
			record.events.append(CreateTenantEvent(TenantEvent::s_contractPermissionsWithdrawn, contract, actor.tenantId, {granteeTenantId}));
		}

		return true;
	}, errorMessage);
}


bool CContractManagerComp::AddAttachment(
			const ContractActor& actor,
			const QByteArray& contractId,
			const IContractInfo::Attachment& attachment,
			bool isInternal,
			QString& errorMessage)
{
	if (attachment.attachmentId.isEmpty() || attachment.fileName.isEmpty()){
		errorMessage = QStringLiteral("An attachment is a stored file with a name");

		return false;
	}

	IContractInfo::Attachment newAttachment = attachment;
	newAttachment.addedByTenantId = actor.tenantId;
	newAttachment.addedByUserId = actor.userId;
	newAttachment.addedAt = QDateTime::currentDateTimeUtc();
	newAttachment.isReplaced = false;

	if (isInternal){
		QMutexLocker locker(&m_changeMutex);

		IContractInfoUniquePtr contractPtr = ReadContract(contractId);
		if (!contractPtr.IsValid() || FindContractParty(contractPtr->GetParties(), actor.tenantId) < 0){
			errorMessage = QStringLiteral("The organization is not a party of the contract");

			return false;
		}

		IContractPartyPrivateInfoUniquePtr privateInfoPtr = ReadPrivateInfo(contractId, actor.tenantId);
		if (!privateInfoPtr.IsValid()){
			errorMessage = QStringLiteral("The records of the party could not be read");

			return false;
		}

		IContractInfo::Attachments attachments = privateInfoPtr->GetAttachments();
		for (IContractInfo::Attachment& otherAttachment : attachments){
			if (otherAttachment.attachmentId == newAttachment.replacesAttachmentId){
				otherAttachment.isReplaced = true;
			}
		}

		attachments.append(newAttachment);
		privateInfoPtr->SetAttachments(attachments);
		privateInfoPtr->SetUpdatedAt(newAttachment.addedAt);
		if (!StorePrivateInfo(*privateInfoPtr)){
			errorMessage = QStringLiteral("The records of the party could not be stored");

			return false;
		}

		ChangeRecord record;
		JournalEntry entry = CreateEntry(actor, IContractEventInfo::ET_ATTACHMENT_ADDED, newAttachment.fileName);
		entry.subjectId = newAttachment.attachmentId;
		entry.isPrivate = true;
		record.journal.append(entry);
		Announce(*contractPtr, record);

		return true;
	}

	return ChangeContract(actor, contractId, [&](IContractInfo& contract, ChangeRecord& record, QString& error){
		const ContractState state = ComputeContractState(contract, QDateTime::currentDateTimeUtc()).state;
		// Before the contract is concluded its files are terms the parties sign, after it they are not
		const bool isTermsPart = !contract.GetSignedAt().isValid();
		if (contract.GetCancelledAt().isValid() || state == CS_EXPIRED || state == CS_TERMINATED || state == CS_SUPERSEDED){
			error = QStringLiteral("An ended contract gets no attachments");

			return false;
		}

		if (isTermsPart && actor.tenantId != contract.GetInitiatorTenantId()){
			error = QStringLiteral("The initiator attaches the files of the terms");

			return false;
		}

		IContractInfo::Attachments attachments = contract.GetAttachments();
		if (!newAttachment.replacesAttachmentId.isEmpty()){
			const int replacedIndex = FindContractAttachment(attachments, newAttachment.replacesAttachmentId);
			if (replacedIndex < 0 || attachments[replacedIndex].isReplaced || IsTermsAttachment(contract, attachments[replacedIndex]) != isTermsPart){
				error = QStringLiteral("A new version replaces the current version of a file; the signed files stay");

				return false;
			}

			if (attachments[replacedIndex].addedByTenantId != actor.tenantId){
				error = QStringLiteral("A new version of a document is attached by the organization that attached it");

				return false;
			}

			attachments[replacedIndex].isReplaced = true;
		}

		attachments.append(newAttachment);
		contract.SetAttachments(attachments);

		// The parties signed other files
		if (isTermsPart){
			RestartReview(contract);
		}

		JournalEntry entry = CreateEntry(
					actor,
					newAttachment.replacesAttachmentId.isEmpty() ? IContractEventInfo::ET_ATTACHMENT_ADDED : IContractEventInfo::ET_ATTACHMENT_REPLACED,
					newAttachment.fileName);
		entry.subjectId = newAttachment.attachmentId;
		record.journal.append(entry);
		if (isTermsPart && contract.GetSubmittedAt().isValid()){
			record.events.append(CreateTenantEvent(TenantEvent::s_contractTermsChanged, contract, actor.tenantId, GetContractPartyTenantIds(contract)));
		}

		return true;
	}, errorMessage);
}


bool CContractManagerComp::SetPrivateInfo(const ContractActor& actor, const IContractPartyPrivateInfo& privateInfo, QString& errorMessage)
{
	QMutexLocker locker(&m_changeMutex);

	const QByteArray contractId = privateInfo.GetContractId();
	IContractInfoUniquePtr contractPtr = ReadContract(contractId);
	if (!contractPtr.IsValid() || FindContractParty(contractPtr->GetParties(), actor.tenantId) < 0){
		errorMessage = QStringLiteral("The organization is not a party of the contract");

		return false;
	}

	IContractPartyPrivateInfoUniquePtr storedInfoPtr = ReadPrivateInfo(contractId, actor.tenantId);
	if (!storedInfoPtr.IsValid()){
		errorMessage = QStringLiteral("The records of the party could not be read");

		return false;
	}

	// The notifications of the contract go to the responsible user: one of the organization only
	const QByteArray responsibleUserId = privateInfo.GetResponsibleUserId();
	if (!responsibleUserId.isEmpty() && !IsTenantUser(actor.tenantId, responsibleUserId)){
		errorMessage = QStringLiteral("The responsible user is not a member of the organization");

		return false;
	}

	storedInfoPtr->SetInternalNumber(privateInfo.GetInternalNumber());
	storedInfoPtr->SetResponsibleUserId(privateInfo.GetResponsibleUserId());
	storedInfoPtr->SetNotes(privateInfo.GetNotes());
	storedInfoPtr->SetReminderDays(qMax(0, privateInfo.GetReminderDays()));
	storedInfoPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc());
	if (!StorePrivateInfo(*storedInfoPtr)){
		errorMessage = QStringLiteral("The records of the party could not be stored");

		return false;
	}

	ChangeRecord record;
	JournalEntry entry = CreateEntry(actor, IContractEventInfo::ET_PRIVATE_INFO_CHANGED);
	entry.isPrivate = true;
	record.journal.append(entry);
	Announce(*contractPtr, record);

	return true;
}


bool CContractManagerComp::IsTenantUser(const QByteArray& tenantId, const QByteArray& userId) const
{
	ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
	if (tenantPtr.IsValid() && (tenantPtr->GetOwnerId() == userId || tenantPtr->GetCreatorId() == userId)){
		return true;
	}

	ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->FindMembership(userId, tenantId);

	return membershipPtr.IsValid() && membershipPtr->IsActive();
}


bool CContractManagerComp::SetAllocationGroups(
			const ContractActor& actor,
			const QByteArray& contractId,
			const QByteArray& allocationId,
			const QByteArrayList& groupIds,
			QString& errorMessage)
{
	QMutexLocker locker(&m_changeMutex);

	IContractInfoUniquePtr contractPtr = ReadContract(contractId);
	if (!contractPtr.IsValid()){
		errorMessage = QStringLiteral("The contract could not be read");

		return false;
	}

	const IContractInfo::Allocations allocations = contractPtr->GetAllocations();
	const int allocationIndex = FindContractAllocation(allocations, allocationId);
	if (allocationIndex < 0 || allocations[allocationIndex].granteeTenantId != actor.tenantId || !contractPtr->GetSubmittedAt().isValid()){
		errorMessage = QStringLiteral("The organization gives only the permissions it receives");

		return false;
	}

	QByteArrayList checkedGroupIds;
	for (const QByteArray& groupId : groupIds){
		bool isRead = false;
		if (!m_bindingManagerCompPtr->HasBinding(actor.tenantId, s_groupEntityType, groupId, &isRead)){
			errorMessage = isRead
						? QStringLiteral("The group '%1' is not a group of the organization").arg(QString::fromUtf8(groupId))
						: QStringLiteral("The groups of the organization could not be read");

			return false;
		}

		if (!checkedGroupIds.contains(groupId)){
			checkedGroupIds.append(groupId);
		}
	}

	IContractPartyPrivateInfoUniquePtr privateInfoPtr = ReadPrivateInfo(contractId, actor.tenantId);
	if (!privateInfoPtr.IsValid()){
		errorMessage = QStringLiteral("The records of the party could not be read");

		return false;
	}

	SetAllocationGroupIds(*privateInfoPtr, allocationId, checkedGroupIds);
	privateInfoPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc());
	if (!StorePrivateInfo(*privateInfoPtr)){
		errorMessage = QStringLiteral("The records of the party could not be stored");

		return false;
	}

	ChangeRecord record;
	JournalEntry entry = CreateEntry(actor, IContractEventInfo::ET_ALLOCATION_GROUPS_CHANGED);
	entry.subjectId = allocationId;
	entry.isPrivate = true;
	record.journal.append(entry);
	Announce(*contractPtr, record);

	return true;
}


bool CContractManagerComp::RecordReminder(const QByteArray& contractId, const QByteArray& tenantId, const QByteArray& reminderKey)
{
	QMutexLocker locker(&m_changeMutex);

	IContractInfoUniquePtr contractPtr = ReadContract(contractId);
	if (!contractPtr.IsValid()){
		return false;
	}

	bool isRead = false;
	for (const IContractEventInfoSharedPtr& eventPtr : GetEvents(contractId, tenantId, &isRead)){
		if (eventPtr->GetEventType() == IContractEventInfo::ET_REMINDER_SENT && eventPtr->GetTenantId() == tenantId && eventPtr->GetSubjectId() == reminderKey){
			return false;
		}
	}

	if (!isRead){
		return false;
	}

	ChangeRecord record;
	JournalEntry entry;
	entry.type = IContractEventInfo::ET_REMINDER_SENT;
	entry.tenantId = tenantId;
	entry.subjectId = reminderKey;
	entry.isPrivate = true;
	record.journal.append(entry);

	QVariantMap reminder = CreateTenantEvent(TenantEvent::s_contractReminder, *contractPtr, QByteArray(), {tenantId});
	reminder.insert(TenantEvent::s_reminderKey, reminderKey);
	record.events.append(reminder);
	Announce(*contractPtr, record);

	return true;
}


// private methods

bool CContractManagerComp::ChangeContract(const ContractActor& actor, const QByteArray& contractId, const ContractChange& change, QString& errorMessage)
{
	QMutexLocker locker(&m_changeMutex);

	IContractInfoUniquePtr contractPtr = ReadContract(contractId);
	IContractInfoUniquePtr previousContractPtr = ReadContract(contractId);
	if (!contractPtr.IsValid() || !previousContractPtr.IsValid()){
		errorMessage = QStringLiteral("The contract could not be read");

		return false;
	}

	// The draft is the business of its initiator
	const bool isParty = !actor.tenantId.isEmpty() && FindContractParty(contractPtr->GetParties(), actor.tenantId) >= 0;
	if (!isParty || (!contractPtr->GetSubmittedAt().isValid() && actor.tenantId != contractPtr->GetInitiatorTenantId())){
		errorMessage = QStringLiteral("The organization is not a party of the contract");

		return false;
	}

	ChangeRecord record;
	if (!change(*contractPtr, record, errorMessage)){
		return false;
	}

	if (contractPtr->IsEqual(*previousContractPtr) && record.journal.isEmpty()){
		return true;
	}

	contractPtr->SetContentHash(ComputeContractContentHash(*contractPtr));
	contractPtr->SetUpdatedAt(QDateTime::currentDateTimeUtc());

	if (!StoreContract(*contractPtr, false)){
		errorMessage = QStringLiteral("The contract could not be stored");

		return false;
	}

	RecordInvitations(contractPtr->GetInitiatorTenantId(), record.unknownInvitations);

	if (record.isConcluded){
		CompleteConclusion(*contractPtr);
	}

	Announce(*contractPtr, record);

	return true;
}


IContractInfoUniquePtr CContractManagerComp::CopyContract(const IContractInfo& contract) const
{
	IContractInfoUniquePtr retVal = m_contractFactoryCompPtr.CreateInstance();
	if (!retVal.IsValid() || !retVal->CopyFrom(contract)){
		return nullptr;
	}

	return retVal;
}


IContractInfoUniquePtr CContractManagerComp::ReadContract(const QByteArray& contractId) const
{
	if (contractId.isEmpty()){
		return nullptr;
	}

	quint64 generation = 0;
	{
		QMutexLocker locker(&m_cacheMutex);
		auto cacheIter = m_contractCache.constFind(contractId);
		if (cacheIter != m_contractCache.constEnd()){
			return CopyContract(**cacheIter);
		}

		generation = m_cacheGeneration;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_contractCollectionCompPtr->GetObjectData(contractId, dataPtr)){
		return nullptr;
	}

	const IContractInfo* storedContractPtr = dynamic_cast<const IContractInfo*>(dataPtr.GetPtr());
	if (storedContractPtr == nullptr || storedContractPtr->GetContractId() != contractId){
		return nullptr;
	}

	IContractInfoUniquePtr cachedPtr = CopyContract(*storedContractPtr);
	if (!cachedPtr.IsValid()){
		return nullptr;
	}

	IContractInfoUniquePtr retVal = CopyContract(*cachedPtr);

	QMutexLocker locker(&m_cacheMutex);
	if (m_cacheGeneration == generation){
		m_contractCache.insert(contractId, IContractInfoSharedPtr::CreateFromUnique(std::move(cachedPtr)));
	}

	return retVal;
}


bool CContractManagerComp::StoreContract(const IContractInfo& contract, bool isNew)
{
	const QByteArray contractId = contract.GetContractId();
	QByteArrayList affectedTenantIds = GetContractPartyTenantIds(contract);
	if (!isNew){
		IContractInfoUniquePtr previousContractPtr = ReadContract(contractId);
		if (previousContractPtr.IsValid()){
			affectedTenantIds += GetContractPartyTenantIds(*previousContractPtr);
		}
	}

	IContractInfoUniquePtr cachedPtr = CopyContract(contract);

	istd::CChangeNotifier changeNotifier(this);

	const bool isStored = isNew
				? !m_contractCollectionCompPtr->InsertNewObject("Contract", contract.GetName(), contract.GetDescription(), &contract, contractId).isEmpty()
				: m_contractCollectionCompPtr->SetObjectData(contractId, contract);

	QMutexLocker locker(&m_cacheMutex);
	++m_cacheGeneration;

	// The parties of before and after see another list now
	for (const QByteArray& tenantId : std::as_const(affectedTenantIds)){
		m_partyContractIdsCache.remove(tenantId);
	}

	if (isStored && cachedPtr.IsValid()){
		m_contractCache.insert(contractId, IContractInfoSharedPtr::CreateFromUnique(std::move(cachedPtr)));
	}
	else{
		m_contractCache.remove(contractId);
	}

	return isStored;
}


IContractPartyPrivateInfoUniquePtr CContractManagerComp::ReadPrivateInfo(const QByteArray& contractId, const QByteArray& tenantId) const
{
	IContractPartyPrivateInfoUniquePtr retVal = m_privateFactoryCompPtr.CreateInstance();
	if (!retVal.IsValid() || contractId.isEmpty() || tenantId.isEmpty()){
		return nullptr;
	}

	const QByteArray key = GetPrivateInfoKey(contractId, tenantId);
	quint64 generation = 0;
	{
		QMutexLocker locker(&m_cacheMutex);
		generation = m_cacheGeneration;
		auto cacheIter = m_privateInfoCache.constFind(key);
		if (cacheIter != m_privateInfoCache.constEnd()){
			if (!retVal->CopyFrom(**cacheIter)){
				return nullptr;
			}

			return retVal;
		}
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_privateCollectionCompPtr->GetObjectData(key, dataPtr)){
		const IContractPartyPrivateInfo* storedInfoPtr = dynamic_cast<const IContractPartyPrivateInfo*>(dataPtr.GetPtr());
		if (storedInfoPtr == nullptr || !retVal->CopyFrom(*storedInfoPtr)){
			return nullptr;
		}
	}
	else{
		// No record, or a record that could not be read
		bool isRead = true;
		const QByteArrayList ids = CollectionSelection::SelectIds(
					*m_privateCollectionCompPtr,
					CollectionSelection::ColumnEquals("ContractId", contractId),
					QByteArrayList(),
					&isRead);
		if (!isRead || ids.contains(key)){
			return nullptr;
		}

		retVal->SetContractId(contractId);
		retVal->SetTenantId(tenantId);
	}

	IContractPartyPrivateInfoUniquePtr cachedPtr = m_privateFactoryCompPtr.CreateInstance();
	if (cachedPtr.IsValid() && cachedPtr->CopyFrom(*retVal)){
		QMutexLocker locker(&m_cacheMutex);
		if (m_cacheGeneration == generation){
			m_privateInfoCache.insert(key, IContractPartyPrivateInfoSharedPtr::CreateFromUnique(std::move(cachedPtr)));
		}
	}

	return retVal;
}


bool CContractManagerComp::StorePrivateInfo(const IContractPartyPrivateInfo& privateInfo)
{
	const QByteArray key = GetPrivateInfoKey(privateInfo.GetContractId(), privateInfo.GetTenantId());

	istd::CChangeNotifier changeNotifier(this);

	imtbase::IObjectCollection::DataPtr existingPtr;
	const bool isStored = m_privateCollectionCompPtr->GetObjectData(key, existingPtr)
				? m_privateCollectionCompPtr->SetObjectData(key, privateInfo)
				: !m_privateCollectionCompPtr->InsertNewObject("ContractPartyPrivateInfo", QString(), QString(), &privateInfo, key).isEmpty();

	IContractPartyPrivateInfoUniquePtr cachedPtr = m_privateFactoryCompPtr.CreateInstance();

	QMutexLocker locker(&m_cacheMutex);
	++m_cacheGeneration;
	if (isStored && cachedPtr.IsValid() && cachedPtr->CopyFrom(privateInfo)){
		m_privateInfoCache.insert(key, IContractPartyPrivateInfoSharedPtr::CreateFromUnique(std::move(cachedPtr)));
	}
	else{
		m_privateInfoCache.remove(key);
	}

	return isStored;
}


void CContractManagerComp::Announce(const IContractInfo& contract, const ChangeRecord& record)
{
	const QDateTime now = QDateTime::currentDateTimeUtc();
	for (const JournalEntry& entry : record.journal){
		IContractEventInfoUniquePtr eventPtr = m_eventFactoryCompPtr.CreateInstance();
		if (!eventPtr.IsValid()){
			continue;
		}

		const QByteArray eventId = CreateId();
		eventPtr->SetEventId(eventId);
		eventPtr->SetContractId(contract.GetContractId());
		eventPtr->SetEventType(entry.type);
		eventPtr->SetTenantId(entry.tenantId);
		eventPtr->SetUserId(entry.userId);
		eventPtr->SetUserName(entry.userName);
		eventPtr->SetTime(now);
		eventPtr->SetRevision(contract.GetRevision());
		eventPtr->SetComment(entry.comment);
		eventPtr->SetSubjectId(entry.subjectId);
		eventPtr->SetPrivate(entry.isPrivate);

		if (m_eventCollectionCompPtr->InsertNewObject("ContractEvent", QString(), QString(), eventPtr.GetPtr(), eventId).isEmpty()){
			SendErrorMessage(0, QStringLiteral("An entry of the journal of the contract '%1' could not be stored").arg(QString::fromUtf8(contract.GetContractId())), "CContractManagerComp");
		}
	}

	for (const QVariantMap& event : record.events){
		TenantEvent::Publish(this, event);
	}

	// Who changed the contract; a change of the private data of a party is told to that party only
	JournalEntry actor;
	bool isPrivate = !record.journal.isEmpty();
	for (const JournalEntry& entry : record.journal){
		if (!entry.userId.isEmpty()){
			actor = entry;
		}

		isPrivate = isPrivate && entry.isPrivate;
	}

	QVariantMap changed = CreateTenantEvent(
				TenantEvent::s_contractChanged,
				contract,
				QByteArray(),
				isPrivate ? QByteArrayList({actor.tenantId}) : GetContractPartyTenantIds(contract));
	changed.insert(TenantEvent::s_userId, actor.userId);
	changed.insert(TenantEvent::s_userName, actor.userName);
	TenantEvent::Publish(this, changed);
}


void CContractManagerComp::AddSchemeLinks(IContractInfo& contract, const QByteArray& tenantId) const
{
	const QByteArray initiatorTenantId = contract.GetInitiatorTenantId();
	bool isStar = false;
	QByteArray starRoleId;
	if (!contract.GetTypeId().isEmpty()){
		IContractTypeInfoUniquePtr typePtr = m_typeCatalogCompPtr->GetType(contract.GetTypeId());
		if (typePtr.IsValid()){
			isStar = typePtr->GetLinkScheme() == IContractTypeInfo::LS_STAR;
			starRoleId = typePtr->GetStarRoleId();
		}
	}

	const IContractInfo::Parties parties = contract.GetParties();
	QByteArray centerTenantId;
	if (isStar){
		for (const IContractInfo::Party& party : parties){
			if (party.roleId == starRoleId){
				centerTenantId = party.tenantId;

				break;
			}
		}

		if (centerTenantId.isEmpty() && !parties.isEmpty()){
			centerTenantId = parties.first().tenantId;
		}
	}

	const int partyIndex = FindContractParty(parties, tenantId);
	if (partyIndex < 0){
		return;
	}

	// A role of a party is its role on its links when the role is one of the links
	const QByteArray roleId = parties[partyIndex].roleId;
	const QString roleName = roleId.isEmpty() ? QString() : FindRoleName(roleId, initiatorTenantId, IPartyRoleInfo::RA_LINK);

	IContractInfo::Links links = contract.GetLinks();
	for (const IContractInfo::Party& other : parties){
		if (other.tenantId == tenantId || FindContractLinkOfPair(links, tenantId, other.tenantId) >= 0){
			continue;
		}

		if (isStar && tenantId != centerTenantId && other.tenantId != centerTenantId){
			continue;
		}

		IContractInfo::Link link;
		link.linkId = CreateId();
		link.tenantAId = other.tenantId;
		link.tenantBId = tenantId;
		link.roleAName = other.roleId.isEmpty() ? QString() : FindRoleName(other.roleId, initiatorTenantId, IPartyRoleInfo::RA_LINK);
		link.roleAId = link.roleAName.isEmpty() ? QByteArray() : other.roleId;
		link.roleBName = roleName;
		link.roleBId = roleName.isEmpty() ? QByteArray() : roleId;
		links.append(link);
	}

	contract.SetLinks(links);
}


bool CContractManagerComp::ApplyDraft(const IContractInfo& draft, IContractInfo& contract, QString& errorMessage) const
{
	const QByteArray initiatorTenantId = contract.GetInitiatorTenantId();
	const QDateTime now = QDateTime::currentDateTimeUtc();
	const bool isSubmitted = contract.GetSubmittedAt().isValid();

	contract.SetName(draft.GetName().trimmed());
	contract.SetDescription(draft.GetDescription());
	contract.SetTerms(draft.GetTerms());
	contract.SetEffectiveFrom(draft.GetEffectiveFrom());
	contract.SetTermDays(draft.GetTermDays());
	contract.SetValidUntil(draft.GetValidUntil());
	contract.SetAutoRenew(draft.IsAutoRenew());
	contract.SetRenewalPeriodDays(draft.GetRenewalPeriodDays());
	contract.SetRenewalNoticeDays(draft.GetRenewalNoticeDays());
	contract.SetTerminationNoticeDays(draft.GetTerminationNoticeDays());

	// The type is chosen with the draft; its slots are part of the terms
	if (draft.GetTypeId() != contract.GetTypeId() || contract.GetTypeName().isEmpty()){
		contract.SetTypeId(draft.GetTypeId());
		contract.SetTypeName(QString());
		contract.SetRoleSlots(IContractInfo::RoleSlots());
		if (!draft.GetTypeId().isEmpty()){
			IContractTypeInfoUniquePtr typePtr = m_typeCatalogCompPtr->GetType(draft.GetTypeId());
			if (!typePtr.IsValid() || typePtr->IsArchived()
						|| (!typePtr->GetOwnerTenantId().isEmpty() && typePtr->GetOwnerTenantId() != initiatorTenantId)){
				errorMessage = QStringLiteral("The type of the contract is not available to the organization");

				return false;
			}

			contract.SetTypeName(typePtr->GetName());
			contract.SetRoleSlots(typePtr->GetRoleSlots());
		}
	}

	const IContractInfo::Parties previousParties = contract.GetParties();
	IContractInfo::Parties parties;
	for (const IContractInfo::Party& draftParty : draft.GetParties()){
		if (draftParty.tenantId.isEmpty()){
			errorMessage = QStringLiteral("A party needs an organization");

			return false;
		}

		if (FindContractParty(parties, draftParty.tenantId) >= 0){
			errorMessage = QStringLiteral("An organization is a party once");

			return false;
		}

		IContractInfo::Party party;
		const int existingIndex = FindContractParty(previousParties, draftParty.tenantId);
		if (existingIndex >= 0){
			party = previousParties[existingIndex];
		}
		else{
			// A new party: only an organization the initiator knows
			if (!FindChannel(initiatorTenantId, draftParty.tenantId, party.channel)){
				errorMessage = QStringLiteral("The organization '%1' is not in the address book of the initiator, not a party of a common contract and not open to contracts of unknown organizations")
							.arg(QString::fromUtf8(draftParty.tenantId));

				return false;
			}

			party.tenantId = draftParty.tenantId;
			party.tenantName = FindTenantName(draftParty.tenantId);
			party.state = isSubmitted ? IContractInfo::PS_REVIEWING : IContractInfo::PS_INVITED;
			party.stateChangedAt = now;
		}

		party.roleId = draftParty.roleId;
		party.roleName.clear();
		if (!party.roleId.isEmpty()){
			party.roleName = FindRoleName(party.roleId, initiatorTenantId, IPartyRoleInfo::RA_PARTY);
			if (party.roleName.isEmpty()){
				errorMessage = QStringLiteral("The role '%1' is not available for a party").arg(QString::fromUtf8(party.roleId));

				return false;
			}
		}

		parties.append(party);
	}

	// The initiator stays a party of its contract
	if (FindContractParty(parties, initiatorTenantId) < 0){
		const int initiatorIndex = FindContractParty(previousParties, initiatorTenantId);
		if (initiatorIndex >= 0){
			parties.prepend(previousParties[initiatorIndex]);
		}
		else{
			IContractInfo::Party initiator;
			initiator.tenantId = initiatorTenantId;
			initiator.tenantName = FindTenantName(initiatorTenantId);
			initiator.channel = IContractInfo::PC_INITIATOR;
			initiator.state = isSubmitted ? IContractInfo::PS_REVIEWING : IContractInfo::PS_INVITED;
			initiator.stateChangedAt = now;
			parties.prepend(initiator);
		}
	}

	contract.SetParties(parties);

	IContractInfo::Links links;
	for (IContractInfo::Link link : draft.GetLinks()){
		if (link.linkId.isEmpty()){
			link.linkId = CreateId();
		}

		link.roleAName.clear();
		link.roleBName.clear();
		if (!link.roleAId.isEmpty()){
			link.roleAName = FindRoleName(link.roleAId, initiatorTenantId, IPartyRoleInfo::RA_LINK);
			if (link.roleAName.isEmpty()){
				errorMessage = QStringLiteral("The role '%1' is not available for a link").arg(QString::fromUtf8(link.roleAId));

				return false;
			}
		}

		if (!link.roleBId.isEmpty()){
			link.roleBName = FindRoleName(link.roleBId, initiatorTenantId, IPartyRoleInfo::RA_LINK);
			if (link.roleBName.isEmpty()){
				errorMessage = QStringLiteral("The role '%1' is not available for a link").arg(QString::fromUtf8(link.roleBId));

				return false;
			}
		}

		links.append(link);
	}

	contract.SetLinks(links);

	// The permissions a party gives are its own: they stay while their link does
	IContractInfo::Allocations allocations;
	for (const IContractInfo::Allocation& allocation : contract.GetAllocations()){
		if (FindContractLink(links, allocation.linkId) >= 0){
			allocations.append(allocation);
		}
	}

	contract.SetAllocations(allocations);

	IContractInfo::References references;
	for (IContractInfo::Reference reference : draft.GetReferences()){
		IContractInfoUniquePtr referencedPtr = ReadContract(reference.contractId);
		if (!referencedPtr.IsValid() || FindContractParty(referencedPtr->GetParties(), initiatorTenantId) < 0 || !referencedPtr->GetSubmittedAt().isValid()){
			errorMessage = QStringLiteral("The referenced contract is not a sent contract of the organization");

			return false;
		}

		reference.contractNumber = referencedPtr->GetNumber();
		reference.contractName = referencedPtr->GetName();
		references.append(reference);
	}

	contract.SetReferences(references);

	return CheckStructure(contract, errorMessage);
}


bool CContractManagerComp::CheckStructure(const IContractInfo& contract, QString& errorMessage) const
{
	if (contract.GetTermDays() < 0 || contract.GetRenewalPeriodDays() < 0 || contract.GetRenewalNoticeDays() < 0 || contract.GetTerminationNoticeDays() < 0){
		errorMessage = QStringLiteral("The periods of a contract are not negative");

		return false;
	}

	if (contract.GetValidUntil().isValid() && contract.GetEffectiveFrom().isValid() && contract.GetValidUntil() <= contract.GetEffectiveFrom()){
		errorMessage = QStringLiteral("The contract ends after it starts");

		return false;
	}

	const IContractInfo::Parties parties = contract.GetParties();
	const IContractInfo::Links links = contract.GetLinks();
	QSet<QByteArray> linkIds;
	for (int i = 0; i < links.size(); ++i){
		const IContractInfo::Link& link = links[i];
		if (linkIds.contains(link.linkId)){
			errorMessage = QStringLiteral("A link has a unique id");

			return false;
		}

		linkIds.insert(link.linkId);

		if (link.tenantAId == link.tenantBId || FindContractParty(parties, link.tenantAId) < 0 || FindContractParty(parties, link.tenantBId) < 0){
			errorMessage = QStringLiteral("A link connects two parties of the contract");

			return false;
		}

		for (int j = 0; j < i; ++j){
			if (links[j].Connects(link.tenantAId, link.tenantBId)){
				errorMessage = QStringLiteral("Two parties have one link");

				return false;
			}
		}

		if (link.validFrom.isValid() && link.validUntil.isValid() && link.validUntil <= link.validFrom){
			errorMessage = QStringLiteral("A link ends after it starts");

			return false;
		}
	}

	QSet<QByteArray> referencedIds;
	int frameworkCount = 0;
	for (const IContractInfo::Reference& reference : contract.GetReferences()){
		if (reference.contractId == contract.GetContractId() || referencedIds.contains(reference.contractId)){
			errorMessage = QStringLiteral("A contract references another contract once");

			return false;
		}

		referencedIds.insert(reference.contractId);
		if (reference.type == IContractInfo::RT_FRAMEWORK){
			++frameworkCount;
		}
	}

	if (frameworkCount > 1){
		errorMessage = QStringLiteral("A contract is concluded under one framework contract");

		return false;
	}

	return true;
}


bool CContractManagerComp::CheckSendable(const IContractInfo& contract, QString& errorMessage) const
{
	if (contract.GetName().isEmpty()){
		errorMessage = QStringLiteral("A contract needs a name");

		return false;
	}

	const IContractInfo::Parties parties = contract.GetParties();
	if (parties.size() < 2){
		errorMessage = QStringLiteral("A contract has at least two parties");

		return false;
	}

	const IContractInfo::Links links = contract.GetLinks();
	for (const IContractInfo::Party& party : parties){
		if (party.roleId.isEmpty()){
			errorMessage = QStringLiteral("Every party has a role");

			return false;
		}

		bool isLinked = false;
		for (const IContractInfo::Link& link : links){
			isLinked = isLinked || link.tenantAId == party.tenantId || link.tenantBId == party.tenantId;
		}

		if (!isLinked){
			errorMessage = QStringLiteral("The party '%1' has no link to another party").arg(party.tenantName);

			return false;
		}

		ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(party.tenantId);
		if (!tenantPtr.IsValid() || !tenantPtr->IsActive()){
			errorMessage = QStringLiteral("The party '%1' is not an active organization").arg(party.tenantName);

			return false;
		}
	}

	for (const IContractInfo::RoleSlot& slot : contract.GetRoleSlots()){
		int count = 0;
		for (const IContractInfo::Party& party : parties){
			if (party.roleId == slot.roleId){
				++count;
			}
		}

		if (count < slot.minCount || (slot.maxCount > 0 && count > slot.maxCount)){
			IPartyRoleInfoUniquePtr rolePtr = m_roleCatalogCompPtr->GetRole(slot.roleId);
			const QString roleName = rolePtr.IsValid() ? rolePtr->GetName() : QString::fromUtf8(slot.roleId);
			const QString countText = slot.maxCount <= 0
						? QStringLiteral("at least %1").arg(slot.minCount)
						: (slot.minCount == slot.maxCount
							? QString::number(slot.minCount)
							: QStringLiteral("%1 to %2").arg(QString::number(slot.minCount), QString::number(slot.maxCount)));
			errorMessage = QStringLiteral("The type of the contract needs %1 parties of the role '%2', the contract has %3").arg(countText, roleName, QString::number(count));

			return false;
		}
	}

	if (contract.IsAutoRenew() && (contract.GetRenewalPeriodDays() <= 0 || (contract.GetTermDays() <= 0 && !contract.GetValidUntil().isValid()))){
		errorMessage = QStringLiteral("A renewing contract has an end and the length of a renewal");

		return false;
	}

	return CheckReferences(contract, errorMessage);
}


bool CContractManagerComp::CheckReferences(const IContractInfo& contract, QString& errorMessage) const
{
	const QDateTime now = QDateTime::currentDateTimeUtc();
	const IContractInfo::Parties parties = contract.GetParties();

	for (const IContractInfo::Reference& reference : contract.GetReferences()){
		IContractInfoUniquePtr referencedPtr = ReadContract(reference.contractId);
		if (!referencedPtr.IsValid()){
			errorMessage = QStringLiteral("The referenced contract could not be read");

			return false;
		}

		const ContractState referencedState = ComputeContractState(*referencedPtr, now).state;
		if (reference.type == IContractInfo::RT_SUPERSEDES){
			if (!IsConcludedState(referencedState) || !referencedPtr->GetSupersededBy().isEmpty()){
				errorMessage = QStringLiteral("Only a concluded contract that was not replaced is superseded");

				return false;
			}

			// The replacement ends the old contract for all its parties: each of them agrees to it by being a party
			for (const IContractInfo::Party& party : referencedPtr->GetParties()){
				if (IsPartyBoundAt(*referencedPtr, party, now) && FindContractParty(parties, party.tenantId) < 0){
					errorMessage = QStringLiteral("The parties of the replaced contract are parties of the new one");

					return false;
				}
			}
		}
		else if (reference.type == IContractInfo::RT_FRAMEWORK){
			if (!IsConcludedState(referencedState)){
				errorMessage = QStringLiteral("A contract is concluded under a concluded framework contract");

				return false;
			}

			const IContractInfo::Parties frameworkParties = referencedPtr->GetParties();
			for (const IContractInfo::Party& party : parties){
				if (FindContractParty(frameworkParties, party.tenantId) < 0){
					errorMessage = QStringLiteral("The parties of a contract are parties of its framework contract");

					return false;
				}
			}

			const QDateTime frameworkEnd = GetContractPeriodEnd(*referencedPtr, now);
			if (frameworkEnd.isValid() && contract.GetValidUntil().isValid() && contract.GetValidUntil() > frameworkEnd){
				errorMessage = QStringLiteral("A contract ends with its framework contract at the latest");

				return false;
			}
		}
	}

	// No contract references itself through others
	QList<QByteArray> pendingIds;
	for (const IContractInfo::Reference& reference : contract.GetReferences()){
		pendingIds.append(reference.contractId);
	}

	QSet<QByteArray> visitedIds;
	while (!pendingIds.isEmpty()){
		const QByteArray contractId = pendingIds.takeFirst();
		if (contractId == contract.GetContractId()){
			errorMessage = QStringLiteral("The references of the contracts form a cycle");

			return false;
		}

		if (visitedIds.contains(contractId)){
			continue;
		}

		visitedIds.insert(contractId);

		IContractInfoUniquePtr referencedPtr = ReadContract(contractId);
		if (!referencedPtr.IsValid()){
			errorMessage = QStringLiteral("A referenced contract could not be read");

			return false;
		}

		for (const IContractInfo::Reference& reference : referencedPtr->GetReferences()){
			pendingIds.append(reference.contractId);
		}
	}

	return true;
}


bool CContractManagerComp::CheckAllocationScope(const IContractInfo& contract, const IContractInfo::Allocation& allocation, QString& errorMessage) const
{
	QSet<QByteArray> scope;
	for (const QByteArray& permissionId : m_tenantManagerCompPtr->GetTenantPermissions(allocation.grantorTenantId)){
		scope.insert(permissionId.trimmed());
	}

	for (const QByteArray& permissionId : allocation.permissionIds){
		if (!scope.contains(permissionId)){
			errorMessage = QStringLiteral("The permission '%1' is not available to your organization").arg(QString::fromUtf8(permissionId));

			return false;
		}
	}

	// A contract under a framework gives no more than the framework does between the same parties
	for (const IContractInfo::Reference& reference : contract.GetReferences()){
		if (reference.type != IContractInfo::RT_FRAMEWORK){
			continue;
		}

		IContractInfoUniquePtr frameworkPtr = ReadContract(reference.contractId);
		if (!frameworkPtr.IsValid()){
			errorMessage = QStringLiteral("The framework contract could not be read");

			return false;
		}

		QSet<QByteArray> frameworkPermissions;
		for (const IContractInfo::Allocation& frameworkAllocation : frameworkPtr->GetAllocations()){
			if (frameworkAllocation.grantorTenantId == allocation.grantorTenantId
						&& frameworkAllocation.granteeTenantId == allocation.granteeTenantId
						&& frameworkAllocation.productId == allocation.productId){
				for (const QByteArray& permissionId : frameworkAllocation.permissionIds){
					frameworkPermissions.insert(permissionId);
				}
			}
		}

		for (const QByteArray& permissionId : allocation.permissionIds){
			if (!frameworkPermissions.contains(permissionId)){
				errorMessage = QStringLiteral("The permission '%1' is not given by the framework contract").arg(QString::fromUtf8(permissionId));

				return false;
			}
		}
	}

	return true;
}


bool CContractManagerComp::CanPartyChangeAllocations(const IContractInfo& contract, const QByteArray& tenantId, QString& errorMessage) const
{
	const ContractState state = ComputeContractState(contract, QDateTime::currentDateTimeUtc()).state;
	if (state == CS_CANCELLED || state == CS_EXPIRED || state == CS_TERMINATED || state == CS_SUPERSEDED){
		errorMessage = QStringLiteral("The permissions of an ended contract stay as they were");

		return false;
	}

	const IContractInfo::Parties parties = contract.GetParties();
	const int partyIndex = FindContractParty(parties, tenantId);
	if (partyIndex < 0 || parties[partyIndex].state == IContractInfo::PS_DECLINED || parties[partyIndex].state == IContractInfo::PS_WITHDRAWN){
		errorMessage = QStringLiteral("A party that declined or left the contract gives no permissions");

		return false;
	}

	return true;
}


void CContractManagerComp::RestartReview(IContractInfo& contract) const
{
	contract.SetRevision(contract.GetRevision() + 1);

	if (!contract.GetSubmittedAt().isValid()){
		return;
	}

	const QDateTime now = QDateTime::currentDateTimeUtc();
	IContractInfo::Parties parties = contract.GetParties();
	for (IContractInfo::Party& party : parties){
		// A refusal stays until the initiator removes the party
		if (party.state != IContractInfo::PS_DECLINED){
			party.state = IContractInfo::PS_REVIEWING;
			party.stateComment.clear();
			party.stateChangedAt = now;
		}

		party.signatures.clear();
	}

	contract.SetParties(parties);
}


bool CContractManagerComp::Conclude(IContractInfo& contract, ChangeRecord& record, QString& errorMessage)
{
	if (!CheckSendable(contract, errorMessage)){
		return false;
	}

	contract.SetSignedAt(QDateTime::currentDateTimeUtc());

	JournalEntry concluded;
	concluded.type = IContractEventInfo::ET_CONCLUDED;
	record.journal.append(concluded);
	record.events.append(CreateTenantEvent(TenantEvent::s_contractConcluded, contract, QByteArray(), GetContractPartyTenantIds(contract)));

	const QByteArrayList granteeTenantIds = GetGranteeTenantIds(contract, QByteArray());
	if (!granteeTenantIds.isEmpty()){
		record.events.append(CreateTenantEvent(TenantEvent::s_contractPermissionsReceived, contract, QByteArray(), granteeTenantIds));
	}

	record.isConcluded = true;

	return true;
}


void CContractManagerComp::CompleteConclusion(const IContractInfo& contract)
{
	const QDateTime now = QDateTime::currentDateTimeUtc();

	// The superseded contracts end when this one comes into force, so the parties keep their access in between
	const QDateTime effectiveStart = GetContractEffectiveStart(contract);
	for (const IContractInfo::Reference& reference : contract.GetReferences()){
		if (reference.type != IContractInfo::RT_SUPERSEDES){
			continue;
		}

		IContractInfoUniquePtr supersededPtr = ReadContract(reference.contractId);
		if (!supersededPtr.IsValid()){
			SendErrorMessage(0, QStringLiteral("The contract '%1' superseded by '%2' could not be read").arg(QString::fromUtf8(reference.contractId), QString::fromUtf8(contract.GetContractId())), "CContractManagerComp");

			continue;
		}

		supersededPtr->SetSupersededBy(contract.GetContractId());
		supersededPtr->SetSupersededAt(effectiveStart);
		supersededPtr->SetUpdatedAt(now);
		if (!StoreContract(*supersededPtr, false)){
			SendErrorMessage(0, QStringLiteral("The contract '%1' superseded by '%2' could not be stored").arg(QString::fromUtf8(reference.contractId), QString::fromUtf8(contract.GetContractId())), "CContractManagerComp");

			continue;
		}

		ChangeRecord supersededRecord;
		JournalEntry superseded;
		superseded.type = IContractEventInfo::ET_SUPERSEDED;
		superseded.subjectId = contract.GetContractId();
		supersededRecord.journal.append(superseded);
		supersededRecord.events.append(CreateTenantEvent(TenantEvent::s_contractSuperseded, *supersededPtr, QByteArray(), GetContractPartyTenantIds(*supersededPtr)));
		Announce(*supersededPtr, supersededRecord);
	}

	// The parties of a link know each other now
	for (const IContractInfo::Link& link : contract.GetLinks()){
		if (m_connectionManagerCompPtr->EnsureConnection(link.tenantAId, link.tenantBId).isEmpty()){
			SendWarningMessage(0, QStringLiteral("The organizations of a link of the contract '%1' could not be connected").arg(QString::fromUtf8(contract.GetContractId())), "CContractManagerComp");
		}
	}
}


bool CContractManagerComp::FindChannel(const QByteArray& initiatorTenantId, const QByteArray& tenantId, IContractInfo::PartyChannel& channel) const
{
	if (tenantId == initiatorTenantId){
		channel = IContractInfo::PC_INITIATOR;

		return true;
	}

	if (m_connectionManagerCompPtr->AreConnected(initiatorTenantId, tenantId)){
		channel = IContractInfo::PC_ADDRESS_BOOK;

		return true;
	}

	if (HasCommonContract(initiatorTenantId, tenantId)){
		channel = IContractInfo::PC_CO_PARTY;

		return true;
	}

	ITenantDirectoryProfileUniquePtr profilePtr = m_directoryCompPtr->GetProfile(tenantId);
	if (profilePtr.IsValid() && profilePtr->IsListed() && profilePtr->IsAcceptingInvitationsFromUnknown()){
		channel = IContractInfo::PC_DIRECTORY;

		return true;
	}

	return false;
}


bool CContractManagerComp::HasCommonContract(const QByteArray& tenantId, const QByteArray& otherTenantId) const
{
	const QDateTime now = QDateTime::currentDateTimeUtc();
	for (const IContractInfoSharedPtr& contractPtr : GetContractsOfParty(tenantId)){
		if (FindContractParty(contractPtr->GetParties(), otherTenantId) >= 0 && IsConcludedState(ComputeContractState(*contractPtr, now).state)){
			return true;
		}
	}

	return false;
}


bool CContractManagerComp::CheckInvitationLimit(const QByteArray& initiatorTenantId, int unknownCount, QString& errorMessage)
{
	if (unknownCount <= 0){
		return true;
	}

	// Only the invitations of the last hour count; an initiator without any is forgotten
	const QDateTime now = QDateTime::currentDateTimeUtc();
	auto invitationsIter = m_unknownInvitations.find(initiatorTenantId);
	if (invitationsIter != m_unknownInvitations.end()){
		invitationsIter->removeIf([&now](const QDateTime& time){
			return time.secsTo(now) > 3600;
		});

		if (invitationsIter->isEmpty()){
			m_unknownInvitations.erase(invitationsIter);
			invitationsIter = m_unknownInvitations.end();
		}
	}

	const int recentCount = (invitationsIter != m_unknownInvitations.end()) ? int(invitationsIter->size()) : 0;
	if (recentCount + unknownCount > *m_invitationLimitAttrPtr){
		errorMessage = QStringLiteral("The organization invited too many unknown organizations in the last hour");

		return false;
	}

	return true;
}


void CContractManagerComp::RecordInvitations(const QByteArray& initiatorTenantId, int unknownCount)
{
	if (unknownCount <= 0){
		return;
	}

	QList<QDateTime>& invitations = m_unknownInvitations[initiatorTenantId];
	const QDateTime now = QDateTime::currentDateTimeUtc();
	for (int i = 0; i < unknownCount; ++i){
		invitations.append(now);
	}
}


QString CContractManagerComp::FindRoleName(const QByteArray& roleId, const QByteArray& initiatorTenantId, IPartyRoleInfo::Applicability use) const
{
	IPartyRoleInfoUniquePtr rolePtr = m_roleCatalogCompPtr->GetRole(roleId);
	if (!rolePtr.IsValid() || rolePtr->IsArchived() || (rolePtr->GetApplicability() & use) == 0){
		return QString();
	}

	const QByteArray ownerTenantId = rolePtr->GetOwnerTenantId();
	if (!ownerTenantId.isEmpty() && ownerTenantId != initiatorTenantId){
		return QString();
	}

	return rolePtr->GetName();
}


QString CContractManagerComp::FindTenantName(const QByteArray& tenantId) const
{
	ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);

	return tenantPtr.IsValid() ? tenantPtr->GetTenantName() : QString();
}


QString CContractManagerComp::CreateNumber(const IContractInfo& contract) const
{
	QString prefix = QStringLiteral("C");
	if (!contract.GetTypeId().isEmpty()){
		IContractTypeInfoUniquePtr typePtr = m_typeCatalogCompPtr->GetType(contract.GetTypeId());
		if (typePtr.IsValid() && !typePtr->GetNumberPrefix().isEmpty()){
			prefix = typePtr->GetNumberPrefix();
		}
	}

	const QString start = QStringLiteral("%1-%2-").arg(prefix).arg(QDateTime::currentDateTimeUtc().date().year());

	// The highest number of the year continues; a removed draft never had one
	int lastNumber = 0;
	for (const IContractInfoSharedPtr& otherContractPtr : GetContractsOfParty(contract.GetInitiatorTenantId())){
		if (otherContractPtr->GetInitiatorTenantId() == contract.GetInitiatorTenantId() && otherContractPtr->GetNumber().startsWith(start)){
			lastNumber = qMax(lastNumber, otherContractPtr->GetNumber().mid(start.size()).toInt());
		}
	}

	return start + QStringLiteral("%1").arg(lastNumber + 1, 4, 10, QLatin1Char('0'));
}


bool CContractManagerComp::CheckRevision(const IContractInfo& contract, int expectedRevision, QString& errorMessage)
{
	if (contract.GetRevision() != expectedRevision){
		errorMessage = QStringLiteral("The contract was changed meanwhile; read it again");

		return false;
	}

	return true;
}


CContractManagerComp::JournalEntry CContractManagerComp::CreateEntry(const ContractActor& actor, IContractEventInfo::EventType type, const QString& comment)
{
	JournalEntry retVal;
	retVal.type = type;
	retVal.tenantId = actor.tenantId;
	retVal.userId = actor.userId;
	retVal.userName = actor.userName;
	retVal.comment = comment;

	return retVal;
}


QVariantMap CContractManagerComp::CreateTenantEvent(const QString& type, const IContractInfo& contract, const QByteArray& actorTenantId, const QByteArrayList& recipientTenantIds)
{
	QStringList recipients;
	for (const QByteArray& tenantId : recipientTenantIds){
		if (tenantId != actorTenantId){
			recipients.append(QString::fromUtf8(tenantId));
		}
	}

	return {
		{TenantEvent::s_type, type},
		{TenantEvent::s_contractId, contract.GetContractId()},
		{TenantEvent::s_sourceTenantId, actorTenantId},
		{TenantEvent::s_recipientTenantIds, recipients}};
}


} // namespace imtauth


