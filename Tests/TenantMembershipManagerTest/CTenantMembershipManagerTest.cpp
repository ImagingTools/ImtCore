// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTenantMembershipManagerTest.h"


void CTenantMembershipManagerTest::init()
{
	m_managerPtr = new imtauth::CMockMembershipManager();
	m_tenantId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	m_userId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	m_managerPtr->addValidTenant(m_tenantId, "owner-user-id");
}


void CTenantMembershipManagerTest::cleanup()
{
	delete m_managerPtr;
	m_managerPtr = nullptr;
}


// --- Add membership tests ---

void CTenantMembershipManagerTest::testAddMembership_Success()
{
	QByteArray membershipId = m_managerPtr->AddMembership(m_userId, m_tenantId, "member");

	QVERIFY(!membershipId.isEmpty());
	auto membership = m_managerPtr->GetMembership(membershipId);
	QVERIFY(membership != nullptr);
	QCOMPARE(membership->userId, m_userId);
	QCOMPARE(membership->tenantId, m_tenantId);
	QCOMPARE(membership->roleId, QByteArray("member"));
	QVERIFY(membership->isActive);
	QVERIFY(!membership->joinedAt.isEmpty());
}


void CTenantMembershipManagerTest::testAddMembership_DuplicateUserTenant_Fails()
{
	QByteArray id1 = m_managerPtr->AddMembership(m_userId, m_tenantId, "member");
	QVERIFY(!id1.isEmpty());

	// Second attempt with same user+tenant should fail (UNIQUE constraint)
	QByteArray id2 = m_managerPtr->AddMembership(m_userId, m_tenantId, "admin");
	QVERIFY(id2.isEmpty());
	QCOMPARE(m_managerPtr->m_memberships.size(), 1);
}


void CTenantMembershipManagerTest::testAddMembership_InvalidTenant_Fails()
{
	QByteArray fakeTenantId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QByteArray id = m_managerPtr->AddMembership(m_userId, fakeTenantId, "member");
	QVERIFY(id.isEmpty());
}


// --- Get membership tests ---

void CTenantMembershipManagerTest::testGetMembership_Existing()
{
	QByteArray id = m_managerPtr->AddMembership(m_userId, m_tenantId, "admin");
	auto membership = m_managerPtr->GetMembership(id);
	QVERIFY(membership != nullptr);
	QCOMPARE(membership->id, id);
}


void CTenantMembershipManagerTest::testGetMembership_NonExisting_ReturnsNull()
{
	QByteArray fakeId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	auto membership = m_managerPtr->GetMembership(fakeId);
	QVERIFY(membership == nullptr);
}


void CTenantMembershipManagerTest::testFindMembership_Existing()
{
	m_managerPtr->AddMembership(m_userId, m_tenantId, "member");
	auto membership = m_managerPtr->FindMembership(m_userId, m_tenantId);
	QVERIFY(membership != nullptr);
	QCOMPARE(membership->userId, m_userId);
	QCOMPARE(membership->tenantId, m_tenantId);
}


void CTenantMembershipManagerTest::testFindMembership_NonExisting_ReturnsNull()
{
	QByteArray otherUser = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	auto membership = m_managerPtr->FindMembership(otherUser, m_tenantId);
	QVERIFY(membership == nullptr);
}


void CTenantMembershipManagerTest::testGetMembershipsByTenant_Empty()
{
	QByteArrayList ids = m_managerPtr->GetMembershipsByTenant(m_tenantId);
	QVERIFY(ids.isEmpty());
}


void CTenantMembershipManagerTest::testGetMembershipsByTenant_Multiple()
{
	QByteArray user1 = "user1";
	QByteArray user2 = "user2";
	QByteArray user3 = "user3";

	m_managerPtr->AddMembership(user1, m_tenantId, "member");
	m_managerPtr->AddMembership(user2, m_tenantId, "admin");
	m_managerPtr->AddMembership(user3, m_tenantId, "viewer");

	QByteArrayList ids = m_managerPtr->GetMembershipsByTenant(m_tenantId);
	QCOMPARE(ids.size(), 3);
}


void CTenantMembershipManagerTest::testGetMembershipsByUser_MultipleTenants()
{
	QByteArray tenant2 = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QByteArray tenant3 = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	m_managerPtr->addValidTenant(tenant2);
	m_managerPtr->addValidTenant(tenant3);

	m_managerPtr->AddMembership(m_userId, m_tenantId, "member");
	m_managerPtr->AddMembership(m_userId, tenant2, "admin");
	m_managerPtr->AddMembership(m_userId, tenant3, "viewer");

	QByteArrayList ids = m_managerPtr->GetMembershipsByUser(m_userId);
	QCOMPARE(ids.size(), 3);
}


// --- Remove/update tests ---

void CTenantMembershipManagerTest::testRemoveMembership_Existing()
{
	QByteArray id = m_managerPtr->AddMembership(m_userId, m_tenantId, "member");
	QVERIFY(m_managerPtr->RemoveMembership(id));
	QVERIFY(m_managerPtr->GetMembership(id) == nullptr);
}


void CTenantMembershipManagerTest::testRemoveMembership_NonExisting_ReturnsFalse()
{
	QByteArray fakeId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QVERIFY(!m_managerPtr->RemoveMembership(fakeId));
}


void CTenantMembershipManagerTest::testUpdateMembershipRole_Success()
{
	QByteArray id = m_managerPtr->AddMembership(m_userId, m_tenantId, "member");
	QVERIFY(m_managerPtr->UpdateMembershipRole(id, "admin"));

	auto membership = m_managerPtr->GetMembership(id);
	QCOMPARE(membership->roleId, QByteArray("admin"));
}


void CTenantMembershipManagerTest::testUpdateMembershipRole_NonExisting_ReturnsFalse()
{
	QByteArray fakeId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QVERIFY(!m_managerPtr->UpdateMembershipRole(fakeId, "admin"));
}


// --- Permission checks ---

void CTenantMembershipManagerTest::testIsMember_True()
{
	m_managerPtr->AddMembership(m_userId, m_tenantId, "member");
	QVERIFY(m_managerPtr->IsMember(m_userId, m_tenantId));
}


void CTenantMembershipManagerTest::testIsMember_False()
{
	QByteArray otherUser = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QVERIFY(!m_managerPtr->IsMember(otherUser, m_tenantId));
}


void CTenantMembershipManagerTest::testHasMinimumRole_Owner()
{
	// Owner (identified by tenantOwners map, not by membership role) has all roles
	QVERIFY(m_managerPtr->HasMinimumRole("owner-user-id", m_tenantId, "admin"));
	QVERIFY(m_managerPtr->HasMinimumRole("owner-user-id", m_tenantId, "owner"));
}


void CTenantMembershipManagerTest::testHasMinimumRole_Admin()
{
	m_managerPtr->AddMembership(m_userId, m_tenantId, "admin");
	QVERIFY(m_managerPtr->HasMinimumRole(m_userId, m_tenantId, "member"));
	QVERIFY(m_managerPtr->HasMinimumRole(m_userId, m_tenantId, "admin"));
	QVERIFY(!m_managerPtr->HasMinimumRole(m_userId, m_tenantId, "owner"));
}


void CTenantMembershipManagerTest::testHasMinimumRole_Member_BelowRequired()
{
	m_managerPtr->AddMembership(m_userId, m_tenantId, "member");
	QVERIFY(m_managerPtr->HasMinimumRole(m_userId, m_tenantId, "viewer"));
	QVERIFY(m_managerPtr->HasMinimumRole(m_userId, m_tenantId, "member"));
	QVERIFY(!m_managerPtr->HasMinimumRole(m_userId, m_tenantId, "admin"));
}


void CTenantMembershipManagerTest::testHasMinimumRole_NonMember_ReturnsFalse()
{
	QByteArray otherUser = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QVERIFY(!m_managerPtr->HasMinimumRole(otherUser, m_tenantId, "viewer"));
}
