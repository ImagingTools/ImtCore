#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <istd/TOptDelPtr.h>
#include <istd/CSystem.h>
#include <istd/CCrcCalculator.h>


namespace imtdb
{


static const QByteArray s_documentIdColumn = "DocumentId";
static const QByteArray s_idColumn = "Id";


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CSqlDatabaseDocumentDelegateComp::GetSelectionQuery(const QByteArray& objectId, int offset, int count, const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		QString baseQuery = GetBaseSelectionQuery();

		return QString(
			baseQuery + QString(" AND \"%1\".Id = '%2'").arg(qPrintable(*m_tableNameAttrPtr)).arg(qPrintable(objectId))).toLocal8Bit();
	}

	return BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);
}


istd::IChangeable* CSqlDatabaseDocumentDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_documentFactoriesCompPtr.IsValid()){
		return nullptr;
	}

	QByteArray typeId = GetObjectTypeId(GetObjectIdFromRecord(record));

	istd::TDelPtr<istd::IChangeable> documentPtr = CreateObject(typeId);
	if (!documentPtr.IsValid()){
		return nullptr;
	}

	if (record.contains(*m_documentContentColumnIdAttrPtr)){
		QByteArray documentContent = record.value(qPrintable(*m_documentContentColumnIdAttrPtr)).toByteArray();

		documentContent = QByteArray::fromBase64(documentContent);

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
		workingDocumentPtr.SetPtr(CreateObject(typeId));
	}

	if (workingDocumentPtr.IsValid()){
		QByteArray documentContent;
		if (WriteDataToMemory(*workingDocumentPtr, documentContent)){
			QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8() : proposedObjectId;
			QByteArray revisionUuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();

			retVal.query = QString("INSERT INTO \"%1\"(Id, Name, Description, Added, LastRevisionId) VALUES('%2', '%3', '%4', '%5', '%6');")
						.arg(qPrintable(*m_tableNameAttrPtr))
						.arg(qPrintable(objectId))
						.arg(objectName)
						.arg(objectDescription)
						.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
						.arg(qPrintable(revisionUuid))
						.toLocal8Bit();

			quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());

			retVal.query += QString("INSERT INTO \"%1\"(Id, %2, %3, RevisionNumber, Comment, LastModified, Checksum) VALUES('%4', '%5', '%6', '%7', '%8', '%9', %10);")
				.arg(qPrintable(*m_revisionsTableNameAttrPtr))
				.arg(qPrintable(s_documentIdColumn))
				.arg(qPrintable(*m_documentContentColumnIdAttrPtr))
				.arg(qPrintable(revisionUuid))
				.arg(qPrintable(objectId))
				.arg(qPrintable(documentContent.toBase64()))
				.arg(1)
				.arg(QObject::tr("Initial revision"))
				.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
				.arg(checksum)
				.toLocal8Bit();

			if (m_metaInfoTableDelegateCompPtr.IsValid()){
				idoc::MetaInfoPtr metaInfoPtr = m_metaInfoTableDelegateCompPtr->CreateMetaInfo(valuePtr, typeId);
				if (metaInfoPtr.IsValid()){
					retVal.query += "\n";

					QByteArrayList columnIds = {s_idColumn, "RevisionId"};
					columnIds += m_metaInfoTableDelegateCompPtr->GetColumnIds();

					QStringList tableValues;
					tableValues.push_back("'" + QUuid::createUuid().toString(QUuid::WithoutBraces) + "'");
					tableValues.push_back("'" + revisionUuid + "'");

					for (const QByteArray& columnId : m_metaInfoTableDelegateCompPtr->GetColumnIds()){
						QVariant data = metaInfoPtr->GetMetaInfo(m_metaInfoTableDelegateCompPtr->GetMetaInfoType(columnId));

						QString value = m_metaInfoTableDelegateCompPtr->ToTableRepresentation(data, columnId).toString();

						tableValues.push_back("'" + value + "'");
					}

					retVal.query += QString("INSERT INTO \"%1\"(%2) VALUES(%3);")
								.arg(qPrintable(*m_metaInfoTableNameAttrPtr))
								.arg(qPrintable(columnIds.join(", ")))
								.arg(tableValues.join(", "))
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
	QByteArray retVal = QString("DELETE FROM \"%1\" WHERE %2 = '%3';").arg(qPrintable(*m_tableNameAttrPtr)).arg(qPrintable(s_idColumn)).arg(qPrintable(objectId)).toLocal8Bit();

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object) const
{
	QByteArray retVal;

	QByteArray documentContent;
	if (WriteDataToMemory(object, documentContent)){
		quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());
		QByteArray revisionUuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();

		retVal = QString("UPDATE \"%1\" SET LastRevisionId = '%2' WHERE %3 = '%4';")
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(qPrintable(revisionUuid))
					.arg(qPrintable(s_idColumn))
					.arg(qPrintable(objectId))
					.toLocal8Bit();

		retVal += QString("INSERT INTO \"%1\"(Id, %2, %3, RevisionNumber, Comment, LastModified, Checksum) VALUES('%4', '%5', '%6', '%7', '%8', '%9', %10);")
					.arg(qPrintable(*m_revisionsTableNameAttrPtr))
					.arg(qPrintable(s_documentIdColumn))
					.arg(qPrintable(*m_documentContentColumnIdAttrPtr))
					.arg(qPrintable(revisionUuid))
					.arg(qPrintable(objectId))
					.arg(qPrintable(documentContent.toBase64()))
					.arg(777777)
					.arg(QObject::tr("...."))
					.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
					.arg(checksum)
					.toLocal8Bit();

		if (m_metaInfoTableDelegateCompPtr.IsValid()) {
			idoc::MetaInfoPtr metaInfoPtr = m_metaInfoTableDelegateCompPtr->CreateMetaInfo(&object, collection.GetObjectTypeId(objectId));
			if (metaInfoPtr.IsValid()) {
				retVal += "\n";

				QByteArrayList columnIds = { s_idColumn, "RevisionId" };
				columnIds += m_metaInfoTableDelegateCompPtr->GetColumnIds();

				QStringList tableValues;
				tableValues.push_back("'" + QUuid::createUuid().toString(QUuid::WithoutBraces) + "'");
				tableValues.push_back("'" + revisionUuid + "'");

				for (const QByteArray& columnId : m_metaInfoTableDelegateCompPtr->GetColumnIds()) {
					QVariant data = metaInfoPtr->GetMetaInfo(m_metaInfoTableDelegateCompPtr->GetMetaInfoType(columnId));

					QString value = m_metaInfoTableDelegateCompPtr->ToTableRepresentation(data, columnId).toString();

					tableValues.push_back("'" + value + "'");
				}

				retVal += QString("INSERT INTO \"%1\"(%2) VALUES(%3);")
					.arg(qPrintable(*m_metaInfoTableNameAttrPtr))
					.arg(qPrintable(columnIds.join(", ")))
					.arg(tableValues.join(", "))
					.toLocal8Bit();
			}
		}
	}

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*newObjectName*/) const
{
	QByteArray retVal;

	return retVal;
}


QByteArray CSqlDatabaseDocumentDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/) const
{
	QByteArray retVal;

	return retVal;
}


// protected methods

istd::IChangeable* CSqlDatabaseDocumentDelegateComp::CreateObject(const QByteArray& typeId) const
{
	if (!m_typesCompPtr.IsValid()){
		return nullptr;
	}

	Q_ASSERT_X(m_documentFactoriesCompPtr.GetCount() == m_typesCompPtr->GetOptionsCount(), __FILE__, "Number of factories and resource types doesn't match");

	for (int i = 0; i < m_typesCompPtr->GetOptionsCount(); ++i){
		if (i < m_documentFactoriesCompPtr.GetCount()){
			if (typeId == m_typesCompPtr->GetOptionId(i)){
				return m_documentFactoriesCompPtr.CreateInstance(i);
			}
		}
	}
	return nullptr;
}


bool CSqlDatabaseDocumentDelegateComp::WriteDataToMemory(const istd::IChangeable& object, QByteArray& data) const
{
	if (!m_documentPersistenceCompPtr.IsValid()){
		return false;
	}

	QString tempFolder = QDir::tempPath() + "/ImtCore/SqlDatabaseDocumentDelegate/" + QUuid::createUuid().toString();

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
	if (!documentFile.open(QFile::ReadOnly)){
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
	if (!m_documentPersistenceCompPtr.IsValid()){
		return false;
	}

	QString tempFolder = QDir::tempPath() + "/ImtCore/SqlDatabaseDocumentDelegate/" + QUuid::createUuid().toString();

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

QString CSqlDatabaseDocumentDelegateComp::GetBaseSelectionQuery() const
{
	QString metaInfoValuesQuery;
	QString joinMetaInfoQuery;
	if (m_metaInfoTableDelegateCompPtr.IsValid()){
		QByteArrayList columnIds = m_metaInfoTableDelegateCompPtr->GetColumnIds();
		for (int i = 0; i < columnIds.count(); ++i){
			metaInfoValuesQuery += QString(", \"%1\".%2").arg(qPrintable(*m_metaInfoTableNameAttrPtr)).arg(qPrintable(columnIds[i]));
		}

		joinMetaInfoQuery = QString("JOIN \"%1\" ON \"%2\".LastRevisionId = \"%1\".RevisionId")
				.arg(qPrintable(*m_metaInfoTableNameAttrPtr))
				.arg(qPrintable(*m_tableNameAttrPtr));
	}

	return QString("SELECT \"%1\".*, \"%2\".document, \"%2\".lastmodified, \"%2\".checksum, \"%2\".RevisionNumber, \"%2\".Comment %3 FROM \"%1\" JOIN \"%2\" ON \"%1\".LastRevisionId = \"%2\".Id %4")
		.arg(qPrintable(*m_tableNameAttrPtr))
		.arg(qPrintable(*m_revisionsTableNameAttrPtr))
		.arg(metaInfoValuesQuery)
		.arg(joinMetaInfoQuery);
}


bool CSqlDatabaseDocumentDelegateComp::CreateObjectInfoFromRecord(
		const QSqlRecord& record,
		idoc::MetaInfoPtr& objectMetaInfoPtr,
		idoc::MetaInfoPtr& collectionItemMetaInfoPtr) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	QByteArray objectId = record.value(qPrintable(s_idColumn)).toByteArray();

	QByteArray sqlMetaInfoQuery = QString("SELECT * FROM \"%1\" WHERE RevisionId = (SELECT lastRevisionId FROM \"%3\" WHERE Id = '%2')")
				.arg(qPrintable(*m_metaInfoTableNameAttrPtr))
				.arg(qPrintable(objectId))
				.arg(qPrintable(*m_tableNameAttrPtr))
				.toLocal8Bit();

	QSqlQuery metaInfoQuery = m_databaseEngineCompPtr->ExecSqlQuery(sqlMetaInfoQuery);
	if (metaInfoQuery.next()){
		QSqlRecord metaInfoRecord = metaInfoQuery.record();

		objectMetaInfoPtr = CreateObjectMetaInfo(QByteArray());
		if (objectMetaInfoPtr.IsValid()){
			if (!SetObjectMetaInfoFromRecord(metaInfoRecord, *objectMetaInfoPtr)){
				objectMetaInfoPtr.Reset();

				return false;
			}
		}
	}

	collectionItemMetaInfoPtr.SetPtr(CreateCollectionItemMetaInfo(QByteArray()));
	if (collectionItemMetaInfoPtr.IsValid()){
		if (!SetCollectionItemMetaInfoFromRecord(record, *collectionItemMetaInfoPtr)){
			collectionItemMetaInfoPtr.Reset();

			return false;
		}
	}

	return true;
}



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
					QVariant data = m_metaInfoTableDelegateCompPtr->FromTableRepresentation(record.value(qPrintable(columnId)), columnId);
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


