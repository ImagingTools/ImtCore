// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CTenantDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ACF includes
#include <imtauth/ITenantInfo.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtauthdb
{


QString CTenantDbDelegateComp::UtcNow() const
{
	return QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
}


QString CTenantDbDelegateComp::EscapeSql(const QString& value) const
{
	QString escaped = value;
	escaped.replace('\'', "''");
	return escaped;
}


QByteArray CTenantDbDelegateComp::GetSelectionQuery(
		const QByteArray& objectId,
		int offset,
		int count,
		const iprm::IParamsSet* paramsPtr) const
{
	return BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);
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
		tenantPtr->SetTenantId(record.value("Id").toByteArray());
	}
	if (record.contains("Name")){
		tenantPtr->SetTenantName(record.value("Name").toString());
	}
	if (record.contains("Description")){
		tenantPtr->SetTenantDescription(record.value("Description").toString());
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
		id = EscapeSql(QString::fromUtf8(proposedObjectId));
	}
	else{
		id = QUuid::createUuid().toString(QUuid::WithoutBraces);
	}

	QString name = EscapeSql(tenantPtr != nullptr ? tenantPtr->GetTenantName() : objectName);
	QString description = EscapeSql(tenantPtr != nullptr ? tenantPtr->GetTenantDescription() : objectDescription);
	bool isActive = tenantPtr != nullptr ? tenantPtr->IsActive() : true;
	QString now = UtcNow();

	result.query = QString(
		"INSERT INTO \"%1\" (\"Id\", \"Name\", \"Description\", \"IsActive\", \"CreatedAt\", \"UpdatedAt\") "
		"VALUES ('%2', '%3', '%4', %5, '%6', '%7');")
		.arg(*m_tableNameAttrPtr,
			 id,
			 name,
			 description,
			 isActive ? "1" : "0",
			 now,
			 now).toUtf8();

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

	QString escapedId = EscapeSql(QString::fromUtf8(objectId));
	QString now = UtcNow();

	return QString(
		"UPDATE \"%1\" SET "
		"\"Name\"='%2', "
		"\"Description\"='%3', "
		"\"IsActive\"=%4, "
		"\"UpdatedAt\"='%5' "
		"WHERE \"Id\"='%6';")
		.arg(*m_tableNameAttrPtr,
			 EscapeSql(tenantPtr->GetTenantName()),
			 EscapeSql(tenantPtr->GetTenantDescription()),
			 tenantPtr->IsActive() ? "1" : "0",
			 now,
			 escapedId).toUtf8();
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
		escapedIds << QString("'%1'").arg(EscapeSql(QString::fromUtf8(id)));
	}

	return QString("DELETE FROM \"%1\" WHERE \"Id\" IN (%2);")
			.arg(*m_tableNameAttrPtr, escapedIds.join(", ")).toUtf8();
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
				 EscapeSql(newObjectName),
				 UtcNow(),
				 EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


QByteArray CTenantDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const QString& description,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QString("UPDATE \"%1\" SET \"Description\"='%2', \"UpdatedAt\"='%3' WHERE \"Id\"='%4';")
			.arg(*m_tableNameAttrPtr,
				 EscapeSql(description),
				 UtcNow(),
				 EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


bool CTenantDbDelegateComp::SetCollectionItemMetaInfoFromRecord(
		const QSqlRecord& record,
		idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("Id")){
		metaInfo.SetMetaInfo(imtauth::ITenantInfo::MIT_TENANT_ID, record.value("Id").toByteArray());
	}
	if (record.contains("Name")){
		metaInfo.SetMetaInfo(imtauth::ITenantInfo::MIT_TENANT_NAME, record.value("Name").toString());
	}
	if (record.contains("Description")){
		metaInfo.SetMetaInfo(imtauth::ITenantInfo::MIT_TENANT_DESCRIPTION, record.value("Description").toString());
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


} // namespace imtauthdb
