// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CCrossOrgGrantDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtauth/ITenantFilterParam.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <iprm/TParamsPtr.h>


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

	return QStringLiteral("'%1'").arg(imtdb::EscapeSql(value));
}


static QString NullableSqlDateTime(const QString& value)
{
	if (value.isEmpty()){
		return QStringLiteral("NULL");
	}

	return QStringLiteral("'%1'").arg(imtdb::EscapeSql(value));
}


istd::IChangeableUniquePtr CCrossOrgGrantDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_grantFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::ICrossOrgGrantData> grantPtr = m_grantFactCompPtr.CreateInstance();
	if (!grantPtr.IsValid()){
		return nullptr;
	}

	imtauth::CrossOrgGrantInfo info;
	if (record.contains("Id")){
		info.grantId = imtdb::VariantToByteArray(record.value("Id"));
	}
	if (record.contains("SourceTenantId")){
		info.sourceTenantId = imtdb::VariantToByteArray(record.value("SourceTenantId"));
	}
	if (record.contains("TargetTenantId")){
		info.targetTenantId = imtdb::VariantToByteArray(record.value("TargetTenantId"));
	}
	if (record.contains("Name")){
		info.name = record.value("Name").toString();
	}
	if (record.contains("RoleIds")){
		QString roleIdsStr = record.value("RoleIds").toString();
		if (!roleIdsStr.isEmpty()){
			QStringList parts = roleIdsStr.split(';', Qt::SkipEmptyParts);
			for (const QString& part : parts){
				info.roleIds.append(part.trimmed().toUtf8());
			}
		}
	}
	if (record.contains("Description")){
		info.description = record.value("Description").toString();
	}
	info.createdAt = RecordDateTimeToString(record, "CreatedAt");
	info.expiresAt = RecordDateTimeToString(record, "ExpiresAt");
	info.isActive = record.contains("IsActive") ? (record.value("IsActive").toInt() != 0) : true;

	grantPtr->SetGrantInfo(info);

	return grantPtr;
}


CCrossOrgGrantDbDelegateComp::NewObjectQuery CCrossOrgGrantDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery result;

	const imtauth::ICrossOrgGrantData* grantPtr = dynamic_cast<const imtauth::ICrossOrgGrantData*>(valuePtr);
	if (grantPtr == nullptr){
		return result;
	}

	imtauth::CrossOrgGrantInfo info = grantPtr->GetGrantInfo();

	QString id = imtdb::EscapeSql(QString::fromUtf8(!proposedObjectId.isEmpty() ? proposedObjectId : info.grantId));
	QString sourceTenantId = imtdb::EscapeSql(QString::fromUtf8(info.sourceTenantId));
	QString targetTenantId = imtdb::EscapeSql(QString::fromUtf8(info.targetTenantId));
	QString name = NullableSqlText(info.name);
	QString roleIdsStr = NullableSqlText(QString::fromUtf8(info.roleIds.join(';')));
	QString description = NullableSqlText(info.description);
	QString createdAt = !info.createdAt.isEmpty() ? imtdb::EscapeSql(info.createdAt) : imtdb::UtcNow();
	QString expiresAt = NullableSqlDateTime(info.expiresAt);
	int isActive = info.isActive ? 1 : 0;

	result.query = QString(
		"INSERT INTO \"%1\" (\"Id\", \"SourceTenantId\", \"TargetTenantId\", \"Name\", \"RoleIds\", \"Description\", \"CreatedAt\", \"ExpiresAt\", \"IsActive\") "
		"VALUES ('%2', '%3', '%4', %5, %6, %7, '%8', %9, %10);")
		.arg(*m_tableNameAttrPtr)
		.arg(id)
		.arg(sourceTenantId)
		.arg(targetTenantId)
		.arg(name)
		.arg(roleIdsStr)
		.arg(description)
		.arg(createdAt)
		.arg(expiresAt)
		.arg(QString::number(isActive)).toUtf8();

	return result;
}


QByteArray CCrossOrgGrantDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtauth::ICrossOrgGrantData* grantPtr = dynamic_cast<const imtauth::ICrossOrgGrantData*>(&object);
	if (grantPtr == nullptr){
		return QByteArray();
	}

	imtauth::CrossOrgGrantInfo info = grantPtr->GetGrantInfo();

	return QString(
		"UPDATE \"%1\" SET "
		"\"Name\"=%2, "
		"\"RoleIds\"=%3, "
		"\"Description\"=%4, "
		"\"ExpiresAt\"=%5, "
		"\"IsActive\"=%6 "
		"WHERE \"Id\"='%7';")
		.arg(*m_tableNameAttrPtr)
		.arg(NullableSqlText(info.name))
		.arg(NullableSqlText(QString::fromUtf8(info.roleIds.join(';'))))
		.arg(NullableSqlText(info.description))
		.arg(NullableSqlDateTime(info.expiresAt))
		.arg(QString::number(info.isActive ? 1 : 0))
		.arg(imtdb::EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


QByteArray CCrossOrgGrantDbDelegateComp::CreateDeleteObjectsQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArrayList& objectIds,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectIds.isEmpty()){
		return QByteArray();
	}

	QStringList escapedIds;
	for (const QByteArray& id : objectIds){
		escapedIds << QStringLiteral("'%1'").arg(imtdb::EscapeSql(QString::fromUtf8(id)));
	}

	return QStringLiteral(R"(DELETE FROM "%1" WHERE "Id" IN (%2);)")
			.arg(*m_tableNameAttrPtr, escapedIds.join(", ")).toUtf8();
}


QByteArray CCrossOrgGrantDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CCrossOrgGrantDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const QString& newObjectName,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectId.isEmpty()){
		return QByteArray();
	}

	return QStringLiteral(R"(UPDATE "%1" SET "Name"=%2 WHERE "Id"='%3';)")
			.arg(*m_tableNameAttrPtr)
			.arg(NullableSqlText(newObjectName))
			.arg(imtdb::EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


QByteArray CCrossOrgGrantDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


bool CCrossOrgGrantDbDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("CreatedAt")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, record.value("CreatedAt").toDateTime());
	}

	if (record.contains("Name")){
		metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_NAME, record.value("Name").toString());
	}

	if (record.contains("Description")){
		metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION, record.value("Description").toString());
	}

	return true;
}


QString CCrossOrgGrantDbDelegateComp::GetBaseSelectionQuery() const
{
	// The "Name" column is stored directly on the grant (auto-generated from the
	// target tenant and roles when not set explicitly), so no join is required.
	return QStringLiteral(R"(SELECT * FROM "%1")").arg(*m_tableNameAttrPtr);
}


QString CCrossOrgGrantDbDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	// Restrict grant visibility: only return grants where the requesting tenant
	// is either the source (grantor) or the target (grantee).
	iprm::TParamsPtr<imtauth::ITenantFilterParam> tenantFilterPtr(&filterParams, "TenantFilter");
	if (tenantFilterPtr.IsValid()){
		QByteArray tenantId = tenantFilterPtr->GetTenantId();
		if (!tenantId.isEmpty()){
			QString escapedTenantId = imtdb::EscapeSql(QString::fromUtf8(tenantId));
			return QStringLiteral(R"(("SourceTenantId"='%1' OR "TargetTenantId"='%1'))")
					.arg(escapedTenantId);
		}
	}

	return QString();
}


idoc::MetaInfoPtr CCrossOrgGrantDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo);
	return metaInfoPtr;
}


bool CCrossOrgGrantDbDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


} // namespace imtauthdb
