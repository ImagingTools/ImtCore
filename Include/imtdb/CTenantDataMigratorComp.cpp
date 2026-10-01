// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CTenantDataMigratorComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QRegularExpression>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// ImtCore includes
#include <imtbase/CTenantContextScope.h>
#include <imtdb/CSqlDatabaseFileDocumentDelegateComp.h>
#include <imtdb/CTenantDataMigrator.h>
#include <imtdb/CTenantStorageDbStore.h>


namespace imtdb
{


// reimplemented (imtdb::ITenantDataMigrator)

bool CTenantDataMigratorComp::MigrateTenantData(const QByteArray& tenantId)
{
	if (tenantId.isEmpty()){
		SendErrorMessage(0, QStringLiteral("Tenant data migration rejected: empty tenant ID"), "CTenantDataMigratorComp");

		return false;
	}

	TenantStorageInfo storageInfo;
	if (!m_storageResolverCompPtr->ResolveTenantStorage(tenantId, storageInfo)){
		SendErrorMessage(0, QStringLiteral("Tenant data migration failed: storage of tenant '%1' could not be resolved").arg(QString(tenantId)), "CTenantDataMigratorComp");

		return false;
	}

	if (storageInfo.storageKind != TSK_OWN_SCHEMA || storageInfo.schemaName.isEmpty()){
		SendErrorMessage(0, QStringLiteral("Tenant data migration failed: tenant '%1' has no dedicated schema").arg(QString(tenantId)), "CTenantDataMigratorComp");

		return false;
	}

	if (storageInfo.status != TSS_ACTIVE && storageInfo.status != TSS_MIGRATING){
		SendErrorMessage(0, QStringLiteral("Tenant data migration failed: storage of tenant '%1' is not active").arg(QString(tenantId)), "CTenantDataMigratorComp");

		return false;
	}

	TenantStorageStatus previousStatus = storageInfo.status;

	storageInfo.status = TSS_MIGRATING;
	if (!PersistStatus(tenantId, storageInfo) || !m_storageResolverCompPtr->RegisterTenantStorage(tenantId, storageInfo)){
		SendErrorMessage(0, QStringLiteral("Tenant data migration failed: status of tenant '%1' could not be updated").arg(QString(tenantId)), "CTenantDataMigratorComp");

		return false;
	}

	// without the tenant bound, RLS hides the source rows and zero copied rows would "verify"
	imtbase::CTenantContextScope tenantContextScope(tenantId);

	CTenantDataMigrator migrator(*m_databaseEngineCompPtr);
	migrator.SetSourceSchema(m_sourceSchemaAttrPtr.IsValid() ? *m_sourceSchemaAttrPtr : QByteArrayLiteral("public"));
	migrator.SetTenantIdColumn(m_tenantIdColumnAttrPtr.IsValid() ? *m_tenantIdColumnAttrPtr : QByteArrayLiteral("TenantId"));
	migrator.SetChecksumVerificationEnabled(m_verifyChecksumsAttrPtr.IsValid() && *m_verifyChecksumsAttrPtr);

	for (int tableIndex = 0; tableIndex < m_tableNamesAttrPtr.GetCount(); ++tableIndex){
		QByteArray tableName = m_tableNamesAttrPtr[tableIndex];

		bool isFileDocumentTable = false;
		for (int fileTableIndex = 0; fileTableIndex < m_fileDocumentTableNamesAttrPtr.GetCount(); ++fileTableIndex){
			isFileDocumentTable = isFileDocumentTable || (m_fileDocumentTableNamesAttrPtr[fileTableIndex] == tableName);
		}

		int migratedRowCount = 0;
		QString errorMessage;
		if (	!migrator.MigrateTable(tableName, tenantId, storageInfo.schemaName, migratedRowCount, errorMessage) ||
				(isFileDocumentTable && !CopyReferencedFiles(tableName, storageInfo.schemaName, errorMessage))){
			SendErrorMessage(0, QStringLiteral("Tenant data migration for tenant '%1' failed: %2").arg(QString(tenantId), errorMessage), "CTenantDataMigratorComp");

			storageInfo.status = previousStatus;
			PersistStatus(tenantId, storageInfo);
			m_storageResolverCompPtr->RegisterTenantStorage(tenantId, storageInfo);

			return false;
		}

		SendInfoMessage(0, QStringLiteral("Migrated %1 rows of table '%2' for tenant '%3' into schema '%4'").arg(migratedRowCount).arg(QString(tableName), QString(tenantId), QString(storageInfo.schemaName)), "CTenantDataMigratorComp");
	}

	if (m_purgeSourceRowsAttrPtr.IsValid() && *m_purgeSourceRowsAttrPtr){
		for (int tableIndex = 0; tableIndex < m_tableNamesAttrPtr.GetCount(); ++tableIndex){
			QByteArray tableName = m_tableNamesAttrPtr[tableIndex];

			QString errorMessage;
			if (!migrator.RemoveSourceRows(tableName, tenantId, errorMessage)){
				SendWarningMessage(0, QStringLiteral("Cleanup of migrated rows for tenant '%1' failed: %2").arg(QString(tenantId), errorMessage), "CTenantDataMigratorComp");
			}
		}
	}

	storageInfo.status = TSS_ACTIVE;
	if (!PersistStatus(tenantId, storageInfo) || !m_storageResolverCompPtr->RegisterTenantStorage(tenantId, storageInfo)){
		SendErrorMessage(0, QStringLiteral("Tenant data migration for tenant '%1' completed, but the status could not be set to active").arg(QString(tenantId)), "CTenantDataMigratorComp");

		return false;
	}

	SendInfoMessage(0, QStringLiteral("Tenant data migration for tenant '%1' completed").arg(QString(tenantId)), "CTenantDataMigratorComp");

	return true;
}


bool CTenantDataMigratorComp::MigrateAllTenants()
{
	bool retVal = true;

	QByteArrayList tenantIds = m_storageResolverCompPtr->GetRegisteredTenantIds();
	for (const QByteArray& tenantId: tenantIds){
		TenantStorageInfo storageInfo;
		if (!m_storageResolverCompPtr->ResolveTenantStorage(tenantId, storageInfo)){
			continue;
		}

		if (storageInfo.storageKind != TSK_OWN_SCHEMA){
			// shared-storage tenants have nothing to migrate
			continue;
		}

		if (!MigrateTenantData(tenantId)){
			retVal = false;
		}
	}

	return retVal;
}


// private methods

bool CTenantDataMigratorComp::CopyReferencedFiles(const QByteArray& tableName, const QByteArray& schemaName, QString& errorMessage) const
{
	static const QRegularExpression contentHashPattern(QStringLiteral("^[0-9a-f]{64}$"));

	if (!m_fileStorageRootCompPtr.IsValid() || m_fileStorageRootCompPtr->GetPath().isEmpty()){
		errorMessage = QStringLiteral("File document table '%1' requires the attribute 'FileStorageRoot'").arg(QString(tableName));

		return false;
	}

	QByteArray quotedSchema = CTenantDataMigrator::QuoteIdentifier(schemaName);
	QByteArray quotedTable = CTenantDataMigrator::QuoteIdentifier(tableName);
	if (quotedSchema.isEmpty() || quotedTable.isEmpty()){
		errorMessage = QStringLiteral("Invalid table or schema identifier");

		return false;
	}

	QSqlError sqlError;
	QSqlQuery query = m_databaseEngineCompPtr->ExecSqlQuery(
				QByteArrayLiteral("SELECT DISTINCT \"") + CSqlDatabaseDocumentDelegateCompBase::s_documentColumn + QByteArrayLiteral("\" FROM ") + quotedSchema + '.' + quotedTable,
				&sqlError,
				true);
	if (sqlError.type() != QSqlError::NoError){
		errorMessage = QStringLiteral("Reading the store descriptors of '%1' failed: %2").arg(QString(tableName), sqlError.text());

		return false;
	}

	const QString sharedStorePath = m_fileStorageRootCompPtr->GetPath();
	const QString tenantStorePath = CSqlDatabaseFileDocumentDelegateComp::GetTenantStorePath(sharedStorePath, schemaName);

	int copiedCount = 0;
	while (query.next()){
		const QByteArray descriptorValue = query.value(0).toByteArray();
		if (descriptorValue.isEmpty()){
			continue;
		}

		const QString contentHash = QJsonDocument::fromJson(descriptorValue).object().value(QStringLiteral("hash")).toString();
		if (!contentHashPattern.match(contentHash).hasMatch()){
			// a row the store cannot attribute would become unreadable in the tenant store
			errorMessage = QStringLiteral("Table '%1' contains a row without a valid store descriptor").arg(QString(tableName));

			return false;
		}

		const QString relativePath = contentHash.left(2) + '/' + contentHash + QStringLiteral(".bin");
		const QString sourceFilePath = QDir(sharedStorePath).filePath(relativePath);
		const QString targetFilePath = QDir(tenantStorePath).filePath(relativePath);

		// content-addressed: an existing target of the same size is the same content
		const QFileInfo targetInfo(targetFilePath);
		const QFileInfo sourceInfo(sourceFilePath);
		if (targetInfo.exists() && (targetInfo.size() == sourceInfo.size())){
			continue;
		}

		if (!sourceInfo.exists()){
			errorMessage = QStringLiteral("Referenced document content '%1' is missing from the shared store").arg(sourceFilePath);

			return false;
		}

		QFile::remove(targetFilePath);
		if (!QDir().mkpath(targetInfo.absolutePath()) || !QFile::copy(sourceFilePath, targetFilePath)){
			errorMessage = QStringLiteral("Copying document content '%1' into the tenant store failed").arg(sourceFilePath);

			return false;
		}

		++copiedCount;
	}

	SendInfoMessage(0, QStringLiteral("Copied %1 document content file(s) of table '%2' into '%3'").arg(copiedCount).arg(QString(tableName), tenantStorePath), "CTenantDataMigratorComp");

	return true;
}


bool CTenantDataMigratorComp::PersistStatus(const QByteArray& tenantId, const TenantStorageInfo& info) const
{
	CTenantStorageDbStore dbStore(*m_databaseEngineCompPtr, m_registryTableSchemaAttrPtr.IsValid() ? *m_registryTableSchemaAttrPtr : QByteArray());

	return dbStore.SaveAssignment(tenantId, info);
}


} // namespace imtdb
