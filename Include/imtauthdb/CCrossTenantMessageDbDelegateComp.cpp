// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CCrossTenantMessageDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtauthdb
{


static QString RecordDateTimeToString(const QSqlRecord& record, const char* field)
{
	if (!record.contains(field)){
		return QString();
	}

	QVariant val = record.value(field);
	QDateTime dt = val.toDateTime();
	return dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString();
}


static QString NullableSqlText(const QString& value)
{
	if (value.isEmpty()){
		return QStringLiteral("NULL");
	}

	return QString("'%1'").arg(imtdb::EscapeSql(value));
}


static QString NullableSqlDateTime(const QString& value)
{
	if (value.isEmpty()){
		return QStringLiteral("NULL");
	}

	return QString("'%1'").arg(imtdb::EscapeSql(value));
}


istd::IChangeableUniquePtr CCrossTenantMessageDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_messageFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::ICrossTenantMessageData> messagePtr = m_messageFactCompPtr.CreateInstance();
	if (!messagePtr.IsValid()){
		return nullptr;
	}

	imtauth::CrossTenantMessageInfo info;
	if (record.contains("Id")){
		info.messageId = imtdb::VariantToByteArray(record.value("Id"));
	}
	if (record.contains("SourceTenantId")){
		info.sourceTenantId = imtdb::VariantToByteArray(record.value("SourceTenantId"));
	}
	if (record.contains("TargetTenantId")){
		info.targetTenantId = imtdb::VariantToByteArray(record.value("TargetTenantId"));
	}
	if (record.contains("RelationshipId")){
		info.relationshipId = imtdb::VariantToByteArray(record.value("RelationshipId"));
	}
	if (record.contains("ContractId")){
		info.contractId = imtdb::VariantToByteArray(record.value("ContractId"));
	}
	if (record.contains("SourceObjectId")){
		info.sourceObjectId = imtdb::VariantToByteArray(record.value("SourceObjectId"));
	}
	if (record.contains("TargetObjectId")){
		info.targetObjectId = imtdb::VariantToByteArray(record.value("TargetObjectId"));
	}
	if (record.contains("MessageType")){
		info.messageType = static_cast<imtauth::CrossTenantMessageType>(record.value("MessageType").toInt());
	}
	if (record.contains("CustomType")){
		info.customType = record.value("CustomType").toString();
	}
	if (record.contains("Payload")){
		info.payload = record.value("Payload").toString().toUtf8();
	}
	if (record.contains("Status")){
		info.status = static_cast<imtauth::CrossTenantMessageStatus>(record.value("Status").toInt());
	}
	if (record.contains("ErrorMessage")){
		info.errorMessage = record.value("ErrorMessage").toString();
	}
	if (record.contains("AccessMode")){
		info.accessMode = static_cast<imtauth::DocumentShareAccessMode>(record.value("AccessMode").toInt());
	}
	info.createdAt = RecordDateTimeToString(record, "CreatedAt");
	info.updatedAt = RecordDateTimeToString(record, "UpdatedAt");
	info.expiresAt = RecordDateTimeToString(record, "ExpiresAt");

	messagePtr->SetMessageInfo(info);

	return messagePtr;
}


CCrossTenantMessageDbDelegateComp::NewObjectQuery CCrossTenantMessageDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery result;

	const imtauth::ICrossTenantMessageData* messagePtr = dynamic_cast<const imtauth::ICrossTenantMessageData*>(valuePtr);
	if (messagePtr == nullptr){
		return result;
	}

	imtauth::CrossTenantMessageInfo info = messagePtr->GetMessageInfo();

	QString id = imtdb::EscapeSql(QString::fromUtf8(!proposedObjectId.isEmpty() ? proposedObjectId : info.messageId));
	QString sourceTenantId = imtdb::EscapeSql(QString::fromUtf8(info.sourceTenantId));
	QString targetTenantId = imtdb::EscapeSql(QString::fromUtf8(info.targetTenantId));
	QString relationshipId = imtdb::EscapeSql(QString::fromUtf8(info.relationshipId));
	QString contractId = NullableSqlText(QString::fromUtf8(info.contractId));
	QString sourceObjectId = NullableSqlText(QString::fromUtf8(info.sourceObjectId));
	QString targetObjectId = NullableSqlText(QString::fromUtf8(info.targetObjectId));
	int messageType = static_cast<int>(info.messageType);
	QString customType = NullableSqlText(info.customType);
	QString payload = NullableSqlText(QString::fromUtf8(info.payload));
	int status = static_cast<int>(info.status);
	QString errorMessage = NullableSqlText(info.errorMessage);
	int accessMode = static_cast<int>(info.accessMode);
	QString createdAt = !info.createdAt.isEmpty() ? imtdb::EscapeSql(info.createdAt) : imtdb::UtcNow();
	QString updatedAt = !info.updatedAt.isEmpty() ? imtdb::EscapeSql(info.updatedAt) : imtdb::UtcNow();
	QString expiresAt = NullableSqlDateTime(info.expiresAt);

	result.query = QString(
		"INSERT INTO \"%1\" (\"Id\", \"SourceTenantId\", \"TargetTenantId\", \"RelationshipId\", \"ContractId\", \"SourceObjectId\", \"TargetObjectId\", \"MessageType\", \"CustomType\", \"Payload\", \"Status\", \"ErrorMessage\", \"AccessMode\", \"CreatedAt\", \"UpdatedAt\", \"ExpiresAt\") "
		"VALUES ('%2', '%3', '%4', '%5', %6, %7, %8, %9, %10, %11, %12, %13, %14, '%15', '%16', %17);")
		.arg(*m_tableNameAttrPtr)
		.arg(id)
		.arg(sourceTenantId)
		.arg(targetTenantId)
		.arg(relationshipId)
		.arg(contractId)
		.arg(sourceObjectId)
		.arg(targetObjectId)
		.arg(QString::number(messageType))
		.arg(customType)
		.arg(payload)
		.arg(QString::number(status))
		.arg(errorMessage)
		.arg(QString::number(accessMode))
		.arg(createdAt)
		.arg(updatedAt)
		.arg(expiresAt).toUtf8();

	return result;
}


QByteArray CCrossTenantMessageDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtauth::ICrossTenantMessageData* messagePtr = dynamic_cast<const imtauth::ICrossTenantMessageData*>(&object);
	if (messagePtr == nullptr){
		return QByteArray();
	}

	imtauth::CrossTenantMessageInfo info = messagePtr->GetMessageInfo();

	return QString(
		"UPDATE \"%1\" SET "
		"\"TargetObjectId\"=%2, "
		"\"Status\"=%3, "
		"\"ErrorMessage\"=%4, "
		"\"UpdatedAt\"='%5', "
		"\"ExpiresAt\"=%6 "
		"WHERE \"Id\"='%7';")
		.arg(*m_tableNameAttrPtr)
		.arg(NullableSqlText(QString::fromUtf8(info.targetObjectId)))
		.arg(QString::number(static_cast<int>(info.status)))
		.arg(NullableSqlText(info.errorMessage))
		.arg(!info.updatedAt.isEmpty() ? imtdb::EscapeSql(info.updatedAt) : imtdb::UtcNow())
		.arg(NullableSqlDateTime(info.expiresAt))
		.arg(imtdb::EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


QByteArray CCrossTenantMessageDbDelegateComp::CreateDeleteObjectsQuery(
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

	return QString("DELETE FROM \"%1\" WHERE \"Id\" IN (%2);")
			.arg(*m_tableNameAttrPtr, escapedIds.join(", ")).toUtf8();
}


QByteArray CCrossTenantMessageDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CCrossTenantMessageDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CCrossTenantMessageDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


bool CCrossTenantMessageDbDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("CreatedAt")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, record.value("CreatedAt").toDateTime());
	}
	return true;
}


idoc::MetaInfoPtr CCrossTenantMessageDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo);
	return metaInfoPtr;
}


bool CCrossTenantMessageDbDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


} // namespace imtauthdb
