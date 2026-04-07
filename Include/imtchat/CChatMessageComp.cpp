// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchat/CChatMessageComp.h>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtchat
{


// reimplemented (imtchat::IChatMessage)

QByteArray CChatMessageComp::GetId() const
{
	return m_id;
}


void CChatMessageComp::SetId(const QByteArray& id)
{
	if (m_id != id){
		istd::CChangeNotifier notifier(this);

		m_id = id;
	}
}


QByteArray CChatMessageComp::GetConversationId() const
{
	return m_conversationId;
}


void CChatMessageComp::SetConversationId(const QByteArray& conversationId)
{
	if (m_conversationId != conversationId){
		istd::CChangeNotifier notifier(this);

		m_conversationId = conversationId;
	}
}


QByteArray CChatMessageComp::GetSenderId() const
{
	return m_senderId;
}


void CChatMessageComp::SetSenderId(const QByteArray& senderId)
{
	if (m_senderId != senderId){
		istd::CChangeNotifier notifier(this);

		m_senderId = senderId;
	}
}


QString CChatMessageComp::GetContent() const
{
	return m_content;
}


void CChatMessageComp::SetContent(const QString& content)
{
	if (m_content != content){
		istd::CChangeNotifier notifier(this);

		m_content = content;
	}
}


IChatMessage::MessageStatus CChatMessageComp::GetStatus() const
{
	return m_status;
}


void CChatMessageComp::SetStatus(IChatMessage::MessageStatus status)
{
	if (m_status != status){
		istd::CChangeNotifier notifier(this);

		m_status = status;
	}
}


QString CChatMessageComp::GetCreatedAt() const
{
	return m_createdAt;
}


void CChatMessageComp::SetCreatedAt(const QString& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier notifier(this);

		m_createdAt = createdAt;
	}
}


QString CChatMessageComp::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CChatMessageComp::SetUpdatedAt(const QString& updatedAt)
{
	if (m_updatedAt != updatedAt){
		istd::CChangeNotifier notifier(this);

		m_updatedAt = updatedAt;
	}
}


QByteArrayList CChatMessageComp::GetEntityReferences() const
{
	return m_entityReferences;
}


void CChatMessageComp::SetEntityReferences(const QByteArrayList& entityReferences)
{
	if (m_entityReferences != entityReferences){
		istd::CChangeNotifier notifier(this);

		m_entityReferences = entityReferences;
	}
}


QByteArrayList CChatMessageComp::GetAttachmentIds() const
{
	return m_attachmentIds;
}


void CChatMessageComp::SetAttachmentIds(const QByteArrayList& attachmentIds)
{
	if (m_attachmentIds != attachmentIds){
		istd::CChangeNotifier notifier(this);

		m_attachmentIds = attachmentIds;
	}
}


// reimplemented (iser::ISerializable)

bool CChatMessageComp::Serialize(iser::IArchive& archive)
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

	retVal = retVal && I_SERIALIZE_ENUM(MessageStatus, archive, m_status);

	static iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	static iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && archive.Process(m_updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_entityReferences, "EntityReferences", "EntityReference");

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_attachmentIds, "AttachmentIds", "AttachmentId");

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CChatMessageComp::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IChatMessage* srcPtr = dynamic_cast<const IChatMessage*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

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


bool CChatMessageComp::IsEqual(const IChangeable& object) const
{
	const IChatMessage* srcPtr = dynamic_cast<const IChatMessage*>(&object);
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


istd::IChangeableUniquePtr CChatMessageComp::CloneMe(CompatibilityMode mode) const
{
	istd::TUniqueInterfacePtr<CChatMessageComp> clonePtr(new CChatMessageComp());
	clonePtr->CopyFrom(*this, mode);
	return clonePtr;
}


bool CChatMessageComp::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_id.clear();
	m_conversationId.clear();
	m_senderId.clear();
	m_content.clear();
	m_status = IChatMessage::MS_SENT;
	m_createdAt.clear();
	m_updatedAt.clear();
	m_entityReferences.clear();
	m_attachmentIds.clear();
	return true;
}


} // namespace imtchat
