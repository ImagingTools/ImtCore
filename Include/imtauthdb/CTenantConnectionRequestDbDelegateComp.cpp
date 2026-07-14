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

	istd::TUniqueInterfacePtr<imtauth::ITenantConnectionRequestInfo> requestPtr = m_requestFactCompPtr.CreateInstance();
	if (!requestPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		requestPtr->SetRequestId(imtdb::VariantToByteArray(record.value("Id")));
	}
	if (record.contains("SourceTenantId")){
		requestPtr->SetSourceTenantId(imtdb::VariantToByteArray(record.value("SourceTenantId")));
	}
	if (record.contains("TargetTenantId")){
		requestPtr->SetTargetTenantId(imtdb::VariantToByteArray(record.value("TargetTenantId")));
	}
	if (record.contains("ConnectionCode")){
		requestPtr->SetConnectionCode(record.value("ConnectionCode").toString());
	}
	if (record.contains("Message")){
		requestPtr->SetMessage(record.value("Message").toString());
	}
	if (record.contains("Status")){
		requestPtr->SetStatus(static_cast<imtauth::ITenantConnectionRequestInfo::ConnectionRequestStatus>(record.value("Status").toInt()));
	}
	requestPtr->SetCreatedAt(RecordDateTimeToString(record, "CreatedAt"));
	requestPtr->SetRespondedAt(RecordDateTimeToString(record, "RespondedAt"));

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

	const imtauth::ITenantConnectionRequestInfo* requestPtr = dynamic_cast<const imtauth::ITenantConnectionRequestInfo*>(valuePtr);
	if (requestPtr == nullptr){
		return result;
	}

	QString id = imtdb::EscapeSql(QString::fromUtf8(!proposedObjectId.isEmpty() ? proposedObjectId : requestPtr->GetRequestId()));
	QString sourceTenantId = imtdb::EscapeSql(QString::fromUtf8(requestPtr->GetSourceTenantId()));
	QString targetTenantId = NullableSqlText(QString::fromUtf8(requestPtr->GetTargetTenantId()));
	QString connectionCode = NullableSqlText(requestPtr->GetConnectionCode());
	QString message = NullableSqlText(requestPtr->GetMessage());
	int status = static_cast<int>(requestPtr->GetStatus());
	QString createdAt = !requestPtr->GetCreatedAt().isEmpty() ? imtdb::EscapeSql(requestPtr->GetCreatedAt()) : imtdb::UtcNow();
	QString respondedAt = NullableSqlDateTime(requestPtr->GetRespondedAt());

	result.query = QString(
		"INSERT INTO \"%1\" (\"Id\", \"SourceTenantId\", \"TargetTenantId\", \"ConnectionCode\", \"Message\", \"Status\", \"CreatedAt\", \"RespondedAt\") "
		"VALUES ('%2', '%3', %4, %5, %6, %7, '%8', %9);")
		.arg(*m_tableNameAttrPtr)
		.arg(id)
		.arg(sourceTenantId)
		.arg(targetTenantId)
		.arg(connectionCode)
		.arg(message)
		.arg(QString::number(status))
		.arg(createdAt)
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
	const imtauth::ITenantConnectionRequestInfo* requestPtr = dynamic_cast<const imtauth::ITenantConnectionRequestInfo*>(&object);
	if (requestPtr == nullptr){
		return QByteArray();
	}

	return QString(
		"UPDATE \"%1\" SET "
		"\"TargetTenantId\"=%2, "
		"\"Status\"=%3, "
		"\"Message\"=%4, "
		"\"RespondedAt\"=%5 "
		"WHERE \"Id\"='%6';")
		.arg(*m_tableNameAttrPtr)
		.arg(NullableSqlText(QString::fromUtf8(requestPtr->GetTargetTenantId())))
		.arg(QString::number(static_cast<int>(requestPtr->GetStatus())))
		.arg(NullableSqlText(requestPtr->GetMessage()))
		.arg(NullableSqlDateTime(requestPtr->GetRespondedAt()))
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
