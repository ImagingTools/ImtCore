// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CTenantConnectionDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtauthdb
{


static QString NullableSqlDateTime(const QString& value)
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


istd::IChangeableUniquePtr CTenantConnectionDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_connectionFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::ITenantConnectionData> connPtr = m_connectionFactCompPtr.CreateInstance();
	if (!connPtr.IsValid()){
		return nullptr;
	}

	imtauth::ITenantConnectionRequest::TenantConnectionInfo info;
	if (record.contains("Id")){
		info.connectionId = imtdb::VariantToByteArray(record.value("Id"));
	}
	if (record.contains("TenantAId")){
		info.tenantAId = imtdb::VariantToByteArray(record.value("TenantAId"));
	}
	if (record.contains("TenantBId")){
		info.tenantBId = imtdb::VariantToByteArray(record.value("TenantBId"));
	}
	if (record.contains("Status")){
		info.status = static_cast<imtauth::ITenantConnectionRequest::ConnectionStatus>(record.value("Status").toInt());
	}
	info.createdAt = RecordDateTimeToString(record, "CreatedAt");
	info.updatedAt = RecordDateTimeToString(record, "UpdatedAt");

	connPtr->SetConnectionInfo(info);

	return connPtr;
}


CTenantConnectionDbDelegateComp::NewObjectQuery CTenantConnectionDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery result;

	const imtauth::ITenantConnectionData* connPtr = dynamic_cast<const imtauth::ITenantConnectionData*>(valuePtr);
	if (connPtr == nullptr){
		return result;
	}

	imtauth::ITenantConnectionRequest::TenantConnectionInfo info = connPtr->GetConnectionInfo();

	QString id = imtdb::EscapeSql(QString::fromUtf8(!proposedObjectId.isEmpty() ? proposedObjectId : info.connectionId));
	QString tenantAId = imtdb::EscapeSql(QString::fromUtf8(info.tenantAId));
	QString tenantBId = imtdb::EscapeSql(QString::fromUtf8(info.tenantBId));
	int status = static_cast<int>(info.status);
	QString createdAt = !info.createdAt.isEmpty() ? imtdb::EscapeSql(info.createdAt) : imtdb::UtcNow();
	QString updatedAt = NullableSqlDateTime(info.updatedAt);

	result.query = QString(
		"INSERT INTO \"%1\" (\"Id\", \"TenantAId\", \"TenantBId\", \"Status\", \"CreatedAt\", \"UpdatedAt\") "
		"VALUES ('%2', '%3', '%4', %5, '%6', %7);")
		.arg(*m_tableNameAttrPtr)
		.arg(id)
		.arg(tenantAId)
		.arg(tenantBId)
		.arg(QString::number(status))
		.arg(createdAt)
		.arg(updatedAt).toUtf8();

	return result;
}


QByteArray CTenantConnectionDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtauth::ITenantConnectionData* connPtr = dynamic_cast<const imtauth::ITenantConnectionData*>(&object);
	if (connPtr == nullptr){
		return QByteArray();
	}

	imtauth::ITenantConnectionRequest::TenantConnectionInfo info = connPtr->GetConnectionInfo();

	return QString(
		"UPDATE \"%1\" SET "
		"\"Status\"=%2, "
		"\"UpdatedAt\"=%3 "
		"WHERE \"Id\"='%4';")
		.arg(*m_tableNameAttrPtr)
		.arg(QString::number(static_cast<int>(info.status)))
		.arg(NullableSqlDateTime(info.updatedAt))
		.arg(imtdb::EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


QByteArray CTenantConnectionDbDelegateComp::CreateDeleteObjectsQuery(
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


QByteArray CTenantConnectionDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTenantConnectionDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTenantConnectionDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


bool CTenantConnectionDbDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("CreatedAt")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, record.value("CreatedAt").toDateTime());
	}
	return true;
}


idoc::MetaInfoPtr CTenantConnectionDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo);
	return metaInfoPtr;
}


bool CTenantConnectionDbDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


} // namespace imtauthdb
