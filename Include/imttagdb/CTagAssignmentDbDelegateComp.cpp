// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttagdb/CTagAssignmentDbDelegateComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>
#include <imtdb/imtdb.h>


namespace imttagdb
{


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeableUniquePtr CTagAssignmentDbDelegateComp::CreateObjectFromRecord(
			const QSqlRecord& record,
			const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_assignmentFactCompPtr.IsValid()){
		return nullptr;
	}

	imttag::ITagAssignmentUniquePtr assignmentPtr = m_assignmentFactCompPtr.CreateInstance();
	if (!assignmentPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("TagId")){
		assignmentPtr->SetTagId(imtdb::VariantToByteArray(record.value("TagId")));
	}

	if (record.contains("EntityType")){
		assignmentPtr->SetEntityType(imtdb::VariantToByteArray(record.value("EntityType")));
	}

	if (record.contains("EntityId")){
		assignmentPtr->SetEntityId(imtdb::VariantToByteArray(record.value("EntityId")));
	}

	if (record.contains("CreatedAt")){
		QDateTime createdAt = record.value("CreatedAt").toDateTime();
		createdAt.setTimeSpec(Qt::UTC);
		assignmentPtr->SetCreatedAt(createdAt);
	}

	if (record.contains("CreatedByUserId")){
		assignmentPtr->SetCreatedByUserId(imtdb::VariantToByteArray(record.value("CreatedByUserId")));
	}

	return assignmentPtr;
}


CTagAssignmentDbDelegateComp::NewObjectQuery CTagAssignmentDbDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& proposedObjectId,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	NewObjectQuery result;

	const imttag::ITagAssignment* assignmentPtr = dynamic_cast<const imttag::ITagAssignment*>(valuePtr);
	if (assignmentPtr == nullptr){
		return result;
	}

	const QString id = proposedObjectId.isEmpty()
				? QUuid::createUuid().toString(QUuid::WithoutBraces)
				: imtdb::EscapeSql(QString::fromUtf8(proposedObjectId));

	const QDateTime createdAt = assignmentPtr->GetCreatedAt();
	const QString createdAtLiteral = createdAt.isValid() ? createdAt.toUTC().toString(Qt::ISODateWithMs) : imtdb::UtcNow();

	const QByteArray createdByUserId = assignmentPtr->GetCreatedByUserId();
	const QString createdByLiteral = createdByUserId.isEmpty()
				? imtdb::NULL_DATA_LITERAL
				: QStringLiteral("'%1'").arg(imtdb::EscapeSql(QString::fromUtf8(createdByUserId)));

	const bool isSqlite = m_databaseEngineCompPtr.IsValid() && (m_databaseEngineCompPtr->GetDatabaseDriverId() == "QSQLITE");
	const QString insertClause = isSqlite ? QStringLiteral("INSERT OR IGNORE INTO") : QStringLiteral("INSERT INTO");
	const QString conflictClause = isSqlite ? QString() : QStringLiteral(R"( ON CONFLICT ("TagId", "EntityType", "EntityId") DO NOTHING)");

	result.query = QStringLiteral(R"(%1 "%2" ("Id", "TagId", "EntityType", "EntityId", "CreatedAt", "CreatedByUserId") VALUES ('%3', '%4', '%5', '%6', '%7', %8)%9;)")
				.arg(
							/*1*/ insertClause,
							/*2*/ *m_tableNameAttrPtr,
							/*3*/ id,
							/*4*/ imtdb::EscapeSql(QString::fromUtf8(assignmentPtr->GetTagId())),
							/*5*/ imtdb::EscapeSql(QString::fromUtf8(assignmentPtr->GetEntityType())),
							/*6*/ imtdb::EscapeSql(QString::fromUtf8(assignmentPtr->GetEntityId())),
							/*7*/ createdAtLiteral,
							/*8*/ createdByLiteral,
							/*9*/ conflictClause).toUtf8();
	result.objectName = id;

	return result;
}


QByteArray CTagAssignmentDbDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const istd::IChangeable& /*object*/,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	// Assignments are immutable.
	return QByteArray();
}


QByteArray CTagAssignmentDbDelegateComp::CreateDeleteObjectsQuery(
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

	return QStringLiteral(R"(DELETE FROM "%1" WHERE "Id" IN (%2);)").arg(*m_tableNameAttrPtr, escapedIds.join(", ")).toUtf8();
}


QByteArray CTagAssignmentDbDelegateComp::CreateDeleteObjectSetQuery(
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

	return QStringLiteral(R"(DELETE FROM "%1" WHERE %2;)").arg(*m_tableNameAttrPtr, filterQuery).toUtf8();
}


QByteArray CTagAssignmentDbDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*newObjectName*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CTagAssignmentDbDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


bool CTagAssignmentDbDelegateComp::SetCollectionItemMetaInfoFromRecord(
			const QSqlRecord& record,
			idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("CreatedAt")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, record.value("CreatedAt").toDateTime());
	}

	return true;
}


// protected methods

// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

idoc::MetaInfoPtr CTagAssignmentDbDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return idoc::MetaInfoPtr(new idoc::CStandardDocumentMetaInfo);
}


bool CTagAssignmentDbDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	return SetCollectionItemMetaInfoFromRecord(record, metaInfo);
}


} // namespace imttagdb


