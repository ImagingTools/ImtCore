// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchat/CMessageComp.h>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtchat
{


// reimplemented (imtchat::IMessage)

QByteArray CMessageComp::GetId() const
{
	return m_id;
}


void CMessageComp::SetId(const QByteArray& id)
{
	m_id = id;
}


QByteArray CMessageComp::GetConversationId() const
{
	return m_conversationId;
}


void CMessageComp::SetConversationId(const QByteArray& conversationId)
{
	m_conversationId = conversationId;
}


QByteArray CMessageComp::GetSenderId() const
{
	return m_senderId;
}


void CMessageComp::SetSenderId(const QByteArray& senderId)
{
	m_senderId = senderId;
}


QString CMessageComp::GetContent() const
{
	return m_content;
}


void CMessageComp::SetContent(const QString& content)
{
	m_content = content;
}


int CMessageComp::GetStatus() const
{
	return m_status;
}


void CMessageComp::SetStatus(int status)
{
	m_status = status;
}


QString CMessageComp::GetCreatedAt() const
{
	return m_createdAt;
}


void CMessageComp::SetCreatedAt(const QString& createdAt)
{
	m_createdAt = createdAt;
}


QString CMessageComp::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CMessageComp::SetUpdatedAt(const QString& updatedAt)
{
	m_updatedAt = updatedAt;
}


QString CMessageComp::GetEntityReferences() const
{
	return m_entityReferences;
}


void CMessageComp::SetEntityReferences(const QString& entityReferences)
{
	m_entityReferences = entityReferences;
}


QByteArrayList CMessageComp::GetAttachmentIds() const
{
	return m_attachmentIds;
}


void CMessageComp::SetAttachmentIds(const QByteArrayList& attachmentIds)
{
	m_attachmentIds = attachmentIds;
}


// reimplemented (iser::ISerializable)

bool CMessageComp::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag conversationIdTag("ConversationId", "Conversation id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(conversationIdTag);
	retVal = retVal && archive.Process(m_conversationId);
	retVal = retVal && archive.EndTag(conversationIdTag);

	static iser::CArchiveTag senderIdTag("SenderId", "Sender id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(senderIdTag);
	retVal = retVal && archive.Process(m_senderId);
	retVal = retVal && archive.EndTag(senderIdTag);

	static iser::CArchiveTag contentTag("Content", "Content", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(contentTag);
	retVal = retVal && archive.Process(m_content);
	retVal = retVal && archive.EndTag(contentTag);

	static iser::CArchiveTag statusTag("Status", "Status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	retVal = retVal && archive.Process(m_status);
	retVal = retVal && archive.EndTag(statusTag);

	static iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	static iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && archive.Process(m_updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	static iser::CArchiveTag entityReferencesTag("EntityReferences", "Entity references", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(entityReferencesTag);
	retVal = retVal && archive.Process(m_entityReferences);
	retVal = retVal && archive.EndTag(entityReferencesTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_attachmentIds, "AttachmentIds", "AttachmentId");

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CMessageComp::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IMessage* srcPtr = dynamic_cast<const IMessage*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	m_id = srcPtr->GetId();
	m_conversationId = srcPtr->GetConversationId();
	m_senderId = srcPtr->GetSenderId();
	m_content = srcPtr->GetContent();
	m_status = srcPtr->GetStatus();
	m_createdAt = srcPtr->GetCreatedAt();
	m_updatedAt = srcPtr->GetUpdatedAt();
	m_entityReferences = srcPtr->GetEntityReferences();
	m_attachmentIds = srcPtr->GetAttachmentIds();
	return true;
}


bool CMessageComp::IsEqual(const IChangeable& object) const
{
	const IMessage* srcPtr = dynamic_cast<const IMessage*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	return m_id == srcPtr->GetId()
		&& m_conversationId == srcPtr->GetConversationId()
		&& m_senderId == srcPtr->GetSenderId()
		&& m_content == srcPtr->GetContent()
		&& m_status == srcPtr->GetStatus()
		&& m_createdAt == srcPtr->GetCreatedAt()
		&& m_updatedAt == srcPtr->GetUpdatedAt()
		&& m_entityReferences == srcPtr->GetEntityReferences()
		&& m_attachmentIds == srcPtr->GetAttachmentIds();
}


istd::IChangeableUniquePtr CMessageComp::CloneMe(CompatibilityMode mode) const
{
	istd::TUniqueInterfacePtr<CMessageComp> clonePtr(new CMessageComp());
	clonePtr->CopyFrom(*this, mode);
	return clonePtr;
}


bool CMessageComp::ResetData(CompatibilityMode /*mode*/)
{
	m_id.clear();
	m_conversationId.clear();
	m_senderId.clear();
	m_content.clear();
	m_status = 0;
	m_createdAt.clear();
	m_updatedAt.clear();
	m_entityReferences.clear();
	m_attachmentIds.clear();
	return true;
}


} // namespace imtchat
