// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>


namespace imtdb
{


/**
	Core logic copying tenant rows from a shared schema into a dedicated tenant schema.
	Uses parameterized queries for all values; identifiers are validated and quoted.
*/
class CTenantDataMigrator
{
public:
	explicit CTenantDataMigrator(const IDatabaseEngine& databaseEngine);

	void SetSourceSchema(const QByteArray& sourceSchema);
	void SetTenantIdColumn(const QByteArray& tenantIdColumn);

	/**
		Copy all rows of the given tenant from the source schema table into the
		same table inside the target schema and verify the copied row count.
		The operation is idempotent: if the target table already contains exactly
		the source row count for this tenant, the table is skipped.
		\param migratedRowCount Receives the number of rows present in the target table.
		\return \c true if the table content was migrated and verified.
	*/
	bool MigrateTable(
				const QByteArray& tableName,
				const QByteArray& tenantId,
				const QByteArray& targetSchema,
				int& migratedRowCount,
				QString& errorMessage) const;

	/**
		Remove the rows of the given tenant from the source schema table
		(cleanup after a successful and verified migration).
	*/
	bool RemoveSourceRows(const QByteArray& tableName, const QByteArray& tenantId, QString& errorMessage) const;

	/**
		Quote an SQL identifier. Returns an empty byte array if the identifier
		is empty or contains a double quote character.
	*/
	static QByteArray QuoteIdentifier(const QByteArray& identifier);

private:
	bool CountTenantRows(const QByteArray& schemaName, const QByteArray& tableName, const QByteArray& tenantId, int& count, QString& errorMessage) const;
	QByteArray CreateQualifiedTableName(const QByteArray& schemaName, const QByteArray& tableName) const;

	const IDatabaseEngine& m_databaseEngine;
	QByteArray m_sourceSchema;
	QByteArray m_tenantIdColumn;
};


} // namespace imtdb
