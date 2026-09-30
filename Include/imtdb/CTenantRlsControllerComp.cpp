// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantRlsControllerComp.h>


// Qt includes
#include <QtSql/QSqlError>

// ImtCore includes
#include <imtdb/CTenantRlsPolicyBuilder.h>


namespace imtdb
{


// reimplemented (imtdb::ITenantRlsController)

bool CTenantRlsControllerComp::ApplyRowLevelSecurity()
{
	if (!IsPostgresDriver()){
		SendWarningMessage(0, QStringLiteral("Row-Level Security is only supported for Postgres databases, no policies were applied"), "CTenantRlsControllerComp");

		return false;
	}

	for (int tableIndex = 0; tableIndex < m_tableNamesAttrPtr.GetCount(); ++tableIndex){
		QByteArray tableName = m_tableNamesAttrPtr[tableIndex];

		QByteArrayList statements = CTenantRlsPolicyBuilder::CreateEnableRlsStatements(
					*m_tableSchemaAttrPtr,
					tableName,
					*m_tenantIdColumnAttrPtr,
					*m_sessionVariableNameAttrPtr);
		if (statements.isEmpty()){
			SendErrorMessage(0, QStringLiteral("Row-Level Security for table '%1' rejected: invalid schema, table, column or session variable identifier").arg(QString(tableName)), "CTenantRlsControllerComp");

			return false;
		}

		for (const QByteArray& statement: statements){
			QSqlError sqlError;
			m_databaseEngineCompPtr->ExecSqlQuery(statement, &sqlError);
			if (sqlError.type() != QSqlError::NoError){
				SendErrorMessage(0, QStringLiteral("Applying Row-Level Security to table '%1' failed: %2").arg(QString(tableName), sqlError.text()), "CTenantRlsControllerComp");

				return false;
			}
		}

		SendInfoMessage(0, QStringLiteral("Row-Level Security policy applied to table '%1'").arg(QString(tableName)), "CTenantRlsControllerComp");
	}

	return true;
}


bool CTenantRlsControllerComp::BindSessionTenant(const QByteArray& tenantId)
{
	if (tenantId.isEmpty()){
		SendErrorMessage(0, QStringLiteral("Binding of the tenant session variable rejected: empty tenant ID"), "CTenantRlsControllerComp");

		return false;
	}

	QByteArray bindQuery = CTenantRlsPolicyBuilder::CreateBindSessionTenantQuery(*m_sessionVariableNameAttrPtr);
	if (bindQuery.isEmpty()){
		SendErrorMessage(0, QStringLiteral("Binding of the tenant session variable rejected: invalid session variable name"), "CTenantRlsControllerComp");

		return false;
	}

	QVariantMap bindValues;
	bindValues[QStringLiteral(":tenantId")] = QString(tenantId);

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(bindQuery, bindValues, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, QStringLiteral("Binding of the tenant session variable for tenant '%1' failed: %2").arg(QString(tenantId), sqlError.text()), "CTenantRlsControllerComp");

		return false;
	}

	return true;
}


bool CTenantRlsControllerComp::UnbindSessionTenant()
{
	QByteArray unbindQuery = CTenantRlsPolicyBuilder::CreateUnbindSessionTenantQuery(*m_sessionVariableNameAttrPtr);
	if (unbindQuery.isEmpty()){
		SendErrorMessage(0, QStringLiteral("Clearing of the tenant session variable rejected: invalid session variable name"), "CTenantRlsControllerComp");

		return false;
	}

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(unbindQuery, &sqlError);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, QStringLiteral("Clearing of the tenant session variable failed: %1").arg(sqlError.text()), "CTenantRlsControllerComp");

		return false;
	}

	return true;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CTenantRlsControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (*m_applyOnStartupAttrPtr){
		ApplyRowLevelSecurity();
	}
}


// private methods

bool CTenantRlsControllerComp::IsPostgresDriver() const
{
	return m_databaseEngineCompPtr->GetDatabaseDriverId().startsWith(QByteArrayLiteral("QPSQL"));
}


} // namespace imtdb
