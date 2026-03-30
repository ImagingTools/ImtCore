// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchat/CTicketComp.h>

// ACF includes
#include <iser/IArchive.h>


namespace imtchat
{


// reimplemented (imtchat::ITicket)

QByteArray CTicketComp::GetId() const
{
	return m_id;
}


void CTicketComp::SetId(const QByteArray& id)
{
	m_id = id;
}


QString CTicketComp::GetTitle() const
{
	return m_title;
}


void CTicketComp::SetTitle(const QString& title)
{
	m_title = title;
}


QString CTicketComp::GetDescription() const
{
	return m_description;
}


void CTicketComp::SetDescription(const QString& description)
{
	m_description = description;
}


int CTicketComp::GetTicketType() const
{
	return m_ticketType;
}


void CTicketComp::SetTicketType(int ticketType)
{
	m_ticketType = ticketType;
}


int CTicketComp::GetStatus() const
{
	return m_status;
}


void CTicketComp::SetStatus(int status)
{
	m_status = status;
}


int CTicketComp::GetPriority() const
{
	return m_priority;
}


void CTicketComp::SetPriority(int priority)
{
	m_priority = priority;
}


QByteArray CTicketComp::GetAssigneeId() const
{
	return m_assigneeId;
}


void CTicketComp::SetAssigneeId(const QByteArray& assigneeId)
{
	m_assigneeId = assigneeId;
}


QByteArray CTicketComp::GetReporterId() const
{
	return m_reporterId;
}


void CTicketComp::SetReporterId(const QByteArray& reporterId)
{
	m_reporterId = reporterId;
}


QByteArray CTicketComp::GetConversationId() const
{
	return m_conversationId;
}


void CTicketComp::SetConversationId(const QByteArray& conversationId)
{
	m_conversationId = conversationId;
}


QByteArray CTicketComp::GetMessageId() const
{
	return m_messageId;
}


void CTicketComp::SetMessageId(const QByteArray& messageId)
{
	m_messageId = messageId;
}


int CTicketComp::GetEnvironment() const
{
	return m_environment;
}


void CTicketComp::SetEnvironment(int environment)
{
	m_environment = environment;
}


QStringList CTicketComp::GetTags() const
{
	return m_tags;
}


void CTicketComp::SetTags(const QStringList& tags)
{
	m_tags = tags;
}


QString CTicketComp::GetCreatedAt() const
{
	return m_createdAt;
}


void CTicketComp::SetCreatedAt(const QString& createdAt)
{
	m_createdAt = createdAt;
}


QString CTicketComp::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CTicketComp::SetUpdatedAt(const QString& updatedAt)
{
	m_updatedAt = updatedAt;
}


QString CTicketComp::GetResolvedAt() const
{
	return m_resolvedAt;
}


void CTicketComp::SetResolvedAt(const QString& resolvedAt)
{
	m_resolvedAt = resolvedAt;
}


// reimplemented (iser::ISerializable)

bool CTicketComp::Serialize(iser::IArchive& archive)
{
	archive.Serialize(m_id, "id");
	archive.Serialize(m_title, "title");
	archive.Serialize(m_description, "description");
	archive.Serialize(m_ticketType, "ticketType");
	archive.Serialize(m_status, "status");
	archive.Serialize(m_priority, "priority");
	archive.Serialize(m_assigneeId, "assigneeId");
	archive.Serialize(m_reporterId, "reporterId");
	archive.Serialize(m_conversationId, "conversationId");
	archive.Serialize(m_messageId, "messageId");
	archive.Serialize(m_environment, "environment");
	archive.Serialize(m_createdAt, "createdAt");
	archive.Serialize(m_updatedAt, "updatedAt");
	archive.Serialize(m_resolvedAt, "resolvedAt");
	return archive.IsOk();
}


// reimplemented (istd::IChangeable)

bool CTicketComp::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const ITicket* srcPtr = dynamic_cast<const ITicket*>(&object);
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


bool CTicketComp::IsEqual(const IChangeable& object) const
{
	const ITicket* srcPtr = dynamic_cast<const ITicket*>(&object);
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


istd::IChangeableUniquePtr CTicketComp::CloneMe(CompatibilityMode mode) const
{
	istd::TUniqueInterfacePtr<CTicketComp> clonePtr(new CTicketComp());
	clonePtr->CopyFrom(*this, mode);
	return clonePtr;
}


bool CTicketComp::ResetData(CompatibilityMode /*mode*/)
{
	m_id.clear();
	m_title.clear();
	m_description.clear();
	m_ticketType = 0;
	m_status = 0;
	m_priority = 1;
	m_assigneeId.clear();
	m_reporterId.clear();
	m_conversationId.clear();
	m_messageId.clear();
	m_environment = 2;
	m_tags.clear();
	m_createdAt.clear();
	m_updatedAt.clear();
	m_resolvedAt.clear();
	return true;
}


} // namespace imtchat
