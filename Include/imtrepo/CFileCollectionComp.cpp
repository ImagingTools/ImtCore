#include <imtrepo/CFileCollectionComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtCore/QDir>

// ACF includes
#include <istd/TOptDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CSystem.h>
#include <istd/CCrcCalculator.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <ilog/CMessage.h>
#include <ifile/CFileListProviderComp.h>
#include <ifile/CCompactXmlFileReadArchive.h>
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtbase/CObjectCollectionInsertEvent.h>
#include <imtbase/CObjectCollectionUpdateEvent.h>
#include <imtbase/CObjectCollectionRemoveEvent.h>


namespace imtrepo
{


// public methods

CFileCollectionComp::CFileCollectionComp()
	:m_collectionLock(QReadWriteLock::Recursive),
	m_directoryBlocked(false),
	m_readerThread(this)
{
	m_resourceTypeConstraints.SetParent(this);
}


// reimplemented (imtbase::IRevisionController)

imtbase::IRevisionController::RevisionInfoList CFileCollectionComp::GetRevisionInfoList(const imtbase::IObjectCollection& /*collection*/, const QByteArray& objectId) const
{
	imtbase::IRevisionController::RevisionInfoList retVal;

	if (*m_isEnableRevisionHistoryAttrPtr){
		RevisionsContents revisionsContents;
		if (!LoadRevisionsContents(objectId, revisionsContents)){
			if (CreateRevisionsContents(objectId)){
				LoadRevisionsContents(objectId, revisionsContents);
			}
		}

		QReadLocker locker(&m_collectionLock);

		int fileIndex = GetFileIndexById(objectId);
		if (fileIndex >= 0){
			CollectionItem& item = m_files[fileIndex];
			QFileInfo info(item.filePathInRepository);
			QString itemPath = info.absolutePath();

			for (int key : revisionsContents.keys()){
				RevisionInfoListItem revisionInfoListItem;
				RevisionsContentsItem revisionsContentsItem = revisionsContents.value(key);
				*dynamic_cast<RevisionInfo*>(&revisionInfoListItem) = *dynamic_cast<RevisionInfo*>(&revisionsContentsItem);

				revisionInfoListItem.isArchivePresent = false;
				if (QFile::exists(itemPath + "/Revisions/" + revisionsContentsItem.path)){
					revisionInfoListItem.isArchivePresent = true;
				}

				retVal.append(revisionInfoListItem);
			}
		}
	}

	return retVal;
}


bool CFileCollectionComp::RestoreObject(const imtbase::IObjectCollection& /*collection*/, const QByteArray& objectId, int revision) const
{
	if (!*m_isEnableRevisionHistoryAttrPtr){
		return false;
	}

	if (!m_compressorCompPtr.IsValid()){
		return false;
	}

	if (m_rightsProviderCompPtr.IsValid() && !m_rightsProviderCompPtr->HasRight(*m_restoreRevisionRightIdAttrPtr)){
		return false;
	}

	QWriteLocker readLocker(&m_collectionLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex < 0){
		return false;
	}

	CollectionItem& item = m_files[fileIndex];

	readLocker.unlock();

	QFileInfo info(item.filePathInRepository);
	QString revisionsPath = info.absolutePath() + "/Revisions";

	QString tempPath = GetTempDirectory() + "/ImtCore/" + QUuid::createUuid().toString();
	QDir tempDir(tempPath);

	RevisionsContents revisionsContents;
	if (LoadRevisionsContents(objectId, revisionsContents)){
		if (revisionsContents.contains(revision)){
			if (!istd::CSystem::EnsurePathExists(tempPath)){
				SendErrorMessage(0, QString(QObject::tr("Target folder '%1' could not be created")).arg(tempPath));
				tempDir.removeRecursively();
				return false;
			}

			QWriteLocker writeLocker(&m_collectionLock);

			QString revisionPath = revisionsPath + "/" + revisionsContents[revision].path;
			if (!m_compressorCompPtr->DecompressFolder(revisionPath, tempPath)){
				SendErrorMessage(0, QString(QObject::tr("Revision '%1' could not be decompressed")).arg(revisionPath));
				tempDir.removeRecursively();
				return false;
			}

			QString objectDataFilePath = tempPath + "/object.data";
			QString objectItemFilePath = tempPath + "/object.item";
			QString objectMetaFilePath = tempPath + "/object.meta";
			QString revisionMetaFilePath = tempPath + "/revision.meta";

			if (QFile::exists(objectDataFilePath) && QFile::exists(objectItemFilePath) && QFile::exists(objectMetaFilePath)){
				RevisionMetaInfo revisionMetaInfo;
				ifile::CCompactXmlFileReadArchive revisionMetaArchive(revisionMetaFilePath, m_versionInfoCompPtr.GetPtr());
				if (revisionMetaInfo.Serialize(revisionMetaArchive)){
					static ChangeSet changes(CF_UPDATED);
					istd::CChangeNotifier changeNotifier(const_cast<CFileCollectionComp*>(this), &changes);

					if (revisionMetaInfo.revision == revision){
						ifile::CCompactXmlFileReadArchive revisionItemArchive(objectItemFilePath, m_versionInfoCompPtr.GetPtr());
						CollectionItem revisionItem("");

						if (!revisionItem.Serialize(revisionItemArchive)){
							SendErrorMessage(0, QString("Collection item could not be loaded from '%1'").arg(objectItemFilePath));
							return false;
						}

						QString itemMetaFilePath = GetMetaInfoFilePath(m_files[fileIndex]);

						istd::CSystem::FileCopy(objectDataFilePath, item.filePathInRepository, true);
						istd::CSystem::FileCopy(objectMetaFilePath, itemMetaFilePath, true);

						if (!item.contentsMetaInfoPtr.IsValid()){
							imtbase::IMetaInfoCreator::MetaInfoPtr retVal;

							if (m_metaInfoCreatorMap.contains(item.typeId)){
								m_metaInfoCreatorMap[item.typeId]->CreateMetaInfo(nullptr, item.typeId, retVal);
							}

							if (!retVal.IsValid()){
								retVal.SetPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>);
							}

							Q_ASSERT(retVal.IsValid());

							item.contentsMetaInfoPtr = retVal;
						}

						item.metaInfo.CopyFrom(revisionItem.metaInfo);

						SaveCollectionItem(item);

						if (item.contentsMetaInfoPtr.IsValid()){
							LoadFileMetaInfo(*item.contentsMetaInfoPtr, itemMetaFilePath);
						}

						writeLocker.unlock();

						imtbase::CObjectCollectionUpdateEvent event(objectId, imtbase::CObjectCollectionUpdateEvent::UT_DATA);
						for (imtbase::IObjectCollectionEventHandler* eventHandlerPtr : m_eventHandlerList){
							eventHandlerPtr->OnObjectCollectionEvent(this, &event);
						}

						tempDir.removeRecursively();

						return true;
					}
				}
			}
		}
	}

	tempDir.removeRecursively();
	return false;
}


int CFileCollectionComp::BackupObject(const imtbase::IObjectCollection& /*collection*/, const QByteArray& objectId, const QString& userComment) const
{
	if (!*m_isEnableRevisionHistoryAttrPtr){
		return -1;
	}

	if (!m_compressorCompPtr.IsValid()){
		SendCriticalMessage(0, "Compression component was not set. Backup is not possible", "File Repository");

		return -1;
	}

	QReadLocker readLocker(&m_collectionLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex < 0){
		return -1;
	}

	CollectionItem& collectionItem = m_files[fileIndex];

	readLocker.unlock();

	RevisionsContents revisionsContents;
	LoadRevisionsContents(objectId, revisionsContents);
	int newRevision = 0;

	for (int revision : revisionsContents.keys()){
		if (newRevision < revision){
			newRevision = revision;
		}
	}

	int revision = collectionItem.metaInfo.GetMetaInfo(imtrepo::IFileObjectCollection::MIT_REVISION).toInt();
	if (revision >= newRevision){
		newRevision = revision;
	}

	newRevision++;

	collectionItem.metaInfo.SetMetaInfo(imtrepo::IFileObjectCollection::MIT_REVISION, newRevision);
	collectionItem.metaInfo.SetMetaInfo(MIT_LAST_OPERATION_TIME, QDateTime::currentDateTime());

	SaveCollectionItem(collectionItem);

	QFileInfo info(collectionItem.filePathInRepository);
	QString revisionsPath = info.absolutePath() + "/Revisions/";

	QString tempPath = GetTempDirectory() + "/ImtCore/" + QUuid::createUuid().toString();
	QDir tempDir(tempPath);

	if (!istd::CSystem::EnsurePathExists(tempPath)){
		SendErrorMessage(0, QString(QObject::tr("Target folder '%1' could not be created")).arg(tempPath));
		return -1;
	}

	readLocker.relock();

	QFileInfo dataFile(m_files[fileIndex].filePathInRepository);
	QFileInfo itemFile(GetDataItemFilePath(m_files[fileIndex]));
	QFileInfo metaFile(GetMetaInfoFilePath(m_files[fileIndex]));

	if (istd::CSystem::FileCopy(dataFile.absoluteFilePath(), tempPath + "/object.data")){
		if (istd::CSystem::FileCopy(itemFile.absoluteFilePath(), tempPath + "/object.item")){
			if (istd::CSystem::FileCopy(metaFile.absoluteFilePath(), tempPath + "/object.meta")){
				RevisionMetaInfo revisionMetaInfo;
				revisionMetaInfo.revision = newRevision;
				revisionMetaInfo.timestamp = QDateTime::currentDateTime();
				revisionMetaInfo.comment = userComment;
				if (m_loginProviderCompPtr.IsValid()){
					iauth::CUser* userPtr = m_loginProviderCompPtr->GetLoggedUser();
					if (userPtr != nullptr){
						revisionMetaInfo.user = userPtr->GetUserName();
					}
				}

				ifile::CCompactXmlFileWriteArchive archive(tempPath + "/revision.meta", m_versionInfoCompPtr.GetPtr());
				if (revisionMetaInfo.Serialize(archive)){
					archive.Flush();
					QString revisionPath = revisionsPath + QString("/%1.zip").arg(newRevision);
					if (m_compressorCompPtr->CompressFolder(tempPath, revisionPath, false)){
						imtbase::CObjectCollectionUpdateEvent event(objectId, imtbase::CObjectCollectionUpdateEvent::UT_DATA);
						for (imtbase::IObjectCollectionEventHandler* eventHandlerPtr : m_eventHandlerList){
							eventHandlerPtr->OnObjectCollectionEvent(this, &event);
						}

						RevisionsContentsItem revisionsContentsItem;
						*dynamic_cast<RevisionInfo*>(&revisionsContentsItem) = *dynamic_cast<RevisionInfo*>(&revisionMetaInfo);
						revisionsContentsItem.path = QDir(revisionsPath).relativeFilePath(revisionPath);
						revisionsContents[newRevision] = revisionsContentsItem;

						SaveRevisionsContents(objectId, revisionsContents);

						tempDir.removeRecursively();
						return newRevision;
					}
				}
			}
		}
	}

	tempDir.removeRecursively();
	return -1;
}


// reimplemented (IFileObjectCollection)

const ifile::IFileResourceTypeConstraints* CFileCollectionComp::GetFileTypeConstraints() const
{
	if (!m_resourceFileTypesCompPtr.IsValid() || !m_resourceTypesCompPtr.IsValid()){
		return nullptr;
	}

	return &m_resourceTypeConstraints;
}


IFileObjectCollection::FileInfo CFileCollectionComp::GetFileInfo(const QByteArray& objectId) const
{
	FileInfo retVal;

	QReadLocker locker(&m_collectionLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		CollectionItem collectionItem = m_files[fileIndex];

		locker.unlock();

		QString repositoryFilePath = collectionItem.filePathInRepository;

		retVal.fileName = QFileInfo(repositoryFilePath).fileName();

		if (!*m_useSubfolderAttrPtr){
			int index = retVal.fileName.indexOf("} - ");
			if (index  != -1){
				retVal.fileName = retVal.fileName.mid(index + 4);
			}
		}

		retVal.filePath = repositoryFilePath;
		retVal.lastModified = collectionItem.metaInfo.GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME).toDateTime();
	}

	return retVal;
}


QString CFileCollectionComp::GetFile(
			const QByteArray& objectId,
			const QString& targetFilePath) const
{
	QReadLocker locker(&m_collectionLock);

	QString outputFilePath = targetFilePath;

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		QString filePathInRepository =  m_files[fileIndex].filePathInRepository;

		QFileInfo fileInfo(filePathInRepository);
		if (fileInfo.exists()){
			// If the output file path is empty, then create the file path automatically:
			if (outputFilePath.isEmpty()){
				QString fileExtension = QFileInfo(filePathInRepository).suffix();

				outputFilePath =  GetTempDirectory() + "/ImtCore/" + QFileInfo(filePathInRepository).completeBaseName() + "_" + QUuid::createUuid().toString() + "." + fileExtension;
			}

			if (filePathInRepository == outputFilePath){
				return outputFilePath;
			}

			if (istd::CSystem::FileCopy(filePathInRepository, outputFilePath, true)){
				return outputFilePath;
			}
			else{
				SendErrorMessage(0, QString("File could not be copied from '%1' to '%2'").arg(filePathInRepository).arg(targetFilePath));
			}
		}
		else{
			SendErrorMessage(0, QString("Collection file doesn't exist '%1'").arg(filePathInRepository));
		}
	}

	return QString();
}


QByteArray CFileCollectionComp::InsertFile(
			const QString& localFilePath,
			const QByteArray& typeId,
			const QString& objectName,
			const QString& objectDescription,
			const QByteArray& proposedObjectId)
{
	static QByteArray emptyId;

	QWriteLocker locker(&m_collectionLock);

	for (int i = 0; i < m_files.count(); ++i){
		CollectionItem& collectionItem = m_files[i];

		if (!proposedObjectId.isEmpty()){
			if (collectionItem.fileId == proposedObjectId){
				SendErrorMessage(0, QString("File '%1' could not be inserted into the repository. Proposed object-ID ('%2') already in use").arg(localFilePath).arg(qPrintable(proposedObjectId)));

				return emptyId;
			}
		}

		if (m_files[i].filePathInRepository == localFilePath){
			if (proposedObjectId.isEmpty()){
				SendInfoMessage(0, QString("The file '%1' already exists in the repository").arg(localFilePath));

				return collectionItem.fileId;
			}

			SendInfoMessage(0, QString("Trying to add already existing file '%1' using a different file-ID. New repository item will be created").arg(localFilePath));
		}

		if (!m_repositoryPathCompPtr.IsValid() && (collectionItem.sourceFilePath == localFilePath)){
			if (proposedObjectId.isEmpty() || (!proposedObjectId.isEmpty() && (proposedObjectId == collectionItem.fileId))){
				QFileInfo inputFileInfo(localFilePath);
				if (inputFileInfo.lastModified() == collectionItem.metaInfo.GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME).toDateTime()){
					istd::CChangeNotifier changeNotifier(this);

					collectionItem.metaInfo.SetMetaInfo(MIT_LAST_OPERATION_TIME, QDateTime::currentDateTime());
					collectionItem.metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION, objectDescription);

					SendVerboseMessage(QString("The file '%1' is already exists in the repository").arg(localFilePath));

					QByteArray fileId = collectionItem.fileId;

					locker.unlock();
	
					return fileId;
				}
			}
		}
	}

	locker.unlock();

	QString targetFilePath;

	QFileInfo inputFileInfo(localFilePath);

	QString targetResourceName = objectName.isEmpty() ? inputFileInfo.completeBaseName() : objectName;

	if (!InsertFileIntoCollection(localFilePath, targetResourceName, typeId, nullptr, targetFilePath)){
		return emptyId;
	}

	QFileInfo targetFilePathInfo(targetFilePath);

	QByteArray fileId = proposedObjectId.isEmpty() ? QUuid::createUuid().toByteArray() : proposedObjectId;

	CollectionItem collectionItem(GetCollectionRootFolder());
	collectionItem.fileId = fileId;
	collectionItem.typeId = typeId;
	collectionItem.objectName = targetFilePathInfo.completeBaseName();
	collectionItem.filePathInRepository = targetFilePath;
	collectionItem.sourceFilePath = localFilePath;

	UpdateItemMetaInfo(collectionItem);

	collectionItem.metaInfo.SetMetaInfo(MIT_LAST_OPERATION_TIME, QDateTime::currentDateTime());
	collectionItem.metaInfo.SetMetaInfo(MIT_INSERTION_TIME, QDateTime::currentDateTime());
	collectionItem.metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION, objectDescription);

	QString savedPath = SaveCollectionItem(collectionItem);
	if (!savedPath.isEmpty()){
		static ChangeSet changes(CF_ADDED);
		istd::CChangeNotifier changeNotifier(this, &changes);

		locker.relock();

		// Insert into the file list:
		m_files.push_back(collectionItem);

		locker.unlock();

		imtbase::CObjectCollectionInsertEvent event(fileId);
		for (imtbase::IObjectCollectionEventHandler* eventHandlerPtr : m_eventHandlerList){
			eventHandlerPtr->OnObjectCollectionEvent(this, &event);
		}

		return fileId;
	}
	else{
		if (!QFile::remove(targetFilePath)){
			SendErrorMessage(0, QString("File '%1' could not be deleted").arg(targetFilePath));
		}
	}

	SendErrorMessage(0, "File could not be added to the repository");

	return QByteArray();
}


bool CFileCollectionComp::UpdateFile(
			const QString& localFilePath,
			const QByteArray& objectId)
{
	QWriteLocker locker(&m_collectionLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex < 0){
		SendErrorMessage(0, QString("No file with the ID '%1' was found in the repository. File could not be updated").arg(objectId.constData()));

		return false;
	}

	CollectionItem collectionItem = m_files[fileIndex];

	locker.unlock();

	// Create meta-informations for the file contents:
	collectionItem.contentsMetaInfoPtr = CreateFileContentsMetaInfo(localFilePath, collectionItem.typeId);
	if (!collectionItem.contentsMetaInfoPtr.IsValid()){
		SendErrorMessage(0, QString("Meta-informations could not be created for'%1'. File could not be updated").arg(localFilePath));

		return false;
	}

	QFileInfo inputFileInfo(localFilePath);

	QString targetFilePath = collectionItem.filePathInRepository;

	if (istd::CSystem::FileCopy(localFilePath, targetFilePath, true)){
		if (!QFile::setPermissions(targetFilePath, QFile::WriteGroup)){
			SendErrorMessage(0, QString("Permissions for the file '%1' could not be set").arg(targetFilePath));
		}

		locker.relock();

		static ChangeSet changes(CF_UPDATED);
		istd::CChangeNotifier changeNotifier(this, &changes);

		m_files[fileIndex].CopyFrom(collectionItem);

		SaveCollectionItem(collectionItem);

		QString metaInfoFilePath = GetMetaInfoFilePath(collectionItem);
		if (!SaveMetaInfo(*collectionItem.contentsMetaInfoPtr, metaInfoFilePath)){
			SendErrorMessage(0, QString("Meta information of the file '%1' could not be updated").arg(targetFilePath));
		}

		locker.unlock();

		imtbase::CObjectCollectionUpdateEvent event(objectId, imtbase::CObjectCollectionUpdateEvent::UT_DATA);
		for (imtbase::IObjectCollectionEventHandler* eventHandlerPtr : m_eventHandlerList){
			eventHandlerPtr->OnObjectCollectionEvent(this, &event);
		}

		return true;
	}

	SendErrorMessage(0, QString("File '%1' could not be copied to %2").arg(localFilePath).arg(targetFilePath));

	return false;
}


// reimplemented (IFileCollectionInfo)

QString CFileCollectionComp::GetCollectionRootFolder() const
{
	QString retVal;

	if (m_repositoryPathCompPtr.IsValid()){
		retVal = m_repositoryPathCompPtr->GetPath();
	}

	return retVal;
}


// reimplemented (IObjectCollection)

const imtbase::IRevisionController * CFileCollectionComp::GetRevisionController() const
{
	return this;
}


int CFileCollectionComp::GetOperationFlags(const QByteArray& /*objectId*/) const
{
	return OF_SUPPORT_DELETE | OF_SUPPORT_EDIT | OF_SUPPORT_INSERT | OF_SUPPORT_RENAME;
}


bool CFileCollectionComp::GetDataMetaInfo(const QByteArray& objectId, ifile::IFileMetaInfoProvider::MetaInfoPtr& metaInfoPtr) const
{
	QReadLocker lockCollection(&m_collectionLock);

	// Looking for file item:
	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex < 0){
		SendVerboseMessage(QString("Collection item doesn't exist for the given object-ID (%1). Meta-information could not be provided").arg(objectId.constData()), "File Collection");

		return false;
	}

	// Get meta-information from cache:
	CollectionItem& item = m_files[fileIndex];

	if (item.contentsMetaInfoPtr.IsValid()){
		metaInfoPtr.SetCastedOrRemove(item.contentsMetaInfoPtr->CloneMe());

		return metaInfoPtr.IsValid();
	}

	return false;
}


QByteArray CFileCollectionComp::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& /*description*/,
			const istd::IChangeable * defaultValuePtr,
			const QByteArray& proposedObjectId)
{
	istd::TOptDelPtr<const istd::IChangeable> newObjectPtr;

	if (defaultValuePtr != nullptr){
		newObjectPtr.SetPtr(defaultValuePtr, false);
	}
	else{
		newObjectPtr.SetPtr(CreateDataObject(typeId), true);
	}

	if (newObjectPtr.IsValid()){
		const ifile::IFilePersistence* persistencePtr = GetPersistenceForResource(typeId);
		if (persistencePtr != nullptr){
			QStringList supportedExts;
			persistencePtr->GetFileExtensions(supportedExts, defaultValuePtr, ifile::IFilePersistence::QF_SAVE);

			QString targetFolder = GetTempDirectory() + "/ImtCore";

			if (!istd::CSystem::EnsurePathExists(targetFolder)){
				SendErrorMessage(0, QString(QObject::tr("Target folder '%1' could not be created")).arg(targetFolder));

				return QByteArray();
			}

			QString tempFilePath = targetFolder + "/" + QUuid::createUuid().toString() + "." + supportedExts[0];

			if (persistencePtr->SaveToFile(*newObjectPtr, tempFilePath) == ifile::IFilePersistence::OS_OK){
				QByteArray retval = InsertFile(tempFilePath, typeId, name, QString(), proposedObjectId);

				QFile::remove(tempFilePath);

				return retval;
			}
			else{
				SendErrorMessage(0, QString(QObject::tr("File could not be saved into '%1'")).arg(tempFilePath));
			}
		}
	}

	return QByteArray();
}


bool CFileCollectionComp::RemoveObject(const QByteArray& objectId)
{
	if (objectId.isEmpty()){
		SendErrorMessage(0, "object-ID is empty. Unknown resource could not be removed");

		return false;
	}

	QWriteLocker repositoryDataLocker(&m_collectionLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		// If the file was copied into the repository, remove also the file from repository folder:
		const CollectionItem& itemToRemove = m_files[fileIndex];

		QFileInfo fileInfo(itemToRemove.filePathInRepository);

		QString dataFilePath = GetDataItemFilePath(itemToRemove);
		QFile dataFile(dataFilePath);
		if (!dataFile.remove()){
			SendErrorMessage(0, QString("Collection data file '%1' could not be removed. Error status: %2").arg(dataFilePath).arg(dataFile.errorString()));

			return false;
		}

		QString metaDataFilePath = GetMetaInfoFilePath(itemToRemove);
		QFile metaDataFile(metaDataFilePath);
		if (!metaDataFile.remove()){
			SendErrorMessage(0, QString("Collection meta-data file '%1' could not be removed. Error status: %2").arg(metaDataFilePath).arg(metaDataFile.errorString()));

			return false;
		}

		if (itemToRemove.filePathInRepository != itemToRemove.sourceFilePath){
			QString parentDirectory = fileInfo.absolutePath();

			if (*m_useSubfolderAttrPtr){
				if (!istd::CSystem::RemoveDirectory(parentDirectory)){
					SendErrorMessage(0, QString("Folder containing file '%1'could not be removed").arg(itemToRemove.filePathInRepository));

					return false;
				}
			}
			else {
				if (fileInfo.exists()){
					QFile repositoryFile(itemToRemove.filePathInRepository);

					if (!repositoryFile.remove()){
						SendErrorMessage(0, QString("File '%1' could not be removed. Error status: %2").arg(itemToRemove.filePathInRepository).arg(repositoryFile.errorString()));

						return false;
					}
				}
				else {
					SendInfoMessage(0, QString("File '%1' doesn't exists and therefore cannot not be removed").arg(itemToRemove.filePathInRepository));
				}
			}
		}

		{
			static ChangeSet changes(CF_REMOVED);

			istd::CChangeNotifier changeNotifier(this, &changes);

			// Remove the repository item with the corresponding object-ID:
			m_files.removeAt(fileIndex);

			repositoryDataLocker.unlock();
		}

		imtbase::CObjectCollectionRemoveEvent event(objectId);
		for (imtbase::IObjectCollectionEventHandler* eventHandlerPtr : m_eventHandlerList){
			eventHandlerPtr->OnObjectCollectionEvent(this, &event);
		}

		return true;
	}
	else{
		SendInfoMessage(0, QString("Resource '%1' doesn't exist").arg(qPrintable(objectId)));
	}

	return true;
}


const istd::IChangeable* CFileCollectionComp::GetObjectPtr(const QByteArray& /*objectId*/) const
{
	return nullptr;
}


bool CFileCollectionComp::GetObjectData(const QByteArray& objectId, DataPtr& dataPtr) const
{
	for (const CollectionItem& item : m_files){
		if (item.fileId == objectId){
			istd::TDelPtr<istd::IChangeable> dataObjectPtr(CreateObjectFromFile(item.filePathInRepository, item.typeId));
			if (!dataObjectPtr.IsValid()){
				return false;
			}

			if (!dataPtr.IsValid()){
				istd::TDelPtr<istd::IChangeable> newInstancePtr(CreateDataObject(item.typeId));
				if (newInstancePtr.IsValid()){
					if (newInstancePtr->CopyFrom(*dataObjectPtr)){
						dataPtr.SetPtr(newInstancePtr.PopPtr());

						return true;
					}
				}
			}
			else{
				return dataPtr->CopyFrom(*dataObjectPtr);
			}
		}
	}

	return false;
}


bool CFileCollectionComp::SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const ifile::IFilePersistence* persistencePtr = GetPersistenceForResource(GetObjectTypeId(objectId));
	if (persistencePtr == nullptr){
		return false;
	}

	QStringList extensions;
	if (!persistencePtr->GetFileExtensions(extensions, &object, ifile::IFilePersistence::QF_SAVE)){
		return false;
	}

	QString extension = extensions.isEmpty() ? QString() : extensions[0];

	QString tempFilePath = GetTempDirectory() + "/ImtCore/" + QUuid::createUuid().toString() + "." + extension;

	if (persistencePtr->SaveToFile(object, tempFilePath) == ifile::IFilePersistence::OS_OK){
		bool retVal = UpdateFile(tempFilePath, objectId);

		QFile::remove(tempFilePath);

		return retVal;
	}

	return false;
}


void CFileCollectionComp::SetObjectName(const QByteArray& objectId, const QString& objectName)
{
	QWriteLocker locker(&m_collectionLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		CollectionItem& item = m_files[fileIndex];

		if (item.objectName != objectName){
			int foundIndex = GetFileIndexByName(objectName);
			if (foundIndex >= 0){
				bool isSameResourceType = (m_files[fileIndex].typeId == m_files[foundIndex].typeId);

				if (isSameResourceType || (!isSameResourceType && !*m_useSubfolderAttrPtr)){
					SendErrorMessage(0, QString("Resource with the name '%1' already exists").arg(objectName));

					return;
				}
			}

			istd::CChangeNotifier changeNotifier(this);

			QFileInfo fileInfo(item.filePathInRepository);
			QDir resourceDir = fileInfo.absoluteDir();

			QString resouceDirPath = resourceDir.absolutePath();
			QString targetFolderPath = resouceDirPath;

			QString fileName = fileInfo.fileName();

			QString newPhysicalName = objectName;
#if _WIN32
			QString newFullName = objectName + "." + fileInfo.suffix();
			newPhysicalName = CalculateShortFileName(newFullName, QFileInfo(newFullName), "");
			newPhysicalName = QFileInfo(newPhysicalName).completeBaseName();
#endif
			QString newFileName = newPhysicalName + "." + fileInfo.suffix();

			// Remove old data file:
			QString oldDataItemFilePath = GetDataItemFilePath(item);
			if (!QFile::remove(oldDataItemFilePath)){
				SendWarningMessage(0, QString("Data file '%1' could not be removed").arg(oldDataItemFilePath));
			}

			// Rename meta info file:
			QString oldMetaInfoFilePath = GetMetaInfoFilePath(item);
			QFileInfo metaFileInfo(oldMetaInfoFilePath);
			if (metaFileInfo.exists()){
				QString newMetaInfoFilePath = metaFileInfo.absolutePath() + "/" + newFileName + "." + GetRepositoryInfo().metaInfoFileSuffix;

				if (!QFile::rename(oldMetaInfoFilePath, newMetaInfoFilePath)){
					SendWarningMessage(0, QString("Meta-info file '%1' could not be removed").arg(oldMetaInfoFilePath));

					QFile::remove(oldMetaInfoFilePath);
				}
			}

			if (*m_useSubfolderAttrPtr){
				targetFolderPath = GetCollectionRootFolder();
			
				if (!item.typeId.isEmpty()){
					Q_ASSERT(m_resourceTypesCompPtr.IsValid());

					if (m_resourceTypesCompPtr->GetOptionsCount() > 1){
						targetFolderPath += "/" + QString(item.typeId.constData());
					}
				}

				targetFolderPath += "/" + newPhysicalName;

				if (!resourceDir.rename(resouceDirPath, targetFolderPath)){
					SendErrorMessage(0, QString("Resource path could not be renamed from '%1' into '%2'. Resource could not be renamed to '%3'").arg(resouceDirPath).arg(targetFolderPath).arg(objectName));

					locker.unlock();

					return;
				}
			}

			QString newFileRepositoryPath = targetFolderPath + "/" + newFileName;
			QFile::rename(targetFolderPath + "/" + fileName, newFileRepositoryPath);

	
			item.objectName = objectName;
			item.filePathInRepository = newFileRepositoryPath;

			SaveCollectionItem(item);

			locker.unlock();

			imtbase::CObjectCollectionUpdateEvent event(objectId, imtbase::CObjectCollectionUpdateEvent::UT_NAME);
			for (imtbase::IObjectCollectionEventHandler* eventHandlerPtr : m_eventHandlerList){
				eventHandlerPtr->OnObjectCollectionEvent(this, &event);
			}

			return;
		}

		return;
	}

	SendErrorMessage(0, QString("Resource with the ID '%1' doesn't exist").arg(objectId.constData()));
}


void CFileCollectionComp::SetObjectDescription(const QByteArray& objectId, const QString& objectDescription)
{
	QWriteLocker locker(&m_collectionLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		CollectionItem& item = m_files[fileIndex];

		QString oldDescription = item.metaInfo.GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION).toString();

		if (oldDescription != objectDescription){
			istd::CChangeNotifier changeNotifier(this);

			item.metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION, objectDescription);

			SaveCollectionItem(item);

			locker.unlock();

			imtbase::CObjectCollectionUpdateEvent event(objectId, imtbase::CObjectCollectionUpdateEvent::UT_DESCRIPTION);
			for (imtbase::IObjectCollectionEventHandler* eventHandlerPtr : m_eventHandlerList){
				eventHandlerPtr->OnObjectCollectionEvent(this, &event);
			}
		}
	}
}


void CFileCollectionComp::SetObjectEnabled(const QByteArray& /*objectId*/, bool /*isEnabled*/)
{
}


bool CFileCollectionComp::RegisterEventHandler(imtbase::IObjectCollectionEventHandler* eventHandler)
{
	if (!m_eventHandlerList.contains(eventHandler)){
		m_eventHandlerList.append(eventHandler);

		eventHandler->OnCollectionConnected(this);

		return true;
	}

	return false;
}


bool CFileCollectionComp::UnregisterEventHandler(imtbase::IObjectCollectionEventHandler* eventHandler)
{
	int index = m_eventHandlerList.indexOf(eventHandler, 0);
	if (index >= 0){
		eventHandler->OnCollectionDisconnected(this);

		m_eventHandlerList.removeAt(index);

		return true;
	}

	return false;
}


// reimplemented (IObjectCollectionInfo)

bool CFileCollectionComp::GetCollectionItemMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& metaInfo) const
{
	QReadLocker lockCollection(&m_collectionLock);

	// Looking for file item:
	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex < 0){
		SendVerboseMessage(QString("Collection item doesn't exist for the given object-ID (%1). Meta-information could not be provided").arg(objectId.constData()), "File Collection");

		return false;
	}

	// Get meta-information from cache:
	CollectionItem& item = m_files[fileIndex];

	return metaInfo.CopyFrom(item.metaInfo);
}


const iprm::IOptionsList* CFileCollectionComp::GetObjectTypesInfo() const
{
	return &m_resourceTypeConstraints;
}


imtbase::IObjectCollectionInfo::Id CFileCollectionComp::GetObjectTypeId(const QByteArray & objectId) const
{
	m_collectionLock.lockForRead();

	QByteArray retVal;

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		retVal = m_files[fileIndex].typeId;
	}

	m_collectionLock.unlock();

	return retVal;
}


// reimplemented (ICollectionInfo)

imtbase::ICollectionInfo::Ids CFileCollectionComp::GetElementIds() const
{
	Ids retVal;

	m_collectionLock.lockForRead();

	for (int fileIndex = 0; fileIndex < m_files.count(); ++fileIndex){
		retVal.append(m_files[fileIndex].fileId);
	}

	m_collectionLock.unlock();

	return retVal;
}


QVariant CFileCollectionComp::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	QReadLocker locker(&m_collectionLock);

	int fileIndex = GetFileIndexById(elementId);
	if (fileIndex >= 0){
		const CollectionItem& item = m_files[fileIndex];

		switch (infoType){
		case EIT_NAME:
			return item.objectName;
			break;

		case EIT_DESCRIPTION:
			return item.metaInfo.GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION).toString();
			break;

		case EIT_TYPE_ID:
			return item.typeId;
			break;
		}
	}

	return QString();
}


// reimplemented (istd::IChangeable)

int CFileCollectionComp::GetSupportedOperations() const
{
	return SO_COPY | SO_RESET;
}


bool CFileCollectionComp::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CFileCollectionComp* sourcePtr = dynamic_cast<const CFileCollectionComp*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		{
			QWriteLocker locker(&m_collectionLock);

			m_files = sourcePtr->m_files;
		}

		return true;
	}

	return false;
}


bool CFileCollectionComp::ResetData(CompatibilityMode /*mode*/)
{
	if (!m_files.isEmpty()){
		istd::CChangeNotifier changeNotifier(this);

		m_files.clear();
	}

	return true;
}


// static protected methods

CFileCollectionComp::RepositoryInfo CFileCollectionComp::GetRepositoryInfo()
{
	RepositoryInfo retVal;

	retVal.metaInfoFileSuffix = "meta";
	retVal.dataFileSuffix = "item";

	return retVal;
}


// protected methods

istd::IChangeable* CFileCollectionComp::CreateDataObject(const QByteArray& typeId) const
{
	int factoryIndex = -1;

	if (m_resourceTypesCompPtr.IsValid()){
		for (int i = 0; i < m_resourceTypesCompPtr->GetOptionsCount(); ++i){
			if (typeId == m_resourceTypesCompPtr->GetOptionId(i)){
				factoryIndex = i;
				break;
			}
		}
	}

	if ((factoryIndex >= 0) && factoryIndex < m_objectFactoryListCompPtr.GetCount()){
		return m_objectFactoryListCompPtr.CreateInstance(factoryIndex);
	}

	return nullptr;
}


const ifile::IFilePersistence* CFileCollectionComp::GetPersistenceForResource(const QByteArray& typeId) const
{
	int factoryIndex = -1;

	if (m_resourceTypesCompPtr.IsValid()){
		for (int i = 0; i < m_resourceTypesCompPtr->GetOptionsCount(); ++i){
			if (typeId == m_resourceTypesCompPtr->GetOptionId(i)){
				factoryIndex = i;
				break;
			}
		}
	}

	if ((factoryIndex >= 0) && factoryIndex < m_objectPersistenceListCompPtr.GetCount()){
		return m_objectPersistenceListCompPtr[factoryIndex];
	}

	return nullptr;
}


int CFileCollectionComp::GetFileIndexById(const QByteArray& fileId) const
{
	if (fileId.isEmpty()){
		return -1;
	}

	for (int i = 0; i < m_files.count(); ++i){
		if (m_files[i].fileId == fileId){
			return i;
		}
	}

	return -1;
}


int CFileCollectionComp::GetCollectionItemById(const QByteArray& fileId, CollectionItem& item) const
{
	if (fileId.isEmpty()){
		return -1;
	}

	for (int i = 0; i < m_files.count(); ++i){
		if (m_files[i].fileId == fileId){
			item.CopyFrom(m_files[i]);

			return i;
		}
	}

	return -1;
}


int CFileCollectionComp::GetFileIndexByName(const QString& fileName) const
{
	for (int i = 0; i < m_files.count(); ++i){
		if (m_files[i].objectName == fileName){
			return i;
		}
	}

	return -1;
}


istd::IChangeable* CFileCollectionComp::CreateObjectFromFile(const QString& filePath, const QByteArray& typeId) const
{
	istd::IChangeable* retVal = CreateDataObject(typeId);
	if (retVal != nullptr){
		const ifile::IFilePersistence* filePersistenceCompPtr = GetPersistenceForResource(typeId);
		if (filePersistenceCompPtr != nullptr){
			int loadState = filePersistenceCompPtr->LoadFromFile(*retVal, filePath);
			if (loadState == ifile::IFilePersistence::OS_OK){
				return retVal;
			}
		}
	}

	return nullptr;
}


imtbase::IMetaInfoCreator::MetaInfoPtr CFileCollectionComp::CreateFileContentsMetaInfo(const QString& filePath, const QByteArray& typeId) const
{
	imtbase::IMetaInfoCreator::MetaInfoPtr retVal;

	QFileInfo fileInfo(filePath);
	if (fileInfo.exists()){
		if (m_metaInfoCreatorMap.contains(typeId)){
			istd::TDelPtr<istd::IChangeable> dataObjectPtr(CreateObjectFromFile(filePath, typeId));
			if (!dataObjectPtr.IsValid()){
				return retVal;
			}

			m_metaInfoCreatorMap[typeId]->CreateMetaInfo(dataObjectPtr.GetPtr(), typeId, retVal);
		}

		if (!retVal.IsValid()){
			retVal.SetPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>);
		}

		Q_ASSERT(retVal.IsValid());

		retVal->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, fileInfo.lastModified());

		if (m_isCalculateCheckSumAttrPtr.IsValid()){
			if (*m_isCalculateCheckSumAttrPtr){
				retVal->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CONTENT_CHECKSUM, istd::CCrcCalculator::GetCrcFromFile(filePath));
			}
		}
	}

	return retVal;
}


void CFileCollectionComp::UpdateItemMetaInfo(CollectionItem& item) const
{
	QString metaInfoFilePath = GetMetaInfoFilePath(item);
	QFileInfo fileInfo(metaInfoFilePath);

	bool retVal = false;
	if (item.contentsMetaInfoPtr.IsValid()){
		retVal = LoadFileMetaInfo(*item.contentsMetaInfoPtr, metaInfoFilePath);
	}

	if (!retVal){
		item.contentsMetaInfoPtr = CreateFileContentsMetaInfo(item.filePathInRepository, item.typeId);
		if (!item.contentsMetaInfoPtr.IsValid()){
			SendErrorMessage(0, QString("Meta-information for the file '%1' could not be created. Meta-information could not be provided").arg(metaInfoFilePath), "File Collection");
		}
		else{
			bool isSuccessfulySaved = SaveMetaInfo(*item.contentsMetaInfoPtr, metaInfoFilePath);
			if (!isSuccessfulySaved){
				SendWarningMessage(0, QString("Meta-information for the file '%1' created, but could not be saved to file").arg(metaInfoFilePath), "File Collection");
			}
		}
	}
}


bool CFileCollectionComp::SaveMetaInfo(const idoc::IDocumentMetaInfo& metaInfo, const QString& filePath) const
{
	const iser::ISerializable* serializablePtr = dynamic_cast<const iser::ISerializable*>(&metaInfo);
	if (serializablePtr != nullptr){
		ifile::CCompactXmlFileWriteArchive archive(filePath, m_versionInfoCompPtr.GetPtr());

		return (const_cast<iser::ISerializable*>(serializablePtr))->Serialize(archive);
	}

	return false;
}


bool CFileCollectionComp::LoadFileMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QString& filePath) const
{
	if (QFile::exists(filePath)){
		iser::ISerializable* serializablePtr = dynamic_cast<iser::ISerializable*>(&metaInfo);
		if (serializablePtr != nullptr){
			ifile::CCompactXmlFileReadArchive archive(filePath, m_versionInfoCompPtr.GetPtr());

			return serializablePtr->Serialize(archive);
		}
	}

	return false;
}


QString CFileCollectionComp::CalculateFolderPathInRepository(
			const QString& localFilePath,
			const QString& objectName,
			const QByteArray& typeId,
			bool useSubfolder,
			bool useNameCounting,
			ilog::IMessageConsumer* messageConsumerPtr) const
{
	QFileInfo inputFileInfo(localFilePath);
	if (!inputFileInfo.isFile()){
		SendErrorMessage(0 , QString("Path '%1' is not a valid path to an existing file").arg(localFilePath));

		return QString();
	}

	QString targetDirPath = inputFileInfo.absoluteDir().absolutePath();

	if (m_repositoryPathCompPtr.IsValid()){
		QString repositoryDirPath = m_repositoryPathCompPtr->GetPath();
		if (repositoryDirPath.isEmpty()){
			return QString();
		}

		targetDirPath = repositoryDirPath;

		// If object-ID is non empty, create subfolder for this:
		if (!typeId.isEmpty()){
			Q_ASSERT(m_resourceTypesCompPtr.IsValid());

			if (m_resourceTypesCompPtr->GetOptionsCount() > 1){
				targetDirPath += QString("/") + typeId.constData();
			}
		}

		if (useSubfolder){
			QString subfolderName = objectName;
#ifdef Q_OS_WIN
			if (inputFileInfo.fileName() == objectName){
				subfolderName = inputFileInfo.completeBaseName();
			}
#endif
			targetDirPath += QString("/") + subfolderName;

			if (useNameCounting){
				int count = 0;
				QString newDirPath = targetDirPath;
				while (QFileInfo::exists(newDirPath)){
					newDirPath = QString("%1 - %2").arg(targetDirPath).arg(++count);
				}

				if (newDirPath != targetDirPath){
					QString warning = tr("Input file name %1 renamed to %2").arg(QDir(targetDirPath).dirName()).arg(QDir(newDirPath).dirName());

					SendWarningMessage(0, warning);

					if (messageConsumerPtr != nullptr){
						messageConsumerPtr->AddMessage(ilog::IMessageConsumer::MessagePtr(new ilog::CMessage(istd::IInformationProvider::IC_WARNING, 0, warning, "File Collection")));
					}
				}

				targetDirPath = newDirPath;
			}
		}
	}

	return targetDirPath;
}


QString CFileCollectionComp::CalculateTargetFilePath(
			const QString& targetFolderPath,
			const QString& localFilePath,
			bool useSubfolder) const
{
	QFileInfo inputFileInfo(localFilePath);

	QString targetFilePath;

	Q_ASSERT(inputFileInfo.fileName().size() <= 255); // would be an invalid filename!

	if (useSubfolder){
		QString targetFileName = inputFileInfo.fileName();
		QString dirName = QDir(targetFolderPath).dirName();
		if (!dirName.isEmpty() && (dirName != ".")){
			targetFileName = dirName + "." + inputFileInfo.suffix();
		}

#ifdef Q_OS_WIN
		targetFileName = CalculateShortFileName(targetFileName, inputFileInfo, "");
#endif
		targetFilePath = targetFolderPath + QString("/") + targetFileName;
	}
	else{
		QString uuidPrefix = QUuid::createUuid().toString() + QString(" - ");
		QString targetFileName = uuidPrefix + inputFileInfo.fileName();

#ifdef Q_OS_WIN
		targetFileName = CalculateShortFileName(targetFileName, inputFileInfo, uuidPrefix);
#endif
		targetFilePath = targetFolderPath + QString("/") + targetFileName;
	}

	return targetFilePath;
}


bool CFileCollectionComp::LoadRevisionsContents(const QByteArray& objectId, RevisionsContents& revisionsContents) const
{
	QReadLocker locker(&m_collectionLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		CollectionItem& item = m_files[fileIndex];
		QFileInfo info(item.filePathInRepository);
		QString revisionArchiveContentsPath = info.absolutePath() + "/Revisions/contents.xml";

		if (!QFile::exists(revisionArchiveContentsPath)){
			SendErrorMessage(0, QString(QObject::tr("Revision contents file '%1' not exists")).arg(revisionArchiveContentsPath));
			return false;
		}

		ifile::CCompactXmlFileReadArchive archive(revisionArchiveContentsPath, m_versionInfoCompPtr.GetPtr());
		return revisionsContents.Serialize(archive);
	}

	return false;
}


bool CFileCollectionComp::SaveRevisionsContents(const QByteArray& objectId, RevisionsContents& revisionsContents) const
{
	QReadLocker locker(&m_collectionLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		CollectionItem& item = m_files[fileIndex];
		QFileInfo info(item.filePathInRepository);
		QString revisionsPath = info.absolutePath() + "/Revisions";

		if (!istd::CSystem::EnsurePathExists(revisionsPath)){
			SendErrorMessage(0, QString(QObject::tr("Target folder '%1' could not be created")).arg(revisionsPath));
			return false;
		}

		ifile::CCompactXmlFileWriteArchive archive(revisionsPath + "/contents.xml", m_versionInfoCompPtr.GetPtr());
		return revisionsContents.Serialize(archive);
	}

	return false;
}


bool CFileCollectionComp::CreateRevisionsContents(const QByteArray& objectId) const
{
	if (m_compressorCompPtr.IsValid()){
		QReadLocker locker(&m_collectionLock);

		int fileIndex = GetFileIndexById(objectId);
		if (fileIndex >= 0){
			CollectionItem& item = m_files[fileIndex];
			QFileInfo info(item.filePathInRepository);
			QString revisionsPath = info.absolutePath() + "/Revisions";
			QDir revisionsDir(revisionsPath);

			if (!revisionsDir.exists()){
				return false;
			}

			QFileInfoList fileList;
			ifile::CFileListProviderComp::CreateFileList(revisionsDir, 0, 0, QStringList() << "*.zip", QDir::Time | QDir::Reversed, fileList);

			QString tempPath = GetTempDirectory() + "/ImtCore/" + QUuid::createUuid().toString();
			QDir tempDir(tempPath);

			RevisionsContents revisionsContents;

			for (int i = 0; i < fileList.count(); i++){
				QString revisionPath = tempPath + "/" + fileList[i].fileName();

				if (!istd::CSystem::EnsurePathExists(revisionPath)){
					SendErrorMessage(0, QString(QObject::tr("Target folder '%1' could not be created")).arg(revisionPath));
					QDir(revisionPath).removeRecursively();
					continue;
				}

				if (!m_compressorCompPtr->DecompressFolder(fileList[i].absoluteFilePath(), revisionPath)){
					SendErrorMessage(0, QString(QObject::tr("Revision '%1' could not be decompressed")).arg(fileList[i].absoluteFilePath()));
					QDir(revisionPath).removeRecursively();
					continue;
				}

				QString revisionMetaInfoPath = revisionPath + "/revision.meta";
				if (QFile::exists(revisionMetaInfoPath)){
					RevisionMetaInfo revisionMetaInfo;
					ifile::CCompactXmlFileReadArchive archive(revisionMetaInfoPath, m_versionInfoCompPtr.GetPtr());
					if (revisionMetaInfo.Serialize(archive)){
						RevisionsContentsItem revisionsContentsItem;
						*dynamic_cast<RevisionInfo*>(&revisionsContentsItem) = *dynamic_cast<RevisionInfo*>(&revisionMetaInfo);
						revisionsContentsItem.path = revisionsDir.relativeFilePath(fileList[i].absoluteFilePath());
						
						revisionsContents[revisionMetaInfo.revision] = revisionsContentsItem;
					}
				}

				QDir(revisionPath).removeRecursively();
			}

			tempDir.removeRecursively();

			return SaveRevisionsContents(objectId, revisionsContents);
		}
	}

	return false;
}


// reimplemented (icomp::CComponentBase)

void CFileCollectionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	for (int i = 0; i < m_metaInfoCreatorListCompPtr.GetCount(); ++i){
		const imtbase::IMetaInfoCreator* metaInfoCreatorPtr = m_metaInfoCreatorListCompPtr[i];
		if (metaInfoCreatorPtr != nullptr){
			QByteArrayList supportedtypeIds = metaInfoCreatorPtr->GetSupportedTypeIds();
			for (const QByteArray& typeId : supportedtypeIds){
				m_metaInfoCreatorMap[typeId] = metaInfoCreatorPtr;
			}
		}
	}

	QString path;

	// Ensure, that the repository folder was created:
	if (m_repositoryPathCompPtr.IsValid()){
		Q_ASSERT(m_repositoryPathCompPtr->GetPathType() == ifile::IFileNameParam::PT_DIRECTORY);

		path = m_repositoryPathCompPtr->GetPath();
		if (!path.isEmpty()){
			if (!istd::CSystem::EnsurePathExists(path)){
				SendCriticalMessage(0, QString("Root folder for the file collection could not be created in '%1'").arg(path));
			}
		}
	}

	if (*m_asynchronousReadingAttrPtr){
		StartReader();
	}
	else{
		SyncRead();

		if (*m_pollFileSystemAttrPtr && !path.isEmpty()){
			connect(&m_syncTimer, &QTimer::timeout, this, &CFileCollectionComp::OnSync);
			m_syncTimer.start(*m_pollingPeriodAttrPtr * 1000);
		}
	}

	if (m_eventHandlerListCompPtr.IsValid()){
		for (int i = 0; i < m_eventHandlerListCompPtr.GetCount(); i++){
			RegisterEventHandler(m_eventHandlerListCompPtr[i]);
		}
	}
}


void CFileCollectionComp::OnComponentDestroyed()
{
	m_eventHandlerList.clear();

	m_readerThread.requestInterruption();
	m_readerThread.wait();

	m_collectionLock.lockForWrite();
	m_files.clear();
	m_collectionLock.unlock();

	BaseClass::OnComponentDestroyed();
}


// private methods

bool CFileCollectionComp::InsertFileIntoCollection(
			const QString& filePath,
			const QString& objectName,
			const QByteArray& typeId,
			ilog::IMessageConsumer* messageConsumerPtr,
			QString& filePathInRepository)
{
	QFileInfo inputFileInfo(filePath);
	if (!inputFileInfo.exists()){
		SendErrorMessage(0, QString("Input file '%1' doesn't exist").arg(filePath));

		return false;
	}

	QString targetFolderPath = CalculateFolderPathInRepository(filePath, objectName, typeId, *m_useSubfolderAttrPtr, true, messageConsumerPtr);
	if (targetFolderPath.isEmpty()){
		SendErrorMessage(0, QString("Collection folder for the input file '%1'could not be calculated").arg(filePath));

		return false;
	}

	QFileInfo targetFolderInfo(targetFolderPath);
	if (targetFolderInfo.exists() && *m_useSubfolderAttrPtr){
		QString errorMessage = QString("File could not be added. Resource with the same name already exists %1").arg(targetFolderPath);

		SendErrorMessage(0, errorMessage);

		if (messageConsumerPtr != nullptr){
			messageConsumerPtr->AddMessage(ilog::IMessageConsumer::MessagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0,  errorMessage, "File Collection")));
		}

		return false;
	}

	if (!targetFolderInfo.exists()){
		QDir targetFolder(targetFolderPath);

		if (!targetFolder.mkpath(targetFolderPath)){
			SendErrorMessage(0, QString("File repository could not be set up. Collection directory '%1' could not be created").arg(targetFolderPath));

			return false;
		}
	}

	filePathInRepository = CalculateTargetFilePath(targetFolderPath, filePath, *m_useSubfolderAttrPtr);

	if (!istd::CSystem::FileCopy(filePath, filePathInRepository)){
		SendErrorMessage(0, QString("File '%1' could not be copied to %2").arg(filePath).arg(filePathInRepository));

		return false;
	}

	if (!QFile::setPermissions(filePathInRepository,
		QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner |
		QFile::ReadUser | QFile::WriteUser | QFile::ExeUser |
		QFile::ReadGroup | QFile::WriteGroup | QFile::ExeGroup |
		QFile::ReadOther | QFile::WriteOther | QFile::ExeOther)){
		SendErrorMessage(0, QString("Permissions for the file '%1' could not be set").arg(filePathInRepository));
	}

	return true;
}


QString CFileCollectionComp::SaveCollectionItem(const CollectionItem& collectionItem) const
{
	DirectoryBlocker blocker(*this);

	QString itemFilePath = GetDataItemFilePath(collectionItem);

	ifile::CCompactXmlFileWriteArchive archive(itemFilePath, m_versionInfoCompPtr.GetPtr());

	if (!const_cast<CollectionItem&>(collectionItem).Serialize(archive)){
		SendErrorMessage(0, QString("Collection item could not be saved into '%1'").arg(itemFilePath));

		return QString();
	}

	return itemFilePath;
}


QString CFileCollectionComp::GetTempDirectory() const
{
	return QDir::tempPath();
}


QString CFileCollectionComp::GetDataItemFilePath(const CollectionItem& repositoryFile) const
{
	QString subFolder = *m_useSubfolderAttrPtr ? repositoryFile.objectName : QString();
	QString itemFilePath;

	QString baseName = QFileInfo(repositoryFile.filePathInRepository).fileName();
	if (baseName.isEmpty()){
		return QString();
	}

	QFileInfo fileInfo(repositoryFile.filePathInRepository);

	itemFilePath = fileInfo.absolutePath() + "/" + baseName + "." + GetRepositoryInfo().dataFileSuffix;

	return itemFilePath;
}


QString CFileCollectionComp::GetMetaInfoFilePath(const CollectionItem& repositoryFile) const
{
	QString subFolder = *m_useSubfolderAttrPtr ? repositoryFile.objectName : QString();
	QString itemFilePath;

	QString baseName = QFileInfo(repositoryFile.filePathInRepository).fileName();
	if (baseName.isEmpty()){
		return QString();
	}

	QFileInfo fileInfo(repositoryFile.filePathInRepository);

	itemFilePath = fileInfo.absolutePath() + "/" + baseName + "." + GetRepositoryInfo().metaInfoFileSuffix;

	return itemFilePath;
}


QString CFileCollectionComp::CalculateShortFileName(const QString& fileName, const QFileInfo& fileInfo, const QString& prefix) const
{
	const int maxFileNameLength = 255;

	int fileSuffixSize = GetRepositoryInfo().dataFileSuffix.size() + 1;

	if (fileName.size() + fileSuffixSize > maxFileNameLength){
		int reducedNameSize = maxFileNameLength - fileSuffixSize - prefix.size();

		QString reducedFileName = fileInfo.fileName().mid(0, reducedNameSize - (fileInfo.suffix().size() + 1));
		reducedFileName.chop(1);
		reducedFileName.append("~.").append(fileInfo.suffix());
		reducedFileName.prepend(prefix);

		Q_ASSERT(reducedFileName.size() <= maxFileNameLength - fileSuffixSize);

		return reducedFileName;
	}
	else{
		return fileName;
	}
}


void CFileCollectionComp::SyncRead()
{
	istd::CChangeNotifier changeNotifier(this, &istd::IChangeable::GetAllChanges());

	QFileInfoList fileList;
	GetRepositoryFileList(fileList);

	{
		QWriteLocker locker(&m_collectionLock);
		m_files.clear();
		for (int i = 0; i < fileList.count(); ++i){
			ReadItem(m_files, fileList[i].absoluteFilePath());
		}
	}
}


void CFileCollectionComp::GetRepositoryFileList(QFileInfoList& fileList)
{
	QString repositoryRootPath = GetCollectionRootFolder();
	QDir repositoryRootDir(repositoryRootPath);

	ifile::CFileListProviderComp::CreateFileList(repositoryRootDir, 0, 2, QStringList() << QString("*.%1").arg(GetRepositoryInfo().dataFileSuffix), QDir::Name, fileList);
}


void CFileCollectionComp::ReadItem(Files& filesPtr, const QString& itemFilePath)
{
	ifile::CCompactXmlFileReadArchive archive(itemFilePath, m_versionInfoCompPtr.GetPtr());
	CollectionItem fileItem(GetCollectionRootFolder());

	if (!fileItem.Serialize(archive)){
		SendErrorMessage(0, QString("Collection item could not be loaded from '%1'").arg(itemFilePath));
		return;
	}

	QFileInfo repositoryFileInfo(fileItem.filePathInRepository);
	if (repositoryFileInfo.exists()){
		if (!fileItem.contentsMetaInfoPtr.IsValid()){
			imtbase::IMetaInfoCreator::MetaInfoPtr retVal;

			if (m_metaInfoCreatorMap.contains(fileItem.typeId)){
				m_metaInfoCreatorMap[fileItem.typeId]->CreateMetaInfo(nullptr, fileItem.typeId, retVal);
			}

			if (!retVal.IsValid()){
				retVal.SetPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>);
			}

			Q_ASSERT(retVal.IsValid());

			fileItem.contentsMetaInfoPtr = retVal;
		}

		if (fileItem.contentsMetaInfoPtr.IsValid()){
			LoadFileMetaInfo(*fileItem.contentsMetaInfoPtr, GetMetaInfoFilePath(fileItem));

			filesPtr.push_back(fileItem);
		}
	}
	else{
		SendErrorMessage(0, QString("File '%1' doesn't exist. Collection item was automatically removed").arg(fileItem.filePathInRepository));
	}
}


void CFileCollectionComp::StartReader()
{
	if (m_progressManagerListCompPtr.IsValid()){
		for (int i = 0; i < m_progressManagerListCompPtr.GetCount(); i++){
			m_progressManagerListCompPtr[i]->BeginProgressSession("FileCollection", "Start reading file collection");
		}
	}

	m_readerThread.start();
}


void CFileCollectionComp::OnReaderProgress(int progress)
{
	if (m_progressManagerListCompPtr.IsValid()){
		for (int i = 0; i < m_progressManagerListCompPtr.GetCount(); i++){
			m_progressManagerListCompPtr[i]->OnProgress(0, progress);
		}
	}
}


void CFileCollectionComp::OnReaderFinished()
{
	istd::CChangeNotifier changeNotifier(this, &istd::IChangeable::GetAllChanges());

	QWriteLocker locker(&m_collectionLock);
	m_files.append(m_readerFiles);
	locker.unlock();
	
	if (m_progressManagerListCompPtr.IsValid()){
		for (int i = 0; i < m_progressManagerListCompPtr.GetCount(); i++){
			m_progressManagerListCompPtr[i]->EndProgressSession(0);
		}
	}
}


void CFileCollectionComp::OnReaderInterrupted()
{
}


// private slots

void CFileCollectionComp::OnSync()
{
	if (!m_directoryBlocked){
		SyncRead();
	}
}


// public methods of the embedded class ResourceTypeConstraints

CFileCollectionComp::ResourceTypeConstraints::ResourceTypeConstraints()
	:m_parentPtr(nullptr)
{
}


void CFileCollectionComp::ResourceTypeConstraints::SetParent(CFileCollectionComp* parentPtr)
{
	m_parentPtr = parentPtr;
}


// reimplemented (IFileResourceTypeConstraints)

const ifile::IFileTypeInfo* CFileCollectionComp::ResourceTypeConstraints::GetFileTypeInfo(int typeIndex) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());
	Q_ASSERT(typeIndex >= 0);

	if (typeIndex < m_parentPtr->m_resourceFileTypesCompPtr.GetCount()){
		return m_parentPtr->m_resourceFileTypesCompPtr[typeIndex];
	}

	return nullptr;
}


// reimplemented (iprm::IOptionsList)

int CFileCollectionComp::ResourceTypeConstraints::GetOptionsFlags() const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionsFlags();
}


int CFileCollectionComp::ResourceTypeConstraints::GetOptionsCount() const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionsCount();
}


QString CFileCollectionComp::ResourceTypeConstraints::GetOptionName(int index) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionName(index);
}


QString CFileCollectionComp::ResourceTypeConstraints::GetOptionDescription(int index) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionDescription(index);
}


QByteArray CFileCollectionComp::ResourceTypeConstraints::GetOptionId(int index) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionId(index);
}


bool CFileCollectionComp::ResourceTypeConstraints::IsOptionEnabled(int index) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->IsOptionEnabled(index);
}


// public methods of embedded class ReaderThread

CFileCollectionComp::ReaderThread::ReaderThread(CFileCollectionComp* parentPtr)
	:m_parentPtr(parentPtr)
{
}


// private methods of embedded class ReaderThread

// reimplemented (QThread)

void CFileCollectionComp::ReaderThread::run()
{
	QFileInfoList fileList;
	m_parentPtr->GetRepositoryFileList(fileList);

	int progress = 0;

	for (int i = 0; i < fileList.count(); i++){
		if (isInterruptionRequested()){
			break;
		}

		m_parentPtr->ReadItem(m_parentPtr->m_readerFiles, fileList[i].absoluteFilePath());

		int currentProgress = i * 100 / fileList.count();
		if (progress != currentProgress){
			progress = currentProgress;
			QMetaObject::invokeMethod(m_parentPtr, "OnReaderProgress", Qt::QueuedConnection, Q_ARG(int, progress));
		}
	}

	if (isInterruptionRequested()){
		QMetaObject::invokeMethod(m_parentPtr, "OnReaderInterrupted", Qt::QueuedConnection);
	}
	else{
		QMetaObject::invokeMethod(m_parentPtr, "OnReaderFinished", Qt::QueuedConnection);
	}
}


// public methods of embedded class CollectionItem

// reimplement (iser::ISerializable)

bool CFileCollectionComp::CollectionItem::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	static iser::CArchiveTag fileIdTag("FileID", "ID of the file in the repository");
	retVal = retVal && archive.BeginTag(fileIdTag);
	retVal = retVal && archive.Process(fileId);
	retVal = retVal && archive.EndTag(fileIdTag);

	// Change file path in the repository to a relative path (relvative to repository's folder):
	QDir repositoryDir(m_repositoryFolderPath);
	QString relativeFilePathInRepository = repositoryDir.relativeFilePath(filePathInRepository);

	static iser::CArchiveTag filePathInrepositoryItemsTag("RepositoryFilePath", "File path in the repository");
	retVal = retVal && archive.BeginTag(filePathInrepositoryItemsTag);
	retVal = retVal && archive.Process(relativeFilePathInRepository);
	retVal = retVal && archive.EndTag(filePathInrepositoryItemsTag);

	if (retVal && !archive.IsStoring()){
		filePathInRepository = QDir::cleanPath(repositoryDir.absoluteFilePath(relativeFilePathInRepository));
	}

	QString relativeSourceFilePath = repositoryDir.relativeFilePath(sourceFilePath);

	static iser::CArchiveTag sourceFilePathTag("SourceFilePath", "Source location of the file");
	retVal = retVal && archive.BeginTag(sourceFilePathTag);
	retVal = retVal && archive.Process(relativeSourceFilePath);
	retVal = retVal && archive.EndTag(sourceFilePathTag);

	if (retVal && !archive.IsStoring()){
		sourceFilePath = QDir::cleanPath(repositoryDir.absoluteFilePath(relativeSourceFilePath));
	}

	static iser::CArchiveTag resourceNameTag("Name", "Name of the file resource");
	retVal = retVal && archive.BeginTag(resourceNameTag);
	retVal = retVal && archive.Process(objectName);
	retVal = retVal && archive.EndTag(resourceNameTag);

	static iser::CArchiveTag resourceTypeIdTag("TypeID", "Type ID of the file resource");
	retVal = retVal && archive.BeginTag(resourceTypeIdTag);
	retVal = retVal && archive.Process(typeId);
	retVal = retVal && archive.EndTag(resourceTypeIdTag);

	static iser::CArchiveTag metaInfoTag("MetaInfo", "Collection related meta-informations", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(metaInfoTag);
	retVal = retVal && metaInfo.Serialize(archive);
	retVal = retVal && archive.EndTag(metaInfoTag);

	return retVal;
}


// reimplement (istd::IChangeable)

bool CFileCollectionComp::CollectionItem::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CollectionItem* sourceItemPtr = dynamic_cast<const CollectionItem*>(&object);
	if (sourceItemPtr != nullptr){
		fileId = sourceItemPtr->fileId;
		filePathInRepository = sourceItemPtr->filePathInRepository;
		sourceFilePath = sourceItemPtr->sourceFilePath;
		objectName = sourceItemPtr->objectName;
		typeId = sourceItemPtr->typeId;

		if (sourceItemPtr->contentsMetaInfoPtr.IsValid()){
			contentsMetaInfoPtr.SetCastedOrRemove(sourceItemPtr->contentsMetaInfoPtr->CloneMe());
			if (!contentsMetaInfoPtr.IsValid()){
				return false;
			}
		}

		return metaInfo.CopyFrom(sourceItemPtr->metaInfo);
	}

	return false;
}


// public methods of embedded class RevisionMetaInfo

// reimplement (iser::ISerializable)

bool CFileCollectionComp::RevisionMetaInfo::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	static iser::CArchiveTag revisionTag("Revision", "File revision");
	retVal = retVal && archive.BeginTag(revisionTag);
	retVal = retVal && archive.Process(revision);
	retVal = retVal && archive.EndTag(revisionTag);

	static iser::CArchiveTag timestampTag("Timestamp", "Revision timestamp");
	retVal = retVal && archive.BeginTag(timestampTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, timestamp);
	retVal = retVal && archive.EndTag(timestampTag);

	static iser::CArchiveTag userTag("User", "User");
	retVal = retVal && archive.BeginTag(userTag);
	retVal = retVal && archive.Process(user);
	retVal = retVal && archive.EndTag(userTag);

	static iser::CArchiveTag commentTag("Comment", "Comment");
	retVal = retVal && archive.BeginTag(commentTag);
	retVal = retVal && archive.Process(comment);
	retVal = retVal && archive.EndTag(commentTag);

	return retVal;
}


// public methods of embedded class RevisionArchiveContents

// reimplement (iser::ISerializable)

bool CFileCollectionComp::RevisionsContents::Serialize(iser::IArchive& archive)
{
	int revisionCount = count();

	if (!archive.IsStoring()){
		revisionCount = 0;

		clear();
	}

	static iser::CArchiveTag revisionListTag("RevisionList", "List of revisions", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag revisionItemTag("RevisionItem", "Revision item", iser::CArchiveTag::TT_GROUP, &revisionListTag);

	bool retVal = true;

	retVal = retVal && archive.BeginMultiTag(revisionListTag, revisionItemTag, revisionCount);

	QList<int> keys(keys());

	for (int i = 0; i < revisionCount; i++){
		RevisionsContentsItem revisionsContentsItem;

		if (archive.IsStoring()){
			revisionsContentsItem = value(keys[i]);
		}

		retVal = retVal && archive.BeginTag(revisionItemTag);

		static iser::CArchiveTag revisionTag("Revision", "File revision");
		retVal = retVal && archive.BeginTag(revisionTag);
		retVal = retVal && archive.Process(revisionsContentsItem.revision);
		retVal = retVal && archive.EndTag(revisionTag);

		static iser::CArchiveTag timestampTag("Timestamp", "Revision timestamp");
		retVal = retVal && archive.BeginTag(timestampTag);
		retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, revisionsContentsItem.timestamp);
		retVal = retVal && archive.EndTag(timestampTag);

		static iser::CArchiveTag userTag("User", "User");
		retVal = retVal && archive.BeginTag(userTag);
		retVal = retVal && archive.Process(revisionsContentsItem.user);
		retVal = retVal && archive.EndTag(userTag);

		static iser::CArchiveTag commentTag("Comment", "Comment");
		retVal = retVal && archive.BeginTag(commentTag);
		retVal = retVal && archive.Process(revisionsContentsItem.comment);
		retVal = retVal && archive.EndTag(commentTag);

		static iser::CArchiveTag pathTag("Path", "Revision archive path");
		retVal = retVal && archive.BeginTag(pathTag);
		retVal = retVal && archive.Process(revisionsContentsItem.path);
		retVal = retVal && archive.EndTag(pathTag);

		if (retVal && !archive.IsStoring()){
			insert(revisionsContentsItem.revision, revisionsContentsItem);
		}

		retVal = retVal && archive.EndTag(revisionItemTag);
	}

	retVal = retVal && archive.EndTag(revisionListTag);

	return retVal;
}


} // namespace imtrepo


