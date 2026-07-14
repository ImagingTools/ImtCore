// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CTenantEntityBindingDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtauth/ITenantEntityBinding.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <iprm/TParamsPtr.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtauthdb
{


istd::IChangeableUniquePtr CTenantEntityBindingDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid() || !m_bindingFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::ITenantEntityBinding> bindingPtr = m_bindingFactCompPtr.CreateInstance();
	if (!bindingPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("TenantId")){
		bindingPtr->SetTenantId(imtdb::VariantToByteArray(record.value("TenantId")));
	}
	if (record.contains("EntityType")){
		bindingPtr->SetEntityType(imtdb::VariantToByteArray(record.value("EntityType")));
	}
	if (record.contains("EntityId")){
		bindingPtr->SetEntityId(imtdb::VariantToByteArray(record.value("EntityId")));
	}
	if (record.contains("CreatedAt")){
		QVariant val = record.value("CreatedAt");
		QDateTime dt = val.toDateTime();
		bindingPtr->SetCreatedAt(dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString());
	}
	if (record.contains("CreatedByUserId")){
		bindingPtr->SetCreatedByUserId(imtdb::VariantToByteArray(record.value("CreatedByUserId")));
	}

	return bindingPtr;
}


CTenantEntityBindingDbDelegateComp::NewObjectQuery CTenantEntityBindingDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery result;

	const imtauth::ITenantEntityBinding* bindingPtr = dynamic_cast<const imtauth::ITenantEntityBinding*>(valuePtr);
	if (bindingPtr == nullptr){
		return result;
	}

	QString id;
	if (!proposedObjectId.isEmpty()){
		id = imtdb::EscapeSql(QString::fromUtf8(proposedObjectId));
	}
	else{
		id = QUuid::createUuid().toString(QUuid::WithoutBraces);
	}

	QString tenantId = imtdb::EscapeSql(QString::fromUtf8(bindingPtr->GetTenantId()));
	QString entityType = imtdb::EscapeSql(QString::fromUtf8(bindingPtr->GetEntityType()));
	QString entityId = imtdb::EscapeSql(QString::fromUtf8(bindingPtr->GetEntityId()));
	QString createdAt = !bindingPtr->GetCreatedAt().isEmpty() ? imtdb::EscapeSql(bindingPtr->GetCreatedAt()) : imtdb::UtcNow();
	QByteArray createdByUserId = bindingPtr->GetCreatedByUserId();

	QString createdByLiteral = createdByUserId.isEmpty()
			? QStringLiteral("NULL")
			: QString("'%1'").arg(imtdb::EscapeSql(QString::fromUtf8(createdByUserId)));

	const bool isSqlite = m_databaseEngineCompPtr.IsValid() &&
			m_databaseEngineCompPtr->GetDatabaseDriverId() == "QSQLITE";
	const QString conflictPrefix = isSqlite
			? QStringLiteral("INSERT OR IGNORE INTO")
			: QStringLiteral("INSERT INTO");
	const QString onConflictSuffix = isSqlite
			? QString()
			: QStringLiteral(" ON CONFLICT (\"TenantId\", \"EntityType\", \"EntityId\") DO NOTHING");

	result.query = QString(
		"%1 \"%2\" (\"Id\", \"TenantId\", \"EntityType\", \"EntityId\", \"CreatedAt\", \"CreatedByUserId\") "
		"VALUES ('%3', '%4', '%5', '%6', '%7', %8)%9;")
		.arg(conflictPrefix,
			 *m_tableNameAttrPtr,
			 id,
			 tenantId,
			 entityType,
			 entityId,
			 createdAt,
			 createdByLiteral,
			 onConflictSuffix).toUtf8();

	return result;
}


QByteArray CTenantEntityBindingDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const istd::IChangeable& /*object*/,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	// Bindings are immutable — no update logic needed
	return QByteArray();
}


QByteArray CTenantEntityBindingDbDelegateComp::CreateDeleteObjectsQuery(
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


QByteArray CTenantEntityBindingDbDelegateComp::CreateDeleteObjectSetQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const iprm::IParamsSet* paramsPtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (paramsPtr == nullptr){
		// No filter means delete all — return a simple delete
		return QString("DELETE FROM \"%1\";").arg(*m_tableNameAttrPtr).toUtf8();
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

	return QString("DELETE FROM \"%1\" WHERE %2;")
			.arg(*m_tableNameAttrPtr, filterQuery).toUtf8();
}


QByteArray CTenantEntityBindingDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTenantEntityBindingDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


bool CTenantEntityBindingDbDelegateComp::SetCollectionItemMetaInfoFromRecord(
		const QSqlRecord& record,
		idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("CreatedAt")){
		QDateTime createdAtTime = record.value("CreatedAt").toDateTime();
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, createdAtTime);
	}

	return true;
}


idoc::MetaInfoPtr CTenantEntityBindingDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo);
	return metaInfoPtr;
}


bool CTenantEntityBindingDbDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


} // namespace imtauthdb
