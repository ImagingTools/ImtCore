// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchat/CConversationComp.h>

// ACF includes
#include <iser/IArchive.h>


namespace imtchat
{


// reimplemented (imtchat::IConversation)

QByteArray CConversationComp::GetId() const
{
	return m_id;
}


void CConversationComp::SetId(const QByteArray& id)
{
	m_id = id;
}


QString CConversationComp::GetName() const
{
	return m_name;
}


void CConversationComp::SetName(const QString& name)
{
	m_name = name;
}


int CConversationComp::GetConversationType() const
{
	return m_conversationType;
}


void CConversationComp::SetConversationType(int conversationType)
{
	m_conversationType = conversationType;
}


QByteArrayList CConversationComp::GetParticipantIds() const
{
	return m_participantIds;
}


void CConversationComp::SetParticipantIds(const QByteArrayList& participantIds)
{
	m_participantIds = participantIds;
}


QString CConversationComp::GetCreatedAt() const
{
	return m_createdAt;
}


void CConversationComp::SetCreatedAt(const QString& createdAt)
{
	m_createdAt = createdAt;
}


QString CConversationComp::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CConversationComp::SetUpdatedAt(const QString& updatedAt)
{
	m_updatedAt = updatedAt;
}


QString CConversationComp::GetMetadata() const
{
	return m_metadata;
}


void CConversationComp::SetMetadata(const QString& metadata)
{
	m_metadata = metadata;
}


// reimplemented (iser::ISerializable)

bool CConversationComp::Serialize(iser::IArchive& archive)
{
	archive.Serialize(m_id, "id");
	archive.Serialize(m_name, "name");
	archive.Serialize(m_conversationType, "conversationType");
	archive.Serialize(m_createdAt, "createdAt");
	archive.Serialize(m_updatedAt, "updatedAt");
	archive.Serialize(m_metadata, "metadata");
	return archive.IsOk();
}


// reimplemented (istd::IChangeable)

bool CConversationComp::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IConversation* srcPtr = dynamic_cast<const IConversation*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	m_id = srcPtr->GetId();
	m_name = srcPtr->GetName();
	m_conversationType = srcPtr->GetConversationType();
	m_participantIds = srcPtr->GetParticipantIds();
	m_createdAt = srcPtr->GetCreatedAt();
	m_updatedAt = srcPtr->GetUpdatedAt();
	m_metadata = srcPtr->GetMetadata();
	return true;
}


bool CConversationComp::IsEqual(const IChangeable& object) const
{
	const IConversation* srcPtr = dynamic_cast<const IConversation*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	return m_id == srcPtr->GetId()
		&& m_name == srcPtr->GetName()
		&& m_conversationType == srcPtr->GetConversationType()
		&& m_participantIds == srcPtr->GetParticipantIds()
		&& m_createdAt == srcPtr->GetCreatedAt()
		&& m_updatedAt == srcPtr->GetUpdatedAt()
		&& m_metadata == srcPtr->GetMetadata();
}


istd::IChangeableUniquePtr CConversationComp::CloneMe(CompatibilityMode mode) const
{
	istd::TUniqueInterfacePtr<CConversationComp> clonePtr(new CConversationComp());
	clonePtr->CopyFrom(*this, mode);
	return clonePtr;
}


bool CConversationComp::ResetData(CompatibilityMode /*mode*/)
{
	m_id.clear();
	m_name.clear();
	m_conversationType = 0;
	m_participantIds.clear();
	m_createdAt.clear();
	m_updatedAt.clear();
	m_metadata.clear();
	return true;
}


} // namespace imtchat
