// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtSql/QSqlDatabase>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


/**
	Integration test of the tenant Row Level Security policies on a real PostgreSQL server.

	The test is skipped if no server is configured. Configuration via environment variables:
	- IMT_TEST_POSTGRES_HOST (required), IMT_TEST_POSTGRES_PORT (default 5432),
	- IMT_TEST_POSTGRES_USER and IMT_TEST_POSTGRES_PASSWORD - administrative account allowed to create roles and schemas,
	- IMT_TEST_POSTGRES_DATABASE (default 'postgres').

	The test creates a temporary non-superuser role and schema, which are removed after the test.
*/
class CTenantRowLevelSecurityTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	void init();
	void cleanupTestCase();

	void TenantCannotReadOtherTenantDataTest();
	void TenantCannotModifyOtherTenantDataTest();
	void NoTenantContextDeniesTenantDataTest();
	void UserCanReadOwnRowsOnlyTest();
	void CrossTenantTableTest();
	void BindingScopedCollectionTest();
	void SystemContextTest();
	void SystemContextDisabledTest();
	void MaliciousContextValueTest();

private:
	bool ExecuteAdminQuery(const QByteArray& query);
	bool ExecuteAppQuery(const QByteArray& query, QSqlError* errorPtr = nullptr);
	bool SetContext(const QByteArray& tenantId, const QByteArray& userId, bool isSystemContext);
	int GetRowCount(const QByteArray& tableName);
	QStringList GetProductNames();

private:
	QString m_skipReason;
	QString m_adminConnectionName;
	QString m_appConnectionName;
	QByteArray m_roleName;
	QByteArray m_schemaName;
};


