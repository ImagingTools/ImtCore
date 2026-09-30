// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtdb/CTenantStorageRegistry.h>


class CTenantStorageResolverTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void init();
	void cleanup();

	// fail-closed behavior
	void testResolveUnknownTenantFails();
	void testResolveEmptyTenantIdFails();
	void testSharedFallbackWhenEnabled();

	// registration
	void testRegisterAndResolveOwnSchema();
	void testRegisterAndResolveOwnDatabase();
	void testRegisterSharedSchemaAssignmentGetsSharedSchemaName();
	void testRegisterRejectsEmptyTenantId();
	void testRegisterRejectsDedicatedStorageWithoutTarget();
	void testUnregisterTenantStorage();
	void testGetRegisteredTenantIds();

	// schema name derivation
	void testCreateSchemaNameForTenantSanitizesId();
	void testStaticCreateSchemaName();

	// persistence (in-memory SQLite)
	void testDbStoreSaveLoadRemove();
	void testDbStoreUpsert();

	// concurrency
	void testConcurrentRegistrationAndResolution();

	// tenant context scope
	void testTenantContextScopeActivation();
	void testTenantContextScopeNesting();
	void testTenantContextScopeIsThreadLocal();

	// data migration (in-memory SQLite with attached tenant schema)
	void testMigrateTableCopiesAndVerifies();
	void testMigrateTableIsIdempotent();
	void testMigrateTableFailsOnPartialCopy();
	void testRemoveSourceRows();
	void testQuoteIdentifier();

	// row-level security statement generation
	void testRlsStatementsGeneration();
	void testRlsStatementsRejectInvalidIdentifiers();
	void testRlsSessionVariableValidation();
	void testRlsBindAndUnbindQueries();

private:
	imtdb::CTenantStorageRegistry* m_registryPtr = nullptr;
};
