// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CTenantDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ACF includes
#include <istd/TOptDelPtr.h>
#include <imtauth/ITenantInfo.h>
#include <iprm/IIdParam.h>
#include <iprm/CParamsSet.h>
#include <iprm/TParamsPtr.h>
#include <imtbase/ICollectionFilter.h>
#include <imtbase/IComplexCollectionFilter.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtauthdb
{


namespace
{

const int s_invitationStatusPending = 0;

} // namespace


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
	if (record.contains("OwnerId")){
		tenantPtr->SetOwnerId(record.value("OwnerId").toByteArray());
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
		id = imtdb::EscapeSql(QString::fromUtf8(proposedObjectId));
	}
	else{
		id = QUuid::createUuid().toString(QUuid::WithoutBraces);
	}

	QString name = imtdb::EscapeSql(tenantPtr != nullptr ? tenantPtr->GetTenantName() : objectName);
	QString description = imtdb::EscapeSql(tenantPtr != nullptr ? tenantPtr->GetTenantDescription() : objectDescription);
	QString ownerId = imtdb::EscapeSql(tenantPtr != nullptr ? QString::fromUtf8(tenantPtr->GetOwnerId()) : QString());
	bool isActive = tenantPtr != nullptr ? tenantPtr->IsActive() : true;
	QString now = imtdb::UtcNow();

	result.query = QString(
		"INSERT INTO \"%1\" (\"Id\", \"Name\", \"Description\", \"OwnerId\", \"IsActive\", \"CreatedAt\", \"UpdatedAt\") "
		"VALUES ('%2', '%3', '%4', '%5', %6, '%7', '%8');")
		.arg(*m_tableNameAttrPtr,
			 id,
			 name,
			 description,
			 ownerId,
			 isActive ? "true" : "false",
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

	QString escapedId = imtdb::EscapeSql(QString::fromUtf8(objectId));
	QString now = imtdb::UtcNow();

	return QString(
		"UPDATE \"%1\" SET "
		"\"Name\"='%2', "
		"\"Description\"='%3', "
		"\"OwnerId\"='%4', "
		"\"IsActive\"=%5, "
		"\"UpdatedAt\"='%6' "
		"WHERE \"Id\"='%7';")
		.arg(*m_tableNameAttrPtr,
			 imtdb::EscapeSql(tenantPtr->GetTenantName()),
			 imtdb::EscapeSql(tenantPtr->GetTenantDescription()),
			 imtdb::EscapeSql(QString::fromUtf8(tenantPtr->GetOwnerId())),
			 tenantPtr->IsActive() ? "true" : "false",
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
		escapedIds << QString("'%1'").arg(imtdb::EscapeSql(QString::fromUtf8(id)));
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
				 imtdb::EscapeSql(newObjectName),
				 imtdb::UtcNow(),
				 imtdb::EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


QByteArray CTenantDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const QString& description,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QString("UPDATE \"%1\" SET \"Description\"='%2', \"UpdatedAt\"='%3' WHERE \"Id\"='%4';")
			.arg(*m_tableNameAttrPtr,
				 imtdb::EscapeSql(description),
				 imtdb::UtcNow(),
				 imtdb::EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


bool CTenantDbDelegateComp::SetCollectionItemMetaInfoFromRecord(
		const QSqlRecord& record,
		idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("Name")){
		metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_NAME, record.value("Name").toString());
	}

	if (record.contains("Description")){
		metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION, record.value("Description").toString());
	}

	if (record.contains("UpdatedAt")){
		QDateTime lastModificationTime = record.value("UpdatedAt").toDateTime();
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, lastModificationTime);
	}

	if (record.contains("CreatedAt")){
		QDateTime createdAtTime = record.value("CreatedAt").toDateTime();
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, createdAtTime);
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


QByteArray CTenantDbDelegateComp::ExtractUserId(const iprm::IParamsSet* paramsPtr)
{
	if (paramsPtr == nullptr){
		return QByteArray();
	}

	iprm::IParamsSet::Ids paramIds = paramsPtr->GetParamIds();
	if (paramIds.contains("UserId")){
		iprm::TParamsPtr<iprm::IIdParam> userIdParamPtr(paramsPtr, "UserId");
		if (userIdParamPtr.IsValid()){
			return userIdParamPtr->GetId();
		}
	}

	return QByteArray();
}


QString CTenantDbDelegateComp::GetTenantRelationScopeSubquery(const QByteArray& userId) const
{
	QString tableName = qPrintable(*m_tableNameAttrPtr);

	// For admin users (no userId), add NULL columns so filters referencing them don't cause SQL errors
	if (userId.isEmpty()){
		return QString("SELECT *, NULL AS \"TenantRelationScope\", NULL AS \"InvitationId\" FROM \"%1\"").arg(tableName);
	}

	QString escapedUserId = imtdb::EscapeSql(QString::fromUtf8(userId));

	return QString(
		"SELECT *, "
		"CASE "
		"WHEN \"OwnerId\"='%1' THEN 'Owner' "
		"WHEN \"Id\" IN (SELECT \"TenantId\" FROM \"TenantMemberships\" WHERE \"UserId\"='%1' AND \"IsActive\"=true) THEN 'Member' "
		"WHEN \"Id\" IN (SELECT \"TenantId\"::uuid FROM \"TenantInvitations\" WHERE \"UserId\"='%1' AND \"Status\"=%2) THEN 'Invited' "
		"ELSE NULL "
		"END AS \"TenantRelationScope\", "
		"(SELECT \"Id\" FROM \"TenantInvitations\" WHERE \"TenantId\"::uuid = \"%3\".\"Id\" AND \"UserId\"='%1' AND \"Status\"=%2 LIMIT 1) AS \"InvitationId\" "
		"FROM \"%3\"")
		.arg(escapedUserId)
		.arg(s_invitationStatusPending)
		.arg(tableName);
}


QByteArray CTenantDbDelegateComp::GetSelectionQuery(
		const QByteArray& objectId,
		int offset,
		int count,
		const iprm::IParamsSet* paramsPtr) const
{
	// Single-object selection does not need the computed column
	if (!objectId.isEmpty()){
		return BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);
	}

	QByteArray userId = ExtractUserId(paramsPtr);

	if (count == 0){
		return QByteArray();
	}

	QString sortQuery;
	QString filterQuery;

	istd::TOptDelPtr<const iprm::IParamsSet> selectionParamsPtr;
	if (paramsPtr != nullptr){
		selectionParamsPtr.SetPtr(paramsPtr, false);
	}
	else{
		selectionParamsPtr.SetPtr(new iprm::CParamsSet(), true);
	}

	if (selectionParamsPtr.IsValid()){
		if (!CreateFilterQuery(*selectionParamsPtr, filterQuery)){
			return QByteArray();
		}

		iprm::IParamsSet::Ids paramIds = selectionParamsPtr->GetParamIds();
		if (paramIds.contains("ComplexFilter")){
			iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterParamPtr(selectionParamsPtr.GetPtr(), "ComplexFilter");
			if (!CreateSortQuery(*complexFilterParamPtr, sortQuery)){
				return QByteArray();
			}
		}
		else if (paramIds.contains("Filter")){
			iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(selectionParamsPtr.GetPtr(), "Filter");
			if (!CreateSortQuery(*collectionFilterParamPtr, sortQuery)){
				return QByteArray();
			}
		}
	}

	QByteArray paginationQuery;
	if (!CreatePaginationQuery(offset, count, paginationQuery)){
		return QByteArray();
	}

	QString baseQuery = QString("SELECT * FROM (%1) AS _t").arg(GetTenantRelationScopeSubquery(userId));

	// Due to a bug in qt in the context of resolving of an expression like this: '%<SOME_NUMBER>%'
	QString retVal = "(" + baseQuery;
	retVal += QString(" ") + filterQuery;
	retVal += QString(" ") + qPrintable(paginationQuery) + ")";
	retVal += QString(" ") + sortQuery;

	return retVal.toUtf8();
}


QByteArray CTenantDbDelegateComp::GetCountQuery(const iprm::IParamsSet* paramsPtr) const
{
	QByteArray userId = ExtractUserId(paramsPtr);

	QString filterQuery;
	if (paramsPtr != nullptr){
		if (!CreateFilterQuery(*paramsPtr, filterQuery)){
			return QByteArray();
		}
	}

	return QString("SELECT COUNT(*) FROM (%1) AS _t %2")
			.arg(GetTenantRelationScopeSubquery(userId))
			.arg(filterQuery)
			.toUtf8();
}


QString CTenantDbDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	QByteArray userId = ExtractUserId(&filterParams);
	if (userId.isEmpty()){
		return QString();
	}

	// With TenantRelationScope as a computed column in the subquery,
	// only show tenants where the user has a relationship (Owner, Member, or Invited)
	return QStringLiteral("\"TenantRelationScope\" IS NOT NULL");
}


} // namespace imtauthdb
