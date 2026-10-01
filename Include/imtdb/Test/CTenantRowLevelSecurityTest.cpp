// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTenantRowLevelSecurityTest.h"


// Qt includes
#include <QtCore/QUuid>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// ImtCore includes
#include <imtdb/CTenantRlsPolicyBuilder.h>


namespace
{

const QByteArray s_tenantA = QByteArrayLiteral("11111111-1111-1111-1111-111111111111");
const QByteArray s_tenantB = QByteArrayLiteral("22222222-2222-2222-2222-222222222222");
const QByteArray s_tenantC = QByteArrayLiteral("33333333-3333-3333-3333-333333333333");
const QByteArray s_user1 = QByteArrayLiteral("aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa");
const QByteArray s_user2 = QByteArrayLiteral("bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbbbbbb");
const QByteArray s_documentA = QByteArrayLiteral("d0000000-0000-0000-0000-00000000000a");
const QByteArray s_documentB = QByteArrayLiteral("d0000000-0000-0000-0000-00000000000b");
const QByteArray s_documentGlobal = QByteArrayLiteral("d0000000-0000-0000-0000-0000000000ff");

} // namespace


void CTenantRowLevelSecurityTest::initTestCase()
{
	const QString hostName = qEnvironmentVariable("IMT_TEST_POSTGRES_HOST");
	if (hostName.isEmpty()){
		m_skipReason = QStringLiteral("IMT_TEST_POSTGRES_HOST is not set, PostgreSQL RLS integration tests are skipped");

		return;
	}

	if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QPSQL"))){
		m_skipReason = QStringLiteral("QPSQL driver is not available");

		return;
	}

	const int port = qEnvironmentVariableIsSet("IMT_TEST_POSTGRES_PORT") ? qEnvironmentVariableIntValue("IMT_TEST_POSTGRES_PORT") : 5432;
	QString databaseName = qEnvironmentVariable("IMT_TEST_POSTGRES_DATABASE");
	if (databaseName.isEmpty()){
		databaseName = QStringLiteral("postgres");
	}

	const QByteArray uniqueSuffix = QUuid::createUuid().toByteArray(QUuid::Id128).left(12);
	m_roleName = QByteArrayLiteral("imt_rls_test_") + uniqueSuffix;
	m_schemaName = QByteArrayLiteral("imt_rls_test_") + uniqueSuffix;
	m_adminConnectionName = QStringLiteral("RlsTestAdmin");
	m_appConnectionName = QStringLiteral("RlsTestApp");

	{
		QSqlDatabase adminDatabase = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"), m_adminConnectionName);
		adminDatabase.setHostName(hostName);
		adminDatabase.setPort(port);
		adminDatabase.setDatabaseName(databaseName);
		adminDatabase.setUserName(qEnvironmentVariable("IMT_TEST_POSTGRES_USER"));
		adminDatabase.setPassword(qEnvironmentVariable("IMT_TEST_POSTGRES_PASSWORD"));
		if (!adminDatabase.open()){
			m_skipReason = QStringLiteral("PostgreSQL server could not be connected: %1").arg(adminDatabase.lastError().text());

			return;
		}
	}

	// Application role: neither superuser nor BYPASSRLS, otherwise RLS would not be applied at all.
	const QByteArray appPassword = QUuid::createUuid().toByteArray(QUuid::Id128);
	QVERIFY(ExecuteAdminQuery("CREATE ROLE " + m_roleName + " LOGIN NOSUPERUSER NOBYPASSRLS PASSWORD '" + appPassword + "'"));
	QVERIFY(ExecuteAdminQuery("CREATE SCHEMA " + m_schemaName + " AUTHORIZATION " + m_roleName));

	{
		QSqlDatabase appDatabase = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"), m_appConnectionName);
		appDatabase.setHostName(hostName);
		appDatabase.setPort(port);
		appDatabase.setDatabaseName(databaseName);
		appDatabase.setUserName(m_roleName);
		appDatabase.setPassword(appPassword);
		QVERIFY2(appDatabase.open(), qPrintable(appDatabase.lastError().text()));
	}

	// The application role owns the tables (FORCE ROW LEVEL SECURITY applies the policies to the owner too)
	const QByteArray schema = m_schemaName;
	QVERIFY(ExecuteAppQuery("CREATE TABLE " + schema + R"(."TenantMemberships" ("Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(), "UserId" UUID NOT NULL, "TenantId" UUID NOT NULL))"));
	QVERIFY(ExecuteAppQuery("CREATE TABLE " + schema + R"(."Contracts" ("Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(), "SourceTenantId" UUID NOT NULL, "TargetTenantId" UUID NOT NULL))"));
	QVERIFY(ExecuteAppQuery("CREATE TABLE " + schema + R"(."AuditRecords" ("Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(), "TenantId" TEXT NOT NULL))"));
	QVERIFY(ExecuteAppQuery("CREATE TABLE " + schema + R"(."Products" ("Id" UUID PRIMARY KEY DEFAULT gen_random_uuid(), "DocumentId" UUID NOT NULL, "Name" TEXT))"));
	QVERIFY(ExecuteAppQuery("CREATE TABLE " + schema + R"(."TenantEntityBindings" ("Id" TEXT PRIMARY KEY, "TenantId" TEXT NOT NULL, "EntityType" TEXT NOT NULL, "EntityId" TEXT NOT NULL, UNIQUE ("TenantId", "EntityType", "EntityId")))"));

	imtdb::CTenantRlsPolicyBuilder::TenantOwnedTableInfo tableInfo;
	QVERIFY(imtdb::CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec(schema + ".TenantMemberships:TenantId:UserId", tableInfo));
	QVERIFY(ExecuteAppQuery(imtdb::CTenantRlsPolicyBuilder::CreateTenantOwnedTablePolicyQuery(tableInfo, true)));
	QVERIFY(imtdb::CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec(schema + ".Contracts:SourceTenantId,TargetTenantId", tableInfo));
	QVERIFY(ExecuteAppQuery(imtdb::CTenantRlsPolicyBuilder::CreateTenantOwnedTablePolicyQuery(tableInfo, true)));
	QVERIFY(imtdb::CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec(schema + ".AuditRecords:TenantId", tableInfo));
	QVERIFY(ExecuteAppQuery(imtdb::CTenantRlsPolicyBuilder::CreateTenantOwnedTablePolicyQuery(tableInfo, false)));
	QVERIFY(ExecuteAppQuery(imtdb::CTenantRlsPolicyBuilder::CreateTenantBindingsTablePolicyQuery(schema, true)));
	QVERIFY(ExecuteAppQuery(imtdb::CTenantRlsPolicyBuilder::CreateBindingScopedTablePolicyQuery(schema, "Products", true)));

	// Seed test data as trusted system operation
	QVERIFY(SetContext(QByteArray(), QByteArray(), true));
	QVERIFY(ExecuteAppQuery("INSERT INTO " + schema + R"(."TenantMemberships" ("UserId", "TenantId") VALUES )"
				"('" + s_user1 + "', '" + s_tenantA + "'), ('" + s_user2 + "', '" + s_tenantB + "'), ('" + s_user2 + "', '" + s_tenantA + "')"));
	QVERIFY(ExecuteAppQuery("INSERT INTO " + schema + R"(."Contracts" ("SourceTenantId", "TargetTenantId") VALUES )"
				"('" + s_tenantA + "', '" + s_tenantB + "'), ('" + s_tenantB + "', '" + s_tenantC + "')"));
	QVERIFY(ExecuteAppQuery("INSERT INTO " + schema + R"(."Products" ("DocumentId", "Name") VALUES )"
				"('" + s_documentA + "', 'ProductA'), ('" + s_documentB + "', 'ProductB'), ('" + s_documentGlobal + "', 'ProductGlobal')"));
	QVERIFY(ExecuteAppQuery("INSERT INTO " + schema + R"(."TenantEntityBindings" ("Id", "TenantId", "EntityType", "EntityId") VALUES )"
				"('1', '" + s_tenantA + "', 'Products', '" + s_documentA + "'), ('2', '" + s_tenantB + "', 'Products', '" + s_documentB + "')"));

	// System context is not allowed for AuditRecords, data must be written in the tenant context
	QVERIFY(SetContext(s_tenantA, QByteArray(), false));
	QVERIFY(ExecuteAppQuery("INSERT INTO " + schema + R"(."AuditRecords" ("TenantId") VALUES (')" + s_tenantA + "')"));
	QVERIFY(SetContext(s_tenantB, QByteArray(), false));
	QVERIFY(ExecuteAppQuery("INSERT INTO " + schema + R"(."AuditRecords" ("TenantId") VALUES (')" + s_tenantB + "')"));
}


void CTenantRowLevelSecurityTest::init()
{
	if (!m_skipReason.isEmpty()){
		QSKIP(qPrintable(m_skipReason));
	}

	QVERIFY(SetContext(QByteArray(), QByteArray(), false));
}


void CTenantRowLevelSecurityTest::cleanupTestCase()
{
	if (m_appConnectionName.isEmpty()){
		return;
	}

	if (QSqlDatabase::contains(m_appConnectionName)){
		QSqlDatabase::database(m_appConnectionName).close();
		QSqlDatabase::removeDatabase(m_appConnectionName);
	}

	if (QSqlDatabase::contains(m_adminConnectionName)){
		if (QSqlDatabase::database(m_adminConnectionName).isOpen()){
			ExecuteAdminQuery("DROP SCHEMA IF EXISTS " + m_schemaName + " CASCADE");
			ExecuteAdminQuery("DROP ROLE IF EXISTS " + m_roleName);
		}

		QSqlDatabase::database(m_adminConnectionName).close();
		QSqlDatabase::removeDatabase(m_adminConnectionName);
	}
}


void CTenantRowLevelSecurityTest::TenantCannotReadOtherTenantDataTest()
{
	QVERIFY(SetContext(s_tenantA, s_user1, false));
	QCOMPARE(GetRowCount("TenantMemberships"), 2);
	QCOMPARE(GetRowCount("AuditRecords"), 1);

	QVERIFY(SetContext(s_tenantB, s_user1, false));
	QCOMPARE(GetRowCount("AuditRecords"), 1);

	// User1 is not member of B, but can see own membership in A (user access policy) and the one of B
	QCOMPARE(GetRowCount("TenantMemberships"), 2);

	QVERIFY(SetContext(s_tenantC, QByteArray(), false));
	QCOMPARE(GetRowCount("TenantMemberships"), 0);
	QCOMPARE(GetRowCount("AuditRecords"), 0);
}


void CTenantRowLevelSecurityTest::TenantCannotModifyOtherTenantDataTest()
{
	QVERIFY(SetContext(s_tenantA, QByteArray(), false));

	QSqlQuery updateQuery(QSqlDatabase::database(m_appConnectionName));
	QVERIFY(updateQuery.exec(QString(QByteArray("UPDATE " + m_schemaName + R"(."AuditRecords" SET "TenantId" = "TenantId" WHERE "TenantId" = ')" + s_tenantB + "'"))));
	QCOMPARE(updateQuery.numRowsAffected(), 0);

	QSqlQuery deleteQuery(QSqlDatabase::database(m_appConnectionName));
	QVERIFY(deleteQuery.exec(QString(QByteArray("DELETE FROM " + m_schemaName + R"(."TenantMemberships" WHERE "TenantId" = ')" + s_tenantB + "'"))));
	QCOMPARE(deleteQuery.numRowsAffected(), 0);

	// Rows can't be created for or moved to another tenant
	QSqlError sqlError;
	QVERIFY(!ExecuteAppQuery("INSERT INTO " + m_schemaName + R"(."AuditRecords" ("TenantId") VALUES (')" + s_tenantB + "')", &sqlError));
	QVERIFY(sqlError.text().contains(QStringLiteral("row-level security")));
	QVERIFY(!ExecuteAppQuery("UPDATE " + m_schemaName + R"(."AuditRecords" SET "TenantId" = ')" + s_tenantB + "'"));

	QVERIFY(SetContext(s_tenantB, QByteArray(), false));
	QCOMPARE(GetRowCount("AuditRecords"), 1);
	QCOMPARE(GetRowCount("TenantMemberships"), 1);
}


void CTenantRowLevelSecurityTest::NoTenantContextDeniesTenantDataTest()
{
	QCOMPARE(GetRowCount("TenantMemberships"), 0);
	QCOMPARE(GetRowCount("Contracts"), 0);
	QCOMPARE(GetRowCount("AuditRecords"), 0);
	QCOMPARE(GetRowCount("TenantEntityBindings"), 2);

	// Only global (not bound) documents are visible
	QCOMPARE(GetProductNames(), QStringList() << QStringLiteral("ProductGlobal"));

	QVERIFY(!ExecuteAppQuery("INSERT INTO " + m_schemaName + R"(."AuditRecords" ("TenantId") VALUES (''))"));
	QVERIFY(!ExecuteAppQuery("INSERT INTO " + m_schemaName + R"(."TenantEntityBindings" ("Id", "TenantId", "EntityType", "EntityId") VALUES ('3', '', 'Products', ')" + s_documentGlobal + "')"));
}


void CTenantRowLevelSecurityTest::UserCanReadOwnRowsOnlyTest()
{
	QVERIFY(SetContext(QByteArray(), s_user2, false));
	QCOMPARE(GetRowCount("TenantMemberships"), 2);

	// Read-only access: own rows of a tenant different from the current one can't be modified
	QSqlQuery deleteQuery(QSqlDatabase::database(m_appConnectionName));
	QVERIFY(deleteQuery.exec(QString(QByteArray("DELETE FROM " + m_schemaName + R"(."TenantMemberships")"))));
	QCOMPARE(deleteQuery.numRowsAffected(), 0);

	QVERIFY(SetContext(QByteArray(), s_user1, false));
	QCOMPARE(GetRowCount("TenantMemberships"), 1);
}


void CTenantRowLevelSecurityTest::CrossTenantTableTest()
{
	QVERIFY(SetContext(s_tenantA, QByteArray(), false));
	QCOMPARE(GetRowCount("Contracts"), 1);

	QVERIFY(SetContext(s_tenantB, QByteArray(), false));
	QCOMPARE(GetRowCount("Contracts"), 2);

	QVERIFY(SetContext(s_tenantC, QByteArray(), false));
	QCOMPARE(GetRowCount("Contracts"), 1);
}


void CTenantRowLevelSecurityTest::BindingScopedCollectionTest()
{
	QVERIFY(SetContext(s_tenantA, QByteArray(), false));
	QCOMPARE(GetProductNames(), QStringList() << QStringLiteral("ProductA") << QStringLiteral("ProductGlobal"));

	// Document of another tenant can't be changed
	QSqlQuery updateQuery(QSqlDatabase::database(m_appConnectionName));
	QVERIFY(updateQuery.exec(QString(QByteArray("UPDATE " + m_schemaName + R"(."Products" SET "Name" = 'Changed' WHERE "DocumentId" = ')" + s_documentB + "'"))));
	QCOMPARE(updateQuery.numRowsAffected(), 0);

	// A tenant can't bind documents to another tenant
	QVERIFY(!ExecuteAppQuery("INSERT INTO " + m_schemaName + R"(."TenantEntityBindings" ("Id", "TenantId", "EntityType", "EntityId") VALUES ('3', ')" + s_tenantB + "', 'Products', '" + s_documentGlobal + "')"));

	// A tenant can't remove bindings of another tenant (it would make the document global)
	QSqlQuery deleteQuery(QSqlDatabase::database(m_appConnectionName));
	QVERIFY(deleteQuery.exec(QString(QByteArray("DELETE FROM " + m_schemaName + R"(."TenantEntityBindings" WHERE "EntityId" = ')" + s_documentB + "'"))));
	QCOMPARE(deleteQuery.numRowsAffected(), 0);

	QVERIFY(SetContext(s_tenantB, QByteArray(), false));
	QCOMPARE(GetProductNames(), QStringList() << QStringLiteral("ProductB") << QStringLiteral("ProductGlobal"));
}


void CTenantRowLevelSecurityTest::SystemContextTest()
{
	QVERIFY(SetContext(QByteArray(), QByteArray(), true));

	QCOMPARE(GetRowCount("TenantMemberships"), 3);
	QCOMPARE(GetRowCount("Contracts"), 2);
	QCOMPARE(GetProductNames(), QStringList() << QStringLiteral("ProductA") << QStringLiteral("ProductB") << QStringLiteral("ProductGlobal"));
}


void CTenantRowLevelSecurityTest::SystemContextDisabledTest()
{
	// AuditRecords policy is created without system context support
	QVERIFY(SetContext(QByteArray(), QByteArray(), true));
	QCOMPARE(GetRowCount("AuditRecords"), 0);
}


void CTenantRowLevelSecurityTest::MaliciousContextValueTest()
{
	// Context values are bound parameters and never interpreted as SQL
	QVERIFY(SetContext(QByteArrayLiteral("x' OR '1'='1"), QByteArrayLiteral("'; DROP TABLE x; --"), false));
	QCOMPARE(GetRowCount("TenantMemberships"), 0);
	QCOMPARE(GetRowCount("AuditRecords"), 0);
}


// private methods

bool CTenantRowLevelSecurityTest::ExecuteAdminQuery(const QByteArray& query)
{
	QSqlQuery sqlQuery(QSqlDatabase::database(m_adminConnectionName));
	if (!sqlQuery.exec(QString(query))){
		qWarning() << sqlQuery.lastError().text();

		return false;
	}

	return true;
}


bool CTenantRowLevelSecurityTest::ExecuteAppQuery(const QByteArray& query, QSqlError* errorPtr)
{
	QSqlQuery sqlQuery(QSqlDatabase::database(m_appConnectionName));
	if (!sqlQuery.exec(QString(query))){
		if (errorPtr != nullptr){
			*errorPtr = sqlQuery.lastError();
		}

		return false;
	}

	return true;
}


bool CTenantRowLevelSecurityTest::SetContext(const QByteArray& tenantId, const QByteArray& userId, bool isSystemContext)
{
	// Same query as used by imtdb::CDatabaseEngineComp
	QSqlQuery sqlQuery(QSqlDatabase::database(m_appConnectionName));
	sqlQuery.prepare(QString(imtdb::CTenantRlsPolicyBuilder::CreateContextSyncQuery()));
	sqlQuery.bindValue(QStringLiteral(":TenantId"), QString(tenantId));
	sqlQuery.bindValue(QStringLiteral(":UserId"), QString(userId));
	sqlQuery.bindValue(QStringLiteral(":SystemContext"), isSystemContext ? QStringLiteral("on") : QStringLiteral("off"));

	return sqlQuery.exec();
}


int CTenantRowLevelSecurityTest::GetRowCount(const QByteArray& tableName)
{
	QSqlQuery sqlQuery(QSqlDatabase::database(m_appConnectionName));
	if (!sqlQuery.exec(QString(QByteArray("SELECT COUNT(*) FROM " + m_schemaName + ".\"" + tableName + '"'))) || !sqlQuery.next()){
		return -1;
	}

	return sqlQuery.value(0).toInt();
}


QStringList CTenantRowLevelSecurityTest::GetProductNames()
{
	QStringList retVal;

	QSqlQuery sqlQuery(QSqlDatabase::database(m_appConnectionName));
	if (!sqlQuery.exec(QString(QByteArray("SELECT \"Name\" FROM " + m_schemaName + R"(."Products" ORDER BY "Name")")))){
		return retVal;
	}

	while (sqlQuery.next()){
		retVal.append(sqlQuery.value(0).toString());
	}

	return retVal;
}


I_ADD_TEST(CTenantRowLevelSecurityTest);


