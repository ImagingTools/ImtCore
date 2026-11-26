// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CContractInfo.h>


// ACF includes
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <istd/CChangeNotifier.h>


namespace imtauth
{


namespace
{


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
	bool retVal = true;

	iser::CArchiveTag userIdTag("UserId", "User ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(item.userId);
	retVal = retVal && archive.EndTag(userIdTag);

	iser::CArchiveTag userNameTag("UserName", "User name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userNameTag);
	retVal = retVal && archive.Process(item.userName);
	retVal = retVal && archive.EndTag(userNameTag);

	iser::CArchiveTag positionTag("Position", "Position", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(positionTag);
	retVal = retVal && archive.Process(item.position);
	retVal = retVal && archive.EndTag(positionTag);

	iser::CArchiveTag signedAtTag("SignedAt", "Signed at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(signedAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, item.signedAt);
	retVal = retVal && archive.EndTag(signedAtTag);

	iser::CArchiveTag revisionTag("Revision", "Revision", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(revisionTag);
	retVal = retVal && archive.Process(item.revision);
	retVal = retVal && archive.EndTag(revisionTag);

	iser::CArchiveTag contentHashTag("ContentHash", "Content hash", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(contentHashTag);
	retVal = retVal && archive.Process(item.contentHash);
	retVal = retVal && archive.EndTag(contentHashTag);

	return retVal;
}


bool SerializeParty(iser::IArchive& archive, IContractInfo::Party& item)
{
	bool retVal = true;

	iser::CArchiveTag tenantIdTag("TenantId", "Tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tenantIdTag);
	retVal = retVal && archive.Process(item.tenantId);
	retVal = retVal && archive.EndTag(tenantIdTag);

	iser::CArchiveTag tenantNameTag("TenantName", "Tenant name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tenantNameTag);
	retVal = retVal && archive.Process(item.tenantName);
	retVal = retVal && archive.EndTag(tenantNameTag);

	iser::CArchiveTag roleIdTag("RoleId", "Role ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(roleIdTag);
	retVal = retVal && archive.Process(item.roleId);
	retVal = retVal && archive.EndTag(roleIdTag);

	iser::CArchiveTag roleNameTag("RoleName", "Role name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(roleNameTag);
	retVal = retVal && archive.Process(item.roleName);
	retVal = retVal && archive.EndTag(roleNameTag);

	iser::CArchiveTag channelTag("Channel", "Channel", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(channelTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<IContractInfo::PartyChannel, IContractInfo::ToString, IContractInfo::FromString>(archive, item.channel);
	retVal = retVal && archive.EndTag(channelTag);

	iser::CArchiveTag stateTag("State", "State", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(stateTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<IContractInfo::PartyState, IContractInfo::ToString, IContractInfo::FromString>(archive, item.state);
	retVal = retVal && archive.EndTag(stateTag);

	iser::CArchiveTag stateCommentTag("StateComment", "State comment", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(stateCommentTag);
	retVal = retVal && archive.Process(item.stateComment);
	retVal = retVal && archive.EndTag(stateCommentTag);

	iser::CArchiveTag stateChangedAtTag("StateChangedAt", "State changed at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(stateChangedAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, item.stateChangedAt);
	retVal = retVal && archive.EndTag(stateChangedAtTag);

	iser::CArchiveTag signaturesTag("Signatures", "Signatures", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag signatureTag("Signature", "Signature", iser::CArchiveTag::TT_GROUP, &signaturesTag);
	int signaturesCount = item.signatures.size();
	retVal = retVal && archive.BeginMultiTag(signaturesTag, signatureTag, signaturesCount);
	if (retVal && !archive.IsStoring()){
		item.signatures.clear();
		item.signatures.resize(signaturesCount);
	}

	for (int i = 0; retVal && i < signaturesCount; ++i){
		retVal = archive.BeginTag(signatureTag) && SerializeSignature(archive, item.signatures[i]) && archive.EndTag(signatureTag);
	}

	retVal = retVal && archive.EndTag(signaturesTag);

	iser::CArchiveTag withdrawAtTag("WithdrawAt", "Withdraw at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(withdrawAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, item.withdrawAt);
	retVal = retVal && archive.EndTag(withdrawAtTag);

	iser::CArchiveTag withdrawReasonTag("WithdrawReason", "Withdraw reason", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(withdrawReasonTag);
	retVal = retVal && archive.Process(item.withdrawReason);
	retVal = retVal && archive.EndTag(withdrawReasonTag);

	iser::CArchiveTag nonRenewalNoticeAtTag("NonRenewalNoticeAt", "Non renewal notice at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nonRenewalNoticeAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, item.nonRenewalNoticeAt);
	retVal = retVal && archive.EndTag(nonRenewalNoticeAtTag);

	return retVal;
}


bool SerializeLink(iser::IArchive& archive, IContractInfo::Link& item)
{
	bool retVal = true;

	iser::CArchiveTag linkIdTag("LinkId", "Link ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(linkIdTag);
	retVal = retVal && archive.Process(item.linkId);
	retVal = retVal && archive.EndTag(linkIdTag);

	iser::CArchiveTag tenantAIdTag("TenantAId", "Tenant a ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tenantAIdTag);
	retVal = retVal && archive.Process(item.tenantAId);
	retVal = retVal && archive.EndTag(tenantAIdTag);

	iser::CArchiveTag tenantBIdTag("TenantBId", "Tenant b ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tenantBIdTag);
	retVal = retVal && archive.Process(item.tenantBId);
	retVal = retVal && archive.EndTag(tenantBIdTag);

	iser::CArchiveTag roleAIdTag("RoleAId", "Role a ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(roleAIdTag);
	retVal = retVal && archive.Process(item.roleAId);
	retVal = retVal && archive.EndTag(roleAIdTag);

	iser::CArchiveTag roleANameTag("RoleAName", "Role a name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(roleANameTag);
	retVal = retVal && archive.Process(item.roleAName);
	retVal = retVal && archive.EndTag(roleANameTag);

	iser::CArchiveTag roleBIdTag("RoleBId", "Role b ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(roleBIdTag);
	retVal = retVal && archive.Process(item.roleBId);
	retVal = retVal && archive.EndTag(roleBIdTag);

	iser::CArchiveTag roleBNameTag("RoleBName", "Role b name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(roleBNameTag);
	retVal = retVal && archive.Process(item.roleBName);
	retVal = retVal && archive.EndTag(roleBNameTag);

	iser::CArchiveTag validFromTag("ValidFrom", "Valid from", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(validFromTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, item.validFrom);
	retVal = retVal && archive.EndTag(validFromTag);

	iser::CArchiveTag validUntilTag("ValidUntil", "Valid until", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(validUntilTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, item.validUntil);
	retVal = retVal && archive.EndTag(validUntilTag);

	iser::CArchiveTag isMessagingAllowedTag("IsMessagingAllowed", "Is messaging allowed", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isMessagingAllowedTag);
	retVal = retVal && archive.Process(item.isMessagingAllowed);
	retVal = retVal && archive.EndTag(isMessagingAllowedTag);

	iser::CArchiveTag isSharingAllowedTag("IsSharingAllowed", "Is sharing allowed", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isSharingAllowedTag);
	retVal = retVal && archive.Process(item.isSharingAllowed);
	retVal = retVal && archive.EndTag(isSharingAllowedTag);

	return retVal;
}


bool SerializeAllocation(iser::IArchive& archive, IContractInfo::Allocation& item)
{
	bool retVal = true;

	iser::CArchiveTag allocationIdTag("AllocationId", "Allocation ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(allocationIdTag);
	retVal = retVal && archive.Process(item.allocationId);
	retVal = retVal && archive.EndTag(allocationIdTag);

	iser::CArchiveTag linkIdTag("LinkId", "Link ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(linkIdTag);
	retVal = retVal && archive.Process(item.linkId);
	retVal = retVal && archive.EndTag(linkIdTag);

	iser::CArchiveTag grantorTenantIdTag("GrantorTenantId", "Grantor tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(grantorTenantIdTag);
	retVal = retVal && archive.Process(item.grantorTenantId);
	retVal = retVal && archive.EndTag(grantorTenantIdTag);

	iser::CArchiveTag granteeTenantIdTag("GranteeTenantId", "Grantee tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(granteeTenantIdTag);
	retVal = retVal && archive.Process(item.granteeTenantId);
	retVal = retVal && archive.EndTag(granteeTenantIdTag);

	iser::CArchiveTag productIdTag("ProductId", "Product ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productIdTag);
	retVal = retVal && archive.Process(item.productId);
	retVal = retVal && archive.EndTag(productIdTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, item.permissionIds, "PermissionIds", "PermissionId");

	iser::CArchiveTag validFromTag("ValidFrom", "Valid from", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(validFromTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, item.validFrom);
	retVal = retVal && archive.EndTag(validFromTag);

	iser::CArchiveTag validUntilTag("ValidUntil", "Valid until", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(validUntilTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, item.validUntil);
	retVal = retVal && archive.EndTag(validUntilTag);

	iser::CArchiveTag commentTag("Comment", "Comment", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(commentTag);
	retVal = retVal && archive.Process(item.comment);
	retVal = retVal && archive.EndTag(commentTag);

	return retVal;
}


bool SerializeReference(iser::IArchive& archive, IContractInfo::Reference& item)
{
	bool retVal = true;

	iser::CArchiveTag contractIdTag("ContractId", "Contract ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(contractIdTag);
	retVal = retVal && archive.Process(item.contractId);
	retVal = retVal && archive.EndTag(contractIdTag);

	iser::CArchiveTag typeTag("Type", "Type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(typeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<IContractInfo::ReferenceType, IContractInfo::ToString, IContractInfo::FromString>(archive, item.type);
	retVal = retVal && archive.EndTag(typeTag);

	iser::CArchiveTag contractNumberTag("ContractNumber", "Contract number", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(contractNumberTag);
	retVal = retVal && archive.Process(item.contractNumber);
	retVal = retVal && archive.EndTag(contractNumberTag);

	iser::CArchiveTag contractNameTag("ContractName", "Contract name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(contractNameTag);
	retVal = retVal && archive.Process(item.contractName);
	retVal = retVal && archive.EndTag(contractNameTag);

	return retVal;
}


bool SerializeRoleSlot(iser::IArchive& archive, IContractInfo::RoleSlot& item)
{
	bool retVal = true;

	iser::CArchiveTag roleIdTag("RoleId", "Role ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(roleIdTag);
	retVal = retVal && archive.Process(item.roleId);
	retVal = retVal && archive.EndTag(roleIdTag);

	iser::CArchiveTag minCountTag("MinCount", "Min count", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(minCountTag);
	retVal = retVal && archive.Process(item.minCount);
	retVal = retVal && archive.EndTag(minCountTag);

	iser::CArchiveTag maxCountTag("MaxCount", "Max count", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(maxCountTag);
	retVal = retVal && archive.Process(item.maxCount);
	retVal = retVal && archive.EndTag(maxCountTag);

	return retVal;
}


bool SerializeAttachment(iser::IArchive& archive, IContractInfo::Attachment& item)
{
	bool retVal = true;

	iser::CArchiveTag attachmentIdTag("AttachmentId", "Attachment ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(attachmentIdTag);
	retVal = retVal && archive.Process(item.attachmentId);
	retVal = retVal && archive.EndTag(attachmentIdTag);

	iser::CArchiveTag fileNameTag("FileName", "File name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(fileNameTag);
	retVal = retVal && archive.Process(item.fileName);
	retVal = retVal && archive.EndTag(fileNameTag);

	iser::CArchiveTag mimeTypeTag("MimeType", "Mime type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(mimeTypeTag);
	retVal = retVal && archive.Process(item.mimeType);
	retVal = retVal && archive.EndTag(mimeTypeTag);

	iser::CArchiveTag sizeTag("Size", "Size", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sizeTag);
	retVal = retVal && archive.Process(item.size);
	retVal = retVal && archive.EndTag(sizeTag);

	iser::CArchiveTag contentHashTag("ContentHash", "Content hash", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(contentHashTag);
	retVal = retVal && archive.Process(item.contentHash);
	retVal = retVal && archive.EndTag(contentHashTag);

	iser::CArchiveTag addedByTenantIdTag("AddedByTenantId", "Added by tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(addedByTenantIdTag);
	retVal = retVal && archive.Process(item.addedByTenantId);
	retVal = retVal && archive.EndTag(addedByTenantIdTag);

	iser::CArchiveTag addedByUserIdTag("AddedByUserId", "Added by user ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(addedByUserIdTag);
	retVal = retVal && archive.Process(item.addedByUserId);
	retVal = retVal && archive.EndTag(addedByUserIdTag);

	iser::CArchiveTag addedAtTag("AddedAt", "Added at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(addedAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, item.addedAt);
	retVal = retVal && archive.EndTag(addedAtTag);

	iser::CArchiveTag replacesAttachmentIdTag("ReplacesAttachmentId", "Replaces attachment ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(replacesAttachmentIdTag);
	retVal = retVal && archive.Process(item.replacesAttachmentId);
	retVal = retVal && archive.EndTag(replacesAttachmentIdTag);

	iser::CArchiveTag isReplacedTag("IsReplaced", "Is replaced", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isReplacedTag);
	retVal = retVal && archive.Process(item.isReplaced);
	retVal = retVal && archive.EndTag(isReplacedTag);

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
				&& nonRenewalNoticeAt == other.nonRenewalNoticeAt;
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

	bool retVal = true;

	iser::CArchiveTag contractIdTag("ContractId", "Contract ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(contractIdTag);
	retVal = retVal && archive.Process(m_contractId);
	retVal = retVal && archive.EndTag(contractIdTag);

	iser::CArchiveTag numberTag("Number", "Number", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(numberTag);
	retVal = retVal && archive.Process(m_number);
	retVal = retVal && archive.EndTag(numberTag);

	iser::CArchiveTag nameTag("Name", "Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	iser::CArchiveTag termsTag("Terms", "Terms", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(termsTag);
	retVal = retVal && archive.Process(m_terms);
	retVal = retVal && archive.EndTag(termsTag);

	iser::CArchiveTag typeIdTag("TypeId", "Type ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(typeIdTag);
	retVal = retVal && archive.Process(m_typeId);
	retVal = retVal && archive.EndTag(typeIdTag);

	iser::CArchiveTag typeNameTag("TypeName", "Type name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(typeNameTag);
	retVal = retVal && archive.Process(m_typeName);
	retVal = retVal && archive.EndTag(typeNameTag);

	iser::CArchiveTag initiatorTenantIdTag("InitiatorTenantId", "Initiator tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(initiatorTenantIdTag);
	retVal = retVal && archive.Process(m_initiatorTenantId);
	retVal = retVal && archive.EndTag(initiatorTenantIdTag);

	iser::CArchiveTag createdByUserIdTag("CreatedByUserId", "Created by user ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdByUserIdTag);
	retVal = retVal && archive.Process(m_createdByUserId);
	retVal = retVal && archive.EndTag(createdByUserIdTag);

	iser::CArchiveTag effectiveFromTag("EffectiveFrom", "Effective from", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(effectiveFromTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_effectiveFrom);
	retVal = retVal && archive.EndTag(effectiveFromTag);

	iser::CArchiveTag termDaysTag("TermDays", "Term days", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(termDaysTag);
	retVal = retVal && archive.Process(m_termDays);
	retVal = retVal && archive.EndTag(termDaysTag);

	iser::CArchiveTag validUntilTag("ValidUntil", "Valid until", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(validUntilTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_validUntil);
	retVal = retVal && archive.EndTag(validUntilTag);

	iser::CArchiveTag autoRenewTag("AutoRenew", "Auto renew", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(autoRenewTag);
	retVal = retVal && archive.Process(m_autoRenew);
	retVal = retVal && archive.EndTag(autoRenewTag);

	iser::CArchiveTag renewalPeriodDaysTag("RenewalPeriodDays", "Renewal period days", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(renewalPeriodDaysTag);
	retVal = retVal && archive.Process(m_renewalPeriodDays);
	retVal = retVal && archive.EndTag(renewalPeriodDaysTag);

	iser::CArchiveTag renewalNoticeDaysTag("RenewalNoticeDays", "Renewal notice days", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(renewalNoticeDaysTag);
	retVal = retVal && archive.Process(m_renewalNoticeDays);
	retVal = retVal && archive.EndTag(renewalNoticeDaysTag);

	iser::CArchiveTag terminationNoticeDaysTag("TerminationNoticeDays", "Termination notice days", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(terminationNoticeDaysTag);
	retVal = retVal && archive.Process(m_terminationNoticeDays);
	retVal = retVal && archive.EndTag(terminationNoticeDaysTag);

	iser::CArchiveTag partiesTag("Parties", "Parties", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag partyTag("Party", "Party", iser::CArchiveTag::TT_GROUP, &partiesTag);
	int partiesCount = m_parties.size();
	retVal = retVal && archive.BeginMultiTag(partiesTag, partyTag, partiesCount);
	if (retVal && !archive.IsStoring()){
		m_parties.clear();
		m_parties.resize(partiesCount);
	}

	for (int i = 0; retVal && i < partiesCount; ++i){
		retVal = archive.BeginTag(partyTag) && SerializeParty(archive, m_parties[i]) && archive.EndTag(partyTag);
	}

	retVal = retVal && archive.EndTag(partiesTag);

	iser::CArchiveTag linksTag("Links", "Links", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag linkTag("Link", "Link", iser::CArchiveTag::TT_GROUP, &linksTag);
	int linksCount = m_links.size();
	retVal = retVal && archive.BeginMultiTag(linksTag, linkTag, linksCount);
	if (retVal && !archive.IsStoring()){
		m_links.clear();
		m_links.resize(linksCount);
	}

	for (int i = 0; retVal && i < linksCount; ++i){
		retVal = archive.BeginTag(linkTag) && SerializeLink(archive, m_links[i]) && archive.EndTag(linkTag);
	}

	retVal = retVal && archive.EndTag(linksTag);

	iser::CArchiveTag allocationsTag("Allocations", "Allocations", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag allocationTag("Allocation", "Allocation", iser::CArchiveTag::TT_GROUP, &allocationsTag);
	int allocationsCount = m_allocations.size();
	retVal = retVal && archive.BeginMultiTag(allocationsTag, allocationTag, allocationsCount);
	if (retVal && !archive.IsStoring()){
		m_allocations.clear();
		m_allocations.resize(allocationsCount);
	}

	for (int i = 0; retVal && i < allocationsCount; ++i){
		retVal = archive.BeginTag(allocationTag) && SerializeAllocation(archive, m_allocations[i]) && archive.EndTag(allocationTag);
	}

	retVal = retVal && archive.EndTag(allocationsTag);

	iser::CArchiveTag referencesTag("References", "References", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag referenceTag("Reference", "Reference", iser::CArchiveTag::TT_GROUP, &referencesTag);
	int referencesCount = m_references.size();
	retVal = retVal && archive.BeginMultiTag(referencesTag, referenceTag, referencesCount);
	if (retVal && !archive.IsStoring()){
		m_references.clear();
		m_references.resize(referencesCount);
	}

	for (int i = 0; retVal && i < referencesCount; ++i){
		retVal = archive.BeginTag(referenceTag) && SerializeReference(archive, m_references[i]) && archive.EndTag(referenceTag);
	}

	retVal = retVal && archive.EndTag(referencesTag);

	iser::CArchiveTag attachmentsTag("Attachments", "Attachments", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag attachmentTag("Attachment", "Attachment", iser::CArchiveTag::TT_GROUP, &attachmentsTag);
	int attachmentsCount = m_attachments.size();
	retVal = retVal && archive.BeginMultiTag(attachmentsTag, attachmentTag, attachmentsCount);
	if (retVal && !archive.IsStoring()){
		m_attachments.clear();
		m_attachments.resize(attachmentsCount);
	}

	for (int i = 0; retVal && i < attachmentsCount; ++i){
		retVal = archive.BeginTag(attachmentTag) && SerializeAttachment(archive, m_attachments[i]) && archive.EndTag(attachmentTag);
	}

	retVal = retVal && archive.EndTag(attachmentsTag);

	iser::CArchiveTag roleSlotsTag("RoleSlots", "Role slots", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag roleSlotTag("RoleSlot", "Role slot", iser::CArchiveTag::TT_GROUP, &roleSlotsTag);
	int roleSlotsCount = m_roleSlots.size();
	retVal = retVal && archive.BeginMultiTag(roleSlotsTag, roleSlotTag, roleSlotsCount);
	if (retVal && !archive.IsStoring()){
		m_roleSlots.clear();
		m_roleSlots.resize(roleSlotsCount);
	}

	for (int i = 0; retVal && i < roleSlotsCount; ++i){
		retVal = archive.BeginTag(roleSlotTag) && SerializeRoleSlot(archive, m_roleSlots[i]) && archive.EndTag(roleSlotTag);
	}

	retVal = retVal && archive.EndTag(roleSlotsTag);

	iser::CArchiveTag revisionTag("Revision", "Revision", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(revisionTag);
	retVal = retVal && archive.Process(m_revision);
	retVal = retVal && archive.EndTag(revisionTag);

	iser::CArchiveTag contentHashTag("ContentHash", "Content hash", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(contentHashTag);
	retVal = retVal && archive.Process(m_contentHash);
	retVal = retVal && archive.EndTag(contentHashTag);

	iser::CArchiveTag submittedAtTag("SubmittedAt", "Submitted at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(submittedAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_submittedAt);
	retVal = retVal && archive.EndTag(submittedAtTag);

	iser::CArchiveTag cancelledAtTag("CancelledAt", "Cancelled at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(cancelledAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_cancelledAt);
	retVal = retVal && archive.EndTag(cancelledAtTag);

	iser::CArchiveTag signedAtTag("SignedAt", "Signed at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(signedAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_signedAt);
	retVal = retVal && archive.EndTag(signedAtTag);

	iser::CArchiveTag terminatedAtTag("TerminatedAt", "Terminated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(terminatedAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_terminatedAt);
	retVal = retVal && archive.EndTag(terminatedAtTag);

	iser::CArchiveTag terminationReasonTag("TerminationReason", "Termination reason", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(terminationReasonTag);
	retVal = retVal && archive.Process(m_terminationReason);
	retVal = retVal && archive.EndTag(terminationReasonTag);

	iser::CArchiveTag supersededByTag("SupersededBy", "Superseded by", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(supersededByTag);
	retVal = retVal && archive.Process(m_supersededBy);
	retVal = retVal && archive.EndTag(supersededByTag);

	iser::CArchiveTag supersededAtTag("SupersededAt", "Superseded at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(supersededAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_supersededAt);
	retVal = retVal && archive.EndTag(supersededAtTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

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


