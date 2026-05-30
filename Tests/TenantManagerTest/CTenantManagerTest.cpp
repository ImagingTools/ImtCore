// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTenantManagerTest.h"


void CTenantManagerTest::init()
{
	m_managerPtr = new imtauth::CMockTenantManager();
}


void CTenantManagerTest::cleanup()
{
	delete m_managerPtr;
	m_managerPtr = nullptr;
}


void CTenantManagerTest::testCreateTenant_Success()
{
	QByteArray id = m_managerPtr->CreateTenant("TestTenant", "A description", "owner1");

	QVERIFY(!id.isEmpty());
	auto tenant = m_managerPtr->GetTenant(id);
	QVERIFY(tenant != nullptr);
	QCOMPARE(tenant->name, QString("TestTenant"));
	QCOMPARE(tenant->description, QString("A description"));
	QCOMPARE(tenant->ownerId, QByteArray("owner1"));
	QVERIFY(tenant->isActive);
	QVERIFY(tenant->createdAt.isValid());
	QVERIFY(tenant->updatedAt.isValid());
}


void CTenantManagerTest::testCreateTenant_EmptyName_Fails()
{
	QByteArray id = m_managerPtr->CreateTenant("", "desc", "owner1");
	QVERIFY(id.isEmpty());
	QCOMPARE(m_managerPtr->GetTenantIds().size(), 0);
}


void CTenantManagerTest::testGetTenant_Existing()
{
	QByteArray id = m_managerPtr->CreateTenant("Existing", "desc", "owner1");
	auto tenant = m_managerPtr->GetTenant(id);

	QVERIFY(tenant != nullptr);
	QCOMPARE(tenant->id, id);
	QCOMPARE(tenant->name, QString("Existing"));
}


void CTenantManagerTest::testGetTenant_NonExisting_ReturnsNull()
{
	QByteArray fakeId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	auto tenant = m_managerPtr->GetTenant(fakeId);
	QVERIFY(tenant == nullptr);
}


void CTenantManagerTest::testGetTenantIds_Empty()
{
	QByteArrayList ids = m_managerPtr->GetTenantIds();
	QVERIFY(ids.isEmpty());
}


void CTenantManagerTest::testGetTenantIds_Multiple()
{
	m_managerPtr->CreateTenant("T1", "", "owner1");
	m_managerPtr->CreateTenant("T2", "", "owner2");
	m_managerPtr->CreateTenant("T3", "", "owner3");

	QByteArrayList ids = m_managerPtr->GetTenantIds();
	QCOMPARE(ids.size(), 3);
}


void CTenantManagerTest::testRemoveTenant_Existing()
{
	QByteArray id = m_managerPtr->CreateTenant("ToRemove", "", "owner1");
	QVERIFY(m_managerPtr->RemoveTenant(id));
	QVERIFY(m_managerPtr->GetTenant(id) == nullptr);
	QCOMPARE(m_managerPtr->GetTenantIds().size(), 0);
}


void CTenantManagerTest::testRemoveTenant_NonExisting_ReturnsFalse()
{
	QByteArray fakeId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QVERIFY(!m_managerPtr->RemoveTenant(fakeId));
}


void CTenantManagerTest::testUpdateTenant_Success()
{
	QByteArray id = m_managerPtr->CreateTenant("Original", "desc1", "owner1");
	QDateTime beforeUpdate = m_managerPtr->GetTenant(id)->updatedAt;

	QTest::qWait(5); // ensure timestamp difference
	QVERIFY(m_managerPtr->UpdateTenant(id, "Updated", "desc2"));

	auto tenant = m_managerPtr->GetTenant(id);
	QCOMPARE(tenant->name, QString("Updated"));
	QCOMPARE(tenant->description, QString("desc2"));
	QVERIFY(tenant->updatedAt >= beforeUpdate);
}


void CTenantManagerTest::testUpdateTenant_NonExisting_ReturnsFalse()
{
	QByteArray fakeId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QVERIFY(!m_managerPtr->UpdateTenant(fakeId, "Name", "desc"));
}


void CTenantManagerTest::testUpdateTenant_EmptyName_Fails()
{
	QByteArray id = m_managerPtr->CreateTenant("Original", "desc", "owner1");
	QVERIFY(!m_managerPtr->UpdateTenant(id, "", "desc2"));

	auto tenant = m_managerPtr->GetTenant(id);
	QCOMPARE(tenant->name, QString("Original"));
}


void CTenantManagerTest::testSetTenantActive_Activate()
{
	QByteArray id = m_managerPtr->CreateTenant("T", "", "owner1");
	m_managerPtr->SetTenantActive(id, false);
	QVERIFY(m_managerPtr->SetTenantActive(id, true));

	auto tenant = m_managerPtr->GetTenant(id);
	QVERIFY(tenant->isActive);
}


void CTenantManagerTest::testSetTenantActive_Deactivate()
{
	QByteArray id = m_managerPtr->CreateTenant("T", "", "owner1");
	QVERIFY(m_managerPtr->SetTenantActive(id, false));

	auto tenant = m_managerPtr->GetTenant(id);
	QVERIFY(!tenant->isActive);
}


void CTenantManagerTest::testSetTenantActive_NonExisting_ReturnsFalse()
{
	QByteArray fakeId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	QVERIFY(!m_managerPtr->SetTenantActive(fakeId, true));
}


void CTenantManagerTest::testCreateTenant_DuplicateNameAllowed()
{
	QByteArray id1 = m_managerPtr->CreateTenant("SameName", "", "owner1");
	QByteArray id2 = m_managerPtr->CreateTenant("SameName", "", "owner2");

	QVERIFY(!id1.isEmpty());
	QVERIFY(!id2.isEmpty());
	QVERIFY(id1 != id2);
	QCOMPARE(m_managerPtr->GetTenantIds().size(), 2);
}


void CTenantManagerTest::testRemoveTenant_CascadesMemberships()
{
	QByteArray tenantId = m_managerPtr->CreateTenant("T", "", "owner1");

	// Add memberships manually to simulate DB state
	imtauth::MembershipData m1;
	m1.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	m1.userId = "user1";
	m1.tenantId = tenantId;
	m1.role = "member";
	m_managerPtr->m_memberships[m1.id] = m1;

	imtauth::MembershipData m2;
	m2.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	m2.userId = "user2";
	m2.tenantId = tenantId;
	m2.role = "admin";
	m_managerPtr->m_memberships[m2.id] = m2;

	// Add a membership for another tenant (should not be removed)
	imtauth::MembershipData m3;
	m3.id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	m3.userId = "user3";
	m3.tenantId = "other-tenant-id";
	m3.role = "member";
	m_managerPtr->m_memberships[m3.id] = m3;

	QCOMPARE(m_managerPtr->m_memberships.size(), 3);
	QVERIFY(m_managerPtr->RemoveTenant(tenantId));
	QCOMPARE(m_managerPtr->m_memberships.size(), 1);
	QVERIFY(m_managerPtr->m_memberships.contains(m3.id));
}


void CTenantManagerTest::testRemoveTenant_CascadesPermissions()
{
	QByteArray tenantId = m_managerPtr->CreateTenant("T", "", "owner1");
	QByteArray otherTenantId = "other-tenant-id";

	imtauth::PermissionData p1{tenantId, "product1", "read"};
	imtauth::PermissionData p2{tenantId, "product1", "write"};
	imtauth::PermissionData p3{otherTenantId, "product2", "read"};
	m_managerPtr->m_permissions.append(p1);
	m_managerPtr->m_permissions.append(p2);
	m_managerPtr->m_permissions.append(p3);

	QCOMPARE(m_managerPtr->m_permissions.size(), 3);
	QVERIFY(m_managerPtr->RemoveTenant(tenantId));
	QCOMPARE(m_managerPtr->m_permissions.size(), 1);
	QCOMPARE(m_managerPtr->m_permissions[0].tenantId, otherTenantId);
}
