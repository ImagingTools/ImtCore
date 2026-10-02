// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// std includes
#include <memory>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/ISqlDatabaseObjectDelegate.h>


namespace imtdb
{


/**
	Installs PostgreSQL Row Level Security (RLS) policies for tenant isolation.

	The component is executed once on creation (configure it for automatic instantiation) and idempotently
	(re-)creates the policies (see CTenantRlsPolicyBuilder) for:
	- tables with tenant-owned rows (attribute \c TenantOwnedTables);
	- document collection tables, whose tenant ownership is stored in \c TenantEntityBindings (reference \c BindingScopedCollections),
	  including the \c TenantEntityBindings table itself.

	The policies only take effect if the database engine passes the access context to PostgreSQL
	(reference \c AccessContext of CDatabaseEngineComp) and the application connects with a role
	that is neither superuser nor has the \c BYPASSRLS attribute.
	Installing the policies is DDL and is not restricted by them, so no system access context is required.
	For other database drivers (e.g. SQLite) no policies are installed.
*/
class CTenantRowLevelSecurityControllerComp: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantRowLevelSecurityControllerComp);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine", true, "DatabaseEngine");
		I_ASSIGN_MULTI_0(m_tableDelegatesCompPtr, "TableDelegates", "Delegates creating the protected tables. They are instantiated before the policies are installed, to ensure that the tables exist", false);
		I_ASSIGN_MULTI_0(m_tenantOwnedTablesAttrPtr, "TenantOwnedTables", "Tables with tenant-owned rows in format: [Schema.]Table:TenantColumn[,TenantColumn...][:UserColumn]", false);
		I_ASSIGN_MULTI_0(m_bindingScopedCollectionsCompPtr, "BindingScopedCollections", "Delegates of document collections whose tenant ownership is stored in TenantEntityBindings", false);
		I_ASSIGN(m_allowSystemContextAttrPtr, "AllowSystemContext", "If enabled, operations executed in the system security context bypass the tenant isolation", true, true);
	I_END_COMPONENT;

protected:
	/**
		Install the RLS policies for all configured tables.
		\return \c true if all policies were installed successfully.
	*/
	virtual bool InstallPolicies() const;

	/**
		Check that the installed policies are enforced: PostgreSQL does not apply RLS to roles with SUPERUSER or BYPASSRLS.
		\return \c true if the role of the database connection is restricted by the policies.
	*/
	virtual bool CheckEnforcement() const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void InstallAndCheckPolicies() const;
	bool ExecutePolicyQuery(const QByteArray& tableName, const QByteArray& query) const;

private:
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_MULTIREF(imtdb::ISqlDatabaseObjectDelegate, m_tableDelegatesCompPtr);
	I_MULTIATTR(QByteArray, m_tenantOwnedTablesAttrPtr);
	I_MULTIREF(imtdb::ISqlDatabaseObjectDelegate, m_bindingScopedCollectionsCompPtr);
	I_ATTR(bool, m_allowSystemContextAttrPtr);

	std::shared_ptr<bool> m_aliveGuardPtr;
};


} // namespace imtdb


