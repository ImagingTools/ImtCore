// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CDelegatedAccessTest.h"


void CDelegatedAccessTest::init()
{
	m_membershipPtr = new imtauth::CMockMembershipManager();
	m_grantPtr = new imtauth::CMockCrossOrgGrantForDelegated();
	m_resolverPtr = new imtauth::CMockDelegatedAccessResolver(m_membershipPtr, m_grantPtr);
}


void CDelegatedAccessTest::cleanup()
{
	delete m_resolverPtr;
	m_resolverPtr = nullptr;
	delete m_grantPtr;
	m_grantPtr = nullptr;
	delete m_membershipPtr;
	m_membershipPtr = nullptr;
}


// ===========================================================================
// Org visibility tests
// ===========================================================================

void CDelegatedAccessTest::testGetAccessibleTenants_DirectMembershipOnly()
{
	m_membershipPtr->AddMembership("user1", "tenantA", "admin");
	m_membershipPtr->AddMembership("user1", "tenantB", "member");

	imtauth::DelegatedTenantList tenants = m_resolverPtr->GetAccessibleTenants("user1", "tenantA");

	QCOMPARE(tenants.size(), 2);
	QCOMPARE(tenants[0].tenantId, QByteArray("tenantA"));
	QVERIFY(!tenants[0].isDelegated);
	QCOMPARE(tenants[1].tenantId, QByteArray("tenantB"));
	QVERIFY(!tenants[1].isDelegated);
}


void CDelegatedAccessTest::testGetAccessibleTenants_DelegatedOnly()
{
	// User is member of tenantB only; tenantA grants access to tenantB
	m_membershipPtr->AddMembership("user1", "tenantB", "member");
	m_grantPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-support"});

	imtauth::DelegatedTenantList tenants = m_resolverPtr->GetAccessibleTenants("user1", "tenantB");

	// Should see tenantB (direct) + tenantA (delegated)
	QCOMPARE(tenants.size(), 2);
	QCOMPARE(tenants[0].tenantId, QByteArray("tenantB"));
	QVERIFY(!tenants[0].isDelegated);
	QCOMPARE(tenants[1].tenantId, QByteArray("tenantA"));
	QVERIFY(tenants[1].isDelegated);
	QVERIFY(tenants[1].roleIds.contains("role-support"));
}


void CDelegatedAccessTest::testGetAccessibleTenants_MixedMembershipAndDelegated()
{
	m_membershipPtr->AddMembership("user1", "tenantB", "member");
	m_membershipPtr->AddMembership("user1", "tenantC", "admin");
	m_grantPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-read"});

	imtauth::DelegatedTenantList tenants = m_resolverPtr->GetAccessibleTenants("user1", "tenantB");

	QCOMPARE(tenants.size(), 3);
	// Direct memberships first
	QCOMPARE(tenants[0].tenantId, QByteArray("tenantB"));
	QVERIFY(!tenants[0].isDelegated);
	QCOMPARE(tenants[1].tenantId, QByteArray("tenantC"));
	QVERIFY(!tenants[1].isDelegated);
	// Delegated
	QCOMPARE(tenants[2].tenantId, QByteArray("tenantA"));
	QVERIFY(tenants[2].isDelegated);
}


void CDelegatedAccessTest::testGetAccessibleTenants_DirectTakesPrecedence()
{
	// User is direct member of tenantA AND there's a grant from tenantA to tenantB
	m_membershipPtr->AddMembership("user1", "tenantA", "admin");
	m_membershipPtr->AddMembership("user1", "tenantB", "member");
	m_grantPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-read"});

	imtauth::DelegatedTenantList tenants = m_resolverPtr->GetAccessibleTenants("user1", "tenantB");

	// tenantA should appear as direct (not delegated) since user is already a member
	QCOMPARE(tenants.size(), 2);
	bool foundA = false;
	for (const imtauth::DelegatedTenantInfo& t : tenants){
		if (t.tenantId == "tenantA"){
			QVERIFY(!t.isDelegated);
			foundA = true;
		}
	}
	QVERIFY(foundA);
}


void CDelegatedAccessTest::testGetAccessibleTenants_InactiveGrantExcluded()
{
	m_membershipPtr->AddMembership("user1", "tenantB", "member");
	QByteArray grantId = m_grantPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-read"});
	m_grantPtr->RevokeGrant(grantId);

	imtauth::DelegatedTenantList tenants = m_resolverPtr->GetAccessibleTenants("user1", "tenantB");

	// Only direct membership
	QCOMPARE(tenants.size(), 1);
	QCOMPARE(tenants[0].tenantId, QByteArray("tenantB"));
}


void CDelegatedAccessTest::testGetAccessibleTenants_ExpiredGrantExcluded()
{
	m_membershipPtr->AddMembership("user1", "tenantB", "member");
	m_grantPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-read"}, QString(), "2000-01-01T00:00:00.000Z");

	imtauth::DelegatedTenantList tenants = m_resolverPtr->GetAccessibleTenants("user1", "tenantB");

	QCOMPARE(tenants.size(), 1);
	QCOMPARE(tenants[0].tenantId, QByteArray("tenantB"));
}


void CDelegatedAccessTest::testGetAccessibleTenants_MultipleGrantsMergeRoles()
{
	m_membershipPtr->AddMembership("user1", "tenantB", "member");
	m_grantPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-read"});
	m_grantPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-write"});

	imtauth::DelegatedTenantList tenants = m_resolverPtr->GetAccessibleTenants("user1", "tenantB");

	QCOMPARE(tenants.size(), 2);
	// Delegated entry should have merged roles
	const imtauth::DelegatedTenantInfo& delegated = tenants[1];
	QCOMPARE(delegated.tenantId, QByteArray("tenantA"));
	QVERIFY(delegated.isDelegated);
	QVERIFY(delegated.roleIds.contains("role-read"));
	QVERIFY(delegated.roleIds.contains("role-write"));
}


// ===========================================================================
// Permission check tests
// ===========================================================================

void CDelegatedAccessTest::testHasDelegatedRole_DirectMembership()
{
	m_membershipPtr->AddMembership("user1", "tenantA", "admin");

	QVERIFY(m_resolverPtr->HasDelegatedRole("user1", "tenantB", "tenantA", "admin"));
	QVERIFY(!m_resolverPtr->HasDelegatedRole("user1", "tenantB", "tenantA", "superadmin"));
}


void CDelegatedAccessTest::testHasDelegatedRole_DelegatedGrant()
{
	m_membershipPtr->AddMembership("user1", "tenantB", "member");
	m_grantPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-support"});

	QVERIFY(m_resolverPtr->HasDelegatedRole("user1", "tenantB", "tenantA", "role-support"));
}


void CDelegatedAccessTest::testHasDelegatedRole_NoAccess()
{
	m_membershipPtr->AddMembership("user1", "tenantB", "member");

	QVERIFY(!m_resolverPtr->HasDelegatedRole("user1", "tenantB", "tenantA", "role-support"));
}


void CDelegatedAccessTest::testHasDelegatedRole_EmptyRoleMeansAny()
{
	m_membershipPtr->AddMembership("user1", "tenantB", "member");
	m_grantPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-support"});

	// Empty role means "any access"
	QVERIFY(m_resolverPtr->HasDelegatedRole("user1", "tenantB", "tenantA", QByteArray()));
}


void CDelegatedAccessTest::testHasDelegatedRole_WrongRole()
{
	m_membershipPtr->AddMembership("user1", "tenantB", "member");
	m_grantPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-support"});

	QVERIFY(!m_resolverPtr->HasDelegatedRole("user1", "tenantB", "tenantA", "role-admin"));
}


void CDelegatedAccessTest::testHasDelegatedRole_RevokedGrant()
{
	m_membershipPtr->AddMembership("user1", "tenantB", "member");
	QByteArray grantId = m_grantPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-support"});
	m_grantPtr->RevokeGrant(grantId);

	QVERIFY(!m_resolverPtr->HasDelegatedRole("user1", "tenantB", "tenantA", "role-support"));
}


// ===========================================================================
// Delegated roles tests
// ===========================================================================

void CDelegatedAccessTest::testGetDelegatedRoles_ReturnsGrantedRoles()
{
	m_grantPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-read", "role-write"});

	QByteArrayList roles = m_resolverPtr->GetDelegatedRoles("tenantB", "tenantA");

	QCOMPARE(roles.size(), 2);
	QVERIFY(roles.contains("role-read"));
	QVERIFY(roles.contains("role-write"));
}


void CDelegatedAccessTest::testGetDelegatedRoles_ExcludesExpired()
{
	m_grantPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-admin"}, QString(), "2000-01-01T00:00:00.000Z");

	QByteArrayList roles = m_resolverPtr->GetDelegatedRoles("tenantB", "tenantA");

	QVERIFY(roles.isEmpty());
}


void CDelegatedAccessTest::testGetDelegatedRoles_NoneForDirectMember()
{
	// Grant roles are returned regardless of membership (they are grant-specific)
	// But if there's no grant, no roles are returned
	QByteArrayList roles = m_resolverPtr->GetDelegatedRoles("tenantB", "tenantA");
	QVERIFY(roles.isEmpty());
}


// ===========================================================================
// Context detection tests
// ===========================================================================

void CDelegatedAccessTest::testIsDelegatedAccess_TrueForGrantOnly()
{
	m_membershipPtr->AddMembership("user1", "tenantB", "member");
	m_grantPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-support"});

	QVERIFY(m_resolverPtr->IsDelegatedAccess("user1", "tenantB", "tenantA"));
}


void CDelegatedAccessTest::testIsDelegatedAccess_FalseForDirectMember()
{
	m_membershipPtr->AddMembership("user1", "tenantA", "admin");
	m_grantPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-support"});

	// User is direct member of tenantA — not delegated
	QVERIFY(!m_resolverPtr->IsDelegatedAccess("user1", "tenantB", "tenantA"));
}


void CDelegatedAccessTest::testIsDelegatedAccess_FalseForNoAccess()
{
	m_membershipPtr->AddMembership("user1", "tenantB", "member");

	// No grant from tenantA to tenantB
	QVERIFY(!m_resolverPtr->IsDelegatedAccess("user1", "tenantB", "tenantA"));
}
