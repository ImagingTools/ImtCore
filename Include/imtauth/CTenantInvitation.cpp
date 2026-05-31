// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantInvitation.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


CTenantInvitation::CTenantInvitation():
	m_status(TIS_PENDING)
{
}


#define IMT_TENANT_INVITATION_GET_SET(TYPE, NAME, MEMBER) \
TYPE CTenantInvitation::Get##NAME() const { return MEMBER; } \
void CTenantInvitation::Set##NAME(const TYPE& value) { if (MEMBER != value){ istd::CChangeNotifier notifier(this); MEMBER = value; } }

IMT_TENANT_INVITATION_GET_SET(QByteArray, InvitationId, m_invitationId)
IMT_TENANT_INVITATION_GET_SET(QByteArray, UserId, m_userId)
IMT_TENANT_INVITATION_GET_SET(QByteArray, TenantId, m_tenantId)
IMT_TENANT_INVITATION_GET_SET(QByteArray, InvitedByUserId, m_invitedByUserId)
IMT_TENANT_INVITATION_GET_SET(QString, CreatedAt, m_createdAt)
IMT_TENANT_INVITATION_GET_SET(QString, UpdatedAt, m_updatedAt)
IMT_TENANT_INVITATION_GET_SET(QString, ExpiresAt, m_expiresAt)
IMT_TENANT_INVITATION_GET_SET(QString, AcceptedAt, m_acceptedAt)
IMT_TENANT_INVITATION_GET_SET(QString, RejectedAt, m_rejectedAt)
IMT_TENANT_INVITATION_GET_SET(QString, RevokedAt, m_revokedAt)
IMT_TENANT_INVITATION_GET_SET(QByteArray, RevokedByUserId, m_revokedByUserId)
IMT_TENANT_INVITATION_GET_SET(QString, LastSentAt, m_lastSentAt)

#undef IMT_TENANT_INVITATION_GET_SET


QByteArray CTenantInvitation::GetRoleId() const
{
	return m_roleId;
}


void CTenantInvitation::SetRoleId(const QByteArray& roleId)
{
	if (m_roleId != roleId){
		istd::CChangeNotifier notifier(this);
		m_roleId = roleId;
	}
}


ITenantInvitation::TenantInvitationStatus CTenantInvitation::GetStatus() const
{
	return m_status;
}


void CTenantInvitation::SetStatus(TenantInvitationStatus status)
{
	if (m_status != status){
		istd::CChangeNotifier notifier(this);
		m_status = status;
	}
}


bool CTenantInvitation::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_invitationId);
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
	retVal = retVal && archive.Process(m_roleId);
	retVal = retVal && archive.EndTag(roleTag);

	iser::CArchiveTag statusTag("Status", "Status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	retVal = retVal && I_SERIALIZE_ENUM(TenantInvitationStatus, archive, m_status);
	retVal = retVal && archive.EndTag(statusTag);

	iser::CArchiveTag invitedByTag("InvitedByUserId", "Invited by user ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(invitedByTag);
	retVal = retVal && archive.Process(m_invitedByUserId);
	retVal = retVal && archive.EndTag(invitedByTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && archive.Process(m_updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	iser::CArchiveTag expiresAtTag("ExpiresAt", "Expires at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(expiresAtTag);
	retVal = retVal && archive.Process(m_expiresAt);
	retVal = retVal && archive.EndTag(expiresAtTag);

	iser::CArchiveTag acceptedAtTag("AcceptedAt", "Accepted at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(acceptedAtTag);
	retVal = retVal && archive.Process(m_acceptedAt);
	retVal = retVal && archive.EndTag(acceptedAtTag);

	iser::CArchiveTag rejectedAtTag("RejectedAt", "Rejected at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(rejectedAtTag);
	retVal = retVal && archive.Process(m_rejectedAt);
	retVal = retVal && archive.EndTag(rejectedAtTag);

	iser::CArchiveTag revokedAtTag("RevokedAt", "Revoked at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(revokedAtTag);
	retVal = retVal && archive.Process(m_revokedAt);
	retVal = retVal && archive.EndTag(revokedAtTag);

	iser::CArchiveTag revokedByTag("RevokedByUserId", "Revoked by user ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(revokedByTag);
	retVal = retVal && archive.Process(m_revokedByUserId);
	retVal = retVal && archive.EndTag(revokedByTag);

	iser::CArchiveTag lastSentAtTag("LastSentAt", "Last sent at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(lastSentAtTag);
	retVal = retVal && archive.Process(m_lastSentAt);
	retVal = retVal && archive.EndTag(lastSentAtTag);

	return retVal;
}


bool CTenantInvitation::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTenantInvitation* sourcePtr = dynamic_cast<const CTenantInvitation*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_invitationId = sourcePtr->m_invitationId;
	m_userId = sourcePtr->m_userId;
	m_tenantId = sourcePtr->m_tenantId;
	m_roleId = sourcePtr->m_roleId;
	m_status = sourcePtr->m_status;
	m_invitedByUserId = sourcePtr->m_invitedByUserId;
	m_createdAt = sourcePtr->m_createdAt;
	m_updatedAt = sourcePtr->m_updatedAt;
	m_expiresAt = sourcePtr->m_expiresAt;
	m_acceptedAt = sourcePtr->m_acceptedAt;
	m_rejectedAt = sourcePtr->m_rejectedAt;
	m_revokedAt = sourcePtr->m_revokedAt;
	m_revokedByUserId = sourcePtr->m_revokedByUserId;
	m_lastSentAt = sourcePtr->m_lastSentAt;

	return true;
}


istd::IChangeableUniquePtr CTenantInvitation::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTenantInvitation);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CTenantInvitation::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_invitationId.clear();
	m_userId.clear();
	m_tenantId.clear();
	m_roleId.clear();
	m_status = TIS_PENDING;
	m_invitedByUserId.clear();
	m_createdAt.clear();
	m_updatedAt.clear();
	m_expiresAt.clear();
	m_acceptedAt.clear();
	m_rejectedAt.clear();
	m_revokedAt.clear();
	m_revokedByUserId.clear();
	m_lastSentAt.clear();

	return true;
}


} // namespace imtauth
