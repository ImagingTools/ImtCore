// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchatdb/CConversationDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtchat/IConversation.h>
#include <imtdb/CDatabaseEngineComp.h>


namespace imtchatdb
{


namespace
{

QString GetSqlResourcePath(const imtdb::IDatabaseEngine& databaseEngine, const QString& fileName)
{
	const QByteArray driverName = databaseEngine.GetDatabaseDriverId();
	const bool isSqlite = driverName.compare(QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;
	const QString prefix = isSqlite ? QStringLiteral(":/SQL/SQLite/") : QStringLiteral(":/SQL/Postgres/");
	return prefix + fileName;
}

} // anonymous namespace


istd::IChangeableUniquePtr CConversationDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid() || !m_conversationFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtchat::IConversation> convPtr = m_conversationFactCompPtr.CreateInstance();
	if (!convPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		convPtr->SetId(record.value("Id").toByteArray());
	}
	if (record.contains("Name")){
		convPtr->SetName(record.value("Name").toString());
	}
	if (record.contains("ConversationType")){
		convPtr->SetConversationType(record.value("ConversationType").toInt());
	}
	if (record.contains("Metadata")){
		convPtr->SetMetadata(record.value("Metadata").toString());
	}
	if (record.contains("CreatedAt")){
		convPtr->SetCreatedAt(record.value("CreatedAt").toString());
	}
	if (record.contains("UpdatedAt")){
		convPtr->SetUpdatedAt(record.value("UpdatedAt").toString());
	}

	return convPtr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CConversationDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& typeId,
		const QByteArray& proposedObjectId,
		const QString& objectName,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (typeId.isEmpty() || typeId != "Conversation"){
		return NewObjectQuery();
	}

	const imtchat::IConversation* convPtr = dynamic_cast<const imtchat::IConversation*>(valuePtr);
	if (convPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray convId = proposedObjectId;
	if (convId.isEmpty()){
		convId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	QString name = convPtr->GetName();
	if (name.isEmpty()){
		name = objectName;
	}

	const int conversationType = convPtr->GetConversationType();
	const QString metadata = convPtr->GetMetadata();
	const QString metadataSql = metadata.isEmpty() ? "NULL" : QString("'%1'").arg(metadata);

	NewObjectQuery retVal;
	retVal.query = QString(
		"INSERT INTO \"Conversations\" "
		"(\"Id\", \"Name\", \"ConversationType\", \"Metadata\") "
		"VALUES('%1', '%2', %3, %4);")
		.arg(QString::fromUtf8(convId))
		.arg(name)
		.arg(conversationType)
		.arg(metadataSql)
		.toUtf8();

	return retVal;
}


QByteArray CConversationDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const istd::IChangeable& object,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	const imtchat::IConversation* convPtr = dynamic_cast<const imtchat::IConversation*>(&object);
	if (convPtr == nullptr){
		return QByteArray();
	}

	const QString metadata = convPtr->GetMetadata();
	const QString metadataSql = metadata.isEmpty() ? "NULL" : QString("'%1'").arg(metadata);

	return QString(
		"UPDATE \"Conversations\" SET "
		"\"Name\"='%1', "
		"\"ConversationType\"=%2, "
		"\"Metadata\"=%3, "
		"\"UpdatedAt\"=NOW() "
		"WHERE \"Id\"='%4';")
		.arg(convPtr->GetName())
		.arg(convPtr->GetConversationType())
		.arg(metadataSql)
		.arg(QString::fromUtf8(objectId))
		.toUtf8();
}


QByteArray CConversationDbDelegateComp::CreateDeleteObjectsQuery(
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

	return QString("UPDATE \"Conversations\" SET \"IsActive\"=FALSE WHERE \"Id\" IN (%1);")
		.arg(idsStr)
		.toUtf8();
}


QByteArray CConversationDbDelegateComp::CreateDeleteObjectSetQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const iprm::IParamsSet* /*paramsPtr*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CConversationDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& objectId,
		const QString& newObjectName,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectId.isEmpty() || newObjectName.isEmpty()){
		return QByteArray();
	}

	return QString("UPDATE \"Conversations\" SET \"Name\"='%1', \"UpdatedAt\"=NOW() WHERE \"Id\"='%2';")
		.arg(newObjectName)
		.arg(QString::fromUtf8(objectId))
		.toUtf8();
}


QByteArray CConversationDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


void CConversationDbDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_databaseEngineCompPtr.IsValid()){
		return;
	}

	const QString tableName = GetTableName();
	if (TableExists(tableName)){
		return;
	}

	QFile scriptFile(GetSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("CreateConversationsTable.sql")));
	if (!scriptFile.open(QFile::ReadOnly)){
		SendErrorMessage(0, QString("Conversations table creation script '%1' could not be loaded").arg(scriptFile.fileName()));
		return;
	}

	QByteArray query = scriptFile.readAll();
	scriptFile.close();
	query.replace("${TableScheme}", "public");

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| Conversations table could not be created"
					<< "\n\t| Error:" << sqlError
					<< "\n\t| Query:" << query;
		SendErrorMessage(0, QString("Conversations table could not be created: %1").arg(sqlError.text()));
	}
}


idoc::MetaInfoPtr CConversationDbDelegateComp::CreateObjectMetaInfo(const QByteArray& typeId) const
{
	return BaseClass::CreateObjectMetaInfo(typeId);
}


bool CConversationDbDelegateComp::SetObjectMetaInfoFromRecord(
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
