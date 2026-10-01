// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTenantSecurityContextTest.h"


// Qt includes
#include <QtCore/QThread>

// ImtCore includes
#include <imtbase/CTenantSecurityContextScope.h>


void CTenantSecurityContextTest::DefaultContextIsUndefinedTest()
{
	const imtbase::CTenantSecurityContext context = imtbase::CTenantSecurityContext::GetCurrentContext();

	QVERIFY(!context.IsDefined());
	QVERIFY(!context.IsSystemContext());
	QVERIFY(context.GetTenantId().isEmpty());
	QVERIFY(context.GetUserId().isEmpty());
}


void CTenantSecurityContextTest::TenantContextTest()
{
	{
		imtbase::CTenantSecurityContextScope scope(imtbase::CTenantSecurityContext("TenantA", "User1"));

		const imtbase::CTenantSecurityContext context = imtbase::CTenantSecurityContext::GetCurrentContext();
		QVERIFY(context.IsDefined());
		QVERIFY(!context.IsSystemContext());
		QCOMPARE(context.GetTenantId(), QByteArray("TenantA"));
		QCOMPARE(context.GetUserId(), QByteArray("User1"));
	}

	QVERIFY(!imtbase::CTenantSecurityContext::GetCurrentContext().IsDefined());
}


void CTenantSecurityContextTest::SystemContextTest()
{
	{
		imtbase::CTenantSecurityContextScope scope(imtbase::CTenantSecurityContext::CreateSystemContext());

		const imtbase::CTenantSecurityContext context = imtbase::CTenantSecurityContext::GetCurrentContext();
		QVERIFY(context.IsDefined());
		QVERIFY(context.IsSystemContext());
		QVERIFY(context.GetTenantId().isEmpty());
	}

	QVERIFY(!imtbase::CTenantSecurityContext::GetCurrentContext().IsSystemContext());
}


void CTenantSecurityContextTest::NestedScopesRestorePreviousContextTest()
{
	const imtbase::CTenantSecurityContext tenantAContext("TenantA", "User1");
	const imtbase::CTenantSecurityContext tenantBContext("TenantB", "User2");

	{
		imtbase::CTenantSecurityContextScope outerScope(tenantAContext);
		{
			imtbase::CTenantSecurityContextScope innerScope(tenantBContext);
			QVERIFY(imtbase::CTenantSecurityContext::GetCurrentContext() == tenantBContext);
			{
				imtbase::CTenantSecurityContextScope systemScope(imtbase::CTenantSecurityContext::CreateSystemContext());
				QVERIFY(imtbase::CTenantSecurityContext::GetCurrentContext().IsSystemContext());
			}
			QVERIFY(imtbase::CTenantSecurityContext::GetCurrentContext() == tenantBContext);
		}
		QVERIFY(imtbase::CTenantSecurityContext::GetCurrentContext() == tenantAContext);
	}

	QVERIFY(imtbase::CTenantSecurityContext::GetCurrentContext() == imtbase::CTenantSecurityContext());
}


void CTenantSecurityContextTest::ContextIsThreadLocalTest()
{
	imtbase::CTenantSecurityContextScope scope(imtbase::CTenantSecurityContext("TenantA", "User1"));

	bool isOtherThreadContextDefined = true;
	QThread* threadPtr = QThread::create([&isOtherThreadContextDefined](){
		isOtherThreadContextDefined = imtbase::CTenantSecurityContext::GetCurrentContext().IsDefined();
	});

	threadPtr->start();
	threadPtr->wait();
	delete threadPtr;

	// Tenant context must not leak to other threads: without explicit context the access must be fail-closed.
	QVERIFY(!isOtherThreadContextDefined);
	QCOMPARE(imtbase::CTenantSecurityContext::GetCurrentContext().GetTenantId(), QByteArray("TenantA"));
}


I_ADD_TEST(CTenantSecurityContextTest);


