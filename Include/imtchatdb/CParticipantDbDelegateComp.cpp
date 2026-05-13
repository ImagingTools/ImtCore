// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchatdb/CParticipantDbDelegateComp.h>

// Qt includes
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtdb/CDatabaseEngineComp.h>
#include <imtdb/imtdb.h>


namespace imtchatdb
{


istd::IChangeableUniquePtr CParticipantDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& /*record*/,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	// Participant rows are not mapped to standalone domain objects.
	return nullptr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CParticipantDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& /*proposedObjectId*/,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* /*valuePtr*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return NewObjectQuery();
}


QByteArray CParticipantDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const istd::IChangeable& /*object*/,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	return QByteArray();
}


QByteArray CParticipantDbDelegateComp::CreateDeleteObjectsQuery(
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

	return QString("DELETE FROM \"ConversationParticipants\" WHERE \"Id\" IN (%1);")
		.arg(idsStr)
		.toUtf8();
}


QByteArray CParticipantDbDelegateComp::CreateDeleteObjectSetQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const iprm::IParamsSet* /*paramsPtr*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CParticipantDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CParticipantDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CParticipantDbDelegateComp::CreateAddParticipantQuery(
		const QByteArray& conversationId,
		const QByteArray& userId) const
{
	if (conversationId.isEmpty() || userId.isEmpty()){
		return QByteArray();
	}

	const bool isSqlite = m_databaseEngineCompPtr.IsValid()
		&& m_databaseEngineCompPtr->GetDatabaseDriverId().compare(
				QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;

	if (isSqlite){
		const QString newId = QUuid::createUuid().toString(QUuid::WithoutBraces);
		return QString(
			"INSERT OR IGNORE INTO \"ConversationParticipants\" (\"Id\", \"ConversationId\", \"UserId\") "
			"VALUES('%1', '%2', '%3');")
			.arg(newId)
			.arg(QString::fromUtf8(conversationId))
			.arg(QString::fromUtf8(userId))
			.toUtf8();
	}

	return QString(
		"INSERT INTO \"ConversationParticipants\" (\"ConversationId\", \"UserId\") "
		"VALUES('%1', '%2') ON CONFLICT DO NOTHING;")
		.arg(QString::fromUtf8(conversationId))
		.arg(QString::fromUtf8(userId))
		.toUtf8();
}


QByteArray CParticipantDbDelegateComp::CreateRemoveParticipantQuery(
		const QByteArray& conversationId,
		const QByteArray& userId) const
{
	if (conversationId.isEmpty() || userId.isEmpty()){
		return QByteArray();
	}

	return QString(
		"DELETE FROM \"ConversationParticipants\" "
		"WHERE \"ConversationId\"='%1' AND \"UserId\"='%2';")
		.arg(QString::fromUtf8(conversationId))
		.arg(QString::fromUtf8(userId))
		.toUtf8();
}


idoc::MetaInfoPtr CParticipantDbDelegateComp::CreateObjectMetaInfo(const QByteArray& typeId) const
{
	return BaseClass::CreateObjectMetaInfo(typeId);
}


bool CParticipantDbDelegateComp::SetObjectMetaInfoFromRecord(
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
