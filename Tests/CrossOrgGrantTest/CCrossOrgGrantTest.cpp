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
	info.relationshipId = "rel-1";
	info.targetTeamId = "team-1";
	info.accessLevel = imtauth::COAL_WRITE;
	info.resourceScope = "documents";
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
}


void CCrossOrgGrantTest::cleanup()
{
	delete m_managerPtr;
	m_managerPtr = nullptr;
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
	QCOMPARE(info.accessLevel, imtauth::COAL_NONE);
	QVERIFY(info.isActive);
}


void CCrossOrgGrantTest::testCreateGrant_Success()
{
	QByteArray grantId = m_managerPtr->CreateGrant("tenantA", "tenantB", "rel-1", imtauth::COAL_READ);
	QVERIFY(!grantId.isEmpty());
	QCOMPARE(m_managerPtr->m_grants.size(), 1);
}


void CCrossOrgGrantTest::testCreateGrant_MissingFields_Fails()
{
	QVERIFY(m_managerPtr->CreateGrant("", "tenantB", "rel-1", imtauth::COAL_READ).isEmpty());
	QVERIFY(m_managerPtr->CreateGrant("tenantA", "", "rel-1", imtauth::COAL_READ).isEmpty());
	QVERIFY(m_managerPtr->CreateGrant("tenantA", "tenantB", "", imtauth::COAL_READ).isEmpty());
	QCOMPARE(m_managerPtr->m_grants.size(), 0);
}


void CCrossOrgGrantTest::testCreateGrant_SameTenant_Fails()
{
	QVERIFY(m_managerPtr->CreateGrant("tenantA", "tenantA", "rel-1", imtauth::COAL_READ).isEmpty());
	QCOMPARE(m_managerPtr->m_grants.size(), 0);
}


void CCrossOrgGrantTest::testRevokeGrant_DeactivatesGrant()
{
	QByteArray grantId = m_managerPtr->CreateGrant("tenantA", "tenantB", "rel-1", imtauth::COAL_ADMIN);
	QVERIFY(m_managerPtr->RevokeGrant(grantId));
	QVERIFY(!m_managerPtr->HasAccess("tenantA", "tenantB", "documents", imtauth::COAL_READ));
	// Re-revoking an already inactive grant returns false.
	QVERIFY(!m_managerPtr->RevokeGrant(grantId));
}


void CCrossOrgGrantTest::testRevokeGrant_Unknown_ReturnsFalse()
{
	QVERIFY(!m_managerPtr->RevokeGrant("does-not-exist"));
}


void CCrossOrgGrantTest::testHasAccess_NoneAlwaysAllowed()
{
	QVERIFY(m_managerPtr->HasAccess("tenantA", "tenantB", "documents", imtauth::COAL_NONE));
}


void CCrossOrgGrantTest::testHasAccess_GrantMatrix()
{
	m_managerPtr->CreateGrant("tenantA", "tenantB", "rel-1", imtauth::COAL_WRITE);

	// A WRITE grant satisfies READ and WRITE but not ADMIN.
	QVERIFY(m_managerPtr->HasAccess("tenantA", "tenantB", "documents", imtauth::COAL_READ));
	QVERIFY(m_managerPtr->HasAccess("tenantA", "tenantB", "documents", imtauth::COAL_WRITE));
	QVERIFY(!m_managerPtr->HasAccess("tenantA", "tenantB", "documents", imtauth::COAL_ADMIN));
}


void CCrossOrgGrantTest::testHasAccess_RevokedDenied()
{
	QByteArray grantId = m_managerPtr->CreateGrant("tenantA", "tenantB", "rel-1", imtauth::COAL_ADMIN);
	QVERIFY(m_managerPtr->HasAccess("tenantA", "tenantB", "documents", imtauth::COAL_ADMIN));
	m_managerPtr->RevokeGrant(grantId);
	QVERIFY(!m_managerPtr->HasAccess("tenantA", "tenantB", "documents", imtauth::COAL_ADMIN));
}


void CCrossOrgGrantTest::testHasAccess_ExpiredDenied()
{
	m_managerPtr->CreateGrant("tenantA", "tenantB", "rel-1", imtauth::COAL_ADMIN,
				QString(), QByteArray(), QString(), "2000-01-01T00:00:00.000Z");
	QVERIFY(!m_managerPtr->HasAccess("tenantA", "tenantB", "documents", imtauth::COAL_READ));
}


void CCrossOrgGrantTest::testHasAccess_ScopedGrant()
{
	m_managerPtr->CreateGrant("tenantA", "tenantB", "rel-1", imtauth::COAL_WRITE, "documents");

	// Scoped grant only applies to its resource scope.
	QVERIFY(m_managerPtr->HasAccess("tenantA", "tenantB", "documents", imtauth::COAL_READ));
	QVERIFY(!m_managerPtr->HasAccess("tenantA", "tenantB", "billing", imtauth::COAL_READ));
}


void CCrossOrgGrantTest::testHasAccess_WrongSourceDenied()
{
	m_managerPtr->CreateGrant("tenantA", "tenantB", "rel-1", imtauth::COAL_ADMIN);
	QVERIFY(!m_managerPtr->HasAccess("tenantC", "tenantB", "documents", imtauth::COAL_READ));
}
