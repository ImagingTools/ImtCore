#include <imtrepo/CFileRepositoryComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QUuid>
#include <QtCore/QStandardPaths>
#include <QtCore/QElapsedTimer>
#include <QtCore/QTimer>

// ACF includes
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


namespace imtrepo
{


// public methods

CFileRepositoryComp::CFileRepositoryComp()
	:m_repositoryLock(QReadWriteLock::Recursive)
{
	m_resourceTypeConstraints.SetParent(this);

	connect(this, SIGNAL(EmitRemoveFile(const QByteArray&)), this, SLOT(OnFileRemove(const QByteArray&)), Qt::QueuedConnection);
}


// reimplemented (ifile::IFileResourceValidator)

istd::IInformationProvider::InformationCategory CFileRepositoryComp::ValidateResource(
			const QByteArray& resourceId,
			const ifile::IFileResourcesManager* fileRepositoryPtr,
			ilog::IMessageConsumer* messageConsumerPtr) const
{
	QByteArray resourceTypeId = GetResourceTypeId(resourceId);
	if (resourceTypeId.isEmpty()){
		if (messageConsumerPtr != NULL){
			messageConsumerPtr->AddMessage(ilog::IMessageConsumer::MessagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, "Resource is not available in the repository", "File Repository")));
		}

		return istd::IInformationProvider::IC_NONE;
	}

	const ifile::IFileResourceValidator* resourceValidatorPtr = GetResourceValidator(resourceTypeId);
	if (resourceValidatorPtr != NULL){
		return resourceValidatorPtr->ValidateResource(resourceId, fileRepositoryPtr, messageConsumerPtr);
	}

	return istd::IInformationProvider::IC_NONE;
}


// reimplemented (IDataObjectFactory)

istd::IChangeable* CFileRepositoryComp::CreateInstance(const QByteArray& keyId) const
{
	int factoryIndex = -1;

	if (m_resourceTypesCompPtr.IsValid()){
		for (int i = 0; i < m_resourceTypesCompPtr->GetOptionsCount(); ++i){
			if (keyId == m_resourceTypesCompPtr->GetOptionId(i)){
				factoryIndex = i;
				break;
			}
		}
	}

	if ((factoryIndex >= 0) && factoryIndex < m_objectFactoryListCompPtr.GetCount()){
		return m_objectFactoryListCompPtr.CreateInstance(factoryIndex);
	}

	return NULL;
}


// reimplemented (istd::IFactoryInfo)

istd::IFactoryInfo::KeyList CFileRepositoryComp::GetFactoryKeys() const
{
	istd::IFactoryInfo::KeyList retVal;

	if (m_resourceTypesCompPtr.IsValid()){
		int resourceTypesCount = m_resourceTypesCompPtr->GetOptionsCount();
		for (int i = 0; i < resourceTypesCount; ++i){
			retVal.insert(m_resourceTypesCompPtr->GetOptionId(i));
		}
	}

	return retVal;
}


// reimplemented (ifile::IFileResourcesManager)

ifile::IFileResourcesManager::Ids CFileRepositoryComp::GetFileIds() const
{
	Ids retVal;

	m_repositoryLock.lockForRead();

	for (int fileIndex = 0; fileIndex < m_files.count(); ++fileIndex){
		retVal.append(m_files[fileIndex].fileId);
	}

	m_repositoryLock.unlock();

	return retVal;
}


const ifile::IFileResourceTypeConstraints* CFileRepositoryComp::GetResourceTypeConstraints() const
{
	if (!m_resourceFileTypesCompPtr.IsValid() || !m_resourceTypesCompPtr.IsValid()){
		return NULL;
	}

	return &m_resourceTypeConstraints;
}


QByteArray CFileRepositoryComp::GetResourceTypeId(const QByteArray& resourceId) const
{
	m_repositoryLock.lockForRead();

	QByteArray retVal;

	int fileIndex = GetFileIndexById(resourceId);
	if (fileIndex >= 0){
		retVal = m_files[fileIndex].resourceTypeId;
	}

	m_repositoryLock.unlock();

	return retVal;
}


QByteArray CFileRepositoryComp::CreateNewFile(
			const QByteArray& fileTypeId,
			const istd::IChangeable* dataObjectPtr,
			const QString& /*filePath*/,
			const QString& resourceName)
{
	if (dataObjectPtr != NULL){
		const ifile::IFilePersistence* persistencePtr = GetPersistenceForResource(fileTypeId);
		if (persistencePtr != NULL){
			QStringList supportedExts;
			persistencePtr->GetFileExtensions(supportedExts, dataObjectPtr, ifile::IFilePersistence::QF_SAVE);

			QString tempFilePath = QDir::tempPath() + "/" + QUuid::createUuid().toString() + "." + supportedExts[0];

			if (persistencePtr->SaveToFile(*dataObjectPtr, tempFilePath) == ifile::IFilePersistence::OS_OK){
				return AddFile(tempFilePath, fileTypeId, resourceName);
			}
		}
	}

	return QByteArray();
}


bool CFileRepositoryComp::RemoveFile(const QByteArray& resourceId)
{
	if (resourceId.isEmpty()){
		SendErrorMessage(0, "Resource ID is empty. Unknown resource could not be removed");

		return false;
	}

	QWriteLocker repositoryDataLocker(&m_repositoryLock);

	int fileIndex = GetFileIndexById(resourceId);
	if (fileIndex >= 0){
		// If the file was copied into the repository, remove also the file from repository folder:
		const RepositoryItem& itemToRemove = m_files[fileIndex];

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
				if (m_dataCache[cacheItemIndex]->fileId == resourceId){
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
		SendInfoMessage(0, QString("Resource '%1' doesn't exist").arg(qPrintable(resourceId)));
	}

	return true;
}


ifile::IFileResourcesManager::DataObjectPtr CFileRepositoryComp::GetDataObject(const QByteArray& fileId) const
{
	ifile::IFileResourcesManager::DataObjectPtr retVal;

	QMutexLocker lockCache(&m_cacheMutex);

	for (DataCache::ConstIterator iter = m_dataCache.constBegin(); iter != m_dataCache.constEnd(); ++iter){
		if ((*iter)->fileId == fileId){
			retVal = (*iter)->dataObjectPtr;

			return retVal;
		}
	}

	int maxCacheSize = 100;
	if ((maxCacheSize >= 0) && m_dataCache.count() > maxCacheSize){
		m_dataCache.pop_front();
	}

	QReadLocker locker(&m_repositoryLock);

	int fileIndex = GetFileIndexById(fileId);
	if (m_resourceTypesCompPtr.IsValid() && (fileIndex >= 0)){
		RepositoryItem item = m_files[fileIndex];

		locker.unlock();

		retVal = CreateObjectFromFile(item.filePathInRepository, item.resourceTypeId);
		if (retVal.IsValid()){
			DataCachePtr dataPtr(new Data());

			dataPtr->fileId = fileId;
			dataPtr->dataObjectPtr = retVal;
			dataPtr->tempFilePath = item.filePathInRepository;

			m_dataCache.push_back(dataPtr);

			return retVal;
		}
		else{
			SendErrorMessage(0, QString("Data object (Resource type: %1) could not be created").arg(qPrintable(item.resourceTypeId)));
		}
	}

	return DataObjectPtr();
}


CFileRepositoryComp::MetaInfoPtr CFileRepositoryComp::GetFileMetaInfo(const QByteArray& resourceId) const
{
	CFileRepositoryComp::MetaInfoPtr retVal;

	QReadLocker lockRepository(&m_repositoryLock);

	// Looking for file item:
	int fileIndex = GetFileIndexById(resourceId);
	if (fileIndex < 0){
		/*
			While there is no transaction mechanism for single File (sequence GetFileIds() - GetFileMetaInfo(id) can failed)
			SendVerboseMessage should be used (warning is also aggressive for user)
		*/
		SendVerboseMessage(QString("Repository item doesn't exist for the given resource ID (%1). Meta-information could not be provided").arg(resourceId.constData()), "File Repository");

		return CFileRepositoryComp::MetaInfoPtr();
	}

	// Get meta-information from cache:
	RepositoryItem& item = m_files[fileIndex];
	if (item.metaInfoPtr.IsValid()){
		retVal = item.metaInfoPtr;

		return retVal;
	}

	return item.metaInfoPtr;
}


QString CFileRepositoryComp::GetResourceName(const QByteArray& resourceId)  const
{
	QString retVal;

	QReadLocker locker(&m_repositoryLock);

	int fileIndex = GetFileIndexById(resourceId);
	if (fileIndex >= 0){
		retVal = m_files[fileIndex].resourceName;
	}

	return retVal;
}


bool CFileRepositoryComp::SetResourceName(const QByteArray& resourceId, const QString& name)
{
	QWriteLocker locker(&m_repositoryLock);

	int fileIndex = GetFileIndexById(resourceId);
	if (fileIndex >= 0){
		RepositoryItem& item = m_files[fileIndex];

		if (item.resourceName != name){
			int foundIndex = GetFileIndexByName(name);
			if (foundIndex >= 0){
				bool isSameResourceType = (m_files[fileIndex].resourceTypeId == m_files[foundIndex].resourceTypeId);

				if (isSameResourceType || (!isSameResourceType && !*m_useSubfolderAttrPtr)){
					SendErrorMessage(0, QString("Resource with the name '%1' already exists").arg(name));

					return false;
				}
			}

			istd::CChangeNotifier changeNotifier(this);

			QFileInfo fileInfo(item.filePathInRepository);
			QDir resourceDir = fileInfo.absoluteDir();

			QString resouceDirPath = resourceDir.absolutePath();
			QString targetFolderPath = resouceDirPath;

			QString fileName = fileInfo.fileName();

			QString newPhysicalName = name;
#if _WIN32
			QString newFullName = name + "." + fileInfo.suffix();
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
					SendErrorMessage(0, QString("Resource path could not be renamed from '%1' into '%2'. Resource could not be renamed to '%3'").arg(resouceDirPath).arg(targetFolderPath).arg(name));

					locker.unlock();

					return false;
				}
			}

			QString newFileRepositoryPath = targetFolderPath + "/" + newFileName;
			QFile::rename(targetFolderPath + "/" + fileName, newFileRepositoryPath);

	
			item.resourceName = name;
			item.filePathInRepository = newFileRepositoryPath;

			SaveRepositoryItem(item);

			locker.unlock();

			return true;
		}

		return true;
	}

	SendErrorMessage(0, QString("Resource with the ID '%1' doesn't exist").arg(resourceId.constData()));

	return false;
}


QString CFileRepositoryComp::GetResourceDescription(const QByteArray& resourceId)  const
{
	QString retVal;

	QReadLocker locker(&m_repositoryLock);

	int fileIndex = GetFileIndexById(resourceId);
	if (fileIndex >= 0){
		retVal = m_files[fileIndex].resourceDescription;
	}

	return retVal;
}


bool CFileRepositoryComp::SetResourceDescription(const QByteArray& resourceId, const QString& description)
{
	QWriteLocker locker(&m_repositoryLock);

	int fileIndex = GetFileIndexById(resourceId);
	if (fileIndex >= 0){
		RepositoryItem& item = m_files[fileIndex];

		if (item.resourceDescription != description){
			istd::CChangeNotifier changeNotifier(this);

			item.resourceDescription = description;

			locker.unlock();
		}

		return true;
	}

	return false;
}


IFileRepository::LicenseStatus CFileRepositoryComp::GetLicenseStatus(const QByteArray& /*resourceId*/, const QByteArray& /*resourceTypeId*/, ilog::IMessageConsumer* /*messageConsumerPtr*/) const
{
	return LS_NONE;
}


// reimplemented (IFileRepository)

IFileRepository::FileInfo CFileRepositoryComp::GetFileInfo(const QByteArray& resourceId) const
{
	FileInfo retVal;

	QReadLocker locker(&m_repositoryLock);

	int fileIndex = GetFileIndexById(resourceId);
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


QByteArray CFileRepositoryComp::FindResourceByName(const QString& resourceName) const
{
	QByteArray retVal;

	QReadLocker locker(&m_repositoryLock);

	for (int fileIndex = 0; fileIndex < m_files.count(); ++fileIndex){
		if (m_files[fileIndex].resourceName == resourceName){
			retVal = m_files[fileIndex].fileId;
		}
	}

	return retVal;
}


QString CFileRepositoryComp::GetFile(
			const QByteArray& resourceId,
			const QString& targetFilePath,
			ibase::IProgressManager* /*progressManagerPtr*/,
			ilog::IMessageConsumer* /*messageConsumerPtr*/) const
{
	QReadLocker locker(&m_repositoryLock);

	QString outputFilePath = targetFilePath;

	int fileIndex = GetFileIndexById(resourceId);
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


QByteArray CFileRepositoryComp::AddFile(
			const QString& localFilePath,
			const QByteArray& resourceTypeId,
			const QString& resourceName,
			const QString& resourceDescription,
			const QByteArray& resourceId,
			ibase::IProgressManager* /*progressManagerPtr*/,
			ilog::IMessageConsumer* messageConsumerPtr)
{
	static QByteArray emptyId;

	QWriteLocker locker(&m_repositoryLock);

	for (int i = 0; i < m_files.count(); ++i){
		if (!resourceId.isEmpty()){
			if ((m_files[i].fileId == resourceId)){
				SendErrorMessage(0, QString("File '%1' could not be added to the repository. Proposed resource ID ('%2') already in use").arg(localFilePath).arg(qPrintable(resourceId)));

				return emptyId;
			}
		}

		if (m_files[i].filePathInRepository == localFilePath){
			if (resourceId.isEmpty()){
				SendInfoMessage(0, QString("The file '%1' already exists in the repository").arg(localFilePath));

				return m_files[i].fileId;
			}

			SendInfoMessage(0, QString("Trying to add already existing file '%1' using a different file ID. New repository item will be created").arg(localFilePath));
		}

		if (!m_repositoryPathCompPtr.IsValid() && (m_files[i].sourceFilePath == localFilePath)){
			if (resourceId.isEmpty() || (!resourceId.isEmpty() && (resourceId == m_files[i].fileId))){
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

	if (!InsertFileIntoRepository(localFilePath, targetResourceName, resourceTypeId, messageConsumerPtr, targetFilePath)){
		return emptyId;
	}

	QFileInfo targetFilePathInfo(targetFilePath);

	QByteArray fileId = resourceId.isEmpty() ? QUuid::createUuid().toByteArray() : resourceId;

	RepositoryItem repositoryItem(*this);
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

	QString savedPath = SaveRepositoryItem(repositoryItem);
	if (!savedPath.isEmpty()){
		locker.relock();

		// Insert into the file list:
		m_files.push_back(repositoryItem);

		locker.unlock();

		return fileId;
	}
	else{
		if (!QFile::remove(targetFilePath)){
			SendErrorMessage(0, QString("Cleanup for unsuccessfule repository operation failed, file '%1' could not be deleted").arg(targetFilePath));
		}
	}

	SendErrorMessage(0, "File could not be added to the repository");

	return QByteArray();
}


bool CFileRepositoryComp::UpdateFile(
			const QString& localFilePath,
			const QByteArray& resourceId,
			ibase::IProgressManager* /*progressManagerPtr*/,
			ilog::IMessageConsumer* /*messageConsumerPtr*/)
{
	QWriteLocker locker(&m_repositoryLock);

	int fileIndex = GetFileIndexById(resourceId);
	if (fileIndex < 0){
		SendErrorMessage(0, QString("No file with the ID '%1' was found in the repository. File could not be updated").arg(resourceId.constData()));

		return false;
	}

	RepositoryItem itemToUpdate = m_files[fileIndex];

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

		SaveRepositoryItem(itemToUpdate);

		if (itemToUpdate.metaInfoPtr.IsValid()){
			QString metaInfoFilePath = GetMetaInfoFilePath(itemToUpdate);
			if (!SaveMetaInfo(*itemToUpdate.metaInfoPtr, metaInfoFilePath)){
				SendErrorMessage(0, QString("Meta information of the file '%1' could not be updated").arg(targetFilePath));
			}
		}

		locker.unlock();

		QMutexLocker lockCache(&m_cacheMutex);

		for (int cacheItemIndex = 0; cacheItemIndex < m_dataCache.count(); ++cacheItemIndex){
			if (m_dataCache[cacheItemIndex]->fileId == resourceId){
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


bool CFileRepositoryComp::UpdateContent(
			const istd::IChangeable& fileContents,
			const QByteArray& resourceId,
			ibase::IProgressManager* /*progressManagerPtr*/,
			ilog::IMessageConsumer* /*messageConsumerPtr*/)
{
	QByteArray resourceTypeId = GetResourceTypeId(resourceId);
	if (!resourceTypeId.isEmpty()){
		const ifile::IFilePersistence* persistencePtr = GetPersistenceForResource(resourceTypeId);
		if (persistencePtr != NULL){
			FileInfo fileInfo = GetFileInfo(resourceId);

			QString tempFile = QDir::tempPath() + "/" + QUuid::createUuid().toString() + "." + QFileInfo(fileInfo.filePath).suffix();

			bool retVal = true;
			if (persistencePtr->SaveToFile(fileContents, tempFile) == ifile::IFilePersistence::OS_OK){
				retVal = UpdateFile(tempFile, resourceId);
			}

			QFile::remove(tempFile);

			return retVal;
		}
	}

	return false;
}


int CFileRepositoryComp::GetSupportedEditOperations(const QByteArray& /*resourceTypeId*/) const
{
	return OF_ALL;
}


// reimplemented (IFileRepositoryInfo)

QString CFileRepositoryComp::GetRepositoryPath() const
{
	QString retVal;

	if (m_repositoryPathCompPtr.IsValid()){
		retVal = m_repositoryPathCompPtr->GetPath();
	}

	return retVal;
}


// reimplemented (iser::ISerializable)

bool CFileRepositoryComp::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? NULL: this);

	bool retVal = true;

	if (!archive.IsStoring()){
		m_repositoryLock.lockForWrite();

		m_files.clear();
	}
	else{
		m_repositoryLock.lockForRead();
	}

	static iser::CArchiveTag repositoryItemsTag("RepositoryItems", "List of repository items");
	static iser::CArchiveTag repositoryItemTag("FileItem", "Single file item in the repository");

	int filesCount = m_files.count();

	retVal = retVal && archive.BeginMultiTag(repositoryItemsTag, repositoryItemTag, filesCount);

	if (archive.IsStoring()){
		for (Files::Iterator iter = m_files.begin(); iter != m_files.end(); ++iter){
			retVal = retVal && archive.BeginTag(repositoryItemTag);
			retVal = retVal && (*iter).Serialize(archive);
			retVal = retVal && archive.EndTag(repositoryItemTag);
		}
	}
	else{
		for (int i = 0; i < filesCount; ++i){
			RepositoryItem repositoryItem(*this);

			retVal = retVal && archive.BeginTag(repositoryItemTag);
			retVal = retVal && repositoryItem.Serialize(archive);
			retVal = retVal && archive.EndTag(repositoryItemTag);

			if (retVal){
				QFileInfo fileInfo(repositoryItem.filePathInRepository);
				if (fileInfo.exists()){
					m_files.push_back(repositoryItem);
				}
				else{
					SendWarningMessage(0, QString("File %1 was not found in the repository folder. The repository item will be removed").arg(repositoryItem.filePathInRepository));
				}
			}
		}
	}

	retVal = retVal && archive.EndTag(repositoryItemsTag);

	m_repositoryLock.unlock();

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CFileRepositoryComp::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	QWriteLocker locker(&m_repositoryLock);

	const CFileRepositoryComp* sourcePtr = dynamic_cast<const CFileRepositoryComp*>(&object);
	if (sourcePtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_files = sourcePtr->m_files; 

		locker.unlock();

		return true;
	}

	return false;
}


// static protected methods

QString CFileRepositoryComp::GetInfoFileExtention()
{
	return ".info";
}


QString CFileRepositoryComp::GetDataFileExtention()
{
	return ".xdat";
}


// protected methods

const ifile::IFileResourceValidator* CFileRepositoryComp::GetResourceValidator(const QByteArray& resourceTypeId) const
{
	if (m_validatorMap.contains(resourceTypeId)){
		return m_validatorMap[resourceTypeId];
	}

	return NULL;
}


CFileRepositoryComp::DataObjectPtr CFileRepositoryComp::CreateDataObject(const QByteArray& resourceTypeId) const
{
	istd::IChangeable* dataObjectPtr = CreateInstance(resourceTypeId);
	if (dataObjectPtr != NULL){
		return DataObjectPtr(dataObjectPtr);
	}

	return DataObjectPtr();
}


const ifile::IFilePersistence* CFileRepositoryComp::GetPersistenceForResource(const QByteArray& resourceTypeId) const
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


int CFileRepositoryComp::GetFileIndexById(const QByteArray& fileId) const
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


int CFileRepositoryComp::GetRepositoryItemById(const QByteArray& fileId, RepositoryItem& item) const
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


int CFileRepositoryComp::GetFileIndexByName(const QString& fileName) const
{
	for (int i = 0; i < m_files.count(); ++i){
		if (m_files[i].resourceName == fileName){
			return i;
		}
	}

	return -1;
}


ifile::IFileResourcesManager::DataObjectPtr CFileRepositoryComp::CreateObjectFromFile(const QString& filePath, const QByteArray& resourceTypeId) const
{
	ifile::IFileResourcesManager::DataObjectPtr retVal = CreateDataObject(resourceTypeId);
	if (retVal.IsValid()){
		const ifile::IFilePersistence* filePersistenceCompPtr = GetPersistenceForResource(resourceTypeId);
		if (filePersistenceCompPtr != NULL){
			int loadState = filePersistenceCompPtr->LoadFromFile(*retVal.GetPtr(), filePath);
			if (loadState == ifile::IFilePersistence::OS_OK){
				return retVal;
			}
		}
	}

	return DataObjectPtr();
}


ifile::IFileMetaInfoProvider::MetaInfoPtr CFileRepositoryComp::CreateFileMetaInfo(
			const QString& filePath,
			const QByteArray& typeId,
			const quint32& checkSum) const
{
	ifile::IFileResourcesManager::DataObjectPtr dataObjectPtr;

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


ifile::IFileMetaInfoProvider::MetaInfoPtr CFileRepositoryComp::TryLoadMetaInfoFromFile(const QString& metaInfoFilePath) const
{
	if (QFileInfo(metaInfoFilePath).exists()){
		ifile::IFileMetaInfoProvider::MetaInfoPtr fileMetaInfoPtr(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>);

		if (LoadFileMetaInfo(*fileMetaInfoPtr.GetPtr(), metaInfoFilePath)){
			return fileMetaInfoPtr;
		}
	}

	return ifile::IFileMetaInfoProvider::MetaInfoPtr();
}


void CFileRepositoryComp::UpdateItemMetaInfo(RepositoryItem& item)
{
	QString metaInfoFilePath = GetMetaInfoFilePath(item);
	QFileInfo fileInfo(metaInfoFilePath);

	MetaInfoPtr retVal = TryLoadMetaInfoFromFile(metaInfoFilePath);
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


bool CFileRepositoryComp::SaveMetaInfo(const idoc::IDocumentMetaInfo& metaInfo, const QString& filePath) const
{
	const iser::ISerializable* serializablePtr = dynamic_cast<const iser::ISerializable*>(&metaInfo);
	if (serializablePtr != NULL){
		ifile::CCompactXmlFileWriteArchive archive(filePath, m_versionInfoCompPtr.GetPtr());

		return (const_cast<iser::ISerializable*>(serializablePtr))->Serialize(archive);
	}

	return false;
}


bool CFileRepositoryComp::LoadFileMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QString& filePath) const
{
	iser::ISerializable* serializablePtr = dynamic_cast<iser::ISerializable*>(&metaInfo);
	if (serializablePtr != NULL){
		ifile::CCompactXmlFileReadArchive archive(filePath, m_versionInfoCompPtr.GetPtr());

		return serializablePtr->Serialize(archive);
	}

	return false;
}


QString CFileRepositoryComp::CalculateFolderPathInRepository(
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


QString CFileRepositoryComp::CalculateTargetFilePath(
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

void CFileRepositoryComp::OnComponentCreated()
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

	const ifile::IFileResourceTypeConstraints* resourceTypesPtr = GetResourceTypeConstraints();
	if (resourceTypesPtr != NULL){
		int validatorsCount = qMin(m_resourceValidatorListCompPtr.GetCount(), resourceTypesPtr->GetOptionsCount());
		for (int i = 0; i < validatorsCount; ++i){
			IFileResourceValidator* validatorPtr = m_resourceValidatorListCompPtr[i];
			if (validatorPtr != NULL){
				QByteArray resourceTypeId = resourceTypesPtr->GetOptionId(i);

				m_validatorMap[resourceTypeId] = validatorPtr;
			}
		}
	}

	ReadRepositoryItems();

	QTimer::singleShot(5000, this, SLOT(OnStartValidationByTimer()));
}


void CFileRepositoryComp::OnComponentDestroyed()
{
	m_repositoryLock.lockForWrite();
	m_files.clear();
	m_repositoryLock.unlock();

	QMutexLocker lockCache(&m_cacheMutex);
	m_dataCache.clear();

	BaseClass::OnComponentDestroyed();
}


// private methods

bool CFileRepositoryComp::InsertFileIntoRepository(
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

	QString targetFolderPath = CalculateFolderPathInRepository(filePath, resourceName, resourceTypeId, *m_useSubfolderAttrPtr, *m_useNameCountingAttrPtr, messageConsumerPtr);
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


QString CFileRepositoryComp::SaveRepositoryItem(const RepositoryItem& repositoryItem) const
{
	QString itemFilePath = GetDataItemFilePath(repositoryItem);

	ifile::CCompactXmlFileWriteArchive archive(itemFilePath, m_versionInfoCompPtr.GetPtr());

	if (!const_cast<RepositoryItem&>(repositoryItem).Serialize(archive)){
		SendErrorMessage(0, QString("Repository item could not be saved into '%1'").arg(itemFilePath));

		return QString();
	}

	return itemFilePath;
}


void CFileRepositoryComp::ReadRepositoryItems()
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
		RepositoryItem fileItem(*this);

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


QString CFileRepositoryComp::GetTempDirectory() const
{
	return QDir::tempPath();
}


QString CFileRepositoryComp::GetDataItemFilePath(const RepositoryItem& repositoryFile) const
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


QString CFileRepositoryComp::GetMetaInfoFilePath(const RepositoryItem& repositoryFile) const
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


QString CFileRepositoryComp::ShortenWindowsFilename(const QString& fileName, const QFileInfo& fileInfo, const QString& prefix) const
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


QStringList CFileRepositoryComp::ConvertMessageContainer(ilog::CMessageContainer& messageContainer)
{
	QStringList messages;

	ilog::CMessageContainer::Messages containerMessages = messageContainer.GetMessages();
	for (int i = 0; i < containerMessages.count(); ++i){
		messages.push_back(containerMessages[i]->GetInformationDescription());
	}

	return messages;
}


void CFileRepositoryComp::OnFileRemove(const QByteArray& fileId)
{
	RemoveFile(fileId);
}


// public methods of the embedded class ResourceTypeConstraints

CFileRepositoryComp::ResourceTypeConstraints::ResourceTypeConstraints()
	:m_parentPtr(NULL)
{
}


void CFileRepositoryComp::ResourceTypeConstraints::SetParent(CFileRepositoryComp* parentPtr)
{
	m_parentPtr = parentPtr;
}


// reimplemented (IFileResourceTypeConstraints)

const ifile::IFileTypeInfo* CFileRepositoryComp::ResourceTypeConstraints::GetFileTypeInfo(int resourceTypeIndex) const
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

int CFileRepositoryComp::ResourceTypeConstraints::GetOptionsFlags() const
{
	Q_ASSERT(m_parentPtr != NULL);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionsFlags();
}


int CFileRepositoryComp::ResourceTypeConstraints::GetOptionsCount() const
{
	Q_ASSERT(m_parentPtr != NULL);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionsCount();
}


QString CFileRepositoryComp::ResourceTypeConstraints::GetOptionName(int index) const
{
	Q_ASSERT(m_parentPtr != NULL);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionName(index);
}


QString CFileRepositoryComp::ResourceTypeConstraints::GetOptionDescription(int index) const
{
	Q_ASSERT(m_parentPtr != NULL);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionDescription(index);
}


QByteArray CFileRepositoryComp::ResourceTypeConstraints::GetOptionId(int index) const
{
	Q_ASSERT(m_parentPtr != NULL);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionId(index);
}


bool CFileRepositoryComp::ResourceTypeConstraints::IsOptionEnabled(int index) const
{
	Q_ASSERT(m_parentPtr != NULL);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->IsOptionEnabled(index);
}


// public methods of embedded class RepositoryItem

// reimplement (iser::ISerializable)

bool CFileRepositoryComp::RepositoryItem::Serialize(iser::IArchive& archive)
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

bool CFileRepositoryComp::RepositoryItem::CopyFrom(const istd::IChangeable& object, CompatibilityMode mode)
{
	const RepositoryItem* sourceItemPtr = dynamic_cast<const RepositoryItem*>(&object);
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


} // namespace imtrepo


