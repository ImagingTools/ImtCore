// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CDataTenantTest.h"


// ImtCore includes
#include <imtgql/CGqlContext.h>
#include <imtgql/imtgql.h>


void CDataTenantTest::OrganizationSessionOwnsDataTest()
{
	imtgql::CGqlContext context;
	context.SetPrincipalKind(imtgql::IGqlContext::PK_USER);
	context.SetUserId("user-1");
	context.SetTenantId("tenant-1");

	imtbase::TenantType tenantType = imtbase::TT_USER;
	QCOMPARE(imtgql::GetDataTenantId(context, tenantType), QByteArray("tenant-1"));
	QCOMPARE(tenantType, imtbase::TT_ORGANIZATION);
}


void CDataTenantTest::UserOutsideOrganizationOwnsDataTest()
{
	imtgql::CGqlContext context;
	context.SetPrincipalKind(imtgql::IGqlContext::PK_USER);
	context.SetUserId("user-1");

	imtbase::TenantType tenantType = imtbase::TT_ORGANIZATION;
	QCOMPARE(imtgql::GetDataTenantId(context, tenantType), QByteArray("user-1"));
	QCOMPARE(tenantType, imtbase::TT_USER);
}


void CDataTenantTest::ServerOutsideOrganizationHasNoTenantTest()
{
	imtgql::CGqlContext context;
	context.SetPrincipalKind(imtgql::IGqlContext::PK_SERVICE);
	context.SetUserId("server-user");

	imtbase::TenantType tenantType = imtbase::TT_USER;
	QVERIFY(imtgql::GetDataTenantId(context, tenantType).isEmpty());
	QCOMPARE(tenantType, imtbase::TT_ORGANIZATION);
}


void CDataTenantTest::AnonymousOutsideOrganizationHasNoTenantTest()
{
	imtgql::CGqlContext context;
	context.SetUserId("user-1");

	imtbase::TenantType tenantType = imtbase::TT_USER;
	QVERIFY(imtgql::GetDataTenantId(context, tenantType).isEmpty());
	QCOMPARE(tenantType, imtbase::TT_ORGANIZATION);
}


void CDataTenantTest::ServerInOrganizationUsesOrganizationTest()
{
	imtgql::CGqlContext context;
	context.SetPrincipalKind(imtgql::IGqlContext::PK_SERVICE);
	context.SetUserId("server-user");
	context.SetTenantId("tenant-1");

	imtbase::TenantType tenantType = imtbase::TT_USER;
	QCOMPARE(imtgql::GetDataTenantId(context, tenantType), QByteArray("tenant-1"));
	QCOMPARE(tenantType, imtbase::TT_ORGANIZATION);
}


I_ADD_TEST(CDataTenantTest);


