// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantMembership.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

CTenantMembership::CTenantMembership():
	m_role(TMR_MEMBER),
	m_isActive(true)
{
}


// reimplemented (ITenantMembership)

QByteArray CTenantMembership::GetMembershipId() const
{
	return m_membershipId;
}


void CTenantMembership::SetMembershipId(const QByteArray& membershipId)
{
	if (m_membershipId != membershipId){
		istd::CChangeNotifier changeNotifier(this);

		m_membershipId = membershipId;
	}
}


QByteArray CTenantMembership::GetUserId() const
{
	return m_userId;
}


void CTenantMembership::SetUserId(const QByteArray& userId)
{
	if (m_userId != userId){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = userId;
	}
}


QByteArray CTenantMembership::GetTenantId() const
{
	return m_tenantId;
}


void CTenantMembership::SetTenantId(const QByteArray& tenantId)
{
	if (m_tenantId != tenantId){
		istd::CChangeNotifier changeNotifier(this);

		m_tenantId = tenantId;
	}
}


ITenantMembership::TenantMemberRole CTenantMembership::GetRole() const
{
	return m_role;
}


void CTenantMembership::SetRole(TenantMemberRole role)
{
	if (m_role != role){
		istd::CChangeNotifier changeNotifier(this);

		m_role = role;
	}
}


QString CTenantMembership::GetJoinedAt() const
{
	return m_joinedAt;
}


void CTenantMembership::SetJoinedAt(const QString& joinedAt)
{
	if (m_joinedAt != joinedAt){
		istd::CChangeNotifier notifier(this);

		m_joinedAt = joinedAt;
	}
}


bool CTenantMembership::IsActive() const
{
	return m_isActive;
}


void CTenantMembership::SetActive(bool isActive)
{
	if (m_isActive != isActive){
		istd::CChangeNotifier notifier(this);

		m_isActive = isActive;
	}
}


QString CTenantMembership::GetUpdatedAt() const
{
	return m_updatedAt;
}


void CTenantMembership::SetUpdatedAt(const QString& updatedAt)
{
	if (m_updatedAt != updatedAt){
		istd::CChangeNotifier notifier(this);

		m_updatedAt = updatedAt;
	}
}


// reimplemented (iser::ISerializable)

bool CTenantMembership::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_membershipId);
	retVal = retVal && archive.EndTag(idTag);

	iser::CArchiveTag userIdTag("UserId", "User ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	iser::CArchiveTag tenantIdTag("TenantId", "Tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tenantIdTag);
	retVal = retVal && archive.Process(m_tenantId);
	retVal = retVal && archive.EndTag(tenantIdTag);

	iser::CArchiveTag roleTag("Role", "Role", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(roleTag);
	retVal = retVal && I_SERIALIZE_ENUM(TenantMemberRole, archive, m_role);
	retVal = retVal && archive.EndTag(roleTag);

	iser::CArchiveTag joinedAtTag("JoinedAt", "Joined at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(joinedAtTag);
	retVal = retVal && archive.Process(m_joinedAt);
	retVal = retVal && archive.EndTag(joinedAtTag);

	iser::CArchiveTag isActiveTag("IsActive", "Is active", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isActiveTag);
	retVal = retVal && archive.Process(m_isActive);
	retVal = retVal && archive.EndTag(isActiveTag);

	iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && archive.Process(m_updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CTenantMembership::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTenantMembership* sourcePtr = dynamic_cast<const CTenantMembership*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier notifier(this);

		m_membershipId = sourcePtr->m_membershipId;
		m_userId = sourcePtr->m_userId;
		m_tenantId = sourcePtr->m_tenantId;
		m_role = sourcePtr->m_role;
		m_joinedAt = sourcePtr->m_joinedAt;
		m_updatedAt = sourcePtr->m_updatedAt;
		m_isActive = sourcePtr->m_isActive;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CTenantMembership::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTenantMembership);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTenantMembership::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_membershipId.clear();
	m_userId.clear();
	m_tenantId.clear();
	m_role = TMR_MEMBER;
	m_joinedAt.clear();
	m_updatedAt.clear();
	m_isActive = true;

	return true;
}


} // namespace imtauth
