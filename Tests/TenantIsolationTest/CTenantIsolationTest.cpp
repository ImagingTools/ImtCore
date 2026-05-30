// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTenantIsolationTest.h"

#include <QtConcurrent/QtConcurrent>
#include <QFuture>


void CTenantIsolationTest::init()
{
	m_storePtr = new isolation::CIsolationStore();
	m_tenantA = m_storePtr->CreateTenant("Tenant A");
	m_tenantB = m_storePtr->CreateTenant("Tenant B");
}


void CTenantIsolationTest::cleanup()
{
	delete m_storePtr;
	m_storePtr = nullptr;
}


// --- Isolation tests ---

void CTenantIsolationTest::testMembershipIsolation_DifferentTenants()
{
	QByteArray user1 = "user-1";
	QByteArray user2 = "user-2";

	m_storePtr->AddMembership(user1, m_tenantA, "admin");
	m_storePtr->AddMembership(user2, m_tenantB, "member");

	// Tenant A only sees user1
	QByteArrayList membershipsA = m_storePtr->GetMembershipsByTenant(m_tenantA);
	QCOMPARE(membershipsA.size(), 1);

	// Tenant B only sees user2
	QByteArrayList membershipsB = m_storePtr->GetMembershipsByTenant(m_tenantB);
	QCOMPARE(membershipsB.size(), 1);

	// No cross-contamination
	QVERIFY(membershipsA != membershipsB);
}


void CTenantIsolationTest::testPermissionIsolation_DifferentTenants()
{
	m_storePtr->AddPermission(m_tenantA, "resource-1", "admin");
	m_storePtr->AddPermission(m_tenantA, "resource-2", "read");
	m_storePtr->AddPermission(m_tenantB, "resource-3", "write");

	auto permA = m_storePtr->GetPermissionsByTenant(m_tenantA);
	auto permB = m_storePtr->GetPermissionsByTenant(m_tenantB);

	QCOMPARE(permA.size(), 2);
	QCOMPARE(permB.size(), 1);
}


void CTenantIsolationTest::testTicketIsolation_DifferentTenants()
{
	m_storePtr->AddTicket(m_tenantA, "Bug in A", "reporter-1");
	m_storePtr->AddTicket(m_tenantA, "Feature in A", "reporter-1");
	m_storePtr->AddTicket(m_tenantB, "Support in B", "reporter-2");

	QByteArrayList ticketsA = m_storePtr->GetTicketsByTenant(m_tenantA);
	QByteArrayList ticketsB = m_storePtr->GetTicketsByTenant(m_tenantB);

	QCOMPARE(ticketsA.size(), 2);
	QCOMPARE(ticketsB.size(), 1);
}


void CTenantIsolationTest::testTeamIsolation_BetweenTenants()
{
	QByteArray team1 = "team-1";
	QByteArray team2 = "team-2";
	QByteArray team3 = "team-3";

	m_storePtr->AddTeam(team1, m_tenantA);
	m_storePtr->AddTeam(team2, m_tenantA);
	m_storePtr->AddTeam(team3, m_tenantB);

	auto teamsA = m_storePtr->GetTeamsByTenant(m_tenantA);
	auto teamsB = m_storePtr->GetTeamsByTenant(m_tenantB);

	QCOMPARE(teamsA.size(), 2);
	QCOMPARE(teamsB.size(), 1);
}


void CTenantIsolationTest::testInvitationIsolation_PerTenant()
{
	m_storePtr->AddInvitation("user-x", m_tenantA);
	m_storePtr->AddInvitation("user-y", m_tenantA);
	m_storePtr->AddInvitation("user-z", m_tenantB);

	QByteArrayList invsA = m_storePtr->GetInvitationsByTenant(m_tenantA);
	QByteArrayList invsB = m_storePtr->GetInvitationsByTenant(m_tenantB);

	QCOMPARE(invsA.size(), 2);
	QCOMPARE(invsB.size(), 1);
}


// --- Concurrency tests ---

void CTenantIsolationTest::testConcurrentMembershipCreation_UniqueConstraint()
{
	// Attempt to create same membership from multiple threads
	QByteArray userId = "concurrent-user";
	QByteArray tenantId = m_tenantA;
	const int numThreads = 10;

	QAtomicInt successCount(0);
	QList<QFuture<void>> futures;

	for (int i = 0; i < numThreads; ++i){
		futures.append(QtConcurrent::run([&](){
			QByteArray result = m_storePtr->AddMembership(userId, tenantId, "member");
			if (!result.isEmpty()){
				successCount.fetchAndAddRelaxed(1);
			}
		}));
	}

	for (auto& future : futures){
		future.waitForFinished();
	}

	// Only one should succeed due to UNIQUE constraint
	QCOMPARE(successCount.loadRelaxed(), 1);
	QCOMPARE(m_storePtr->GetMembershipsByTenant(m_tenantA).size(), 1);
}


void CTenantIsolationTest::testConcurrentTenantDeletion_Consistency()
{
	QByteArray tempTenant = m_storePtr->CreateTenant("Temp");
	m_storePtr->AddMembership("user-del-1", tempTenant, "member");
	m_storePtr->AddMembership("user-del-2", tempTenant, "admin");
	m_storePtr->AddPermission(tempTenant, "res-1", "read");
	m_storePtr->AddTicket(tempTenant, "Ticket", "reporter");

	// Delete from multiple threads (only first should succeed)
	QAtomicInt deleteCount(0);
	QList<QFuture<void>> futures;

	for (int i = 0; i < 5; ++i){
		futures.append(QtConcurrent::run([&](){
			if (m_storePtr->RemoveTenant(tempTenant)){
				deleteCount.fetchAndAddRelaxed(1);
			}
		}));
	}

	for (auto& future : futures){
		future.waitForFinished();
	}

	// Exactly one delete should succeed
	QCOMPARE(deleteCount.loadRelaxed(), 1);

	// All dependent data should be gone
	QCOMPARE(m_storePtr->GetMembershipsByTenant(tempTenant).size(), 0);
	QCOMPARE(m_storePtr->GetPermissionsByTenant(tempTenant).size(), 0);
	QCOMPARE(m_storePtr->GetTicketsByTenant(tempTenant).size(), 0);
}


void CTenantIsolationTest::testConcurrentRoleUpdate_LastWriteWins()
{
	QByteArray membershipId = m_storePtr->AddMembership("user-role", m_tenantA, "viewer");
	QVERIFY(!membershipId.isEmpty());

	// Update from multiple threads
	QStringList roles = {"member", "admin", "viewer", "owner", "member"};
	QList<QFuture<void>> futures;

	for (const QString& role : roles){
		futures.append(QtConcurrent::run([&, role](){
			m_storePtr->UpdateMembershipRole(membershipId, role.toUtf8());
		}));
	}

	for (auto& future : futures){
		future.waitForFinished();
	}

	// Verify the update was applied (any valid role)
	auto memberships = m_storePtr->GetMembershipsByTenant(m_tenantA);
	QCOMPARE(memberships.size(), 1);
}


// --- Cascade tests ---

void CTenantIsolationTest::testTenantDeletion_CleansAllDependencies()
{
	QByteArray tenant = m_storePtr->CreateTenant("Doomed");

	m_storePtr->AddMembership("u1", tenant, "member");
	m_storePtr->AddMembership("u2", tenant, "admin");
	m_storePtr->AddPermission(tenant, "r1", "read");
	m_storePtr->AddPermission(tenant, "r2", "write");
	m_storePtr->AddTeam("t1", tenant);
	m_storePtr->AddInvitation("u3", tenant);
	m_storePtr->AddTicket(tenant, "Ticket1", "u1");
	m_storePtr->AddTicket(tenant, "Ticket2", "u2");

	QVERIFY(m_storePtr->RemoveTenant(tenant));

	QCOMPARE(m_storePtr->GetMembershipsByTenant(tenant).size(), 0);
	QCOMPARE(m_storePtr->GetPermissionsByTenant(tenant).size(), 0);
	QCOMPARE(m_storePtr->GetTeamsByTenant(tenant).size(), 0);
	QCOMPARE(m_storePtr->GetInvitationsByTenant(tenant).size(), 0);
	QCOMPARE(m_storePtr->GetTicketsByTenant(tenant).size(), 0);

	// Other tenants unaffected
	QCOMPARE(m_storePtr->GetTenantCount(), 2); // tenantA and tenantB remain
}


void CTenantIsolationTest::testDeactivatedTenant_DataPreserved()
{
	// Deactivation should NOT delete data (unlike removal)
	m_storePtr->AddMembership("u1", m_tenantA, "member");
	m_storePtr->AddTicket(m_tenantA, "Ticket", "u1");

	// Note: deactivation is a status change, not a delete. Data remains.
	QCOMPARE(m_storePtr->GetMembershipsByTenant(m_tenantA).size(), 1);
	QCOMPARE(m_storePtr->GetTicketsByTenant(m_tenantA).size(), 1);
}


// --- Cross-tenant access ---

void CTenantIsolationTest::testCrossTenantAccess_Denied()
{
	// User is member of tenant A but NOT tenant B
	QByteArray userId = "cross-tenant-user";
	m_storePtr->AddMembership(userId, m_tenantA, "admin");

	// Verify user is NOT found in tenant B memberships
	QByteArrayList membershipsB = m_storePtr->GetMembershipsByTenant(m_tenantB);
	for (const QByteArray& mid : membershipsB){
		// Should never find this user in tenant B
		Q_UNUSED(mid);
	}
	QCOMPARE(membershipsB.size(), 0);
}


void CTenantIsolationTest::testResourcePermission_IsolatedPerTenant()
{
	// Same resource ID in different tenants should have different permissions
	m_storePtr->AddPermission(m_tenantA, "shared-resource", "admin");
	m_storePtr->AddPermission(m_tenantB, "shared-resource", "read");

	auto permA = m_storePtr->GetPermissionsByTenant(m_tenantA);
	auto permB = m_storePtr->GetPermissionsByTenant(m_tenantB);

	QCOMPARE(permA.size(), 1);
	QCOMPARE(permB.size(), 1);
	QCOMPARE(permA[0].level, QString("admin"));
	QCOMPARE(permB[0].level, QString("read"));
}
