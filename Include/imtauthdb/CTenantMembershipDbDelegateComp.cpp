// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CTenantMembershipDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ACF includes
#include <imtauth/ITenantMembership.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtauthdb
{


istd::IChangeableUniquePtr CTenantMembershipDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid() || !m_membershipFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::ITenantMembership> membershipPtr = m_membershipFactCompPtr.CreateInstance();
	if (!membershipPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		membershipPtr->SetMembershipId(imtdb::VariantToByteArray(record.value("Id")));
	}
	if (record.contains("UserId")){
		membershipPtr->SetUserId(imtdb::VariantToByteArray(record.value("UserId")));
	}
	if (record.contains("TenantId")){
		membershipPtr->SetTenantId(imtdb::VariantToByteArray(record.value("TenantId")));
	}
	if (record.contains("IsActive")){
		membershipPtr->SetActive(record.value("IsActive").toBool());
	}
	if (record.contains("JoinedAt")){
		QVariant val = record.value("JoinedAt");
		QDateTime dt = val.toDateTime();
		membershipPtr->SetJoinedAt(dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString());
	}
	if (record.contains("UpdatedAt")){
		QVariant val = record.value("UpdatedAt");
		QDateTime dt = val.toDateTime();
		membershipPtr->SetUpdatedAt(dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString());
	}

	// Load org-specific permissions for the membership (pipe-separated in OrgPermissions column)
	if (record.contains("OrgPermissions")){
		QString permsStr = record.value("OrgPermissions").toString();
		QByteArrayList perms;
		if (!permsStr.isEmpty()){
			QStringList parts = permsStr.split('|', Qt::SkipEmptyParts);
			for (const QString& p : parts){
				QByteArray np = p.trimmed().toUtf8();
				if (!np.isEmpty()) perms << np;
			}
		}
		membershipPtr->SetOrganizationPermissions(perms);
	}

	return membershipPtr;
}


CTenantMembershipDbDelegateComp::NewObjectQuery CTenantMembershipDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery result;

	const imtauth::ITenantMembership* membershipPtr = dynamic_cast<const imtauth::ITenantMembership*>(valuePtr);

	QString id;
	if (!proposedObjectId.isEmpty()){
		id = imtdb::EscapeSql(QString::fromUtf8(proposedObjectId));
	}
	else{
		id = QUuid::createUuid().toString(QUuid::WithoutBraces);
	}

	QString userId = membershipPtr != nullptr ? imtdb::EscapeSql(QString::fromUtf8(membershipPtr->GetUserId())) : QString();
	QString tenantId = membershipPtr != nullptr ? imtdb::EscapeSql(QString::fromUtf8(membershipPtr->GetTenantId())) : QString();
	bool isActive = membershipPtr != nullptr ? membershipPtr->IsActive() : true;
	QString now = imtdb::UtcNow();

	QByteArrayList orgPerms = membershipPtr != nullptr ? membershipPtr->GetOrganizationPermissions() : QByteArrayList();
	QString orgPermsStr = orgPerms.isEmpty() ? QString() : QString::fromUtf8(orgPerms.join('|'));

	result.query = QString(
		"INSERT INTO \"%1\" (\"Id\", \"UserId\", \"TenantId\", \"IsActive\", \"JoinedAt\", \"UpdatedAt\", \"OrgPermissions\") "
		"VALUES ('%2', '%3', '%4', %5, '%6', '%7', '%8');")
		.arg(*m_tableNameAttrPtr,
			 id,
			 userId,
			 tenantId,
			 isActive ? "true" : "false",
			 now,
			 now,
			 imtdb::EscapeSql(orgPermsStr)).toUtf8();

	return result;
}


QByteArray CTenantMembershipDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtauth::ITenantMembership* membershipPtr = dynamic_cast<const imtauth::ITenantMembership*>(&object);
	if (membershipPtr == nullptr){
		return QByteArray();
	}

	QString escapedId = imtdb::EscapeSql(QString::fromUtf8(objectId));
	QString now = imtdb::UtcNow();

	QByteArrayList orgPerms = membershipPtr->GetOrganizationPermissions();
	QString orgPermsStr = orgPerms.isEmpty() ? QString() : QString::fromUtf8(orgPerms.join('|'));

	return QString(
		"UPDATE \"%1\" SET "
		"\"IsActive\"=%2, "
		"\"UpdatedAt\"='%3', "
		"\"OrgPermissions\"='%4' "
		"WHERE \"Id\"='%5';")
		.arg(*m_tableNameAttrPtr,
			 membershipPtr->IsActive() ? "true" : "false",
			 now,
			 imtdb::EscapeSql(orgPermsStr),
			 escapedId).toUtf8();
}


QByteArray CTenantMembershipDbDelegateComp::CreateDeleteObjectsQuery(
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


QByteArray CTenantMembershipDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTenantMembershipDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	// Memberships don't have a name field
	return QByteArray();
}


QByteArray CTenantMembershipDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	// Memberships don't have a description field
	return QByteArray();
}


bool CTenantMembershipDbDelegateComp::SetCollectionItemMetaInfoFromRecord(
		const QSqlRecord& record,
		idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("JoinedAt")){
		QDateTime joinedAtTime = record.value("JoinedAt").toDateTime();
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, joinedAtTime);
	}

	if (record.contains("UpdatedAt")){
		QDateTime updatedAtTime = record.value("UpdatedAt").toDateTime();
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, updatedAtTime);
	}

	return true;
}


idoc::MetaInfoPtr CTenantMembershipDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo);
	return metaInfoPtr;
}


bool CTenantMembershipDbDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


} // namespace imtauthdb
