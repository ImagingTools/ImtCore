// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttag/CTagEvent.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imttag
{


// public methods

CTagEvent::CTagEvent()
	:m_action(A_TAGGED),
	m_entityRevision(-1)
{
}


QByteArray CTagEvent::ToActionId(Action action)
{
	return (action == A_UNTAGGED) ? QByteArrayLiteral("Untagged") : QByteArrayLiteral("Tagged");
}


ITagEvent::Action CTagEvent::FromActionId(const QByteArray& actionId)
{
	return (actionId == "Untagged") ? A_UNTAGGED : A_TAGGED;
}


// reimplemented (imttag::ITagEvent)

ITagEvent::Action CTagEvent::GetAction() const
{
	return m_action;
}


void CTagEvent::SetAction(Action action)
{
	if (m_action != action){
		istd::CChangeNotifier notifier(this);

		m_action = action;
	}
}


QByteArray CTagEvent::GetTagId() const
{
	return m_tagId;
}


void CTagEvent::SetTagId(const QByteArray& tagId)
{
	if (m_tagId != tagId){
		istd::CChangeNotifier notifier(this);

		m_tagId = tagId;
	}
}


QString CTagEvent::GetTagName() const
{
	return m_tagName;
}


void CTagEvent::SetTagName(const QString& tagName)
{
	if (m_tagName != tagName){
		istd::CChangeNotifier notifier(this);

		m_tagName = tagName;
	}
}


QString CTagEvent::GetTagColor() const
{
	return m_tagColor;
}


void CTagEvent::SetTagColor(const QString& tagColor)
{
	if (m_tagColor != tagColor){
		istd::CChangeNotifier notifier(this);

		m_tagColor = tagColor;
	}
}


QByteArray CTagEvent::GetEntityType() const
{
	return m_entityType;
}


void CTagEvent::SetEntityType(const QByteArray& entityType)
{
	if (m_entityType != entityType){
		istd::CChangeNotifier notifier(this);

		m_entityType = entityType;
	}
}


QByteArray CTagEvent::GetEntityId() const
{
	return m_entityId;
}


void CTagEvent::SetEntityId(const QByteArray& entityId)
{
	if (m_entityId != entityId){
		istd::CChangeNotifier notifier(this);

		m_entityId = entityId;
	}
}


qlonglong CTagEvent::GetEntityRevision() const
{
	return m_entityRevision;
}


void CTagEvent::SetEntityRevision(qlonglong revision)
{
	if (m_entityRevision != revision){
		istd::CChangeNotifier notifier(this);

		m_entityRevision = revision;
	}
}


QByteArray CTagEvent::GetActorId() const
{
	return m_actorId;
}


void CTagEvent::SetActorId(const QByteArray& actorId)
{
	if (m_actorId != actorId){
		istd::CChangeNotifier notifier(this);

		m_actorId = actorId;
	}
}


QString CTagEvent::GetActorName() const
{
	return m_actorName;
}


void CTagEvent::SetActorName(const QString& actorName)
{
	if (m_actorName != actorName){
		istd::CChangeNotifier notifier(this);

		m_actorName = actorName;
	}
}


QByteArray CTagEvent::GetTenantId() const
{
	return m_tenantId;
}


void CTagEvent::SetTenantId(const QByteArray& tenantId)
{
	if (m_tenantId != tenantId){
		istd::CChangeNotifier notifier(this);

		m_tenantId = tenantId;
	}
}


QDateTime CTagEvent::GetTimestamp() const
{
	return m_timestamp;
}


void CTagEvent::SetTimestamp(const QDateTime& timestamp)
{
	if (m_timestamp != timestamp){
		istd::CChangeNotifier notifier(this);

		m_timestamp = timestamp;
	}
}


// reimplemented (iser::ISerializable)

bool CTagEvent::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	QByteArray actionId = ToActionId(m_action);
	static iser::CArchiveTag actionTag("Action", "Action", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(actionTag);
	retVal = retVal && archive.Process(actionId);
	retVal = retVal && archive.EndTag(actionTag);
	if (!archive.IsStoring()){
		m_action = FromActionId(actionId);
	}

	static iser::CArchiveTag tagIdTag("TagId", "Tag ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tagIdTag);
	retVal = retVal && archive.Process(m_tagId);
	retVal = retVal && archive.EndTag(tagIdTag);

	static iser::CArchiveTag tagNameTag("TagName", "Tag name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tagNameTag);
	retVal = retVal && archive.Process(m_tagName);
	retVal = retVal && archive.EndTag(tagNameTag);

	static iser::CArchiveTag tagColorTag("TagColor", "Tag color", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tagColorTag);
	retVal = retVal && archive.Process(m_tagColor);
	retVal = retVal && archive.EndTag(tagColorTag);

	static iser::CArchiveTag entityTypeTag("EntityType", "Entity type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(entityTypeTag);
	retVal = retVal && archive.Process(m_entityType);
	retVal = retVal && archive.EndTag(entityTypeTag);

	static iser::CArchiveTag entityIdTag("EntityId", "Entity ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(entityIdTag);
	retVal = retVal && archive.Process(m_entityId);
	retVal = retVal && archive.EndTag(entityIdTag);

	qint64 entityRevision = m_entityRevision;
	static iser::CArchiveTag entityRevisionTag("EntityRevision", "Entity revision", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(entityRevisionTag);
	retVal = retVal && archive.Process(entityRevision);
	retVal = retVal && archive.EndTag(entityRevisionTag);
	if (!archive.IsStoring()){
		m_entityRevision = entityRevision;
	}

	static iser::CArchiveTag actorIdTag("ActorId", "Actor ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(actorIdTag);
	retVal = retVal && archive.Process(m_actorId);
	retVal = retVal && archive.EndTag(actorIdTag);

	static iser::CArchiveTag actorNameTag("ActorName", "Actor name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(actorNameTag);
	retVal = retVal && archive.Process(m_actorName);
	retVal = retVal && archive.EndTag(actorNameTag);

	static iser::CArchiveTag tenantIdTag("TenantId", "Tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tenantIdTag);
	retVal = retVal && archive.Process(m_tenantId);
	retVal = retVal && archive.EndTag(tenantIdTag);

	QString timestamp = m_timestamp.toString(Qt::ISODateWithMs);
	static iser::CArchiveTag timestampTag("Timestamp", "Timestamp", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(timestampTag);
	retVal = retVal && archive.Process(timestamp);
	retVal = retVal && archive.EndTag(timestampTag);
	if (!archive.IsStoring()){
		m_timestamp = QDateTime::fromString(timestamp, Qt::ISODateWithMs);
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

int CTagEvent::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_RESET;
}


bool CTagEvent::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const ITagEvent* sourcePtr = dynamic_cast<const ITagEvent*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_action = sourcePtr->GetAction();
	m_tagId = sourcePtr->GetTagId();
	m_tagName = sourcePtr->GetTagName();
	m_tagColor = sourcePtr->GetTagColor();
	m_entityType = sourcePtr->GetEntityType();
	m_entityId = sourcePtr->GetEntityId();
	m_entityRevision = sourcePtr->GetEntityRevision();
	m_actorId = sourcePtr->GetActorId();
	m_actorName = sourcePtr->GetActorName();
	m_tenantId = sourcePtr->GetTenantId();
	m_timestamp = sourcePtr->GetTimestamp();

	return true;
}


istd::IChangeableUniquePtr CTagEvent::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTagEvent);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTagEvent::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_action = A_TAGGED;
	m_tagId.clear();
	m_tagName.clear();
	m_tagColor.clear();
	m_entityType.clear();
	m_entityId.clear();
	m_entityRevision = -1;
	m_actorId.clear();
	m_actorName.clear();
	m_tenantId.clear();
	m_timestamp = QDateTime();

	return true;
}


} // namespace imttag


