// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchatdb/CMessageStatusDbDelegateComp.h>

// Qt includes
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>


namespace imtchatdb
{


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
