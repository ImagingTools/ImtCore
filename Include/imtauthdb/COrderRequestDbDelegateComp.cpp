// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/COrderRequestDbDelegateComp.h>

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


istd::IChangeableUniquePtr COrderRequestDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_orderRequestFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::IOrderRequestData> orderRequestPtr = m_orderRequestFactCompPtr.CreateInstance();
	if (!orderRequestPtr.IsValid()){
		return nullptr;
	}

	imtauth::OrderRequestInfo info;
	if (record.contains("Id")){
		info.orderRequestId = imtdb::VariantToByteArray(record.value("Id"));
	}
	if (record.contains("MessageId")){
		info.messageId = imtdb::VariantToByteArray(record.value("MessageId"));
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
	if (record.contains("SourceOrderId")){
		info.sourceOrderId = imtdb::VariantToByteArray(record.value("SourceOrderId"));
	}
	if (record.contains("ArticleNumber")){
		info.articleNumber = record.value("ArticleNumber").toString();
	}
	if (record.contains("Quantity")){
		info.quantity = record.value("Quantity").toInt();
	}
	if (record.contains("Note")){
		info.note = record.value("Note").toString();
	}
	if (record.contains("Status")){
		info.status = static_cast<imtauth::OrderRequestStatus>(record.value("Status").toInt());
	}
	if (record.contains("StatusNote")){
		info.statusNote = record.value("StatusNote").toString();
	}
	info.createdAt = RecordDateTimeToString(record, "CreatedAt");
	info.updatedAt = RecordDateTimeToString(record, "UpdatedAt");

	orderRequestPtr->SetOrderRequestInfo(info);

	return orderRequestPtr;
}


COrderRequestDbDelegateComp::NewObjectQuery COrderRequestDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery result;

	const imtauth::IOrderRequestData* orderRequestPtr = dynamic_cast<const imtauth::IOrderRequestData*>(valuePtr);
	if (orderRequestPtr == nullptr){
		return result;
	}

	imtauth::OrderRequestInfo info = orderRequestPtr->GetOrderRequestInfo();

	QString id = imtdb::EscapeSql(QString::fromUtf8(!proposedObjectId.isEmpty() ? proposedObjectId : info.orderRequestId));
	QString messageId = NullableSqlText(QString::fromUtf8(info.messageId));
	QString sourceTenantId = imtdb::EscapeSql(QString::fromUtf8(info.sourceTenantId));
	QString targetTenantId = imtdb::EscapeSql(QString::fromUtf8(info.targetTenantId));
	QString relationshipId = imtdb::EscapeSql(QString::fromUtf8(info.relationshipId));
	QString sourceOrderId = NullableSqlText(QString::fromUtf8(info.sourceOrderId));
	QString articleNumber = NullableSqlText(info.articleNumber);
	int quantity = info.quantity;
	QString note = NullableSqlText(info.note);
	int status = static_cast<int>(info.status);
	QString statusNote = NullableSqlText(info.statusNote);
	QString createdAt = !info.createdAt.isEmpty() ? imtdb::EscapeSql(info.createdAt) : imtdb::UtcNow();
	QString updatedAt = !info.updatedAt.isEmpty() ? imtdb::EscapeSql(info.updatedAt) : imtdb::UtcNow();

	result.query = QString(
		"INSERT INTO \"%1\" (\"Id\", \"MessageId\", \"SourceTenantId\", \"TargetTenantId\", \"RelationshipId\", \"SourceOrderId\", \"ArticleNumber\", \"Quantity\", \"Note\", \"Status\", \"StatusNote\", \"CreatedAt\", \"UpdatedAt\") "
		"VALUES ('%2', %3, '%4', '%5', '%6', %7, %8, %9, %10, %11, %12, '%13', '%14');")
		.arg(*m_tableNameAttrPtr)
		.arg(id)
		.arg(messageId)
		.arg(sourceTenantId)
		.arg(targetTenantId)
		.arg(relationshipId)
		.arg(sourceOrderId)
		.arg(articleNumber)
		.arg(QString::number(quantity))
		.arg(note)
		.arg(QString::number(status))
		.arg(statusNote)
		.arg(createdAt)
		.arg(updatedAt).toUtf8();

	return result;
}


QByteArray COrderRequestDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtauth::IOrderRequestData* orderRequestPtr = dynamic_cast<const imtauth::IOrderRequestData*>(&object);
	if (orderRequestPtr == nullptr){
		return QByteArray();
	}

	imtauth::OrderRequestInfo info = orderRequestPtr->GetOrderRequestInfo();

	return QString(
		"UPDATE \"%1\" SET "
		"\"Status\"=%2, "
		"\"StatusNote\"=%3, "
		"\"UpdatedAt\"='%4' "
		"WHERE \"Id\"='%5';")
		.arg(*m_tableNameAttrPtr)
		.arg(QString::number(static_cast<int>(info.status)))
		.arg(NullableSqlText(info.statusNote))
		.arg(!info.updatedAt.isEmpty() ? imtdb::EscapeSql(info.updatedAt) : imtdb::UtcNow())
		.arg(imtdb::EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


QByteArray COrderRequestDbDelegateComp::CreateDeleteObjectsQuery(
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


QByteArray COrderRequestDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray COrderRequestDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray COrderRequestDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


bool COrderRequestDbDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("CreatedAt")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, record.value("CreatedAt").toDateTime());
	}
	return true;
}


idoc::MetaInfoPtr COrderRequestDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo);
	return metaInfoPtr;
}


bool COrderRequestDbDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


} // namespace imtauthdb
