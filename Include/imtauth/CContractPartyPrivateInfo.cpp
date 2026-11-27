// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CContractPartyPrivateInfo.h>


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


bool SerializeAllocationGroups(iser::IArchive& archive, IContractPartyPrivateInfo::AllocationGroups& item)
{
	bool retVal = true;

	iser::CArchiveTag allocationIdTag("AllocationId", "Allocation ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(allocationIdTag);
	retVal = retVal && archive.Process(item.allocationId);
	retVal = retVal && archive.EndTag(allocationIdTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, item.groupIds, "GroupIds", "GroupId");

	return retVal;
}


} // anonymous namespace


// public methods of the types of IContractPartyPrivateInfo

bool IContractPartyPrivateInfo::AllocationGroups::operator==(const AllocationGroups& other) const
{
	return allocationId == other.allocationId
				&& groupIds == other.groupIds;
}


bool IContractPartyPrivateInfo::AllocationGroups::operator!=(const AllocationGroups& other) const
{
	return !(*this == other);
}


// public methods

CContractPartyPrivateInfo::CContractPartyPrivateInfo():
	m_reminderDays(0)
{
}


// reimplemented (imtauth::IContractPartyPrivateInfo)

QByteArray CContractPartyPrivateInfo::GetContractId() const
{
	return m_contractId;
}


void CContractPartyPrivateInfo::SetContractId(const QByteArray& value)
{
	AssignValue(*this, m_contractId, value);
}


QByteArray CContractPartyPrivateInfo::GetTenantId() const
{
	return m_tenantId;
}


void CContractPartyPrivateInfo::SetTenantId(const QByteArray& value)
{
	AssignValue(*this, m_tenantId, value);
}


QString CContractPartyPrivateInfo::GetInternalNumber() const
{
	return m_internalNumber;
}


void CContractPartyPrivateInfo::SetInternalNumber(const QString& value)
{
	AssignValue(*this, m_internalNumber, value);
}


QByteArray CContractPartyPrivateInfo::GetResponsibleUserId() const
{
	return m_responsibleUserId;
}


void CContractPartyPrivateInfo::SetResponsibleUserId(const QByteArray& value)
{
	AssignValue(*this, m_responsibleUserId, value);
}


QString CContractPartyPrivateInfo::GetNotes() const
{
	return m_notes;
}


void CContractPartyPrivateInfo::SetNotes(const QString& value)
{
	AssignValue(*this, m_notes, value);
}


IContractInfo::Attachments CContractPartyPrivateInfo::GetAttachments() const
{
	return m_attachments;
}


void CContractPartyPrivateInfo::SetAttachments(const IContractInfo::Attachments& value)
{
	AssignValue(*this, m_attachments, value);
}


IContractPartyPrivateInfo::AllocationGroupsList CContractPartyPrivateInfo::GetAllocationGroups() const
{
	return m_allocationGroups;
}


void CContractPartyPrivateInfo::SetAllocationGroups(const IContractPartyPrivateInfo::AllocationGroupsList& value)
{
	AssignValue(*this, m_allocationGroups, value);
}


int CContractPartyPrivateInfo::GetReminderDays() const
{
	return m_reminderDays;
}


void CContractPartyPrivateInfo::SetReminderDays(int value)
{
	AssignValue(*this, m_reminderDays, value);
}


QDateTime CContractPartyPrivateInfo::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CContractPartyPrivateInfo::SetUpdatedAt(const QDateTime& value)
{
	AssignValue(*this, m_updatedAt, value);
}


// reimplemented (iser::ISerializable)

bool CContractPartyPrivateInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag contractIdTag("ContractId", "Contract ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(contractIdTag);
	retVal = retVal && archive.Process(m_contractId);
	retVal = retVal && archive.EndTag(contractIdTag);

	iser::CArchiveTag tenantIdTag("TenantId", "Tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tenantIdTag);
	retVal = retVal && archive.Process(m_tenantId);
	retVal = retVal && archive.EndTag(tenantIdTag);

	iser::CArchiveTag internalNumberTag("InternalNumber", "Internal number", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(internalNumberTag);
	retVal = retVal && archive.Process(m_internalNumber);
	retVal = retVal && archive.EndTag(internalNumberTag);

	iser::CArchiveTag responsibleUserIdTag("ResponsibleUserId", "Responsible user ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(responsibleUserIdTag);
	retVal = retVal && archive.Process(m_responsibleUserId);
	retVal = retVal && archive.EndTag(responsibleUserIdTag);

	iser::CArchiveTag notesTag("Notes", "Notes", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(notesTag);
	retVal = retVal && archive.Process(m_notes);
	retVal = retVal && archive.EndTag(notesTag);

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

	iser::CArchiveTag allocationGroupsTag("AllocationGroups", "Allocation groups", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag entryTag("Entry", "Entry", iser::CArchiveTag::TT_GROUP, &allocationGroupsTag);
	int allocationGroupsCount = m_allocationGroups.size();
	retVal = retVal && archive.BeginMultiTag(allocationGroupsTag, entryTag, allocationGroupsCount);
	if (retVal && !archive.IsStoring()){
		m_allocationGroups.clear();
		m_allocationGroups.resize(allocationGroupsCount);
	}

	for (int i = 0; retVal && i < allocationGroupsCount; ++i){
		retVal = archive.BeginTag(entryTag) && SerializeAllocationGroups(archive, m_allocationGroups[i]) && archive.EndTag(entryTag);
	}

	retVal = retVal && archive.EndTag(allocationGroupsTag);

	iser::CArchiveTag reminderDaysTag("ReminderDays", "Reminder days", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(reminderDaysTag);
	retVal = retVal && archive.Process(m_reminderDays);
	retVal = retVal && archive.EndTag(reminderDaysTag);

	iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CContractPartyPrivateInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IContractPartyPrivateInfo* sourcePtr = dynamic_cast<const IContractPartyPrivateInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_contractId = sourcePtr->GetContractId();
	m_tenantId = sourcePtr->GetTenantId();
	m_internalNumber = sourcePtr->GetInternalNumber();
	m_responsibleUserId = sourcePtr->GetResponsibleUserId();
	m_notes = sourcePtr->GetNotes();
	m_attachments = sourcePtr->GetAttachments();
	m_allocationGroups = sourcePtr->GetAllocationGroups();
	m_reminderDays = sourcePtr->GetReminderDays();
	m_updatedAt = sourcePtr->GetUpdatedAt();

	return true;
}


bool CContractPartyPrivateInfo::IsEqual(const IChangeable& object) const
{
	const IContractPartyPrivateInfo* otherPtr = dynamic_cast<const IContractPartyPrivateInfo*>(&object);
	if (otherPtr == nullptr){
		return false;
	}

	return m_contractId == otherPtr->GetContractId()
				&& m_tenantId == otherPtr->GetTenantId()
				&& m_internalNumber == otherPtr->GetInternalNumber()
				&& m_responsibleUserId == otherPtr->GetResponsibleUserId()
				&& m_notes == otherPtr->GetNotes()
				&& m_attachments == otherPtr->GetAttachments()
				&& m_allocationGroups == otherPtr->GetAllocationGroups()
				&& m_reminderDays == otherPtr->GetReminderDays()
				&& m_updatedAt == otherPtr->GetUpdatedAt();
}


istd::IChangeableUniquePtr CContractPartyPrivateInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CContractPartyPrivateInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CContractPartyPrivateInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_contractId = QByteArray();
	m_tenantId = QByteArray();
	m_internalNumber = QString();
	m_responsibleUserId = QByteArray();
	m_notes = QString();
	m_attachments = IContractInfo::Attachments();
	m_allocationGroups = IContractPartyPrivateInfo::AllocationGroupsList();
	m_reminderDays = 0;
	m_updatedAt = QDateTime();

	return true;
}


} // namespace imtauth


