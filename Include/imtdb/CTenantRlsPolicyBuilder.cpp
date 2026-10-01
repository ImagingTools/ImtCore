// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantRlsPolicyBuilder.h>


// ImtCore includes
#include <imtdb/CTenantDataMigrator.h>


namespace imtdb
{


namespace
{


bool IsValidVariableSegment(const QByteArray& segment)
{
	if (segment.isEmpty()){
		return false;
	}

	for (char character: segment){
		bool isValid = (character >= 'a' && character <= 'z') || (character >= '0' && character <= '9') || character == '_';
		if (!isValid){
			return false;
		}
	}

	return true;
}


} // namespace


// static methods

QByteArrayList CTenantRlsPolicyBuilder::CreateEnableRlsStatements(
			const QByteArray& schemaName,
			const QByteArray& tableName,
			const QByteArray& tenantIdColumn,
			const QByteArray& sessionVariableName)
{
	QByteArray quotedSchema = CTenantDataMigrator::QuoteIdentifier(schemaName);
	QByteArray quotedTable = CTenantDataMigrator::QuoteIdentifier(tableName);
	QByteArray quotedColumn = CTenantDataMigrator::QuoteIdentifier(tenantIdColumn);
	QByteArray quotedPolicy = CTenantDataMigrator::QuoteIdentifier(CreatePolicyName(tableName));
	if (quotedSchema.isEmpty() || quotedTable.isEmpty() || quotedColumn.isEmpty() || quotedPolicy.isEmpty()){
		return QByteArrayList();
	}

	if (!IsValidSessionVariableName(sessionVariableName)){
		return QByteArrayList();
	}

	QByteArray qualifiedTable = quotedSchema + '.' + quotedTable;

	QByteArrayList retVal;
	retVal << QByteArrayLiteral("ALTER TABLE ") + qualifiedTable + QByteArrayLiteral(" ENABLE ROW LEVEL SECURITY");
	retVal << QByteArrayLiteral("ALTER TABLE ") + qualifiedTable + QByteArrayLiteral(" FORCE ROW LEVEL SECURITY");
	retVal << QByteArrayLiteral("DROP POLICY IF EXISTS ") + quotedPolicy + QByteArrayLiteral(" ON ") + qualifiedTable;
	retVal << QByteArrayLiteral("CREATE POLICY ") + quotedPolicy + QByteArrayLiteral(" ON ") + qualifiedTable +
				QByteArrayLiteral(" USING (") + quotedColumn + QByteArrayLiteral("::text = current_setting('") + sessionVariableName + QByteArrayLiteral("', true))");

	return retVal;
}


QByteArray CTenantRlsPolicyBuilder::CreateBindSessionTenantQuery(const QByteArray& sessionVariableName)
{
	if (!IsValidSessionVariableName(sessionVariableName)){
		return QByteArray();
	}

	return QByteArrayLiteral("SELECT set_config('") + sessionVariableName + QByteArrayLiteral("', :tenantId, false)");
}


QByteArray CTenantRlsPolicyBuilder::CreateUnbindSessionTenantQuery(const QByteArray& sessionVariableName)
{
	if (!IsValidSessionVariableName(sessionVariableName)){
		return QByteArray();
	}

	return QByteArrayLiteral("SELECT set_config('") + sessionVariableName + QByteArrayLiteral("', '', false)");
}


bool CTenantRlsPolicyBuilder::IsValidSessionVariableName(const QByteArray& sessionVariableName)
{
	int dotIndex = sessionVariableName.indexOf('.');
	if (dotIndex < 0){
		return false;
	}

	QByteArray prefix = sessionVariableName.left(dotIndex);
	QByteArray suffix = sessionVariableName.mid(dotIndex + 1);
	if (suffix.contains('.')){
		return false;
	}

	return IsValidVariableSegment(prefix) && IsValidVariableSegment(suffix);
}


QByteArray CTenantRlsPolicyBuilder::CreatePolicyName(const QByteArray& tableName)
{
	return QByteArrayLiteral("TenantIsolation_") + tableName;
}


bool CTenantRlsPolicyBuilder::IsCrossTenantCatalogTable(const QByteArray& tableName)
{
	static const QByteArrayList catalogTableNames = {
				QByteArrayLiteral("tenants"),
				QByteArrayLiteral("tenantmemberships"),
				QByteArrayLiteral("tenantinvitations"),
				QByteArrayLiteral("tenantrelationships"),
				QByteArrayLiteral("tenantrelationshipproposals"),
				QByteArrayLiteral("tenantconnections"),
				QByteArrayLiteral("tenantconnectioncodes"),
				QByteArrayLiteral("tenantconnectionrequests"),
				QByteArrayLiteral("tenantpermissions"),
				QByteArrayLiteral("tenantentitybindings"),
				QByteArrayLiteral("crosstenantmessages"),
				QByteArrayLiteral("contracts"),
				QByteArrayLiteral("crossorggrants"),
				QByteArrayLiteral("orderrequests"),
				QByteArrayLiteral("usersessions"),
				QByteArrayLiteral("tenantstorage")};

	return catalogTableNames.contains(tableName.toLower());
}


} // namespace imtdb
