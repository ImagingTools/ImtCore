// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTenantInvitationManagerTest.h"


void CTenantInvitationManagerTest::init()
{
	m_managerPtr = new imtauth::CMockInvitationManager();
	m_tenantId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	m_userId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	m_invitedByUserId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
}


void CTenantInvitationManagerTest::cleanup()
{
	delete m_managerPtr;
	m_managerPtr = nullptr;
}


// --- Create invitation tests ---

void CTenantInvitationManagerTest::testCreateInvitation_Success()
{
	QByteArray id = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);

	QVERIFY(!id.isEmpty());
	auto inv = m_managerPtr->GetInvitation(id);
	QVERIFY(inv != nullptr);
	QCOMPARE(inv->userId, m_userId);
	QCOMPARE(inv->tenantId, m_tenantId);
	QCOMPARE(inv->status, imtauth::TIS_PENDING);
	QCOMPARE(inv->invitedByUserId, m_invitedByUserId);
	QVERIFY(inv->expiresAt.isValid());
	QVERIFY(inv->lastSentAt.isValid());
}


void CTenantInvitationManagerTest::testCreateInvitation_DuplicatePending_Fails()
{
	QByteArray id1 = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	QVERIFY(!id1.isEmpty());

	// Second pending invitation for same user+tenant should fail
	QByteArray id2 = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	QVERIFY(id2.isEmpty());
}


void CTenantInvitationManagerTest::testCreateInvitation_AfterRejection_Allowed()
{
	QByteArray id1 = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	m_managerPtr->RejectInvitation(id1, m_userId);

	// After rejection, a new invitation should be allowed
	QByteArray id2 = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	QVERIFY(!id2.isEmpty());
	QVERIFY(id1 != id2);
}


// --- Accept invitation tests ---

void CTenantInvitationManagerTest::testAcceptInvitation_Success_CreatesMembership()
{
	QByteArray invId = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	QByteArray membershipId = m_managerPtr->AcceptInvitation(invId, m_userId);

	QVERIFY(!membershipId.isEmpty());
	auto inv = m_managerPtr->GetInvitation(invId);
	QCOMPARE(inv->status, imtauth::TIS_ACCEPTED);
	QVERIFY(inv->acceptedAt.isValid());
}


void CTenantInvitationManagerTest::testAcceptInvitation_WrongUser_Fails()
{
	QByteArray invId = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	QByteArray wrongUser = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	QByteArray membershipId = m_managerPtr->AcceptInvitation(invId, wrongUser);
	QVERIFY(membershipId.isEmpty());

	auto inv = m_managerPtr->GetInvitation(invId);
	QCOMPARE(inv->status, imtauth::TIS_PENDING); // Status unchanged
}


void CTenantInvitationManagerTest::testAcceptInvitation_AlreadyAccepted_Fails()
{
	QByteArray invId = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	m_managerPtr->AcceptInvitation(invId, m_userId);

	// Try to accept again
	QByteArray result = m_managerPtr->AcceptInvitation(invId, m_userId);
	QVERIFY(result.isEmpty());
}


void CTenantInvitationManagerTest::testAcceptInvitation_Expired_Fails()
{
	QByteArray invId = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);

	// Force expiration by setting expiresAt to the past
	m_managerPtr->m_invitations[invId].expiresAt = QDateTime::currentDateTimeUtc().addDays(-1);

	QByteArray membershipId = m_managerPtr->AcceptInvitation(invId, m_userId);
	QVERIFY(membershipId.isEmpty());
}


// --- Reject/Revoke tests ---

void CTenantInvitationManagerTest::testRejectInvitation_Success()
{
	QByteArray invId = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	QVERIFY(m_managerPtr->RejectInvitation(invId, m_userId));

	auto inv = m_managerPtr->GetInvitation(invId);
	QCOMPARE(inv->status, imtauth::TIS_REJECTED);
	QVERIFY(inv->rejectedAt.isValid());
}


void CTenantInvitationManagerTest::testRejectInvitation_WrongUser_Fails()
{
	QByteArray invId = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	QByteArray wrongUser = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	QVERIFY(!m_managerPtr->RejectInvitation(invId, wrongUser));
}


void CTenantInvitationManagerTest::testRevokeInvitation_Success()
{
	QByteArray invId = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	QVERIFY(m_managerPtr->RevokeInvitation(invId, m_invitedByUserId));

	auto inv = m_managerPtr->GetInvitation(invId);
	QCOMPARE(inv->status, imtauth::TIS_REVOKED);
	QCOMPARE(inv->revokedByUserId, m_invitedByUserId);
	QVERIFY(inv->revokedAt.isValid());
}


// --- Resend tests ---

void CTenantInvitationManagerTest::testResendInvitation_Success_UpdatesLastSentAt()
{
	QByteArray invId = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	QDateTime originalSentAt = m_managerPtr->GetInvitation(invId)->lastSentAt;

	QTest::qWait(5);
	QVERIFY(m_managerPtr->ResendInvitation(invId));

	auto inv = m_managerPtr->GetInvitation(invId);
	QVERIFY(inv->lastSentAt >= originalSentAt);
	QVERIFY(inv->expiresAt > originalSentAt); // Extended expiry
}


void CTenantInvitationManagerTest::testResendInvitation_NonPending_Fails()
{
	QByteArray invId = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	m_managerPtr->AcceptInvitation(invId, m_userId);

	QVERIFY(!m_managerPtr->ResendInvitation(invId));
}


// --- Status and query tests ---

void CTenantInvitationManagerTest::testGetEffectiveStatus_Pending_NotExpired()
{
	QByteArray invId = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	auto inv = m_managerPtr->GetInvitation(invId);
	QCOMPARE(m_managerPtr->GetEffectiveStatus(*inv), imtauth::TIS_PENDING);
}


void CTenantInvitationManagerTest::testGetEffectiveStatus_Pending_Expired()
{
	QByteArray invId = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	m_managerPtr->m_invitations[invId].expiresAt = QDateTime::currentDateTimeUtc().addSecs(-60);

	auto inv = m_managerPtr->GetInvitation(invId);
	QCOMPARE(m_managerPtr->GetEffectiveStatus(*inv), imtauth::TIS_EXPIRED);
}


void CTenantInvitationManagerTest::testGetEffectiveStatus_Accepted()
{
	QByteArray invId = m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	m_managerPtr->AcceptInvitation(invId, m_userId);

	auto inv = m_managerPtr->GetInvitation(invId);
	QCOMPARE(m_managerPtr->GetEffectiveStatus(*inv), imtauth::TIS_ACCEPTED);
}


void CTenantInvitationManagerTest::testGetInvitationsByTenant_FilterByStatus()
{
	QByteArray user2 = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QByteArray user3 = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	QByteArray inv2 = m_managerPtr->CreateInvitation(m_invitedByUserId, user2, m_tenantId);
	m_managerPtr->CreateInvitation(m_invitedByUserId, user3, m_tenantId);

	// Accept one
	m_managerPtr->AcceptInvitation(inv2, user2);

	// Query only pending
	QList<imtauth::TenantInvitationStatus> filter;
	filter.append(imtauth::TIS_PENDING);
	QByteArrayList pending = m_managerPtr->GetInvitationsByTenant(m_tenantId, filter);
	QCOMPARE(pending.size(), 2);

	// Query only accepted
	filter.clear();
	filter.append(imtauth::TIS_ACCEPTED);
	QByteArrayList accepted = m_managerPtr->GetInvitationsByTenant(m_tenantId, filter);
	QCOMPARE(accepted.size(), 1);
}


void CTenantInvitationManagerTest::testGetInvitationsByUser_AllStatuses()
{
	QByteArray tenant2 = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, tenant2);

	QByteArrayList all = m_managerPtr->GetInvitationsByUser(m_userId);
	QCOMPARE(all.size(), 2);
}


void CTenantInvitationManagerTest::testFindPendingInvitation_Exists()
{
	m_managerPtr->CreateInvitation(m_invitedByUserId, m_userId, m_tenantId);
	auto inv = m_managerPtr->FindPendingInvitation(m_userId, m_tenantId);
	QVERIFY(inv != nullptr);
	QCOMPARE(inv->status, imtauth::TIS_PENDING);
}


void CTenantInvitationManagerTest::testFindPendingInvitation_NotExists()
{
	QByteArray otherUser = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	auto inv = m_managerPtr->FindPendingInvitation(otherUser, m_tenantId);
	QVERIFY(inv == nullptr);
}
