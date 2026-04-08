// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchatdb/CMessageDbDelegateComp.h>

// Qt includes
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtchat/IChatMessage.h>
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>


namespace imtchatdb
{


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
	if (record.contains("AttachmentIds")){
		const QString attachStr = record.value("AttachmentIds").toString();
		QByteArrayList attachIds;
		if (!attachStr.isEmpty()){
			for (const QString& part : attachStr.split(',')){
				const QString trimmed = part.trimmed();
				if (!trimmed.isEmpty()){
					attachIds.append(trimmed.toUtf8());
				}
			}
		}
		msgPtr->SetAttachmentIds(attachIds);
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
		msgPtr->SetCreatedAt(record.value("CreatedAt").toString());
	}
	if (record.contains("UpdatedAt")){
		msgPtr->SetUpdatedAt(record.value("UpdatedAt").toString());
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
	if (typeId.isEmpty() || typeId != "Message"){
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

	QString attachStr;
	const QByteArrayList attachIds = msgPtr->GetAttachmentIds();
	for (int i = 0; i < attachIds.size(); ++i){
		if (i > 0) attachStr += ",";
		attachStr += QString::fromUtf8(attachIds[i]);
	}
	const QString attachSql = attachStr.isEmpty() ? "NULL" : QString("'%1'").arg(attachStr);

	const QString reactionsStr = msgPtr->GetReactions().join(',');
	const QString reactionsSql = reactionsStr.isEmpty() ? "NULL" : QString("'%1'").arg(reactionsStr);

	NewObjectQuery retVal;
	retVal.query = QString(
		"INSERT INTO \"Messages\" "
		"(\"Id\", \"ConversationId\", \"SenderId\", \"Content\", \"EntityReferences\", \"AttachmentIds\", \"Reactions\", \"Status\") "
		"VALUES('%1', '%2', '%3', '%4', %5, %6, %7, %8);")
		.arg(QString::fromUtf8(msgId))
		.arg(QString::fromUtf8(msgPtr->GetConversationId()))
		.arg(QString::fromUtf8(msgPtr->GetSenderId()))
		.arg(msgPtr->GetContent())
		.arg(entityRefsSql)
		.arg(attachSql)
		.arg(reactionsSql)
		.arg(msgPtr->GetStatus())
		.toUtf8();

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
		"\"UpdatedAt\"=NOW() "
		"WHERE \"Id\"='%5';")
		.arg(msgPtr->GetContent())
		.arg(entityRefsSql)
		.arg(reactionsSql)
		.arg(msgPtr->GetStatus())
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

	return QString("DELETE FROM \"Messages\" WHERE \"Id\" IN (%1);")
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

	const QString tableName = GetTableName();
	if (TableExists(tableName)){
		return;
	}

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
