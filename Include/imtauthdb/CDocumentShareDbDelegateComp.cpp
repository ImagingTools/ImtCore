// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CDocumentShareDbDelegateComp.h>

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


istd::IChangeableUniquePtr CDocumentShareDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_shareFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::IDocumentShareData> sharePtr = m_shareFactCompPtr.CreateInstance();
	if (!sharePtr.IsValid()){
		return nullptr;
	}

	imtauth::DocumentShareInfo info;
	if (record.contains("Id")){
		info.shareId = imtdb::VariantToByteArray(record.value("Id"));
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
	if (record.contains("DocumentId")){
		info.documentId = imtdb::VariantToByteArray(record.value("DocumentId"));
	}
	if (record.contains("DocumentType")){
		info.documentType = record.value("DocumentType").toString();
	}
	if (record.contains("DocumentName")){
		info.documentName = record.value("DocumentName").toString();
	}
	if (record.contains("AccessMode")){
		info.accessMode = static_cast<imtauth::DocumentShareAccessMode>(record.value("AccessMode").toInt());
	}
	if (record.contains("Status")){
		info.status = static_cast<imtauth::DocumentShareStatus>(record.value("Status").toInt());
	}
	if (record.contains("SharedByUserId")){
		info.sharedByUserId = imtdb::VariantToByteArray(record.value("SharedByUserId"));
	}
	info.createdAt = RecordDateTimeToString(record, "CreatedAt");
	info.updatedAt = RecordDateTimeToString(record, "UpdatedAt");
	info.expiresAt = RecordDateTimeToString(record, "ExpiresAt");
	info.revokedAt = RecordDateTimeToString(record, "RevokedAt");

	sharePtr->SetShareInfo(info);

	return sharePtr;
}


CDocumentShareDbDelegateComp::NewObjectQuery CDocumentShareDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery result;

	const imtauth::IDocumentShareData* sharePtr = dynamic_cast<const imtauth::IDocumentShareData*>(valuePtr);
	if (sharePtr == nullptr){
		return result;
	}

	imtauth::DocumentShareInfo info = sharePtr->GetShareInfo();

	QString id = imtdb::EscapeSql(QString::fromUtf8(!proposedObjectId.isEmpty() ? proposedObjectId : info.shareId));
	QString sourceTenantId = imtdb::EscapeSql(QString::fromUtf8(info.sourceTenantId));
	QString targetTenantId = imtdb::EscapeSql(QString::fromUtf8(info.targetTenantId));
	QString relationshipId = NullableSqlText(QString::fromUtf8(info.relationshipId));
	QString documentId = imtdb::EscapeSql(QString::fromUtf8(info.documentId));
	QString documentType = NullableSqlText(info.documentType);
	QString documentName = NullableSqlText(info.documentName);
	int accessMode = static_cast<int>(info.accessMode);
	int status = static_cast<int>(info.status);
	QString sharedByUserId = NullableSqlText(QString::fromUtf8(info.sharedByUserId));
	QString createdAt = !info.createdAt.isEmpty() ? imtdb::EscapeSql(info.createdAt) : imtdb::UtcNow();
	QString updatedAt = !info.updatedAt.isEmpty() ? imtdb::EscapeSql(info.updatedAt) : imtdb::UtcNow();
	QString expiresAt = NullableSqlDateTime(info.expiresAt);
	QString revokedAt = NullableSqlDateTime(info.revokedAt);

	result.query = QString(
		"INSERT INTO \"%1\" (\"Id\", \"SourceTenantId\", \"TargetTenantId\", \"RelationshipId\", \"DocumentId\", \"DocumentType\", \"DocumentName\", \"AccessMode\", \"Status\", \"SharedByUserId\", \"CreatedAt\", \"UpdatedAt\", \"ExpiresAt\", \"RevokedAt\") "
		"VALUES ('%2', '%3', '%4', %5, '%6', %7, %8, %9, %10, %11, '%12', '%13', %14, %15);")
		.arg(*m_tableNameAttrPtr)
		.arg(id)
		.arg(sourceTenantId)
		.arg(targetTenantId)
		.arg(relationshipId)
		.arg(documentId)
		.arg(documentType)
		.arg(documentName)
		.arg(QString::number(accessMode))
		.arg(QString::number(status))
		.arg(sharedByUserId)
		.arg(createdAt)
		.arg(updatedAt)
		.arg(expiresAt)
		.arg(revokedAt).toUtf8();

	return result;
}


QByteArray CDocumentShareDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtauth::IDocumentShareData* sharePtr = dynamic_cast<const imtauth::IDocumentShareData*>(&object);
	if (sharePtr == nullptr){
		return QByteArray();
	}

	imtauth::DocumentShareInfo info = sharePtr->GetShareInfo();

	return QString(
		"UPDATE \"%1\" SET "
		"\"DocumentName\"=%2, "
		"\"AccessMode\"=%3, "
		"\"Status\"=%4, "
		"\"UpdatedAt\"='%5', "
		"\"ExpiresAt\"=%6, "
		"\"RevokedAt\"=%7 "
		"WHERE \"Id\"='%8';")
		.arg(*m_tableNameAttrPtr)
		.arg(NullableSqlText(info.documentName))
		.arg(QString::number(static_cast<int>(info.accessMode)))
		.arg(QString::number(static_cast<int>(info.status)))
		.arg(!info.updatedAt.isEmpty() ? imtdb::EscapeSql(info.updatedAt) : imtdb::UtcNow())
		.arg(NullableSqlDateTime(info.expiresAt))
		.arg(NullableSqlDateTime(info.revokedAt))
		.arg(imtdb::EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


QByteArray CDocumentShareDbDelegateComp::CreateDeleteObjectsQuery(
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


QByteArray CDocumentShareDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CDocumentShareDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CDocumentShareDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


bool CDocumentShareDbDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("CreatedAt")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, record.value("CreatedAt").toDateTime());
	}
	return true;
}


idoc::MetaInfoPtr CDocumentShareDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo);
	return metaInfoPtr;
}


bool CDocumentShareDbDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


} // namespace imtauthdb
