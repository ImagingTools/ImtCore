// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchatdb/CAttachmentDbDelegateComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtSql/QSqlRecord>

// ImtCore includes
#include <imtchat/IAttachment.h>
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


istd::IChangeableUniquePtr CAttachmentDbDelegateComp::CreateObjectFromRecord(
		const QSqlRecord& record,
		const iprm::IParamsSet* /*dataConfigurationPtr*/) const
{
	if (!m_databaseEngineCompPtr.IsValid() || !m_attachmentFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtchat::IAttachment> attachPtr = m_attachmentFactCompPtr.CreateInstance();
	if (!attachPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Id")){
		attachPtr->SetId(record.value("Id").toByteArray());
	}
	if (record.contains("FileName")){
		attachPtr->SetFileName(record.value("FileName").toString());
	}
	if (record.contains("MimeType")){
		attachPtr->SetMimeType(record.value("MimeType").toString());
	}
	if (record.contains("Data")){
		attachPtr->SetData(record.value("Data").toByteArray());
	}
	if (record.contains("FileSize")){
		attachPtr->SetFileSize(record.value("FileSize").toLongLong());
	}
	if (record.contains("CreatedAt")){
		QVariant val = record.value("CreatedAt");
		QDateTime dt = val.toDateTime();
		attachPtr->SetCreatedAt(dt.isValid() ? dt.toString(Qt::ISODateWithMs) : val.toString());
	}

	return attachPtr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CAttachmentDbDelegateComp::CreateNewObjectQuery(
		const QByteArray& typeId,
		const QByteArray& proposedObjectId,
		const QString& /*objectName*/,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (typeId.isEmpty() || typeId != "Attachment"){
		return NewObjectQuery();
	}

	const imtchat::IAttachment* attachPtr = dynamic_cast<const imtchat::IAttachment*>(valuePtr);
	if (attachPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray attachId = proposedObjectId;
	if (attachId.isEmpty()){
		attachId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	const QString nowUtc = utcNow();

	// Encode binary data as hex for PostgreSQL BYTEA / SQLite BLOB
	const QByteArray hexData = attachPtr->GetData().toHex();

	// Escape single quotes in user-supplied strings to prevent SQL injection
	QString escapedFileName = attachPtr->GetFileName();
	escapedFileName.replace('\'', "''");
	QString escapedMimeType = attachPtr->GetMimeType();
	escapedMimeType.replace('\'', "''");

	NewObjectQuery retVal;
	retVal.query = QString(
		"INSERT INTO \"Attachments\" "
		"(\"Id\", \"FileName\", \"MimeType\", \"Data\", \"FileSize\", \"CreatedAt\") "
		"VALUES('%1', '%2', '%3', E'\\\\x%4', %5, '%6');")
		.arg(QString::fromUtf8(attachId))
		.arg(escapedFileName)
		.arg(escapedMimeType)
		.arg(QString::fromUtf8(hexData))
		.arg(attachPtr->GetFileSize())
		.arg(nowUtc)
		.toUtf8();

	return retVal;
}


QByteArray CAttachmentDbDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const istd::IChangeable& /*object*/,
		const imtbase::IOperationContext* /*operationContextPtr*/,
		bool /*useExternDelegate*/) const
{
	// Attachments are immutable — no update support
	return QByteArray();
}


QByteArray CAttachmentDbDelegateComp::CreateDeleteObjectsQuery(
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
		QString escaped = QString::fromUtf8(objectIds[i]);
		escaped.replace('\'', "''");
		idsStr += QString("'%1'").arg(escaped);
	}

	return QString("DELETE FROM \"Attachments\" WHERE \"Id\" IN (%1);")
		.arg(idsStr)
		.toUtf8();
}


QByteArray CAttachmentDbDelegateComp::CreateDeleteObjectSetQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const iprm::IParamsSet* /*paramsPtr*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CAttachmentDbDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*newObjectName*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CAttachmentDbDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& /*collection*/,
		const QByteArray& /*objectId*/,
		const QString& /*description*/,
		const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


void CAttachmentDbDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_databaseEngineCompPtr.IsValid()){
		return;
	}

	const QString tableName = GetTableName();
	if (TableExists(tableName)){
		return;
	}

	QFile scriptFile(imtdb::GetSqlResourcePath(*m_databaseEngineCompPtr, QStringLiteral("CreateAttachmentsTable.sql")));
	if (!scriptFile.open(QFile::ReadOnly)){
		SendErrorMessage(0, QString("Attachments table creation script '%1' could not be loaded").arg(scriptFile.fileName()));
		return;
	}

	QByteArray query = scriptFile.readAll();
	scriptFile.close();
	query.replace("${TableScheme}", "public");

	QSqlError sqlError;
	m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| Attachments table could not be created"
					<< "\n\t| Error:" << sqlError
					<< "\n\t| Query:" << query;
		SendErrorMessage(0, QString("Attachments table could not be created: %1").arg(sqlError.text()));
	}
}


idoc::MetaInfoPtr CAttachmentDbDelegateComp::CreateObjectMetaInfo(const QByteArray& typeId) const
{
	return BaseClass::CreateObjectMetaInfo(typeId);
}


bool CAttachmentDbDelegateComp::SetObjectMetaInfoFromRecord(
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
