// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayList>


namespace imtdb
{


/**
	Builder creating validated SQL statements for tenant Row-Level Security
	on shared Postgres tables.
	All identifiers are validated and quoted; the tenant ID itself is always
	bound as a query parameter by the caller.
*/
class CTenantRlsPolicyBuilder
{
public:
	/**
		Create the statements enabling and forcing Row-Level Security on a table
		and (re-)creating the tenant isolation policy bound to the session variable.
		\return The list of statements, or an empty list if any identifier is invalid.
	*/
	static QByteArrayList CreateEnableRlsStatements(
				const QByteArray& schemaName,
				const QByteArray& tableName,
				const QByteArray& tenantIdColumn,
				const QByteArray& sessionVariableName);

	/**
		Create the parameterized query binding the tenant ID of the current
		database session to the session variable (bind value \c :tenantId).
		\return The query, or an empty byte array if the variable name is invalid.
	*/
	static QByteArray CreateBindSessionTenantQuery(const QByteArray& sessionVariableName);

	/**
		Create the query clearing the tenant session variable.
		\return The query, or an empty byte array if the variable name is invalid.
	*/
	static QByteArray CreateUnbindSessionTenantQuery(const QByteArray& sessionVariableName);

	/**
		Check if the session variable name consists of exactly two
		lowercase alphanumeric segments separated by a dot (e.g. \c app.tenant_id).
	*/
	static bool IsValidSessionVariableName(const QByteArray& sessionVariableName);

	/**
		Create the name of the tenant isolation policy for a table.
	*/
	static QByteArray CreatePolicyName(const QByteArray& tableName);

	/**
		Check if the table belongs to the shared catalog, whose rows are read across tenant
		boundaries by design (e.g. all memberships of a user, grants targeting a tenant).
		A tenant isolation policy on such a table breaks tenant switching and delegated access.
	*/
	static bool IsCrossTenantCatalogTable(const QByteArray& tableName);
};


} // namespace imtdb
