// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantRelationshipInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


CTenantRelationshipInfo::CTenantRelationshipInfo():
	m_sourceRole(TRR_PARTNER),
	m_targetRole(TRR_PARTNER),
	m_status(TRS_ACTIVE)
{
}


#define IMT_REL_GET_SET(TYPE, NAME, MEMBER) \
TYPE CTenantRelationshipInfo::Get##NAME() const { return MEMBER; } \
void CTenantRelationshipInfo::Set##NAME(const TYPE& value) { if (MEMBER != value){ istd::CChangeNotifier notifier(this); MEMBER = value; } }

IMT_REL_GET_SET(QByteArray, RelationshipId, m_relationshipId)
IMT_REL_GET_SET(QByteArray, ConnectionId, m_connectionId)
IMT_REL_GET_SET(QByteArray, SourceTenantId, m_sourceTenantId)
IMT_REL_GET_SET(QByteArray, TargetTenantId, m_targetTenantId)
IMT_REL_GET_SET(QString, Scope, m_scope)
IMT_REL_GET_SET(QString, ValidFrom, m_validFrom)
IMT_REL_GET_SET(QString, ValidUntil, m_validUntil)
IMT_REL_GET_SET(QString, Description, m_description)
IMT_REL_GET_SET(QString, CreatedAt, m_createdAt)
IMT_REL_GET_SET(QString, UpdatedAt, m_updatedAt)

#undef IMT_REL_GET_SET


ITenantRelationshipInfo::TenantRelationshipRole CTenantRelationshipInfo::GetSourceRole() const
{
	return m_sourceRole;
}


void CTenantRelationshipInfo::SetSourceRole(TenantRelationshipRole role)
{
	if (m_sourceRole != role){
		istd::CChangeNotifier notifier(this);
		m_sourceRole = role;
	}
}


ITenantRelationshipInfo::TenantRelationshipRole CTenantRelationshipInfo::GetTargetRole() const
{
	return m_targetRole;
}


void CTenantRelationshipInfo::SetTargetRole(TenantRelationshipRole role)
{
	if (m_targetRole != role){
		istd::CChangeNotifier notifier(this);
		m_targetRole = role;
	}
}


ITenantRelationshipInfo::TenantRelationshipStatus CTenantRelationshipInfo::GetStatus() const
{
	return m_status;
}


void CTenantRelationshipInfo::SetStatus(TenantRelationshipStatus status)
{
	if (m_status != status){
		istd::CChangeNotifier notifier(this);
		m_status = status;
	}
}


bool CTenantRelationshipInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Relationship ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_relationshipId);
	retVal = retVal && archive.EndTag(idTag);

	iser::CArchiveTag connectionIdTag("ConnectionId", "Connection ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(connectionIdTag);
	retVal = retVal && archive.Process(m_connectionId);
	retVal = retVal && archive.EndTag(connectionIdTag);

	iser::CArchiveTag sourceTag("SourceTenantId", "Source tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sourceTag);
	retVal = retVal && archive.Process(m_sourceTenantId);
	retVal = retVal && archive.EndTag(sourceTag);

	iser::CArchiveTag targetTag("TargetTenantId", "Target tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetTag);
	retVal = retVal && archive.Process(m_targetTenantId);
	retVal = retVal && archive.EndTag(targetTag);

	iser::CArchiveTag sourceRoleTag("SourceRole", "Source role", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sourceRoleTag);
	retVal = retVal && I_SERIALIZE_ENUM(TenantRelationshipRole, archive, m_sourceRole);
	retVal = retVal && archive.EndTag(sourceRoleTag);

	iser::CArchiveTag targetRoleTag("TargetRole", "Target role", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetRoleTag);
	retVal = retVal && I_SERIALIZE_ENUM(TenantRelationshipRole, archive, m_targetRole);
	retVal = retVal && archive.EndTag(targetRoleTag);

	iser::CArchiveTag scopeTag("Scope", "Scope", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(scopeTag);
	retVal = retVal && archive.Process(m_scope);
	retVal = retVal && archive.EndTag(scopeTag);

	iser::CArchiveTag validFromTag("ValidFrom", "Valid from", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(validFromTag);
	retVal = retVal && archive.Process(m_validFrom);
	retVal = retVal && archive.EndTag(validFromTag);

	iser::CArchiveTag validUntilTag("ValidUntil", "Valid until", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(validUntilTag);
	retVal = retVal && archive.Process(m_validUntil);
	retVal = retVal && archive.EndTag(validUntilTag);

	iser::CArchiveTag statusTag("Status", "Status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	retVal = retVal && I_SERIALIZE_ENUM(TenantRelationshipStatus, archive, m_status);
	retVal = retVal && archive.EndTag(statusTag);

	iser::CArchiveTag descTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && archive.Process(m_updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	return retVal;
}


bool CTenantRelationshipInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTenantRelationshipInfo* sourcePtr = dynamic_cast<const CTenantRelationshipInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_relationshipId = sourcePtr->m_relationshipId;
	m_connectionId = sourcePtr->m_connectionId;
	m_sourceTenantId = sourcePtr->m_sourceTenantId;
	m_targetTenantId = sourcePtr->m_targetTenantId;
	m_sourceRole = sourcePtr->m_sourceRole;
	m_targetRole = sourcePtr->m_targetRole;
	m_scope = sourcePtr->m_scope;
	m_validFrom = sourcePtr->m_validFrom;
	m_validUntil = sourcePtr->m_validUntil;
	m_status = sourcePtr->m_status;
	m_description = sourcePtr->m_description;
	m_createdAt = sourcePtr->m_createdAt;
	m_updatedAt = sourcePtr->m_updatedAt;

	return true;
}


istd::IChangeableUniquePtr CTenantRelationshipInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTenantRelationshipInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTenantRelationshipInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_relationshipId.clear();
	m_connectionId.clear();
	m_sourceTenantId.clear();
	m_targetTenantId.clear();
	m_sourceRole = TRR_PARTNER;
	m_targetRole = TRR_PARTNER;
	m_scope.clear();
	m_validFrom.clear();
	m_validUntil.clear();
	m_status = TRS_ACTIVE;
	m_description.clear();
	m_createdAt.clear();
	m_updatedAt.clear();

	return true;
}


} // namespace imtauth
