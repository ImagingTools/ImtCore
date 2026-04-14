// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchatdb/CMessageDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtchat/IChatMessage.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>


namespace imtchatdb
{


namespace
{

// Helper: return current UTC timestamp in ISO 8601 with milliseconds
QString utcNow()
{
	return QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
}

} // anonymous namespace


istd::IChangeableUniquePtr CMessageDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid() || !m_messageFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtchat::IChatMessage> msgPtr = m_messageFactCompPtr.CreateInstance();
	if (!msgPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		msgPtr->SetId(record.value("Id").toByteArray());
	}
	if (record.contains("ConversationId")){
		msgPtr->SetConversationId(record.value("ConversationId").toByteArray());
	}
	if (record.contains("SenderId")){
		msgPtr->SetSenderId(record.value("SenderId").toByteArray());
	}
	if (record.contains("Content")){
		msgPtr->SetContent(record.value("Content").toString());
	}
	if (record.contains("EntityReferences")){
		const QString entityRefStr = record.value("EntityReferences").toString();
		QByteArrayList entityRefIds;
		if (!entityRefStr.isEmpty()){
			for (const QString& part : entityRefStr.split(',')){
				const QString trimmed = part.trimmed();
				if (!trimmed.isEmpty()){
					entityRefIds.append(trimmed.toUtf8());
				}
			}
		}
		msgPtr->SetEntityReferences(entityRefIds);
	}
	// AttachmentIds are loaded from the MessageAttachments junction table
	if (m_databaseEngineCompPtr.IsValid() && record.contains("Id")){
		QByteArray messageId = record.value("Id").toByteArray();
		if (!messageId.isEmpty()){
			QString escaped = QString::fromUtf8(messageId);
			escaped.replace('\'', "''");
			QByteArray attachQuery = QString(
				"SELECT \"AttachmentId\" FROM \"MessageAttachments\" WHERE \"MessageId\"='%1' ORDER BY \"CreatedAt\" ASC;")
				.arg(escaped).toUtf8();

			QSqlError sqlError;
			QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(attachQuery, &sqlError);
			if (sqlError.type() == QSqlError::NoError){
				QByteArrayList attachIds;
				while (sqlQuery.next()){
					QSqlRecord r = sqlQuery.record();
					if (r.contains("AttachmentId")){
						QByteArray aid = r.value("AttachmentId").toByteArray();
						if (!aid.isEmpty()){
							attachIds.append(aid);
						}
					}
				}
				msgPtr->SetAttachmentIds(attachIds);
			}
		}
	}
	if (record.contains("Status")){
		msgPtr->SetStatus(static_cast<imtchat::IChatMessage::MessageStatus>(record.value("Status").toInt()));
	}
	if (record.contains("Reactions")){
		const QString reactionsStr = record.value("Reactions").toString();
		QStringList reactions;
		if (!reactionsStr.isEmpty()){
			reactions = reactionsStr.split(',');
		}
		msgPtr->SetReactions(reactions);
	}
	if (record.contains("CreatedAt")){
		QVariant val = record.value("CreatedAt");
		QDateTime dt = val.toDateTime();
		msgPtr->SetCreatedAt(dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString());
	}
	if (record.contains("UpdatedAt")){
		QVariant val = record.value("UpdatedAt");
		QDateTime dt = val.toDateTime();
		msgPtr->SetUpdatedAt(dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString());
	}

	return msgPtr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CMessageDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& typeId,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (typeId.isEmpty() || typeId != "ChatMessage"){
		return NewObjectQuery();
	}

	const imtchat::IChatMessage* msgPtr = dynamic_cast<const imtchat::IChatMessage*>(valuePtr);
	if (msgPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray msgId = proposedObjectId;
	if (msgId.isEmpty()){
		msgId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	QString entityRefsStr;
	const QByteArrayList entityRefIds = msgPtr->GetEntityReferences();
	for (int i = 0; i < entityRefIds.size(); ++i){
		if (i > 0) entityRefsStr += ",";
		entityRefsStr += QString::fromUtf8(entityRefIds[i]);
	}
	const QString entityRefsSql = entityRefsStr.isEmpty() ? "NULL" : QString("'%1'").arg(entityRefsStr);

	const QString reactionsStr = msgPtr->GetReactions().join(',');
	const QString reactionsSql = reactionsStr.isEmpty() ? "NULL" : QString("'%1'").arg(reactionsStr);

	const QString nowUtc = utcNow();

	QString combinedQuery;

	// INSERT into Messages table (without AttachmentIds column)
	combinedQuery += QString(
		"INSERT INTO \"Messages\" "
		"(\"Id\", \"ConversationId\", \"SenderId\", \"Content\", \"EntityReferences\", \"Reactions\", \"Status\", \"CreatedAt\", \"UpdatedAt\") "
		"VALUES('%1', '%2', '%3', '%4', %5, %6, %7, '%8', '%9');")
		.arg(QString::fromUtf8(msgId))
		.arg(QString::fromUtf8(msgPtr->GetConversationId()))
		.arg(QString::fromUtf8(msgPtr->GetSenderId()))
		.arg(msgPtr->GetContent())
		.arg(entityRefsSql)
		.arg(reactionsSql)
		.arg(msgPtr->GetStatus())
		.arg(nowUtc)
		.arg(nowUtc);

	// INSERT into MessageAttachments junction table for each attachment
	const QByteArrayList attachIds = msgPtr->GetAttachmentIds();
	for (const QByteArray& attachId : attachIds){
		if (attachId.isEmpty()){
			continue;
		}
		QString escapedAttachId = QString::fromUtf8(attachId);
		escapedAttachId.replace('\'', "''");
		combinedQuery += QString(
			"\nINSERT INTO \"MessageAttachments\" "
			"(\"MessageId\", \"AttachmentId\", \"CreatedAt\") "
			"VALUES('%1', '%2', '%3');")
			.arg(QString::fromUtf8(msgId))
			.arg(escapedAttachId)
			.arg(nowUtc);
	}

	NewObjectQuery retVal;
	retVal.query = combinedQuery.toUtf8();

	return retVal;
}


QByteArray CMessageDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtchat::IChatMessage* msgPtr = dynamic_cast<const imtchat::IChatMessage*>(&object);
	if (msgPtr == nullptr){
		return QByteArray();
	}

	QString entityRefsStr;
	const QByteArrayList entityRefIds = msgPtr->GetEntityReferences();
	for (int i = 0; i < entityRefIds.size(); ++i){
		if (i > 0) entityRefsStr += ",";
		entityRefsStr += QString::fromUtf8(entityRefIds[i]);
	}
	const QString entityRefsSql = entityRefsStr.isEmpty() ? "NULL" : QString("'%1'").arg(entityRefsStr);

	const QString reactionsStr = msgPtr->GetReactions().join(',');
	const QString reactionsSql = reactionsStr.isEmpty() ? "NULL" : QString("'%1'").arg(reactionsStr);

	return QString(
		"UPDATE \"Messages\" SET "
		"\"Content\"='%1', "
		"\"EntityReferences\"=%2, "
		"\"Reactions\"=%3, "
		"\"Status\"=%4, "
		"\"UpdatedAt\"='%5' "
		"WHERE \"Id\"='%6';")
		.arg(msgPtr->GetContent())
		.arg(entityRefsSql)
		.arg(reactionsSql)
		.arg(msgPtr->GetStatus())
		.arg(utcNow())
		.arg(QString::fromUtf8(objectId))
		.toUtf8();
}


QByteArray CMessageDbDelegateComp::CreateDeleteObjectsQuery(
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

	// Delete junction records first, then the messages
	return QString("DELETE FROM \"MessageAttachments\" WHERE \"MessageId\" IN (%1);\n"
		"DELETE FROM \"Messages\" WHERE \"Id\" IN (%1);")
		.arg(idsStr)
		.toUtf8();
}


QByteArray CMessageDbDelegateComp::CreateDeleteObjectSetQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const iprm::IParamsSet* /*paramsPtr*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CMessageDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CMessageDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


void CMessageDbDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_databaseEngineCompPtr.IsValid()){
		return;
	}

	// Create Messages table
	const QString tableName = GetTableName();
	if (!TableExists(tableName)){
		QFile scriptFile(imtdb::GetSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("CreateMessagesTable.sql")));
		if (!scriptFile.open(QFile::ReadOnly)){
			SendErrorMessage(0, QString("Messages table creation script '%1' could not be loaded").arg(scriptFile.fileName()));
			return;
		}

		QByteArray query = scriptFile.readAll();
		scriptFile.close();
		query.replace("${TableScheme}", "public");

		QSqlError sqlError;
		m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);

		if (sqlError.type() != QSqlError::NoError){
			qCritical() << __FILE__ << __LINE__
						<< "\n\t| Messages table could not be created"
						<< "\n\t| Error:" << sqlError
						<< "\n\t| Query:" << query;
			SendErrorMessage(0, QString("Messages table could not be created: %1").arg(sqlError.text()));
		}
	}

	// Create MessageAttachments junction table
	if (!TableExists("MessageAttachments")){
		QFile junctionScriptFile(imtdb::GetSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("CreateMessageAttachmentsTable.sql")));
		if (!junctionScriptFile.open(QFile::ReadOnly)){
			SendErrorMessage(0, QString("MessageAttachments table creation script '%1' could not be loaded").arg(junctionScriptFile.fileName()));
			return;
		}

		QByteArray junctionQuery = junctionScriptFile.readAll();
		junctionScriptFile.close();
		junctionQuery.replace("${TableScheme}", "public");

		QSqlError junctionError;
		m_databaseEngineCompPtr->ExecSqlQuery(junctionQuery, &junctionError);

		if (junctionError.type() != QSqlError::NoError){
			qCritical() << __FILE__ << __LINE__
						<< "\n\t| MessageAttachments table could not be created"
						<< "\n\t| Error:" << junctionError
						<< "\n\t| Query:" << junctionQuery;
			SendErrorMessage(0, QString("MessageAttachments table could not be created: %1").arg(junctionError.text()));
		}
	}
}


QString CMessageDbDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

	if (paramIds.contains("ConversationId")){
		iprm::TParamsPtr<iprm::IIdParam> idParamPtr(&filterParams, "ConversationId");
		if (idParamPtr.IsValid()){
			QByteArray conversationId = idParamPtr->GetId();
			if (!conversationId.isEmpty()){
				QString escaped = QString::fromUtf8(conversationId);
				escaped.replace('\'', "''");
				return QString("\"ConversationId\"='%1'").arg(escaped);
			}
		}
	}

	return QString();
}


idoc::MetaInfoPtr CMessageDbDelegateComp::CreateObjectMetaInfo(const QByteArray& typeId) const
{
	return BaseClass::CreateObjectMetaInfo(typeId);
}


bool CMessageDbDelegateComp::SetObjectMetaInfoFromRecord(
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


} // namespace imtchatdb
