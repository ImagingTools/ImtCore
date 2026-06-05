// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CTenantRelationshipDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtauthdb
{


static QString NullableSqlText(const QString& value)
{
	if (value.isEmpty()){
		return QStringLiteral("NULL");
	}

	return QString("'%1'").arg(imtdb::EscapeSql(value));
}


static QString RecordDateTimeToString(const QSqlRecord& record, const char* field)
{
	if (!record.contains(field)){
		return QString();
	}

	QVariant val = record.value(field);
	QDateTime dt = val.toDateTime();
	return dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString();
}


istd::IChangeableUniquePtr CTenantRelationshipDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_relationshipFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::ITenantRelationshipInfo> relPtr = m_relationshipFactCompPtr.CreateInstance();
	if (!relPtr.IsValid()){
		return nullptr;
	}

	relPtr->SetRelationshipId(imtdb::VariantToByteArray(record.value("Id")));
	relPtr->SetConnectionId(imtdb::VariantToByteArray(record.value("ConnectionId")));
	relPtr->SetSourceTenantId(imtdb::VariantToByteArray(record.value("SourceTenantId")));
	relPtr->SetTargetTenantId(imtdb::VariantToByteArray(record.value("TargetTenantId")));
	relPtr->SetSourceRole(static_cast<imtauth::TenantRelationshipRole>(record.value("SourceRole").toInt()));
	relPtr->SetTargetRole(static_cast<imtauth::TenantRelationshipRole>(record.value("TargetRole").toInt()));
	relPtr->SetScope(record.value("Scope").toString());
	relPtr->SetValidFrom(record.value("ValidFrom").toString());
	relPtr->SetValidUntil(record.value("ValidUntil").toString());
	relPtr->SetStatus(static_cast<imtauth::TenantRelationshipStatus>(record.value("Status").toInt()));
	relPtr->SetDescription(record.value("Description").toString());
	relPtr->SetCreatedAt(RecordDateTimeToString(record, "CreatedAt"));
	relPtr->SetUpdatedAt(RecordDateTimeToString(record, "UpdatedAt"));

	return relPtr.staticCast<istd::IChangeable>();
}


imtdb::ISqlDatabaseObjectDelegate::NewObjectQuery CTenantRelationshipDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery result;

	const imtauth::ITenantRelationshipInfo* relPtr =
			dynamic_cast<const imtauth::ITenantRelationshipInfo*>(valuePtr);
	if (relPtr == nullptr){
		return result;
	}

	QByteArray objectId = proposedObjectId.isEmpty() ? relPtr->GetRelationshipId() : proposedObjectId;
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	QString tableName = GetTableName();
	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	QByteArray query = QString(
		"INSERT INTO \"%1\" (\"Id\", \"ConnectionId\", \"SourceTenantId\", \"TargetTenantId\", "
		"\"SourceRole\", \"TargetRole\", \"Scope\", \"ValidFrom\", \"ValidUntil\", "
		"\"Status\", \"Description\", \"CreatedAt\", \"UpdatedAt\") VALUES ("
		"'%2', '%3', '%4', '%5', %6, %7, %8, %9, %10, %11, %12, '%13', '%14')")
		.arg(tableName)
		.arg(imtdb::EscapeSql(QString::fromUtf8(objectId)))
		.arg(imtdb::EscapeSql(QString::fromUtf8(relPtr->GetConnectionId())))
		.arg(imtdb::EscapeSql(QString::fromUtf8(relPtr->GetSourceTenantId())))
		.arg(imtdb::EscapeSql(QString::fromUtf8(relPtr->GetTargetTenantId())))
		.arg(static_cast<int>(relPtr->GetSourceRole()))
		.arg(static_cast<int>(relPtr->GetTargetRole()))
		.arg(NullableSqlText(relPtr->GetScope()))
		.arg(NullableSqlText(relPtr->GetValidFrom()))
		.arg(NullableSqlText(relPtr->GetValidUntil()))
		.arg(static_cast<int>(relPtr->GetStatus()))
		.arg(NullableSqlText(relPtr->GetDescription()))
		.arg(imtdb::EscapeSql(now))
		.arg(imtdb::EscapeSql(now))
		.toUtf8();

	result.objectId = objectId;
	result.query = query;

	return result;
}


QByteArray CTenantRelationshipDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtauth::ITenantRelationshipInfo* relPtr =
			dynamic_cast<const imtauth::ITenantRelationshipInfo*>(&object);
	if (relPtr == nullptr){
		return QByteArray();
	}

	QString tableName = GetTableName();
	QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

	return QString(
		"UPDATE \"%1\" SET "
		"\"ConnectionId\" = '%2', "
		"\"SourceTenantId\" = '%3', "
		"\"TargetTenantId\" = '%4', "
		"\"SourceRole\" = %5, "
		"\"TargetRole\" = %6, "
		"\"Scope\" = %7, "
		"\"ValidFrom\" = %8, "
		"\"ValidUntil\" = %9, "
		"\"Status\" = %10, "
		"\"Description\" = %11, "
		"\"UpdatedAt\" = '%12' "
		"WHERE \"Id\" = '%13'")
		.arg(tableName)
		.arg(imtdb::EscapeSql(QString::fromUtf8(relPtr->GetConnectionId())))
		.arg(imtdb::EscapeSql(QString::fromUtf8(relPtr->GetSourceTenantId())))
		.arg(imtdb::EscapeSql(QString::fromUtf8(relPtr->GetTargetTenantId())))
		.arg(static_cast<int>(relPtr->GetSourceRole()))
		.arg(static_cast<int>(relPtr->GetTargetRole()))
		.arg(NullableSqlText(relPtr->GetScope()))
		.arg(NullableSqlText(relPtr->GetValidFrom()))
		.arg(NullableSqlText(relPtr->GetValidUntil()))
		.arg(static_cast<int>(relPtr->GetStatus()))
		.arg(NullableSqlText(relPtr->GetDescription()))
		.arg(imtdb::EscapeSql(now))
		.arg(imtdb::EscapeSql(QString::fromUtf8(objectId)))
		.toUtf8();
}


QByteArray CTenantRelationshipDbDelegateComp::CreateDeleteObjectsQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArrayList& objectIds,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectIds.isEmpty()){
		return QByteArray();
	}

	QString tableName = GetTableName();
	QStringList idList;
	for (const QByteArray& id : objectIds){
		idList.append(QString("'%1'").arg(imtdb::EscapeSql(QString::fromUtf8(id))));
	}

	return QString("DELETE FROM \"%1\" WHERE \"Id\" IN (%2)")
			.arg(tableName)
			.arg(idList.join(", "))
			.toUtf8();
}


QByteArray CTenantRelationshipDbDelegateComp::CreateDeleteObjectSetQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const iprm::IParamsSet* /*paramsPtr*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QString tableName = GetTableName();
	return QString("DELETE FROM \"%1\"").arg(tableName).toUtf8();
}


QByteArray CTenantRelationshipDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTenantRelationshipDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const QString& description,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QString tableName = GetTableName();
	return QString("UPDATE \"%1\" SET \"Description\" = %2 WHERE \"Id\" = '%3'")
			.arg(tableName)
			.arg(NullableSqlText(description))
			.arg(imtdb::EscapeSql(QString::fromUtf8(objectId)))
			.toUtf8();
}


bool CTenantRelationshipDbDelegateComp::SetCollectionItemMetaInfoFromRecord(
		const QSqlRecord& record,
		idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetObjectMetaInfoFromRecord(record, metaInfo);
}


idoc::MetaInfoPtr CTenantRelationshipDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return idoc::MetaInfoPtr(new idoc::CStandardDocumentMetaInfo());
}


bool CTenantRelationshipDbDelegateComp::SetObjectMetaInfoFromRecord(
		const QSqlRecord& record,
		idoc::IDocumentMetaInfo& metaInfo) const
{
	metaInfo.SetMetaInfoValue(
		imtauth::ITenantRelationshipInfo::MIT_RELATIONSHIP_ID,
		imtdb::VariantToByteArray(record.value("Id")));

	if (record.contains("ConnectionId")){
		metaInfo.SetMetaInfoValue(
			imtauth::ITenantRelationshipInfo::MIT_RELATIONSHIP_CONNECTION_ID,
			imtdb::VariantToByteArray(record.value("ConnectionId")));
	}

	if (record.contains("SourceTenantId")){
		metaInfo.SetMetaInfoValue(
			imtauth::ITenantRelationshipInfo::MIT_RELATIONSHIP_SOURCE_TENANT_ID,
			imtdb::VariantToByteArray(record.value("SourceTenantId")));
	}

	if (record.contains("TargetTenantId")){
		metaInfo.SetMetaInfoValue(
			imtauth::ITenantRelationshipInfo::MIT_RELATIONSHIP_TARGET_TENANT_ID,
			imtdb::VariantToByteArray(record.value("TargetTenantId")));
	}

	if (record.contains("Status")){
		metaInfo.SetMetaInfoValue(
			imtauth::ITenantRelationshipInfo::MIT_RELATIONSHIP_STATUS,
			record.value("Status").toInt());
	}

	return true;
}


} // namespace imtauthdb
