// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchat/CConversationComp.h>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtchat
{


// reimplemented (imtchat::IConversation)

QByteArray CConversationComp::GetId() const
{
	return m_id;
}


void CConversationComp::SetId(const QByteArray& id)
{
	if (m_id != id){
		istd::CChangeNotifier notifier(this);

		m_id = id;
	}
}


QString CConversationComp::GetName() const
{
	return m_name;
}


void CConversationComp::SetName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier notifier(this);

		m_name = name;
	}
}


IConversation::ConversationType CConversationComp::GetConversationType() const
{
	return m_conversationType;
}


void CConversationComp::SetConversationType(IConversation::ConversationType conversationType)
{
	if (m_conversationType != conversationType){
		istd::CChangeNotifier notifier(this);

		m_conversationType = conversationType;
	}
}


QByteArrayList CConversationComp::GetParticipantIds() const
{
	return m_participantIds;
}


void CConversationComp::SetParticipantIds(const QByteArrayList& participantIds)
{
	if (m_participantIds != participantIds){
		istd::CChangeNotifier notifier(this);

		m_participantIds = participantIds;
	}
}


QString CConversationComp::GetCreatedAt() const
{
	return m_createdAt;
}


void CConversationComp::SetCreatedAt(const QString& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier notifier(this);

		m_createdAt = createdAt;
	}
}


QString CConversationComp::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CConversationComp::SetUpdatedAt(const QString& updatedAt)
{
	if (m_updatedAt != updatedAt){
		istd::CChangeNotifier notifier(this);

		m_updatedAt = updatedAt;
	}
}


QString CConversationComp::GetMetadata() const
{
	return m_metadata;
}


void CConversationComp::SetMetadata(const QString& metadata)
{
	if (m_metadata != metadata){
		istd::CChangeNotifier notifier(this);

		m_metadata = metadata;
	}
}


// reimplemented (iser::ISerializable)

bool CConversationComp::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag nameTag("Name", "Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	retVal = retVal && I_SERIALIZE_ENUM(ConversationType, archive, m_conversationType);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_participantIds, "ParticipantIds", "ParticipantId");

	static iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	static iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && archive.Process(m_updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	static iser::CArchiveTag metadataTag("Metadata", "Metadata", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(metadataTag);
	retVal = retVal && archive.Process(m_metadata);
	retVal = retVal && archive.EndTag(metadataTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CConversationComp::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IConversation* srcPtr = dynamic_cast<const IConversation*>(&object);
	if (srcPtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

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
	istd::CChangeNotifier notifier(this);

	m_id.clear();
	m_name.clear();
	m_conversationType = IConversation::CT_DIRECT;
	m_participantIds.clear();
	m_createdAt.clear();
	m_updatedAt.clear();
	m_metadata.clear();
	return true;
}


} // namespace imtchat
