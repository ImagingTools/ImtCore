// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CContractInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtauth/ArchiveFields.h>


namespace imtauth
{


namespace
{


namespace af = ArchiveFields;


template <typename Value>
void AssignValue(istd::IChangeable& object, Value& member, const Value& value)
{
	if (member != value){
		istd::CChangeNotifier notifier(&object);

		member = value;
	}
}


bool SerializeSignature(iser::IArchive& archive, IContractInfo::Signature& item)
{
	bool retVal = af::Leaf(archive, "UserId", item.userId);
	retVal = retVal && af::Leaf(archive, "UserName", item.userName);
	retVal = retVal && af::Leaf(archive, "Position", item.position);
	retVal = retVal && af::Leaf(archive, "SignedAt", item.signedAt);
	retVal = retVal && af::Leaf(archive, "Revision", item.revision);
	retVal = retVal && af::Leaf(archive, "ContentHash", item.contentHash);

	return retVal;
}


bool SerializeParty(iser::IArchive& archive, IContractInfo::Party& item)
{
	bool retVal = af::Leaf(archive, "TenantId", item.tenantId);
	retVal = retVal && af::Leaf(archive, "TenantName", item.tenantName);
	retVal = retVal && af::Leaf(archive, "RoleId", item.roleId);
	retVal = retVal && af::Leaf(archive, "RoleName", item.roleName);
	retVal = retVal && af::EnumLeaf(archive, "Channel", item.channel);
	retVal = retVal && af::EnumLeaf(archive, "State", item.state);
	retVal = retVal && af::Leaf(archive, "StateComment", item.stateComment);
	retVal = retVal && af::Leaf(archive, "StateChangedAt", item.stateChangedAt);
	retVal = retVal && af::List(archive, "Signatures", "Signature", item.signatures, SerializeSignature);
	retVal = retVal && af::Leaf(archive, "WithdrawAt", item.withdrawAt);
	retVal = retVal && af::Leaf(archive, "WithdrawReason", item.withdrawReason);
	retVal = retVal && af::Leaf(archive, "NonRenewalNoticeAt", item.nonRenewalNoticeAt);
	retVal = retVal && af::Leaf(archive, "AllocationsSuspended", item.allocationsSuspended);

	return retVal;
}


bool SerializeLink(iser::IArchive& archive, IContractInfo::Link& item)
{
	bool retVal = af::Leaf(archive, "LinkId", item.linkId);
	retVal = retVal && af::Leaf(archive, "TenantAId", item.tenantAId);
	retVal = retVal && af::Leaf(archive, "TenantBId", item.tenantBId);
	retVal = retVal && af::Leaf(archive, "RoleAId", item.roleAId);
	retVal = retVal && af::Leaf(archive, "RoleAName", item.roleAName);
	retVal = retVal && af::Leaf(archive, "RoleBId", item.roleBId);
	retVal = retVal && af::Leaf(archive, "RoleBName", item.roleBName);
	retVal = retVal && af::Leaf(archive, "ValidFrom", item.validFrom);
	retVal = retVal && af::Leaf(archive, "ValidUntil", item.validUntil);
	retVal = retVal && af::Leaf(archive, "IsMessagingAllowed", item.isMessagingAllowed);
	retVal = retVal && af::Leaf(archive, "IsSharingAllowed", item.isSharingAllowed);

	return retVal;
}


bool SerializeAllocation(iser::IArchive& archive, IContractInfo::Allocation& item)
{
	bool retVal = af::Leaf(archive, "AllocationId", item.allocationId);
	retVal = retVal && af::Leaf(archive, "LinkId", item.linkId);
	retVal = retVal && af::Leaf(archive, "GrantorTenantId", item.grantorTenantId);
	retVal = retVal && af::Leaf(archive, "GranteeTenantId", item.granteeTenantId);
	retVal = retVal && af::Leaf(archive, "ProductId", item.productId);
	retVal = retVal && af::Leaf(archive, "PermissionIds", item.permissionIds);
	retVal = retVal && af::Leaf(archive, "ValidFrom", item.validFrom);
	retVal = retVal && af::Leaf(archive, "ValidUntil", item.validUntil);
	retVal = retVal && af::Leaf(archive, "Comment", item.comment);

	return retVal;
}


bool SerializeReference(iser::IArchive& archive, IContractInfo::Reference& item)
{
	bool retVal = af::Leaf(archive, "ContractId", item.contractId);
	retVal = retVal && af::EnumLeaf(archive, "Type", item.type);
	retVal = retVal && af::Leaf(archive, "ContractNumber", item.contractNumber);
	retVal = retVal && af::Leaf(archive, "ContractName", item.contractName);

	return retVal;
}


bool SerializeRoleSlot(iser::IArchive& archive, IContractInfo::RoleSlot& item)
{
	bool retVal = af::Leaf(archive, "RoleId", item.roleId);
	retVal = retVal && af::Leaf(archive, "MinCount", item.minCount);
	retVal = retVal && af::Leaf(archive, "MaxCount", item.maxCount);

	return retVal;
}


bool SerializeAttachment(iser::IArchive& archive, IContractInfo::Attachment& item)
{
	bool retVal = af::Leaf(archive, "AttachmentId", item.attachmentId);
	retVal = retVal && af::Leaf(archive, "FileName", item.fileName);
	retVal = retVal && af::Leaf(archive, "MimeType", item.mimeType);
	retVal = retVal && af::Leaf(archive, "Size", item.size);
	retVal = retVal && af::Leaf(archive, "ContentHash", item.contentHash);
	retVal = retVal && af::Leaf(archive, "AddedByTenantId", item.addedByTenantId);
	retVal = retVal && af::Leaf(archive, "AddedByUserId", item.addedByUserId);
	retVal = retVal && af::Leaf(archive, "AddedAt", item.addedAt);
	retVal = retVal && af::Leaf(archive, "ReplacesAttachmentId", item.replacesAttachmentId);
	retVal = retVal && af::Leaf(archive, "IsReplaced", item.isReplaced);

	return retVal;
}


} // anonymous namespace


// public methods of the types of IContractInfo

bool IContractInfo::Signature::operator==(const Signature& other) const
{
	return userId == other.userId
				&& userName == other.userName
				&& position == other.position
				&& signedAt == other.signedAt
				&& revision == other.revision
				&& contentHash == other.contentHash;
}


bool IContractInfo::Signature::operator!=(const Signature& other) const
{
	return !(*this == other);
}


bool IContractInfo::Party::operator==(const Party& other) const
{
	return tenantId == other.tenantId
				&& tenantName == other.tenantName
				&& roleId == other.roleId
				&& roleName == other.roleName
				&& channel == other.channel
				&& state == other.state
				&& stateComment == other.stateComment
				&& stateChangedAt == other.stateChangedAt
				&& signatures == other.signatures
				&& withdrawAt == other.withdrawAt
				&& withdrawReason == other.withdrawReason
				&& nonRenewalNoticeAt == other.nonRenewalNoticeAt
				&& allocationsSuspended == other.allocationsSuspended;
}


bool IContractInfo::Party::operator!=(const Party& other) const
{
	return !(*this == other);
}


bool IContractInfo::Link::Connects(const QByteArray& tenantId, const QByteArray& otherTenantId) const
{
	return (tenantAId == tenantId && tenantBId == otherTenantId) || (tenantAId == otherTenantId && tenantBId == tenantId);
}


bool IContractInfo::Link::operator==(const Link& other) const
{
	return linkId == other.linkId
				&& tenantAId == other.tenantAId
				&& tenantBId == other.tenantBId
				&& roleAId == other.roleAId
				&& roleAName == other.roleAName
				&& roleBId == other.roleBId
				&& roleBName == other.roleBName
				&& validFrom == other.validFrom
				&& validUntil == other.validUntil
				&& isMessagingAllowed == other.isMessagingAllowed
				&& isSharingAllowed == other.isSharingAllowed;
}


bool IContractInfo::Link::operator!=(const Link& other) const
{
	return !(*this == other);
}


bool IContractInfo::Allocation::operator==(const Allocation& other) const
{
	return allocationId == other.allocationId
				&& linkId == other.linkId
				&& grantorTenantId == other.grantorTenantId
				&& granteeTenantId == other.granteeTenantId
				&& productId == other.productId
				&& permissionIds == other.permissionIds
				&& validFrom == other.validFrom
				&& validUntil == other.validUntil
				&& comment == other.comment;
}


bool IContractInfo::Allocation::operator!=(const Allocation& other) const
{
	return !(*this == other);
}


bool IContractInfo::Reference::operator==(const Reference& other) const
{
	return contractId == other.contractId
				&& type == other.type
				&& contractNumber == other.contractNumber
				&& contractName == other.contractName;
}


bool IContractInfo::Reference::operator!=(const Reference& other) const
{
	return !(*this == other);
}


bool IContractInfo::Attachment::operator==(const Attachment& other) const
{
	return attachmentId == other.attachmentId
				&& fileName == other.fileName
				&& mimeType == other.mimeType
				&& size == other.size
				&& contentHash == other.contentHash
				&& addedByTenantId == other.addedByTenantId
				&& addedByUserId == other.addedByUserId
				&& addedAt == other.addedAt
				&& replacesAttachmentId == other.replacesAttachmentId
				&& isReplaced == other.isReplaced;
}


bool IContractInfo::Attachment::operator!=(const Attachment& other) const
{
	return !(*this == other);
}


bool IContractInfo::RoleSlot::operator==(const RoleSlot& other) const
{
	return roleId == other.roleId
				&& minCount == other.minCount
				&& maxCount == other.maxCount;
}


bool IContractInfo::RoleSlot::operator!=(const RoleSlot& other) const
{
	return !(*this == other);
}


// public methods

CContractInfo::CContractInfo():
	m_termDays(0),
	m_autoRenew(false),
	m_renewalPeriodDays(0),
	m_renewalNoticeDays(0),
	m_terminationNoticeDays(0),
	m_revision(0)
{
}


// reimplemented (imtauth::IContractInfo)

QByteArray CContractInfo::GetContractId() const
{
	return m_contractId;
}


void CContractInfo::SetContractId(const QByteArray& value)
{
	AssignValue(*this, m_contractId, value);
}


QString CContractInfo::GetNumber() const
{
	return m_number;
}


void CContractInfo::SetNumber(const QString& value)
{
	AssignValue(*this, m_number, value);
}


QString CContractInfo::GetName() const
{
	return m_name;
}


void CContractInfo::SetName(const QString& value)
{
	AssignValue(*this, m_name, value);
}


QString CContractInfo::GetDescription() const
{
	return m_description;
}


void CContractInfo::SetDescription(const QString& value)
{
	AssignValue(*this, m_description, value);
}


QString CContractInfo::GetTerms() const
{
	return m_terms;
}


void CContractInfo::SetTerms(const QString& value)
{
	AssignValue(*this, m_terms, value);
}


QByteArray CContractInfo::GetTypeId() const
{
	return m_typeId;
}


void CContractInfo::SetTypeId(const QByteArray& value)
{
	AssignValue(*this, m_typeId, value);
}


QString CContractInfo::GetTypeName() const
{
	return m_typeName;
}


void CContractInfo::SetTypeName(const QString& value)
{
	AssignValue(*this, m_typeName, value);
}


QByteArray CContractInfo::GetInitiatorTenantId() const
{
	return m_initiatorTenantId;
}


void CContractInfo::SetInitiatorTenantId(const QByteArray& value)
{
	AssignValue(*this, m_initiatorTenantId, value);
}


QByteArray CContractInfo::GetCreatedByUserId() const
{
	return m_createdByUserId;
}


void CContractInfo::SetCreatedByUserId(const QByteArray& value)
{
	AssignValue(*this, m_createdByUserId, value);
}


QDateTime CContractInfo::GetEffectiveFrom() const
{
	return m_effectiveFrom;
}


void CContractInfo::SetEffectiveFrom(const QDateTime& value)
{
	AssignValue(*this, m_effectiveFrom, value);
}


int CContractInfo::GetTermDays() const
{
	return m_termDays;
}


void CContractInfo::SetTermDays(int value)
{
	AssignValue(*this, m_termDays, value);
}


QDateTime CContractInfo::GetValidUntil() const
{
	return m_validUntil;
}


void CContractInfo::SetValidUntil(const QDateTime& value)
{
	AssignValue(*this, m_validUntil, value);
}


bool CContractInfo::IsAutoRenew() const
{
	return m_autoRenew;
}


void CContractInfo::SetAutoRenew(bool value)
{
	AssignValue(*this, m_autoRenew, value);
}


int CContractInfo::GetRenewalPeriodDays() const
{
	return m_renewalPeriodDays;
}


void CContractInfo::SetRenewalPeriodDays(int value)
{
	AssignValue(*this, m_renewalPeriodDays, value);
}


int CContractInfo::GetRenewalNoticeDays() const
{
	return m_renewalNoticeDays;
}


void CContractInfo::SetRenewalNoticeDays(int value)
{
	AssignValue(*this, m_renewalNoticeDays, value);
}


int CContractInfo::GetTerminationNoticeDays() const
{
	return m_terminationNoticeDays;
}


void CContractInfo::SetTerminationNoticeDays(int value)
{
	AssignValue(*this, m_terminationNoticeDays, value);
}


IContractInfo::Parties CContractInfo::GetParties() const
{
	return m_parties;
}


void CContractInfo::SetParties(const IContractInfo::Parties& value)
{
	AssignValue(*this, m_parties, value);
}


IContractInfo::Links CContractInfo::GetLinks() const
{
	return m_links;
}


void CContractInfo::SetLinks(const IContractInfo::Links& value)
{
	AssignValue(*this, m_links, value);
}


IContractInfo::Allocations CContractInfo::GetAllocations() const
{
	return m_allocations;
}


void CContractInfo::SetAllocations(const IContractInfo::Allocations& value)
{
	AssignValue(*this, m_allocations, value);
}


IContractInfo::References CContractInfo::GetReferences() const
{
	return m_references;
}


void CContractInfo::SetReferences(const IContractInfo::References& value)
{
	AssignValue(*this, m_references, value);
}


IContractInfo::Attachments CContractInfo::GetAttachments() const
{
	return m_attachments;
}


void CContractInfo::SetAttachments(const IContractInfo::Attachments& value)
{
	AssignValue(*this, m_attachments, value);
}


IContractInfo::RoleSlots CContractInfo::GetRoleSlots() const
{
	return m_roleSlots;
}


void CContractInfo::SetRoleSlots(const IContractInfo::RoleSlots& value)
{
	AssignValue(*this, m_roleSlots, value);
}


int CContractInfo::GetRevision() const
{
	return m_revision;
}


void CContractInfo::SetRevision(int value)
{
	AssignValue(*this, m_revision, value);
}


QByteArray CContractInfo::GetContentHash() const
{
	return m_contentHash;
}


void CContractInfo::SetContentHash(const QByteArray& value)
{
	AssignValue(*this, m_contentHash, value);
}


QDateTime CContractInfo::GetSubmittedAt() const
{
	return m_submittedAt;
}


void CContractInfo::SetSubmittedAt(const QDateTime& value)
{
	AssignValue(*this, m_submittedAt, value);
}


QDateTime CContractInfo::GetCancelledAt() const
{
	return m_cancelledAt;
}


void CContractInfo::SetCancelledAt(const QDateTime& value)
{
	AssignValue(*this, m_cancelledAt, value);
}


QDateTime CContractInfo::GetSignedAt() const
{
	return m_signedAt;
}


void CContractInfo::SetSignedAt(const QDateTime& value)
{
	AssignValue(*this, m_signedAt, value);
}


QDateTime CContractInfo::GetTerminatedAt() const
{
	return m_terminatedAt;
}


void CContractInfo::SetTerminatedAt(const QDateTime& value)
{
	AssignValue(*this, m_terminatedAt, value);
}


QString CContractInfo::GetTerminationReason() const
{
	return m_terminationReason;
}


void CContractInfo::SetTerminationReason(const QString& value)
{
	AssignValue(*this, m_terminationReason, value);
}


QByteArray CContractInfo::GetSupersededBy() const
{
	return m_supersededBy;
}


void CContractInfo::SetSupersededBy(const QByteArray& value)
{
	AssignValue(*this, m_supersededBy, value);
}


QDateTime CContractInfo::GetSupersededAt() const
{
	return m_supersededAt;
}


void CContractInfo::SetSupersededAt(const QDateTime& value)
{
	AssignValue(*this, m_supersededAt, value);
}


QDateTime CContractInfo::GetCreatedAt() const
{
	return m_createdAt;
}


void CContractInfo::SetCreatedAt(const QDateTime& value)
{
	AssignValue(*this, m_createdAt, value);
}


QDateTime CContractInfo::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CContractInfo::SetUpdatedAt(const QDateTime& value)
{
	AssignValue(*this, m_updatedAt, value);
}


// reimplemented (iser::ISerializable)

bool CContractInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = af::Leaf(archive, "ContractId", m_contractId);
	retVal = retVal && af::Leaf(archive, "Number", m_number);
	retVal = retVal && af::Leaf(archive, "Name", m_name);
	retVal = retVal && af::Leaf(archive, "Description", m_description);
	retVal = retVal && af::Leaf(archive, "Terms", m_terms);
	retVal = retVal && af::Leaf(archive, "TypeId", m_typeId);
	retVal = retVal && af::Leaf(archive, "TypeName", m_typeName);
	retVal = retVal && af::Leaf(archive, "InitiatorTenantId", m_initiatorTenantId);
	retVal = retVal && af::Leaf(archive, "CreatedByUserId", m_createdByUserId);
	retVal = retVal && af::Leaf(archive, "EffectiveFrom", m_effectiveFrom);
	retVal = retVal && af::Leaf(archive, "TermDays", m_termDays);
	retVal = retVal && af::Leaf(archive, "ValidUntil", m_validUntil);
	retVal = retVal && af::Leaf(archive, "AutoRenew", m_autoRenew);
	retVal = retVal && af::Leaf(archive, "RenewalPeriodDays", m_renewalPeriodDays);
	retVal = retVal && af::Leaf(archive, "RenewalNoticeDays", m_renewalNoticeDays);
	retVal = retVal && af::Leaf(archive, "TerminationNoticeDays", m_terminationNoticeDays);
	retVal = retVal && af::List(archive, "Parties", "Party", m_parties, SerializeParty);
	retVal = retVal && af::List(archive, "Links", "Link", m_links, SerializeLink);
	retVal = retVal && af::List(archive, "Allocations", "Allocation", m_allocations, SerializeAllocation);
	retVal = retVal && af::List(archive, "References", "Reference", m_references, SerializeReference);
	retVal = retVal && af::List(archive, "Attachments", "Attachment", m_attachments, SerializeAttachment);
	retVal = retVal && af::List(archive, "RoleSlots", "RoleSlot", m_roleSlots, SerializeRoleSlot);
	retVal = retVal && af::Leaf(archive, "Revision", m_revision);
	retVal = retVal && af::Leaf(archive, "ContentHash", m_contentHash);
	retVal = retVal && af::Leaf(archive, "SubmittedAt", m_submittedAt);
	retVal = retVal && af::Leaf(archive, "CancelledAt", m_cancelledAt);
	retVal = retVal && af::Leaf(archive, "SignedAt", m_signedAt);
	retVal = retVal && af::Leaf(archive, "TerminatedAt", m_terminatedAt);
	retVal = retVal && af::Leaf(archive, "TerminationReason", m_terminationReason);
	retVal = retVal && af::Leaf(archive, "SupersededBy", m_supersededBy);
	retVal = retVal && af::Leaf(archive, "SupersededAt", m_supersededAt);
	retVal = retVal && af::Leaf(archive, "CreatedAt", m_createdAt);
	retVal = retVal && af::Leaf(archive, "UpdatedAt", m_updatedAt);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CContractInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IContractInfo* sourcePtr = dynamic_cast<const IContractInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_contractId = sourcePtr->GetContractId();
	m_number = sourcePtr->GetNumber();
	m_name = sourcePtr->GetName();
	m_description = sourcePtr->GetDescription();
	m_terms = sourcePtr->GetTerms();
	m_typeId = sourcePtr->GetTypeId();
	m_typeName = sourcePtr->GetTypeName();
	m_initiatorTenantId = sourcePtr->GetInitiatorTenantId();
	m_createdByUserId = sourcePtr->GetCreatedByUserId();
	m_effectiveFrom = sourcePtr->GetEffectiveFrom();
	m_termDays = sourcePtr->GetTermDays();
	m_validUntil = sourcePtr->GetValidUntil();
	m_autoRenew = sourcePtr->IsAutoRenew();
	m_renewalPeriodDays = sourcePtr->GetRenewalPeriodDays();
	m_renewalNoticeDays = sourcePtr->GetRenewalNoticeDays();
	m_terminationNoticeDays = sourcePtr->GetTerminationNoticeDays();
	m_parties = sourcePtr->GetParties();
	m_links = sourcePtr->GetLinks();
	m_allocations = sourcePtr->GetAllocations();
	m_references = sourcePtr->GetReferences();
	m_attachments = sourcePtr->GetAttachments();
	m_roleSlots = sourcePtr->GetRoleSlots();
	m_revision = sourcePtr->GetRevision();
	m_contentHash = sourcePtr->GetContentHash();
	m_submittedAt = sourcePtr->GetSubmittedAt();
	m_cancelledAt = sourcePtr->GetCancelledAt();
	m_signedAt = sourcePtr->GetSignedAt();
	m_terminatedAt = sourcePtr->GetTerminatedAt();
	m_terminationReason = sourcePtr->GetTerminationReason();
	m_supersededBy = sourcePtr->GetSupersededBy();
	m_supersededAt = sourcePtr->GetSupersededAt();
	m_createdAt = sourcePtr->GetCreatedAt();
	m_updatedAt = sourcePtr->GetUpdatedAt();

	return true;
}


bool CContractInfo::IsEqual(const IChangeable& object) const
{
	const IContractInfo* otherPtr = dynamic_cast<const IContractInfo*>(&object);
	if (otherPtr == nullptr){
		return false;
	}

	return m_contractId == otherPtr->GetContractId()
				&& m_number == otherPtr->GetNumber()
				&& m_name == otherPtr->GetName()
				&& m_description == otherPtr->GetDescription()
				&& m_terms == otherPtr->GetTerms()
				&& m_typeId == otherPtr->GetTypeId()
				&& m_typeName == otherPtr->GetTypeName()
				&& m_initiatorTenantId == otherPtr->GetInitiatorTenantId()
				&& m_createdByUserId == otherPtr->GetCreatedByUserId()
				&& m_effectiveFrom == otherPtr->GetEffectiveFrom()
				&& m_termDays == otherPtr->GetTermDays()
				&& m_validUntil == otherPtr->GetValidUntil()
				&& m_autoRenew == otherPtr->IsAutoRenew()
				&& m_renewalPeriodDays == otherPtr->GetRenewalPeriodDays()
				&& m_renewalNoticeDays == otherPtr->GetRenewalNoticeDays()
				&& m_terminationNoticeDays == otherPtr->GetTerminationNoticeDays()
				&& m_parties == otherPtr->GetParties()
				&& m_links == otherPtr->GetLinks()
				&& m_allocations == otherPtr->GetAllocations()
				&& m_references == otherPtr->GetReferences()
				&& m_attachments == otherPtr->GetAttachments()
				&& m_roleSlots == otherPtr->GetRoleSlots()
				&& m_revision == otherPtr->GetRevision()
				&& m_contentHash == otherPtr->GetContentHash()
				&& m_submittedAt == otherPtr->GetSubmittedAt()
				&& m_cancelledAt == otherPtr->GetCancelledAt()
				&& m_signedAt == otherPtr->GetSignedAt()
				&& m_terminatedAt == otherPtr->GetTerminatedAt()
				&& m_terminationReason == otherPtr->GetTerminationReason()
				&& m_supersededBy == otherPtr->GetSupersededBy()
				&& m_supersededAt == otherPtr->GetSupersededAt()
				&& m_createdAt == otherPtr->GetCreatedAt()
				&& m_updatedAt == otherPtr->GetUpdatedAt();
}


istd::IChangeableUniquePtr CContractInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CContractInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CContractInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_contractId = QByteArray();
	m_number = QString();
	m_name = QString();
	m_description = QString();
	m_terms = QString();
	m_typeId = QByteArray();
	m_typeName = QString();
	m_initiatorTenantId = QByteArray();
	m_createdByUserId = QByteArray();
	m_effectiveFrom = QDateTime();
	m_termDays = 0;
	m_validUntil = QDateTime();
	m_autoRenew = false;
	m_renewalPeriodDays = 0;
	m_renewalNoticeDays = 0;
	m_terminationNoticeDays = 0;
	m_parties = IContractInfo::Parties();
	m_links = IContractInfo::Links();
	m_allocations = IContractInfo::Allocations();
	m_references = IContractInfo::References();
	m_attachments = IContractInfo::Attachments();
	m_roleSlots = IContractInfo::RoleSlots();
	m_revision = 0;
	m_contentHash = QByteArray();
	m_submittedAt = QDateTime();
	m_cancelledAt = QDateTime();
	m_signedAt = QDateTime();
	m_terminatedAt = QDateTime();
	m_terminationReason = QString();
	m_supersededBy = QByteArray();
	m_supersededAt = QDateTime();
	m_createdAt = QDateTime();
	m_updatedAt = QDateTime();

	return true;
}


} // namespace imtauth


