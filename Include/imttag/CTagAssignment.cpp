// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttag/CTagAssignment.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imttag
{


// reimplemented (imttag::ITagAssignment)

QByteArray CTagAssignment::GetTagId() const
{
	return m_tagId;
}


void CTagAssignment::SetTagId(const QByteArray& tagId)
{
	if (m_tagId != tagId){
		istd::CChangeNotifier notifier(this);

		m_tagId = tagId;
	}
}


QByteArray CTagAssignment::GetEntityType() const
{
	return m_entityType;
}


void CTagAssignment::SetEntityType(const QByteArray& entityType)
{
	if (m_entityType != entityType){
		istd::CChangeNotifier notifier(this);

		m_entityType = entityType;
	}
}


QByteArray CTagAssignment::GetEntityId() const
{
	return m_entityId;
}


void CTagAssignment::SetEntityId(const QByteArray& entityId)
{
	if (m_entityId != entityId){
		istd::CChangeNotifier notifier(this);

		m_entityId = entityId;
	}
}


QDateTime CTagAssignment::GetCreatedAt() const
{
	return m_createdAt;
}


void CTagAssignment::SetCreatedAt(const QDateTime& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier notifier(this);

		m_createdAt = createdAt;
	}
}


QByteArray CTagAssignment::GetCreatedByUserId() const
{
	return m_createdByUserId;
}


void CTagAssignment::SetCreatedByUserId(const QByteArray& userId)
{
	if (m_createdByUserId != userId){
		istd::CChangeNotifier notifier(this);

		m_createdByUserId = userId;
	}
}


// reimplemented (iser::ISerializable)

bool CTagAssignment::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag tagIdTag("TagId", "Tag ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tagIdTag);
	retVal = retVal && archive.Process(m_tagId);
	retVal = retVal && archive.EndTag(tagIdTag);

	static iser::CArchiveTag entityTypeTag("EntityType", "Entity type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(entityTypeTag);
	retVal = retVal && archive.Process(m_entityType);
	retVal = retVal && archive.EndTag(entityTypeTag);

	static iser::CArchiveTag entityIdTag("EntityId", "Entity ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(entityIdTag);
	retVal = retVal && archive.Process(m_entityId);
	retVal = retVal && archive.EndTag(entityIdTag);

	QString createdAt = m_createdAt.toString(Qt::ISODateWithMs);
	static iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);
	if (!archive.IsStoring()){
		m_createdAt = QDateTime::fromString(createdAt, Qt::ISODateWithMs);
	}

	static iser::CArchiveTag createdByTag("CreatedByUserId", "Created by user ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdByTag);
	retVal = retVal && archive.Process(m_createdByUserId);
	retVal = retVal && archive.EndTag(createdByTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CTagAssignment::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_RESET;
}


bool CTagAssignment::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const ITagAssignment* sourcePtr = dynamic_cast<const ITagAssignment*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_tagId = sourcePtr->GetTagId();
	m_entityType = sourcePtr->GetEntityType();
	m_entityId = sourcePtr->GetEntityId();
	m_createdAt = sourcePtr->GetCreatedAt();
	m_createdByUserId = sourcePtr->GetCreatedByUserId();

	return true;
}


istd::IChangeableUniquePtr CTagAssignment::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTagAssignment);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTagAssignment::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_tagId.clear();
	m_entityType.clear();
	m_entityId.clear();
	m_createdAt = QDateTime();
	m_createdByUserId.clear();

	return true;
}


} // namespace imttag


