// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CDocumentShareInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtauth
{


namespace
{


template <typename Value>
bool SerializeLeaf(iser::IArchive& archive, const QByteArray& id, Value& value)
{
	iser::CArchiveTag tag(id, id, iser::CArchiveTag::TT_LEAF);

	bool retVal = archive.BeginTag(tag);
	retVal = retVal && archive.Process(value);
	retVal = retVal && archive.EndTag(tag);

	return retVal;
}


template <typename Value>
void SetValue(istd::IChangeable& object, Value& member, const Value& value)
{
	if (member != value){
		istd::CChangeNotifier notifier(&object);

		member = value;
	}
}


} // anonymous namespace


CDocumentShareInfo::CDocumentShareInfo()
	:m_accessLevel(DAL_VIEW),
	m_status(DSS_PENDING)
{
}


// reimplemented (imtauth::IDocumentShareInfo)

QByteArray CDocumentShareInfo::GetShareId() const
{
	return m_shareId;
}


void CDocumentShareInfo::SetShareId(const QByteArray& shareId)
{
	SetValue(*this, m_shareId, shareId);
}


QByteArray CDocumentShareInfo::GetCollectionId() const
{
	return m_collectionId;
}


void CDocumentShareInfo::SetCollectionId(const QByteArray& collectionId)
{
	SetValue(*this, m_collectionId, collectionId);
}


QByteArray CDocumentShareInfo::GetEntityId() const
{
	return m_entityId;
}


void CDocumentShareInfo::SetEntityId(const QByteArray& entityId)
{
	SetValue(*this, m_entityId, entityId);
}


QString CDocumentShareInfo::GetEntityName() const
{
	return m_entityName;
}


void CDocumentShareInfo::SetEntityName(const QString& entityName)
{
	SetValue(*this, m_entityName, entityName);
}


QByteArray CDocumentShareInfo::GetOwnerTenantId() const
{
	return m_ownerTenantId;
}


void CDocumentShareInfo::SetOwnerTenantId(const QByteArray& tenantId)
{
	SetValue(*this, m_ownerTenantId, tenantId);
}


QString CDocumentShareInfo::GetOwnerTenantName() const
{
	return m_ownerTenantName;
}


void CDocumentShareInfo::SetOwnerTenantName(const QString& tenantName)
{
	SetValue(*this, m_ownerTenantName, tenantName);
}


QByteArray CDocumentShareInfo::GetTargetTenantId() const
{
	return m_targetTenantId;
}


void CDocumentShareInfo::SetTargetTenantId(const QByteArray& tenantId)
{
	SetValue(*this, m_targetTenantId, tenantId);
}


QString CDocumentShareInfo::GetTargetTenantName() const
{
	return m_targetTenantName;
}


void CDocumentShareInfo::SetTargetTenantName(const QString& tenantName)
{
	SetValue(*this, m_targetTenantName, tenantName);
}


QByteArrayList CDocumentShareInfo::GetTargetGroupIds() const
{
	return m_targetGroupIds;
}


void CDocumentShareInfo::SetTargetGroupIds(const QByteArrayList& groupIds)
{
	QByteArrayList nonEmptyIds = groupIds;
	nonEmptyIds.removeAll(QByteArray());

	SetValue(*this, m_targetGroupIds, nonEmptyIds);
}


IDocumentShareInfo::AccessLevel CDocumentShareInfo::GetAccessLevel() const
{
	return m_accessLevel;
}


void CDocumentShareInfo::SetAccessLevel(AccessLevel accessLevel)
{
	SetValue(*this, m_accessLevel, accessLevel);
}


IDocumentShareInfo::ShareStatus CDocumentShareInfo::GetStatus() const
{
	return m_status;
}


void CDocumentShareInfo::SetStatus(ShareStatus status)
{
	SetValue(*this, m_status, status);
}


QDateTime CDocumentShareInfo::GetValidUntil() const
{
	return m_validUntil;
}


void CDocumentShareInfo::SetValidUntil(const QDateTime& validUntil)
{
	SetValue(*this, m_validUntil, validUntil);
}


QString CDocumentShareInfo::GetMessage() const
{
	return m_message;
}


void CDocumentShareInfo::SetMessage(const QString& message)
{
	SetValue(*this, m_message, message);
}


QByteArray CDocumentShareInfo::GetCreatedByUserId() const
{
	return m_createdByUserId;
}


void CDocumentShareInfo::SetCreatedByUserId(const QByteArray& userId)
{
	SetValue(*this, m_createdByUserId, userId);
}


QDateTime CDocumentShareInfo::GetCreatedAt() const
{
	return m_createdAt;
}


void CDocumentShareInfo::SetCreatedAt(const QDateTime& createdAt)
{
	SetValue(*this, m_createdAt, createdAt);
}


QDateTime CDocumentShareInfo::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CDocumentShareInfo::SetUpdatedAt(const QDateTime& updatedAt)
{
	SetValue(*this, m_updatedAt, updatedAt);
}


// reimplemented (iser::ISerializable)

bool CDocumentShareInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	QByteArray targetGroupIds = m_targetGroupIds.join(';');
	int accessLevel = m_accessLevel;
	int status = m_status;
	QString validUntil = m_validUntil.toString(Qt::ISODateWithMs);
	QString createdAt = m_createdAt.toString(Qt::ISODateWithMs);
	QString updatedAt = m_updatedAt.toString(Qt::ISODateWithMs);

	bool retVal = true;
	retVal = retVal && SerializeLeaf(archive, "Id", m_shareId);
	retVal = retVal && SerializeLeaf(archive, "CollectionId", m_collectionId);
	retVal = retVal && SerializeLeaf(archive, "EntityId", m_entityId);
	retVal = retVal && SerializeLeaf(archive, "EntityName", m_entityName);
	retVal = retVal && SerializeLeaf(archive, "OwnerTenantId", m_ownerTenantId);
	retVal = retVal && SerializeLeaf(archive, "OwnerTenantName", m_ownerTenantName);
	retVal = retVal && SerializeLeaf(archive, "TargetTenantId", m_targetTenantId);
	retVal = retVal && SerializeLeaf(archive, "TargetTenantName", m_targetTenantName);
	retVal = retVal && SerializeLeaf(archive, "TargetGroupIds", targetGroupIds);
	retVal = retVal && SerializeLeaf(archive, "AccessLevel", accessLevel);
	retVal = retVal && SerializeLeaf(archive, "Status", status);
	retVal = retVal && SerializeLeaf(archive, "ValidUntil", validUntil);
	retVal = retVal && SerializeLeaf(archive, "Message", m_message);
	retVal = retVal && SerializeLeaf(archive, "CreatedByUserId", m_createdByUserId);
	retVal = retVal && SerializeLeaf(archive, "CreatedAt", createdAt);
	retVal = retVal && SerializeLeaf(archive, "UpdatedAt", updatedAt);

	if (!archive.IsStoring()){
		m_targetGroupIds = targetGroupIds.split(';');
		m_targetGroupIds.removeAll(QByteArray());
		m_accessLevel = static_cast<AccessLevel>(accessLevel);
		m_status = static_cast<ShareStatus>(status);
		m_validUntil = QDateTime::fromString(validUntil, Qt::ISODateWithMs);
		m_createdAt = QDateTime::fromString(createdAt, Qt::ISODateWithMs);
		m_updatedAt = QDateTime::fromString(updatedAt, Qt::ISODateWithMs);
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CDocumentShareInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IDocumentShareInfo* sourcePtr = dynamic_cast<const IDocumentShareInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_shareId = sourcePtr->GetShareId();
	m_collectionId = sourcePtr->GetCollectionId();
	m_entityId = sourcePtr->GetEntityId();
	m_entityName = sourcePtr->GetEntityName();
	m_ownerTenantId = sourcePtr->GetOwnerTenantId();
	m_ownerTenantName = sourcePtr->GetOwnerTenantName();
	m_targetTenantId = sourcePtr->GetTargetTenantId();
	m_targetTenantName = sourcePtr->GetTargetTenantName();
	m_targetGroupIds = sourcePtr->GetTargetGroupIds();
	m_accessLevel = sourcePtr->GetAccessLevel();
	m_status = sourcePtr->GetStatus();
	m_validUntil = sourcePtr->GetValidUntil();
	m_message = sourcePtr->GetMessage();
	m_createdByUserId = sourcePtr->GetCreatedByUserId();
	m_createdAt = sourcePtr->GetCreatedAt();
	m_updatedAt = sourcePtr->GetUpdatedAt();

	return true;
}


bool CDocumentShareInfo::IsEqual(const IChangeable& object) const
{
	const IDocumentShareInfo* otherPtr = dynamic_cast<const IDocumentShareInfo*>(&object);
	if (otherPtr == nullptr){
		return false;
	}

	return m_shareId == otherPtr->GetShareId()
				&& m_collectionId == otherPtr->GetCollectionId()
				&& m_entityId == otherPtr->GetEntityId()
				&& m_entityName == otherPtr->GetEntityName()
				&& m_ownerTenantId == otherPtr->GetOwnerTenantId()
				&& m_ownerTenantName == otherPtr->GetOwnerTenantName()
				&& m_targetTenantId == otherPtr->GetTargetTenantId()
				&& m_targetTenantName == otherPtr->GetTargetTenantName()
				&& m_targetGroupIds == otherPtr->GetTargetGroupIds()
				&& m_accessLevel == otherPtr->GetAccessLevel()
				&& m_status == otherPtr->GetStatus()
				&& m_validUntil == otherPtr->GetValidUntil()
				&& m_message == otherPtr->GetMessage()
				&& m_createdByUserId == otherPtr->GetCreatedByUserId()
				&& m_createdAt == otherPtr->GetCreatedAt()
				&& m_updatedAt == otherPtr->GetUpdatedAt();
}


istd::IChangeableUniquePtr CDocumentShareInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CDocumentShareInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CDocumentShareInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_shareId.clear();
	m_collectionId.clear();
	m_entityId.clear();
	m_entityName.clear();
	m_ownerTenantId.clear();
	m_ownerTenantName.clear();
	m_targetTenantId.clear();
	m_targetTenantName.clear();
	m_targetGroupIds.clear();
	m_accessLevel = DAL_VIEW;
	m_status = DSS_PENDING;
	m_validUntil = QDateTime();
	m_message.clear();
	m_createdByUserId.clear();
	m_createdAt = QDateTime();
	m_updatedAt = QDateTime();

	return true;
}


} // namespace imtauth


