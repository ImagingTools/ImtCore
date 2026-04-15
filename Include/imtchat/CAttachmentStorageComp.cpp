// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtchat/CAttachmentStorageComp.h>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>


namespace imtchat
{


// reimplemented (imtchat::IAttachmentStorage)

QByteArray CAttachmentStorageComp::StoreAttachment(
		const QByteArray& data,
		const QString& fileName,
		const QString& mimeType)
{
	if (!m_attachmentCollectionCompPtr.IsValid() || !m_attachmentFactCompPtr.IsValid()){
		return QByteArray();
	}

	istd::TUniqueInterfacePtr<IAttachment> attachPtr = m_attachmentFactCompPtr.CreateInstance();
	if (!attachPtr.IsValid()){
		return QByteArray();
	}

	QByteArray attachId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	attachPtr->SetId(attachId);
	attachPtr->SetFileName(fileName);
	attachPtr->SetMimeType(mimeType);
	attachPtr->SetData(data);
	attachPtr->SetFileSize(data.size());
	attachPtr->SetCreatedAt(QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));

	QByteArray result = m_attachmentCollectionCompPtr->InsertNewObject(QByteArrayLiteral("Attachment"), fileName, "", attachPtr.GetPtr(), attachId);
	if (result.isEmpty()){
		return QByteArray();
	}

	return attachId;
}


bool CAttachmentStorageComp::GetAttachment(
		const QByteArray& attachmentId,
		QByteArray& data,
		QString& fileName,
		QString& mimeType) const
{
	if (!m_attachmentCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_attachmentCollectionCompPtr->GetObjectData(attachmentId, dataPtr)){
		return false;
	}

	const IAttachment* attachPtr = dynamic_cast<const IAttachment*>(dataPtr.GetPtr());
	if (attachPtr == nullptr){
		return false;
	}

	data = attachPtr->GetData();
	fileName = attachPtr->GetFileName();
	mimeType = attachPtr->GetMimeType();

	return true;
}


bool CAttachmentStorageComp::DeleteAttachment(const QByteArray& attachmentId)
{
	if (!m_attachmentCollectionCompPtr.IsValid()){
		return false;
	}

	return m_attachmentCollectionCompPtr->RemoveElements({attachmentId});
}


int CAttachmentStorageComp::CleanupOrphanedAttachments(int ttlMinutes)
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return -1;
	}

	// Compute the cutoff time in C++ so the query works for both PostgreSQL and SQLite
	QString cutoffTime = QDateTime::currentDateTimeUtc().addSecs(-qint64(ttlMinutes) * 60).toString(Qt::ISODateWithMs);

	// Delete attachments not referenced by any message and older than TTL
	QByteArray query = QString(
		"DELETE FROM \"Attachments\" "
		"WHERE \"Id\" NOT IN (SELECT \"AttachmentId\" FROM \"MessageAttachments\") "
		"AND \"CreatedAt\" < '%1';")
		.arg(cutoffTime)
		.toUtf8();

	QSqlError sqlError;
	QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(query, &sqlError);

	if (sqlError.type() != QSqlError::NoError){
		qWarning() << "CAttachmentStorageComp: orphan cleanup failed:" << sqlError.text();
		return -1;
	}

	int deleted = sqlQuery.numRowsAffected();
	if (deleted > 0){
		qInfo() << "CAttachmentStorageComp: cleaned up" << deleted << "orphaned attachment(s) older than" << ttlMinutes << "minutes";
	}

	return deleted;
}


} // namespace imtchat
