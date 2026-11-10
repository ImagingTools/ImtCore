// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CProductCatalogDatabaseDelegateComp.h>


// ImtCore includes
#include <imtauth/IConnectedProduct.h>
#include <imtauth/CProductPermissionSnapshot.h>
#include <imtauthdb/ProductCatalogSql.h>
#include <imtdb/imtdb.h>


namespace imtauthdb
{


QByteArray CProductCatalogDatabaseDelegateComp::GetCountQuery(const iprm::IParamsSet* paramsPtr) const
{
	return m_isSchemaReady ? BaseClass::GetCountQuery(paramsPtr) : QByteArray();
}


QByteArray CProductCatalogDatabaseDelegateComp::GetSelectionQuery(
			const QByteArray& objectId, int offset, int count, const iprm::IParamsSet* paramsPtr) const
{
	return m_isSchemaReady ? BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr) : QByteArray();
}


bool CProductCatalogDatabaseDelegateComp::CreateTableIfNeeded()
{
	m_isSchemaReady = false;
	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	if (!BaseClass::CreateTableIfNeeded()){
		return false;
	}

	const QString table = QualifiedTable();
	if (!m_databaseEngineCompPtr->BeginTransaction()){
		return false;
	}

	QSqlError error;
	QSqlQuery columns = m_databaseEngineCompPtr->ExecSqlQuery(QStringLiteral("SELECT * FROM %1 LIMIT 0").arg(table).toUtf8(), &error);
	bool success = error.type() == QSqlError::NoError && columns.isActive();
	const bool hasProductId = columns.record().contains("ProductId");
	columns.finish();
	if (success && !hasProductId){
		success = ExecuteSchemaQuery(QStringLiteral("ALTER TABLE %1 ADD COLUMN \"ProductId\" VARCHAR").arg(table));
	}
	if (success){
		const QString productId = BaseClass::CreateJsonExtractSql("Document", "ProductId");
		success = !productId.isEmpty() && ExecuteSchemaQuery(
					QStringLiteral("UPDATE %1 SET \"ProductId\" = %2 WHERE \"ProductId\" IS NULL").arg(table, productId));
	}
	if (success){
		QSqlQuery invalid = m_databaseEngineCompPtr->ExecSqlQuery(
					QStringLiteral("SELECT COUNT(*) FROM %1 WHERE \"State\" = 'Active' AND (\"ProductId\" IS NULL OR \"ProductId\" = '')").arg(table).toUtf8(), &error);
		success = error.type() == QSqlError::NoError && invalid.next() && invalid.value(0).toLongLong() == 0;
	}
	const QByteArray schema = GetTableScheme();
	const QString indexPrefix = IsSqliteDriver() && !schema.isEmpty() ? imtdb::QuoteIdentifier(QString::fromUtf8(schema)) + "." : QString();
	const QString indexTable = IsSqliteDriver() ? imtdb::QuoteIdentifier(QString::fromUtf8(GetTableName())) : table;
	if (success){
		// Old and disabled revisions keep their product ID without taking part in the uniqueness
		success = ExecuteSchemaQuery(ProductCatalogSql::ActiveProductIndex(
					indexPrefix + imtdb::QuoteIdentifier(QString::fromUtf8(GetTableName()) + "_ActiveProductId"), indexTable));
	}
	if (success){
		success = ExecuteSchemaQuery(QStringLiteral("CREATE INDEX IF NOT EXISTS %1 ON %2 (\"ProductId\", \"State\")")
					.arg(indexPrefix + imtdb::QuoteIdentifier(QString::fromUtf8(GetTableName()) + "_ProductId"), indexTable));
	}
	if (!success){
		m_databaseEngineCompPtr->CancelTransaction();
		SendErrorMessage(0, QStringLiteral("Cannot enforce product uniqueness in %1. Existing data was preserved; repair duplicate or invalid active ProductIds before retrying.").arg(table));

		return false;
	}

	m_isSchemaReady = m_databaseEngineCompPtr->FinishTransaction();

	return m_isSchemaReady;
}


QVariantMap CProductCatalogDatabaseDelegateComp::GetAdditionalInsertValues(const istd::IChangeable& object) const
{
	return {{"ProductId", QString::fromUtf8(GetProductId(object))}};
}


QByteArray CProductCatalogDatabaseDelegateComp::PrepareInsertNewObjectQuery(
			const QByteArray& typeId,
			const QByteArray& objectId,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* operationContextPtr,
			const QVariant& revisionArgument) const
{
	if (!m_isSchemaReady || GetProductId(object).isEmpty()){
		return QByteArray();
	}

	return BaseClass::PrepareInsertNewObjectQuery(typeId, objectId, objectName, objectDescription, object, operationContextPtr, revisionArgument);
}


QString CProductCatalogDatabaseDelegateComp::CreateJsonExtractSql(
			const QString& jsonName, const QString& key, QMetaType::Type metaType, const QString& tableAlias) const
{
	if (jsonName == "Document" && key == "ProductId"){
		return tableAlias.isEmpty() ? QStringLiteral("\"ProductId\"") : tableAlias + QStringLiteral(".\"ProductId\"");
	}

	return BaseClass::CreateJsonExtractSql(jsonName, key, metaType, tableAlias);
}


QString CProductCatalogDatabaseDelegateComp::QualifiedTable() const
{
	const QByteArray schema = GetTableScheme();

	return (schema.isEmpty() ? QString() : imtdb::QuoteIdentifier(QString::fromUtf8(schema)) + ".") + imtdb::QuoteIdentifier(QString::fromUtf8(GetTableName()));
}


bool CProductCatalogDatabaseDelegateComp::ExecuteSchemaQuery(const QString& query) const
{
	QSqlError error;
	QSqlQuery result = m_databaseEngineCompPtr->ExecSqlQuery(query.toUtf8(), &error);
	if (error.type() != QSqlError::NoError || !result.isActive()){
		SendErrorMessage(0, error.text());

		return false;
	}

	return true;
}


QByteArray CProductCatalogDatabaseDelegateComp::GetProductId(const istd::IChangeable& object) const
{
	const imtauth::IConnectedProduct* productPtr = dynamic_cast<const imtauth::IConnectedProduct*>(&object);
	if (productPtr != nullptr){
		return productPtr->GetProductId();
	}
	const imtauth::CProductPermissionSnapshot* snapshotPtr = dynamic_cast<const imtauth::CProductPermissionSnapshot*>(&object);

	return snapshotPtr != nullptr ? snapshotPtr->GetProductId() : QByteArray();
}


} // namespace imtauthdb
