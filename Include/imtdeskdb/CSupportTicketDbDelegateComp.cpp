// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdeskdb/CSupportTicketDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

// ACF includes
#include <imtdesk/ISupportTicket.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>
#include <iprm/TParamsPtr.h>
#include <imtauth/IUserGroupFilter.h>
#include <imtauth/IUserInfo.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <imtgql/IGqlContext.h>


namespace imtdeskdb
{


QString CSupportTicketDbDelegateComp::UtcNow() const
{
	return QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
}

QString CSupportTicketDbDelegateComp::EscapeSql(const QString& value) const
{
	QString escaped = value;
	escaped.replace('\'', "''");
	return escaped;
}

QString CSupportTicketDbDelegateComp::EscapeSqlLikePattern(const QString& value) const
{
	QString escaped = value;
	escaped.replace("\\", "\\\\");
	escaped.replace("%", "\\%");
	escaped.replace("_", "\\_");
	escaped.replace('\'', "''");
	return escaped;
}

bool CSupportTicketDbDelegateComp::IsSqliteDatabase() const
{
	return m_databaseEngineCompPtr.IsValid()
			&& m_databaseEngineCompPtr->GetDatabaseDriverId().compare(
				QByteArrayLiteral("QSQLITE"),
				Qt::CaseInsensitive) == 0;
}

QString CSupportTicketDbDelegateComp::CreateVisibilityCondition(
		const QByteArray& userId,
		const QByteArrayList& currentUserGroups) const
{
	if (userId.isEmpty()){
		return QString();
	}

	const QString escapedUserId = EscapeSql(QString::fromUtf8(userId));
	const QString escapedUserIdForLike = EscapeSqlLikePattern(QString::fromUtf8(userId));
	const QString assigneeLikePattern = QString("%%,%1,%%").arg(escapedUserIdForLike);
	QStringList escapedCurrentUserGroups;
	for (const QByteArray& groupId : currentUserGroups){
		const QString escapedGroupId = EscapeSql(QString::fromUtf8(groupId));
		if (!escapedGroupId.isEmpty()){
			escapedCurrentUserGroups << QString("'%1'").arg(escapedGroupId);
		}
	}

	QStringList visibilityConditions;
	visibilityConditions << QString("\"ReporterId\"='%1'").arg(escapedUserId);
	visibilityConditions << QString("(\"AssigneeIds\" IS NOT NULL AND (',' || \"AssigneeIds\" || ',') LIKE '%1' ESCAPE '\\')").arg(assigneeLikePattern);
	if (!escapedCurrentUserGroups.isEmpty() && m_userCollectionCompPtr.IsValid()){
		if (IsSqliteDatabase()){
			const QString groupsInClause = escapedCurrentUserGroups.join(", ");
			visibilityConditions << QString(
				"(EXISTS (SELECT 1 FROM \"Users\" AS \"ReporterUser\" "
				"JOIN json_each(\"ReporterUser\".\"Document\", '$.Groups') AS \"ReporterGroup\" "
				"WHERE \"ReporterUser\".\"State\"='Active' "
				"AND \"ReporterUser\".\"DocumentId\" = \"ReporterId\" "
				"AND \"ReporterGroup\".\"value\" IN (%1)) "
				"OR EXISTS (SELECT 1 FROM \"Users\" AS \"AssigneeUser\" "
				"JOIN json_each(\"AssigneeUser\".\"Document\", '$.Groups') AS \"AssigneeGroup\" "
				"WHERE \"AssigneeUser\".\"State\"='Active' "
				"AND \"AssigneeIds\" IS NOT NULL "
				"AND (',' || \"AssigneeIds\" || ',') LIKE ('%%,' || \"AssigneeUser\".\"DocumentId\" || ',%%') "
				"AND \"AssigneeGroup\".\"value\" IN (%1)))").arg(groupsInClause);
		}
		else{
			const QString groupsArray = QString("array[%1]").arg(escapedCurrentUserGroups.join(", "));
			visibilityConditions << QString(
				"(EXISTS (SELECT 1 FROM \"Users\" AS \"ReporterUser\" "
				"WHERE \"ReporterUser\".\"State\"='Active' "
				"AND \"ReporterUser\".\"DocumentId\"::text = \"ReporterId\" "
				"AND (\"ReporterUser\".\"Document\"->'Groups' ?| %1)) "
				"OR EXISTS (SELECT 1 FROM \"Users\" AS \"AssigneeUser\" "
				"WHERE \"AssigneeUser\".\"State\"='Active' "
				"AND \"AssigneeIds\" IS NOT NULL "
				"AND (',' || \"AssigneeIds\" || ',') LIKE ('%%,' || \"AssigneeUser\".\"DocumentId\"::text || ',%%') "
				"AND (\"AssigneeUser\".\"Document\"->'Groups' ?| %1)))").arg(groupsArray);
		}
	}

	return QString("(%1)").arg(visibilityConditions.join(" OR "));
}


QByteArray CSupportTicketDbDelegateComp::GetSelectionQuery(
		const QByteArray& objectId,
		int offset,
		int count,
		const iprm::IParamsSet* paramsPtr) const
{
	QByteArray baseQuery = BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);
	if (baseQuery.isEmpty()){
		return baseQuery;
	}

	iprm::TParamsPtr<imtauth::IUserGroupFilter> groupFilterParamPtr(paramsPtr, "GroupFilter");
	if (!groupFilterParamPtr.IsValid()){
		return baseQuery;
	}

	const QString visibilityCondition = CreateVisibilityCondition(
				groupFilterParamPtr->GetUserId(),
				groupFilterParamPtr->GetGroupIds());
	if (visibilityCondition.isEmpty()){
		return QString("SELECT * FROM (%1) AS \"FilteredTickets\" WHERE 1=0")
				.arg(QString::fromUtf8(baseQuery)).toUtf8();
	}

	const QString baseQueryStr = QString::fromUtf8(baseQuery);
	return QString("SELECT * FROM (%1) AS \"FilteredTickets\" WHERE %2")
			.arg(baseQueryStr, visibilityCondition).toUtf8();
}


istd::IChangeableUniquePtr CSupportTicketDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid() || !m_ticketFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtdesk::ISupportTicket> ticketPtr = m_ticketFactCompPtr.CreateInstance();
	if (!ticketPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		ticketPtr->SetId(record.value("Id").toByteArray());
	}
	if (record.contains("Title")){
		ticketPtr->SetTitle(record.value("Title").toString());
	}
	if (record.contains("Description")){
		ticketPtr->SetDescription(record.value("Description").toString());
	}
	if (record.contains("TicketType")){
		ticketPtr->SetTicketType(static_cast<imtdesk::ISupportTicket::TicketType>(record.value("TicketType").toInt()));
	}
	if (record.contains("Status")){
		ticketPtr->SetStatus(static_cast<imtdesk::ISupportTicket::TicketStatus>(record.value("Status").toInt()));
	}
	if (record.contains("StateReason")){
		ticketPtr->SetStateReason(static_cast<imtdesk::ISupportTicket::StateReason>(record.value("StateReason").toInt()));
	}
	if (record.contains("Priority")){
		ticketPtr->SetPriority(static_cast<imtdesk::ISupportTicket::TicketPriority>(record.value("Priority").toInt()));
	}
	if (record.contains("AssigneeIds")){
		const QString assigneesStr = record.value("AssigneeIds").toString();
		QByteArrayList assigneeIds;
		if (!assigneesStr.isEmpty()){
			for (const QString& s : assigneesStr.split(',')){
				assigneeIds.append(s.trimmed().toUtf8());
			}
		}
		ticketPtr->SetAssigneeIds(assigneeIds);
	}
	if (record.contains("ReporterId")){
		ticketPtr->SetReporterId(record.value("ReporterId").toByteArray());
	}
	if (record.contains("ConversationId")){
		ticketPtr->SetConversationId(record.value("ConversationId").toByteArray());
	}
	if (record.contains("MessageId")){
		ticketPtr->SetMessageId(record.value("MessageId").toByteArray());
	}
	if (record.contains("Tags")){
		const QString tagsStr = record.value("Tags").toString();
		QStringList tags;
		if (!tagsStr.isEmpty()){
			tags = tagsStr.split(',');
		}
		ticketPtr->SetTags(tags);
	}
	if (record.contains("LabelIds")){
		const QString labelsStr = record.value("LabelIds").toString();
		QByteArrayList labelIds;
		if (!labelsStr.isEmpty()){
			for (const QString& s : labelsStr.split(',')){
				labelIds.append(s.trimmed().toUtf8());
			}
		}
		ticketPtr->SetLabelIds(labelIds);
	}
	if (record.contains("Locked")){
		ticketPtr->SetLocked(record.value("Locked").toBool());
	}
	if (record.contains("LockReason")){
		ticketPtr->SetLockReason(record.value("LockReason").toString());
	}
	if (record.contains("Number")){
		ticketPtr->SetNumber(record.value("Number").toInt());
	}
	if (record.contains("CreatedAt")){
		QVariant val = record.value("CreatedAt");
		QDateTime dt = val.toDateTime();
		ticketPtr->SetCreatedAt(dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString());
	}
	if (record.contains("UpdatedAt")){
		QVariant val = record.value("UpdatedAt");
		QDateTime dt = val.toDateTime();
		ticketPtr->SetUpdatedAt(dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString());
	}
	if (record.contains("ClosedAt")){
		QVariant val = record.value("ClosedAt");
		QDateTime dt = val.toDateTime();
		ticketPtr->SetClosedAt(dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString());
	}
	if (record.contains("ResolvedAt")){
		QVariant val = record.value("ResolvedAt");
		QDateTime dt = val.toDateTime();
		ticketPtr->SetResolvedAt(dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString());
	}
	// EntityReferences are loaded from the TicketEntityReferences junction table
	if (m_databaseEngineCompPtr.IsValid() && record.contains("Id")){
		QByteArray ticketId = record.value("Id").toByteArray();
		if (!ticketId.isEmpty()){
			QString escaped = QString::fromUtf8(ticketId);
			escaped.replace('\'', "''");
			QByteArray refQuery = QString(
				"SELECT ter.\"EntityReferenceId\" "
				"FROM \"TicketEntityReferences\" ter "
				"WHERE ter.\"TicketId\"='%1' ORDER BY ter.\"CreatedAt\" ASC;")
				.arg(escaped).toUtf8();

			QSqlError sqlError;
			QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(refQuery, &sqlError);
			if (sqlError.type() == QSqlError::NoError){
				QByteArrayList refIds;
				while (sqlQuery.next()){
					refIds << sqlQuery.record().value("EntityReferenceId").toByteArray();
				}
				if (!refIds.isEmpty()){
					ticketPtr->SetEntityReferences(refIds);
				}
			}
		}
	}

	return ticketPtr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CSupportTicketDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& typeId,
		const QByteArray& proposedObjectId,
		const QString& objectName,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (typeId.isEmpty() || typeId != "Ticket"){
		return NewObjectQuery();
	}

	const imtdesk::ISupportTicket* ticketPtr = dynamic_cast<const imtdesk::ISupportTicket*>(valuePtr);
	if (ticketPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray ticketId = proposedObjectId;
	if (ticketId.isEmpty()){
		ticketId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QString title = ticketPtr->GetTitle();
	if (title.isEmpty()){
		title = objectName;
	}

	// Build assigneeIds as comma-separated string
	QStringList assigneeStrs;
	for (const QByteArray& aid : ticketPtr->GetAssigneeIds()){
		assigneeStrs.append(QString::fromUtf8(aid));
	}
	const QString assigneeIdsStr = assigneeStrs.join(',');

	QString reporterId = QString::fromUtf8(ticketPtr->GetReporterId());
	if (reporterId.isEmpty()){
		const imtgql::IGqlContext* contextPtr = imtgql::CGqlRequestContextManager::GetContext();
		if (contextPtr != nullptr){
			reporterId = QString::fromUtf8(contextPtr->GetUserId());
		}
	}
	const QString conversationId = QString::fromUtf8(ticketPtr->GetConversationId());
	const QString messageId = QString::fromUtf8(ticketPtr->GetMessageId());
	const QString resolvedAt = ticketPtr->GetResolvedAt();
	const QString closedAt = ticketPtr->GetClosedAt();

	const QString assigneesSql = assigneeIdsStr.isEmpty() ? "NULL" : QString("'%1'").arg(assigneeIdsStr);
	const QString convSql = conversationId.isEmpty() ? "NULL" : QString("'%1'").arg(conversationId);
	const QString msgSql = messageId.isEmpty() ? "NULL" : QString("'%1'").arg(messageId);
	const QString resolvedSql = resolvedAt.isEmpty() ? "NULL" : QString("'%1'").arg(resolvedAt);
	const QString closedSql = closedAt.isEmpty() ? "NULL" : QString("'%1'").arg(closedAt);

	const QString tagsStr = ticketPtr->GetTags().join(',');
	const QString tagsSql = tagsStr.isEmpty() ? "NULL" : QString("'%1'").arg(tagsStr);

	// Build labelIds as comma-separated string
	QStringList labelStrs;
	for (const QByteArray& lid : ticketPtr->GetLabelIds()){
		labelStrs.append(EscapeSql(QString::fromUtf8(lid)));
	}
	const QString labelIdsStr = labelStrs.join(',');
	const QString labelsSql = labelIdsStr.isEmpty() ? "NULL" : QString("'%1'").arg(labelIdsStr);

	const QString lockReasonSql = ticketPtr->GetLockReason().isEmpty()
			? "NULL"
			: QString("'%1'").arg(EscapeSql(ticketPtr->GetLockReason()));

	const QString nowUtc = UtcNow();

	QString combinedQuery;

	// INSERT into Tickets table (without EntityReferences column — now in junction table)
	combinedQuery += QString(
		"INSERT INTO \"Tickets\" "
		"(\"Id\", \"Title\", \"Description\", \"TicketType\", \"Status\", \"StateReason\", \"Priority\", "
		"\"AssigneeIds\", \"ReporterId\", \"ConversationId\", \"MessageId\", "
		"\"Tags\", \"LabelIds\", \"Locked\", \"LockReason\", "
		"\"ResolvedAt\", \"ClosedAt\", \"CreatedAt\", \"UpdatedAt\") "
		"VALUES('%1', '%2', '%3', %4, %5, %6, %7, %8, '%9', %10, %11, %12, %13, %14, %15, %16, %17, '%18', '%19');")
		.arg(QString::fromUtf8(ticketId))
		.arg(title)
		.arg(ticketPtr->GetDescription())
		.arg(ticketPtr->GetTicketType())
		.arg(ticketPtr->GetStatus())
		.arg(ticketPtr->GetStateReason())
		.arg(ticketPtr->GetPriority())
		.arg(assigneesSql)
		.arg(reporterId)
		.arg(convSql)
		.arg(msgSql)
		.arg(tagsSql)
		.arg(labelsSql)
		.arg(ticketPtr->IsLocked() ? "TRUE" : "FALSE")
		.arg(lockReasonSql)
		.arg(resolvedSql)
		.arg(closedSql)
		.arg(nowUtc)
		.arg(nowUtc);

	// INSERT into TicketEntityReferences junction table using entity reference IDs
	const QByteArrayList entityRefIds = ticketPtr->GetEntityReferences();
	for (const QByteArray& refId : entityRefIds){
		QString escapedRefId = QString::fromUtf8(refId);
		escapedRefId.replace('\'', "''");
		combinedQuery += QString(
			"\nINSERT INTO \"TicketEntityReferences\" "
			"(\"TicketId\", \"EntityReferenceId\", \"CreatedAt\") "
			"VALUES('%1', '%2', '%3');")
			.arg(QString::fromUtf8(ticketId))
			.arg(escapedRefId)
			.arg(nowUtc);
	}

	NewObjectQuery retVal;
	retVal.query = combinedQuery.toUtf8();

	return retVal;
}


QByteArray CSupportTicketDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtdesk::ISupportTicket* ticketPtr = dynamic_cast<const imtdesk::ISupportTicket*>(&object);
	if (ticketPtr == nullptr){
		return QByteArray();
	}

	// Build assigneeIds as comma-separated string
	QStringList assigneeStrs;
	for (const QByteArray& aid : ticketPtr->GetAssigneeIds()){
		assigneeStrs.append(QString::fromUtf8(aid));
	}
	const QString assigneeIdsStr = assigneeStrs.join(',');

	const QString conversationId = QString::fromUtf8(ticketPtr->GetConversationId());
	const QString messageId = QString::fromUtf8(ticketPtr->GetMessageId());
	const QString resolvedAt = ticketPtr->GetResolvedAt();
	const QString closedAt = ticketPtr->GetClosedAt();
	const QString assigneesSql = assigneeIdsStr.isEmpty() ? "NULL" : QString("'%1'").arg(assigneeIdsStr);
	const QString convSql = conversationId.isEmpty() ? "NULL" : QString("'%1'").arg(conversationId);
	const QString msgSql = messageId.isEmpty() ? "NULL" : QString("'%1'").arg(messageId);
	const QString resolvedSql = resolvedAt.isEmpty() ? "NULL" : QString("'%1'").arg(resolvedAt);
	const QString closedSql = closedAt.isEmpty() ? "NULL" : QString("'%1'").arg(closedAt);
	const QString tagsStr = ticketPtr->GetTags().join(',');
	const QString tagsSql = tagsStr.isEmpty() ? "NULL" : QString("'%1'").arg(tagsStr);

	// Build labelIds as comma-separated string
	QStringList labelStrs;
	for (const QByteArray& lid : ticketPtr->GetLabelIds()){
		labelStrs.append(EscapeSql(QString::fromUtf8(lid)));
	}
	const QString labelIdsStr = labelStrs.join(',');
	const QString labelsSql = labelIdsStr.isEmpty() ? "NULL" : QString("'%1'").arg(labelIdsStr);

	const QString lockReasonSql = ticketPtr->GetLockReason().isEmpty()
			? "NULL"
			: QString("'%1'").arg(EscapeSql(ticketPtr->GetLockReason()));

	const QString nowUtc = UtcNow();

	QString combinedQuery;

	combinedQuery += QString(
		"UPDATE \"Tickets\" SET "
		"\"Title\"='%1', "
		"\"Description\"='%2', "
		"\"TicketType\"=%3, "
		"\"Status\"=%4, "
		"\"StateReason\"=%5, "
		"\"Priority\"=%6, "
		"\"AssigneeIds\"=%7, "
		"\"ConversationId\"=%8, "
		"\"MessageId\"=%9, "
		"\"Tags\"=%10, "
		"\"LabelIds\"=%11, "
		"\"Locked\"=%12, "
		"\"LockReason\"=%13, "
		"\"ResolvedAt\"=%14, "
		"\"ClosedAt\"=%15, "
		"\"UpdatedAt\"='%16' "
		"WHERE \"Id\"='%17';")
		.arg(ticketPtr->GetTitle())
		.arg(ticketPtr->GetDescription())
		.arg(ticketPtr->GetTicketType())
		.arg(ticketPtr->GetStatus())
		.arg(ticketPtr->GetStateReason())
		.arg(ticketPtr->GetPriority())
		.arg(assigneesSql)
		.arg(convSql)
		.arg(msgSql)
		.arg(tagsSql)
		.arg(labelsSql)
		.arg(ticketPtr->IsLocked() ? "TRUE" : "FALSE")
		.arg(lockReasonSql)
		.arg(resolvedSql)
		.arg(closedSql)
		.arg(nowUtc)
		.arg(QString::fromUtf8(objectId));

	// Delete old junction rows and re-insert entity references using IDs
	combinedQuery += QString(
		"\nDELETE FROM \"TicketEntityReferences\" WHERE \"TicketId\"='%1';")
		.arg(QString::fromUtf8(objectId));

	const QByteArrayList entityRefIds = ticketPtr->GetEntityReferences();
	for (const QByteArray& refId : entityRefIds){
		QString escapedRefId = QString::fromUtf8(refId);
		escapedRefId.replace('\'', "''");
		combinedQuery += QString(
			"\nINSERT INTO \"TicketEntityReferences\" "
			"(\"TicketId\", \"EntityReferenceId\", \"CreatedAt\") "
			"VALUES('%1', '%2', '%3');")
			.arg(QString::fromUtf8(objectId))
			.arg(escapedRefId)
			.arg(nowUtc);
	}

	return combinedQuery.toUtf8();
}


QByteArray CSupportTicketDbDelegateComp::CreateDeleteObjectsQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArrayList& objectIds,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectIds.isEmpty()){
		return QByteArray();
	}

	QString idsStr;
	for (int i = 0; i < objectIds.size(); ++i){
		if (i > 0) idsStr += ", ";
		idsStr += QString("'%1'").arg(QString::fromUtf8(objectIds[i]));
	}

	// Delete junction records first, then the tickets
	return QString("DELETE FROM \"TicketEntityReferences\" WHERE \"TicketId\" IN (%1);\n"
		"DELETE FROM \"Tickets\" WHERE \"Id\" IN (%1);")
		.arg(idsStr)
		.toUtf8();
}


QByteArray CSupportTicketDbDelegateComp::CreateDeleteObjectSetQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const iprm::IParamsSet* /*paramsPtr*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CSupportTicketDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const QString& newObjectName,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectId.isEmpty() || newObjectName.isEmpty()){
		return QByteArray();
	}

	return QString("UPDATE \"Tickets\" SET \"Title\"='%1', \"UpdatedAt\"='%2' WHERE \"Id\"='%3';")
		.arg(newObjectName)
		.arg(UtcNow())
		.arg(QString::fromUtf8(objectId))
		.toUtf8();
}


QByteArray CSupportTicketDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const QString& description,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectId.isEmpty()){
		return QByteArray();
	}

	return QString("UPDATE \"Tickets\" SET \"Description\"='%1', \"UpdatedAt\"='%2' WHERE \"Id\"='%3';")
		.arg(description)
		.arg(UtcNow())
		.arg(QString::fromUtf8(objectId))
		.toUtf8();
}


bool CSupportTicketDbDelegateComp::SetCollectionItemMetaInfoFromRecord(
			const QSqlRecord& record,
			idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("Title")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_TITLE, record.value("Title"));
	}

	if (record.contains("Description")){
		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION, record.value("Description"));
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


void CSupportTicketDbDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_databaseEngineCompPtr.IsValid()){
		return;
	}

	// Create Tickets table
	const QString tableName = GetTableName();
	if (!TableExists(tableName)){
		QFile scriptFile(imtdb::GetSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("CreateTicketsTable.sql")));
		if (!scriptFile.open(QFile::ReadOnly)){
			SendErrorMessage(0, QString("Tickets table creation script '%1' could not be loaded").arg(scriptFile.fileName()));
			return;
		}

		QByteArray query = scriptFile.readAll();
		scriptFile.close();
		query.replace("${TableScheme}", "public");

		QSqlError sqlError;
		m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);

		if (sqlError.type() != QSqlError::NoError){
			qCritical() << __FILE__ << __LINE__
						<< "\n\t| Tickets table could not be created"
						<< "\n\t| Error:" << sqlError
						<< "\n\t| Query:" << query;
			SendErrorMessage(0, QString("Tickets table could not be created: %1").arg(sqlError.text()));
		}
	}

	// Create TicketEntityReferences junction table
	if (!TableExists("TicketEntityReferences")){
		QFile junctionScriptFile(imtdb::GetSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("CreateTicketEntityReferencesTable.sql")));
		if (!junctionScriptFile.open(QFile::ReadOnly)){
			SendErrorMessage(0, QString("TicketEntityReferences table creation script '%1' could not be loaded").arg(junctionScriptFile.fileName()));
			return;
		}

		QByteArray junctionQuery = junctionScriptFile.readAll();
		junctionScriptFile.close();
		junctionQuery.replace("${TableScheme}", "public");

		QSqlError junctionError;
		m_databaseEngineCompPtr->ExecSqlQuery(junctionQuery, &junctionError);

		if (junctionError.type() != QSqlError::NoError){
			qCritical() << __FILE__ << __LINE__
						<< "\n\t| TicketEntityReferences table could not be created"
						<< "\n\t| Error:" << junctionError
						<< "\n\t| Query:" << junctionQuery;
			SendErrorMessage(0, QString("TicketEntityReferences table could not be created: %1").arg(junctionError.text()));
		}
	}
}


idoc::MetaInfoPtr CSupportTicketDbDelegateComp::CreateObjectMetaInfo(const QByteArray& typeId) const
{
	return BaseClass::CreateObjectMetaInfo(typeId);
}


bool CSupportTicketDbDelegateComp::SetObjectMetaInfoFromRecord(
		const QSqlRecord& record,
		idoc::IDocumentMetaInfo& metaInfo) const
{
	if (!BaseClass::SetObjectMetaInfoFromRecord(record, metaInfo)){
		return false;
	}
	Q_UNUSED(record);
	Q_UNUSED(metaInfo);
	return true;
}


} // namespace imtdeskdb
