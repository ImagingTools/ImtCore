// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CTenantInvitationDbDelegateComp.h>

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


static QString NullableSqlDateTime(const QString& value)
{
	if (value.isEmpty()){
		return imtdb::NULL_DATA_LITERAL;
	}

	return QStringLiteral("'%1'").arg(imtdb::EscapeSql(value));
}


static QString NullableSqlText(const QString& value)
{
	if (value.isEmpty()){
		return imtdb::NULL_DATA_LITERAL;
	}

	return QStringLiteral("'%1'").arg(imtdb::EscapeSql(value));
}


istd::IChangeableUniquePtr CTenantInvitationDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_invitationFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::ITenantInvitation> invitationPtr = m_invitationFactCompPtr.CreateInstance();
	if (!invitationPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		invitationPtr->SetInvitationId(imtdb::VariantToByteArray(record.value("Id")));
	}
	if (record.contains("UserId")){
		invitationPtr->SetUserId(imtdb::VariantToByteArray(record.value("UserId")));
	}
	if (record.contains("TenantId")){
		invitationPtr->SetTenantId(imtdb::VariantToByteArray(record.value("TenantId")));
	}
	if (record.contains("Status")){
		invitationPtr->SetStatus(static_cast<imtauth::ITenantInvitation::TenantInvitationStatus>(record.value("Status").toInt()));
	}
	if (record.contains("InvitedByUserId")){
		invitationPtr->SetInvitedByUserId(imtdb::VariantToByteArray(record.value("InvitedByUserId")));
	}
	if (record.contains("RevokedByUserId")){
		invitationPtr->SetRevokedByUserId(imtdb::VariantToByteArray(record.value("RevokedByUserId")));
	}

	invitationPtr->SetCreatedAt(RecordDateTimeToString(record, "CreatedAt"));
	invitationPtr->SetUpdatedAt(RecordDateTimeToString(record, "UpdatedAt"));
	invitationPtr->SetExpiresAt(RecordDateTimeToString(record, "ExpiresAt"));
	invitationPtr->SetAcceptedAt(RecordDateTimeToString(record, "AcceptedAt"));
	invitationPtr->SetRejectedAt(RecordDateTimeToString(record, "RejectedAt"));
	invitationPtr->SetRevokedAt(RecordDateTimeToString(record, "RevokedAt"));
	invitationPtr->SetLastSentAt(RecordDateTimeToString(record, "LastSentAt"));

	return invitationPtr;
}


CTenantInvitationDbDelegateComp::NewObjectQuery CTenantInvitationDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery result;

	const imtauth::ITenantInvitation* invitationPtr = dynamic_cast<const imtauth::ITenantInvitation*>(valuePtr);
	if (invitationPtr == nullptr){
		return result;
	}

	QString id = imtdb::EscapeSql(QString::fromUtf8(!proposedObjectId.isEmpty() ? proposedObjectId : invitationPtr->GetInvitationId()));
	QString userId = imtdb::EscapeSql(QString::fromUtf8(invitationPtr->GetUserId()));
	QString tenantId = imtdb::EscapeSql(QString::fromUtf8(invitationPtr->GetTenantId()));
	QString invitedByUserId = imtdb::EscapeSql(QString::fromUtf8(invitationPtr->GetInvitedByUserId()));
	QString revokedByUserId = NullableSqlText(QString::fromUtf8(invitationPtr->GetRevokedByUserId()));
	int status = static_cast<int>(invitationPtr->GetStatus());
	QString createdAt = !invitationPtr->GetCreatedAt().isEmpty() ? imtdb::EscapeSql(invitationPtr->GetCreatedAt()) : imtdb::UtcNow();
	QString updatedAt = !invitationPtr->GetUpdatedAt().isEmpty() ? imtdb::EscapeSql(invitationPtr->GetUpdatedAt()) : createdAt;
	QString expiresAt = imtdb::EscapeSql(invitationPtr->GetExpiresAt());
	QString acceptedAt = NullableSqlDateTime(invitationPtr->GetAcceptedAt());
	QString rejectedAt = NullableSqlDateTime(invitationPtr->GetRejectedAt());
	QString revokedAt = NullableSqlDateTime(invitationPtr->GetRevokedAt());
	QString lastSentAt = imtdb::EscapeSql(invitationPtr->GetLastSentAt());

	result.query = QString(
		"INSERT INTO \"%1\" (\"Id\", \"UserId\", \"TenantId\", \"Status\", \"InvitedByUserId\", \"CreatedAt\", \"UpdatedAt\", \"ExpiresAt\", \"AcceptedAt\", \"RejectedAt\", \"RevokedAt\", \"RevokedByUserId\", \"LastSentAt\") "
		"VALUES ('%2', '%3', '%4', %5, '%6', '%7', '%8', '%9', %10, %11, %12, %13, '%14');")
		.arg(*m_tableNameAttrPtr)
		.arg(id)
		.arg(userId)
		.arg(tenantId)
		.arg(QString::number(status))
		.arg(invitedByUserId)
		.arg(createdAt)
		.arg(updatedAt)
		.arg(expiresAt)
		.arg(acceptedAt)
		.arg(rejectedAt)
		.arg(revokedAt)
		.arg(revokedByUserId)
		.arg(lastSentAt).toUtf8();

	return result;
}


QByteArray CTenantInvitationDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtauth::ITenantInvitation* invitationPtr = dynamic_cast<const imtauth::ITenantInvitation*>(&object);
	if (invitationPtr == nullptr){
		return QByteArray();
	}

	return QString(
		"UPDATE \"%1\" SET "
		"\"Status\"=%2, "
		"\"UpdatedAt\"='%3', "
		"\"ExpiresAt\"='%4', "
		"\"AcceptedAt\"=%5, "
		"\"RejectedAt\"=%6, "
		"\"RevokedAt\"=%7, "
		"\"RevokedByUserId\"=%8, "
		"\"LastSentAt\"='%9' "
		"WHERE \"Id\"='%10';")
		.arg(*m_tableNameAttrPtr)
		.arg(QString::number(static_cast<int>(invitationPtr->GetStatus())))
		.arg(imtdb::EscapeSql(invitationPtr->GetUpdatedAt()))
		.arg(imtdb::EscapeSql(invitationPtr->GetExpiresAt()))
		.arg(NullableSqlDateTime(invitationPtr->GetAcceptedAt()))
		.arg(NullableSqlDateTime(invitationPtr->GetRejectedAt()))
		.arg(NullableSqlDateTime(invitationPtr->GetRevokedAt()))
		.arg(NullableSqlText(QString::fromUtf8(invitationPtr->GetRevokedByUserId())))
		.arg(imtdb::EscapeSql(invitationPtr->GetLastSentAt()))
		.arg(imtdb::EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


QByteArray CTenantInvitationDbDelegateComp::CreateDeleteObjectsQuery(
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


QByteArray CTenantInvitationDbDelegateComp::CreateDeleteObjectSetQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const iprm::IParamsSet* /*paramsPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTenantInvitationDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTenantInvitationDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


bool CTenantInvitationDbDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("CreatedAt")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, record.value("CreatedAt").toDateTime());
	}
	if (record.contains("UpdatedAt")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, record.value("UpdatedAt").toDateTime());
	}
	return true;
}


idoc::MetaInfoPtr CTenantInvitationDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo);
	return metaInfoPtr;
}


bool CTenantInvitationDbDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


} // namespace imtauthdb
