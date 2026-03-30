// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchatdb/CMessageStatusDbDelegateComp.h>

// Qt includes
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ImtCore includes
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


istd::IChangeableUniquePtr CMessageStatusDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& /*record*/,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	// MessageStatus records are not directly mapped to domain objects.
	return nullptr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CMessageStatusDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& /*proposedObjectId*/,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* /*valuePtr*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return NewObjectQuery();
}


QByteArray CMessageStatusDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const istd::IChangeable& /*object*/,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	return QByteArray();
}


QByteArray CMessageStatusDbDelegateComp::CreateDeleteObjectsQuery(
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

	return QString("DELETE FROM \"MessageStatus\" WHERE \"Id\" IN (%1);")
		.arg(idsStr)
		.toUtf8();
}


QByteArray CMessageStatusDbDelegateComp::CreateDeleteObjectSetQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const iprm::IParamsSet* /*paramsPtr*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CMessageStatusDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CMessageStatusDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CMessageStatusDbDelegateComp::CreateUpsertStatusQuery(
		const QByteArray& messageId,
		const QByteArray& userId,
		int status) const
{
	if (messageId.isEmpty() || userId.isEmpty()){
		return QByteArray();
	}

	const bool isSqlite = m_databaseEngineCompPtr.IsValid()
		&& m_databaseEngineCompPtr->GetDatabaseDriverId().compare(
				QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;

	if (isSqlite){
		// SQLite uses INSERT OR REPLACE with a generated ID
		const QString newId = QUuid::createUuid().toString(QUuid::WithoutBraces);
		return QString(
			"INSERT OR REPLACE INTO \"MessageStatus\" (\"Id\", \"MessageId\", \"UserId\", \"Status\") "
			"VALUES('%1', '%2', '%3', %4);")
			.arg(newId)
			.arg(QString::fromUtf8(messageId))
			.arg(QString::fromUtf8(userId))
			.arg(status)
			.toUtf8();
	}

	// PostgreSQL upsert via ON CONFLICT
	return QString(
		"INSERT INTO \"MessageStatus\" (\"MessageId\", \"UserId\", \"Status\") "
		"VALUES('%1', '%2', %3) "
		"ON CONFLICT (\"MessageId\", \"UserId\") DO UPDATE SET \"Status\"=%3, \"UpdatedAt\"=NOW();")
		.arg(QString::fromUtf8(messageId))
		.arg(QString::fromUtf8(userId))
		.arg(status)
		.toUtf8();
}


void CMessageStatusDbDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_databaseEngineCompPtr.IsValid()){
		return;
	}

	const QString tableName = GetTableName();
	if (TableExists(tableName)){
		return;
	}

	QFile scriptFile(GetSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("CreateMessageStatusTable.sql")));
	if (!scriptFile.open(QFile::ReadOnly)){
		SendErrorMessage(0, QT_TR_NOOP(QString("MessageStatus table creation script '%1' could not be loaded").arg(scriptFile.fileName())));
		return;
	}

	QByteArray query = scriptFile.readAll();
	scriptFile.close();
	query.replace("${TableScheme}", "public");

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| MessageStatus table could not be created"
					<< "\n\t| Error:" << sqlError
					<< "\n\t| Query:" << query;
		SendErrorMessage(0, QT_TR_NOOP(QString("MessageStatus table could not be created: %1").arg(sqlError.text())));
	}
}


idoc::MetaInfoPtr CMessageStatusDbDelegateComp::CreateObjectMetaInfo(const QByteArray& typeId) const
{
	return BaseClass::CreateObjectMetaInfo(typeId);
}


bool CMessageStatusDbDelegateComp::SetObjectMetaInfoFromRecord(
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
