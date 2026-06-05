// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantRelationshipProposal.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtauth
{


CTenantRelationshipProposal::CTenantRelationshipProposal()
: m_proposalType(RPT_CREATE)
, m_proposedSourceRole(ITenantRelationshipInfo::TRR_PARTNER)
, m_proposedTargetRole(ITenantRelationshipInfo::TRR_PARTNER)
, m_status(RPS_PENDING)
{
}


QByteArray CTenantRelationshipProposal::GetProposalId() const
{
return m_proposalId;
}


void CTenantRelationshipProposal::SetProposalId(const QByteArray& proposalId)
{
if (m_proposalId != proposalId){
istd::CChangeNotifier notifier(this);
m_proposalId = proposalId;
}
}


QByteArray CTenantRelationshipProposal::GetConnectionId() const
{
return m_connectionId;
}


void CTenantRelationshipProposal::SetConnectionId(const QByteArray& connectionId)
{
if (m_connectionId != connectionId){
istd::CChangeNotifier notifier(this);
m_connectionId = connectionId;
}
}


QByteArray CTenantRelationshipProposal::GetExistingRelationshipId() const
{
return m_existingRelationshipId;
}


void CTenantRelationshipProposal::SetExistingRelationshipId(const QByteArray& existingRelationshipId)
{
if (m_existingRelationshipId != existingRelationshipId){
istd::CChangeNotifier notifier(this);
m_existingRelationshipId = existingRelationshipId;
}
}


CTenantRelationshipProposal::RelationshipProposalType CTenantRelationshipProposal::GetProposalType() const
{
return m_proposalType;
}


void CTenantRelationshipProposal::SetProposalType(RelationshipProposalType proposalType)
{
if (m_proposalType != proposalType){
istd::CChangeNotifier notifier(this);
m_proposalType = proposalType;
}
}


QByteArray CTenantRelationshipProposal::GetInitiatorTenantId() const
{
return m_initiatorTenantId;
}


void CTenantRelationshipProposal::SetInitiatorTenantId(const QByteArray& initiatorTenantId)
{
if (m_initiatorTenantId != initiatorTenantId){
istd::CChangeNotifier notifier(this);
m_initiatorTenantId = initiatorTenantId;
}
}


QByteArray CTenantRelationshipProposal::GetCounterpartyTenantId() const
{
return m_counterpartyTenantId;
}


void CTenantRelationshipProposal::SetCounterpartyTenantId(const QByteArray& counterpartyTenantId)
{
if (m_counterpartyTenantId != counterpartyTenantId){
istd::CChangeNotifier notifier(this);
m_counterpartyTenantId = counterpartyTenantId;
}
}


ITenantRelationshipInfo::TenantRelationshipRole CTenantRelationshipProposal::GetProposedSourceRole() const
{
return m_proposedSourceRole;
}


void CTenantRelationshipProposal::SetProposedSourceRole(ITenantRelationshipInfo::TenantRelationshipRole role)
{
if (m_proposedSourceRole != role){
istd::CChangeNotifier notifier(this);
m_proposedSourceRole = role;
}
}


ITenantRelationshipInfo::TenantRelationshipRole CTenantRelationshipProposal::GetProposedTargetRole() const
{
return m_proposedTargetRole;
}


void CTenantRelationshipProposal::SetProposedTargetRole(ITenantRelationshipInfo::TenantRelationshipRole role)
{
if (m_proposedTargetRole != role){
istd::CChangeNotifier notifier(this);
m_proposedTargetRole = role;
}
}


QString CTenantRelationshipProposal::GetProposedScope() const
{
return m_proposedScope;
}


void CTenantRelationshipProposal::SetProposedScope(const QString& scope)
{
if (m_proposedScope != scope){
istd::CChangeNotifier notifier(this);
m_proposedScope = scope;
}
}


QString CTenantRelationshipProposal::GetProposedDescription() const
{
return m_proposedDescription;
}


void CTenantRelationshipProposal::SetProposedDescription(const QString& description)
{
if (m_proposedDescription != description){
istd::CChangeNotifier notifier(this);
m_proposedDescription = description;
}
}


QString CTenantRelationshipProposal::GetProposedValidFrom() const
{
return m_proposedValidFrom;
}


void CTenantRelationshipProposal::SetProposedValidFrom(const QString& validFrom)
{
if (m_proposedValidFrom != validFrom){
istd::CChangeNotifier notifier(this);
m_proposedValidFrom = validFrom;
}
}


QString CTenantRelationshipProposal::GetProposedValidUntil() const
{
return m_proposedValidUntil;
}


void CTenantRelationshipProposal::SetProposedValidUntil(const QString& validUntil)
{
if (m_proposedValidUntil != validUntil){
istd::CChangeNotifier notifier(this);
m_proposedValidUntil = validUntil;
}
}


CTenantRelationshipProposal::RelationshipProposalStatus CTenantRelationshipProposal::GetStatus() const
{
return m_status;
}


void CTenantRelationshipProposal::SetStatus(RelationshipProposalStatus status)
{
if (m_status != status){
istd::CChangeNotifier notifier(this);
m_status = status;
}
}


QString CTenantRelationshipProposal::GetMessage() const
{
return m_message;
}


void CTenantRelationshipProposal::SetMessage(const QString& message)
{
if (m_message != message){
istd::CChangeNotifier notifier(this);
m_message = message;
}
}


QString CTenantRelationshipProposal::GetCreatedAt() const
{
return m_createdAt;
}


void CTenantRelationshipProposal::SetCreatedAt(const QString& createdAt)
{
if (m_createdAt != createdAt){
istd::CChangeNotifier notifier(this);
m_createdAt = createdAt;
}
}


QString CTenantRelationshipProposal::GetUpdatedAt() const
{
return m_updatedAt;
}


void CTenantRelationshipProposal::SetUpdatedAt(const QString& updatedAt)
{
if (m_updatedAt != updatedAt){
istd::CChangeNotifier notifier(this);
m_updatedAt = updatedAt;
}
}


bool CTenantRelationshipProposal::Serialize(iser::IArchive& archive)
{
istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

bool retVal = true;

iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(idTag);
retVal = retVal && archive.Process(m_proposalId);
retVal = retVal && archive.EndTag(idTag);

iser::CArchiveTag connectionIdTag("ConnectionId", "Connection ID", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(connectionIdTag);
retVal = retVal && archive.Process(m_connectionId);
retVal = retVal && archive.EndTag(connectionIdTag);

iser::CArchiveTag existingRelTag("ExistingRelationshipId", "Existing relationship ID", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(existingRelTag);
retVal = retVal && archive.Process(m_existingRelationshipId);
retVal = retVal && archive.EndTag(existingRelTag);

iser::CArchiveTag typeTag("ProposalType", "Proposal type", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(typeTag);
int proposalType = static_cast<int>(m_proposalType);
retVal = retVal && archive.Process(proposalType);
m_proposalType = static_cast<RelationshipProposalType>(proposalType);
retVal = retVal && archive.EndTag(typeTag);

iser::CArchiveTag initiatorTag("InitiatorTenantId", "Initiator tenant ID", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(initiatorTag);
retVal = retVal && archive.Process(m_initiatorTenantId);
retVal = retVal && archive.EndTag(initiatorTag);

iser::CArchiveTag counterpartyTag("CounterpartyTenantId", "Counterparty tenant ID", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(counterpartyTag);
retVal = retVal && archive.Process(m_counterpartyTenantId);
retVal = retVal && archive.EndTag(counterpartyTag);

iser::CArchiveTag sourceRoleTag("ProposedSourceRole", "Proposed source role", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(sourceRoleTag);
int sourceRole = static_cast<int>(m_proposedSourceRole);
retVal = retVal && archive.Process(sourceRole);
m_proposedSourceRole = static_cast<ITenantRelationshipInfo::TenantRelationshipRole>(sourceRole);
retVal = retVal && archive.EndTag(sourceRoleTag);

iser::CArchiveTag targetRoleTag("ProposedTargetRole", "Proposed target role", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(targetRoleTag);
int targetRole = static_cast<int>(m_proposedTargetRole);
retVal = retVal && archive.Process(targetRole);
m_proposedTargetRole = static_cast<ITenantRelationshipInfo::TenantRelationshipRole>(targetRole);
retVal = retVal && archive.EndTag(targetRoleTag);

iser::CArchiveTag scopeTag("ProposedScope", "Proposed scope", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(scopeTag);
retVal = retVal && archive.Process(m_proposedScope);
retVal = retVal && archive.EndTag(scopeTag);

iser::CArchiveTag descriptionTag("ProposedDescription", "Proposed description", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(descriptionTag);
retVal = retVal && archive.Process(m_proposedDescription);
retVal = retVal && archive.EndTag(descriptionTag);

iser::CArchiveTag validFromTag("ProposedValidFrom", "Proposed valid from", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(validFromTag);
retVal = retVal && archive.Process(m_proposedValidFrom);
retVal = retVal && archive.EndTag(validFromTag);

iser::CArchiveTag validUntilTag("ProposedValidUntil", "Proposed valid until", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(validUntilTag);
retVal = retVal && archive.Process(m_proposedValidUntil);
retVal = retVal && archive.EndTag(validUntilTag);

iser::CArchiveTag statusTag("Status", "Status", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(statusTag);
int status = static_cast<int>(m_status);
retVal = retVal && archive.Process(status);
m_status = static_cast<RelationshipProposalStatus>(status);
retVal = retVal && archive.EndTag(statusTag);

iser::CArchiveTag messageTag("Message", "Message", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(messageTag);
retVal = retVal && archive.Process(m_message);
retVal = retVal && archive.EndTag(messageTag);

iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(createdAtTag);
retVal = retVal && archive.Process(m_createdAt);
retVal = retVal && archive.EndTag(createdAtTag);

iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(updatedAtTag);
retVal = retVal && archive.Process(m_updatedAt);
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
m_proposalId = sourcePtr->m_proposalId;
m_connectionId = sourcePtr->m_connectionId;
m_existingRelationshipId = sourcePtr->m_existingRelationshipId;
m_proposalType = sourcePtr->m_proposalType;
m_initiatorTenantId = sourcePtr->m_initiatorTenantId;
m_counterpartyTenantId = sourcePtr->m_counterpartyTenantId;
m_proposedSourceRole = sourcePtr->m_proposedSourceRole;
m_proposedTargetRole = sourcePtr->m_proposedTargetRole;
m_proposedScope = sourcePtr->m_proposedScope;
m_proposedDescription = sourcePtr->m_proposedDescription;
m_proposedValidFrom = sourcePtr->m_proposedValidFrom;
m_proposedValidUntil = sourcePtr->m_proposedValidUntil;
m_status = sourcePtr->m_status;
m_message = sourcePtr->m_message;
m_createdAt = sourcePtr->m_createdAt;
m_updatedAt = sourcePtr->m_updatedAt;

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
m_proposalId.clear();
m_connectionId.clear();
m_existingRelationshipId.clear();
m_proposalType = RPT_CREATE;
m_initiatorTenantId.clear();
m_counterpartyTenantId.clear();
m_proposedSourceRole = ITenantRelationshipInfo::TRR_PARTNER;
m_proposedTargetRole = ITenantRelationshipInfo::TRR_PARTNER;
m_proposedScope.clear();
m_proposedDescription.clear();
m_proposedValidFrom.clear();
m_proposedValidUntil.clear();
m_status = RPS_PENDING;
m_message.clear();
m_createdAt.clear();
m_updatedAt.clear();

return true;
}


} // namespace imtauth
