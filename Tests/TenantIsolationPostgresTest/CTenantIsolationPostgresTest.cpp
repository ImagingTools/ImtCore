// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CTenantIsolationPostgresTest.h"

#include <itest/CStandardTestExecutor.h>

#include <QtConcurrent/QtConcurrent>
#include <QtCore/QRegularExpression>
#include <QtCore/QStandardPaths>
#include <QtCore/QTemporaryDir>
#include <QtCore/QUuid>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include <icomp/TSimComponentWrap.h>
#include <ifile/CFileNameParamComp.h>

#include <imtbase/COperationContext.h>

#include <imtdb/CDatabaseAccessSettingsComp.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/CFileDocumentGarbageCollectorComp.h>
#include <imtdb/CSqlDatabaseObjectCollectionComp.h>
#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>
#include <imtdb/CTenantDataMigrator.h>
#include <imtdb/CTenantDataMigratorComp.h>
#include <imtdb/CTenantSchemaMigrationControllerComp.h>
#include <imtdb/CTenantStorageBackupComp.h>
#include <imtdb/CTenantStorageAutoProvisioningResolverComp.h>
#include <imtdb/CTenantStorageProvisionerComp.h>
#include <imtdb/CTenantStorageResolverComp.h>


namespace
{


const QString s_databaseName = QStringLiteral("imt_tenant_isolation_test");
const QString s_appRoleName = QStringLiteral("imt_tenant_isolation_app");
const QString s_adminConnectionName = QStringLiteral("imt_tenant_isolation_admin");
const QString s_appConnectionName = QStringLiteral("imt_tenant_isolation_setup");


/**
	Migration of the tenant-owned tables: creates an unqualified table, so the table lands in the
	schema at the head of the search path, and records the tenant context it was executed in.
*/
class CTenantTableMigrationTestComp: public icomp::CComponentBase, virtual public imtdb::IMigrationController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantTableMigrationTestComp);
		I_REGISTER_INTERFACE(imtdb::IMigrationController);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine", true, "DatabaseEngine");
	I_END_COMPONENT;

	// reimplemented (imtdb::IMigrationController)
	virtual istd::CIntRange GetMigrationRange() const override
	{
		return istd::CIntRange(0, 1);
	}

	virtual bool DoMigration(int& resultRevision, const istd::CIntRange& subRange) const override
	{
		resultRevision = -1;

		if (subRange.GetMinValue() > 1){
			return true;
		}

		QSqlError sqlError;
		m_databaseEngineCompPtr->ExecSqlQuery(QByteArrayLiteral("CREATE TABLE \"Items\" (\"TenantId\" TEXT, \"Payload\" TEXT)"), &sqlError);
		if (sqlError.type() != QSqlError::NoError){
			return false;
		}

		QVariantMap bindValues;
		m_databaseEngineCompPtr->ExecSqlQuery(QByteArrayLiteral("INSERT INTO \"Items\" VALUES (current_schema(), 'migrated')"), &sqlError);
		if (sqlError.type() != QSqlError::NoError){
			return false;
		}

		resultRevision = 1;

		return true;
	}

private:
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
};


typedef icomp::TSimComponentWrap<imtdb::CDatabaseEngineComp> DatabaseEngine;
typedef icomp::TSimComponentWrap<CTenantTableMigrationTestComp> TenantTableMigration;
typedef icomp::TSimComponentWrap<imtdb::CTenantSchemaMigrationControllerComp> TenantSchemaMigrationController;
typedef icomp::TSimComponentWrap<imtdb::CTenantStorageResolverComp> TenantStorageResolver;
typedef icomp::TSimComponentWrap<imtdb::CTenantStorageProvisionerComp> TenantStorageProvisioner;
typedef icomp::TSimComponentWrap<imtdb::CTenantDataMigratorComp> TenantDataMigrator;
typedef icomp::TSimComponentWrap<imtdb::CTenantStorageAutoProvisioningResolverComp> AutoProvisioningResolver;
typedef icomp::TSimComponentWrap<imtdb::CDatabaseAccessSettingsComp> DatabaseAccessSettings;
typedef icomp::TSimComponentWrap<imtdb::CTenantStorageBackupComp> TenantStorageBackup;
typedef icomp::TSimComponentWrap<imtdb::CSqlDatabaseDocumentDelegateComp> DocumentDelegate;
typedef icomp::TSimComponentWrap<imtdb::CSqlJsonDatabaseDelegateComp> JsonDocumentDelegate;
typedef icomp::TSimComponentWrap<imtdb::CSqlDatabaseObjectCollectionComp> SqlCollection;
typedef icomp::TSimComponentWrap<ifile::CFileNameParamComp> FileNameParam;
typedef icomp::TSimComponentWrap<imtdb::CFileDocumentGarbageCollectorComp> FileDocumentGarbageCollector;


const QByteArray s_hashShared = QByteArray(64, 'a');
const QByteArray s_hashAlphaReferenced = QByteArray(64, 'c');
const QByteArray s_hashAlphaOrphan = QByteArray(64, 'd');


std::shared_ptr<FileNameParam> CreateDirectoryParam(const QString& path)
{
	std::shared_ptr<FileNameParam> paramPtr = std::make_shared<FileNameParam>();
	paramPtr->SetStringAttr("DefaultPath", path);
	paramPtr->SetIntAttr("PathType", ifile::IFileNameParam::PT_DIRECTORY);
	paramPtr->InitComponent();

	return paramPtr;
}


QString CreateStoreFile(const QString& storePath, const QByteArray& contentHash)
{
	const QString filePath = QDir(storePath).filePath(QString::fromLatin1(contentHash.left(2)) + '/' + QString::fromLatin1(contentHash) + QStringLiteral(".bin"));
	QDir().mkpath(QFileInfo(filePath).absolutePath());

	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly)){
		return QString();
	}

	file.write("content");

	// older than any grace period
	file.setFileTime(QDateTime::currentDateTimeUtc().addDays(-30), QFileDevice::FileModificationTime);
	file.close();

	return filePath;
}


QString CreateDescriptor(const QByteArray& contentHash)
{
	return QStringLiteral("{\"fmt\":1,\"alg\":\"sha256\",\"hash\":\"%1\",\"size\":7}").arg(QString::fromLatin1(contentHash));
}


template <class Delegate>
std::shared_ptr<Delegate> CreateDelegate(
			const icomp::IComponentSharedPtr& engineCompPtr,
			const icomp::IComponentSharedPtr& resolverCompPtr,
			const QByteArray& tableSchema)
{
	std::shared_ptr<Delegate> delegatePtr = std::make_shared<Delegate>();
	delegatePtr->SetRef("DatabaseEngine", engineCompPtr);
	if (resolverCompPtr){
		delegatePtr->SetRef("TenantStorageResolver", resolverCompPtr);
	}
	if (!tableSchema.isEmpty()){
		delegatePtr->SetIdAttr("TableSchema", tableSchema);
	}
	delegatePtr->SetIdAttr("TableName", QByteArrayLiteral("Items"));
	delegatePtr->SetIdAttr("ObjectIdColumn", QByteArrayLiteral("DocumentId"));
	delegatePtr->SetIdAttr("ObjectTypeIdColumn", QByteArrayLiteral("TypeId"));
	delegatePtr->InitComponent();

	return delegatePtr;
}


bool ExecOnConnection(const QString& connectionName, const QString& query)
{
	QSqlQuery sqlQuery(QSqlDatabase::database(connectionName));
	if (!sqlQuery.exec(query)){
		qWarning() << "Setup statement failed:" << query << sqlQuery.lastError().text();

		return false;
	}

	return true;
}


std::shared_ptr<TenantStorageResolver> CreateLoadedResolver(const icomp::IComponentSharedPtr& engineCompPtr)
{
	std::shared_ptr<TenantStorageResolver> resolverPtr = std::make_shared<TenantStorageResolver>();
	resolverPtr->SetRef("DatabaseEngine", engineCompPtr);
	resolverPtr->InitComponent();

	return resolverPtr;
}


std::shared_ptr<TenantStorageBackup> CreateBackup(
			const icomp::IComponentSharedPtr& engineCompPtr,
			const icomp::IComponentSharedPtr& resolverCompPtr,
			const QString& host,
			int port,
			const QString& password)
{
	std::shared_ptr<DatabaseAccessSettings> loginSettingsPtr = std::make_shared<DatabaseAccessSettings>();
	loginSettingsPtr->SetIdAttr("DbName", s_databaseName.toUtf8());
	loginSettingsPtr->SetIdAttr("DbPath", QByteArray());
	loginSettingsPtr->SetIdAttr("UserName", s_appRoleName.toUtf8());
	loginSettingsPtr->SetIdAttr("Pasword", password.toUtf8());
	loginSettingsPtr->SetIdAttr("HostName", host.toUtf8());
	loginSettingsPtr->SetIntAttr("Port", port);
	loginSettingsPtr->SetBoolAttr("UseSSL", false);
	loginSettingsPtr->InitComponent();

	std::shared_ptr<TenantStorageBackup> backupPtr = std::make_shared<TenantStorageBackup>();
	backupPtr->SetRef("DatabaseEngine", engineCompPtr);
	backupPtr->SetRef("DatabaseLoginSettings", loginSettingsPtr);
	backupPtr->SetRef("StorageResolver", resolverCompPtr);
	backupPtr->InitComponent();

	return backupPtr;
}


} // namespace


void CTenantIsolationPostgresTest::initTestCase()
{
	Q_INIT_RESOURCE(imtdb);

	m_adminPassword = qEnvironmentVariable("IMTCORE_TEST_PG_PASSWORD");
	if (m_adminPassword.isEmpty()){
		// skipped in init(): a skip in initTestCase leaks into the next test class of the ACF test executor
		m_skipReason = QStringLiteral("IMTCORE_TEST_PG_PASSWORD is not set, PostgreSQL integration tests skipped");

		return;
	}

	m_host = qEnvironmentVariable("IMTCORE_TEST_PG_HOST", QStringLiteral("127.0.0.1"));
	m_port = qEnvironmentVariable("IMTCORE_TEST_PG_PORT", QStringLiteral("5432")).toInt();
	m_adminUser = qEnvironmentVariable("IMTCORE_TEST_PG_USER", QStringLiteral("postgres"));
	m_appPassword = QUuid::createUuid().toString(QUuid::Id128);

	QSqlDatabase adminDatabase = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"), s_adminConnectionName);
	adminDatabase.setHostName(m_host);
	adminDatabase.setPort(m_port);
	adminDatabase.setUserName(m_adminUser);
	adminDatabase.setPassword(m_adminPassword);
	adminDatabase.setDatabaseName(QStringLiteral("postgres"));
	QVERIFY2(adminDatabase.open(), qPrintable(adminDatabase.lastError().text()));

	QVERIFY(ExecAdmin(QStringLiteral("DROP DATABASE IF EXISTS %1 WITH (FORCE)").arg(s_databaseName)));
	QVERIFY(ExecAdmin(QStringLiteral("DROP ROLE IF EXISTS %1").arg(s_appRoleName)));
	QVERIFY(ExecAdmin(QStringLiteral("CREATE ROLE %1 LOGIN NOSUPERUSER NOBYPASSRLS PASSWORD '%2'").arg(s_appRoleName, m_appPassword)));
	QVERIFY(ExecAdmin(QStringLiteral("CREATE DATABASE %1 OWNER %2").arg(s_databaseName, s_appRoleName)));

	QSqlDatabase appDatabase = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"), s_appConnectionName);
	appDatabase.setHostName(m_host);
	appDatabase.setPort(m_port);
	appDatabase.setUserName(s_appRoleName);
	appDatabase.setPassword(m_appPassword);
	appDatabase.setDatabaseName(s_databaseName);
	QVERIFY2(appDatabase.open(), qPrintable(appDatabase.lastError().text()));

	// tenant registry: two active dedicated schemas and an archived one, created before the engine migrates
	QVERIFY(ExecApp(QStringLiteral("CREATE SCHEMA tenant_alpha")));
	QVERIFY(ExecApp(QStringLiteral("CREATE SCHEMA tenant_beta")));
	QVERIFY(ExecApp(QStringLiteral("CREATE SCHEMA tenant_archived")));
	QVERIFY(ExecApp(QStringLiteral(
				"CREATE TABLE \"TenantStorage\" (\"TenantId\" TEXT PRIMARY KEY, \"StorageKind\" INTEGER NOT NULL DEFAULT 0,"
				" \"Status\" INTEGER NOT NULL DEFAULT 0, \"SchemaName\" TEXT NOT NULL DEFAULT '', \"ConnectionRef\" TEXT NOT NULL DEFAULT '')")));
	QVERIFY(ExecApp(QStringLiteral(
				"INSERT INTO \"TenantStorage\" VALUES ('alpha', 1, 2, 'tenant_alpha', ''), ('beta', 1, 2, 'tenant_beta', ''),"
				" ('archived', 1, 4, 'tenant_archived', '')")));

	// shared table protected by Row-Level Security and the per-tenant target of the data migration
	QVERIFY(ExecApp(QStringLiteral("CREATE TABLE \"Shared\" (\"TenantId\" TEXT, \"Payload\" TEXT)")));
	QVERIFY(ExecApp(QStringLiteral("INSERT INTO \"Shared\" VALUES ('alpha', 'a1'), ('beta', 'b1')")));
	QVERIFY(ExecApp(QStringLiteral("CREATE TABLE tenant_alpha.\"Shared\" (\"TenantId\" TEXT, \"Payload\" TEXT)")));

	// unprotected table for the checksum verification
	QVERIFY(ExecApp(QStringLiteral("CREATE TABLE \"Docs\" (\"TenantId\" TEXT, \"Payload\" TEXT)")));
	QVERIFY(ExecApp(QStringLiteral("INSERT INTO \"Docs\" VALUES ('beta', 'd1'), ('beta', 'd2')")));
	QVERIFY(ExecApp(QStringLiteral("CREATE TABLE tenant_beta.\"Docs\" (\"TenantId\" TEXT, \"Payload\" TEXT)")));

	std::shared_ptr<DatabaseEngine> enginePtr = std::make_shared<DatabaseEngine>();
	std::shared_ptr<TenantTableMigration> tenantMigrationPtr = std::make_shared<TenantTableMigration>();
	std::shared_ptr<TenantSchemaMigrationController> schemaMigrationPtr = std::make_shared<TenantSchemaMigrationController>();

	m_engineCompPtr = enginePtr;
	m_tenantMigrationCompPtr = tenantMigrationPtr;
	m_schemaMigrationCompPtr = schemaMigrationPtr;

	tenantMigrationPtr->SetRef("DatabaseEngine", m_engineCompPtr);
	tenantMigrationPtr->InitComponent();

	schemaMigrationPtr->SetRef("DatabaseEngine", m_engineCompPtr);
	schemaMigrationPtr->SetRef("TenantMigrationController", m_tenantMigrationCompPtr);
	schemaMigrationPtr->InitComponent();

	enginePtr->SetIdAttr("DbType", QByteArrayLiteral("QPSQL"));
	enginePtr->SetIdAttr("DbName", s_databaseName.toUtf8());
	enginePtr->SetIdAttr("UserName", s_appRoleName.toUtf8());
	enginePtr->SetIdAttr("Pasword", m_appPassword.toUtf8());
	enginePtr->SetIdAttr("HostName", m_host.toUtf8());
	enginePtr->SetIdAttr("MaintainanceDatabase", QByteArrayLiteral("postgres"));
	enginePtr->SetIntAttr("AutoCreateDatabase", 0);
	enginePtr->SetIntAttr("AutoCreateTables", 1);
	enginePtr->SetIntAttr("Port", m_port);
	enginePtr->SetRef("MigrationController", m_schemaMigrationCompPtr);

	// runs the migrations of all tenant schemas
	enginePtr->InitComponent();
}


void CTenantIsolationPostgresTest::cleanupTestCase()
{
	if (!m_skipReason.isEmpty()){
		return;
	}

	m_schemaMigrationCompPtr.reset();
	m_tenantMigrationCompPtr.reset();
	m_engineCompPtr.reset();

	QSqlDatabase::database(s_appConnectionName).close();
	QSqlDatabase::removeDatabase(s_appConnectionName);

	ExecAdmin(QStringLiteral("DROP DATABASE IF EXISTS %1 WITH (FORCE)").arg(s_databaseName));
	ExecAdmin(QStringLiteral("DROP ROLE IF EXISTS %1").arg(s_appRoleName));

	QSqlDatabase::database(s_adminConnectionName).close();
	QSqlDatabase::removeDatabase(s_adminConnectionName);
}


void CTenantIsolationPostgresTest::init()
{
	if (!m_skipReason.isEmpty()){
		QSKIP(qPrintable(m_skipReason));
	}

	QVERIFY2(m_engineCompPtr, "Test database setup failed");
}


void CTenantIsolationPostgresTest::testTenantSchemaMigrationRunsPerTenant()
{
	imtdb::IDatabaseEngine* enginePtr = dynamic_cast<imtdb::IDatabaseEngine*>(m_engineCompPtr.get());
	QVERIFY(enginePtr != nullptr);

	// each tenant schema got its own table; the migration ran with that schema at the head of the search path
	QSqlQuery alphaQuery = enginePtr->ExecSqlQuery(QByteArrayLiteral("SELECT \"TenantId\" FROM tenant_alpha.\"Items\""));
	QVERIFY(alphaQuery.next());
	QCOMPARE(alphaQuery.value(0).toString(), QStringLiteral("tenant_alpha"));
	QVERIFY(!alphaQuery.next());

	QSqlQuery betaQuery = enginePtr->ExecSqlQuery(QByteArrayLiteral("SELECT \"TenantId\" FROM tenant_beta.\"Items\""));
	QVERIFY(betaQuery.next());
	QCOMPARE(betaQuery.value(0).toString(), QStringLiteral("tenant_beta"));
	QVERIFY(!betaQuery.next());

	// neither the shared schema nor archived tenants are migrated, and the search path was restored
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT 1 WHERE to_regclass('public.\"Items\"') IS NOT NULL")), 0);
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT 1 WHERE to_regclass('tenant_archived.\"Items\"') IS NOT NULL")), 0);
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT 1 WHERE current_setting('search_path') LIKE '%tenant_%'")), 0);

	QSqlQuery revisionQuery = enginePtr->ExecSqlQuery(QByteArrayLiteral("SELECT MAX(Revision) FROM \"Revisions\""));
	QVERIFY(revisionQuery.next());
	QCOMPARE(revisionQuery.value(0).toInt(), 1);
}


void CTenantIsolationPostgresTest::testProvisionerRollsBackFailedSchema()
{
	std::shared_ptr<TenantStorageResolver> resolverPtr = CreateLoadedResolver(m_engineCompPtr);
	QVERIFY(resolverPtr->IsTenantStorageRegistered(QByteArrayLiteral("alpha")));

	std::shared_ptr<TenantStorageProvisioner> failingProvisionerPtr = std::make_shared<TenantStorageProvisioner>();
	failingProvisionerPtr->SetRef("DatabaseEngine", m_engineCompPtr);
	failingProvisionerPtr->SetRef("StorageResolver", resolverPtr);
	failingProvisionerPtr->InsertMultiAttr("DdlScriptPaths", QStringLiteral(":/Missing/CreateTenantTables.sql"));
	failingProvisionerPtr->InitComponent();

	// a schema created by the failed provisioning is removed again
	QVERIFY(!failingProvisionerPtr->ProvisionTenantStorage(QByteArrayLiteral("gamma")));
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT 1 FROM information_schema.schemata WHERE schema_name = 'tenant_gamma'")), 0);
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT 1 FROM \"TenantStorage\" WHERE \"TenantId\" = 'gamma'")), 0);
	QVERIFY(!resolverPtr->IsTenantStorageRegistered(QByteArrayLiteral("gamma")));

	// a pre-existing schema keeps its data
	QVERIFY(ExecApp(QStringLiteral("CREATE SCHEMA tenant_delta")));
	QVERIFY(ExecApp(QStringLiteral("CREATE TABLE tenant_delta.\"Kept\" (\"Id\" TEXT)")));
	QVERIFY(!failingProvisionerPtr->ProvisionTenantStorage(QByteArrayLiteral("delta")));
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT 1 WHERE to_regclass('tenant_delta.\"Kept\"') IS NOT NULL")), 1);

	std::shared_ptr<TenantStorageProvisioner> provisionerPtr = std::make_shared<TenantStorageProvisioner>();
	provisionerPtr->SetRef("DatabaseEngine", m_engineCompPtr);
	provisionerPtr->SetRef("StorageResolver", resolverPtr);
	provisionerPtr->InitComponent();

	QVERIFY(provisionerPtr->ProvisionTenantStorage(QByteArrayLiteral("epsilon")));
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT 1 FROM information_schema.schemata WHERE schema_name = 'tenant_epsilon'")), 1);
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT 1 FROM \"TenantStorage\" WHERE \"TenantId\" = 'epsilon' AND \"StorageKind\" = 1")), 1);

	// a restarted resolver resolves the tenant from the persisted registry
	std::shared_ptr<TenantStorageResolver> restartedResolverPtr = CreateLoadedResolver(m_engineCompPtr);
	imtdb::TenantStorageInfo storageInfo;
	QVERIFY(restartedResolverPtr->ResolveTenantStorage(QByteArrayLiteral("epsilon"), storageInfo));
	QCOMPARE(storageInfo.schemaName, QByteArrayLiteral("tenant_epsilon"));
	QCOMPARE(int(storageInfo.status), int(imtdb::TSS_ACTIVE));
}


void CTenantIsolationPostgresTest::testAutoProvisioningOnFirstAccess()
{
	std::shared_ptr<TenantStorageResolver> resolverPtr = CreateLoadedResolver(m_engineCompPtr);

	std::shared_ptr<TenantStorageProvisioner> provisionerPtr = std::make_shared<TenantStorageProvisioner>();
	provisionerPtr->SetRef("DatabaseEngine", m_engineCompPtr);
	provisionerPtr->SetRef("StorageResolver", resolverPtr);
	provisionerPtr->InitComponent();

	std::shared_ptr<AutoProvisioningResolver> autoResolverPtr = std::make_shared<AutoProvisioningResolver>();
	autoResolverPtr->SetRef("StorageResolver", resolverPtr);
	autoResolverPtr->SetRef("StorageProvisioner", provisionerPtr);
	autoResolverPtr->InitComponent();

	// concurrent first accesses of the same tenant provision it exactly once and all succeed
	QList<QFuture<bool>> accesses;
	for (int index = 0; index < 8; ++index){
		accesses.append(QtConcurrent::run([autoResolverPtr](){
			imtdb::TenantStorageInfo storageInfo;
			return autoResolverPtr->ResolveTenantStorage(QByteArrayLiteral("zeta"), storageInfo) && (storageInfo.schemaName == QByteArrayLiteral("tenant_zeta"));
		}));
	}

	for (QFuture<bool>& access: accesses){
		QVERIFY(access.result());
	}

	QCOMPARE(CountRows(QByteArrayLiteral("SELECT 1 FROM information_schema.schemata WHERE schema_name = 'tenant_zeta'")), 1);
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT 1 FROM \"TenantStorage\" WHERE \"TenantId\" = 'zeta' AND \"StorageKind\" = 1 AND \"Status\" = 2")), 1);

	// a failed provisioning denies access instead of resolving anything
	std::shared_ptr<TenantStorageProvisioner> failingProvisionerPtr = std::make_shared<TenantStorageProvisioner>();
	failingProvisionerPtr->SetRef("DatabaseEngine", m_engineCompPtr);
	failingProvisionerPtr->SetRef("StorageResolver", resolverPtr);
	failingProvisionerPtr->InsertMultiAttr("DdlScriptPaths", QStringLiteral(":/Missing/CreateTenantTables.sql"));
	failingProvisionerPtr->InitComponent();

	std::shared_ptr<AutoProvisioningResolver> failingResolverPtr = std::make_shared<AutoProvisioningResolver>();
	failingResolverPtr->SetRef("StorageResolver", resolverPtr);
	failingResolverPtr->SetRef("StorageProvisioner", failingProvisionerPtr);
	failingResolverPtr->InitComponent();

	imtdb::TenantStorageInfo storageInfo;
	QVERIFY(!failingResolverPtr->ResolveTenantStorage(QByteArrayLiteral("eta"), storageInfo));
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT 1 FROM information_schema.schemata WHERE schema_name = 'tenant_eta'")), 0);
}


void CTenantIsolationPostgresTest::testDataMigratorCopiesTenantRows()
{
	std::shared_ptr<TenantStorageResolver> resolverPtr = CreateLoadedResolver(m_engineCompPtr);

	std::shared_ptr<TenantDataMigrator> migratorPtr = std::make_shared<TenantDataMigrator>();
	migratorPtr->SetRef("DatabaseEngine", m_engineCompPtr);
	migratorPtr->SetRef("StorageResolver", resolverPtr);
	migratorPtr->InsertMultiAttr("TableNames", QByteArrayLiteral("Shared"));
	migratorPtr->SetBoolAttr("VerifyChecksums", true);
	migratorPtr->InitComponent();

	QVERIFY(migratorPtr->MigrateTenantData(QByteArrayLiteral("alpha")));

	QCOMPARE(CountRows(QByteArrayLiteral("SELECT * FROM tenant_alpha.\"Shared\" WHERE \"TenantId\" = 'alpha' AND \"Payload\" = 'a1'")), 1);
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT * FROM tenant_alpha.\"Shared\"")), 1);
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT 1 FROM \"TenantStorage\" WHERE \"TenantId\" = 'alpha' AND \"Status\" = 2")), 1);
}


void CTenantIsolationPostgresTest::testDataMigratorDetectsChecksumMismatch()
{
	imtdb::IDatabaseEngine* enginePtr = dynamic_cast<imtdb::IDatabaseEngine*>(m_engineCompPtr.get());
	QVERIFY(enginePtr != nullptr);

	imtdb::CTenantDataMigrator migrator(*enginePtr);
	migrator.SetSourceSchema(QByteArrayLiteral("public"));
	migrator.SetChecksumVerificationEnabled(true);

	int migratedRowCount = 0;
	QString errorMessage;
	QVERIFY2(migrator.MigrateTable(QByteArrayLiteral("Docs"), QByteArrayLiteral("beta"), QByteArrayLiteral("tenant_beta"), migratedRowCount, errorMessage), qPrintable(errorMessage));
	QCOMPARE(migratedRowCount, 2);

	// same row count, different content: only the checksum can tell
	QVERIFY(ExecApp(QStringLiteral("UPDATE tenant_beta.\"Docs\" SET \"Payload\" = 'tampered' WHERE \"Payload\" = 'd2'")));

	QVERIFY(!migrator.MigrateTable(QByteArrayLiteral("Docs"), QByteArrayLiteral("beta"), QByteArrayLiteral("tenant_beta"), migratedRowCount, errorMessage));
	QVERIFY(errorMessage.contains(QStringLiteral("Checksum")));

	migrator.SetChecksumVerificationEnabled(false);
	QVERIFY(migrator.MigrateTable(QByteArrayLiteral("Docs"), QByteArrayLiteral("beta"), QByteArrayLiteral("tenant_beta"), migratedRowCount, errorMessage));
}


void CTenantIsolationPostgresTest::testBackupAndRestoreTenantSchema()
{
	if (QStandardPaths::findExecutable(QStringLiteral("pg_dump")).isEmpty() || QStandardPaths::findExecutable(QStringLiteral("pg_restore")).isEmpty()){
		QSKIP("pg_dump/pg_restore not found in PATH");
	}

	QTemporaryDir backupDir;
	QVERIFY(backupDir.isValid());
	const QString backupFilePath = backupDir.filePath(QStringLiteral("alpha.backup"));

	std::shared_ptr<TenantStorageResolver> resolverPtr = CreateLoadedResolver(m_engineCompPtr);
	std::shared_ptr<TenantStorageBackup> backupPtr = CreateBackup(m_engineCompPtr, resolverPtr, m_host, m_port, m_appPassword);

	QVERIFY(backupPtr->BackupTenantStorage(QByteArrayLiteral("alpha"), backupFilePath));

	QVERIFY(ExecApp(QStringLiteral("DELETE FROM tenant_alpha.\"Items\"")));
	QVERIFY(ExecApp(QStringLiteral("CREATE TABLE tenant_alpha.\"CreatedAfterBackup\" (\"Id\" TEXT)")));

	QVERIFY(backupPtr->RestoreTenantStorage(QByteArrayLiteral("alpha"), backupFilePath));

	QCOMPARE(CountRows(QByteArrayLiteral("SELECT * FROM tenant_alpha.\"Items\" WHERE \"TenantId\" = 'tenant_alpha'")), 1);
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT 1 WHERE to_regclass('tenant_alpha.\"CreatedAfterBackup\"') IS NOT NULL")), 0);
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT 1 FROM information_schema.schemata WHERE schema_name LIKE 'imt_restore_%'")), 0);

	imtdb::TenantStorageInfo storageInfo;
	QVERIFY(resolverPtr->ResolveTenantStorage(QByteArrayLiteral("alpha"), storageInfo));
	QCOMPARE(int(storageInfo.status), int(imtdb::TSS_ACTIVE));
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT 1 FROM \"TenantStorage\" WHERE \"TenantId\" = 'alpha' AND \"Status\" = 2")), 1);
}


void CTenantIsolationPostgresTest::testRestoreRejectsArchiveOfOtherTenant()
{
	if (QStandardPaths::findExecutable(QStringLiteral("pg_dump")).isEmpty() || QStandardPaths::findExecutable(QStringLiteral("pg_restore")).isEmpty()){
		QSKIP("pg_dump/pg_restore not found in PATH");
	}

	QTemporaryDir backupDir;
	QVERIFY(backupDir.isValid());
	const QString backupFilePath = backupDir.filePath(QStringLiteral("alpha.backup"));

	std::shared_ptr<TenantStorageResolver> resolverPtr = CreateLoadedResolver(m_engineCompPtr);
	std::shared_ptr<TenantStorageBackup> backupPtr = CreateBackup(m_engineCompPtr, resolverPtr, m_host, m_port, m_appPassword);

	QVERIFY(backupPtr->BackupTenantStorage(QByteArrayLiteral("alpha"), backupFilePath));

	QVERIFY(!backupPtr->RestoreTenantStorage(QByteArrayLiteral("beta"), backupFilePath));

	QCOMPARE(CountRows(QByteArrayLiteral("SELECT * FROM tenant_beta.\"Items\" WHERE \"TenantId\" = 'tenant_beta'")), 1);
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT * FROM tenant_beta.\"Items\" WHERE \"TenantId\" = 'tenant_alpha'")), 0);
	QCOMPARE(CountRows(QByteArrayLiteral("SELECT 1 FROM \"TenantStorage\" WHERE \"TenantId\" = 'beta' AND \"Status\" = 2")), 1);
}


void CTenantIsolationPostgresTest::testDocumentDelegatesAddressTenantSchema()
{
	static const QRegularExpression alphaTable(QStringLiteral("\"tenant_alpha\"\\.\\s*\"Items\""));
	static const QRegularExpression publicTable(QStringLiteral("public\\.\\s*\"Items\""));
	static const QRegularExpression unqualifiedTable(QStringLiteral("(FROM|JOIN|INTO|UPDATE)\\s+\"Items\""));

	const QByteArray alphaTenantId = QByteArrayLiteral("alpha");
	const QByteArray noOrganization;

	std::shared_ptr<TenantStorageResolver> resolverPtr = CreateLoadedResolver(m_engineCompPtr);

	std::shared_ptr<DocumentDelegate> documentDelegatePtr = CreateDelegate<DocumentDelegate>(m_engineCompPtr, resolverPtr, QByteArrayLiteral("public"));
	std::shared_ptr<JsonDocumentDelegate> jsonDelegatePtr = CreateDelegate<JsonDocumentDelegate>(m_engineCompPtr, resolverPtr, QByteArray());

	QVERIFY(documentDelegatePtr->HasTenantStorage());

	const QList<QByteArray> tenantQueries = {
				documentDelegatePtr->GetSelectionQuery(QByteArrayLiteral("doc-1")),
				documentDelegatePtr->GetSelectionQuery(QByteArray(), 0, 10),
				documentDelegatePtr->GetCountQuery(),
				jsonDelegatePtr->GetSelectionQuery(QByteArray(), 0, 10),
				jsonDelegatePtr->GetCountQuery()};

	for (const QByteArray& builtQuery : tenantQueries){
		// a tenant-owned delegate never builds a query against a concrete schema by itself
		QVERIFY2(builtQuery.contains(imtdb::ISqlDatabaseObjectDelegate::s_tenantSchemePrefixPlaceholder), builtQuery.constData());

		// the tenant of the collection wins over the static TableSchema 'public'
		QByteArray alphaQuery = builtQuery;
		QVERIFY(documentDelegatePtr->ApplyTenantStorage(alphaQuery, alphaTenantId));
		QVERIFY2(alphaTable.match(QString::fromUtf8(alphaQuery)).hasMatch(), alphaQuery.constData());
		QVERIFY2(!publicTable.match(QString::fromUtf8(alphaQuery)).hasMatch(), alphaQuery.constData());
		QVERIFY2(!unqualifiedTable.match(QString::fromUtf8(alphaQuery)).hasMatch(), alphaQuery.constData());
	}

	// an unknown tenant is denied; a call without organization addresses the shared schema
	const QByteArray unknownTenantId = QByteArrayLiteral("unknown");
	QByteArray unknownQuery = documentDelegatePtr->GetCountQuery();
	QVERIFY(!documentDelegatePtr->ApplyTenantStorage(unknownQuery, unknownTenantId));

	QByteArray noOrganizationQuery = documentDelegatePtr->GetCountQuery();
	QVERIFY(documentDelegatePtr->ApplyTenantStorage(noOrganizationQuery, noOrganization));
	QVERIFY2(publicTable.match(QString::fromUtf8(noOrganizationQuery)).hasMatch(), noOrganizationQuery.constData());

	// without a resolver the previous behavior stays: TableSchema for the document delegate, unqualified for the JSON delegate
	std::shared_ptr<DocumentDelegate> sharedDocumentDelegatePtr = CreateDelegate<DocumentDelegate>(m_engineCompPtr, nullptr, QByteArrayLiteral("public"));
	std::shared_ptr<JsonDocumentDelegate> sharedJsonDelegatePtr = CreateDelegate<JsonDocumentDelegate>(m_engineCompPtr, nullptr, QByteArrayLiteral("public"));

	QVERIFY(!sharedDocumentDelegatePtr->HasTenantStorage());

	QByteArray sharedQuery = sharedDocumentDelegatePtr->GetCountQuery();
	QVERIFY(!sharedQuery.contains(imtdb::ISqlDatabaseObjectDelegate::s_tenantSchemePrefixPlaceholder));
	QVERIFY(sharedDocumentDelegatePtr->ApplyTenantStorage(sharedQuery, QByteArray()));
	QVERIFY(publicTable.match(QString::fromUtf8(sharedQuery)).hasMatch());
	QByteArray sharedJsonQuery = sharedJsonDelegatePtr->GetSelectionQuery(QByteArray(), 0, 10);
	QVERIFY2(unqualifiedTable.match(QString::fromUtf8(sharedJsonQuery)).hasMatch(), sharedJsonQuery.constData());
	QVERIFY2(!publicTable.match(QString::fromUtf8(sharedJsonQuery)).hasMatch(), sharedJsonQuery.constData());
}


void CTenantIsolationPostgresTest::testTenantCollectionIsBoundToTenant()
{
	std::shared_ptr<TenantStorageResolver> resolverPtr = CreateLoadedResolver(m_engineCompPtr);
	std::shared_ptr<DocumentDelegate> documentDelegatePtr = CreateDelegate<DocumentDelegate>(m_engineCompPtr, resolverPtr, QByteArrayLiteral("public"));

	SqlCollection collection;
	collection.SetRef("DatabaseEngine", m_engineCompPtr);
	collection.SetRef("ObjectDelegate", documentDelegatePtr);
	collection.InitComponent();

	QVERIFY(collection.IsTenantSeparated());

	imtbase::ITenantObjectCollection* alphaCollectionPtr = collection.GetTenantCollection(QByteArrayLiteral("alpha"));
	QVERIFY(alphaCollectionPtr != nullptr);
	QCOMPARE(alphaCollectionPtr->GetTenantId(), QByteArrayLiteral("alpha"));
	QCOMPARE(collection.GetTenantCollection(QByteArrayLiteral("alpha")), alphaCollectionPtr);

	// an operation of a tenant is denied on the data of another tenant and on the data without organization
	imtbase::COperationContext alphaOperation;
	alphaOperation.SetTenantId(QByteArrayLiteral("alpha"));

	const imtbase::ICollectionInfo::Ids objectIds = {QByteArrayLiteral("doc-1")};
	QVERIFY(!collection.GetTenantCollection(QByteArrayLiteral("beta"))->RemoveElements(objectIds, &alphaOperation));
	QVERIFY(!collection.RemoveElements(objectIds, &alphaOperation));

	// without a resolver all tenants share the data of the collection
	std::shared_ptr<DocumentDelegate> sharedDocumentDelegatePtr = CreateDelegate<DocumentDelegate>(m_engineCompPtr, nullptr, QByteArrayLiteral("public"));

	SqlCollection sharedCollection;
	sharedCollection.SetRef("DatabaseEngine", m_engineCompPtr);
	sharedCollection.SetRef("ObjectDelegate", sharedDocumentDelegatePtr);
	sharedCollection.InitComponent();

	QVERIFY(!sharedCollection.IsTenantSeparated());
}


void CTenantIsolationPostgresTest::testGarbageCollectorKeepsTenantReferencedContent()
{
	QTemporaryDir storeDir;
	QVERIFY(storeDir.isValid());
	const QString rootPath = storeDir.path();

	QVERIFY(ExecApp(QStringLiteral("CREATE TABLE \"FileDocs\" (\"Document\" TEXT)")));
	QVERIFY(ExecApp(QStringLiteral("CREATE TABLE tenant_alpha.\"FileDocs\" (\"Document\" TEXT)")));
	QVERIFY(ExecApp(QStringLiteral("INSERT INTO \"FileDocs\" VALUES ('%1')").arg(CreateDescriptor(s_hashShared))));
	QVERIFY(ExecApp(QStringLiteral("INSERT INTO tenant_alpha.\"FileDocs\" VALUES ('%1')").arg(CreateDescriptor(s_hashAlphaReferenced))));

	// one shared store: a tenant row references content in the same store as a shared row
	const QString sharedFile = CreateStoreFile(rootPath, s_hashShared);
	const QString alphaReferencedFile = CreateStoreFile(rootPath, s_hashAlphaReferenced);
	const QString alphaOrphanFile = CreateStoreFile(rootPath, s_hashAlphaOrphan);
	QVERIFY(!sharedFile.isEmpty() && !alphaReferencedFile.isEmpty() && !alphaOrphanFile.isEmpty());

	auto createCollector = [&](const icomp::IComponentSharedPtr& resolverCompPtr){
		std::shared_ptr<FileDocumentGarbageCollector> collectorPtr = std::make_shared<FileDocumentGarbageCollector>();
		collectorPtr->SetRef("DatabaseEngine", m_engineCompPtr);
		collectorPtr->SetRef("StorageRoot", CreateDirectoryParam(rootPath));
		if (resolverCompPtr){
			collectorPtr->SetRef("TenantStorageResolver", resolverCompPtr);
		}
		collectorPtr->SetIdAttr("TableName", QByteArrayLiteral("FileDocs"));
		collectorPtr->SetIntAttr("CheckInterval", 50);
		collectorPtr->SetIntAttr("GracePeriodHours", 1);
		collectorPtr->SetBoolAttr("AuditOnly", false);
		collectorPtr->InitComponent();

		return collectorPtr;
	};

	// content referenced only from a tenant schema stays alive; the unreferenced file marks a finished pass
	std::shared_ptr<TenantStorageResolver> resolverPtr = CreateLoadedResolver(m_engineCompPtr);
	std::shared_ptr<FileDocumentGarbageCollector> collectorPtr = createCollector(resolverPtr);

	QTRY_VERIFY_WITH_TIMEOUT(!QFileInfo::exists(alphaOrphanFile), 10000);
	collectorPtr.reset();

	QVERIFY(QFileInfo::exists(sharedFile));
	QVERIFY(QFileInfo::exists(alphaReferencedFile));
}


// private methods

bool CTenantIsolationPostgresTest::ExecAdmin(const QString& query)
{
	return ExecOnConnection(s_adminConnectionName, query);
}


bool CTenantIsolationPostgresTest::ExecApp(const QString& query)
{
	return ExecOnConnection(s_appConnectionName, query);
}


int CTenantIsolationPostgresTest::CountRows(const QByteArray& query) const
{
	imtdb::IDatabaseEngine* enginePtr = dynamic_cast<imtdb::IDatabaseEngine*>(m_engineCompPtr.get());
	if (enginePtr == nullptr){
		return -1;
	}

	QSqlError sqlError;
	QSqlQuery sqlQuery = enginePtr->ExecSqlQuery(query, &sqlError, true);
	if (sqlError.type() != QSqlError::NoError){
		return -1;
	}

	int count = 0;
	while (sqlQuery.next()){
		++count;
	}

	return count;
}



I_ADD_TEST(CTenantIsolationPostgresTest);


