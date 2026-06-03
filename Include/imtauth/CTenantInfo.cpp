// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

CTenantInfo::CTenantInfo():
	m_isActive(true),
	m_depth(0)
{
}


// reimplemented (ITenantInfo)

QByteArray CTenantInfo::GetTenantId() const
{
	return m_tenantId;
}


void CTenantInfo::SetTenantId(const QByteArray& tenantId)
{
	if (m_tenantId != tenantId){
		istd::CChangeNotifier changeNotifier(this);

		m_tenantId = tenantId;
	}
}


QString CTenantInfo::GetTenantName() const
{
	return m_name;
}


void CTenantInfo::SetTenantName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier changeNotifier(this);

		m_name = name;
	}
}


QString CTenantInfo::GetTenantDescription() const
{
	return m_description;
}


void CTenantInfo::SetTenantDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNotifier(this);

		m_description = description;
	}
}


QByteArray CTenantInfo::GetOwnerId() const
{
	return m_ownerId;
}


void CTenantInfo::SetOwnerId(const QByteArray& ownerId)
{
	if (m_ownerId != ownerId){
		istd::CChangeNotifier changeNotifier(this);

		m_ownerId = ownerId;
	}
}


QByteArray CTenantInfo::GetCreatorId() const
{
	return m_creatorId;
}


void CTenantInfo::SetCreatorId(const QByteArray& creatorId)
{
	if (m_creatorId != creatorId){
		istd::CChangeNotifier changeNotifier(this);

		m_creatorId = creatorId;
	}
}


bool CTenantInfo::IsActive() const
{
	return m_isActive;
}


void CTenantInfo::SetActive(bool isActive)
{
	if (m_isActive != isActive){
		istd::CChangeNotifier notifier(this);

		m_isActive = isActive;
	}
}


QString CTenantInfo::GetCreatedAt() const
{
	return m_createdAt;
}


void CTenantInfo::SetCreatedAt(const QString& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier notifier(this);

		m_createdAt = createdAt;
	}
}


QString CTenantInfo::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CTenantInfo::SetUpdatedAt(const QString& updatedAt)
{
	if (m_updatedAt != updatedAt){
		istd::CChangeNotifier notifier(this);

		m_updatedAt = updatedAt;
	}
}


ITenantInfo::TenantRelationships CTenantInfo::GetRelationships() const
{
	return m_relationships;
}


void CTenantInfo::SetRelationships(const TenantRelationships& relationships)
{
	if (m_relationships != relationships){
		istd::CChangeNotifier notifier(this);

		m_relationships = relationships;
	}
}


void CTenantInfo::AddRelationship(const TenantRelationship& relationship)
{
	istd::CChangeNotifier notifier(this);

	m_relationships.append(relationship);
}


bool CTenantInfo::RemoveRelationship(const QByteArray& relationshipId)
{
	for (int i = 0; i < m_relationships.size(); ++i){
		if (m_relationships[i].relationshipId == relationshipId){
			istd::CChangeNotifier notifier(this);

			m_relationships.removeAt(i);

			return true;
		}
	}

	return false;
}


QByteArrayList CTenantInfo::GetTenantPermissions() const
{
	return m_tenantPermissions;
}


void CTenantInfo::SetTenantPermissions(const QByteArrayList& permissions)
{
	if (m_tenantPermissions != permissions){
		istd::CChangeNotifier notifier(this);

		m_tenantPermissions = permissions;
	}
}


QByteArray CTenantInfo::GetParentTenantId() const
{
	return m_parentTenantId;
}


void CTenantInfo::SetParentTenantId(const QByteArray& parentTenantId)
{
	if (m_parentTenantId != parentTenantId){
		istd::CChangeNotifier notifier(this);

		m_parentTenantId = parentTenantId;
	}
}


int CTenantInfo::GetDepth() const
{
	return m_depth;
}


void CTenantInfo::SetDepth(int depth)
{
	if (m_depth != depth){
		istd::CChangeNotifier notifier(this);

		m_depth = depth;
	}
}


QString CTenantInfo::GetMaterializedPath() const
{
	return m_materializedPath;
}


void CTenantInfo::SetMaterializedPath(const QString& path)
{
	if (m_materializedPath != path){
		istd::CChangeNotifier notifier(this);

		m_materializedPath = path;
	}
}


bool CTenantInfo::IsSystemTenant() const
{
	return m_tenantId == GetSystemTenantId();
}


// reimplemented (iser::ISerializable)

bool CTenantInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_tenantId);
	retVal = retVal && archive.EndTag(idTag);

	iser::CArchiveTag nameTag("Name", "Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	iser::CArchiveTag ownerIdTag("OwnerId", "Owner ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(ownerIdTag);
	retVal = retVal && archive.Process(m_ownerId);
	retVal = retVal && archive.EndTag(ownerIdTag);

	iser::CArchiveTag creatorIdTag("CreatorId", "Creator ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(creatorIdTag);
	retVal = retVal && archive.Process(m_creatorId);
	retVal = retVal && archive.EndTag(creatorIdTag);

	iser::CArchiveTag isActiveTag("IsActive", "Is active", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isActiveTag);
	retVal = retVal && archive.Process(m_isActive);
	retVal = retVal && archive.EndTag(isActiveTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && archive.Process(m_updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	iser::CArchiveTag relationshipsTag("Relationships", "Relationships", iser::CArchiveTag::TT_GROUP);
	if (archive.BeginTag(relationshipsTag)){
		int count = m_relationships.size();
		retVal = retVal && archive.Process(count);

		if (archive.IsStoring()){
			for (int i = 0; i < count; ++i){
				iser::CArchiveTag relTag("Relationship", "Relationship", iser::CArchiveTag::TT_GROUP);
				retVal = retVal && archive.BeginTag(relTag);

				QByteArray relId = m_relationships[i].relationshipId;
				QByteArray targetId = m_relationships[i].targetTenantId;
				int role = static_cast<int>(m_relationships[i].role);
				QString desc = m_relationships[i].description;
				QString created = m_relationships[i].createdAt;
				int sourceRole = static_cast<int>(m_relationships[i].sourceRole);
				int targetRole = static_cast<int>(m_relationships[i].targetRole);
				QString scope = m_relationships[i].scope;
				QString validFrom = m_relationships[i].validFrom;
				QString validUntil = m_relationships[i].validUntil;
				bool isActive = m_relationships[i].isActive;

				retVal = retVal && archive.Process(relId);
				retVal = retVal && archive.Process(targetId);
				retVal = retVal && archive.Process(role);
				retVal = retVal && archive.Process(desc);
				retVal = retVal && archive.Process(created);
				retVal = retVal && archive.Process(sourceRole);
				retVal = retVal && archive.Process(targetRole);
				retVal = retVal && archive.Process(scope);
				retVal = retVal && archive.Process(validFrom);
				retVal = retVal && archive.Process(validUntil);
				retVal = retVal && archive.Process(isActive);

				retVal = retVal && archive.EndTag(relTag);
			}
		}
		else{
			m_relationships.clear();
			for (int i = 0; i < count; ++i){
				iser::CArchiveTag relTag("Relationship", "Relationship", iser::CArchiveTag::TT_GROUP);
				retVal = retVal && archive.BeginTag(relTag);

				TenantRelationship rel;
				int role = 0;
				int sourceRole = 0;
				int targetRole = 0;

				retVal = retVal && archive.Process(rel.relationshipId);
				retVal = retVal && archive.Process(rel.targetTenantId);
				retVal = retVal && archive.Process(role);
				retVal = retVal && archive.Process(rel.description);
				retVal = retVal && archive.Process(rel.createdAt);
				retVal = retVal && archive.Process(sourceRole);
				retVal = retVal && archive.Process(targetRole);
				retVal = retVal && archive.Process(rel.scope);
				retVal = retVal && archive.Process(rel.validFrom);
				retVal = retVal && archive.Process(rel.validUntil);
				retVal = retVal && archive.Process(rel.isActive);

				rel.role = static_cast<TenantRelationshipRole>(role);
				rel.sourceRole = static_cast<TenantRelationshipRole>(sourceRole);
				rel.targetRole = static_cast<TenantRelationshipRole>(targetRole);
				m_relationships.append(rel);

				retVal = retVal && archive.EndTag(relTag);
			}
		}

		retVal = retVal && archive.EndTag(relationshipsTag);
	}

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_tenantPermissions, "TenantPermissions", "TenantPermission");

	iser::CArchiveTag parentTenantIdTag("ParentTenantId", "Parent Tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(parentTenantIdTag);
	retVal = retVal && archive.Process(m_parentTenantId);
	retVal = retVal && archive.EndTag(parentTenantIdTag);

	iser::CArchiveTag depthTag("Depth", "Depth", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(depthTag);
	retVal = retVal && archive.Process(m_depth);
	retVal = retVal && archive.EndTag(depthTag);

	iser::CArchiveTag materializedPathTag("MaterializedPath", "Materialized path", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(materializedPathTag);
	retVal = retVal && archive.Process(m_materializedPath);
	retVal = retVal && archive.EndTag(materializedPathTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CTenantInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTenantInfo* sourcePtr = dynamic_cast<const CTenantInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier notifier(this);

		m_tenantId = sourcePtr->m_tenantId;
		m_name = sourcePtr->m_name;
		m_description = sourcePtr->m_description;
		m_ownerId = sourcePtr->m_ownerId;
		m_creatorId = sourcePtr->m_creatorId;
		m_isActive = sourcePtr->m_isActive;
		m_createdAt = sourcePtr->m_createdAt;
		m_updatedAt = sourcePtr->m_updatedAt;
		m_relationships = sourcePtr->m_relationships;
		m_tenantPermissions = sourcePtr->m_tenantPermissions;
		m_parentTenantId = sourcePtr->m_parentTenantId;
		m_depth = sourcePtr->m_depth;
		m_materializedPath = sourcePtr->m_materializedPath;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CTenantInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTenantInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTenantInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_tenantId.clear();
	m_name.clear();
	m_description.clear();
	m_ownerId.clear();
	m_creatorId.clear();
	m_isActive = true;
	m_createdAt.clear();
	m_updatedAt.clear();
	m_relationships.clear();
	m_tenantPermissions.clear();
	m_parentTenantId.clear();
	m_depth = 0;
	m_materializedPath.clear();

	return true;
}


} // namespace imtauth

