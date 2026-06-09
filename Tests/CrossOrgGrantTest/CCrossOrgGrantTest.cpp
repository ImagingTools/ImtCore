// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CCrossOrgGrantTest.h"

// ImtCore includes
#include <imtauth/CCrossOrgGrant.h>


namespace
{


imtauth::CrossOrgGrantInfo MakeSampleInfo()
{
	imtauth::CrossOrgGrantInfo info;
	info.grantId = "grant-1";
	info.sourceTenantId = "tenantA";
	info.targetTenantId = "tenantB";
	info.roleIds = QByteArrayList{"role-read", "role-write"};
	info.description = "Sample grant";
	info.createdAt = "2026-01-01T00:00:00.000Z";
	info.expiresAt = "2030-01-01T00:00:00.000Z";
	info.isActive = true;
	return info;
}


} // anonymous namespace


void CCrossOrgGrantTest::init()
{
	m_managerPtr = new imtauth::CMockCrossOrgGrantManager();
	m_resolverPtr = new imtauth::CMockDelegatedAccessResolver();
}


void CCrossOrgGrantTest::cleanup()
{
	delete m_managerPtr;
	m_managerPtr = nullptr;
	delete m_resolverPtr;
	m_resolverPtr = nullptr;
}


void CCrossOrgGrantTest::testGrantInfo_RoundTrip()
{
	imtauth::CCrossOrgGrant grant;
	imtauth::CrossOrgGrantInfo info = MakeSampleInfo();
	grant.SetGrantInfo(info);

	QCOMPARE(grant.GetGrantId(), QByteArray("grant-1"));
	QVERIFY(grant.GetGrantInfo() == info);
}


void CCrossOrgGrantTest::testGrantInfo_CopyFromAndClone()
{
	imtauth::CCrossOrgGrant source;
	source.SetGrantInfo(MakeSampleInfo());

	imtauth::CCrossOrgGrant target;
	QVERIFY(target.CopyFrom(source));
	QVERIFY(target.GetGrantInfo() == source.GetGrantInfo());

	istd::IChangeableUniquePtr clonePtr = source.CloneMe();
	QVERIFY(clonePtr.IsValid());
	const imtauth::ICrossOrgGrantData* clonedPtr = dynamic_cast<const imtauth::ICrossOrgGrantData*>(clonePtr.GetPtr());
	QVERIFY(clonedPtr != nullptr);
	QVERIFY(clonedPtr->GetGrantInfo() == source.GetGrantInfo());
}


void CCrossOrgGrantTest::testGrantInfo_ResetDataDefaults()
{
	imtauth::CCrossOrgGrant grant;
	grant.SetGrantInfo(MakeSampleInfo());

	QVERIFY(grant.ResetData());

	imtauth::CrossOrgGrantInfo info = grant.GetGrantInfo();
	QVERIFY(info.grantId.isEmpty());
	QVERIFY(info.sourceTenantId.isEmpty());
	QVERIFY(info.targetTenantId.isEmpty());
	QVERIFY(info.roleIds.isEmpty());
	QVERIFY(info.isActive);
}


void CCrossOrgGrantTest::testCreateGrant_Success()
{
	QByteArray grantId = m_managerPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-read"});
	QVERIFY(!grantId.isEmpty());
	QCOMPARE(m_managerPtr->m_grants.size(), 1);
}


void CCrossOrgGrantTest::testCreateGrant_MissingFields_Fails()
{
	QVERIFY(m_managerPtr->CreateGrant("", "tenantB", QByteArrayList{"role-read"}).isEmpty());
	QVERIFY(m_managerPtr->CreateGrant("tenantA", "", QByteArrayList{"role-read"}).isEmpty());
	QCOMPARE(m_managerPtr->m_grants.size(), 0);
}


void CCrossOrgGrantTest::testCreateGrant_SameTenant_Fails()
{
	QVERIFY(m_managerPtr->CreateGrant("tenantA", "tenantA", QByteArrayList{"role-read"}).isEmpty());
	QCOMPARE(m_managerPtr->m_grants.size(), 0);
}


void CCrossOrgGrantTest::testRevokeGrant_DeactivatesGrant()
{
	QByteArray grantId = m_managerPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-admin"});
	QVERIFY(m_managerPtr->RevokeGrant(grantId));
	QVERIFY(!m_managerPtr->HasAccess("tenantA", "tenantB", "role-admin"));
	// Re-revoking an already inactive grant returns false.
	QVERIFY(!m_managerPtr->RevokeGrant(grantId));
}


void CCrossOrgGrantTest::testRevokeGrant_Unknown_ReturnsFalse()
{
	QVERIFY(!m_managerPtr->RevokeGrant("does-not-exist"));
}


void CCrossOrgGrantTest::testHasAccess_NoneAlwaysAllowed()
{
	// Empty roleId always returns true.
	QVERIFY(m_managerPtr->HasAccess("tenantA", "tenantB", QByteArray()));
}


void CCrossOrgGrantTest::testHasAccess_GrantMatrix()
{
	m_managerPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-read", "role-write"});

	// Has the granted roles.
	QVERIFY(m_managerPtr->HasAccess("tenantA", "tenantB", "role-read"));
	QVERIFY(m_managerPtr->HasAccess("tenantA", "tenantB", "role-write"));
	// Does not have a non-granted role.
	QVERIFY(!m_managerPtr->HasAccess("tenantA", "tenantB", "role-admin"));
}


void CCrossOrgGrantTest::testHasAccess_RevokedDenied()
{
	QByteArray grantId = m_managerPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-admin"});
	QVERIFY(m_managerPtr->HasAccess("tenantA", "tenantB", "role-admin"));
	m_managerPtr->RevokeGrant(grantId);
	QVERIFY(!m_managerPtr->HasAccess("tenantA", "tenantB", "role-admin"));
}


void CCrossOrgGrantTest::testHasAccess_ExpiredDenied()
{
	m_managerPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-admin"},
				QString(), "2000-01-01T00:00:00.000Z");
	QVERIFY(!m_managerPtr->HasAccess("tenantA", "tenantB", "role-admin"));
}


void CCrossOrgGrantTest::testHasAccess_ScopedGrant()
{
	m_managerPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-docs"});

	// Has only the specific role granted.
	QVERIFY(m_managerPtr->HasAccess("tenantA", "tenantB", "role-docs"));
	QVERIFY(!m_managerPtr->HasAccess("tenantA", "tenantB", "role-billing"));
}


void CCrossOrgGrantTest::testHasAccess_WrongSourceDenied()
{
	m_managerPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-admin"});
	QVERIFY(!m_managerPtr->HasAccess("tenantC", "tenantB", "role-admin"));
}


void CCrossOrgGrantTest::testDelegatedSourceTenants_ListsEffectiveGrantsOnce()
{
	m_managerPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-read"});
	m_managerPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-write"});
	m_managerPtr->CreateGrant("tenantC", "tenantB", QByteArrayList{"role-read"});
	// Grant targeting a different tenant must not appear.
	m_managerPtr->CreateGrant("tenantD", "tenantE", QByteArrayList{"role-read"});

	QByteArrayList sources = m_managerPtr->GetDelegatedSourceTenants("tenantB");
	QCOMPARE(sources.size(), 2);
	QVERIFY(sources.contains("tenantA"));
	QVERIFY(sources.contains("tenantC"));
	QVERIFY(!sources.contains("tenantD"));
}


void CCrossOrgGrantTest::testDelegatedSourceTenants_ExcludesRevokedAndExpired()
{
	QByteArray revoked = m_managerPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-read"});
	m_managerPtr->CreateGrant("tenantC", "tenantB", QByteArrayList{"role-read"},
				QString(), "2000-01-01T00:00:00.000Z");
	m_managerPtr->RevokeGrant(revoked);

	QByteArrayList sources = m_managerPtr->GetDelegatedSourceTenants("tenantB");
	QVERIFY(sources.isEmpty());
}


void CCrossOrgGrantTest::testGrantedRoles_UnionAcrossGrants()
{
	m_managerPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-read", "role-write"});
	m_managerPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-write", "role-support"});

	QByteArrayList roles = m_managerPtr->GetGrantedRoles("tenantA", "tenantB");
	QCOMPARE(roles.size(), 3);
	QVERIFY(roles.contains("role-read"));
	QVERIFY(roles.contains("role-write"));
	QVERIFY(roles.contains("role-support"));
}


void CCrossOrgGrantTest::testGrantedRoles_WrongSourceEmpty()
{
	m_managerPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-read"});
	QVERIFY(m_managerPtr->GetGrantedRoles("tenantC", "tenantB").isEmpty());
}


void CCrossOrgGrantTest::testIsDelegatedAccess_ReflectsEffectiveGrant()
{
	QVERIFY(!m_managerPtr->IsDelegatedAccess("tenantA", "tenantB"));

	QByteArray grantId = m_managerPtr->CreateGrant("tenantA", "tenantB", QByteArrayList{"role-read"});
	QVERIFY(m_managerPtr->IsDelegatedAccess("tenantA", "tenantB"));
	// Wrong source remains delegated-access free.
	QVERIFY(!m_managerPtr->IsDelegatedAccess("tenantC", "tenantB"));

	m_managerPtr->RevokeGrant(grantId);
	QVERIFY(!m_managerPtr->IsDelegatedAccess("tenantA", "tenantB"));
}


void CCrossOrgGrantTest::testResolver_AccessibleTenantsCombineMembershipAndDelegated()
{
	// User u1 is a member of B and B2; tenant A delegates to B.
	m_resolverPtr->AddMembership("u1", "tenantB");
	m_resolverPtr->AddMembership("u1", "tenantB2");
	m_resolverPtr->m_grantManager.CreateGrant("tenantA", "tenantB", QByteArrayList{"role-support"});
	// A grant targeting a different tenant must not leak in.
	m_resolverPtr->m_grantManager.CreateGrant("tenantX", "tenantOther", QByteArrayList{"role-support"});

	imtauth::TenantAccessList access = m_resolverPtr->GetAccessibleTenants("u1", "tenantB");
	QCOMPARE(access.size(), 3);

	int membershipCount = 0;
	bool delegatedA = false;
	for (const imtauth::TenantAccessInfo& info : access){
		if (info.accessType == imtauth::TAT_MEMBERSHIP){
			++membershipCount;
		}
		if (info.tenantId == "tenantA"){
			delegatedA = true;
			QCOMPARE(info.accessType, imtauth::TAT_DELEGATED);
			QVERIFY(info.delegatedRoleIds.contains("role-support"));
		}
	}
	QCOMPARE(membershipCount, 2);
	QVERIFY(delegatedA);
}


void CCrossOrgGrantTest::testResolver_MembershipTakesPrecedenceOverDelegated()
{
	// User is both a member of A and reachable to A via a grant: membership wins.
	m_resolverPtr->AddMembership("u1", "tenantA");
	m_resolverPtr->m_grantManager.CreateGrant("tenantA", "tenantB", QByteArrayList{"role-support"});

	imtauth::TenantAccessList access = m_resolverPtr->GetAccessibleTenants("u1", "tenantB");

	int countA = 0;
	for (const imtauth::TenantAccessInfo& info : access){
		if (info.tenantId == "tenantA"){
			++countA;
			QCOMPARE(info.accessType, imtauth::TAT_MEMBERSHIP);
		}
	}
	QCOMPARE(countA, 1);
}


void CCrossOrgGrantTest::testResolver_ResolveTenantAccessTypes()
{
	m_resolverPtr->AddMembership("u1", "tenantB");
	m_resolverPtr->m_grantManager.CreateGrant("tenantA", "tenantB", QByteArrayList{"role-support"});

	// Member of the target.
	QCOMPARE(m_resolverPtr->ResolveTenantAccess("u1", "tenantB", "tenantB").accessType, imtauth::TAT_MEMBERSHIP);

	// Delegated into the target with granted roles.
	imtauth::TenantAccessInfo delegated = m_resolverPtr->ResolveTenantAccess("u1", "tenantB", "tenantA");
	QCOMPARE(delegated.accessType, imtauth::TAT_DELEGATED);
	QVERIFY(delegated.delegatedRoleIds.contains("role-support"));

	// No relationship at all.
	QCOMPARE(m_resolverPtr->ResolveTenantAccess("u1", "tenantB", "tenantZ").accessType, imtauth::TAT_NONE);
}


void CCrossOrgGrantTest::testResolver_IsAccessAllowedDelegatedScope()
{
	m_resolverPtr->m_grantManager.CreateGrant("tenantA", "tenantB", QByteArrayList{"role-support"});

	// Granted role is allowed, ungranted is denied.
	QVERIFY(m_resolverPtr->IsAccessAllowed("u1", "tenantB", "tenantA", "role-support"));
	QVERIFY(!m_resolverPtr->IsAccessAllowed("u1", "tenantB", "tenantA", "role-admin"));
	// Empty required role is always allowed for delegated access.
	QVERIFY(m_resolverPtr->IsAccessAllowed("u1", "tenantB", "tenantA", QByteArray()));
}


void CCrossOrgGrantTest::testResolver_IsAccessAllowedMembershipAndNone()
{
	m_resolverPtr->AddMembership("u1", "tenantB");

	// Membership access defers to personal rights (allowed).
	QVERIFY(m_resolverPtr->IsAccessAllowed("u1", "tenantB", "tenantB", "role-anything"));
	// No access path => denied.
	QVERIFY(!m_resolverPtr->IsAccessAllowed("u1", "tenantB", "tenantZ", "role-support"));
}
