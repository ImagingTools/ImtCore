// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CContractPartyPrivateInfo.h>


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


bool SerializeAttachment(iser::IArchive& archive, IContractInfo::Attachment& item)
{
	bool retVal = af::Leaf(archive, "AttachmentId", item.attachmentId);
	retVal = retVal && af::EnumLeaf(archive, "Kind", item.kind);
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


bool SerializeAllocationGroups(iser::IArchive& archive, IContractPartyPrivateInfo::AllocationGroups& item)
{
	bool retVal = af::Leaf(archive, "AllocationId", item.allocationId);
	retVal = retVal && af::Leaf(archive, "GroupIds", item.groupIds);

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

	bool retVal = af::Leaf(archive, "ContractId", m_contractId);
	retVal = retVal && af::Leaf(archive, "TenantId", m_tenantId);
	retVal = retVal && af::Leaf(archive, "InternalNumber", m_internalNumber);
	retVal = retVal && af::Leaf(archive, "ResponsibleUserId", m_responsibleUserId);
	retVal = retVal && af::Leaf(archive, "Notes", m_notes);
	retVal = retVal && af::List(archive, "Attachments", "Attachment", m_attachments, SerializeAttachment);
	retVal = retVal && af::List(archive, "AllocationGroups", "Entry", m_allocationGroups, SerializeAllocationGroups);
	retVal = retVal && af::Leaf(archive, "ReminderDays", m_reminderDays);
	retVal = retVal && af::Leaf(archive, "UpdatedAt", m_updatedAt);

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


