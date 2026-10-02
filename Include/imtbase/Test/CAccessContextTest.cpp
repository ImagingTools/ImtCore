// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CAccessContextTest.h"


// Qt includes
#include <QtCore/QThread>

// ImtCore includes
#include <imtbase/CAccessContextComp.h>
#include <imtbase/CSystemAccessContextComp.h>


void CAccessContextTest::NoContextByDefaultTest()
{
	imtbase::CAccessContextComp accessContext;

	QCOMPARE(accessContext.GetAccessMode(), imtbase::IAccessContext::AM_NONE);
	QVERIFY(accessContext.GetTenantId().isEmpty());
	QVERIFY(accessContext.GetUserId().isEmpty());
}


void CAccessContextTest::TenantContextTest()
{
	imtbase::CAccessContextComp accessContext;

	accessContext.SetTenantAccessContext("TenantA", "User1");
	QCOMPARE(accessContext.GetAccessMode(), imtbase::IAccessContext::AM_TENANT);
	QCOMPARE(accessContext.GetTenantId(), QByteArray("TenantA"));
	QCOMPARE(accessContext.GetUserId(), QByteArray("User1"));

	accessContext.SetTenantAccessContext("TenantB", "User2");
	QCOMPARE(accessContext.GetTenantId(), QByteArray("TenantB"));
	QCOMPARE(accessContext.GetUserId(), QByteArray("User2"));

	// A user without selected tenant still has a tenant access context (user-owned rows only)
	accessContext.SetTenantAccessContext(QByteArray(), "User3");
	QCOMPARE(accessContext.GetAccessMode(), imtbase::IAccessContext::AM_TENANT);
	QVERIFY(accessContext.GetTenantId().isEmpty());
	QCOMPARE(accessContext.GetUserId(), QByteArray("User3"));
}


void CAccessContextTest::ResetContextTest()
{
	imtbase::CAccessContextComp accessContext;

	accessContext.SetTenantAccessContext("TenantA", "User1");
	accessContext.ResetAccessContext();

	QCOMPARE(accessContext.GetAccessMode(), imtbase::IAccessContext::AM_NONE);
	QVERIFY(accessContext.GetTenantId().isEmpty());
	QVERIFY(accessContext.GetUserId().isEmpty());
}


void CAccessContextTest::ContextIsPerThreadTest()
{
	imtbase::CAccessContextComp accessContext;

	accessContext.SetTenantAccessContext("TenantA", "User1");

	imtbase::IAccessContext::AccessMode otherThreadMode = imtbase::IAccessContext::AM_TENANT;
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
	QCOMPARE(otherThreadMode, imtbase::IAccessContext::AM_NONE);
	QCOMPARE(otherThreadTenantId, QByteArray("TenantB"));
	QCOMPARE(accessContext.GetAccessMode(), imtbase::IAccessContext::AM_TENANT);
	QCOMPARE(accessContext.GetTenantId(), QByteArray("TenantA"));
}


void CAccessContextTest::SystemContextTest()
{
	imtbase::CSystemAccessContextComp accessContext;

	QCOMPARE(accessContext.GetAccessMode(), imtbase::IAccessContext::AM_SYSTEM);
	QVERIFY(accessContext.GetTenantId().isEmpty());
	QVERIFY(accessContext.GetUserId().isEmpty());
}


I_ADD_TEST(CAccessContextTest);


