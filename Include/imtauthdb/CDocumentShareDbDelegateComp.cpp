// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CDocumentShareDbDelegateComp.h>


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QTimeZone>
#include <QtSql/QSqlRecord>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>
#include <imtdb/imtdb.h>


namespace imtauthdb
{


namespace
{


QVariant NullableText(const QString& value)
{
	return value.isEmpty() ? QVariant(QMetaType(QMetaType::QString)) : QVariant(value);
}


QVariant NullableTime(const QDateTime& value)
{
	return value.isValid() ? QVariant(value.toUTC()) : QVariant(QMetaType(QMetaType::QDateTime));
}


QDateTime RecordDateTime(const QSqlRecord& record, const char* field)
{
	const QVariant value = record.value(field);
	if (value.isNull()){
		return QDateTime();
	}

	QDateTime dateTime = value.toDateTime();
	if (!dateTime.isValid()){
		dateTime = QDateTime::fromString(value.toString(), Qt::ISODateWithMs);
	}

	// The columns hold UTC times without a time zone
	if (dateTime.isValid() && dateTime.timeSpec() == Qt::LocalTime){
		dateTime.setTimeZone(QTimeZone::UTC);
	}

	return dateTime;
}


} // anonymous namespace


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeableUniquePtr CDocumentShareDbDelegateComp::CreateObjectFromRecord(
			const QSqlRecord& record,
			const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	imtauth::IDocumentShareInfoUniquePtr sharePtr = m_shareFactCompPtr.CreateInstance();
	if (!sharePtr.IsValid()){
		return nullptr;
	}

	QByteArrayList targetGroupIds = imtdb::VariantToByteArray(record.value("TargetGroupIds")).split(',');
	targetGroupIds.removeAll(QByteArray());

	sharePtr->SetShareId(imtdb::VariantToByteArray(record.value("Id")));
	sharePtr->SetCollectionId(imtdb::VariantToByteArray(record.value("CollectionId")));
	sharePtr->SetEntityId(imtdb::VariantToByteArray(record.value("EntityId")));
	sharePtr->SetEntityName(record.value("EntityName").toString());
	sharePtr->SetOwnerTenantId(imtdb::VariantToByteArray(record.value("OwnerTenantId")));
	sharePtr->SetOwnerTenantName(record.value("OwnerTenantName").toString());
	sharePtr->SetTargetTenantId(imtdb::VariantToByteArray(record.value("TargetTenantId")));
	sharePtr->SetTargetTenantName(record.value("TargetTenantName").toString());
	sharePtr->SetTargetGroupIds(targetGroupIds);
	sharePtr->SetAccessLevel(static_cast<imtauth::IDocumentShareInfo::AccessLevel>(record.value("AccessLevel").toInt()));
	sharePtr->SetStatus(static_cast<imtauth::IDocumentShareInfo::ShareStatus>(record.value("Status").toInt()));
	sharePtr->SetValidUntil(RecordDateTime(record, "ValidUntil"));
	sharePtr->SetMessage(record.value("Message").toString());
	sharePtr->SetCreatedByUserId(imtdb::VariantToByteArray(record.value("CreatedByUserId")));
	sharePtr->SetCreatedAt(RecordDateTime(record, "CreatedAt"));
	sharePtr->SetUpdatedAt(RecordDateTime(record, "UpdatedAt"));

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

	const imtauth::IDocumentShareInfo* sharePtr = dynamic_cast<const imtauth::IDocumentShareInfo*>(valuePtr);
	if (sharePtr == nullptr){
		return result;
	}

	const QByteArray shareId = proposedObjectId.isEmpty() ? sharePtr->GetShareId() : proposedObjectId;
	if (shareId.isEmpty()){
		return result;
	}

	result.objectName = sharePtr->GetEntityName();
	result.query = QStringLiteral(
				R"(INSERT INTO %1 ("Id", "CollectionId", "EntityId", "EntityName", "OwnerTenantId", "OwnerTenantName", )"
				R"("TargetTenantId", "TargetTenantName", "TargetGroupIds", "AccessLevel", "Status", "ValidUntil", "Message", )"
				R"("CreatedByUserId", "CreatedAt", "UpdatedAt") VALUES (:Id, :CollectionId, :EntityId, :EntityName, :OwnerTenantId, )"
				R"(:OwnerTenantName, :TargetTenantId, :TargetTenantName, :TargetGroupIds, :AccessLevel, :Status, :ValidUntil, :Message, )"
				R"(:CreatedByUserId, :CreatedAt, :UpdatedAt))").arg(GetQualifiedTableName()).toUtf8();
	result.bindValues = CreateBindValues(*sharePtr, shareId);

	return result;
}


QByteArray CDocumentShareDbDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const istd::IChangeable& /*object*/,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	// Updates need bind values, see CreateUpdateObjectQueryWithParameters
	return QByteArray();
}


CDocumentShareDbDelegateComp::NewObjectQuery CDocumentShareDbDelegateComp::CreateUpdateObjectQueryWithParameters(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	NewObjectQuery result;

	const imtauth::IDocumentShareInfo* sharePtr = dynamic_cast<const imtauth::IDocumentShareInfo*>(&object);
	if (sharePtr == nullptr || objectId.isEmpty()){
		return result;
	}

	result.query = QStringLiteral(
				R"(UPDATE %1 SET "CollectionId" = :CollectionId, "EntityName" = :EntityName, "OwnerTenantName" = :OwnerTenantName, )"
				R"("TargetTenantName" = :TargetTenantName, "TargetGroupIds" = :TargetGroupIds, "AccessLevel" = :AccessLevel, )"
				R"("Status" = :Status, "ValidUntil" = :ValidUntil, "Message" = :Message, "UpdatedAt" = :UpdatedAt WHERE "Id" = :Id)")
				.arg(GetQualifiedTableName()).toUtf8();

	const QVariantMap allValues = CreateBindValues(*sharePtr, objectId);
	static const QStringList updatedFields = {
				":Id", ":CollectionId", ":EntityName", ":OwnerTenantName", ":TargetTenantName", ":TargetGroupIds",
				":AccessLevel", ":Status", ":ValidUntil", ":Message", ":UpdatedAt"};
	for (const QString& field : updatedFields){
		result.bindValues.insert(field, allValues.value(field));
	}

	return result;
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
		escapedIds << QStringLiteral("'%1'").arg(imtdb::SqlEncode(QString::fromUtf8(id)));
	}

	return QStringLiteral(R"(DELETE FROM %1 WHERE "Id" IN (%2);)")
				.arg(GetQualifiedTableName(), escapedIds.join(", ")).toUtf8();
}


QByteArray CDocumentShareDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* paramsPtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (paramsPtr == nullptr){
		return QByteArray();
	}

	iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(paramsPtr, "ComplexFilter");
	if (!complexFilterParamPtr.IsValid()){
		return QByteArray();
	}

	QString filterQuery;
	CreateObjectFilterQuery(*complexFilterParamPtr, filterQuery);
	if (filterQuery.isEmpty()){
		return QByteArray();
	}

	return QStringLiteral(R"(DELETE FROM %1 WHERE %2;)").arg(GetQualifiedTableName(), filterQuery).toUtf8();
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


bool CDocumentShareDbDelegateComp::SetCollectionItemMetaInfoFromRecord(
			const QSqlRecord& record,
			idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("CreatedAt")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, record.value("CreatedAt").toDateTime());
	}

	if (record.contains("UpdatedAt")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, record.value("UpdatedAt").toDateTime());
	}

	return true;
}


// protected methods

idoc::MetaInfoPtr CDocumentShareDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return idoc::MetaInfoPtr(new idoc::CStandardDocumentMetaInfo);
}


bool CDocumentShareDbDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


// private methods

QVariantMap CDocumentShareDbDelegateComp::CreateBindValues(const imtauth::IDocumentShareInfo& shareInfo, const QByteArray& shareId) const
{
	QVariantMap retVal;

	const QByteArrayList targetGroupIds = shareInfo.GetTargetGroupIds();
	const QDateTime createdAt = shareInfo.GetCreatedAt();

	retVal.insert(":Id", QString::fromUtf8(shareId));
	retVal.insert(":CollectionId", QString::fromUtf8(shareInfo.GetCollectionId()));
	retVal.insert(":EntityId", QString::fromUtf8(shareInfo.GetEntityId()));
	retVal.insert(":EntityName", NullableText(shareInfo.GetEntityName()));
	retVal.insert(":OwnerTenantId", QString::fromUtf8(shareInfo.GetOwnerTenantId()));
	retVal.insert(":OwnerTenantName", NullableText(shareInfo.GetOwnerTenantName()));
	retVal.insert(":TargetTenantId", QString::fromUtf8(shareInfo.GetTargetTenantId()));
	retVal.insert(":TargetTenantName", NullableText(shareInfo.GetTargetTenantName()));

	// Stored as ",id1,id2," so that a single LIKE '%,id,%' finds a group
	retVal.insert(":TargetGroupIds", targetGroupIds.isEmpty() ? QStringLiteral("") : QStringLiteral(",%1,").arg(QString::fromUtf8(targetGroupIds.join(','))));
	retVal.insert(":AccessLevel", static_cast<int>(shareInfo.GetAccessLevel()));
	retVal.insert(":Status", static_cast<int>(shareInfo.GetStatus()));
	retVal.insert(":ValidUntil", NullableTime(shareInfo.GetValidUntil()));
	retVal.insert(":Message", NullableText(shareInfo.GetMessage()));
	retVal.insert(":CreatedByUserId", NullableText(QString::fromUtf8(shareInfo.GetCreatedByUserId())));
	retVal.insert(":CreatedAt", createdAt.isValid() ? createdAt.toUTC() : QDateTime::currentDateTimeUtc());
	retVal.insert(":UpdatedAt", NullableTime(shareInfo.GetUpdatedAt()));

	return retVal;
}


QString CDocumentShareDbDelegateComp::GetQualifiedTableName() const
{
	const QByteArray tableScheme = GetTableScheme();
	if (!tableScheme.isEmpty()){
		return QStringLiteral(R"("%1"."%2")").arg(QString::fromUtf8(tableScheme), *m_tableNameAttrPtr);
	}

	return QStringLiteral(R"("%1")").arg(*m_tableNameAttrPtr);
}


} // namespace imtauthdb


