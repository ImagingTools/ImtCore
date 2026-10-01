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

	// validated before any policy is applied, so a misconfiguration never leaves a partially protected set
	for (int tableIndex = 0; tableIndex < m_tableNamesAttrPtr.GetCount(); ++tableIndex){
		const QByteArray tableName = m_tableNamesAttrPtr[tableIndex];
		if (CTenantRlsPolicyBuilder::IsCrossTenantCatalogTable(tableName)){
			SendErrorMessage(
						0,
						QStringLiteral("Row-Level Security rejected: '%1' is a cross-tenant catalog table, an isolation policy would break tenant switching and delegated access").arg(QString(tableName)),
						"CTenantRlsControllerComp");

			return false;
		}
	}

	const QByteArray tableSchema = m_tableSchemaAttrPtr.IsValid() ? *m_tableSchemaAttrPtr : QByteArrayLiteral("public");
	const QByteArray tenantIdColumn = m_tenantIdColumnAttrPtr.IsValid() ? *m_tenantIdColumnAttrPtr : QByteArrayLiteral("TenantId");
	const QByteArray sessionVariableName = m_sessionVariableNameAttrPtr.IsValid() ? *m_sessionVariableNameAttrPtr : QByteArrayLiteral("app.tenant_id");

	for (int tableIndex = 0; tableIndex < m_tableNamesAttrPtr.GetCount(); ++tableIndex){
		QByteArray tableName = m_tableNamesAttrPtr[tableIndex];

		QByteArrayList statements = CTenantRlsPolicyBuilder::CreateEnableRlsStatements(
					tableSchema,
					tableName,
					tenantIdColumn,
					sessionVariableName);
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


// protected methods

// reimplemented (icomp::CComponentBase)

void CTenantRlsControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_applyOnStartupAttrPtr.IsValid() && *m_applyOnStartupAttrPtr){
		ApplyRowLevelSecurity();
	}
}


// private methods

bool CTenantRlsControllerComp::IsPostgresDriver() const
{
	return m_databaseEngineCompPtr->GetDatabaseDriverId().startsWith(QByteArrayLiteral("QPSQL"));
}


} // namespace imtdb
