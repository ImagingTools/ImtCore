// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantRowLevelSecurityControllerComp.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QSet>
#include <QtCore/QTimer>

// ImtCore includes
#include <imtdb/CTenantRlsPolicyBuilder.h>


namespace imtdb
{


// protected methods

bool CTenantRowLevelSecurityControllerComp::InstallPolicies() const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		SendCriticalMessage(0, QStringLiteral("Tenant RLS policies could not be installed: database engine is not set"));

		return false;
	}

	if (m_databaseEngineCompPtr->GetDatabaseDriverId().compare(QByteArrayLiteral("QPSQL"), Qt::CaseInsensitive) != 0){
		SendWarningMessage(0, QStringLiteral("Tenant RLS policies are supported for PostgreSQL only. Tenant isolation is provided by the application layer"));

		return true;
	}

	// Ensure that the delegates are created and the protected tables exist.
	for (int i = 0; i < m_tableDelegatesCompPtr.GetCount(); ++i){
		if (m_tableDelegatesCompPtr[i] == nullptr){
			SendWarningMessage(0, QStringLiteral("Table delegate %1 is not available").arg(i));
		}
	}

	const bool allowSystemContext = *m_allowSystemContextAttrPtr;

	bool retVal = true;

	for (int i = 0; i < m_tenantOwnedTablesAttrPtr.GetCount(); ++i){
		const QByteArray spec = m_tenantOwnedTablesAttrPtr[i];

		CTenantRlsPolicyBuilder::TenantOwnedTableInfo tableInfo;
		if (!CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec(spec, tableInfo)){
			SendCriticalMessage(0, QStringLiteral("Invalid tenant-owned table specification: '%1'").arg(QString(spec)));

			retVal = false;

			continue;
		}

		const QByteArray query = CTenantRlsPolicyBuilder::CreateTenantOwnedTablePolicyQuery(tableInfo, allowSystemContext);

		retVal = ExecutePolicyQuery(tableInfo.tableName, query) && retVal;
	}

	QSet<QByteArray> bindingSchemes;
	for (int i = 0; i < m_bindingScopedCollectionsCompPtr.GetCount(); ++i){
		const imtdb::ISqlDatabaseObjectDelegate* delegatePtr = m_bindingScopedCollectionsCompPtr[i];
		if (delegatePtr == nullptr){
			continue;
		}

		QByteArray tableScheme = delegatePtr->GetTableScheme();
		if (tableScheme.isEmpty()){
			tableScheme = QByteArrayLiteral("public");
		}

		const QByteArray tableName = delegatePtr->GetTableName();

		if (!bindingSchemes.contains(tableScheme)){
			bindingSchemes.insert(tableScheme);

			const QByteArray bindingsQuery = CTenantRlsPolicyBuilder::CreateTenantBindingsTablePolicyQuery(tableScheme, allowSystemContext);

			retVal = ExecutePolicyQuery(CTenantRlsPolicyBuilder::s_tenantBindingsTableName, bindingsQuery) && retVal;
		}

		const QByteArray query = CTenantRlsPolicyBuilder::CreateBindingScopedTablePolicyQuery(tableScheme, tableName, allowSystemContext);

		retVal = ExecutePolicyQuery(tableName, query) && retVal;
	}

	return retVal;
}


// reimplemented (icomp::CComponentBase)

void CTenantRowLevelSecurityControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// The component can be created while the database migrations are running, and a migration may recreate the protected
	// tables (dropping their policies). The policies are installed when the start is complete.
	QCoreApplication* applicationPtr = QCoreApplication::instance();
	if (applicationPtr == nullptr){
		InstallAndCheckPolicies();

		return;
	}

	m_aliveGuardPtr = std::make_shared<bool>(true);
	std::weak_ptr<bool> aliveGuard(m_aliveGuardPtr);
	QTimer::singleShot(0, applicationPtr, [this, aliveGuard](){
		if (!aliveGuard.expired()){
			InstallAndCheckPolicies();
		}
	});
}


void CTenantRowLevelSecurityControllerComp::OnComponentDestroyed()
{
	m_aliveGuardPtr.reset();

	BaseClass::OnComponentDestroyed();
}


bool CTenantRowLevelSecurityControllerComp::CheckEnforcement() const
{
	if (!m_databaseEngineCompPtr.IsValid() || m_databaseEngineCompPtr->GetDatabaseDriverId().compare(QByteArrayLiteral("QPSQL"), Qt::CaseInsensitive) != 0){
		return false;
	}

	QSqlError sqlError;
	QSqlQuery roleQuery = m_databaseEngineCompPtr->ExecSqlQuery(
				QByteArrayLiteral("SELECT current_user, rolsuper OR rolbypassrls FROM pg_roles WHERE rolname = current_user"),
				&sqlError);
	if (sqlError.type() != QSqlError::NoError || !roleQuery.next()){
		SendWarningMessage(0, QStringLiteral("Enforcement of the tenant RLS policies could not be checked: %1").arg(sqlError.text()));

		return false;
	}

	const QString roleName = roleQuery.value(0).toString();
	if (roleQuery.value(1).toBool()){
		SendWarningMessage(
					0,
					QStringLiteral("Tenant RLS policies are installed but NOT enforced: the database role '%1' has SUPERUSER or BYPASSRLS. "
								"Connect with a restricted role, e.g. by setting the administrative login of the database engine (AdminDatabaseAccessSettings)").arg(roleName));

		return false;
	}

	SendInfoMessage(0, QStringLiteral("Tenant RLS policies are enforced for the database role '%1'").arg(roleName));

	return true;
}


// private methods

void CTenantRowLevelSecurityControllerComp::InstallAndCheckPolicies() const
{
	if (!InstallPolicies()){
		SendCriticalMessage(0, QStringLiteral("Tenant RLS policies were not installed completely. Tenant isolation on the database level is incomplete"));
	}

	CheckEnforcement();
}


bool CTenantRowLevelSecurityControllerComp::ExecutePolicyQuery(const QByteArray& tableName, const QByteArray& query) const
{
	if (query.isEmpty()){
		SendCriticalMessage(0, QStringLiteral("Tenant RLS policy for table '%1' could not be created: invalid table or column name").arg(QString(tableName)));

		return false;
	}

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendCriticalMessage(0, QStringLiteral("Tenant RLS policy for table '%1' could not be installed: %2").arg(QString(tableName), sqlError.text()));

		return false;
	}

	return true;
}


} // namespace imtdb


