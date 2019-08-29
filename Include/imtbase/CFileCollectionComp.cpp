#include <imtbase/CFileCollectionComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QUuid>

// ACF includes
#include <istd/TOptDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <istd/CCrcCalculator.h>
#include <istd/CSystem.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <ifile/CCompactXmlFileReadArchive.h>
#include <ifile/CFileListProviderComp.h>
#include <ifile/IFileResourceTypeConstraints.h>
#include <ilog/CMessage.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtbase
{


// public methods

CFileCollectionComp::CFileCollectionComp()
	:m_repositoryLock(QReadWriteLock::Recursive)
{
	m_resourceTypeConstraints.SetParent(this);
}


// reimplemented (IFileObjectCollection)

const ifile::IFileResourceTypeConstraints* CFileCollectionComp::GetResourceTypeConstraints() const
{
	if (!m_resourceFileTypesCompPtr.IsValid() || !m_resourceTypesCompPtr.IsValid()){
		return NULL;
	}

	return &m_resourceTypeConstraints;
}


ifile::IFileMetaInfoProvider::MetaInfoPtr CFileCollectionComp::GetFileMetaInfo(const QByteArray & objectId) const
{
	ifile::IFileMetaInfoProvider::MetaInfoPtr retVal;

	QReadLocker lockRepository(&m_repositoryLock);

	// Looking for file item:
	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex < 0){
		SendVerboseMessage(QString("Repository item doesn't exist for the given resource ID (%1). Meta-information could not be provided").arg(objectId.constData()), "File Collection");

		return ifile::IFileMetaInfoProvider::MetaInfoPtr();
	}

	// Get meta-information from cache:
	CollectionItem& item = m_files[fileIndex];
	if (item.metaInfoPtr.IsValid()){
		retVal = item.metaInfoPtr;

		return retVal;
	}

	return item.metaInfoPtr;
}


IFileObjectCollection::FileInfo CFileCollectionComp::GetFileInfo(const QByteArray& objectId) const
{
	FileInfo retVal;

	QReadLocker locker(&m_repositoryLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		QString repositoryFilePath = m_files[fileIndex].filePathInRepository;

		retVal.fileName = QFileInfo(repositoryFilePath).fileName();

		// Revert uuid logic for file name in case of plain repository
		if (!*m_useSubfolderAttrPtr){
			int index = retVal.fileName.indexOf("} - ");
			if (index  != -1){
				retVal.fileName = retVal.fileName.mid(index + 4);
			}
		}

		retVal.filePath = repositoryFilePath;
		retVal.lastModified = m_files[fileIndex].lastModificationTime;
	}

	return retVal;
}


QString CFileCollectionComp::GetFile(
			const QByteArray& objectId,
			const QString& targetFilePath) const
{
	QReadLocker locker(&m_repositoryLock);

	QString outputFilePath = targetFilePath;

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		QString filePathInRepository =  m_files[fileIndex].filePathInRepository;

		QFileInfo fileInfo(filePathInRepository);
		if (fileInfo.exists()){
			// If the output file path is empty, then create the file path automatically:
			if (outputFilePath.isEmpty()){
				QString fileExtension = QFileInfo(filePathInRepository).suffix();

				outputFilePath =  GetTempDirectory() + "/" + QFileInfo(filePathInRepository).completeBaseName() + "_" + QUuid::createUuid().toString() + "." + fileExtension;
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
			SendErrorMessage(0, QString("Repository file doesn't exist '%1'").arg(filePathInRepository));
		}
	}

	return QString();
}


QByteArray CFileCollectionComp::AddFile(
			const QString& localFilePath,
			const QByteArray& resourceTypeId,
			const QString& resourceName,
			const QString& resourceDescription,
			const QByteArray& objectId)
{
	static QByteArray emptyId;

	QWriteLocker locker(&m_repositoryLock);

	for (int i = 0; i < m_files.count(); ++i){
		if (!objectId.isEmpty()){
			if ((m_files[i].fileId == objectId)){
				SendErrorMessage(0, QString("File '%1' could not be added to the repository. Proposed resource ID ('%2') already in use").arg(localFilePath).arg(qPrintable(objectId)));

				return emptyId;
			}
		}

		if (m_files[i].filePathInRepository == localFilePath){
			if (objectId.isEmpty()){
				SendInfoMessage(0, QString("The file '%1' already exists in the repository").arg(localFilePath));

				return m_files[i].fileId;
			}

			SendInfoMessage(0, QString("Trying to add already existing file '%1' using a different file ID. New repository item will be created").arg(localFilePath));
		}

		if (!m_repositoryPathCompPtr.IsValid() && (m_files[i].sourceFilePath == localFilePath)){
			if (objectId.isEmpty() || (!objectId.isEmpty() && (objectId == m_files[i].fileId))){
				QFileInfo inputFileInfo(localFilePath);
				if (inputFileInfo.lastModified() == m_files[i].lastModificationTime){
					istd::CChangeNotifier changeNotifier(this);

					m_files[i].addedTime = QDateTime::currentDateTime();

					SendVerboseMessage(QString("The file '%1' is already exists in the repository").arg(localFilePath));

					QByteArray fileId = m_files[i].fileId;

					locker.unlock();
	
					return fileId;
				}
			}
		}
	}

	locker.unlock();

	QString targetFilePath;

	QFileInfo inputFileInfo(localFilePath);

	QString targetResourceName = resourceName.isEmpty() ? inputFileInfo.completeBaseName() : resourceName;

	if (!InsertFileIntoRepository(localFilePath, targetResourceName, resourceTypeId, nullptr, targetFilePath)){
		return emptyId;
	}

	QFileInfo targetFilePathInfo(targetFilePath);

	QByteArray fileId = objectId.isEmpty() ? QUuid::createUuid().toByteArray() : objectId;

	CollectionItem repositoryItem(*this);
	repositoryItem.fileId = fileId;
	repositoryItem.resourceTypeId = resourceTypeId;
	repositoryItem.resourceName = targetFilePathInfo.completeBaseName();
	repositoryItem.resourceDescription = resourceDescription;
	repositoryItem.filePathInRepository = targetFilePath;
	repositoryItem.sourceFilePath = localFilePath;
	repositoryItem.addedTime = QDateTime::currentDateTime();
	repositoryItem.lastModificationTime = inputFileInfo.lastModified();
	repositoryItem.checkSum = 0;

	if (m_isCalculateCheckSumAttrPtr.IsValid() && *m_isCalculateCheckSumAttrPtr){
		repositoryItem.checkSum = istd::CCrcCalculator::GetCrcFromFile(localFilePath);
	}

	UpdateItemMetaInfo(repositoryItem);

	QString savedPath = SaveCollectionItem(repositoryItem);
	if (!savedPath.isEmpty()){
		static ChangeSet changes(CF_FILE_ADDED);
		istd::CChangeNotifier changeNotifier(this, &changes);

		locker.relock();

		// Insert into the file list:
		m_files.push_back(repositoryItem);

		locker.unlock();

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
	QWriteLocker locker(&m_repositoryLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex < 0){
		SendErrorMessage(0, QString("No file with the ID '%1' was found in the repository. File could not be updated").arg(objectId.constData()));

		return false;
	}

	CollectionItem itemToUpdate = m_files[fileIndex];

	locker.unlock();

	QFileInfo inputFileInfo(localFilePath);

	QString targetFilePath = itemToUpdate.filePathInRepository;

	if (istd::CSystem::FileCopy(localFilePath, targetFilePath, true)){
		if (!QFile::setPermissions(targetFilePath, QFile::WriteGroup)){
			SendErrorMessage(0, QString("Permissions for the file '%1' could not be set").arg(targetFilePath));
		}

		itemToUpdate.addedTime = QDateTime::currentDateTime();
		itemToUpdate.lastModificationTime = inputFileInfo.lastModified();
		itemToUpdate.checkSum = 0;
		if (m_isCalculateCheckSumAttrPtr.IsValid()){
			if (*m_isCalculateCheckSumAttrPtr){
				itemToUpdate.checkSum = istd::CCrcCalculator::GetCrcFromFile(localFilePath);
			}
		}

		itemToUpdate.metaInfoPtr = CreateFileMetaInfo(itemToUpdate.filePathInRepository, itemToUpdate.resourceTypeId, itemToUpdate.checkSum);

		locker.relock();

		static ChangeSet changes(CF_FILE_UPDATED);
		istd::CChangeNotifier changeNotifier(this, &changes);

		m_files[fileIndex].CopyFrom(itemToUpdate, istd::IChangeable::CM_WITH_REFS);

		SaveCollectionItem(itemToUpdate);

		if (itemToUpdate.metaInfoPtr.IsValid()){
			QString metaInfoFilePath = GetMetaInfoFilePath(itemToUpdate);
			if (!SaveMetaInfo(*itemToUpdate.metaInfoPtr, metaInfoFilePath)){
				SendErrorMessage(0, QString("Meta information of the file '%1' could not be updated").arg(targetFilePath));
			}
		}

		locker.unlock();

		QMutexLocker lockCache(&m_cacheMutex);

		for (int cacheItemIndex = 0; cacheItemIndex < m_dataCache.count(); ++cacheItemIndex){
			if (m_dataCache[cacheItemIndex]->fileId == objectId){
				m_dataCache.removeAt(cacheItemIndex);
				break;
			}
		}

		lockCache.unlock();

		return true;
	}

	SendErrorMessage(0, QString("File '%1' could not be copied to %2").arg(localFilePath).arg(targetFilePath));

	return false;
}

// reimplemented (IFileCollectionInfo)

QString CFileCollectionComp::GetRepositoryPath() const
{
	QString retVal;

	if (m_repositoryPathCompPtr.IsValid()){
		retVal = m_repositoryPathCompPtr->GetPath();
	}

	return retVal;
}


// reimplemented (IObjectCollection)

int CFileCollectionComp::GetOperationFlags(const QByteArray& /*objectId*/) const
{
	return OF_SUPPORT_DELETE | OF_SUPPORT_EDIT | OF_SUPPORT_INSERT | OF_SUPPORT_RENAME;
}


QByteArray CFileCollectionComp::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& /*description*/,
			const istd::IChangeable * defaultValuePtr)
{
	istd::TOptDelPtr<const istd::IChangeable> newObjectPtr;

	if (defaultValuePtr != NULL){
		newObjectPtr.SetPtr(defaultValuePtr, false);
	}
	else{
		newObjectPtr.SetPtr(CreateDataObject(typeId), true);
	}

	if (newObjectPtr.IsValid()){
		const ifile::IFilePersistence* persistencePtr = GetPersistenceForResource(typeId);
		if (persistencePtr != NULL){
			QStringList supportedExts;
			persistencePtr->GetFileExtensions(supportedExts, defaultValuePtr, ifile::IFilePersistence::QF_SAVE);

			QString tempFilePath = QDir::tempPath() + "/" + QUuid::createUuid().toString() + "." + supportedExts[0];

			if (persistencePtr->SaveToFile(*newObjectPtr, tempFilePath) == ifile::IFilePersistence::OS_OK){
				return AddFile(tempFilePath, typeId, name);
			}
		}
	}

	return QByteArray();
}


istd::IChangeable * CFileCollectionComp::GetEditableObject(const QByteArray& /*objectId*/) const
{
	return nullptr;
}


bool CFileCollectionComp::RemoveObject(const QByteArray& objectId)
{
	if (objectId.isEmpty()){
		SendErrorMessage(0, "Resource ID is empty. Unknown resource could not be removed");

		return false;
	}

	QWriteLocker repositoryDataLocker(&m_repositoryLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		// If the file was copied into the repository, remove also the file from repository folder:
		const CollectionItem& itemToRemove = m_files[fileIndex];

		QFileInfo fileInfo(itemToRemove.filePathInRepository);

		QString dataFilePath = GetDataItemFilePath(itemToRemove);
		QFile dataFile(dataFilePath);
		if (!dataFile.remove()){
			SendErrorMessage(0, QString("Repository data file '%1' could not be removed. Error status: %2").arg(dataFilePath).arg(dataFile.errorString()));

			return false;
		}

		QString metaDataFilePath = GetMetaInfoFilePath(itemToRemove);
		QFile metaDataFile(metaDataFilePath);
		if (!metaDataFile.remove()){
			SendErrorMessage(0, QString("Repository meta-data file '%1' could not be removed. Error status: %2").arg(metaDataFilePath).arg(metaDataFile.errorString()));

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
			ChangeSet changes(CF_FILE_REMOVED);

			istd::CChangeNotifier changeNotifier(this, &changes);

			QMutexLocker lockCache(&m_cacheMutex);

			for (int cacheItemIndex = 0; cacheItemIndex < m_dataCache.count(); ++cacheItemIndex){
				if (m_dataCache[cacheItemIndex]->fileId == objectId){
					m_dataCache.removeAt(cacheItemIndex);
					break;
				}
			}

			// Remove the repository item with the corresponding resource ID:
			m_files.removeAt(fileIndex);

			// Ensure unlocking before change notification:
			lockCache.unlock();

			repositoryDataLocker.unlock();
		}

		return true;
	}
	else{
		SendInfoMessage(0, QString("Resource '%1' doesn't exist").arg(qPrintable(objectId)));
	}

	return true;
}


void CFileCollectionComp::SetObjectName(const QByteArray& objectId, const QString& objectName)
{
	QWriteLocker locker(&m_repositoryLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		CollectionItem& item = m_files[fileIndex];

		if (item.resourceName != objectName){
			int foundIndex = GetFileIndexByName(objectName);
			if (foundIndex >= 0){
				bool isSameResourceType = (m_files[fileIndex].resourceTypeId == m_files[foundIndex].resourceTypeId);

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
			newPhysicalName = ShortenWindowsFilename(newFullName, QFileInfo(newFullName), "");
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
				QString newMetaInfoFilePath = metaFileInfo.absolutePath() + "/" + newFileName + GetInfoFileExtention();

				if (!QFile::rename(oldMetaInfoFilePath, newMetaInfoFilePath)){
					SendWarningMessage(0, QString("Meta-info file '%1' could not be removed").arg(oldMetaInfoFilePath));

					QFile::remove(oldMetaInfoFilePath);
				}
			}

			if (*m_useSubfolderAttrPtr){
				targetFolderPath = GetRepositoryPath();
			
				if (!item.resourceTypeId.isEmpty()){
					Q_ASSERT(m_resourceTypesCompPtr.IsValid());

					if (m_resourceTypesCompPtr->GetOptionsCount() > 1){
						targetFolderPath += "/" + QString(item.resourceTypeId.constData());
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

	
			item.resourceName = objectName;
			item.filePathInRepository = newFileRepositoryPath;

			SaveCollectionItem(item);

			locker.unlock();

			return;
		}

		return;
	}

	SendErrorMessage(0, QString("Resource with the ID '%1' doesn't exist").arg(objectId.constData()));
}


void CFileCollectionComp::SetObjectDescription(const QByteArray & objectId, const QString & objectDescription)
{
	QWriteLocker locker(&m_repositoryLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		CollectionItem& item = m_files[fileIndex];

		if (item.resourceDescription != objectDescription){
			istd::CChangeNotifier changeNotifier(this);

			item.resourceDescription = objectDescription;

			locker.unlock();
		}
	}
}


void CFileCollectionComp::SetObjectEnabled(const QByteArray& /*objectId*/, bool /*isEnabled*/)
{
}


// reimplemented (IObjectProvider)

const istd::IChangeable* CFileCollectionComp::GetDataObject(const QByteArray& /*objectId*/) const
{
	return nullptr;
}


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CFileCollectionComp::GetObjectTypesInfo() const
{
	return &m_resourceTypeConstraints;
}


IObjectCollectionInfo::Id CFileCollectionComp::GetObjectTypeId(const QByteArray & objectId) const
{
	m_repositoryLock.lockForRead();

	QByteArray retVal;

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		retVal = m_files[fileIndex].resourceTypeId;
	}

	m_repositoryLock.unlock();

	return retVal;
}


// reimplemented (ICollectionInfo)

ICollectionInfo::Ids CFileCollectionComp::GetElementIds() const
{
	Ids retVal;

	m_repositoryLock.lockForRead();

	for (int fileIndex = 0; fileIndex < m_files.count(); ++fileIndex){
		retVal.append(m_files[fileIndex].fileId);
	}

	m_repositoryLock.unlock();

	return retVal;
}


QVariant CFileCollectionComp::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	QReadLocker locker(&m_repositoryLock);

	int fileIndex = GetFileIndexById(elementId);
	if (fileIndex >= 0){
		switch (infoType){
		case EIT_NAME:
			return m_files[fileIndex].resourceName;
			break;

		case EIT_DESCRIPTION:
			return m_files[fileIndex].resourceDescription;
			break;
		}
	}

	return QString();
}


// reimplemented (istd::IChangeable)

bool CFileCollectionComp::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	QWriteLocker locker(&m_repositoryLock);

	const CFileCollectionComp* sourcePtr = dynamic_cast<const CFileCollectionComp*>(&object);
	if (sourcePtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_files = sourcePtr->m_files; 

		locker.unlock();

		return true;
	}

	return false;
}


// static protected methods

QString CFileCollectionComp::GetInfoFileExtention()
{
	return ".meta";
}


QString CFileCollectionComp::GetDataFileExtention()
{
	return ".item";
}


// protected methods

istd::IChangeable* CFileCollectionComp::CreateDataObject(const QByteArray& resourceTypeId) const
{
	int factoryIndex = -1;

	if (m_resourceTypesCompPtr.IsValid()){
		for (int i = 0; i < m_resourceTypesCompPtr->GetOptionsCount(); ++i){
			if (resourceTypeId == m_resourceTypesCompPtr->GetOptionId(i)){
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


const ifile::IFilePersistence* CFileCollectionComp::GetPersistenceForResource(const QByteArray& resourceTypeId) const
{
	int factoryIndex = -1;

	if (m_resourceTypesCompPtr.IsValid()){
		for (int i = 0; i < m_resourceTypesCompPtr->GetOptionsCount(); ++i){
			if (resourceTypeId == m_resourceTypesCompPtr->GetOptionId(i)){
				factoryIndex = i;
				break;
			}
		}
	}

	if ((factoryIndex >= 0) && factoryIndex < m_objectPersistenceListCompPtr.GetCount()){
		return m_objectPersistenceListCompPtr[factoryIndex];
	}

	return NULL;
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
		if (m_files[i].resourceName == fileName){
			return i;
		}
	}

	return -1;
}


istd::IChangeable* CFileCollectionComp::CreateObjectFromFile(const QString& filePath, const QByteArray& resourceTypeId) const
{
	istd::IChangeable* retVal = CreateDataObject(resourceTypeId);
	if (retVal != nullptr){
		const ifile::IFilePersistence* filePersistenceCompPtr = GetPersistenceForResource(resourceTypeId);
		if (filePersistenceCompPtr != NULL){
			int loadState = filePersistenceCompPtr->LoadFromFile(*retVal, filePath);
			if (loadState == ifile::IFilePersistence::OS_OK){
				return retVal;
			}
		}
	}

	return nullptr;
}


ifile::IFileMetaInfoProvider::MetaInfoPtr CFileCollectionComp::CreateFileMetaInfo(
			const QString& filePath,
			const QByteArray& typeId,
			const quint32& checkSum) const
{
	QFileInfo fileInfo(filePath);
	if (fileInfo.exists()){
		ifile::IFileMetaInfoProvider::MetaInfoPtr metaInfoPtr(new idoc::CStandardDocumentMetaInfo);

		metaInfoPtr->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, fileInfo.lastModified());
		metaInfoPtr->SetMetaInfo(MIT_CHECKSUM, checkSum);
		metaInfoPtr->SetMetaInfo(MIT_RESOURCE_TYPE_ID, typeId);

		return metaInfoPtr;
	}

	return ifile::IFileMetaInfoProvider::MetaInfoPtr();
}


ifile::IFileMetaInfoProvider::MetaInfoPtr CFileCollectionComp::TryLoadMetaInfoFromFile(const QString& metaInfoFilePath) const
{
	if (QFileInfo(metaInfoFilePath).exists()){
		ifile::IFileMetaInfoProvider::MetaInfoPtr fileMetaInfoPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>);

		if (LoadFileMetaInfo(*fileMetaInfoPtr.GetPtr(), metaInfoFilePath)){
			return fileMetaInfoPtr;
		}
	}

	return ifile::IFileMetaInfoProvider::MetaInfoPtr();
}


void CFileCollectionComp::UpdateItemMetaInfo(CollectionItem& item)
{
	QString metaInfoFilePath = GetMetaInfoFilePath(item);
	QFileInfo fileInfo(metaInfoFilePath);

	ifile::IFileMetaInfoProvider::MetaInfoPtr retVal = TryLoadMetaInfoFromFile(metaInfoFilePath);
	if (!retVal.IsValid()){
		retVal = CreateFileMetaInfo(item.filePathInRepository, item.resourceTypeId, item.checkSum);
		if (!retVal.IsValid()){
			SendErrorMessage(0, QString("Meta-information for the file '%1' could not be created. Meta-information could not be provided").arg(metaInfoFilePath), "File Repository");
		}
		else{
			bool isSuccessfulySaved = SaveMetaInfo(*retVal, metaInfoFilePath);
			if (!isSuccessfulySaved){
				SendWarningMessage(0, QString("Meta-information for the file '%1' created, but could not be saved to file").arg(metaInfoFilePath), "File Repository");
			}
		}
	}

	item.metaInfoPtr = retVal;
}


bool CFileCollectionComp::SaveMetaInfo(const idoc::IDocumentMetaInfo& metaInfo, const QString& filePath) const
{
	const iser::ISerializable* serializablePtr = dynamic_cast<const iser::ISerializable*>(&metaInfo);
	if (serializablePtr != NULL){
		ifile::CCompactXmlFileWriteArchive archive(filePath, m_versionInfoCompPtr.GetPtr());

		return (const_cast<iser::ISerializable*>(serializablePtr))->Serialize(archive);
	}

	return false;
}


bool CFileCollectionComp::LoadFileMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QString& filePath) const
{
	iser::ISerializable* serializablePtr = dynamic_cast<iser::ISerializable*>(&metaInfo);
	if (serializablePtr != NULL){
		ifile::CCompactXmlFileReadArchive archive(filePath, m_versionInfoCompPtr.GetPtr());

		return serializablePtr->Serialize(archive);
	}

	return false;
}


QString CFileCollectionComp::CalculateFolderPathInRepository(
			const QString& localFilePath,
			const QString& resourceName,
			const QByteArray& resourceTypeId,
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

		// If resource ID is non empty, create subfolder for this:
		if (!resourceTypeId.isEmpty()){
			Q_ASSERT(m_resourceTypesCompPtr.IsValid());

			if (m_resourceTypesCompPtr->GetOptionsCount() > 1){
				targetDirPath += QString("/") + resourceTypeId.constData();
			}
		}

		if (useSubfolder){
			QString subfolderName = resourceName;
#ifdef _WIN32
			// shortened on Win?
			if (inputFileInfo.fileName() == resourceName){
				subfolderName = inputFileInfo.completeBaseName();
			}
#endif
			targetDirPath += QString("/") + subfolderName;

			if (useNameCounting){
				int count = 0;
				QString newDirPath = targetDirPath;
				while (QFileInfo::exists(newDirPath)){
					newDirPath = QString("%1_%2").arg(targetDirPath).arg(++count);
				}

				if (newDirPath != targetDirPath){
					QString warning = tr("Input file name %1 renamed to %2").arg(QDir(targetDirPath).dirName()).arg(QDir(newDirPath).dirName());

					SendWarningMessage(0, warning);

					if (messageConsumerPtr != NULL){
						messageConsumerPtr->AddMessage(ilog::IMessageConsumer::MessagePtr(new ilog::CMessage(istd::IInformationProvider::IC_WARNING, 0, warning, "File Repository")));
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

#ifdef _WIN32
		targetFileName = ShortenWindowsFilename(targetFileName, inputFileInfo, "");
#endif
		targetFilePath = targetFolderPath + QString("/") + targetFileName;
	}
	else{
		QString uuidPrefix = QUuid::createUuid().toString() + QString(" - ");
		QString targetFileName = uuidPrefix + inputFileInfo.fileName();
#ifdef _WIN32
		targetFileName = ShortenWindowsFilename(targetFileName, inputFileInfo, uuidPrefix);
#endif
		targetFilePath = targetFolderPath + QString("/") + targetFileName;
	}

	return targetFilePath;
}


// reimplemented (icomp::CComponentBase)

void CFileCollectionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QString path;

	// Ensure, that the repository folder was created:
	if (m_repositoryPathCompPtr.IsValid()){
		Q_ASSERT(m_repositoryPathCompPtr->GetPathType() == ifile::IFileNameParam::PT_DIRECTORY);

		path = m_repositoryPathCompPtr->GetPath();
		if (!path.isEmpty()){
			if (*m_createFolderOnStartAttrPtr && !istd::CSystem::EnsurePathExists(path)){
				SendCriticalMessage(0, QString("File repository could not be created in '%1'").arg(path));
			}
		}
	}

	ReadCollectionItems();
}


void CFileCollectionComp::OnComponentDestroyed()
{
	m_repositoryLock.lockForWrite();
	m_files.clear();
	m_repositoryLock.unlock();

	QMutexLocker lockCache(&m_cacheMutex);
	m_dataCache.clear();

	BaseClass::OnComponentDestroyed();
}


// private methods

bool CFileCollectionComp::InsertFileIntoRepository(
			const QString& filePath,
			const QString& resourceName,
			const QByteArray& resourceTypeId,
			ilog::IMessageConsumer* messageConsumerPtr,
			QString& filePathInRepository)
{
	QFileInfo inputFileInfo(filePath);
	if (!inputFileInfo.exists()){
		SendErrorMessage(0, QString("Input file '%1' doesn't exist").arg(filePath));

		return false;
	}

	QString targetFolderPath = CalculateFolderPathInRepository(filePath, resourceName, resourceTypeId, *m_useSubfolderAttrPtr, true, messageConsumerPtr);
	if (targetFolderPath.isEmpty()){
		SendErrorMessage(0, QString("Repository folder for the input file '%1'could not be calculated").arg(filePath));

		return false;
	}

	QFileInfo targetFolderInfo(targetFolderPath);
	if (targetFolderInfo.exists() && *m_useSubfolderAttrPtr){
		QString errorMessage = QString("File could not be added. Resource with the same name already exists %1").arg(targetFolderPath);

		SendErrorMessage(0, errorMessage);

		if (messageConsumerPtr != NULL){
			messageConsumerPtr->AddMessage(ilog::IMessageConsumer::MessagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0,  errorMessage, "File Repository")));
		}

		return false;
	}

	if (!targetFolderInfo.exists()){
		QDir targetFolder(targetFolderPath);

		if (!targetFolder.mkpath(targetFolderPath)){
			SendErrorMessage(0, QString("File repository could not be set up. Repository directory '%1' could not be created").arg(targetFolderPath));

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


QString CFileCollectionComp::SaveCollectionItem(const CollectionItem& repositoryItem) const
{
	QString itemFilePath = GetDataItemFilePath(repositoryItem);

	ifile::CCompactXmlFileWriteArchive archive(itemFilePath, m_versionInfoCompPtr.GetPtr());

	if (!const_cast<CollectionItem&>(repositoryItem).Serialize(archive)){
		SendErrorMessage(0, QString("Repository item could not be saved into '%1'").arg(itemFilePath));

		return QString();
	}

	return itemFilePath;
}


void CFileCollectionComp::ReadCollectionItems()
{
	istd::CChangeNotifier changeNotifier(this);

	m_repositoryLock.lockForWrite();

	m_files.clear();

	QString repositoryRootPath = GetRepositoryPath();
	QDir repositoryRootDir(repositoryRootPath);

	QFileInfoList repositoryFiles;
	ifile::CFileListProviderComp::CreateFileList(repositoryRootDir, 0, 2, QStringList() << QString("*%1").arg(GetDataFileExtention()), QDir::Name, repositoryFiles);

	for (int fileIndex = 0; fileIndex < repositoryFiles.count(); ++fileIndex){
		QString itemFilePath = repositoryFiles[fileIndex].absoluteFilePath();

		ifile::CCompactXmlFileReadArchive archive(itemFilePath, m_versionInfoCompPtr.GetPtr());
		CollectionItem fileItem(*this);

		if (!fileItem.Serialize(archive)){
			SendErrorMessage(0, QString("Repository item could not be loaded from '%1'").arg(itemFilePath));

			continue;
		}

		QFileInfo repositoryFileInfo(fileItem.filePathInRepository);
		if (repositoryFileInfo.exists()){
			UpdateItemMetaInfo(fileItem);

			m_files.push_back(fileItem);
		}
		else{
			SendErrorMessage(0, QString("File '%1' doesn't exist. Repository item was automatically removed").arg(fileItem.filePathInRepository));
		}
	}

	m_repositoryLock.unlock();
}


QString CFileCollectionComp::GetTempDirectory() const
{
	return QDir::tempPath();
}


QString CFileCollectionComp::GetDataItemFilePath(const CollectionItem& repositoryFile) const
{
	QString subFolder = *m_useSubfolderAttrPtr ? repositoryFile.resourceName : QString();
	QString itemFilePath;

	QString baseName = QFileInfo(repositoryFile.filePathInRepository).fileName();
	if (baseName.isEmpty()){
		return QString();
	}

	QFileInfo fileInfo(repositoryFile.filePathInRepository);

	itemFilePath = fileInfo.absolutePath() + "/" + baseName + GetDataFileExtention();

	return itemFilePath;
}


QString CFileCollectionComp::GetMetaInfoFilePath(const CollectionItem& repositoryFile) const
{
	QString subFolder = *m_useSubfolderAttrPtr ? repositoryFile.resourceName : QString();
	QString itemFilePath;

	QString baseName = QFileInfo(repositoryFile.filePathInRepository).fileName();
	if (baseName.isEmpty()){
		return QString();
	}

	QFileInfo fileInfo(repositoryFile.filePathInRepository);

	itemFilePath = fileInfo.absolutePath() + "/" + baseName + GetInfoFileExtention();

	return itemFilePath;
}


QString CFileCollectionComp::ShortenWindowsFilename(const QString& fileName, const QFileInfo& fileInfo, const QString& prefix) const
{
	if (fileName.size() + GetDataFileExtention().size() > 255){
		int shortenedFilenameSize = 255 - GetDataFileExtention().size() - prefix.size();

		QString shortenedFilename = fileInfo.fileName().mid(0, shortenedFilenameSize - (fileInfo.suffix().size() + 1));
		shortenedFilename.chop(1);
		shortenedFilename.append("~.").append(fileInfo.suffix());
		shortenedFilename.prepend(prefix);

		Q_ASSERT(shortenedFilename.size() <= 255 - GetDataFileExtention().size());

#if _DEBUG
		SendInfoMessage(0, QString("File storage name shortened (too long on Windows), from '%1' to '%2'").arg(fileName).arg(shortenedFilename));
#endif
		return shortenedFilename;
	}
	else{
		return fileName;
	}
}


// public methods of the embedded class ResourceTypeConstraints

CFileCollectionComp::ResourceTypeConstraints::ResourceTypeConstraints()
	:m_parentPtr(NULL)
{
}


void CFileCollectionComp::ResourceTypeConstraints::SetParent(CFileCollectionComp* parentPtr)
{
	m_parentPtr = parentPtr;
}


// reimplemented (IFileResourceTypeConstraints)

const ifile::IFileTypeInfo* CFileCollectionComp::ResourceTypeConstraints::GetFileTypeInfo(int resourceTypeIndex) const
{
	Q_ASSERT(m_parentPtr != NULL);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());
	Q_ASSERT(resourceTypeIndex >= 0);

	if (resourceTypeIndex < m_parentPtr->m_resourceFileTypesCompPtr.GetCount()){
		return m_parentPtr->m_resourceFileTypesCompPtr[resourceTypeIndex];
	}

	return NULL;
}


// reimplemented (iprm::IOptionsList)

int CFileCollectionComp::ResourceTypeConstraints::GetOptionsFlags() const
{
	Q_ASSERT(m_parentPtr != NULL);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionsFlags();
}


int CFileCollectionComp::ResourceTypeConstraints::GetOptionsCount() const
{
	Q_ASSERT(m_parentPtr != NULL);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionsCount();
}


QString CFileCollectionComp::ResourceTypeConstraints::GetOptionName(int index) const
{
	Q_ASSERT(m_parentPtr != NULL);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionName(index);
}


QString CFileCollectionComp::ResourceTypeConstraints::GetOptionDescription(int index) const
{
	Q_ASSERT(m_parentPtr != NULL);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionDescription(index);
}


QByteArray CFileCollectionComp::ResourceTypeConstraints::GetOptionId(int index) const
{
	Q_ASSERT(m_parentPtr != NULL);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionId(index);
}


bool CFileCollectionComp::ResourceTypeConstraints::IsOptionEnabled(int index) const
{
	Q_ASSERT(m_parentPtr != NULL);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->IsOptionEnabled(index);
}


// public methods of embedded class CollectionItem

// reimplement (iser::ISerializable)

bool CFileCollectionComp::CollectionItem::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	static iser::CArchiveTag fileIdTag("FileId", "ID of the file in the repository");
	retVal = retVal && archive.BeginTag(fileIdTag);
	retVal = retVal && archive.Process(fileId);
	retVal = retVal && archive.EndTag(fileIdTag);

	// Change file path in the repository to a relative path (relvative to repository's folder):
	QDir repositoryDir(m_parent->GetRepositoryPath());
	QString relativeFilePathInRepository = repositoryDir.relativeFilePath(filePathInRepository);

	static iser::CArchiveTag filePathInrepositoryItemsTag("RepositoryFilePath", "File path in the repository");
	retVal = retVal && archive.BeginTag(filePathInrepositoryItemsTag);
	retVal = retVal && archive.Process(relativeFilePathInRepository);
	retVal = retVal && archive.EndTag(filePathInrepositoryItemsTag);

	if (retVal && !archive.IsStoring()){
		filePathInRepository = QDir::cleanPath(repositoryDir.absoluteFilePath(relativeFilePathInRepository));
	}

	// Change file path in the repository to a relative path (relvative to repository's folder):
	QString relativeSourceFilePath = repositoryDir.relativeFilePath(sourceFilePath);

	static iser::CArchiveTag sourceFilePathTag("SourceFilePath", "Source location of the file");
	retVal = retVal && archive.BeginTag(sourceFilePathTag);
	retVal = retVal && archive.Process(relativeSourceFilePath);
	retVal = retVal && archive.EndTag(sourceFilePathTag);

	if (retVal && !archive.IsStoring()){
		sourceFilePath = QDir::cleanPath(repositoryDir.absoluteFilePath(relativeSourceFilePath));
	}

	static iser::CArchiveTag resourceNameTag("ResourceName", "Name of the file resource");
	retVal = retVal && archive.BeginTag(resourceNameTag);
	retVal = retVal && archive.Process(resourceName);
	retVal = retVal && archive.EndTag(resourceNameTag);

	static iser::CArchiveTag resourceDescriptionTag("ResourceDescription", "Description of the file resource");
	retVal = retVal && archive.BeginTag(resourceDescriptionTag);
	retVal = retVal && archive.Process(resourceDescription);
	retVal = retVal && archive.EndTag(resourceDescriptionTag);

	static iser::CArchiveTag resourceTypeIdTag("ResourceTypeId", "Type ID of the file resource");
	retVal = retVal && archive.BeginTag(resourceTypeIdTag);
	retVal = retVal && archive.Process(resourceTypeId);
	retVal = retVal && archive.EndTag(resourceTypeIdTag);

	static iser::CArchiveTag addedAtTag("AddedAt", "Time the file was added");
	retVal = retVal && archive.BeginTag(addedAtTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, addedTime);
	retVal = retVal && archive.EndTag(addedAtTag);

	static iser::CArchiveTag lastModificationTimeTag("LastModifiedAt", "Time stamp of the last file modification");
	retVal = retVal && archive.BeginTag(lastModificationTimeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, lastModificationTime);
	retVal = retVal && archive.EndTag(lastModificationTimeTag);

	static iser::CArchiveTag checkSumTag("CheckSum", "Check sum of the file resource");
	retVal = retVal && archive.BeginTag(checkSumTag);
	retVal = retVal && archive.Process(checkSum);
	retVal = retVal && archive.EndTag(checkSumTag);

	return retVal;
}


// reimplement (istd::IChangeable)

bool CFileCollectionComp::CollectionItem::CopyFrom(const istd::IChangeable& object, CompatibilityMode mode)
{
	const CollectionItem* sourceItemPtr = dynamic_cast<const CollectionItem*>(&object);
	if (sourceItemPtr != NULL){
		fileId = sourceItemPtr->fileId;
		filePathInRepository = sourceItemPtr->filePathInRepository;
		sourceFilePath = sourceItemPtr->sourceFilePath;
		resourceName = sourceItemPtr->resourceName;
		resourceDescription = sourceItemPtr->resourceDescription;
		addedTime = sourceItemPtr->addedTime;
		lastModificationTime = sourceItemPtr->lastModificationTime;
		resourceTypeId = sourceItemPtr->resourceTypeId;
		checkSum = sourceItemPtr->checkSum;

		if (mode == istd::IChangeable::CM_WITH_REFS){
			metaInfoPtr = sourceItemPtr->metaInfoPtr;
		}

		return true;
	}

	return false;
}


} // namespace imtbase


