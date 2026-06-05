// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantRelationshipProposal.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


CTenantRelationshipProposal::CTenantRelationshipProposal()
{
	m_info.status = RPS_PENDING;
	m_info.proposalType = RPT_CREATE;
}


RelationshipProposalInfo CTenantRelationshipProposal::GetProposalInfo() const
{
	return m_info;
}


void CTenantRelationshipProposal::SetProposalInfo(const RelationshipProposalInfo& proposalInfo)
{
	if (!(m_info == proposalInfo)){
		istd::CChangeNotifier notifier(this);
		m_info = proposalInfo;
	}
}


QByteArray CTenantRelationshipProposal::GetProposalId() const
{
	return m_info.proposalId;
}


bool CTenantRelationshipProposal::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_info.proposalId);
	retVal = retVal && archive.EndTag(idTag);

	iser::CArchiveTag connectionIdTag("ConnectionId", "Connection ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(connectionIdTag);
	retVal = retVal && archive.Process(m_info.connectionId);
	retVal = retVal && archive.EndTag(connectionIdTag);

	iser::CArchiveTag existingRelTag("ExistingRelationshipId", "Existing relationship ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(existingRelTag);
	retVal = retVal && archive.Process(m_info.existingRelationshipId);
	retVal = retVal && archive.EndTag(existingRelTag);

	iser::CArchiveTag typeTag("ProposalType", "Proposal type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(typeTag);
	int proposalType = static_cast<int>(m_info.proposalType);
	retVal = retVal && archive.Process(proposalType);
	m_info.proposalType = static_cast<RelationshipProposalType>(proposalType);
	retVal = retVal && archive.EndTag(typeTag);

	iser::CArchiveTag initiatorTag("InitiatorTenantId", "Initiator tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(initiatorTag);
	retVal = retVal && archive.Process(m_info.initiatorTenantId);
	retVal = retVal && archive.EndTag(initiatorTag);

	iser::CArchiveTag counterpartyTag("CounterpartyTenantId", "Counterparty tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(counterpartyTag);
	retVal = retVal && archive.Process(m_info.counterpartyTenantId);
	retVal = retVal && archive.EndTag(counterpartyTag);

	iser::CArchiveTag sourceRoleTag("ProposedSourceRole", "Proposed source role", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sourceRoleTag);
	int sourceRole = static_cast<int>(m_info.proposedSourceRole);
	retVal = retVal && archive.Process(sourceRole);
	m_info.proposedSourceRole = static_cast<ITenantRelationshipInfo::TenantRelationshipRole>(sourceRole);
	retVal = retVal && archive.EndTag(sourceRoleTag);

	iser::CArchiveTag targetRoleTag("ProposedTargetRole", "Proposed target role", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetRoleTag);
	int targetRole = static_cast<int>(m_info.proposedTargetRole);
	retVal = retVal && archive.Process(targetRole);
	m_info.proposedTargetRole = static_cast<ITenantRelationshipInfo::TenantRelationshipRole>(targetRole);
	retVal = retVal && archive.EndTag(targetRoleTag);

	iser::CArchiveTag scopeTag("ProposedScope", "Proposed scope", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(scopeTag);
	retVal = retVal && archive.Process(m_info.proposedScope);
	retVal = retVal && archive.EndTag(scopeTag);

	iser::CArchiveTag descriptionTag("ProposedDescription", "Proposed description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_info.proposedDescription);
	retVal = retVal && archive.EndTag(descriptionTag);

	iser::CArchiveTag validFromTag("ProposedValidFrom", "Proposed valid from", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(validFromTag);
	retVal = retVal && archive.Process(m_info.proposedValidFrom);
	retVal = retVal && archive.EndTag(validFromTag);

	iser::CArchiveTag validUntilTag("ProposedValidUntil", "Proposed valid until", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(validUntilTag);
	retVal = retVal && archive.Process(m_info.proposedValidUntil);
	retVal = retVal && archive.EndTag(validUntilTag);

	iser::CArchiveTag statusTag("Status", "Status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	int status = static_cast<int>(m_info.status);
	retVal = retVal && archive.Process(status);
	m_info.status = static_cast<RelationshipProposalStatus>(status);
	retVal = retVal && archive.EndTag(statusTag);

	iser::CArchiveTag messageTag("Message", "Message", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(messageTag);
	retVal = retVal && archive.Process(m_info.message);
	retVal = retVal && archive.EndTag(messageTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_info.createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && archive.Process(m_info.updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	return retVal;
}


bool CTenantRelationshipProposal::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTenantRelationshipProposal* sourcePtr = dynamic_cast<const CTenantRelationshipProposal*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);
	m_info = sourcePtr->m_info;

	return true;
}


istd::IChangeableUniquePtr CTenantRelationshipProposal::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTenantRelationshipProposal);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTenantRelationshipProposal::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);
	m_info = RelationshipProposalInfo();

	return true;
}


} // namespace imtauth
