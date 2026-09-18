// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CTenantRelationshipProposalDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtauthdb
{


static QString NullableSqlText(const QString& value)
{
	if (value.isEmpty()){
		return imtdb::NULL_DATA_LITERAL;
	}

	return QStringLiteral("'%1'").arg(imtdb::EscapeSql(value));
}


static QString NullableSqlDateTime(const QString& value)
{
	if (value.isEmpty()){
		return imtdb::NULL_DATA_LITERAL;
	}

	return QStringLiteral("'%1'").arg(imtdb::EscapeSql(value));
}


static QString RecordDateTimeToString(const QSqlRecord& record, const char* field)
{
	if (!record.contains(field)){
		return QString();
	}

	QVariant val = record.value(field);
	QDateTime dt = val.toDateTime();
	return dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString();
}


istd::IChangeableUniquePtr CTenantRelationshipProposalDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_proposalFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::ITenantRelationshipProposalInfo> proposalPtr = m_proposalFactCompPtr.CreateInstance();
	if (!proposalPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		proposalPtr->SetProposalId(imtdb::VariantToByteArray(record.value("Id")));
	}
	if (record.contains("ConnectionId")){
		proposalPtr->SetConnectionId(imtdb::VariantToByteArray(record.value("ConnectionId")));
	}
	if (record.contains("ExistingRelationshipId")){
		proposalPtr->SetExistingRelationshipId(imtdb::VariantToByteArray(record.value("ExistingRelationshipId")));
	}
	if (record.contains("ProposalType")){
		proposalPtr->SetProposalType(static_cast<imtauth::ITenantRelationshipProposalInfo::RelationshipProposalType>(record.value("ProposalType").toInt()));
	}
	if (record.contains("InitiatorTenantId")){
		proposalPtr->SetInitiatorTenantId(imtdb::VariantToByteArray(record.value("InitiatorTenantId")));
	}
	if (record.contains("CounterpartyTenantId")){
		proposalPtr->SetCounterpartyTenantId(imtdb::VariantToByteArray(record.value("CounterpartyTenantId")));
	}
	if (record.contains("ProposedSourceRole")){
		proposalPtr->SetProposedSourceRole(static_cast<imtauth::ITenantRelationshipInfo::TenantRelationshipRole>(record.value("ProposedSourceRole").toInt()));
	}
	if (record.contains("ProposedTargetRole")){
		proposalPtr->SetProposedTargetRole(static_cast<imtauth::ITenantRelationshipInfo::TenantRelationshipRole>(record.value("ProposedTargetRole").toInt()));
	}
	if (record.contains("ProposedScope")){
		proposalPtr->SetProposedScope(record.value("ProposedScope").toString());
	}
	if (record.contains("ProposedDescription")){
		proposalPtr->SetProposedDescription(record.value("ProposedDescription").toString());
	}
	if (record.contains("ProposedValidFrom")){
		proposalPtr->SetProposedValidFrom(record.value("ProposedValidFrom").toString());
	}
	if (record.contains("ProposedValidUntil")){
		proposalPtr->SetProposedValidUntil(record.value("ProposedValidUntil").toString());
	}
	if (record.contains("Status")){
		proposalPtr->SetStatus(static_cast<imtauth::ITenantRelationshipProposalInfo::RelationshipProposalStatus>(record.value("Status").toInt()));
	}
	if (record.contains("Message")){
		proposalPtr->SetMessage(record.value("Message").toString());
	}
	proposalPtr->SetCreatedAt(RecordDateTimeToString(record, "CreatedAt"));
	proposalPtr->SetUpdatedAt(RecordDateTimeToString(record, "UpdatedAt"));

	return proposalPtr;
}


CTenantRelationshipProposalDbDelegateComp::NewObjectQuery CTenantRelationshipProposalDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery result;

	const imtauth::ITenantRelationshipProposalInfo* proposalPtr = dynamic_cast<const imtauth::ITenantRelationshipProposalInfo*>(valuePtr);
	if (proposalPtr == nullptr){
		return result;
	}

	QString id = imtdb::EscapeSql(QString::fromUtf8(!proposedObjectId.isEmpty() ? proposedObjectId : proposalPtr->GetProposalId()));
	QString connectionId = imtdb::EscapeSql(QString::fromUtf8(proposalPtr->GetConnectionId()));
	QString existingRelId = NullableSqlText(QString::fromUtf8(proposalPtr->GetExistingRelationshipId()));
	int proposalType = static_cast<int>(proposalPtr->GetProposalType());
	QString initiatorTenantId = imtdb::EscapeSql(QString::fromUtf8(proposalPtr->GetInitiatorTenantId()));
	QString counterpartyTenantId = imtdb::EscapeSql(QString::fromUtf8(proposalPtr->GetCounterpartyTenantId()));
	int sourceRole = static_cast<int>(proposalPtr->GetProposedSourceRole());
	int targetRole = static_cast<int>(proposalPtr->GetProposedTargetRole());
	QString scope = NullableSqlText(proposalPtr->GetProposedScope());
	QString description = NullableSqlText(proposalPtr->GetProposedDescription());
	QString validFrom = NullableSqlText(proposalPtr->GetProposedValidFrom());
	QString validUntil = NullableSqlText(proposalPtr->GetProposedValidUntil());
	int status = static_cast<int>(proposalPtr->GetStatus());
	QString message = NullableSqlText(proposalPtr->GetMessage());
	QString createdAt = !proposalPtr->GetCreatedAt().isEmpty() ? imtdb::EscapeSql(proposalPtr->GetCreatedAt()) : imtdb::UtcNow();
	QString updatedAt = NullableSqlDateTime(proposalPtr->GetUpdatedAt());

	result.query = QString(
	"INSERT INTO \"%1\" (\"Id\", \"ConnectionId\", \"ExistingRelationshipId\", \"ProposalType\", "
	"\"InitiatorTenantId\", \"CounterpartyTenantId\", \"ProposedSourceRole\", \"ProposedTargetRole\", "
	"\"ProposedScope\", \"ProposedDescription\", \"ProposedValidFrom\", \"ProposedValidUntil\", "
	"\"Status\", \"Message\", \"CreatedAt\", \"UpdatedAt\") "
	"VALUES ('%2', '%3', %4, %5, '%6', '%7', %8, %9, %10, %11, %12, %13, %14, %15, '%16', %17);")
	.arg(*m_tableNameAttrPtr)
	.arg(id)
	.arg(connectionId)
	.arg(existingRelId)
	.arg(QString::number(proposalType))
	.arg(initiatorTenantId)
	.arg(counterpartyTenantId)
	.arg(QString::number(sourceRole))
	.arg(QString::number(targetRole))
	.arg(scope)
	.arg(description)
	.arg(validFrom)
	.arg(validUntil)
	.arg(QString::number(status))
	.arg(message)
	.arg(createdAt)
	.arg(updatedAt).toUtf8();

	return result;
}


QByteArray CTenantRelationshipProposalDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtauth::ITenantRelationshipProposalInfo* proposalPtr = dynamic_cast<const imtauth::ITenantRelationshipProposalInfo*>(&object);
	if (proposalPtr == nullptr){
		return QByteArray();
	}

	return QString(
	"UPDATE \"%1\" SET "
	"\"Status\"=%2, "
	"\"UpdatedAt\"=%3 "
	"WHERE \"Id\"='%4';")
	.arg(*m_tableNameAttrPtr)
	.arg(QString::number(static_cast<int>(proposalPtr->GetStatus())))
	.arg(NullableSqlDateTime(proposalPtr->GetUpdatedAt()))
	.arg(imtdb::EscapeSql(QString::fromUtf8(objectId))).toUtf8();
}


QByteArray CTenantRelationshipProposalDbDelegateComp::CreateDeleteObjectsQuery(
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


QByteArray CTenantRelationshipProposalDbDelegateComp::CreateDeleteObjectSetQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const iprm::IParamsSet* /*paramsPtr*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTenantRelationshipProposalDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTenantRelationshipProposalDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


bool CTenantRelationshipProposalDbDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("CreatedAt")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, record.value("CreatedAt").toDateTime());
	}
	return true;
}


idoc::MetaInfoPtr CTenantRelationshipProposalDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	idoc::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo);
	return metaInfoPtr;
}


bool CTenantRelationshipProposalDbDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


} // namespace imtauthdb
