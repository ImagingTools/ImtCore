// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchat/CMessageComp.h>

// ACF includes
#include <iser/IArchive.h>


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
	archive.Serialize(m_id, "id");
	archive.Serialize(m_conversationId, "conversationId");
	archive.Serialize(m_senderId, "senderId");
	archive.Serialize(m_content, "content");
	archive.Serialize(m_status, "status");
	archive.Serialize(m_createdAt, "createdAt");
	archive.Serialize(m_updatedAt, "updatedAt");
	archive.Serialize(m_entityReferences, "entityReferences");
	return archive.IsOk();
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
