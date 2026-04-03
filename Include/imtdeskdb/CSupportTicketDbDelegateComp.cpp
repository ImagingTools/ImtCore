// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdeskdb/CSupportTicketDbDelegateComp.h>

// Qt includes
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtdesk/ISupportTicket.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>


namespace imtdeskdb
{


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
	if (record.contains("Priority")){
		ticketPtr->SetPriority(static_cast<imtdesk::ISupportTicket::TicketPriority>(record.value("Priority").toInt()));
	}
	if (record.contains("AssigneeId")){
		ticketPtr->SetAssigneeId(record.value("AssigneeId").toByteArray());
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
	if (record.contains("Environment")){
		ticketPtr->SetEnvironment(static_cast<imtdesk::ISupportTicket::Environment>(record.value("Environment").toInt()));
	}
	if (record.contains("Tags")){
		const QString tagsStr = record.value("Tags").toString();
		QStringList tags;
		if (!tagsStr.isEmpty()){
			tags = tagsStr.split(',');
		}
		ticketPtr->SetTags(tags);
	}
	if (record.contains("CreatedAt")){
		ticketPtr->SetCreatedAt(record.value("CreatedAt").toString());
	}
	if (record.contains("UpdatedAt")){
		ticketPtr->SetUpdatedAt(record.value("UpdatedAt").toString());
	}
	if (record.contains("ResolvedAt")){
		ticketPtr->SetResolvedAt(record.value("ResolvedAt").toString());
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

	const QString assigneeId = QString::fromUtf8(ticketPtr->GetAssigneeId());
	const QString reporterId = QString::fromUtf8(ticketPtr->GetReporterId());
	const QString conversationId = QString::fromUtf8(ticketPtr->GetConversationId());
	const QString messageId = QString::fromUtf8(ticketPtr->GetMessageId());
	const QString resolvedAt = ticketPtr->GetResolvedAt();

	const QString assigneeSql = assigneeId.isEmpty() ? "NULL" : QString("'%1'").arg(assigneeId);
	const QString convSql = conversationId.isEmpty() ? "NULL" : QString("'%1'").arg(conversationId);
	const QString msgSql = messageId.isEmpty() ? "NULL" : QString("'%1'").arg(messageId);
	const QString resolvedSql = resolvedAt.isEmpty() ? "NULL" : QString("'%1'").arg(resolvedAt);

	const QString tagsStr = ticketPtr->GetTags().join(',');
	const QString tagsSql = tagsStr.isEmpty() ? "NULL" : QString("'%1'").arg(tagsStr);

	NewObjectQuery retVal;
	retVal.query = QString(
		"INSERT INTO \"Tickets\" "
		"(\"Id\", \"Title\", \"Description\", \"TicketType\", \"Status\", \"Priority\", "
		"\"AssigneeId\", \"ReporterId\", \"ConversationId\", \"MessageId\", \"Environment\", \"Tags\", \"ResolvedAt\") "
		"VALUES('%1', '%2', '%3', %4, %5, %6, %7, '%8', %9, %10, %11, %12, %13);")
		.arg(QString::fromUtf8(ticketId))
		.arg(title)
		.arg(ticketPtr->GetDescription())
		.arg(ticketPtr->GetTicketType())
		.arg(ticketPtr->GetStatus())
		.arg(ticketPtr->GetPriority())
		.arg(assigneeSql)
		.arg(reporterId)
		.arg(convSql)
		.arg(msgSql)
		.arg(ticketPtr->GetEnvironment())
		.arg(tagsSql)
		.arg(resolvedSql)
		.toUtf8();

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

	const QString assigneeId = QString::fromUtf8(ticketPtr->GetAssigneeId());
	const QString resolvedAt = ticketPtr->GetResolvedAt();
	const QString assigneeSql = assigneeId.isEmpty() ? "NULL" : QString("'%1'").arg(assigneeId);
	const QString resolvedSql = resolvedAt.isEmpty() ? "NULL" : QString("'%1'").arg(resolvedAt);
	const QString tagsStr = ticketPtr->GetTags().join(',');
	const QString tagsSql = tagsStr.isEmpty() ? "NULL" : QString("'%1'").arg(tagsStr);

	return QString(
		"UPDATE \"Tickets\" SET "
		"\"Title\"='%1', "
		"\"Description\"='%2', "
		"\"TicketType\"=%3, "
		"\"Status\"=%4, "
		"\"Priority\"=%5, "
		"\"AssigneeId\"=%6, "
		"\"Environment\"=%7, "
		"\"Tags\"=%8, "
		"\"ResolvedAt\"=%9, "
		"\"UpdatedAt\"=NOW() "
		"WHERE \"Id\"='%10';")
		.arg(ticketPtr->GetTitle())
		.arg(ticketPtr->GetDescription())
		.arg(ticketPtr->GetTicketType())
		.arg(ticketPtr->GetStatus())
		.arg(ticketPtr->GetPriority())
		.arg(assigneeSql)
		.arg(ticketPtr->GetEnvironment())
		.arg(tagsSql)
		.arg(resolvedSql)
		.arg(QString::fromUtf8(objectId))
		.toUtf8();
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

	return QString("UPDATE \"Tickets\" SET \"IsActive\"=FALSE WHERE \"Id\" IN (%1);")
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

	return QString("UPDATE \"Tickets\" SET \"Title\"='%1', \"UpdatedAt\"=NOW() WHERE \"Id\"='%2';")
		.arg(newObjectName)
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

	return QString("UPDATE \"Tickets\" SET \"Description\"='%1', \"UpdatedAt\"=NOW() WHERE \"Id\"='%2';")
		.arg(description)
		.arg(QString::fromUtf8(objectId))
		.toUtf8();
}


void CSupportTicketDbDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_databaseEngineCompPtr.IsValid()){
		return;
	}

	const QString tableName = GetTableName();
	if (TableExists(tableName)){
		return;
	}

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
