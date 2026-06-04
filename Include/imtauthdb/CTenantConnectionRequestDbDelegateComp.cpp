// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CTenantConnectionRequestDbDelegateComp.h>

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


istd::IChangeableUniquePtr CTenantConnectionRequestDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_requestFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::ITenantConnectionRequestData> requestPtr = m_requestFactCompPtr.CreateInstance();
	if (!requestPtr.IsValid()){
		return nullptr;
	}

	imtauth::TenantConnectionRequestInfo info;
	if (record.contains("Id")){
		info.requestId = imtdb::VariantToByteArray(record.value("Id"));
	}
	if (record.contains("SourceTenantId")){
		info.sourceTenantId = imtdb::VariantToByteArray(record.value("SourceTenantId"));
	}
	if (record.contains("TargetTenantId")){
		info.targetTenantId = imtdb::VariantToByteArray(record.value("TargetTenantId"));
	}
	if (record.contains("TargetIdentifier")){
		info.targetIdentifier = record.value("TargetIdentifier").toString();
	}
	if (record.contains("ConnectCode")){
		info.connectCode = record.value("ConnectCode").toString();
	}
	if (record.contains("ProposedSourceRole")){
		info.proposedSourceRole = static_cast<imtauth::ITenantInfo::TenantRelationshipRole>(record.value("ProposedSourceRole").toInt());
	}
	if (record.contains("ProposedTargetRole")){
		info.proposedTargetRole = static_cast<imtauth::ITenantInfo::TenantRelationshipRole>(record.value("ProposedTargetRole").toInt());
	}
	if (record.contains("Message")){
		info.message = record.value("Message").toString();
	}
	if (record.contains("Status")){
		info.status = static_cast<imtauth::ConnectionRequestStatus>(record.value("Status").toInt());
	}
	info.createdAt = RecordDateTimeToString(record, "CreatedAt");
	info.respondedAt = RecordDateTimeToString(record, "RespondedAt");

	requestPtr->SetRequestInfo(info);

	return requestPtr;
}


CTenantConnectionRequestDbDelegateComp::NewObjectQuery CTenantConnectionRequestDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery result;

	const imtauth::ITenantConnectionRequestData* requestPtr = dynamic_cast<const imtauth::ITenantConnectionRequestData*>(valuePtr);
	if (requestPtr == nullptr){
		return result;
	}

	imtauth::TenantConnectionRequestInfo info = requestPtr->GetRequestInfo();

	QString id = imtdb::EscapeSql(QString::fromUtf8(!proposedObjectId.isEmpty() ? proposedObjectId : info.requestId));
	QString sourceTenantId = imtdb::EscapeSql(QString::fromUtf8(info.sourceTenantId));
	QString targetTenantId = NullableSqlText(QString::fromUtf8(info.targetTenantId));
	QString targetIdentifier = NullableSqlText(info.targetIdentifier);
	QString connectCode = NullableSqlText(info.connectCode);
	int proposedSourceRole = static_cast<int>(info.proposedSourceRole);
	int proposedTargetRole = static_cast<int>(info.proposedTargetRole);
	QString message = NullableSqlText(info.message);
	int status = static_cast<int>(info.status);
	QString createdAt = !info.createdAt.isEmpty() ? imtdb::EscapeSql(info.createdAt) : imtdb::UtcNow();
	QString expiresAt = NullableSqlDateTime(info.expiresAt);
	QString respondedAt = NullableSqlDateTime(info.respondedAt);

	result.query = QString(
		"INSERT INTO \"%1\" (\"Id\", \"SourceTenantId\", \"TargetTenantId\", \"TargetIdentifier\", \"ConnectCode\", \"ProposedSourceRole\", \"ProposedTargetRole\", \"Message\", \"Status\", \"CreatedAt\", \"ExpiresAt\", \"RespondedAt\") "
		"VALUES ('%2', '%3', %4, %5, %6, %7, %8, %9, %10, '%11', %12, %13);")
		.arg(*m_tableNameAttrPtr)
		.arg(id)
		.arg(sourceTenantId)
		.arg(targetTenantId)
		.arg(targetIdentifier)
		.arg(connectCode)
		.arg(QString::number(proposedSourceRole))
		.arg(QString::number(proposedTargetRole))
		.arg(message)
		.arg(QString::number(status))
		.arg(createdAt)
		.arg(expiresAt)
		.arg(respondedAt).toUtf8();

	return result;
}


QByteArray CTenantConnectionRequestDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtauth::ITenantConnectionRequestData* requestPtr = dynamic_cast<const imtauth::ITenantConnectionRequestData*>(&object);
	if (requestPtr == nullptr){
		return QByteArray();
	}

	imtauth::TenantConnectionRequestInfo info = requestPtr->GetRequestInfo();

	return QString(
		"UPDATE \"%1\" SET "
		"\"TargetTenantId\"=%2, "
		"\"Status\"=%3, "
		"\"Message\"=%4, "
		"\"ExpiresAt\"=%5, "
		"\"RespondedAt\"=%6 "
		"WHERE \"Id\"='%7';")
		.arg(*m_tableNameAttrPtr)
		.arg(NullableSqlText(QString::fromUtf8(info.targetTenantId)))
		.arg(QString::number(static_cast<int>(info.status)))
		.arg(NullableSqlText(info.message))
		.arg(NullableSqlDateTime(info.expiresAt))
		.arg(NullableSqlDateTime(info.respondedAt))
		.arg(imtdb::EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


QByteArray CTenantConnectionRequestDbDelegateComp::CreateDeleteObjectsQuery(
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


QByteArray CTenantConnectionRequestDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTenantConnectionRequestDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTenantConnectionRequestDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


bool CTenantConnectionRequestDbDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("CreatedAt")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, record.value("CreatedAt").toDateTime());
	}
	return true;
}


idoc::MetaInfoPtr CTenantConnectionRequestDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo);
	return metaInfoPtr;
}


bool CTenantConnectionRequestDbDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


} // namespace imtauthdb
