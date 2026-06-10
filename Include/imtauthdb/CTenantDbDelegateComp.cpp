// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CTenantDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

// ACF includes
#include <istd/TOptDelPtr.h>
#include <imtauth/ITenantInfo.h>
#include <iprm/IIdParam.h>
#include <iprm/ITextParam.h>
#include <iprm/CParamsSet.h>
#include <iprm/TParamsPtr.h>
#include <imtbase/ICollectionFilter.h>
#include <imtbase/IComplexCollectionFilter.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtauthdb
{


namespace
{

const int s_invitationStatusPending = 0;

} // namespace


// reimplemented (icomp::CComponentBase)

void CTenantDbDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!CreatePermissionsTableIfNeeded()){
		qWarning() << "CTenantDbDelegateComp: TenantPermissions table auto-creation failed";
	}

	const QByteArray membershipsScriptPath = m_createMembershipsTableScriptPathAttrPtr.IsValid()
			? *m_createMembershipsTableScriptPathAttrPtr : QByteArray();
	const bool autoCreateMemberships = m_autoCreateMembershipsTableAttrPtr.IsValid() && *m_autoCreateMembershipsTableAttrPtr;
	if (!CreateAuxTableIfNeeded(autoCreateMemberships, QStringLiteral("TenantMemberships"), membershipsScriptPath)){
		qWarning() << "CTenantDbDelegateComp: TenantMemberships table auto-creation failed";
	}

	const QByteArray invitationsScriptPath = m_createInvitationsTableScriptPathAttrPtr.IsValid()
			? *m_createInvitationsTableScriptPathAttrPtr : QByteArray();
	const bool autoCreateInvitations = m_autoCreateInvitationsTableAttrPtr.IsValid() && *m_autoCreateInvitationsTableAttrPtr;
	if (!CreateAuxTableIfNeeded(autoCreateInvitations, QStringLiteral("TenantInvitations"), invitationsScriptPath)){
		qWarning() << "CTenantDbDelegateComp: TenantInvitations table auto-creation failed";
	}

	const QByteArray relationshipsScriptPath = m_createRelationshipsTableScriptPathAttrPtr.IsValid()
			? *m_createRelationshipsTableScriptPathAttrPtr : QByteArray();
	const bool autoCreateRelationships = m_autoCreateRelationshipsTableAttrPtr.IsValid() && *m_autoCreateRelationshipsTableAttrPtr;
	const QString relationshipsTableName = m_relationshipsTableNameAttrPtr.IsValid()
			? QString::fromUtf8(*m_relationshipsTableNameAttrPtr) : QStringLiteral("TenantRelationships");
	if (!CreateAuxTableIfNeeded(autoCreateRelationships, relationshipsTableName, relationshipsScriptPath)){
		qWarning() << "CTenantDbDelegateComp: TenantRelationships table auto-creation failed";
	}
}


istd::IChangeableUniquePtr CTenantDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid() || !m_tenantFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::ITenantInfo> tenantPtr = m_tenantFactCompPtr.CreateInstance();
	if (!tenantPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		tenantPtr->SetTenantId(imtdb::VariantToByteArray(record.value("Id")));
	}
	if (record.contains("Name")){
		tenantPtr->SetTenantName(record.value("Name").toString());
	}
	if (record.contains("Description")){
		tenantPtr->SetTenantDescription(record.value("Description").toString());
	}
	if (record.contains("OwnerId")){
		tenantPtr->SetOwnerId(imtdb::VariantToByteArray(record.value("OwnerId")));
	}
	if (record.contains("CreatorId")){
		tenantPtr->SetCreatorId(imtdb::VariantToByteArray(record.value("CreatorId")));
	}
	if (record.contains("IsActive")){
		tenantPtr->SetActive(record.value("IsActive").toBool());
	}
	if (record.contains("CreatedAt")){
		QVariant val = record.value("CreatedAt");
		QDateTime dt = val.toDateTime();
		tenantPtr->SetCreatedAt(dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString());
	}
	if (record.contains("UpdatedAt")){
		QVariant val = record.value("UpdatedAt");
		QDateTime dt = val.toDateTime();
		tenantPtr->SetUpdatedAt(dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString());
	}

	// Load permissions from the TenantPermissions table
	if (record.contains("Id")){
		QByteArrayList permissions = LoadTenantPermissions(imtdb::VariantToByteArray(record.value("Id")));
		tenantPtr->SetTenantPermissions(permissions);
	}

	if (record.contains("ParentTenantId")){
		tenantPtr->SetParentTenantId(imtdb::VariantToByteArray(record.value("ParentTenantId")));
	}

	// Load relationship IDs from the TenantRelationships table
	if (record.contains("Id")){
		QByteArrayList relationshipIds = LoadTenantRelationshipIds(imtdb::VariantToByteArray(record.value("Id")));
		tenantPtr->SetRelationshipIds(relationshipIds);
	}

	return tenantPtr;
}


CTenantDbDelegateComp::NewObjectQuery CTenantDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& objectName,
		const QString& objectDescription,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery result;

	const imtauth::ITenantInfo* tenantPtr = dynamic_cast<const imtauth::ITenantInfo*>(valuePtr);

	QString id;
	if (!proposedObjectId.isEmpty()){
		id = imtdb::EscapeSql(QString::fromUtf8(proposedObjectId));
	}
	else{
		id = QUuid::createUuid().toString(QUuid::WithoutBraces);
	}

	QString name = imtdb::EscapeSql(tenantPtr != nullptr ? tenantPtr->GetTenantName() : objectName);
	QString description = imtdb::EscapeSql(tenantPtr != nullptr ? tenantPtr->GetTenantDescription() : objectDescription);
	QString ownerId = imtdb::EscapeSql(tenantPtr != nullptr ? QString::fromUtf8(tenantPtr->GetOwnerId()) : QString());
	QString creatorId = imtdb::EscapeSql(tenantPtr != nullptr ? QString::fromUtf8(tenantPtr->GetCreatorId()) : QString());
	bool isActive = tenantPtr != nullptr ? tenantPtr->IsActive() : true;
	QString parentTenantId = imtdb::EscapeSql(tenantPtr != nullptr ? QString::fromUtf8(tenantPtr->GetParentTenantId()) : QString());
	QString now = imtdb::UtcNow();

	result.query = QString(
		"INSERT INTO \"%1\" (\"Id\", \"Name\", \"Description\", \"OwnerId\", \"CreatorId\", \"IsActive\", \"CreatedAt\", \"UpdatedAt\", "
		"\"ParentTenantId\") "
		"VALUES ('%2', '%3', '%4', '%5', '%6', %7, '%8', '%9', '%10');")
		.arg(*m_tableNameAttrPtr)
		.arg(id)
		.arg(name)
		.arg(description)
		.arg(ownerId)
		.arg(creatorId)
		.arg(isActive ? "true" : "false")
		.arg(now)
		.arg(now)
		.arg(parentTenantId).toUtf8();

	// Append permissions insert if tenant has permissions
	if (tenantPtr != nullptr){
		QByteArrayList permissions = tenantPtr->GetTenantPermissions();
		if (!permissions.isEmpty()){
			result.query += CreatePermissionsInsertQuery(id.toUtf8(), permissions);
		}
		QByteArrayList relationshipIds = tenantPtr->GetRelationshipIds();
		if (!relationshipIds.isEmpty()){
			result.query += CreateRelationshipIdsInsertQuery(id.toUtf8(), relationshipIds);
		}
	}

	return result;
}


QByteArray CTenantDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtauth::ITenantInfo* tenantPtr = dynamic_cast<const imtauth::ITenantInfo*>(&object);
	if (tenantPtr == nullptr){
		return QByteArray();
	}

	QString escapedId = imtdb::EscapeSql(QString::fromUtf8(objectId));
	QString now = imtdb::UtcNow();

	return QString(
		"UPDATE \"%1\" SET "
		"\"Name\"='%2', "
		"\"Description\"='%3', "
		"\"OwnerId\"='%4', "
		"\"CreatorId\"='%5', "
		"\"IsActive\"=%6, "
		"\"UpdatedAt\"='%7', "
		"\"ParentTenantId\"='%8' "
		"WHERE \"Id\"='%9';")
		.arg(*m_tableNameAttrPtr)
		.arg(imtdb::EscapeSql(tenantPtr->GetTenantName()))
		.arg(imtdb::EscapeSql(tenantPtr->GetTenantDescription()))
		.arg(imtdb::EscapeSql(QString::fromUtf8(tenantPtr->GetOwnerId())))
		.arg(imtdb::EscapeSql(QString::fromUtf8(tenantPtr->GetCreatorId())))
		.arg(tenantPtr->IsActive() ? "true" : "false")
		.arg(now)
		.arg(imtdb::EscapeSql(QString::fromUtf8(tenantPtr->GetParentTenantId())))
		.arg(escapedId).toUtf8()
		+ CreatePermissionsDeleteQuery(objectId)
		+ CreatePermissionsInsertQuery(objectId, tenantPtr->GetTenantPermissions())
		+ CreateRelationshipIdsDeleteQuery(objectId)
		+ CreateRelationshipIdsInsertQuery(objectId, tenantPtr->GetRelationshipIds());
}


QByteArray CTenantDbDelegateComp::CreateDeleteObjectsQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArrayList& objectIds,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectIds.isEmpty()){
		return QByteArray();
	}

	QStringList escapedIds;
	for (const QByteArray& id : objectIds){
		escapedIds << QString("'%1'").arg(imtdb::EscapeSql(QString::fromUtf8(id)));
	}

	QString idsJoined = escapedIds.join(", ");

	QByteArray result;
	// Delete permissions first (explicit for DBs where FK CASCADE may not be enforced)
	for (const QByteArray& id : objectIds){
		result += CreatePermissionsDeleteQuery(id);
		result += CreateRelationshipIdsDeleteQuery(id);
	}

	result += QString("DELETE FROM \"%1\" WHERE \"Id\" IN (%2);")
				.arg(*m_tableNameAttrPtr, idsJoined).toUtf8();

	return result;
}


QByteArray CTenantDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTenantDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const QString& newObjectName,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QString("UPDATE \"%1\" SET \"Name\"='%2', \"UpdatedAt\"='%3' WHERE \"Id\"='%4';")
			.arg(*m_tableNameAttrPtr,
				 imtdb::EscapeSql(newObjectName),
				 imtdb::UtcNow(),
				 imtdb::EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


QByteArray CTenantDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const QString& description,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QString("UPDATE \"%1\" SET \"Description\"='%2', \"UpdatedAt\"='%3' WHERE \"Id\"='%4';")
			.arg(*m_tableNameAttrPtr,
				 imtdb::EscapeSql(description),
				 imtdb::UtcNow(),
				 imtdb::EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


bool CTenantDbDelegateComp::SetCollectionItemMetaInfoFromRecord(
		const QSqlRecord& record,
		idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("Name")){
		metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_NAME, record.value("Name").toString());
	}

	if (record.contains("Description")){
		metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION, record.value("Description").toString());
	}

	if (record.contains("UpdatedAt")){
		QDateTime lastModificationTime = record.value("UpdatedAt").toDateTime();
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, lastModificationTime);
	}

	if (record.contains("CreatedAt")){
		QDateTime createdAtTime = record.value("CreatedAt").toDateTime();
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, createdAtTime);
	}

	return true;
}


idoc::MetaInfoPtr CTenantDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo);
	return metaInfoPtr;
}


bool CTenantDbDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


QByteArray CTenantDbDelegateComp::ExtractUserId(const iprm::IParamsSet* paramsPtr)
{
	if (paramsPtr == nullptr){
		return QByteArray();
	}

	iprm::IParamsSet::Ids paramIds = paramsPtr->GetParamIds();
	if (paramIds.contains("UserId")){
		iprm::TParamsPtr<iprm::IIdParam> userIdParamPtr(paramsPtr, "UserId");
		if (userIdParamPtr.IsValid()){
			return userIdParamPtr->GetId();
		}
	}

	return QByteArray();
}


QString CTenantDbDelegateComp::GetTenantRelationScopeSubquery(const QByteArray& userId) const
{
	QString tableName = qPrintable(*m_tableNameAttrPtr);
	if (userId.isEmpty()){
		return QString("SELECT *, NULL AS \"TenantRelationScope\" FROM \"%1\"").arg(tableName);
	}

	QString escapedUserId = imtdb::EscapeSql(QString::fromUtf8(userId));
	const QByteArray driverId = m_databaseEngineCompPtr->GetDatabaseDriverId();
	const bool isSQLite = (driverId == "QSQLITE");

	// "TenantId"::uuid — PostgreSQL-only cast, SQLite stores UUID as TEXT
	const QString tenantIdCast = isSQLite
								 ? QString("\"TenantId\"")
								 : QString("\"TenantId\"::uuid");

	return QString(
				"SELECT *, "
				"CASE "
				"WHEN \"CreatorId\"='%1' THEN 'Creator' "
				"WHEN \"OwnerId\"='%1' THEN 'Owner' "
				"WHEN \"Id\" IN (SELECT \"TenantId\" FROM \"TenantMemberships\" WHERE \"UserId\"='%1' AND \"IsActive\"=true) THEN 'Member' "
				"WHEN \"Id\" IN (SELECT %2 FROM \"TenantInvitations\" WHERE \"UserId\"='%1' AND \"Status\"=%3) THEN 'Invited' "
				"ELSE NULL "
				"END AS \"TenantRelationScope\" "
				"FROM \"%4\"")
			.arg(escapedUserId)
			.arg(tenantIdCast)
			.arg(s_invitationStatusPending)
			.arg(tableName);
}


QByteArray CTenantDbDelegateComp::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		return BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);
	}

	QByteArray userId = ExtractUserId(paramsPtr);
	if (count == 0){
		return QByteArray();
	}

	QString sortQuery;
	QString filterQuery;
	istd::TOptDelPtr<const iprm::IParamsSet> selectionParamsPtr;
	if (paramsPtr != nullptr){
		selectionParamsPtr.SetPtr(paramsPtr, false);
	}
	else{
		selectionParamsPtr.SetPtr(new iprm::CParamsSet(), true);
	}

	if (selectionParamsPtr.IsValid()){
		if (!CreateFilterQuery(*selectionParamsPtr, filterQuery)){
			return QByteArray();
		}

		iprm::IParamsSet::Ids paramIds = selectionParamsPtr->GetParamIds();
		if (paramIds.contains("ComplexFilter")){
			iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(selectionParamsPtr.GetPtr(), "ComplexFilter");
			if (!CreateSortQuery(*complexFilterParamPtr, sortQuery)){
				return QByteArray();
			}
		}
		else if (paramIds.contains("Filter")){
			iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(selectionParamsPtr.GetPtr(), "Filter");
			if (!CreateSortQuery(*collectionFilterParamPtr, sortQuery)){
				return QByteArray();
			}
		}
	}

	QByteArray paginationQuery;
	if (!CreatePaginationQuery(offset, count, paginationQuery)){
		return QByteArray();
	}

	QString baseQuery = QString("SELECT * FROM (%1) AS _t").arg(GetTenantRelationScopeSubquery(userId));
	const QByteArray driverId = m_databaseEngineCompPtr->GetDatabaseDriverId();
	const bool isSQLite = (driverId == "QSQLITE");
	QString retVal;
	if (isSQLite){
		// SQLite does not support parentheses around a top-level SELECT statement
		retVal = baseQuery;
		retVal += QString(" ") + filterQuery;
		retVal += QString(" ") + sortQuery;
		retVal += QString(" ") + qPrintable(paginationQuery);
	}
	else{
		// PostgreSQL: wrap in parens as workaround for Qt bug with '%<NUMBER>%' pattern
		retVal = "(" + baseQuery;
		retVal += QString(" ") + filterQuery;
		retVal += QString(" ") + qPrintable(paginationQuery) + ")";
		retVal += QString(" ") + sortQuery;
	}

	return retVal.toUtf8();
}


QByteArray CTenantDbDelegateComp::GetCountQuery(const iprm::IParamsSet* paramsPtr) const
{
	QByteArray userId = ExtractUserId(paramsPtr);

	QString filterQuery;
	if (paramsPtr != nullptr){
		if (!CreateFilterQuery(*paramsPtr, filterQuery)){
			return QByteArray();
		}
	}

	return QString("SELECT COUNT(*) FROM (%1) AS _t %2")
			.arg(GetTenantRelationScopeSubquery(userId))
			.arg(filterQuery)
			.toUtf8();
}


QString CTenantDbDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	QStringList additionalFilters;

	QByteArray userId = ExtractUserId(&filterParams);
	if (!userId.isEmpty()){
		// With TenantRelationScope as a computed column in the subquery,
		// only show tenants where the user has a relationship (Owner, Member, or Invited)
		additionalFilters << QStringLiteral("\"TenantRelationScope\" IS NOT NULL");
	}

	iprm::TParamsPtr<iprm::ITextParam> tenantNameParamPtr(&filterParams, "TenantName");
	if (tenantNameParamPtr.IsValid()){
		const QString tenantName = tenantNameParamPtr->GetText().trimmed();
		if (!tenantName.isEmpty()){
			additionalFilters << QString("\"Name\"='%1'").arg(imtdb::EscapeSql(tenantName));
		}
	}

	iprm::TParamsPtr<iprm::IIdParam> excludedTenantIdParamPtr(&filterParams, "ExcludedTenantId");
	if (excludedTenantIdParamPtr.IsValid()){
		const QByteArray excludedTenantId = excludedTenantIdParamPtr->GetId();
		if (!excludedTenantId.isEmpty()){
			additionalFilters << QString("\"Id\"<>'%1'").arg(imtdb::EscapeSql(QString::fromUtf8(excludedTenantId)));
		}
	}

	return additionalFilters.join(QStringLiteral(" AND "));
}


QByteArray CTenantDbDelegateComp::GetProductId() const
{
	return m_productIdAttrPtr.IsValid() ? *m_productIdAttrPtr : QByteArray();
}


bool CTenantDbDelegateComp::CreateAuxTableIfNeeded(bool autoCreate, const QString& tableName, const QByteArray& scriptPath)
{
	if (!autoCreate){
		return true;
	}

	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	if (tableName.isEmpty() || scriptPath.isEmpty()){
		return false;
	}

	if (TableExists(tableName)){
		return true;
	}

	QString resourcePath = QString::fromUtf8(scriptPath);
	if (!resourcePath.startsWith(QStringLiteral(":/"))){
		resourcePath = imtdb::GetSqlResourcePath(*m_databaseEngineCompPtr, resourcePath);
	}

	QFile scriptFile(resourcePath);
	if (!scriptFile.open(QFile::ReadOnly)){
		qWarning() << "CTenantDbDelegateComp: table creation script" << scriptFile.fileName() << "could not be loaded";
		return false;
	}

	QByteArray createTableQuery = scriptFile.readAll();
	scriptFile.close();

	QByteArray tableScheme = GetTableScheme();
	if (!tableScheme.isEmpty()){
		createTableQuery.replace("${TableScheme}", tableScheme);
	}
	else{
		createTableQuery.replace("${TableScheme}", "public");
	}

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(createTableQuery, &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		qCritical() << "CTenantDbDelegateComp: table" << tableName << "could not be created."
					<< "Error:" << sqlError
					<< "Query:" << createTableQuery;
		return false;
	}

	return true;
}


bool CTenantDbDelegateComp::CreatePermissionsTableIfNeeded()
{
	const bool autoCreate = m_autoCreatePermissionsTableAttrPtr.IsValid() ? *m_autoCreatePermissionsTableAttrPtr : false;
	const QByteArray scriptPath = m_createPermissionsTableScriptPathAttrPtr.IsValid() ? *m_createPermissionsTableScriptPathAttrPtr : QByteArray();
	const QString permissionsTableName = m_permissionsTableNameAttrPtr.IsValid() ? QString::fromUtf8(*m_permissionsTableNameAttrPtr) : QString();
	return CreateAuxTableIfNeeded(autoCreate, permissionsTableName, scriptPath);
}


QByteArrayList CTenantDbDelegateComp::LoadTenantPermissions(const QByteArray& tenantId) const
{
	QByteArrayList result;
	if (!m_databaseEngineCompPtr.IsValid() || tenantId.isEmpty()){
		return result;
	}

	QString permissionsTableName = QString::fromUtf8(*m_permissionsTableNameAttrPtr);
	QString escapedTenantId = imtdb::EscapeSql(QString::fromUtf8(tenantId));

	QString queryStr;
	QByteArray productId = GetProductId();
	if (!productId.isEmpty()){
		queryStr = QString("SELECT \"PermissionId\" FROM \"%1\" WHERE \"TenantId\"='%2' AND \"ProductId\"='%3';")
				.arg(permissionsTableName, escapedTenantId, imtdb::EscapeSql(QString::fromUtf8(productId)));
	} else {
		queryStr = QString("SELECT \"PermissionId\" FROM \"%1\" WHERE \"TenantId\"='%2';")
				.arg(permissionsTableName, escapedTenantId);
	}

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(queryStr.toUtf8(), &sqlError);
	if (sqlError.type() == QSqlError::NoError){
		while (sqlQuery.next()){
			result.append(imtdb::VariantToByteArray(sqlQuery.value(0)));
		}
	}

	return result;
}


QByteArray CTenantDbDelegateComp::CreatePermissionsInsertQuery(const QByteArray& tenantId, const QByteArrayList& permissions) const
{
	if (permissions.isEmpty()){
		return QByteArray();
	}

	QString permissionsTableName = QString::fromUtf8(*m_permissionsTableNameAttrPtr);
	QString escapedTenantId = imtdb::EscapeSql(QString::fromUtf8(tenantId));
	QByteArray productId = GetProductId();
	QString escapedProductId = imtdb::EscapeSql(QString::fromUtf8(productId));

	QStringList valueRows;
	for (const QByteArray& permissionId : permissions){
		valueRows << QString("('%1', '%2', '%3')").arg(escapedTenantId, escapedProductId, imtdb::EscapeSql(QString::fromUtf8(permissionId)));
	}

	return QString("INSERT INTO \"%1\" (\"TenantId\", \"ProductId\", \"PermissionId\") VALUES %2;")
			.arg(permissionsTableName, valueRows.join(", ")).toUtf8();
}


QByteArray CTenantDbDelegateComp::CreatePermissionsDeleteQuery(const QByteArray& tenantId) const
{
	QString permissionsTableName = QString::fromUtf8(*m_permissionsTableNameAttrPtr);
	QString escapedTenantId = imtdb::EscapeSql(QString::fromUtf8(tenantId));

	QByteArray productId = GetProductId();
	if (!productId.isEmpty()){
		return QString("DELETE FROM \"%1\" WHERE \"TenantId\"='%2' AND \"ProductId\"='%3';")
				.arg(permissionsTableName, escapedTenantId, imtdb::EscapeSql(QString::fromUtf8(productId))).toUtf8();
	}

	return QString("DELETE FROM \"%1\" WHERE \"TenantId\"='%2';")
			.arg(permissionsTableName, escapedTenantId).toUtf8();
}


QByteArrayList CTenantDbDelegateComp::LoadTenantRelationshipIds(const QByteArray& tenantId) const
{
	QByteArrayList result;
	if (!m_databaseEngineCompPtr.IsValid() || tenantId.isEmpty()){
		return result;
	}

	QString relationshipsTableName = m_relationshipsTableNameAttrPtr.IsValid()
			? QString::fromUtf8(*m_relationshipsTableNameAttrPtr) : QStringLiteral("TenantRelationships");
	QString escapedTenantId = imtdb::EscapeSql(QString::fromUtf8(tenantId));

	QString queryStr = QString("SELECT \"Id\" FROM \"%1\" WHERE \"SourceTenantId\"='%2' OR \"TargetTenantId\"='%2';")
			.arg(relationshipsTableName, escapedTenantId);

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(queryStr.toUtf8(), &sqlError);
	if (sqlError.type() == QSqlError::NoError){
		while (sqlQuery.next()){
			QSqlRecord record = sqlQuery.record();
			result.append(imtdb::VariantToByteArray(record.value("Id")));
		}
	}

	return result;
}


QByteArray CTenantDbDelegateComp::CreateRelationshipIdsInsertQuery(const QByteArray& tenantId, const QByteArrayList& relationshipIds) const
{
	Q_UNUSED(tenantId);
	Q_UNUSED(relationshipIds);
	// Relationships are stored in TenantRelationships table with SourceTenantId/TargetTenantId columns.
	// No separate junction table is needed; this method is kept for interface compatibility.
	return QByteArray();
}


QByteArray CTenantDbDelegateComp::CreateRelationshipIdsDeleteQuery(const QByteArray& tenantId) const
{
	Q_UNUSED(tenantId);
	// Relationships are stored in TenantRelationships table with SourceTenantId/TargetTenantId columns.
	// No separate junction table is needed; this method is kept for interface compatibility.
	return QByteArray();
}


} // namespace imtauthdb
