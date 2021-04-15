#include <imtrepo/CFileCollectionCompBase.h>


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
#include <imtcore/Version.h>
#include <imtbase/CObjectCollectionInsertEvent.h>
#include <imtbase/CObjectCollectionUpdateEvent.h>
#include <imtbase/CObjectCollectionRemoveEvent.h>


namespace imtrepo
{


// public methods of the class CFileCollectionCompBase2

CFileCollectionCompBase2::CFileCollectionCompBase2()
	:m_filesLock(QReadWriteLock::Recursive),
	m_readerThread(this)
{
	m_resourceTypeConstraints.SetParent(this);
}


// reimplemented (IFileCollectionInfo)

QString CFileCollectionCompBase2::GetCollectionRootFolder() const
{
	QString retVal;

	if (m_repositoryPathCompPtr.IsValid()){
		retVal = m_repositoryPathCompPtr->GetPath();
	}

	return retVal;
}


int CFileCollectionCompBase2::GetRepositoryRevision() const
{
	return *m_revisionAttrPtr;
}


// reimplemented (IFileObjectCollection)

const ifile::IFileResourceTypeConstraints* CFileCollectionCompBase2::GetFileTypeConstraints() const
{
	if (!m_resourceFileTypesCompPtr.IsValid() || !m_resourceTypesCompPtr.IsValid()){
		return nullptr;
	}

	return &m_resourceTypeConstraints;
}


IFileObjectCollection::FileInfo CFileCollectionCompBase2::GetFileInfo(const QByteArray& objectId) const
{
	FileInfo retVal;

	QReadLocker locker(&m_filesLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		CollectionItem collectionItem = m_files[fileIndex];

		locker.unlock();

		QString repositoryFilePath = collectionItem.filePathInRepository;

		retVal.fileName = QFileInfo(repositoryFilePath).fileName();
		retVal.filePath = repositoryFilePath;
		retVal.lastModified = collectionItem.metaInfo.GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME).toDateTime();
	}

	return retVal;
}


QString CFileCollectionCompBase2::GetFile(
	const QByteArray& objectId,
	const QString& targetFilePath) const
{
	QReadLocker locker(&m_filesLock);

	QString outputFilePath = targetFilePath;

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		QString filePathInRepository = m_files[fileIndex].filePathInRepository;

		QFileInfo fileInfo(filePathInRepository);
		if (fileInfo.exists()){
			// If the output file path is empty, then create the file path automatically:
			if (outputFilePath.isEmpty()){
				QString fileExtension = QFileInfo(filePathInRepository).suffix();

				outputFilePath = QDir::tempPath() + "/ImtCore/" + QFileInfo(filePathInRepository).completeBaseName() + "_" + QUuid::createUuid().toString() + "." + fileExtension;
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


QByteArray CFileCollectionCompBase2::InsertFile(
			const QString& localFilePath,
			const QByteArray& typeId,
			const QString& objectName,
			const QString& objectDescription,
			const QByteArray& proposedObjectId)
{
	static QByteArray emptyId;

	if (IsPathInsideRepository(localFilePath)){
		return emptyId;
	}

	QWriteLocker locker(&m_filesLock);

	if (IsObjectIdUsed(proposedObjectId)){
		return emptyId;
	}

	// Generate unique file-ID accroding to \c proposedObjectId variable provided by caller:
	QByteArray fileId = proposedObjectId.isEmpty() ? QUuid::createUuid().toByteArray() : proposedObjectId;

	// Generate teraget absolute file path for the data file:
	QString targetFilePath = CalculateTargetFilePath(localFilePath, objectName, typeId);
	if (targetFilePath.isEmpty()){
		return emptyId;
	}

	QFileInfo targetFileInfo(targetFilePath);
	QString targetName = targetFileInfo.completeBaseName();

	// Check fileId and targetName for locking
	if (IsObjectIdLocked(fileId)){
		SendVerboseMessage(tr("Object-ID '%1' is locked").arg(proposedObjectId.constData()));

		return emptyId;
	}

	if (IsObjectNameLocked(targetName)){
		SendVerboseMessage(tr("Object Name '%1' is locked").arg(targetName));

		return emptyId;
	}

	// Reserve objectId and objectName during running insert-transaction:
	ResourceLocker resourceLocker(*this, fileId, targetName);

	// Release access to collection items:
	locker.unlock();

	// Create structure of the target files in the temporary location:
	QString workingPath = CreateWorkingDir();
	if (workingPath.isEmpty()){
		return emptyId;
	}

	QString workingFilePath = workingPath + "/" + targetFileInfo.fileName();
	if (istd::CSystem::FileCopy(localFilePath, workingFilePath)){
		if (QFile::setPermissions(workingFilePath,
			QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner |
			QFile::ReadUser | QFile::WriteUser | QFile::ExeUser |
			QFile::ReadGroup | QFile::WriteGroup | QFile::ExeGroup |
			QFile::ReadOther | QFile::WriteOther | QFile::ExeOther)){
			QString metaInfoFilePath = workingPath + "/" + targetFileInfo.fileName() + "." + GetRepositoryInfo().metaInfoFileSuffix;

			CollectionItem collectionItem(GetCollectionRootFolder(), *m_revisionAttrPtr);
			collectionItem.fileId = fileId;
			collectionItem.typeId = typeId;
			collectionItem.objectName = targetName;
			collectionItem.filePathInRepository = targetFilePath;
			collectionItem.sourceFilePath = localFilePath;

			collectionItem.metaInfo.SetMetaInfo(MIT_LAST_OPERATION_TIME, QDateTime::currentDateTime());
			collectionItem.metaInfo.SetMetaInfo(MIT_INSERTION_TIME, QDateTime::currentDateTime());
			collectionItem.metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION, objectDescription);

			MetaInfoPtr metaInfoPtr = CreateItemMetaInfo(workingFilePath, typeId);
			bool metaInfoCreated = SaveMetaInfo(*metaInfoPtr, metaInfoFilePath);

			if (metaInfoCreated){
				collectionItem.contentsMetaInfoPtr = metaInfoPtr;

				QString dataFilePath = workingPath + "/" + targetFileInfo.fileName() + "." + GetRepositoryInfo().dataFileSuffix;
				QString savedPath = SaveCollectionItem(collectionItem, dataFilePath);

				// Inserting new item into collection if the structure was created successfully:
				if (!savedPath.isEmpty()){
					if (FinishInsertFileTransaction(workingPath, targetFileInfo.dir().absolutePath(), fileId, collectionItem)){
						QDir(workingPath).removeRecursively();

						return fileId;
					}
					else{
						SendErrorMessage(0, tr("File could not be inserted into the repository"));
					}
				}
			}
			else{
				SendErrorMessage(0, tr("Metainfo for the file '%1' could not be created").arg(workingFilePath));
			}
		}
		else{
			SendErrorMessage(0, tr("Permissions for the file '%1' could not be set").arg(workingFilePath));
		}
	}
	else{
		SendErrorMessage(0, tr("Can't copy the file '%1' to working folder").arg(localFilePath));
	}

	QDir(workingPath).removeRecursively();

	return emptyId;
}


bool CFileCollectionCompBase2::UpdateFile(
	const QString& localFilePath,
	const QByteArray& objectId)
{
	QWriteLocker locker(&m_filesLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex < 0){
		SendErrorMessage(0, QString("No file with the ID '%1' was found in the repository. File could not be updated").arg(objectId.constData()));

		return false;
	}

	CollectionItem collectionItem = m_files[fileIndex];

	locker.unlock();

	// Create meta-informations for the file contents:
	collectionItem.contentsMetaInfoPtr = CreateItemMetaInfo(localFilePath, collectionItem.typeId);
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


bool CFileCollectionCompBase2::ExportFile(const QByteArray& objectId, const QString& targetFilePath) const
{
	if (!targetFilePath.isEmpty()){
		if (!IsPathInsideRepository(targetFilePath)){
			QString resultPath = GetFile(objectId, targetFilePath);

			return resultPath == targetFilePath;
		}
	}

	return false;
}


QByteArray CFileCollectionCompBase2::ImportFile(const QByteArray& /*typeId*/, const QString& /*sourceFilePath*/)
{
	return QByteArray();
}


// reimplemented (IObjectCollection)

const imtbase::IRevisionController* CFileCollectionCompBase2::GetRevisionController() const
{
	return nullptr;
}


int CFileCollectionCompBase2::GetOperationFlags(const QByteArray& /*objectId*/) const
{
	return OF_SUPPORT_DELETE | OF_SUPPORT_EDIT | OF_SUPPORT_INSERT | OF_SUPPORT_RENAME;
}


bool CFileCollectionCompBase2::GetDataMetaInfo(const QByteArray& objectId, ifile::IFileMetaInfoProvider::MetaInfoPtr& metaInfoPtr) const
{
	QReadLocker lockCollection(&m_filesLock);

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


QByteArray CFileCollectionCompBase2::InsertNewObject(
	const QByteArray& typeId,
	const QString& name,
	const QString& description,
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

			QString targetFolder = CreateWorkingDir();
			if (targetFolder.isEmpty()){
				SendErrorMessage(0, QString(QObject::tr("Target folder '%1' could not be created")).arg(targetFolder));

				return QByteArray();
			}

			QString tempFilePath = targetFolder + "/" + QUuid::createUuid().toString() + "." + supportedExts[0];

			if (persistencePtr->SaveToFile(*newObjectPtr, tempFilePath) == ifile::IFilePersistence::OS_OK){
				QByteArray retval = InsertFile(tempFilePath, typeId, name, description, proposedObjectId);

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


bool CFileCollectionCompBase2::RemoveObject(const QByteArray& /*objectId*/)
{
	return false;
}


const istd::IChangeable* CFileCollectionCompBase2::GetObjectPtr(const QByteArray& /*objectId*/) const
{
	return nullptr;
}


bool CFileCollectionCompBase2::GetObjectData(const QByteArray& objectId, DataPtr& dataPtr) const
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


bool CFileCollectionCompBase2::SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode /*mode*/)
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

	QString tempFilePath = QDir::tempPath() + "/ImtCore/" + QUuid::createUuid().toString() + "." + extension;

	if (persistencePtr->SaveToFile(object, tempFilePath) == ifile::IFilePersistence::OS_OK){
		bool retVal = UpdateFile(tempFilePath, objectId);

		QFile::remove(tempFilePath);

		return retVal;
	}

	return false;
}


void CFileCollectionCompBase2::SetObjectName(const QByteArray& /*objectId*/, const QString& /*objectName*/)
{
}


void CFileCollectionCompBase2::SetObjectDescription(const QByteArray& objectId, const QString& objectDescription)
{
	QWriteLocker locker(&m_filesLock);

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


void CFileCollectionCompBase2::SetObjectEnabled(const QByteArray& /*objectId*/, bool /*isEnabled*/)
{
}


bool CFileCollectionCompBase2::RegisterEventHandler(imtbase::IObjectCollectionEventHandler* eventHandler)
{
	if (eventHandler != nullptr && !m_eventHandlerList.contains(eventHandler)){
		m_eventHandlerList.append(eventHandler);

		eventHandler->OnCollectionConnected(this);

		return true;
	}

	return false;
}


bool CFileCollectionCompBase2::UnregisterEventHandler(imtbase::IObjectCollectionEventHandler* eventHandler)
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

bool CFileCollectionCompBase2::GetCollectionItemMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& metaInfo) const
{
	QReadLocker lockCollection(&m_filesLock);

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


const iprm::IOptionsList* CFileCollectionCompBase2::GetObjectTypesInfo() const
{
	return &m_resourceTypeConstraints;
}


imtbase::IObjectCollectionInfo::Id CFileCollectionCompBase2::GetObjectTypeId(const QByteArray& objectId) const
{
	m_filesLock.lockForRead();

	QByteArray retVal;

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		retVal = m_files[fileIndex].typeId;
	}

	m_filesLock.unlock();

	return retVal;
}


// reimplemented (ICollectionInfo)

imtbase::ICollectionInfo::Ids CFileCollectionCompBase2::GetElementIds() const
{
	Ids retVal;

	m_filesLock.lockForRead();

	for (int fileIndex = 0; fileIndex < m_files.count(); ++fileIndex){
		retVal.append(m_files[fileIndex].fileId);
	}

	m_filesLock.unlock();

	return retVal;
}


QVariant CFileCollectionCompBase2::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	QReadLocker locker(&m_filesLock);

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


// static protected methods of the class CFileCollectionCompBase

CFileCollectionCompBase2::RepositoryInfo CFileCollectionCompBase2::GetRepositoryInfo()
{
	RepositoryInfo retVal;

	retVal.metaInfoFileSuffix = "meta";
	retVal.dataFileSuffix = "item";

	return retVal;
}


// protected methods of the class CFileCollectionCompBase

void CFileCollectionCompBase2::EnumerateRepositoryItems(QFileInfoList& fileList) const
{
	QString repositoryRootPath = GetCollectionRootFolder();
	QDir repositoryRootDir(repositoryRootPath);

	ifile::CFileListProviderComp::CreateFileList(repositoryRootDir, 0, 2, QStringList() << QString("*.%1").arg(GetRepositoryInfo().dataFileSuffix), QDir::Name, fileList);
}


QString CFileCollectionCompBase2::CreateWorkingDir() const
{
	QString workingPath = QDir::tempPath() + "/ImtCore/" + QUuid::createUuid().toString();

	if (istd::CSystem::EnsurePathExists(workingPath)){
		return workingPath;
	}

	return QString();
}


istd::IChangeable* CFileCollectionCompBase2::CreateDataObject(const QByteArray& typeId) const
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


istd::IChangeable* CFileCollectionCompBase2::CreateObjectFromFile(const QString& filePath, const QByteArray& typeId) const
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


const ifile::IFilePersistence* CFileCollectionCompBase2::GetPersistenceForResource(const QByteArray& typeId) const
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


QString CFileCollectionCompBase2::SaveCollectionItem(const CollectionItem& collectionItem, const QString& dataFilePath) const
{
	QString itemFilePath = dataFilePath.isEmpty() ? GetDataItemFilePath(collectionItem) : dataFilePath;

	ifile::CCompactXmlFileWriteArchive archive(itemFilePath, m_versionInfoCompPtr.GetPtr());

	if (!const_cast<CollectionItem&>(collectionItem).Serialize(archive)){
		SendErrorMessage(0, QString("Collection item could not be saved into '%1'").arg(itemFilePath));

		return QString();
	}

	return itemFilePath;
}


QString CFileCollectionCompBase2::GetDataItemFilePath(const CollectionItem& repositoryFile) const
{
	QString retVal;

	QFileInfo fileInfo(repositoryFile.filePathInRepository);
	QString baseName = fileInfo.fileName();
	if (!baseName.isEmpty()){
		retVal = fileInfo.absolutePath() + "/" + baseName + "." + GetRepositoryInfo().dataFileSuffix;
	}

	return retVal;
}


QString CFileCollectionCompBase2::GetMetaInfoFilePath(const CollectionItem& repositoryFile) const
{
	QString retVal;

	QFileInfo fileInfo(repositoryFile.filePathInRepository);
	QString baseName = fileInfo.fileName();
	if (!baseName.isEmpty()){
		retVal = fileInfo.absolutePath() + "/" + baseName + "." + GetRepositoryInfo().metaInfoFileSuffix;
	}

	return retVal;
}


QString CFileCollectionCompBase2::CalculateShortFileName(const QString& fileName, const QFileInfo& fileInfo, const QString& prefix) const
{
	const int maxFileNameLength = 255;

	int fileSuffixSize = qMax(GetRepositoryInfo().dataFileSuffix.size(), GetRepositoryInfo().metaInfoFileSuffix.size()) + 1;

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


bool CFileCollectionCompBase2::SaveMetaInfo(const idoc::IDocumentMetaInfo& metaInfo, const QString& metaInfoFilePath) const
{
	const iser::ISerializable* serializablePtr = dynamic_cast<const iser::ISerializable*>(&metaInfo);
	if (serializablePtr != nullptr){
		ifile::CCompactXmlFileWriteArchive archive(metaInfoFilePath, m_versionInfoCompPtr.GetPtr());

		return (const_cast<iser::ISerializable*>(serializablePtr))->Serialize(archive);
	}

	return false;
}


bool CFileCollectionCompBase2::LoadMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QString& metaInfoFilePath) const
{
	if (QFile::exists(metaInfoFilePath)){
		iser::ISerializable* serializablePtr = dynamic_cast<iser::ISerializable*>(&metaInfo);
		if (serializablePtr != nullptr){
			ifile::CCompactXmlFileReadArchive archive(metaInfoFilePath, m_versionInfoCompPtr.GetPtr());

			return serializablePtr->Serialize(archive);
		}
	}

	return false;
}


bool CFileCollectionCompBase2::IsPathInsideRepository(const QString& filePath) const
{
	QString cleanFileDir = QDir::cleanPath(QFileInfo(filePath).absolutePath());
	QString cleanRepositoryDir = QDir::cleanPath(GetCollectionRootFolder());

	return cleanFileDir.contains(cleanRepositoryDir);
}


int CFileCollectionCompBase2::GetFileIndexById(const QByteArray& fileId) const
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


int CFileCollectionCompBase2::GetFileIndexByName(const QString& fileName) const
{
	for (int i = 0; i < m_files.count(); ++i){
		if (m_files[i].objectName == fileName){
			return i;
		}
	}

	return -1;
}


imtbase::IObjectCollection::MetaInfoPtr CFileCollectionCompBase2::CreateItemMetaInfo(const QString& dataObjectFilePath, const QByteArray& typeId) const
{
	IFileObjectCollection::MetaInfoPtr retVal;

	QFileInfo fileInfo(dataObjectFilePath);
	if (fileInfo.exists()){
		if (m_metaInfoCreatorMap.contains(typeId)){
			istd::TDelPtr<istd::IChangeable> dataObjectPtr(CreateObjectFromFile(dataObjectFilePath, typeId));
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
				retVal->SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CONTENT_CHECKSUM, istd::CCrcCalculator::GetCrcFromFile(dataObjectFilePath));
			}
		}
	}

	return retVal;
}


bool CFileCollectionCompBase2::CreateItemMetaInfoFile(const QString& dataObjectFilePath, const QByteArray& typeId, const QString& metaInfoFilePath) const
{
	IFileObjectCollection::MetaInfoPtr metaInfoPtr = CreateItemMetaInfo(dataObjectFilePath, typeId);
	if (metaInfoPtr.IsValid()){
		if (SaveMetaInfo(*metaInfoPtr, metaInfoFilePath)){
			return true;
		}
	}

	return false;
}


void CFileCollectionCompBase2::UpdateItemMetaInfo(CollectionItem& item) const
{
	QString metaInfoFilePath = GetMetaInfoFilePath(item);
	QFileInfo fileInfo(metaInfoFilePath);

	bool retVal = false;

	if (item.contentsMetaInfoPtr.IsValid()){
		retVal = LoadMetaInfo(*item.contentsMetaInfoPtr, metaInfoFilePath);
	}

	if (!retVal){
		retVal = CreateItemMetaInfoFile(item.filePathInRepository, item.typeId, metaInfoFilePath);
		if (!retVal){
			SendErrorMessage(0, QString("Meta-information for the file '%1' could not be created. Meta-information could not be provided").arg(metaInfoFilePath), "File Collection");
		}
	}
}


void CFileCollectionCompBase2::ReadRepositoryItems()
{
	istd::CChangeNotifier changeNotifier(this, &istd::IChangeable::GetAllChanges());

	QFileInfoList fileList;
	EnumerateRepositoryItems(fileList);

	{
		QWriteLocker locker(&m_filesLock);
		m_files.clear();
		for (int i = 0; i < fileList.count(); ++i){
			ReadItem(m_files, fileList[i].absoluteFilePath());
		}
	}
}


void CFileCollectionCompBase2::ReadItem(Files& filesPtr, const QString& itemFilePath)
{
	CollectionItem fileItem(GetCollectionRootFolder(), *m_revisionAttrPtr);
	if (!ReadItemFile(fileItem, itemFilePath)){
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
			LoadMetaInfo(*fileItem.contentsMetaInfoPtr, GetMetaInfoFilePath(fileItem));

			filesPtr.push_back(fileItem);
		}
	}
	else{
		SendErrorMessage(0, QString("File '%1' doesn't exist. Collection item was automatically removed").arg(fileItem.filePathInRepository));
	}
}


bool CFileCollectionCompBase2::ReadItemFile(CollectionItem& collectionItem, const QString& itemFilePath)
{
	ifile::CCompactXmlFileReadArchive archive(itemFilePath, m_versionInfoCompPtr.GetPtr());

	return collectionItem.Serialize(archive);
}


void CFileCollectionCompBase2::StartRepositoryLoader()
{
	if (m_progressManagerListCompPtr.IsValid()){
		for (int i = 0; i < m_progressManagerListCompPtr.GetCount(); i++){
			m_progressManagerListCompPtr[i]->BeginProgressSession("FileCollection", "Start reading file collection");
		}
	}

	m_readerThread.start();
}


void CFileCollectionCompBase2::OnReaderProgress(int progress)
{
	if (m_progressManagerListCompPtr.IsValid()){
		for (int i = 0; i < m_progressManagerListCompPtr.GetCount(); i++){
			m_progressManagerListCompPtr[i]->OnProgress(0, progress);
		}
	}
}


void CFileCollectionCompBase2::OnReaderFinished()
{
	istd::CChangeNotifier changeNotifier(this, &istd::IChangeable::GetAllChanges());

	QWriteLocker locker(&m_filesLock);
	m_files.append(m_readerFiles);
	locker.unlock();

	if (m_progressManagerListCompPtr.IsValid()){
		for (int i = 0; i < m_progressManagerListCompPtr.GetCount(); i++){
			m_progressManagerListCompPtr[i]->EndProgressSession(0);
		}
	}
}


void CFileCollectionCompBase2::OnReaderInterrupted()
{
}


bool CFileCollectionCompBase2::IsObjectIdLocked(const QByteArray& resourceId)
{
	QMutexLocker locker(&m_lockedObjectInfoMutex);

	return m_lockedObjectIds.contains(resourceId);
}


bool CFileCollectionCompBase2::IsObjectNameLocked(const QString& resourceName)
{
	QMutexLocker locker(&m_lockedObjectInfoMutex);

	return m_lockedObjectNames.contains(resourceName);
}


bool CFileCollectionCompBase2::IsObjectIdUsed(const QByteArray& objectId)
{
	if (objectId.isEmpty()){
		return false;
	}

	for (int i = 0; i < m_files.count(); ++i){
		if (m_files[i].fileId == objectId){
			return true;
		}
	}

	return IsObjectIdLocked(objectId);
}


bool CFileCollectionCompBase2::FinishInsertFileTransaction(
	const QString& workingPath,
	const QString& repositoryPath,
	const QByteArray& fileId,
	const CollectionItem& collectionItem)
{
	bool result = false;

	if (istd::CSystem::EnsurePathExists(repositoryPath)){
		result = istd::CSystem::CopyDirectory(workingPath, repositoryPath);
		if (result){
			{
				static ChangeSet changes(CF_ADDED);
				istd::CChangeNotifier changeNotifier(this, &changes);

				QWriteLocker locker(&m_filesLock);
				m_files.push_back(collectionItem);
				locker.unlock();
			}

			imtbase::CObjectCollectionInsertEvent event(fileId);
			for (imtbase::IObjectCollectionEventHandler* eventHandlerPtr : m_eventHandlerList){
				eventHandlerPtr->OnObjectCollectionEvent(this, &event);
			}
		}
		else{
			QDir(repositoryPath).removeRecursively();
		}
	}

	return result;
}


// reimplemented (icomp::CComponentBase)

void CFileCollectionCompBase2::OnComponentCreated()
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

	QString path = GetCollectionRootFolder();

	// Ensure, that the repository folder was created:
	if (!path.isEmpty()){
		if (!istd::CSystem::EnsurePathExists(path)){
			SendCriticalMessage(0, QString("Root folder for the file collection could not be created in '%1'").arg(path));
		}
	}

	if (m_eventHandlerListCompPtr.IsValid()){
		for (int i = 0; i < m_eventHandlerListCompPtr.GetCount(); i++){
			RegisterEventHandler(m_eventHandlerListCompPtr[i]);
		}
	}
}


void CFileCollectionCompBase2::OnComponentDestroyed()
{
	m_readerThread.requestInterruption();
	m_readerThread.wait();

	m_filesLock.lockForWrite();
	m_eventHandlerList.clear();
	m_files.clear();
	m_filesLock.unlock();

	BaseClass::OnComponentDestroyed();
}


// public methods of the embedded class RepositoryItemInfo

void CFileCollectionCompBase2::RepositoryItemInfo::SetRepositoryItemFilePath(RepositoryFileType fileId, const QString& filePath)
{
	m_files[int(fileId)] = filePath;
}


// reimplemented (IRepositoryItemInfo)

IRepositoryItemInfo::RepositoryFileTypes CFileCollectionCompBase2::RepositoryItemInfo::GetRepositoryItemFileTypes() const
{
	IRepositoryItemInfo::RepositoryFileTypes types;

	for (int type : m_files.keys()){
		types.insert((RepositoryFileType)type);
	}

	return types;
}


QString CFileCollectionCompBase2::RepositoryItemInfo::GetRepositoryItemFilePath(RepositoryFileType fileId) const
{
	if (m_files.contains(fileId)){
		return m_files[fileId];
	}

	return QString();
}


// public methods of the embedded class RepositoryItemInfoProvider

CFileCollectionCompBase2::RepositoryItemInfoProvider::RepositoryItemInfoProvider(CFileCollectionCompBase2& parent)
	:m_parent(parent)
{
}


bool CFileCollectionCompBase2::RepositoryItemInfoProvider::UpdateItems()
{
	QFileInfoList fileList;
	m_parent.EnumerateRepositoryItems(fileList);

	QWriteLocker locker(&m_lock);
	m_repositoryItems.clear();

	for (QFileInfo& file : fileList){
		QString itemBasePath = file.absolutePath() + "/" + file.completeBaseName();

		RepositoryItemInfo itemInfo;
		itemInfo.SetRepositoryItemFilePath(IRepositoryItemInfo::RFT_INFO, itemBasePath + '.' + m_parent.GetRepositoryInfo().dataFileSuffix);
		itemInfo.SetRepositoryItemFilePath(IRepositoryItemInfo::RFT_DATA, itemBasePath);
		itemInfo.SetRepositoryItemFilePath(IRepositoryItemInfo::RFT_DATA_METAINFO, itemBasePath + "." + m_parent.GetRepositoryInfo().metaInfoFileSuffix);

		Item item;
		item.id = QUuid::createUuid().toByteArray();
		item.itemInfo = itemInfo;

		m_repositoryItems.append(item);
	}

	return true;
}


// reimplemented (IRepositoryItemInfoProvider)

const imtbase::ICollectionInfo& CFileCollectionCompBase2::RepositoryItemInfoProvider::GetRepositoryItems()
{
	return *this;
}


const IRepositoryItemInfo* CFileCollectionCompBase2::RepositoryItemInfoProvider::GetRepositoryItemInfo(const QByteArray& itemId) const
{
	QReadLocker locker(&m_lock);

	for (const Item& item : m_repositoryItems){
		if (item.id == itemId){
			return &item.itemInfo;
		}
	}

	return nullptr;
}


// public methods of the embedded class ResourceTypeConstraints

CFileCollectionCompBase2::ResourceTypeConstraints::ResourceTypeConstraints()
	:m_parentPtr(nullptr)
{
}


void CFileCollectionCompBase2::ResourceTypeConstraints::SetParent(CFileCollectionCompBase2* parentPtr)
{
	m_parentPtr = parentPtr;
}


// reimplemented (IFileResourceTypeConstraints)

const ifile::IFileTypeInfo* CFileCollectionCompBase2::ResourceTypeConstraints::GetFileTypeInfo(int typeIndex) const
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

int CFileCollectionCompBase2::ResourceTypeConstraints::GetOptionsFlags() const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionsFlags();
}


int CFileCollectionCompBase2::ResourceTypeConstraints::GetOptionsCount() const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionsCount();
}


QString CFileCollectionCompBase2::ResourceTypeConstraints::GetOptionName(int index) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionName(index);
}


QString CFileCollectionCompBase2::ResourceTypeConstraints::GetOptionDescription(int index) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionDescription(index);
}


QByteArray CFileCollectionCompBase2::ResourceTypeConstraints::GetOptionId(int index) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionId(index);
}


bool CFileCollectionCompBase2::ResourceTypeConstraints::IsOptionEnabled(int index) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->IsOptionEnabled(index);
}


// public methods of embedded class QResourceLocker

CFileCollectionCompBase2::ResourceLocker::ResourceLocker(
			CFileCollectionCompBase2& collection,
			const QByteArray& objectId,
			const QString& objectName)
			:m_collection(collection)
{
	QMutexLocker locker(&m_collection.m_lockedObjectInfoMutex);

	if (!objectId.isEmpty()){
		m_resourceId = objectId;
		m_collection.m_lockedObjectIds.append(objectId);
	}

	if (!objectName.isEmpty()){
		m_resourceName = objectName;
		m_collection.m_lockedObjectNames.append(objectName);
	}
}


CFileCollectionCompBase2::ResourceLocker::~ResourceLocker()
{
	QMutexLocker locker(&m_collection.m_lockedObjectInfoMutex);

	if (!m_resourceId.isEmpty()){
		Q_ASSERT(m_collection.m_lockedObjectIds.removeOne(m_resourceId));
	}

	if (!m_resourceName.isEmpty()){
		Q_ASSERT(m_collection.m_lockedObjectNames.removeOne(m_resourceName));
	}
}


// public methods of embedded class ReaderThread

CFileCollectionCompBase2::ReaderThread::ReaderThread(CFileCollectionCompBase2* parentPtr)
	:m_parentPtr(parentPtr)
{
}


// private methods of embedded class ReaderThread

// reimplemented (QThread)

void CFileCollectionCompBase2::ReaderThread::run()
{
	QFileInfoList fileList;
	m_parentPtr->EnumerateRepositoryItems(fileList);

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


} // namespace imtrepo


