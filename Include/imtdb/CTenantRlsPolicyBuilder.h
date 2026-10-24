// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QByteArrayList>


namespace imtdb
{


/**
	Generator of the PostgreSQL Row Level Security (RLS) statements used for tenant isolation.

	The tenant security context is transported to PostgreSQL via session settings (GUC):
	- \c imt.tenant_id - ID of the current tenant ('' if there is no tenant context);
	- \c imt.user_id - ID of the current user ('' if unknown);
	- \c imt.rls_bypass - 'on' for trusted system operations, 'off' otherwise.

	All generated policies are fail-closed: if no tenant context is set, tenant-owned rows are not visible and cannot be written.
	All identifiers are validated (see IsValidIdentifier) and quoted, so no user input can be injected into the generated DDL.
*/
class CTenantRlsPolicyBuilder
{
public:
	/**
		Description of a table with tenant-owned rows.
	*/
	struct TenantOwnedTableInfo
	{
		QByteArray schema;
		QByteArray tableName;

		/**
			Columns containing the owning tenant-ID. A row is accessible if any of the columns matches the current tenant
			(e.g. "SourceTenantId" and "TargetTenantId" for cross-tenant records).
		*/
		QByteArrayList tenantColumns;

		/**
			Optional column containing the owning user-ID. If set, the user can additionally read own rows independently of the current tenant
			(e.g. own memberships or invitations used for tenant selection).
		*/
		QByteArray userColumn;
	};

	static const QByteArray s_tenantIdSettingName;
	static const QByteArray s_userIdSettingName;
	static const QByteArray s_systemContextSettingName;
	static const QByteArray s_isolationPolicyName;
	static const QByteArray s_userAccessPolicyName;
	static const QByteArray s_tenantBindingsTableName;

	/**
		Check if the given string is a safe SQL identifier (letters, digits and underscore, not starting with a digit).
	*/
	static bool IsValidIdentifier(const QByteArray& identifier);

	/**
		Parse the tenant-owned table specification in format: <tt>[Schema.]Table:TenantColumn[,TenantColumn...][:UserColumn]</tt>.
		If no schema is given, \c public is used.
		\return \c true if the specification is valid.
	*/
	static bool ParseTenantOwnedTableSpec(const QByteArray& spec, TenantOwnedTableInfo& info);

	/**
		Query setting the tenant security context of the current database session.
		Placeholders: \c :TenantId, \c :UserId, \c :SystemContext ('on' or 'off').
	*/
	static QByteArray CreateContextSyncQuery();

	/**
		Create RLS statements for a table with tenant-owned rows.
		\return empty array if the table info is invalid.
	*/
	static QByteArray CreateTenantOwnedTablePolicyQuery(const TenantOwnedTableInfo& info, bool allowSystemContext);

	/**
		Create RLS statements for a document collection table whose tenant ownership is defined in the \c TenantEntityBindings table
		(\c EntityType = table name, \c EntityId = \c DocumentId).
		A row is accessible if the document is bound to the current tenant or if it is not bound to any tenant (global document).
		\return empty array if the schema or table name is invalid.
	*/
	static QByteArray CreateBindingScopedTablePolicyQuery(const QByteArray& schema, const QByteArray& tableName, bool allowSystemContext);

	/**
		Create RLS statements for the \c TenantEntityBindings table.
		Bindings are readable (they are required for the visibility checks of the binding-scoped tables),
		but can be created, changed or removed only for the current tenant.
		\return empty array if the schema is invalid.
	*/
	static QByteArray CreateTenantBindingsTablePolicyQuery(const QByteArray& schema, bool allowSystemContext);

private:
	static QByteArray QuoteIdentifier(const QByteArray& identifier);
	static QByteArray CreateQualifiedTableName(const QByteArray& schema, const QByteArray& tableName);
	static QByteArray CreateCurrentTenantExpression();
	static QByteArray CreateCurrentUserExpression();
	static QByteArray CreateSystemContextExpression();
	static QByteArray CreateEnableRlsQuery(const QByteArray& qualifiedTableName);
};


} // namespace imtdb


