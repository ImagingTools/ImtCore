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
		membershipPtr->SetMembershipId(record.value("Id").toByteArray());
	}
	if (record.contains("UserId")){
		membershipPtr->SetUserId(record.value("UserId").toByteArray());
	}
	if (record.contains("TenantId")){
		membershipPtr->SetTenantId(record.value("TenantId").toByteArray());
	}
	if (record.contains("Role")){
		membershipPtr->SetRole(static_cast<imtauth::ITenantMembership::TenantMemberRole>(record.value("Role").toInt()));
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
	int role = membershipPtr != nullptr ? static_cast<int>(membershipPtr->GetRole()) : static_cast<int>(imtauth::ITenantMembership::TMR_MEMBER);
	bool isActive = membershipPtr != nullptr ? membershipPtr->IsActive() : true;
	QString now = imtdb::UtcNow();

	result.query = QString(
		"INSERT INTO \"%1\" (\"Id\", \"UserId\", \"TenantId\", \"Role\", \"IsActive\", \"JoinedAt\", \"UpdatedAt\") "
		"VALUES ('%2', '%3', '%4', %5, %6, '%7', '%8');")
		.arg(*m_tableNameAttrPtr,
			 id,
			 userId,
			 tenantId,
			 QString::number(role),
			 isActive ? "true" : "false",
			 now,
			 now).toUtf8();

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

	return QString(
		"UPDATE \"%1\" SET "
		"\"Role\"=%2, "
		"\"IsActive\"=%3, "
		"\"UpdatedAt\"='%4' "
		"WHERE \"Id\"='%5';")
		.arg(*m_tableNameAttrPtr,
			 QString::number(static_cast<int>(membershipPtr->GetRole())),
			 membershipPtr->IsActive() ? "true" : "false",
			 now,
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
		escapedIds << QString("'%1'").arg(imtdb::EscapeSql(QString::fromUtf8(id)));
	}

	return QString("DELETE FROM \"%1\" WHERE \"Id\" IN (%2);")
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
