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
	m_id = id;
}


QString CSupportTicketComp::GetTitle() const
{
	return m_title;
}


void CSupportTicketComp::SetTitle(const QString& title)
{
	m_title = title;
}


QString CSupportTicketComp::GetDescription() const
{
	return m_description;
}


void CSupportTicketComp::SetDescription(const QString& description)
{
	m_description = description;
}


ISupportTicket::TicketType CSupportTicketComp::GetTicketType() const
{
	return m_ticketType;
}


void CSupportTicketComp::SetTicketType(TicketType ticketType)
{
	m_ticketType = ticketType;
}


ISupportTicket::TicketStatus CSupportTicketComp::GetStatus() const
{
	return m_status;
}


void CSupportTicketComp::SetStatus(TicketStatus status)
{
	m_status = status;
}


ISupportTicket::TicketPriority CSupportTicketComp::GetPriority() const
{
	return m_priority;
}


void CSupportTicketComp::SetPriority(TicketPriority priority)
{
	m_priority = priority;
}


QByteArray CSupportTicketComp::GetAssigneeId() const
{
	return m_assigneeId;
}


void CSupportTicketComp::SetAssigneeId(const QByteArray& assigneeId)
{
	m_assigneeId = assigneeId;
}


QByteArray CSupportTicketComp::GetReporterId() const
{
	return m_reporterId;
}


void CSupportTicketComp::SetReporterId(const QByteArray& reporterId)
{
	m_reporterId = reporterId;
}


QByteArray CSupportTicketComp::GetConversationId() const
{
	return m_conversationId;
}


void CSupportTicketComp::SetConversationId(const QByteArray& conversationId)
{
	m_conversationId = conversationId;
}


QByteArray CSupportTicketComp::GetMessageId() const
{
	return m_messageId;
}


void CSupportTicketComp::SetMessageId(const QByteArray& messageId)
{
	m_messageId = messageId;
}


ISupportTicket::Environment CSupportTicketComp::GetEnvironment() const
{
	return m_environment;
}


void CSupportTicketComp::SetEnvironment(Environment environment)
{
	m_environment = environment;
}


QStringList CSupportTicketComp::GetTags() const
{
	return m_tags;
}


void CSupportTicketComp::SetTags(const QStringList& tags)
{
	m_tags = tags;
}


QString CSupportTicketComp::GetCreatedAt() const
{
	return m_createdAt;
}


void CSupportTicketComp::SetCreatedAt(const QString& createdAt)
{
	m_createdAt = createdAt;
}


QString CSupportTicketComp::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CSupportTicketComp::SetUpdatedAt(const QString& updatedAt)
{
	m_updatedAt = updatedAt;
}


QString CSupportTicketComp::GetResolvedAt() const
{
	return m_resolvedAt;
}


void CSupportTicketComp::SetResolvedAt(const QString& resolvedAt)
{
	m_resolvedAt = resolvedAt;
}


// reimplemented (iser::ISerializable)

bool CSupportTicketComp::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag titleTag("Title", "Title", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(titleTag);
	retVal = retVal && archive.Process(m_title);
	retVal = retVal && archive.EndTag(titleTag);

	static iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	static iser::CArchiveTag ticketTypeTag("TicketType", "Ticket type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(ticketTypeTag);
	retVal = retVal && I_SERIALIZE_ENUM(TicketType, archive, m_ticketType);
	retVal = retVal && archive.EndTag(ticketTypeTag);

	static iser::CArchiveTag statusTag("Status", "Status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	retVal = retVal && I_SERIALIZE_ENUM(TicketStatus, archive, m_status);
	retVal = retVal && archive.EndTag(statusTag);

	static iser::CArchiveTag priorityTag("Priority", "Priority", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(priorityTag);
	retVal = retVal && I_SERIALIZE_ENUM(TicketPriority, archive, m_priority);
	retVal = retVal && archive.EndTag(priorityTag);

	static iser::CArchiveTag assigneeIdTag("AssigneeId", "Assignee ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(assigneeIdTag);
	retVal = retVal && archive.Process(m_assigneeId);
	retVal = retVal && archive.EndTag(assigneeIdTag);

	static iser::CArchiveTag reporterIdTag("ReporterId", "Reporter ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(reporterIdTag);
	retVal = retVal && archive.Process(m_reporterId);
	retVal = retVal && archive.EndTag(reporterIdTag);

	static iser::CArchiveTag conversationIdTag("ConversationId", "Conversation ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(conversationIdTag);
	retVal = retVal && archive.Process(m_conversationId);
	retVal = retVal && archive.EndTag(conversationIdTag);

	static iser::CArchiveTag messageIdTag("MessageId", "Message ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(messageIdTag);
	retVal = retVal && archive.Process(m_messageId);
	retVal = retVal && archive.EndTag(messageIdTag);

	static iser::CArchiveTag environmentTag("Environment", "Environment", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(environmentTag);
	retVal = retVal && I_SERIALIZE_ENUM(Environment, archive, m_environment);
	retVal = retVal && archive.EndTag(environmentTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QStringList>(archive, m_tags, "Tags", "Tag");

	static iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	static iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && archive.Process(m_updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	static iser::CArchiveTag resolvedAtTag("ResolvedAt", "Resolved at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(resolvedAtTag);
	retVal = retVal && archive.Process(m_resolvedAt);
	retVal = retVal && archive.EndTag(resolvedAtTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CSupportTicketComp::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const ISupportTicket* srcPtr = dynamic_cast<const ISupportTicket*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	m_id = srcPtr->GetId();
	m_title = srcPtr->GetTitle();
	m_description = srcPtr->GetDescription();
	m_ticketType = srcPtr->GetTicketType();
	m_status = srcPtr->GetStatus();
	m_priority = srcPtr->GetPriority();
	m_assigneeId = srcPtr->GetAssigneeId();
	m_reporterId = srcPtr->GetReporterId();
	m_conversationId = srcPtr->GetConversationId();
	m_messageId = srcPtr->GetMessageId();
	m_environment = srcPtr->GetEnvironment();
	m_tags = srcPtr->GetTags();
	m_createdAt = srcPtr->GetCreatedAt();
	m_updatedAt = srcPtr->GetUpdatedAt();
	m_resolvedAt = srcPtr->GetResolvedAt();
	return true;
}


bool CSupportTicketComp::IsEqual(const IChangeable& object) const
{
	const ISupportTicket* srcPtr = dynamic_cast<const ISupportTicket*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	return m_id == srcPtr->GetId()
		&& m_title == srcPtr->GetTitle()
		&& m_description == srcPtr->GetDescription()
		&& m_ticketType == srcPtr->GetTicketType()
		&& m_status == srcPtr->GetStatus()
		&& m_priority == srcPtr->GetPriority()
		&& m_assigneeId == srcPtr->GetAssigneeId()
		&& m_reporterId == srcPtr->GetReporterId()
		&& m_conversationId == srcPtr->GetConversationId()
		&& m_messageId == srcPtr->GetMessageId()
		&& m_environment == srcPtr->GetEnvironment()
		&& m_tags == srcPtr->GetTags()
		&& m_createdAt == srcPtr->GetCreatedAt()
		&& m_updatedAt == srcPtr->GetUpdatedAt()
		&& m_resolvedAt == srcPtr->GetResolvedAt();
}


istd::IChangeableUniquePtr CSupportTicketComp::CloneMe(CompatibilityMode mode) const
{
	istd::TUniqueInterfacePtr<CSupportTicketComp> clonePtr(new CSupportTicketComp());
	clonePtr->CopyFrom(*this, mode);
	return clonePtr;
}


bool CSupportTicketComp::ResetData(CompatibilityMode /*mode*/)
{
	m_id.clear();
	m_title.clear();
	m_description.clear();
	m_ticketType = TT_ACCESS_REQUEST;
	m_status = TS_OPEN;
	m_priority = TP_MEDIUM;
	m_assigneeId.clear();
	m_reporterId.clear();
	m_conversationId.clear();
	m_messageId.clear();
	m_environment = ENV_PRODUCTION;
	m_tags.clear();
	m_createdAt.clear();
	m_updatedAt.clear();
	m_resolvedAt.clear();
	return true;
}


} // namespace imtdesk
