// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantEntityBinding.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtauth
{


// public methods

CTenantEntityBinding::CTenantEntityBinding()
{
}


// reimplemented (ITenantEntityBinding)

QByteArray CTenantEntityBinding::GetTenantId() const
{
	return m_tenantId;
}


void CTenantEntityBinding::SetTenantId(const QByteArray& tenantId)
{
	if (m_tenantId != tenantId){
		istd::CChangeNotifier changeNotifier(this);

		m_tenantId = tenantId;
	}
}


QByteArray CTenantEntityBinding::GetEntityType() const
{
	return m_entityType;
}


void CTenantEntityBinding::SetEntityType(const QByteArray& entityType)
{
	if (m_entityType != entityType){
		istd::CChangeNotifier changeNotifier(this);

		m_entityType = entityType;
	}
}


QByteArray CTenantEntityBinding::GetEntityId() const
{
	return m_entityId;
}


void CTenantEntityBinding::SetEntityId(const QByteArray& entityId)
{
	if (m_entityId != entityId){
		istd::CChangeNotifier changeNotifier(this);

		m_entityId = entityId;
	}
}


QString CTenantEntityBinding::GetCreatedAt() const
{
	return m_createdAt;
}


void CTenantEntityBinding::SetCreatedAt(const QString& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier changeNotifier(this);

		m_createdAt = createdAt;
	}
}


QByteArray CTenantEntityBinding::GetCreatedByUserId() const
{
	return m_createdByUserId;
}


void CTenantEntityBinding::SetCreatedByUserId(const QByteArray& userId)
{
	if (m_createdByUserId != userId){
		istd::CChangeNotifier changeNotifier(this);

		m_createdByUserId = userId;
	}
}


// reimplemented (iser::ISerializable)

bool CTenantEntityBinding::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag tenantIdTag("TenantId", "Tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tenantIdTag);
	retVal = retVal && archive.Process(m_tenantId);
	retVal = retVal && archive.EndTag(tenantIdTag);

	iser::CArchiveTag entityTypeTag("EntityType", "Entity Type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(entityTypeTag);
	retVal = retVal && archive.Process(m_entityType);
	retVal = retVal && archive.EndTag(entityTypeTag);

	iser::CArchiveTag entityIdTag("EntityId", "Entity ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(entityIdTag);
	retVal = retVal && archive.Process(m_entityId);
	retVal = retVal && archive.EndTag(entityIdTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	iser::CArchiveTag createdByTag("CreatedByUserId", "Created by user ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdByTag);
	retVal = retVal && archive.Process(m_createdByUserId);
	retVal = retVal && archive.EndTag(createdByTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CTenantEntityBinding::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTenantEntityBinding* sourcePtr = dynamic_cast<const CTenantEntityBinding*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier notifier(this);

		m_tenantId = sourcePtr->m_tenantId;
		m_entityType = sourcePtr->m_entityType;
		m_entityId = sourcePtr->m_entityId;
		m_createdAt = sourcePtr->m_createdAt;
		m_createdByUserId = sourcePtr->m_createdByUserId;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CTenantEntityBinding::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTenantEntityBinding);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTenantEntityBinding::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_tenantId.clear();
	m_entityType.clear();
	m_entityId.clear();
	m_createdAt.clear();
	m_createdByUserId.clear();

	return true;
}


} // namespace imtauth
