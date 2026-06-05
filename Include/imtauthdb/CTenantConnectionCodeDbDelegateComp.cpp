// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CTenantConnectionCodeDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtauthdb
{


istd::IChangeableUniquePtr CTenantConnectionCodeDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_codeFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::ITenantConnectionCodeInfo> codePtr = m_codeFactCompPtr.CreateInstance();
	if (!codePtr.IsValid()){
		return nullptr;
	}

	imtauth::ITenantConnectionRequest::TenantConnectionCodeInfo info;
	if (record.contains("Id")){
		info.tenantId = imtdb::VariantToByteArray(record.value("Id"));
	}
	if (record.contains("ConnectionCode")){
		info.connectionCode = record.value("ConnectionCode").toString();
	}
	if (record.contains("AllowConnectionsByCode")){
		info.allowConnectionsByCode = record.value("AllowConnectionsByCode").toBool();
	}
	if (record.contains("CreatedAt")){
		QDateTime dt = record.value("CreatedAt").toDateTime();
		info.createdAt = dt.isValid() ? dt.toString(Qt::ISODateWithMs) : record.value("CreatedAt").toString();
	}

	codePtr->SetCodeInfo(info);

	return codePtr;
}


CTenantConnectionCodeDbDelegateComp::NewObjectQuery CTenantConnectionCodeDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery result;

	const imtauth::ITenantConnectionCodeInfo* codePtr = dynamic_cast<const imtauth::ITenantConnectionCodeInfo*>(valuePtr);
	if (codePtr == nullptr){
		return result;
	}

	imtauth::ITenantConnectionRequest::TenantConnectionCodeInfo info = codePtr->GetCodeInfo();

	QString id = imtdb::EscapeSql(QString::fromUtf8(!proposedObjectId.isEmpty() ? proposedObjectId : info.tenantId));
	QString connectionCode = imtdb::EscapeSql(info.connectionCode);
	int allowInt = info.allowConnectionsByCode ? 1 : 0;
	QString createdAt = !info.createdAt.isEmpty() ? imtdb::EscapeSql(info.createdAt) : imtdb::UtcNow();

	result.query = QString(
		"INSERT INTO \"%1\" (\"Id\", \"ConnectionCode\", \"AllowConnectionsByCode\", \"CreatedAt\") "
		"VALUES ('%2', '%3', %4, '%5');")
		.arg(*m_tableNameAttrPtr)
		.arg(id)
		.arg(connectionCode)
		.arg(QString::number(allowInt))
		.arg(createdAt).toUtf8();

	return result;
}


QByteArray CTenantConnectionCodeDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtauth::ITenantConnectionCodeInfo* codePtr = dynamic_cast<const imtauth::ITenantConnectionCodeInfo*>(&object);
	if (codePtr == nullptr){
		return QByteArray();
	}

	imtauth::ITenantConnectionRequest::TenantConnectionCodeInfo info = codePtr->GetCodeInfo();

	return QString(
		"UPDATE \"%1\" SET "
		"\"ConnectionCode\"='%2', "
		"\"AllowConnectionsByCode\"=%3 "
		"WHERE \"Id\"='%4';")
		.arg(*m_tableNameAttrPtr)
		.arg(imtdb::EscapeSql(info.connectionCode))
		.arg(QString::number(info.allowConnectionsByCode ? 1 : 0))
		.arg(imtdb::EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


QByteArray CTenantConnectionCodeDbDelegateComp::CreateDeleteObjectsQuery(
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


QByteArray CTenantConnectionCodeDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTenantConnectionCodeDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTenantConnectionCodeDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


bool CTenantConnectionCodeDbDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("CreatedAt")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, record.value("CreatedAt").toDateTime());
	}
	return true;
}


idoc::MetaInfoPtr CTenantConnectionCodeDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo);
	return metaInfoPtr;
}


bool CTenantConnectionCodeDbDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


} // namespace imtauthdb
