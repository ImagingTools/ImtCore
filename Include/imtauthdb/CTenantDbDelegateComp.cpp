// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CTenantDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QRegularExpression>
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ACF includes
#include <imtauth/ITenantInfo.h>
#include <iprm/IIdParam.h>
#include <iprm/TParamsPtr.h>
#include <imtbase/IComplexCollectionFilter.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtauthdb
{


namespace
{

const char s_tenantRelationScopeFieldId[] = "TenantRelationScope";
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


QString CTenantDbDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

	if (paramIds.contains("UserId")){
		iprm::TParamsPtr<iprm::IIdParam> userIdParamPtr(&filterParams, "UserId");
		if (userIdParamPtr.IsValid()){
			QByteArray userId = userIdParamPtr->GetId();
			if (!userId.isEmpty()){
				QString escapedUserId = imtdb::EscapeSql(QString::fromUtf8(userId));

				// Check if ComplexFilter contains a TenantRelationScope field filter
				QString scopeValue;
				if (paramIds.contains("ComplexFilter")){
					iprm::TParamsPtr<imtbase::IComplexCollectionFilter> complexFilterPtr(&filterParams, "ComplexFilter");
					if (complexFilterPtr.IsValid()){
						const imtbase::IComplexCollectionFilter::FilterExpression& expr = complexFilterPtr->GetFilterExpression();
						for (const imtbase::IComplexCollectionFilter::FieldFilter& ff : expr.fieldFilters){
							if (ff.fieldId == s_tenantRelationScopeFieldId){
								scopeValue = ff.filterValue.toString();
								break;
							}
						}
					}
				}

				if (scopeValue == "Owner"){
					return QString("\"OwnerId\"='%1'").arg(escapedUserId);
				}
				else if (scopeValue == "Member"){
					return QString("\"Id\" IN "
						"(SELECT \"TenantId\" FROM \"TenantMemberships\" WHERE \"UserId\"='%1' AND \"IsActive\"=true)")
						.arg(escapedUserId);
				}
				else if (scopeValue == "Invited"){
					return QString("\"Id\" IN "
						"(SELECT \"TenantId\"::uuid FROM \"TenantInvitations\" WHERE \"UserId\"='%1' AND \"Status\"=%2)")
						.arg(escapedUserId)
						.arg(s_invitationStatusPending);
				}
				else{
					// Default: show tenants where user is owner, member, or invited
					return QString("(\"OwnerId\"='%1' OR \"Id\" IN "
						"(SELECT \"TenantId\" FROM \"TenantMemberships\" WHERE \"UserId\"='%1' AND \"IsActive\"=true) OR \"Id\" IN "
						"(SELECT \"TenantId\"::uuid FROM \"TenantInvitations\" WHERE \"UserId\"='%1' AND \"Status\"=%2))")
						.arg(escapedUserId)
						.arg(s_invitationStatusPending);
				}
			}
		}
	}

	return QString();
}


bool CTenantDbDelegateComp::CreateObjectFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& filterQuery) const
{
	// Strip TenantRelationScope from the filter expression before generating SQL —
	// it is a virtual field handled by CreateAdditionalFiltersQuery, not a real column.
	const imtbase::IComplexCollectionFilter::FilterExpression& originalExpr = collectionFilter.GetFilterExpression();

	bool hasTenantRelationScope = false;
	for (const imtbase::IComplexCollectionFilter::FieldFilter& ff : originalExpr.fieldFilters){
		if (ff.fieldId == s_tenantRelationScopeFieldId){
			hasTenantRelationScope = true;
			break;
		}
	}

	if (!hasTenantRelationScope){
		// No virtual field — use base implementation as-is
		return BaseClass::CreateObjectFilterQuery(collectionFilter, filterQuery);
	}

	// Let the base class generate the full SQL, then strip TenantRelationScope clauses
	if (!BaseClass::CreateObjectFilterQuery(collectionFilter, filterQuery)){
		return false;
	}

	if (filterQuery.isEmpty()){
		return true;
	}

	// The converter generates clauses like: ("TenantRelationScope")::text = 'Value'
	// These need to be removed since it's not a real column.
	// Strategy: remove the TenantRelationScope clause and any surrounding AND/OR connector.
	// Pattern in Postgres context: ("TenantRelationScope")::text = 'Value'
	static const QRegularExpression tenantScopeClauseRe(
		QStringLiteral(R"(\s*(?:AND|OR)\s+\(\"TenantRelationScope\"\)::text\s*=\s*'[^']*')")
		+ QStringLiteral("|")
		+ QStringLiteral(R"(\(\"TenantRelationScope\"\)::text\s*=\s*'[^']*'\s*(?:AND|OR)\s*)")
		+ QStringLiteral("|")
		+ QStringLiteral(R"(\(\"TenantRelationScope\"\)::text\s*=\s*'[^']*')")
	);

	filterQuery.replace(tenantScopeClauseRe, QString());
	filterQuery = filterQuery.trimmed();

	// If the entire filter was just TenantRelationScope, clear it
	QString trimmed = filterQuery.trimmed();
	trimmed.remove(QRegularExpression(QStringLiteral(R"(^\(\s*\)$)")));
	filterQuery = trimmed.trimmed();

	return true;
}


} // namespace imtauthdb
