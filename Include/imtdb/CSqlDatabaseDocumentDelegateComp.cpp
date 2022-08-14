#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <istd/TOptDelPtr.h>
#include <istd/CSystem.h>
#include <istd/CCrcCalculator.h>


namespace imtdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CSqlDatabaseDocumentDelegateComp::CreateObjectFromRecord(const QByteArray& /*typeId*/, const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_documentFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<istd::IChangeable> documentPtr = m_documentFactCompPtr.CreateInstance();
	if (!documentPtr.IsValid()){
		return nullptr;
	}

	if (record.contains(*m_documentContentColumnIdAttrPtr)){
		QByteArray documentContent = record.value(qPrintable(*m_documentContentColumnIdAttrPtr)).toByteArray();

		if (ReadDataFromMemory(documentContent, *documentPtr)){
			return documentPtr.PopPtr();
		}
	}

	return nullptr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CSqlDatabaseDocumentDelegateComp::CreateNewObjectQuery(
			const QByteArray& typeId,
			const QByteArray& proposedObjectId,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable* valuePtr) const
{
	NewObjectQuery retVal;

	istd::TOptDelPtr<const istd::IChangeable> workingDocumentPtr;
	if (valuePtr != nullptr){
		workingDocumentPtr.SetPtr(valuePtr, false);
	}
	else{
		if (m_documentFactCompPtr.IsValid()){
			workingDocumentPtr.SetPtr(m_documentFactCompPtr.CreateInstance());
		}
	}

	if (workingDocumentPtr.IsValid()){
		QByteArray documentContent;
		if (WriteDataToMemory(*workingDocumentPtr, documentContent)){
			QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8() : proposedObjectId;

			quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());
//			QString checkSumString = QString::number(checksum, 16).rightJustified(8, '0').toUpper();

			retVal.query = QString("INSERT INTO %1(Id, %2, Name, Description, LastModified, Added, Checksum) VALUES('%3', '%4', '%5', '%6', '%7');")
						.arg(qPrintable(*m_tableNameAttrPtr))
						.arg(qPrintable (*m_documentContentColumnIdAttrPtr))
						.arg(qPrintable(objectId))
						.arg(objectName)
						.arg(objectDescription)
						.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
						.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
						.arg(checksum)
						.toLocal8Bit();
			
			if (m_metaInfoTableDelegateCompPtr.IsValid()){
				idoc::MetaInfoPtr metaInfoPtr = m_metaInfoTableDelegateCompPtr->CreateMetaInfo(valuePtr, typeId);
				if (metaInfoPtr.IsValid()){
					retVal.query += "\n";

					QByteArrayList metaInfoIds = m_metaInfoTableDelegateCompPtr->GetColumnIds();

					QStringList metaInfoTableValues;
					metaInfoTableValues.push_back("'" + objectId + "'");

					for (const QByteArray& metaInfoId : metaInfoIds){
						metaInfoTableValues.push_back(metaInfoPtr->GetMetaInfo(m_metaInfoTableDelegateCompPtr->GetMetaInfoType(metaInfoId)).toString());
					}

					retVal.query += QString("INSERT INTO %1(Id, %2) VALUES(%3);")
								.arg(qPrintable(*m_metaInfoTableNameAttrPtr))
								.arg(qPrintable(metaInfoIds.join(" ")))
								.arg(metaInfoTableValues.join(" "))
								.toLocal8Bit();
				}
			}

			retVal.objectName = objectName;
		}
	}

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId) const
{
	QByteArray retVal = QString("DELETE FROM %1 WHERE Id = '%2';").arg(qPrintable(objectId)).toLocal8Bit();

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const istd::IChangeable& object) const
{
	QByteArray retVal;

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& newObjectName) const
{
	QByteArray retVal;

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& description) const
{
	QByteArray retVal;

	return retVal;
}


// protected methods

bool CSqlDatabaseDocumentDelegateComp::WriteDataToMemory(const istd::IChangeable& object, QByteArray& data) const
{
	if (!m_documentPersistenceCompPtr.IsValid()) {
		return false;
	}

	QString tempFolder = QDir::tempPath() + "/ImtCore";

	if (!istd::CSystem::EnsurePathExists(tempFolder)){
		return false;
	}

	int flags = ifile::IFilePersistence::QF_FILE | ifile::IFilePersistence::QF_SAVE;

	QStringList supportedExts;
	m_documentPersistenceCompPtr->GetFileExtensions(supportedExts, &object, flags);

	QString fileName = QUuid::createUuid().toString();
	QString workingExtension;
	for (const QString ext : supportedExts){
		QString filePath = fileName + "." + ext;
		if (m_documentPersistenceCompPtr->IsOperationSupported(&object, &filePath, flags, false)){
			workingExtension = ext;
			break;
		}
	}

	fileName += "." + workingExtension;

	QString filePath = tempFolder + "/" + fileName;

	int operationState = m_documentPersistenceCompPtr->SaveToFile(object, filePath);
	if (operationState != ifile::IFilePersistence::OS_OK){
		istd::CSystem::RemoveDirectory(tempFolder);

		return false;
	}

	QFile documentFile(filePath);
	if (!documentFile.open(QFile::WriteOnly)){
		istd::CSystem::RemoveDirectory(tempFolder);

		return false;
	}

	data = documentFile.readAll();

	documentFile.close();

	istd::CSystem::RemoveDirectory(tempFolder);

	return true;
}


bool CSqlDatabaseDocumentDelegateComp::ReadDataFromMemory(const QByteArray& data, istd::IChangeable& object) const
{
	if (!m_documentPersistenceCompPtr.IsValid()) {
		return false;
	}

	QString tempFolder = QDir::tempPath() + "/ImtCore";

	if (!istd::CSystem::EnsurePathExists(tempFolder)){
		return false;
	}

	int flags = ifile::IFilePersistence::QF_FILE | ifile::IFilePersistence::QF_SAVE;

	QStringList supportedExts;
	m_documentPersistenceCompPtr->GetFileExtensions(supportedExts, &object, flags);

	QString fileName = QUuid::createUuid().toString();
	QString workingExtension;
	for (const QString ext : supportedExts){
		QString filePath = fileName + "." + ext;
		if (m_documentPersistenceCompPtr->IsOperationSupported(&object, &filePath, flags, false)){
			workingExtension = ext;
			break;
		}
	}

	fileName += "." + workingExtension;

	QString filePath = tempFolder + "/" + fileName;

	QFile documentFile(filePath);
	if (!documentFile.open(QFile::WriteOnly)){
		istd::CSystem::RemoveDirectory(tempFolder);

		return false;
	}

	documentFile.write(data);

	documentFile.close();

	int operationState = m_documentPersistenceCompPtr->LoadFromFile(object, filePath);
	if (operationState != ifile::IFilePersistence::OS_OK){
		istd::CSystem::RemoveDirectory(tempFolder);

		return false;
	}

	istd::CSystem::RemoveDirectory(tempFolder);

	return true;
}


// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

idoc::MetaInfoPtr CSqlDatabaseDocumentDelegateComp::CreateObjectMetaInfo(const QByteArray& typeId) const
{
	if (m_metaInfoTableDelegateCompPtr.IsValid()){
		return m_metaInfoTableDelegateCompPtr->CreateMetaInfo(nullptr, typeId);
	}

	return idoc::MetaInfoPtr();
}


bool CSqlDatabaseDocumentDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (m_metaInfoTableDelegateCompPtr.IsValid()){
		QByteArrayList columnIds = m_metaInfoTableDelegateCompPtr->GetColumnIds();
		for (const QByteArray& columnId : columnIds){
			int metaInfoType = m_metaInfoTableDelegateCompPtr->GetMetaInfoType(columnId);
			if (metaInfoType >= 0){
				if (record.contains(columnId)){
					QVariant data = record.value(qPrintable(columnId));
					if (data.isValid()){
						metaInfo.SetMetaInfo(metaInfoType, data);
					}
				}
			}
		}

		return true;
	}

	return false;
}


} // namespace imtdb


