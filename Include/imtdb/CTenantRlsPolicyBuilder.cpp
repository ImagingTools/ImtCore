// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantRlsPolicyBuilder.h>


namespace imtdb
{


const QByteArray CTenantRlsPolicyBuilder::s_tenantIdSettingName = QByteArrayLiteral("imt.tenant_id");
const QByteArray CTenantRlsPolicyBuilder::s_userIdSettingName = QByteArrayLiteral("imt.user_id");
const QByteArray CTenantRlsPolicyBuilder::s_systemContextSettingName = QByteArrayLiteral("imt.rls_bypass");
const QByteArray CTenantRlsPolicyBuilder::s_isolationPolicyName = QByteArrayLiteral("ImtTenantIsolation");
const QByteArray CTenantRlsPolicyBuilder::s_userAccessPolicyName = QByteArrayLiteral("ImtTenantUserAccess");
const QByteArray CTenantRlsPolicyBuilder::s_tenantBindingsTableName = QByteArrayLiteral("TenantEntityBindings");


// public static methods

bool CTenantRlsPolicyBuilder::IsValidIdentifier(const QByteArray& identifier)
{
	if (identifier.isEmpty() || identifier.size() > 63){
		return false;
	}

	for (int i = 0; i < identifier.size(); ++i){
		const char ch = identifier.at(i);
		const bool isLetter = ((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z')) || (ch == '_');
		const bool isDigit = (ch >= '0') && (ch <= '9');
		if (!isLetter && !(isDigit && (i > 0))){
			return false;
		}
	}

	return true;
}


bool CTenantRlsPolicyBuilder::ParseTenantOwnedTableSpec(const QByteArray& spec, TenantOwnedTableInfo& info)
{
	info = TenantOwnedTableInfo();

	const QByteArrayList parts = spec.trimmed().split(':');
	if ((parts.size() < 2) || (parts.size() > 3)){
		return false;
	}

	const QByteArrayList tableParts = parts[0].trimmed().split('.');
	if (tableParts.size() == 1){
		info.schema = QByteArrayLiteral("public");
		info.tableName = tableParts[0].trimmed();
	}
	else if (tableParts.size() == 2){
		info.schema = tableParts[0].trimmed();
		info.tableName = tableParts[1].trimmed();
	}
	else{
		return false;
	}

	if (!IsValidIdentifier(info.schema) || !IsValidIdentifier(info.tableName)){
		return false;
	}

	const QByteArrayList tenantColumns = parts[1].split(',');
	for (const QByteArray& column : tenantColumns){
		const QByteArray trimmedColumn = column.trimmed();
		if (!IsValidIdentifier(trimmedColumn)){
			return false;
		}

		info.tenantColumns.append(trimmedColumn);
	}

	if (parts.size() == 3){
		info.userColumn = parts[2].trimmed();
		if (!IsValidIdentifier(info.userColumn)){
			return false;
		}
	}

	return true;
}


QByteArray CTenantRlsPolicyBuilder::CreateContextSyncQuery()
{
	return QByteArrayLiteral("SELECT set_config('") + s_tenantIdSettingName + QByteArrayLiteral("', :TenantId, false), ")
				+ QByteArrayLiteral("set_config('") + s_userIdSettingName + QByteArrayLiteral("', :UserId, false), ")
				+ QByteArrayLiteral("set_config('") + s_systemContextSettingName + QByteArrayLiteral("', :SystemContext, false)");
}


QByteArray CTenantRlsPolicyBuilder::CreateTenantOwnedTablePolicyQuery(const TenantOwnedTableInfo& info, bool allowSystemContext)
{
	if (!IsValidIdentifier(info.schema) || !IsValidIdentifier(info.tableName) || info.tenantColumns.isEmpty()){
		return QByteArray();
	}

	const QByteArray currentTenant = CreateCurrentTenantExpression();

	QByteArrayList tenantConditions;
	for (const QByteArray& column : info.tenantColumns){
		if (!IsValidIdentifier(column)){
			return QByteArray();
		}

		tenantConditions.append(QuoteIdentifier(column) + QByteArrayLiteral("::text = ") + currentTenant);
	}

	QByteArray condition = '(' + currentTenant + QByteArrayLiteral(" <> '' AND (") + tenantConditions.join(" OR ") + QByteArrayLiteral("))");
	if (allowSystemContext){
		condition = '(' + CreateSystemContextExpression() + QByteArrayLiteral(" OR ") + condition + ')';
	}

	const QByteArray tableName = CreateQualifiedTableName(info.schema, info.tableName);

	QByteArray retVal = CreateEnableRlsQuery(tableName);
	retVal += QByteArrayLiteral("DROP POLICY IF EXISTS ") + QuoteIdentifier(s_isolationPolicyName) + QByteArrayLiteral(" ON ") + tableName + QByteArrayLiteral(";\n");
	retVal += QByteArrayLiteral("DROP POLICY IF EXISTS ") + QuoteIdentifier(s_userAccessPolicyName) + QByteArrayLiteral(" ON ") + tableName + QByteArrayLiteral(";\n");
	retVal += QByteArrayLiteral("CREATE POLICY ") + QuoteIdentifier(s_isolationPolicyName) + QByteArrayLiteral(" ON ") + tableName
				+ QByteArrayLiteral(" AS PERMISSIVE FOR ALL USING ") + condition + QByteArrayLiteral(" WITH CHECK ") + condition + QByteArrayLiteral(";\n");

	if (info.userColumn.isEmpty()){
		return retVal;
	}

	if (!IsValidIdentifier(info.userColumn)){
		return QByteArray();
	}

	const QByteArray currentUser = CreateCurrentUserExpression();
	const QByteArray userCondition = '(' + currentUser + QByteArrayLiteral(" <> '' AND ") + QuoteIdentifier(info.userColumn) + QByteArrayLiteral("::text = ") + currentUser + ')';

	retVal += QByteArrayLiteral("CREATE POLICY ") + QuoteIdentifier(s_userAccessPolicyName) + QByteArrayLiteral(" ON ") + tableName
				+ QByteArrayLiteral(" AS PERMISSIVE FOR SELECT USING ") + userCondition + QByteArrayLiteral(";\n");

	return retVal;
}


QByteArray CTenantRlsPolicyBuilder::CreateBindingScopedTablePolicyQuery(const QByteArray& schema, const QByteArray& tableName, bool allowSystemContext)
{
	if (!IsValidIdentifier(schema) || !IsValidIdentifier(tableName)){
		return QByteArray();
	}

	const QByteArray currentTenant = CreateCurrentTenantExpression();
	const QByteArray qualifiedTableName = CreateQualifiedTableName(schema, tableName);
	const QByteArray bindingsTableName = CreateQualifiedTableName(schema, s_tenantBindingsTableName);

	// Table name is a validated identifier, so it can be used as string literal without escaping.
	const QByteArray bindingLookup = QByteArrayLiteral("SELECT 1 FROM ") + bindingsTableName + QByteArrayLiteral(" imtTenantBindings")
				+ QByteArrayLiteral(" WHERE imtTenantBindings.\"EntityType\" = '") + tableName + '\''
				+ QByteArrayLiteral(" AND imtTenantBindings.\"EntityId\" = ") + QuoteIdentifier(tableName) + QByteArrayLiteral(".\"DocumentId\"::text");

	const QByteArray boundToCurrentTenant = '(' + currentTenant + QByteArrayLiteral(" <> '' AND EXISTS (") + bindingLookup
				+ QByteArrayLiteral(" AND imtTenantBindings.\"TenantId\" = ") + currentTenant + QByteArrayLiteral("))");
	const QByteArray notBoundToAnyTenant = QByteArrayLiteral("NOT EXISTS (") + bindingLookup
				+ QByteArrayLiteral(" AND COALESCE(imtTenantBindings.\"TenantId\", '') <> '')");

	QByteArray condition = '(' + boundToCurrentTenant + QByteArrayLiteral(" OR ") + notBoundToAnyTenant + ')';
	if (allowSystemContext){
		condition = '(' + CreateSystemContextExpression() + QByteArrayLiteral(" OR ") + condition + ')';
	}

	QByteArray retVal = CreateEnableRlsQuery(qualifiedTableName);
	retVal += QByteArrayLiteral("DROP POLICY IF EXISTS ") + QuoteIdentifier(s_isolationPolicyName) + QByteArrayLiteral(" ON ") + qualifiedTableName + QByteArrayLiteral(";\n");
	retVal += QByteArrayLiteral("CREATE POLICY ") + QuoteIdentifier(s_isolationPolicyName) + QByteArrayLiteral(" ON ") + qualifiedTableName
				+ QByteArrayLiteral(" AS PERMISSIVE FOR ALL USING ") + condition + QByteArrayLiteral(" WITH CHECK ") + condition + QByteArrayLiteral(";\n");

	return retVal;
}


QByteArray CTenantRlsPolicyBuilder::CreateTenantBindingsTablePolicyQuery(const QByteArray& schema, bool allowSystemContext)
{
	if (!IsValidIdentifier(schema)){
		return QByteArray();
	}

	const QByteArray currentTenant = CreateCurrentTenantExpression();
	const QByteArray tableName = CreateQualifiedTableName(schema, s_tenantBindingsTableName);

	QByteArray condition = '(' + currentTenant + QByteArrayLiteral(" <> '' AND \"TenantId\" = ") + currentTenant + ')';
	if (allowSystemContext){
		condition = '(' + CreateSystemContextExpression() + QByteArrayLiteral(" OR ") + condition + ')';
	}

	const QByteArray readPolicyName = QuoteIdentifier(s_isolationPolicyName + QByteArrayLiteral("Read"));
	const QByteArray insertPolicyName = QuoteIdentifier(s_isolationPolicyName + QByteArrayLiteral("Insert"));
	const QByteArray updatePolicyName = QuoteIdentifier(s_isolationPolicyName + QByteArrayLiteral("Update"));
	const QByteArray deletePolicyName = QuoteIdentifier(s_isolationPolicyName + QByteArrayLiteral("Delete"));

	QByteArray retVal = CreateEnableRlsQuery(tableName);
	for (const QByteArray& policyName : {readPolicyName, insertPolicyName, updatePolicyName, deletePolicyName}){
		retVal += QByteArrayLiteral("DROP POLICY IF EXISTS ") + policyName + QByteArrayLiteral(" ON ") + tableName + QByteArrayLiteral(";\n");
	}

	retVal += QByteArrayLiteral("CREATE POLICY ") + readPolicyName + QByteArrayLiteral(" ON ") + tableName + QByteArrayLiteral(" AS PERMISSIVE FOR SELECT USING (true);\n");
	retVal += QByteArrayLiteral("CREATE POLICY ") + insertPolicyName + QByteArrayLiteral(" ON ") + tableName + QByteArrayLiteral(" AS PERMISSIVE FOR INSERT WITH CHECK ") + condition + QByteArrayLiteral(";\n");
	retVal += QByteArrayLiteral("CREATE POLICY ") + updatePolicyName + QByteArrayLiteral(" ON ") + tableName
				+ QByteArrayLiteral(" AS PERMISSIVE FOR UPDATE USING ") + condition + QByteArrayLiteral(" WITH CHECK ") + condition + QByteArrayLiteral(";\n");
	retVal += QByteArrayLiteral("CREATE POLICY ") + deletePolicyName + QByteArrayLiteral(" ON ") + tableName + QByteArrayLiteral(" AS PERMISSIVE FOR DELETE USING ") + condition + QByteArrayLiteral(";\n");

	return retVal;
}


// private static methods

QByteArray CTenantRlsPolicyBuilder::QuoteIdentifier(const QByteArray& identifier)
{
	return '"' + identifier + '"';
}


QByteArray CTenantRlsPolicyBuilder::CreateQualifiedTableName(const QByteArray& schema, const QByteArray& tableName)
{
	return QuoteIdentifier(schema) + '.' + QuoteIdentifier(tableName);
}


QByteArray CTenantRlsPolicyBuilder::CreateCurrentTenantExpression()
{
	return QByteArrayLiteral("COALESCE(current_setting('") + s_tenantIdSettingName + QByteArrayLiteral("', true), '')");
}


QByteArray CTenantRlsPolicyBuilder::CreateCurrentUserExpression()
{
	return QByteArrayLiteral("COALESCE(current_setting('") + s_userIdSettingName + QByteArrayLiteral("', true), '')");
}


QByteArray CTenantRlsPolicyBuilder::CreateSystemContextExpression()
{
	return QByteArrayLiteral("(COALESCE(current_setting('") + s_systemContextSettingName + QByteArrayLiteral("', true), '') = 'on')");
}


QByteArray CTenantRlsPolicyBuilder::CreateEnableRlsQuery(const QByteArray& qualifiedTableName)
{
	return QByteArrayLiteral("ALTER TABLE ") + qualifiedTableName + QByteArrayLiteral(" ENABLE ROW LEVEL SECURITY;\n")
				+ QByteArrayLiteral("ALTER TABLE ") + qualifiedTableName + QByteArrayLiteral(" FORCE ROW LEVEL SECURITY;\n");
}


} // namespace imtdb


