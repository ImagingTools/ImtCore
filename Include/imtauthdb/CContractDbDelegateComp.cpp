// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CContractDbDelegateComp.h>

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


istd::IChangeableUniquePtr CContractDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_contractFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::IContractData> contractPtr = m_contractFactCompPtr.CreateInstance();
	if (!contractPtr.IsValid()){
		return nullptr;
	}

	imtauth::ContractInfo info;
	if (record.contains("Id")){
		info.contractId = imtdb::VariantToByteArray(record.value("Id"));
	}
	if (record.contains("RelationshipId")){
		info.relationshipId = imtdb::VariantToByteArray(record.value("RelationshipId"));
	}
	if (record.contains("SourceTenantId")){
		info.sourceTenantId = imtdb::VariantToByteArray(record.value("SourceTenantId"));
	}
	if (record.contains("TargetTenantId")){
		info.targetTenantId = imtdb::VariantToByteArray(record.value("TargetTenantId"));
	}
	if (record.contains("Status")){
		info.status = static_cast<imtauth::ContractStatus>(record.value("Status").toInt());
	}
	if (record.contains("Scope")){
		info.scope = record.value("Scope").toString();
	}
	info.validFrom = RecordDateTimeToString(record, "ValidFrom");
	info.validUntil = RecordDateTimeToString(record, "ValidUntil");
	if (record.contains("Description")){
		info.description = record.value("Description").toString();
	}
	if (record.contains("Terms")){
		info.terms = record.value("Terms").toString();
	}
	info.createdAt = RecordDateTimeToString(record, "CreatedAt");
	info.updatedAt = RecordDateTimeToString(record, "UpdatedAt");

	contractPtr->SetContractInfo(info);

	return contractPtr;
}


CContractDbDelegateComp::NewObjectQuery CContractDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery result;

	const imtauth::IContractData* contractPtr = dynamic_cast<const imtauth::IContractData*>(valuePtr);
	if (contractPtr == nullptr){
		return result;
	}

	imtauth::ContractInfo info = contractPtr->GetContractInfo();

	QString id = imtdb::EscapeSql(QString::fromUtf8(!proposedObjectId.isEmpty() ? proposedObjectId : info.contractId));
	QString relationshipId = imtdb::EscapeSql(QString::fromUtf8(info.relationshipId));
	QString sourceTenantId = imtdb::EscapeSql(QString::fromUtf8(info.sourceTenantId));
	QString targetTenantId = imtdb::EscapeSql(QString::fromUtf8(info.targetTenantId));
	int status = static_cast<int>(info.status);
	QString scope = NullableSqlText(info.scope);
	QString validFrom = NullableSqlDateTime(info.validFrom);
	QString validUntil = NullableSqlDateTime(info.validUntil);
	QString description = NullableSqlText(info.description);
	QString terms = NullableSqlText(info.terms);
	QString createdAt = !info.createdAt.isEmpty() ? imtdb::EscapeSql(info.createdAt) : imtdb::UtcNow();
	QString updatedAt = !info.updatedAt.isEmpty() ? imtdb::EscapeSql(info.updatedAt) : imtdb::UtcNow();

	result.query = QString(
		"INSERT INTO \"%1\" (\"Id\", \"RelationshipId\", \"SourceTenantId\", \"TargetTenantId\", \"Status\", \"Scope\", \"ValidFrom\", \"ValidUntil\", \"Description\", \"Terms\", \"CreatedAt\", \"UpdatedAt\") "
		"VALUES ('%2', '%3', '%4', '%5', %6, %7, %8, %9, %10, %11, '%12', '%13');")
		.arg(*m_tableNameAttrPtr)
		.arg(id)
		.arg(relationshipId)
		.arg(sourceTenantId)
		.arg(targetTenantId)
		.arg(QString::number(status))
		.arg(scope)
		.arg(validFrom)
		.arg(validUntil)
		.arg(description)
		.arg(terms)
		.arg(createdAt)
		.arg(updatedAt).toUtf8();

	return result;
}


QByteArray CContractDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtauth::IContractData* contractPtr = dynamic_cast<const imtauth::IContractData*>(&object);
	if (contractPtr == nullptr){
		return QByteArray();
	}

	imtauth::ContractInfo info = contractPtr->GetContractInfo();

	return QString(
		"UPDATE \"%1\" SET "
		"\"Status\"=%2, "
		"\"Scope\"=%3, "
		"\"ValidFrom\"=%4, "
		"\"ValidUntil\"=%5, "
		"\"Description\"=%6, "
		"\"Terms\"=%7, "
		"\"UpdatedAt\"='%8' "
		"WHERE \"Id\"='%9';")
		.arg(*m_tableNameAttrPtr)
		.arg(QString::number(static_cast<int>(info.status)))
		.arg(NullableSqlText(info.scope))
		.arg(NullableSqlDateTime(info.validFrom))
		.arg(NullableSqlDateTime(info.validUntil))
		.arg(NullableSqlText(info.description))
		.arg(NullableSqlText(info.terms))
		.arg(!info.updatedAt.isEmpty() ? imtdb::EscapeSql(info.updatedAt) : imtdb::UtcNow())
		.arg(imtdb::EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


QByteArray CContractDbDelegateComp::CreateDeleteObjectsQuery(
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


QByteArray CContractDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CContractDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CContractDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


bool CContractDbDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("CreatedAt")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, record.value("CreatedAt").toDateTime());
	}
	return true;
}


idoc::MetaInfoPtr CContractDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo);
	return metaInfoPtr;
}


bool CContractDbDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


} // namespace imtauthdb
