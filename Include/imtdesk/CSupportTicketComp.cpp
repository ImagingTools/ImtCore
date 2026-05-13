// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdesk/CSupportTicketComp.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtdesk
{


// reimplemented (imtdesk::ISupportTicket)

QByteArray CSupportTicketComp::GetId() const
{
	return m_id;
}


void CSupportTicketComp::SetId(const QByteArray& id)
{
	if (m_id != id){
		istd::CChangeNotifier notifier(this);

		m_id = id;
	}
}


QString CSupportTicketComp::GetTitle() const
{
	return m_title;
}


void CSupportTicketComp::SetTitle(const QString& title)
{
	if (m_title != title){
		istd::CChangeNotifier notifier(this);

		m_title = title;
	}
}


QString CSupportTicketComp::GetDescription() const
{
	return m_description;
}


void CSupportTicketComp::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier notifier(this);

		m_description = description;
	}
}


ISupportTicket::TicketType CSupportTicketComp::GetTicketType() const
{
	return m_ticketType;
}


void CSupportTicketComp::SetTicketType(TicketType ticketType)
{
	if (m_ticketType != ticketType){
		istd::CChangeNotifier notifier(this);

		m_ticketType = ticketType;
	}
}


ISupportTicket::TicketStatus CSupportTicketComp::GetStatus() const
{
	return m_status;
}


void CSupportTicketComp::SetStatus(TicketStatus status)
{
	if (m_status != status){
		istd::CChangeNotifier notifier(this);

		m_status = status;
	}
}


ISupportTicket::StateReason CSupportTicketComp::GetStateReason() const
{
	return m_stateReason;
}


void CSupportTicketComp::SetStateReason(StateReason stateReason)
{
	if (m_stateReason != stateReason){
		istd::CChangeNotifier notifier(this);

		m_stateReason = stateReason;
	}
}


ISupportTicket::TicketPriority CSupportTicketComp::GetPriority() const
{
	return m_priority;
}


void CSupportTicketComp::SetPriority(TicketPriority priority)
{
	if (m_priority != priority){
		istd::CChangeNotifier notifier(this);

		m_priority = priority;
	}
}


QByteArrayList CSupportTicketComp::GetAssigneeIds() const
{
	return m_assigneeIds;
}


void CSupportTicketComp::SetAssigneeIds(const QByteArrayList& assigneeIds)
{
	if (m_assigneeIds != assigneeIds){
		istd::CChangeNotifier notifier(this);

		m_assigneeIds = assigneeIds;
	}
}


QByteArray CSupportTicketComp::GetReporterId() const
{
	return m_reporterId;
}


void CSupportTicketComp::SetReporterId(const QByteArray& reporterId)
{
	if (m_reporterId != reporterId){
		istd::CChangeNotifier notifier(this);

		m_reporterId = reporterId;
	}
}


QByteArray CSupportTicketComp::GetConversationId() const
{
	return m_conversationId;
}


void CSupportTicketComp::SetConversationId(const QByteArray& conversationId)
{
	if (m_conversationId != conversationId){
		istd::CChangeNotifier notifier(this);

		m_conversationId = conversationId;
	}
}


QByteArray CSupportTicketComp::GetMessageId() const
{
	return m_messageId;
}


void CSupportTicketComp::SetMessageId(const QByteArray& messageId)
{
	if (m_messageId != messageId){
		istd::CChangeNotifier notifier(this);

		m_messageId = messageId;
	}
}


QStringList CSupportTicketComp::GetTags() const
{
	return m_tags;
}


void CSupportTicketComp::SetTags(const QStringList& tags)
{
	if (m_tags != tags){
		istd::CChangeNotifier notifier(this);

		m_tags = tags;
	}
}


QByteArrayList CSupportTicketComp::GetLabelIds() const
{
	return m_labelIds;
}


void CSupportTicketComp::SetLabelIds(const QByteArrayList& labelIds)
{
	if (m_labelIds != labelIds){
		istd::CChangeNotifier notifier(this);

		m_labelIds = labelIds;
	}
}


bool CSupportTicketComp::IsLocked() const
{
	return m_locked;
}


void CSupportTicketComp::SetLocked(bool locked)
{
	if (m_locked != locked){
		istd::CChangeNotifier notifier(this);

		m_locked = locked;
	}
}


QString CSupportTicketComp::GetLockReason() const
{
	return m_lockReason;
}


void CSupportTicketComp::SetLockReason(const QString& lockReason)
{
	if (m_lockReason != lockReason){
		istd::CChangeNotifier notifier(this);

		m_lockReason = lockReason;
	}
}


int CSupportTicketComp::GetNumber() const
{
	return m_number;
}


void CSupportTicketComp::SetNumber(int number)
{
	if (m_number != number){
		istd::CChangeNotifier notifier(this);

		m_number = number;
	}
}


QString CSupportTicketComp::GetCreatedAt() const
{
	return m_createdAt;
}


void CSupportTicketComp::SetCreatedAt(const QString& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier notifier(this);

		m_createdAt = createdAt;
	}
}


QString CSupportTicketComp::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CSupportTicketComp::SetUpdatedAt(const QString& updatedAt)
{
	if (m_updatedAt != updatedAt){
		istd::CChangeNotifier notifier(this);

		m_updatedAt = updatedAt;
	}
}


QString CSupportTicketComp::GetClosedAt() const
{
	return m_closedAt;
}


void CSupportTicketComp::SetClosedAt(const QString& closedAt)
{
	if (m_closedAt != closedAt){
		istd::CChangeNotifier notifier(this);

		m_closedAt = closedAt;
	}
}


QString CSupportTicketComp::GetResolvedAt() const
{
	return m_resolvedAt;
}


void CSupportTicketComp::SetResolvedAt(const QString& resolvedAt)
{
	if (m_resolvedAt != resolvedAt){
		istd::CChangeNotifier notifier(this);

		m_resolvedAt = resolvedAt;
	}
}


QByteArrayList CSupportTicketComp::GetEntityReferences() const
{
	return m_entityReferences;
}


void CSupportTicketComp::SetEntityReferences(const QByteArrayList& entityReferences)
{
	if (m_entityReferences != entityReferences){
		istd::CChangeNotifier notifier(this);

		m_entityReferences = entityReferences;
	}
}


// reimplemented (iser::ISerializable)

bool CSupportTicketComp::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	iser::CArchiveTag titleTag("Title", "Title", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(titleTag);
	retVal = retVal && archive.Process(m_title);
	retVal = retVal && archive.EndTag(titleTag);

	iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	iser::CArchiveTag ticketTypeTag("TicketType", "Ticket type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(ticketTypeTag);
	retVal = retVal && I_SERIALIZE_ENUM(TicketType, archive, m_ticketType);
	retVal = retVal && archive.EndTag(ticketTypeTag);

	iser::CArchiveTag statusTag("Status", "Status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	retVal = retVal && I_SERIALIZE_ENUM(TicketStatus, archive, m_status);
	retVal = retVal && archive.EndTag(statusTag);

	iser::CArchiveTag stateReasonTag("StateReason", "State reason", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(stateReasonTag);
	retVal = retVal && I_SERIALIZE_ENUM(StateReason, archive, m_stateReason);
	retVal = retVal && archive.EndTag(stateReasonTag);

	iser::CArchiveTag priorityTag("Priority", "Priority", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(priorityTag);
	retVal = retVal && I_SERIALIZE_ENUM(TicketPriority, archive, m_priority);
	retVal = retVal && archive.EndTag(priorityTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_assigneeIds, "AssigneeIds", "AssigneeId");

	iser::CArchiveTag reporterIdTag("ReporterId", "Reporter ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(reporterIdTag);
	retVal = retVal && archive.Process(m_reporterId);
	retVal = retVal && archive.EndTag(reporterIdTag);

	iser::CArchiveTag conversationIdTag("ConversationId", "Conversation ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(conversationIdTag);
	retVal = retVal && archive.Process(m_conversationId);
	retVal = retVal && archive.EndTag(conversationIdTag);

	iser::CArchiveTag messageIdTag("MessageId", "Message ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(messageIdTag);
	retVal = retVal && archive.Process(m_messageId);
	retVal = retVal && archive.EndTag(messageIdTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QStringList>(archive, m_tags, "Tags", "Tag");

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_labelIds, "LabelIds", "LabelId");

	iser::CArchiveTag lockedTag("Locked", "Locked", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(lockedTag);
	retVal = retVal && archive.Process(m_locked);
	retVal = retVal && archive.EndTag(lockedTag);

	iser::CArchiveTag lockReasonTag("LockReason", "Lock reason", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(lockReasonTag);
	retVal = retVal && archive.Process(m_lockReason);
	retVal = retVal && archive.EndTag(lockReasonTag);

	iser::CArchiveTag numberTag("Number", "Number", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(numberTag);
	retVal = retVal && archive.Process(m_number);
	retVal = retVal && archive.EndTag(numberTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && archive.Process(m_updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	iser::CArchiveTag closedAtTag("ClosedAt", "Closed at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(closedAtTag);
	retVal = retVal && archive.Process(m_closedAt);
	retVal = retVal && archive.EndTag(closedAtTag);

	iser::CArchiveTag resolvedAtTag("ResolvedAt", "Resolved at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(resolvedAtTag);
	retVal = retVal && archive.Process(m_resolvedAt);
	retVal = retVal && archive.EndTag(resolvedAtTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_entityReferences, "EntityReferences", "EntityReferenceId");

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CSupportTicketComp::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const ISupportTicket* srcPtr = dynamic_cast<const ISupportTicket*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_id = srcPtr->GetId();
	m_title = srcPtr->GetTitle();
	m_description = srcPtr->GetDescription();
	m_ticketType = srcPtr->GetTicketType();
	m_status = srcPtr->GetStatus();
	m_stateReason = srcPtr->GetStateReason();
	m_priority = srcPtr->GetPriority();
	m_assigneeIds = srcPtr->GetAssigneeIds();
	m_reporterId = srcPtr->GetReporterId();
	m_conversationId = srcPtr->GetConversationId();
	m_messageId = srcPtr->GetMessageId();
	m_tags = srcPtr->GetTags();
	m_labelIds = srcPtr->GetLabelIds();
	m_locked = srcPtr->IsLocked();
	m_lockReason = srcPtr->GetLockReason();
	m_number = srcPtr->GetNumber();
	m_createdAt = srcPtr->GetCreatedAt();
	m_updatedAt = srcPtr->GetUpdatedAt();
	m_closedAt = srcPtr->GetClosedAt();
	m_resolvedAt = srcPtr->GetResolvedAt();
	m_entityReferences = srcPtr->GetEntityReferences();

	return true;
}


bool CSupportTicketComp::IsEqual(const IChangeable& object) const
{
	const ISupportTicket* srcPtr = dynamic_cast<const ISupportTicket*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	bool result = m_id == srcPtr->GetId()
		&& m_title == srcPtr->GetTitle()
		&& m_description == srcPtr->GetDescription()
		&& m_ticketType == srcPtr->GetTicketType()
		&& m_status == srcPtr->GetStatus()
		&& m_stateReason == srcPtr->GetStateReason()
		&& m_priority == srcPtr->GetPriority()
		&& m_assigneeIds == srcPtr->GetAssigneeIds()
		&& m_reporterId == srcPtr->GetReporterId()
		&& m_conversationId == srcPtr->GetConversationId()
		&& m_messageId == srcPtr->GetMessageId()
		&& m_tags == srcPtr->GetTags()
		&& m_labelIds == srcPtr->GetLabelIds()
		&& m_locked == srcPtr->IsLocked()
		&& m_lockReason == srcPtr->GetLockReason()
		&& m_number == srcPtr->GetNumber()
		&& m_createdAt == srcPtr->GetCreatedAt()
		&& m_updatedAt == srcPtr->GetUpdatedAt()
		&& m_closedAt == srcPtr->GetClosedAt()
		&& m_resolvedAt == srcPtr->GetResolvedAt()
		&& m_entityReferences == srcPtr->GetEntityReferences();

	return result;
}


istd::IChangeableUniquePtr CSupportTicketComp::CloneMe(CompatibilityMode mode) const
{
	istd::TUniqueInterfacePtr<CSupportTicketComp> clonePtr(new CSupportTicketComp());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CSupportTicketComp::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_id.clear();
	m_title.clear();
	m_description.clear();
	m_ticketType = TT_ACCESS_REQUEST;
	m_status = TS_OPEN;
	m_stateReason = SR_NONE;
	m_priority = TP_MEDIUM;
	m_assigneeIds.clear();
	m_reporterId.clear();
	m_conversationId.clear();
	m_messageId.clear();
	m_tags.clear();
	m_labelIds.clear();
	m_locked = false;
	m_lockReason.clear();
	m_number = 0;
	m_createdAt.clear();
	m_updatedAt.clear();
	m_closedAt.clear();
	m_resolvedAt.clear();
	m_entityReferences.clear();

	return true;
}


} // namespace imtdesk
