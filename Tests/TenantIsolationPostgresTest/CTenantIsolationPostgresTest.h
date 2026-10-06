// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <icomp/IComponent.h>


/**
	Integration tests of the tenant isolation components against a real PostgreSQL server.
	The tests create a temporary database owned by a temporary non-superuser role
	and remove both afterwards.
	Connection of the administrative account: environment variables IMTCORE_TEST_PG_HOST,
	IMTCORE_TEST_PG_PORT, IMTCORE_TEST_PG_USER and IMTCORE_TEST_PG_PASSWORD;
	the tests are skipped if IMTCORE_TEST_PG_PASSWORD is not set.
*/
class CTenantIsolationPostgresTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();
	void init();

	void testTenantSchemaMigrationRunsPerTenant();
	void testProvisionerRollsBackFailedSchema();
	void testAutoProvisioningOnFirstAccess();
	void testDataMigratorCopiesTenantRows();
	void testDataMigratorDetectsChecksumMismatch();
	void testBackupAndRestoreTenantSchema();
	void testRestoreRejectsArchiveOfOtherTenant();
	void testDocumentDelegatesAddressTenantSchema();
	void testTenantCollectionIsBoundToTenant();
	void testGarbageCollectorKeepsTenantReferencedContent();

private:
	bool ExecAdmin(const QString& query);
	bool ExecApp(const QString& query);
	int CountRows(const QByteArray& query) const;

	QString m_skipReason;
	QString m_host;
	int m_port = 5432;
	QString m_adminUser;
	QString m_adminPassword;
	QString m_appPassword;

	icomp::IComponentSharedPtr m_engineCompPtr;
	icomp::IComponentSharedPtr m_tenantMigrationCompPtr;
	icomp::IComponentSharedPtr m_schemaMigrationCompPtr;
};


