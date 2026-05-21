// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtCore/QMap>
#include <QtCore/QDateTime>
#include <QtTest/QtTest>

#include <memory>


namespace imtauth
{


enum TenantInvitationStatus
{
	TIS_PENDING = 0,
	TIS_ACCEPTED = 1,
	TIS_REJECTED = 2,
	TIS_REVOKED = 3,
	TIS_EXPIRED = 4
};


struct MockInvitation
{
	QByteArray id;
	QByteArray userId;
	QByteArray tenantId;
	QByteArray roleId;
	TenantInvitationStatus status = TIS_PENDING;
	QByteArray invitedByUserId;
	QDateTime createdAt;
	QDateTime updatedAt;
	QDateTime expiresAt;
	QDateTime acceptedAt;
	QDateTime rejectedAt;
	QDateTime revokedAt;
	QByteArray revokedByUserId;
	QDateTime lastSentAt;
};


/**
	Mock implementation of ITenantInvitationManager for unit testing.
	Simulates UNIQUE INDEX on (TenantId, UserId) WHERE Status=0 (pending).
*/
class CMockInvitationManager
{
public:
	QByteArrayList GetInvitationsByTenant(const QByteArray& tenantId, const QList<TenantInvitationStatus>& statuses = QList<TenantInvitationStatus>()) const
	{
		QByteArrayList result;
		for (auto it = m_invitations.constBegin(); it != m_invitations.constEnd(); ++it){
			if (it.value().tenantId == tenantId){
				if (statuses.isEmpty() || statuses.contains(it.value().status)){
					result.append(it.key());
				}
			}
		}
		return result;
	}

	QByteArrayList GetInvitationsByUser(const QByteArray& userId, const QList<TenantInvitationStatus>& statuses = QList<TenantInvitationStatus>()) const
	{
		QByteArrayList result;
		for (auto it = m_invitations.constBegin(); it != m_invitations.constEnd(); ++it){
			if (it.value().userId == userId){
				if (statuses.isEmpty() || statuses.contains(it.value().status)){
					result.append(it.key());
				}
			}
		}
		return result;
	}

	std::shared_ptr<MockInvitation> GetInvitation(const QByteArray& invitationId) const
	{
		if (m_invitations.contains(invitationId)){
			return std::make_shared<MockInvitation>(m_invitations[invitationId]);
		}
		return nullptr;
	}

	std::shared_ptr<MockInvitation> FindPendingInvitation(const QByteArray& userId, const QByteArray& tenantId) const
	{
		for (auto it = m_invitations.constBegin(); it != m_invitations.constEnd(); ++it){
			if (it.value().userId == userId && it.value().tenantId == tenantId && it.value().status == TIS_PENDING){
				return std::make_shared<MockInvitation>(it.value());
			}
		}
		return nullptr;
	}

	QByteArray CreateInvitation(const QByteArray& invitedByUserId, const QByteArray& userId, const QByteArray& tenantId, const QByteArray& roleId)
	{
		// UNIQUE constraint: only one pending invitation per user+tenant
		for (auto it = m_invitations.constBegin(); it != m_invitations.constEnd(); ++it){
			if (it.value().userId == userId && it.value().tenantId == tenantId && it.value().status == TIS_PENDING){
				return QByteArray(); // Duplicate pending
			}
		}

		MockInvitation inv;
		inv.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		inv.userId = userId;
		inv.tenantId = tenantId;
		inv.roleId = roleId;
		inv.status = TIS_PENDING;
		inv.invitedByUserId = invitedByUserId;
		inv.createdAt = QDateTime::currentDateTimeUtc();
		inv.updatedAt = inv.createdAt;
		inv.expiresAt = inv.createdAt.addDays(7);
		inv.lastSentAt = inv.createdAt;

		m_invitations[inv.id] = inv;
		return inv.id;
	}

	QByteArray AcceptInvitation(const QByteArray& invitationId, const QByteArray& userId)
	{
		if (!m_invitations.contains(invitationId)){
			return QByteArray();
		}

		MockInvitation& inv = m_invitations[invitationId];
		if (inv.userId != userId){
			return QByteArray(); // Wrong user
		}
		if (inv.status != TIS_PENDING){
			return QByteArray(); // Already processed
		}
		if (GetEffectiveStatus(inv) == TIS_EXPIRED){
			return QByteArray(); // Expired
		}

		inv.status = TIS_ACCEPTED;
		inv.acceptedAt = QDateTime::currentDateTimeUtc();
		inv.updatedAt = inv.acceptedAt;

		// Create membership (return mock membership ID)
		QByteArray membershipId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		return membershipId;
	}

	bool RejectInvitation(const QByteArray& invitationId, const QByteArray& userId)
	{
		if (!m_invitations.contains(invitationId)){
			return false;
		}

		MockInvitation& inv = m_invitations[invitationId];
		if (inv.userId != userId){
			return false;
		}
		if (inv.status != TIS_PENDING){
			return false;
		}

		inv.status = TIS_REJECTED;
		inv.rejectedAt = QDateTime::currentDateTimeUtc();
		inv.updatedAt = inv.rejectedAt;
		return true;
	}

	bool RevokeInvitation(const QByteArray& invitationId, const QByteArray& revokedByUserId)
	{
		if (!m_invitations.contains(invitationId)){
			return false;
		}

		MockInvitation& inv = m_invitations[invitationId];
		if (inv.status != TIS_PENDING){
			return false;
		}

		inv.status = TIS_REVOKED;
		inv.revokedAt = QDateTime::currentDateTimeUtc();
		inv.revokedByUserId = revokedByUserId;
		inv.updatedAt = inv.revokedAt;
		return true;
	}

	bool ResendInvitation(const QByteArray& invitationId)
	{
		if (!m_invitations.contains(invitationId)){
			return false;
		}

		MockInvitation& inv = m_invitations[invitationId];
		if (inv.status != TIS_PENDING){
			return false;
		}

		inv.lastSentAt = QDateTime::currentDateTimeUtc();
		inv.expiresAt = inv.lastSentAt.addDays(7);
		inv.updatedAt = inv.lastSentAt;
		return true;
	}

	TenantInvitationStatus GetEffectiveStatus(const MockInvitation& invitation) const
	{
		if (invitation.status == TIS_PENDING && invitation.expiresAt.isValid()){
			if (QDateTime::currentDateTimeUtc() > invitation.expiresAt){
				return TIS_EXPIRED;
			}
		}
		return invitation.status;
	}

	QMap<QByteArray, MockInvitation> m_invitations;
};


} // namespace imtauth


class CTenantInvitationManagerTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// Create invitation tests
	void testCreateInvitation_Success();
	void testCreateInvitation_DuplicatePending_Fails();
	void testCreateInvitation_AfterRejection_Allowed();

	// Accept invitation tests
	void testAcceptInvitation_Success_CreatesMembership();
	void testAcceptInvitation_WrongUser_Fails();
	void testAcceptInvitation_AlreadyAccepted_Fails();
	void testAcceptInvitation_Expired_Fails();

	// Reject/Revoke tests
	void testRejectInvitation_Success();
	void testRejectInvitation_WrongUser_Fails();
	void testRevokeInvitation_Success();

	// Resend tests
	void testResendInvitation_Success_UpdatesLastSentAt();
	void testResendInvitation_NonPending_Fails();

	// Status and query tests
	void testGetEffectiveStatus_Pending_NotExpired();
	void testGetEffectiveStatus_Pending_Expired();
	void testGetEffectiveStatus_Accepted();
	void testGetInvitationsByTenant_FilterByStatus();
	void testGetInvitationsByUser_AllStatuses();
	void testFindPendingInvitation_Exists();
	void testFindPendingInvitation_NotExists();

private:
	imtauth::CMockInvitationManager* m_managerPtr = nullptr;
	QByteArray m_tenantId;
	QByteArray m_userId;
	QByteArray m_invitedByUserId;
};
