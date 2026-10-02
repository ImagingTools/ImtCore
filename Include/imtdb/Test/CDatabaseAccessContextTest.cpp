// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CDatabaseAccessContextTest.h"


// Qt includes
#include <QtCore/QThread>

// ImtCore includes
#include <imtdb/CDatabaseAccessContextComp.h>
#include <imtdb/CSystemDatabaseAccessContextComp.h>


void CDatabaseAccessContextTest::NoContextByDefaultTest()
{
	imtdb::CDatabaseAccessContextComp accessContext;

	QCOMPARE(accessContext.GetAccessMode(), imtdb::IDatabaseAccessContext::AM_NONE);
	QVERIFY(accessContext.GetTenantId().isEmpty());
	QVERIFY(accessContext.GetUserId().isEmpty());
}


void CDatabaseAccessContextTest::TenantContextTest()
{
	imtdb::CDatabaseAccessContextComp accessContext;

	accessContext.SetTenantAccessContext("TenantA", "User1");
	QCOMPARE(accessContext.GetAccessMode(), imtdb::IDatabaseAccessContext::AM_TENANT);
	QCOMPARE(accessContext.GetTenantId(), QByteArray("TenantA"));
	QCOMPARE(accessContext.GetUserId(), QByteArray("User1"));

	accessContext.SetTenantAccessContext("TenantB", "User2");
	QCOMPARE(accessContext.GetTenantId(), QByteArray("TenantB"));
	QCOMPARE(accessContext.GetUserId(), QByteArray("User2"));

	// A user without selected tenant still has a tenant access context (user-owned rows only)
	accessContext.SetTenantAccessContext(QByteArray(), "User3");
	QCOMPARE(accessContext.GetAccessMode(), imtdb::IDatabaseAccessContext::AM_TENANT);
	QVERIFY(accessContext.GetTenantId().isEmpty());
	QCOMPARE(accessContext.GetUserId(), QByteArray("User3"));
}


void CDatabaseAccessContextTest::ResetContextTest()
{
	imtdb::CDatabaseAccessContextComp accessContext;

	accessContext.SetTenantAccessContext("TenantA", "User1");
	accessContext.ResetAccessContext();

	QCOMPARE(accessContext.GetAccessMode(), imtdb::IDatabaseAccessContext::AM_NONE);
	QVERIFY(accessContext.GetTenantId().isEmpty());
	QVERIFY(accessContext.GetUserId().isEmpty());
}


void CDatabaseAccessContextTest::ContextIsPerThreadTest()
{
	imtdb::CDatabaseAccessContextComp accessContext;

	accessContext.SetTenantAccessContext("TenantA", "User1");

	imtdb::IDatabaseAccessContext::AccessMode otherThreadMode = imtdb::IDatabaseAccessContext::AM_TENANT;
	QByteArray otherThreadTenantId;
	QThread* threadPtr = QThread::create([&accessContext, &otherThreadMode, &otherThreadTenantId](){
		otherThreadMode = accessContext.GetAccessMode();

		accessContext.SetTenantAccessContext("TenantB", "User2");
		otherThreadTenantId = accessContext.GetTenantId();
		accessContext.ResetAccessContext();
	});

	threadPtr->start();
	threadPtr->wait();
	delete threadPtr;

	// The context of a thread is neither visible to nor changed by another thread
	QCOMPARE(otherThreadMode, imtdb::IDatabaseAccessContext::AM_NONE);
	QCOMPARE(otherThreadTenantId, QByteArray("TenantB"));
	QCOMPARE(accessContext.GetAccessMode(), imtdb::IDatabaseAccessContext::AM_TENANT);
	QCOMPARE(accessContext.GetTenantId(), QByteArray("TenantA"));
}


void CDatabaseAccessContextTest::SystemContextTest()
{
	imtdb::CSystemDatabaseAccessContextComp accessContext;

	QCOMPARE(accessContext.GetAccessMode(), imtdb::IDatabaseAccessContext::AM_SYSTEM);
	QVERIFY(accessContext.GetTenantId().isEmpty());
	QVERIFY(accessContext.GetUserId().isEmpty());
}


I_ADD_TEST(CDatabaseAccessContextTest);


