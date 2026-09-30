// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QPair>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/ITenantStorageResolver.h>


namespace imtdb
{


/**
	Persistence for tenant storage assignments in the \c TenantStorage registry table.
	Operates on an \c IDatabaseEngine using parameterized queries only.
*/
class CTenantStorageDbStore
{
public:
	typedef QPair<QByteArray, TenantStorageInfo> Assignment;
	typedef QList<Assignment> Assignments;

	explicit CTenantStorageDbStore(const IDatabaseEngine& databaseEngine, const QByteArray& tableSchema = QByteArray());

	/**
		Create the \c TenantStorage registry table if it does not exist.
	*/
	bool EnsureRegistryTable() const;

	/**
		Insert or update the persisted storage assignment for a tenant.
	*/
	bool SaveAssignment(const QByteArray& tenantId, const TenantStorageInfo& info) const;

	/**
		Remove the persisted storage assignment of a tenant.
	*/
	bool RemoveAssignment(const QByteArray& tenantId) const;

	/**
		Load all persisted storage assignments.
		\return \c true if the query succeeded, assignments are appended to \p result.
	*/
	bool LoadAssignments(Assignments& result) const;

private:
	QByteArray GetQualifiedTableName() const;
	bool IsSqliteDriver() const;

	const IDatabaseEngine& m_databaseEngine;
	QByteArray m_tableSchema;
};


} // namespace imtdb
