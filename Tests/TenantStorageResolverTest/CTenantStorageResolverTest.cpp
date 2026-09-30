// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTenantStorageResolverTest.h"

#include <itest/CStandardTestExecutor.h>

#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include <imtdb/CTenantStorageDbStore.h>
#include <imtdb/IDatabaseEngine.h>


using imtdb::CTenantStorageRegistry;
using imtdb::TenantStorageInfo;


namespace
{


/**
	Minimal in-memory SQLite implementation of IDatabaseEngine for the store tests.
*/
class CSqliteTestEngine: public imtdb::IDatabaseEngine
{
public:
	CSqliteTestEngine()
	{
		m_database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("TenantStorageResolverTest"));
		m_database.setDatabaseName(QStringLiteral(":memory:"));
		m_database.open();
	}

	~CSqliteTestEngine() override
	{
		m_database.close();
		m_database = QSqlDatabase();
		QSqlDatabase::removeDatabase(QStringLiteral("TenantStorageResolverTest"));
	}

	bool IsOpen() const
	{
		return m_database.isOpen();
	}

	// reimplemented (imtdb::IDatabaseEngine)
	virtual bool BeginTransaction() const override
	{
		return m_database.transaction();
	}

	virtual bool FinishTransaction() const override
	{
		return m_database.commit();
	}

	virtual bool CancelTransaction() const override
	{
		return m_database.rollback();
	}

	virtual QByteArray GetDatabaseDriverId() const override
	{
		return QByteArrayLiteral("QSQLITE");
	}

	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override
	{
		return ExecSqlQuery(queryString, QVariantMap(), sqlError, isForwardOnly);
	}

	virtual QSqlQuery ExecSqlQuery(const QByteArray& queryString, const QVariantMap& bindValues, QSqlError* sqlError = nullptr, bool isForwardOnly = false) const override
	{
		QSqlQuery query(m_database);
		query.setForwardOnly(isForwardOnly);
		query.prepare(queryString);
		for (auto iter = bindValues.constBegin(); iter != bindValues.constEnd(); ++iter){
			query.bindValue(iter.key(), iter.value());
		}
		query.exec();

		if (sqlError != nullptr){
			*sqlError = query.lastError();
		}

		return query;
	}

	virtual QSqlQuery ExecSqlQueryFromFile(const QString& /*filePath*/, QSqlError* sqlError = nullptr, bool /*isForwardOnly*/ = false) const override
	{
		if (sqlError != nullptr){
			*sqlError = QSqlError();
		}

		return QSqlQuery(m_database);
	}

	virtual QSqlQuery ExecSqlQueryFromFile(const QString& /*filePath*/, const QVariantMap& /*bindValues*/, QSqlError* sqlError = nullptr, bool /*isForwardOnly*/ = false) const override
	{
		if (sqlError != nullptr){
			*sqlError = QSqlError();
		}

		return QSqlQuery(m_database);
	}

private:
	mutable QSqlDatabase m_database;
};


} // namespace



void CTenantStorageResolverTest::init()
{
	m_registryPtr = new CTenantStorageRegistry();
}


void CTenantStorageResolverTest::cleanup()
{
	delete m_registryPtr;
	m_registryPtr = nullptr;
}


// --- fail-closed behavior ---

void CTenantStorageResolverTest::testResolveUnknownTenantFails()
{
	TenantStorageInfo info;
	QVERIFY(!m_registryPtr->ResolveTenantStorage(QByteArrayLiteral("unknown-tenant"), info));
}


void CTenantStorageResolverTest::testResolveEmptyTenantIdFails()
{
	m_registryPtr->SetSharedFallbackEnabled(true);

	TenantStorageInfo info;
	QVERIFY(!m_registryPtr->ResolveTenantStorage(QByteArray(), info));
}


void CTenantStorageResolverTest::testSharedFallbackWhenEnabled()
{
	QVERIFY(!m_registryPtr->IsSharedFallbackEnabled());

	m_registryPtr->SetSharedFallbackEnabled(true);

	TenantStorageInfo info;
	QVERIFY(m_registryPtr->ResolveTenantStorage(QByteArrayLiteral("unknown-tenant"), info));
	QCOMPARE(info.storageKind, imtdb::TSK_SHARED_SCHEMA);
	QCOMPARE(info.status, imtdb::TSS_ACTIVE);
	QCOMPARE(info.schemaName, QByteArrayLiteral("public"));
}


// --- registration ---

void CTenantStorageResolverTest::testRegisterAndResolveOwnSchema()
{
	TenantStorageInfo assignment;
	assignment.storageKind = imtdb::TSK_OWN_SCHEMA;
	assignment.status = imtdb::TSS_ACTIVE;
	assignment.schemaName = QByteArrayLiteral("tenant_alpha");

	QVERIFY(m_registryPtr->RegisterTenantStorage(QByteArrayLiteral("alpha"), assignment));
	QVERIFY(m_registryPtr->IsTenantStorageRegistered(QByteArrayLiteral("alpha")));

	TenantStorageInfo resolved;
	QVERIFY(m_registryPtr->ResolveTenantStorage(QByteArrayLiteral("alpha"), resolved));
	QCOMPARE(resolved.storageKind, imtdb::TSK_OWN_SCHEMA);
	QCOMPARE(resolved.schemaName, QByteArrayLiteral("tenant_alpha"));

	// other tenants are still rejected (isolation)
	TenantStorageInfo other;
	QVERIFY(!m_registryPtr->ResolveTenantStorage(QByteArrayLiteral("beta"), other));
}


void CTenantStorageResolverTest::testRegisterAndResolveOwnDatabase()
{
	TenantStorageInfo assignment;
	assignment.storageKind = imtdb::TSK_OWN_DATABASE;
	assignment.status = imtdb::TSS_PROVISIONING;
	assignment.connectionRef = QByteArrayLiteral("tenant-beta-db");

	QVERIFY(m_registryPtr->RegisterTenantStorage(QByteArrayLiteral("beta"), assignment));

	TenantStorageInfo resolved;
	QVERIFY(m_registryPtr->ResolveTenantStorage(QByteArrayLiteral("beta"), resolved));
	QCOMPARE(resolved.storageKind, imtdb::TSK_OWN_DATABASE);
	QCOMPARE(resolved.status, imtdb::TSS_PROVISIONING);
	QCOMPARE(resolved.connectionRef, QByteArrayLiteral("tenant-beta-db"));
}


void CTenantStorageResolverTest::testRegisterSharedSchemaAssignmentGetsSharedSchemaName()
{
	m_registryPtr->SetSharedSchemaName(QByteArrayLiteral("shared"));

	TenantStorageInfo assignment;
	assignment.storageKind = imtdb::TSK_SHARED_SCHEMA;
	assignment.status = imtdb::TSS_ACTIVE;

	QVERIFY(m_registryPtr->RegisterTenantStorage(QByteArrayLiteral("gamma"), assignment));

	TenantStorageInfo resolved;
	QVERIFY(m_registryPtr->ResolveTenantStorage(QByteArrayLiteral("gamma"), resolved));
	QCOMPARE(resolved.schemaName, QByteArrayLiteral("shared"));
}


void CTenantStorageResolverTest::testRegisterRejectsEmptyTenantId()
{
	TenantStorageInfo assignment;
	assignment.storageKind = imtdb::TSK_OWN_SCHEMA;
	assignment.schemaName = QByteArrayLiteral("tenant_x");

	QVERIFY(!m_registryPtr->RegisterTenantStorage(QByteArray(), assignment));
}


void CTenantStorageResolverTest::testRegisterRejectsDedicatedStorageWithoutTarget()
{
	TenantStorageInfo assignment;
	assignment.storageKind = imtdb::TSK_OWN_SCHEMA;
	// neither schemaName nor connectionRef set

	QVERIFY(!m_registryPtr->RegisterTenantStorage(QByteArrayLiteral("delta"), assignment));
	QVERIFY(!m_registryPtr->IsTenantStorageRegistered(QByteArrayLiteral("delta")));
}


void CTenantStorageResolverTest::testUnregisterTenantStorage()
{
	TenantStorageInfo assignment;
	assignment.storageKind = imtdb::TSK_OWN_SCHEMA;
	assignment.schemaName = QByteArrayLiteral("tenant_alpha");

	QVERIFY(m_registryPtr->RegisterTenantStorage(QByteArrayLiteral("alpha"), assignment));
	QVERIFY(m_registryPtr->UnregisterTenantStorage(QByteArrayLiteral("alpha")));
	QVERIFY(!m_registryPtr->IsTenantStorageRegistered(QByteArrayLiteral("alpha")));

	// tenant is rejected again after offboarding (fail-closed)
	TenantStorageInfo resolved;
	QVERIFY(!m_registryPtr->ResolveTenantStorage(QByteArrayLiteral("alpha"), resolved));

	// removing a non-existing assignment reports false
	QVERIFY(!m_registryPtr->UnregisterTenantStorage(QByteArrayLiteral("alpha")));
}


void CTenantStorageResolverTest::testGetRegisteredTenantIds()
{
	QVERIFY(m_registryPtr->GetRegisteredTenantIds().isEmpty());

	TenantStorageInfo assignment;
	assignment.storageKind = imtdb::TSK_OWN_SCHEMA;
	assignment.schemaName = QByteArrayLiteral("tenant_a");

	QVERIFY(m_registryPtr->RegisterTenantStorage(QByteArrayLiteral("a"), assignment));
	QVERIFY(m_registryPtr->RegisterTenantStorage(QByteArrayLiteral("b"), assignment));

	QByteArrayList tenantIds = m_registryPtr->GetRegisteredTenantIds();
	QCOMPARE(tenantIds.size(), 2);
	QVERIFY(tenantIds.contains(QByteArrayLiteral("a")));
	QVERIFY(tenantIds.contains(QByteArrayLiteral("b")));
}


// --- schema name derivation ---

void CTenantStorageResolverTest::testCreateSchemaNameForTenantSanitizesId()
{
	QCOMPARE(m_registryPtr->CreateSchemaNameForTenant(QByteArrayLiteral("Alpha-42")), QByteArrayLiteral("tenant_alpha_42"));

	m_registryPtr->SetSchemaNamePrefix(QByteArrayLiteral("org_"));
	QCOMPARE(
				m_registryPtr->CreateSchemaNameForTenant(QByteArrayLiteral("6a3f/'; DROP")),
				QByteArrayLiteral("org_6a3f____drop"));
}


void CTenantStorageResolverTest::testStaticCreateSchemaName()
{
	QCOMPARE(
				CTenantStorageRegistry::CreateSchemaName(QByteArrayLiteral("tenant_"), QByteArrayLiteral("Alpha-42")),
				QByteArrayLiteral("tenant_alpha_42"));
}


// --- persistence (in-memory SQLite) ---

void CTenantStorageResolverTest::testDbStoreSaveLoadRemove()
{
	CSqliteTestEngine engine;
	QVERIFY(engine.IsOpen());

	imtdb::CTenantStorageDbStore store(engine);
	QVERIFY(store.EnsureRegistryTable());

	TenantStorageInfo info;
	info.storageKind = imtdb::TSK_OWN_SCHEMA;
	info.status = imtdb::TSS_ACTIVE;
	info.schemaName = QByteArrayLiteral("tenant_alpha");
	info.connectionRef = QByteArrayLiteral("main-db");

	QVERIFY(store.SaveAssignment(QByteArrayLiteral("alpha"), info));
	QVERIFY(!store.SaveAssignment(QByteArray(), info));

	imtdb::CTenantStorageDbStore::Assignments assignments;
	QVERIFY(store.LoadAssignments(assignments));
	QCOMPARE(assignments.size(), 1);
	QCOMPARE(assignments[0].first, QByteArrayLiteral("alpha"));
	QCOMPARE(assignments[0].second.storageKind, imtdb::TSK_OWN_SCHEMA);
	QCOMPARE(assignments[0].second.status, imtdb::TSS_ACTIVE);
	QCOMPARE(assignments[0].second.schemaName, QByteArrayLiteral("tenant_alpha"));
	QCOMPARE(assignments[0].second.connectionRef, QByteArrayLiteral("main-db"));

	QVERIFY(store.RemoveAssignment(QByteArrayLiteral("alpha")));

	assignments.clear();
	QVERIFY(store.LoadAssignments(assignments));
	QVERIFY(assignments.isEmpty());
}


void CTenantStorageResolverTest::testDbStoreUpsert()
{
	CSqliteTestEngine engine;
	QVERIFY(engine.IsOpen());

	imtdb::CTenantStorageDbStore store(engine);
	QVERIFY(store.EnsureRegistryTable());

	TenantStorageInfo info;
	info.storageKind = imtdb::TSK_OWN_SCHEMA;
	info.status = imtdb::TSS_PROVISIONING;
	info.schemaName = QByteArrayLiteral("tenant_beta");

	QVERIFY(store.SaveAssignment(QByteArrayLiteral("beta"), info));

	info.status = imtdb::TSS_ACTIVE;
	QVERIFY(store.SaveAssignment(QByteArrayLiteral("beta"), info));

	imtdb::CTenantStorageDbStore::Assignments assignments;
	QVERIFY(store.LoadAssignments(assignments));
	QCOMPARE(assignments.size(), 1);
	QCOMPARE(assignments[0].second.status, imtdb::TSS_ACTIVE);
}


// --- concurrency ---

void CTenantStorageResolverTest::testConcurrentRegistrationAndResolution()
{
	const int tenantCount = 200;

	QList<QFuture<void>> futures;
	for (int index = 0; index < tenantCount; ++index){
		futures.append(QtConcurrent::run([this, index](){
			QByteArray tenantId = QByteArrayLiteral("tenant-") + QByteArray::number(index);

			TenantStorageInfo assignment;
			assignment.storageKind = imtdb::TSK_OWN_SCHEMA;
			assignment.status = imtdb::TSS_ACTIVE;
			assignment.schemaName = m_registryPtr->CreateSchemaNameForTenant(tenantId);

			QVERIFY(m_registryPtr->RegisterTenantStorage(tenantId, assignment));

			TenantStorageInfo resolved;
			QVERIFY(m_registryPtr->ResolveTenantStorage(tenantId, resolved));
			QCOMPARE(resolved.schemaName, assignment.schemaName);
		}));
	}

	for (QFuture<void>& future: futures){
		future.waitForFinished();
	}

	QCOMPARE(m_registryPtr->GetRegisteredTenantIds().size(), tenantCount);
}


I_ADD_TEST(CTenantStorageResolverTest);
