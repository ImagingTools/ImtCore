// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTenantRlsPolicyBuilderTest.h"


// ImtCore includes
#include <imtdb/CTenantRlsPolicyBuilder.h>


void CTenantRlsPolicyBuilderTest::IsValidIdentifierTest()
{
	QVERIFY(imtdb::CTenantRlsPolicyBuilder::IsValidIdentifier("TenantMemberships"));
	QVERIFY(imtdb::CTenantRlsPolicyBuilder::IsValidIdentifier("_table_1"));

	QVERIFY(!imtdb::CTenantRlsPolicyBuilder::IsValidIdentifier(""));
	QVERIFY(!imtdb::CTenantRlsPolicyBuilder::IsValidIdentifier("1Table"));
	QVERIFY(!imtdb::CTenantRlsPolicyBuilder::IsValidIdentifier("Table\"; DROP TABLE x; --"));
	QVERIFY(!imtdb::CTenantRlsPolicyBuilder::IsValidIdentifier("Table Name"));
	QVERIFY(!imtdb::CTenantRlsPolicyBuilder::IsValidIdentifier("schema.table"));
	QVERIFY(!imtdb::CTenantRlsPolicyBuilder::IsValidIdentifier(QByteArray(64, 'a')));
}


void CTenantRlsPolicyBuilderTest::ParseTenantOwnedTableSpecTest()
{
	imtdb::CTenantRlsPolicyBuilder::TenantOwnedTableInfo info;

	QVERIFY(imtdb::CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec("TenantPermissions:TenantId", info));
	QCOMPARE(info.schema, QByteArray("public"));
	QCOMPARE(info.tableName, QByteArray("TenantPermissions"));
	QCOMPARE(info.tenantColumns, QByteArrayList() << "TenantId");
	QVERIFY(info.userColumn.isEmpty());

	QVERIFY(imtdb::CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec("auth.TenantMemberships:TenantId:UserId", info));
	QCOMPARE(info.schema, QByteArray("auth"));
	QCOMPARE(info.tableName, QByteArray("TenantMemberships"));
	QCOMPARE(info.tenantColumns, QByteArrayList() << "TenantId");
	QCOMPARE(info.userColumn, QByteArray("UserId"));

	QVERIFY(imtdb::CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec(" Contracts : SourceTenantId , TargetTenantId ", info));
	QCOMPARE(info.tableName, QByteArray("Contracts"));
	QCOMPARE(info.tenantColumns, QByteArrayList() << "SourceTenantId" << "TargetTenantId");
}


void CTenantRlsPolicyBuilderTest::ParseInvalidTenantOwnedTableSpecTest()
{
	imtdb::CTenantRlsPolicyBuilder::TenantOwnedTableInfo info;

	QVERIFY(!imtdb::CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec("", info));
	QVERIFY(!imtdb::CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec("TenantPermissions", info));
	QVERIFY(!imtdb::CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec("TenantPermissions:", info));
	QVERIFY(!imtdb::CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec("a.b.c:TenantId", info));
	QVERIFY(!imtdb::CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec("Table:TenantId:UserId:Other", info));
	QVERIFY(!imtdb::CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec("Table\":TenantId", info));
	QVERIFY(!imtdb::CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec("Table:Tenant'Id", info));
	QVERIFY(!imtdb::CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec("Table:TenantId,", info));
}


void CTenantRlsPolicyBuilderTest::TenantOwnedTablePolicyTest()
{
	imtdb::CTenantRlsPolicyBuilder::TenantOwnedTableInfo info;
	QVERIFY(imtdb::CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec("TenantMemberships:TenantId:UserId", info));

	const QByteArray query = imtdb::CTenantRlsPolicyBuilder::CreateTenantOwnedTablePolicyQuery(info, true);

	QVERIFY(query.contains(R"(ALTER TABLE "public"."TenantMemberships" ENABLE ROW LEVEL SECURITY;)"));
	QVERIFY(query.contains(R"(ALTER TABLE "public"."TenantMemberships" FORCE ROW LEVEL SECURITY;)"));
	QVERIFY(query.contains(R"(CREATE POLICY "ImtTenantIsolation" ON "public"."TenantMemberships" AS PERMISSIVE FOR ALL USING )"));
	QVERIFY(query.contains(" WITH CHECK "));
	QVERIFY(query.contains(R"("TenantId"::text = COALESCE(current_setting('imt.tenant_id', true), ''))"));

	// Fail-closed: empty tenant context never matches
	QVERIFY(query.contains(R"(COALESCE(current_setting('imt.tenant_id', true), '') <> '')"));
	QVERIFY(query.contains("imt.rls_bypass"));

	// Users can only read own rows across tenants
	QVERIFY(query.contains(R"(CREATE POLICY "ImtTenantUserAccess" ON "public"."TenantMemberships" AS PERMISSIVE FOR SELECT USING )"));
	QVERIFY(query.contains(R"("UserId"::text = COALESCE(current_setting('imt.user_id', true), ''))"));
}


void CTenantRlsPolicyBuilderTest::TenantOwnedTablePolicyWithoutSystemContextTest()
{
	imtdb::CTenantRlsPolicyBuilder::TenantOwnedTableInfo info;
	QVERIFY(imtdb::CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec("Contracts:SourceTenantId,TargetTenantId", info));

	const QByteArray query = imtdb::CTenantRlsPolicyBuilder::CreateTenantOwnedTablePolicyQuery(info, false);

	QVERIFY(!query.isEmpty());
	QVERIFY(!query.contains("imt.rls_bypass"));
	QVERIFY(!query.contains(R"(CREATE POLICY "ImtTenantUserAccess")"));
	QVERIFY(query.contains(R"("SourceTenantId"::text = )"));
	QVERIFY(query.contains(R"( OR "TargetTenantId"::text = )"));
}


void CTenantRlsPolicyBuilderTest::BindingScopedTablePolicyTest()
{
	const QByteArray query = imtdb::CTenantRlsPolicyBuilder::CreateBindingScopedTablePolicyQuery("public", "Products", true);

	QVERIFY(query.contains(R"(ALTER TABLE "public"."Products" FORCE ROW LEVEL SECURITY;)"));
	QVERIFY(query.contains(R"(CREATE POLICY "ImtTenantIsolation" ON "public"."Products" AS PERMISSIVE FOR ALL USING )"));
	QVERIFY(query.contains(R"(FROM "public"."TenantEntityBindings" imtTenantBindings)"));
	QVERIFY(query.contains(R"(imtTenantBindings."EntityType" = 'Products')"));
	QVERIFY(query.contains(R"(imtTenantBindings."EntityId" = "Products"."DocumentId"::text)"));
	QVERIFY(query.contains("NOT EXISTS ("));
}


void CTenantRlsPolicyBuilderTest::TenantBindingsTablePolicyTest()
{
	const QByteArray query = imtdb::CTenantRlsPolicyBuilder::CreateTenantBindingsTablePolicyQuery("public", false);

	QVERIFY(query.contains(R"(ALTER TABLE "public"."TenantEntityBindings" FORCE ROW LEVEL SECURITY;)"));
	QVERIFY(query.contains(R"(FOR SELECT USING (true))"));
	QVERIFY(query.contains("FOR INSERT WITH CHECK"));
	QVERIFY(query.contains("FOR UPDATE USING"));
	QVERIFY(query.contains("FOR DELETE USING"));
	QVERIFY(!query.contains("imt.rls_bypass"));
}


void CTenantRlsPolicyBuilderTest::InvalidIdentifiersProduceNoQueryTest()
{
	imtdb::CTenantRlsPolicyBuilder::TenantOwnedTableInfo info;
	info.schema = "public";
	info.tableName = "Table\"; DROP TABLE x; --";
	info.tenantColumns << "TenantId";
	QVERIFY(imtdb::CTenantRlsPolicyBuilder::CreateTenantOwnedTablePolicyQuery(info, true).isEmpty());

	info.tableName = "Table";
	info.tenantColumns.clear();
	QVERIFY(imtdb::CTenantRlsPolicyBuilder::CreateTenantOwnedTablePolicyQuery(info, true).isEmpty());

	info.tenantColumns << "TenantId";
	info.userColumn = "User'Id";
	QVERIFY(imtdb::CTenantRlsPolicyBuilder::CreateTenantOwnedTablePolicyQuery(info, true).isEmpty());

	QVERIFY(imtdb::CTenantRlsPolicyBuilder::CreateBindingScopedTablePolicyQuery("public", "Products'", true).isEmpty());
	QVERIFY(imtdb::CTenantRlsPolicyBuilder::CreateBindingScopedTablePolicyQuery("", "Products", true).isEmpty());
	QVERIFY(imtdb::CTenantRlsPolicyBuilder::CreateTenantBindingsTablePolicyQuery("pub lic", true).isEmpty());
}


void CTenantRlsPolicyBuilderTest::ContextSyncQueryTest()
{
	const QByteArray query = imtdb::CTenantRlsPolicyBuilder::CreateContextSyncQuery();

	// The context values are always passed as bound parameters
	QVERIFY(query.contains("set_config('imt.tenant_id', :TenantId, false)"));
	QVERIFY(query.contains("set_config('imt.user_id', :UserId, false)"));
	QVERIFY(query.contains("set_config('imt.rls_bypass', :SystemContext, false)"));
}


I_ADD_TEST(CTenantRlsPolicyBuilderTest);


