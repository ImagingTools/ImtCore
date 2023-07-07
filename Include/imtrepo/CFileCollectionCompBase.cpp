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
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtcore/Version.h>
#include <imtbase/CTempDir.h>
#include <imtbase/CObjectCollection.h>


namespace imtrepo
{


// public methods of the class CFileCollectionCompBase

CFileCollectionCompBase::CFileCollectionCompBase()
	:m_filesLock(QReadWriteLock::Recursive),
	m_readerThread(this),
	m_itemInfoProvider(*this)
{
	m_resourceTypeConstraints.SetParent(this);
}


// reimplemented (IRepositoryItemInfoProvider)

const imtbase::ICollectionInfo& CFileCollectionCompBase::GetRepositoryItems()
{
	return m_itemInfoProvider;
}


const IRepositoryItemInfo* CFileCollectionCompBase::GetRepositoryItemInfo(const QByteArray& itemId) const
{
	return m_itemInfoProvider.GetRepositoryItemInfo(itemId);
}


// reimplemented (IFileCollectionInfo)

QString CFileCollectionCompBase::GetCollectionRootFolder() const
{
	QString retVal;

	if (m_repositoryPathCompPtr.IsValid()){
		retVal = m_repositoryPathCompPtr->GetPath();
	}

	return retVal;
}


int CFileCollectionCompBase::GetRepositoryRevision() const
{
	return *m_revisionAttrPtr;
}


// reimplemented (IFileObjectCollection)

const ifile::IFileResourceTypeConstraints* CFileCollectionCompBase::GetFileTypeConstraints() const
{
	if (!m_resourceFileTypesCompPtr.IsValid() || !m_resourceTypesCompPtr.IsValid()){
		return nullptr;
	}

	return &m_resourceTypeConstraints;
}


IFileObjectCollection::FileInfo CFileCollectionCompBase::GetFileInfo(const QByteArray& objectId) const
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


QString CFileCollectionCompBase::GetFile(
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
				SendErrorMessage(0, QObject::tr("File could not be copied from '%1' to '%2'").arg(filePathInRepository).arg(targetFilePath));
			}
		}
		else{
			SendErrorMessage(0, QObject::tr("File doesn't exist '%1'").arg(filePathInRepository));
		}
	}

	return QString();
}


QByteArray CFileCollectionCompBase::InsertFile(
			const QString& localFilePath,
			const QByteArray& typeId,
			const QString& objectName,
			const QString& objectDescription,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr)
{
	static QByteArray emptyId;

	if (IsPathInsideRepository(localFilePath)){
		return emptyId;
	}

	QWriteLocker locker(&m_filesLock);

	// Generate unique file-ID accroding to \c proposedObjectId variable provided by caller:
	QByteArray fileId = (IsObjectIdUsed(proposedObjectId) || proposedObjectId.isEmpty()) ? QUuid::createUuid().toByteArray() : proposedObjectId;

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

			if (collectionItemMetaInfoPtr != nullptr){
				idoc::CStandardDocumentMetaInfo::MetaInfoTypes metaInfoTypes = collectionItemMetaInfoPtr->GetMetaInfoTypes();
#if QT_VERSION < 0x051500
				QList<int> metaInfoTypesList = metaInfoTypes.toList();
#else
				QList<int> metaInfoTypesList(metaInfoTypes.begin(), metaInfoTypes.end());
#endif
				for (int i = 0; i < metaInfoTypesList.size(); i++){
					QVariant valueMeta = collectionItemMetaInfoPtr->GetMetaInfo(metaInfoTypesList[i]);
					collectionItem.metaInfo.SetMetaInfo(metaInfoTypesList[i], valueMeta);
				}
			}
			else{
				collectionItem.metaInfo.SetMetaInfo(MIT_LAST_OPERATION_TIME, QDateTime::currentDateTime());
				collectionItem.metaInfo.SetMetaInfo(MIT_INSERTION_TIME, QDateTime::currentDateTime());
				collectionItem.metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION, objectDescription);
			}

			idoc::MetaInfoPtr metaInfoPtr = CreateItemMetaInfo(workingFilePath, typeId);
			if (dataMetaInfoPtr != nullptr){
				idoc::CStandardDocumentMetaInfo::MetaInfoTypes dataMetaInfoTypes = dataMetaInfoPtr->GetMetaInfoTypes();
#if QT_VERSION < 0x051500
				QList<int> dataMetaInfoTypesList = dataMetaInfoTypes.toList();
#else
				QList<int> dataMetaInfoTypesList(dataMetaInfoTypes.begin(), dataMetaInfoTypes.end());
#endif
				for (int i = 0; i < dataMetaInfoTypesList.size(); i++){
					QVariant dataValueMeta = dataMetaInfoPtr->GetMetaInfo(dataMetaInfoTypesList[i]);
					metaInfoPtr->SetMetaInfo(dataMetaInfoTypesList[i], dataValueMeta);
				}
			}
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


bool CFileCollectionCompBase::UpdateFile(
	const QString& localFilePath,
	const QByteArray& objectId)
{
	{
		QWriteLocker cacheLocker(&m_objectCacheLock);

		if (m_objectCache.contains(objectId)){
			m_objectCache.remove(objectId);
		}
	}

	QWriteLocker locker(&m_filesLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex < 0){
		SendErrorMessage(0, QObject::tr("No file with the ID '%1' was found in the repository. File could not be updated").arg(objectId.constData()));

		return false;
	}

	CollectionItem collectionItem = m_files[fileIndex];

	locker.unlock();

	// Create meta-informations for the file contents:
	collectionItem.contentsMetaInfoPtr = CreateItemMetaInfo(localFilePath, collectionItem.typeId);
	if (!collectionItem.contentsMetaInfoPtr.IsValid()){
		SendErrorMessage(0, QObject::tr("Meta-informations could not be created for'%1'. File could not be updated").arg(localFilePath));

		return false;
	}

	QFileInfo inputFileInfo(localFilePath);

	QString targetFilePath = collectionItem.filePathInRepository;

	if (istd::CSystem::FileCopy(localFilePath, targetFilePath, true)){
		if (!QFile::setPermissions(targetFilePath, QFile::WriteGroup)){
			SendErrorMessage(0, QObject::tr("Permissions for the file '%1' could not be set").arg(targetFilePath));
		}

		locker.relock();

		static ChangeSet changes(CF_UPDATED);
		changes.SetChangeInfo(CN_ELEMENT_UPDATED, objectId);
		istd::CChangeNotifier changeNotifier(this, &changes);

		m_files[fileIndex].CopyFrom(collectionItem);

		SaveCollectionItem(collectionItem);

		QString metaInfoFilePath = GetMetaInfoFilePath(collectionItem);
		if (!SaveMetaInfo(*collectionItem.contentsMetaInfoPtr, metaInfoFilePath)){
			SendErrorMessage(0, QObject::tr("Meta information of the file '%1' could not be updated").arg(targetFilePath));
		}

		locker.unlock();

		return true;
	}

	SendErrorMessage(0, QObject::tr("File '%1' could not be copied to %2").arg(localFilePath).arg(targetFilePath));

	return false;
}


// reimplemented (ICollectionDataController)

const ifile::IFilePersistence* CFileCollectionCompBase::GetPersistenceForObjectType(const QByteArray& typeId) const
{
	int persistenceIndex = -1;

	if (m_resourceTypesCompPtr.IsValid()){
		for (int i = 0; i < m_resourceTypesCompPtr->GetOptionsCount(); ++i){
			if (typeId == m_resourceTypesCompPtr->GetOptionId(i)){
				persistenceIndex = i;
				break;
			}
		}
	}

	if ((persistenceIndex >= 0) && persistenceIndex < m_objectPersistenceListCompPtr.GetCount()){
		return m_objectPersistenceListCompPtr[persistenceIndex];
	}

	return nullptr;
}


bool CFileCollectionCompBase::ExportFile(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const QString& targetFilePath) const
{
	if (!targetFilePath.isEmpty()){
		if (!IsPathInsideRepository(targetFilePath)){
			QString resultPath = GetFile(objectId, targetFilePath);

			return resultPath == targetFilePath;
		}
	}

	return false;
}


QByteArray CFileCollectionCompBase::ImportFile(
			imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*typeId*/,
			const QString& /*sourceFilePath*/) const
{
	return QByteArray();
}


// reimplemented (IObjectCollection)

const imtbase::IRevisionController* CFileCollectionCompBase::GetRevisionController() const
{
	return nullptr;
}


const imtbase::ICollectionDataController* CFileCollectionCompBase::GetDataController() const
{
	return this;
}


int CFileCollectionCompBase::GetOperationFlags(const QByteArray& /*objectId*/) const
{
	return OF_SUPPORT_DELETE | OF_SUPPORT_EDIT | OF_SUPPORT_INSERT | OF_SUPPORT_RENAME;
}


QByteArray CFileCollectionCompBase::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			DataPtr defaultValuePtr,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* elementMetaInfoPtr,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	DataPtr newObjectPtr;

	if (defaultValuePtr.IsValid()){
		newObjectPtr = defaultValuePtr;
	}
	else{
		newObjectPtr = CreateDataObject(typeId);
	}

	if (newObjectPtr.IsValid()){
		const ifile::IFilePersistence* persistencePtr = GetPersistenceForObjectType(typeId);
		if (persistencePtr != nullptr){
			imtbase::CTempDir tempDir("ImtCore");
			if (tempDir.Path().isEmpty()){
				SendErrorMessage(0, tr("Temp folder could not be created"));

				return QByteArray();
			}

			QString tempFileBaseName = tempDir.Path() + "/" + QUuid::createUuid().toString();

			QString workingExt = GetWorkingExt(
						persistencePtr,
						newObjectPtr.GetPtr(),
						tempFileBaseName);

			QString tempFilePath = tempFileBaseName;
			tempFilePath += workingExt.isEmpty() ? "" : "." + workingExt;

			if (persistencePtr->SaveToFile(*newObjectPtr, tempFilePath) == ifile::IFilePersistence::OS_OK){

				QByteArray retval = InsertFile(tempFilePath, typeId, name, description, proposedObjectId, dataMetaInfoPtr, elementMetaInfoPtr);
				return retval;
			}
			else{
				SendErrorMessage(0, QObject::tr("File could not be saved into '%1'").arg(tempFilePath));
			}
		}
	}

	return QByteArray();
}


bool CFileCollectionCompBase::RemoveElement(const Id& /*elementId*/, const imtbase::IOperationContext* /*operationContextPtr*/)
{
	return false;
}


const istd::IChangeable* CFileCollectionCompBase::GetObjectPtr(const QByteArray& /*objectId*/) const
{
	return nullptr;
}


bool CFileCollectionCompBase::GetObjectData(const QByteArray& objectId, DataPtr& dataPtr) const
{
	QByteArray typeId = GetObjectTypeId(objectId);

	// Check the object cache first:
	{
		QReadLocker lockCache(&m_objectCacheLock);
		if (m_objectCache.contains(objectId)){
			DataPtr dataObjectPtr = m_objectCache[objectId];
			Q_ASSERT(dataObjectPtr.IsValid());

			if (!dataPtr.IsValid()){
				DataPtr newInstancePtr = CreateDataObject(typeId);
				if (newInstancePtr.IsValid()){
					if (newInstancePtr->CopyFrom(*dataObjectPtr)){
						dataPtr = newInstancePtr;

						return true;
					}
				}
			}
			else{
				return dataPtr->CopyFrom(*dataObjectPtr.GetPtr());
			}
		}
	}

	for (const CollectionItem& item : m_files){
		if (item.fileId == objectId){
			DataPtr dataObjectPtr = CreateObjectFromFile(item.filePathInRepository, typeId);
			if (!dataObjectPtr.IsValid()){
				return false;
			}

			
			if (!dataPtr.IsValid()){
				DataPtr newInstancePtr = CreateDataObject(typeId);
				if (newInstancePtr.IsValid()){
					if (newInstancePtr->CopyFrom(*dataObjectPtr)){
						dataPtr = newInstancePtr;

						QWriteLocker lockCache(&m_objectCacheLock);

						m_objectCache[objectId] = dataPtr;

						return true;
					}
				}
			}
			else{
				if (dataPtr->CopyFrom(*dataObjectPtr)){
					QWriteLocker lockCache(&m_objectCacheLock);

					m_objectCache[objectId] = dataPtr;

					return true;
				}
			}
		}
	}

	return false;
}


bool CFileCollectionCompBase::SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode /*mode*/, const imtbase::IOperationContext* /*operationContextPtr*/)
{
	const ifile::IFilePersistence* persistencePtr = GetPersistenceForObjectType(GetObjectTypeId(objectId));
	if (persistencePtr != nullptr){
		imtbase::CTempDir tempDir("ImtCore");
		if (tempDir.Path().isEmpty()){
			SendErrorMessage(0, tr("Temp folder could not be created"));

			return false;
		}

		QString tempFileBaseName = tempDir.Path() + "/" + QUuid::createUuid().toString();

		QString workingExt = GetWorkingExt(
					persistencePtr,
					&object,
					tempFileBaseName);

		QString tempFilePath = tempFileBaseName;
		tempFilePath += workingExt.isEmpty() ? "" : "." + workingExt;

		if (persistencePtr->SaveToFile(object, tempFilePath) == ifile::IFilePersistence::OS_OK){
			bool retVal = UpdateFile(tempFilePath, objectId);

			return retVal;
		}
		else{
			SendErrorMessage(0, QObject::tr("File could not be saved into '%1'").arg(tempFilePath));
		}
	}

	return false;
}


imtbase::IObjectCollection* CFileCollectionCompBase::CreateSubCollection(
			int offset,
			int count,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	imtbase::IObjectCollection* collectionPtr = new imtbase::CObjectCollection;
	m_filesLock.lockForRead();

	int objectsCount = count >= 0 ? qMin(count, m_files.count()) : m_files.count();

	for (int fileIndex = offset; fileIndex < objectsCount; ++fileIndex){
		collectionPtr->InsertNewObject(m_files[fileIndex].typeId, m_files[fileIndex].objectName, "", m_files[fileIndex].CloneMe());
	}

	m_filesLock.unlock();

	return collectionPtr;
}


imtbase::IObjectCollectionIterator* CFileCollectionCompBase::CreateObjectCollectionIterator(
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return nullptr;
}


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CFileCollectionCompBase::GetObjectTypesInfo() const
{
	return &m_resourceTypeConstraints;
}


imtbase::IObjectCollectionInfo::Id CFileCollectionCompBase::GetObjectTypeId(const QByteArray& objectId) const
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


idoc::MetaInfoPtr CFileCollectionCompBase::GetDataMetaInfo(const Id& objectId) const
{
	idoc::MetaInfoPtr metaInfoPtr;

	QReadLocker lockCollection(&m_filesLock);

	// Looking for file item:
	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex < 0){
		SendVerboseMessage(QObject::tr("Collection item doesn't exist for the given object-ID (%1). Meta-information could not be provided").arg(objectId.constData()), "File Collection");

		return metaInfoPtr;
	}

	// Get meta-information from cache:
	CollectionItem& item = m_files[fileIndex];

	if (item.contentsMetaInfoPtr.IsValid()){
		metaInfoPtr.SetCastedOrRemove(item.contentsMetaInfoPtr->CloneMe());
	}

	return metaInfoPtr;
}


// reimplemented (ICollectionInfo)

int CFileCollectionCompBase::GetElementsCount(const iprm::IParamsSet* /*selectionParamPtr*/) const
{
	m_filesLock.lockForRead();

	int retVal = m_files.count();

	m_filesLock.unlock();

	return retVal;
}


imtbase::ICollectionInfo::Ids CFileCollectionCompBase::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	Ids retVal;

	m_filesLock.lockForRead();

	int objectsCount = count >= 0 ? qMin(count, m_files.count()) : m_files.count();

	for (int fileIndex = offset; fileIndex < objectsCount; ++fileIndex){
		retVal.append(m_files[fileIndex].fileId);
	}

	m_filesLock.unlock();

	return retVal;
}


bool CFileCollectionCompBase::GetSubsetInfo(
			imtbase::ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return false;
}


QVariant CFileCollectionCompBase::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	QReadLocker locker(&m_filesLock);

	int fileIndex = GetFileIndexById(elementId);
	if (fileIndex >= 0){
		const CollectionItem& item = m_files[fileIndex];

		switch (infoType){
		case EIT_NAME:
			return item.objectName;

		case EIT_DESCRIPTION:
			return item.metaInfo.GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION).toString();
		}
	}

	return QString();
}


idoc::MetaInfoPtr CFileCollectionCompBase::GetElementMetaInfo(const Id& elementId) const
{
	idoc::MetaInfoPtr metaInfoPtr;

	QReadLocker lockCollection(&m_filesLock);

	// Looking for file item:
	int fileIndex = GetFileIndexById(elementId);
	if (fileIndex < 0){
		SendVerboseMessage(QObject::tr("Collection item doesn't exist for the given object-ID (%1). Meta-information could not be provided").arg(elementId.constData()), "File Collection");

		return metaInfoPtr;
	}

	// Get meta-information from cache:
	CollectionItem& item = m_files[fileIndex];

	metaInfoPtr.SetCastedOrRemove(item.metaInfo.CloneMe());

	return metaInfoPtr;
}


bool CFileCollectionCompBase::SetElementName(const Id& /*elementId*/, const QString& /*name*/)
{
	return false;
}


bool CFileCollectionCompBase::SetElementDescription(const Id& elementId, const QString& description)
{
	QWriteLocker locker(&m_filesLock);

	int fileIndex = GetFileIndexById(elementId);
	if (fileIndex >= 0){
		CollectionItem& item = m_files[fileIndex];

		QString oldDescription = item.metaInfo.GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION).toString();

		if (oldDescription != description){
			static ChangeSet changes(CF_UPDATED);
			changes.SetChangeInfo(CN_ELEMENT_UPDATED, elementId);
			istd::CChangeNotifier changeNotifier(this, &changes);

			item.metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION, description);

			SaveCollectionItem(item);

			locker.unlock();
		}

		return true;
	}

	return false;
}


bool CFileCollectionCompBase::SetElementEnabled(const Id& /*elementId*/, bool /*isEnabled*/)
{
	return false;
}


// reimplemented (IObjectCollection::IDataFactory)

CFileCollectionCompBase::DataPtr CFileCollectionCompBase::CreateInstance(const QByteArray& keyId) const
{
	return CreateDataObject(keyId);
}


// reimplemented (istd::IFactoryInfo)

istd::IFactoryInfo::KeyList CFileCollectionCompBase::GetFactoryKeys() const
{
	istd::IFactoryInfo::KeyList retVal;

	for (int i = 0; i < m_resourceTypeConstraints.GetOptionsCount(); ++i){
		retVal.insert(m_resourceTypeConstraints.GetOptionId(i));
	}

	return retVal;
}


// static protected methods of the class CFileCollectionCompBase

CFileCollectionCompBase::RepositoryInfo CFileCollectionCompBase::GetRepositoryInfo()
{
	RepositoryInfo retVal;

	retVal.metaInfoFileSuffix = "meta";
	retVal.dataFileSuffix = "item";

	return retVal;
}


// protected methods of the class CFileCollectionCompBase

void CFileCollectionCompBase::EnumerateRepositoryItems(QFileInfoList& fileList) const
{
	QString repositoryRootPath = GetCollectionRootFolder();
	QDir repositoryRootDir(repositoryRootPath);

	ifile::CFileListProviderComp::CreateFileList(repositoryRootDir, 0, 2, QStringList() << QString("*.%1").arg(GetRepositoryInfo().dataFileSuffix), QDir::Name, fileList);
}


QString CFileCollectionCompBase::CreateWorkingDir() const
{
	QString workingPath = QDir::tempPath() + "/ImtCore/" + QUuid::createUuid().toString();

	if (istd::CSystem::EnsurePathExists(workingPath)){
		return workingPath;
	}

	return QString();
}


QString CFileCollectionCompBase::GetWorkingExt(
			const ifile::IFilePersistence* persistencePtr,
			const istd::IChangeable* dataObjectPtr,
			const QString& fileName)
{
	int flags = ifile::IFilePersistence::QF_FILE | ifile::IFilePersistence::QF_SAVE;

	QStringList supportedExts;
	persistencePtr->GetFileExtensions(supportedExts, dataObjectPtr, flags);

	for (const QString ext : supportedExts){
		QString filePath = fileName + "." + ext;
		if (persistencePtr->IsOperationSupported(dataObjectPtr, &filePath, flags, false)){
			return ext;
		}
	}

	return QString();
}


CFileCollectionCompBase::DataPtr CFileCollectionCompBase::CreateDataObject(const QByteArray& typeId) const
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
		icomp::IComponent* compPtr = m_objectFactoryListCompPtr.CreateComponent(factoryIndex);
		return CFileCollectionCompBase::DataPtr(
			DataPtr::RootObjectPtr(compPtr),
			[this, compPtr]() {
				return m_objectFactoryListCompPtr.ExtractInterface(compPtr);
			});
	}

	return CFileCollectionCompBase::DataPtr();
}


CFileCollectionCompBase::DataPtr CFileCollectionCompBase::CreateObjectFromFile(const QString& filePath, const QByteArray& typeId) const
{
	CFileCollectionCompBase::DataPtr retVal = CreateDataObject(typeId);
	if (retVal.IsValid()){
		const ifile::IFilePersistence* filePersistenceCompPtr = GetPersistenceForObjectType(typeId);
		if (filePersistenceCompPtr != nullptr){
			int loadState = filePersistenceCompPtr->LoadFromFile(*retVal, filePath);
			if (loadState == ifile::IFilePersistence::OS_OK){
				return retVal;
			}
		}
	}

	return CFileCollectionCompBase::DataPtr();
}


QString CFileCollectionCompBase::SaveCollectionItem(const CollectionItem& collectionItem, const QString& dataFilePath) const
{
	QString itemFilePath = dataFilePath.isEmpty() ? GetDataItemFilePath(collectionItem) : dataFilePath;

	if (m_helperFilesPersistence.IsValid()){
		if (m_helperFilesPersistence->SaveToFile(collectionItem, itemFilePath) == ifile::IFilePersistence::OS_OK)
			return itemFilePath;
	}
	else{
		ifile::CCompactXmlFileWriteArchive archive(itemFilePath, m_versionInfoCompPtr.GetPtr());
		if (const_cast<CollectionItem&>(collectionItem).Serialize(archive))
			return itemFilePath;
	}


	SendErrorMessage(0, QObject::tr("Collection item could not be saved into '%1'").arg(itemFilePath));
	return QString();
}


QString CFileCollectionCompBase::GetDataItemFilePath(const CollectionItem& repositoryFile) const
{
	QString retVal;

	QFileInfo fileInfo(repositoryFile.filePathInRepository);
	QString baseName = fileInfo.fileName();
	if (!baseName.isEmpty()){
		retVal = fileInfo.absolutePath() + "/" + baseName + "." + GetRepositoryInfo().dataFileSuffix;
	}

	return retVal;
}


QString CFileCollectionCompBase::GetMetaInfoFilePath(const CollectionItem& repositoryFile) const
{
	QString retVal;

	QFileInfo fileInfo(repositoryFile.filePathInRepository);
	QString baseName = fileInfo.fileName();
	if (!baseName.isEmpty()){
		retVal = fileInfo.absolutePath() + "/" + baseName + "." + GetRepositoryInfo().metaInfoFileSuffix;
	}

	return retVal;
}


QString CFileCollectionCompBase::CalculateShortFileName(const QString& fileName, const QFileInfo& fileInfo, const QString& prefix) const
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


bool CFileCollectionCompBase::SaveMetaInfo(const idoc::IDocumentMetaInfo& metaInfo, const QString& metaInfoFilePath) const
{
	iser::ISerializable* serializablePtr = const_cast<iser::ISerializable*>(dynamic_cast<const iser::ISerializable*>(&metaInfo));
	if (serializablePtr == nullptr){
		Q_ASSERT_X(false, __FILE__, "Serializable interface doesn't support by the meta-info object");

		return false;
	}

	if (m_helperFilesPersistence.IsValid()){
		return m_helperFilesPersistence->SaveToFile(*serializablePtr, metaInfoFilePath) == ifile::IFilePersistence::OS_OK;
	}

	// Use default serializer:
	ifile::CCompactXmlFileWriteArchive archive(metaInfoFilePath, m_versionInfoCompPtr.GetPtr());

	return serializablePtr->Serialize(archive);
}


bool CFileCollectionCompBase::LoadMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QString& metaInfoFilePath) const
{
	if (!QFile::exists(metaInfoFilePath))
		return false;

	iser::ISerializable* serializablePtr = dynamic_cast<iser::ISerializable*>(&metaInfo);
	if (!serializablePtr)
		return false;

	if (m_helperFilesPersistence.IsValid())
		return m_helperFilesPersistence->LoadFromFile(*serializablePtr, metaInfoFilePath) == ifile::IFilePersistence::OS_OK;

	// using default serializer
	ifile::CCompactXmlFileReadArchive archive(metaInfoFilePath, m_versionInfoCompPtr.GetPtr());
	return serializablePtr->Serialize(archive);
}


bool CFileCollectionCompBase::IsPathInsideRepository(const QString& filePath) const
{
	QString cleanFileDir = QDir::cleanPath(QFileInfo(filePath).absolutePath());
	QString cleanRepositoryDir = QDir::cleanPath(GetCollectionRootFolder());

	return cleanFileDir.contains(cleanRepositoryDir);
}


int CFileCollectionCompBase::GetFileIndexById(const QByteArray& fileId) const
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


int CFileCollectionCompBase::GetFileIndexByName(const QString& fileName) const
{
	for (int i = 0; i < m_files.count(); ++i){
		if (m_files[i].objectName == fileName){
			return i;
		}
	}

	return -1;
}


idoc::MetaInfoPtr CFileCollectionCompBase::CreateItemMetaInfo(const QString& dataObjectFilePath, const QByteArray& typeId) const
{
	idoc::MetaInfoPtr retVal;

	QFileInfo fileInfo(dataObjectFilePath);
	if (fileInfo.exists()){
		if (m_metaInfoCreatorMap.contains(typeId)){
			DataPtr dataObjectPtr = CreateObjectFromFile(dataObjectFilePath, typeId);
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


bool CFileCollectionCompBase::CreateItemMetaInfoFile(const QString& dataObjectFilePath, const QByteArray& typeId, const QString& metaInfoFilePath) const
{
	idoc::MetaInfoPtr metaInfoPtr = CreateItemMetaInfo(dataObjectFilePath, typeId);
	if (metaInfoPtr.IsValid()){
		if (SaveMetaInfo(*metaInfoPtr, metaInfoFilePath)){
			return true;
		}
	}

	return false;
}


void CFileCollectionCompBase::UpdateItemMetaInfo(CollectionItem& item) const
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
			SendErrorMessage(0, QObject::tr("Meta-information for the file '%1' could not be created. Meta-information could not be provided").arg(metaInfoFilePath), "File Collection");
		}
	}
}


void CFileCollectionCompBase::ReadRepositoryItems()
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


void CFileCollectionCompBase::ReadItem(Files& filesPtr, const QString& itemFilePath) const
{
	CollectionItem fileItem(GetCollectionRootFolder(), *m_revisionAttrPtr);
	if (!ReadItemFile(fileItem, itemFilePath)){
		SendErrorMessage(0, QObject::tr("Collection item could not be loaded from '%1'").arg(itemFilePath));
		return;
	}

	QFileInfo repositoryFileInfo(fileItem.filePathInRepository);
	if (repositoryFileInfo.exists()){
		if (!fileItem.contentsMetaInfoPtr.IsValid()){
			idoc::MetaInfoPtr retVal;

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
		SendErrorMessage(0, QObject::tr("File '%1' doesn't exist. Collection item was automatically removed").arg(fileItem.filePathInRepository));
	}
}


bool CFileCollectionCompBase::ReadItemFile(CollectionItem& collectionItem, const QString& itemFilePath) const
{
	if (itemFilePath.isEmpty())
		return false;

	if (m_helperFilesPersistence.IsValid())
		return m_helperFilesPersistence->LoadFromFile(collectionItem, itemFilePath) == ifile::IFilePersistence::OS_OK;

	ifile::CCompactXmlFileReadArchive archive(itemFilePath, m_versionInfoCompPtr.GetPtr());
	return collectionItem.Serialize(archive);
}


void CFileCollectionCompBase::StartRepositoryLoader()
{
	if (m_progressManagerListCompPtr.IsValid()){
		for (int i = 0; i < m_progressManagerListCompPtr.GetCount(); i++){
			m_progressManagerListCompPtr[i]->BeginProgressSession("FileCollection", "Start reading file collection");
		}
	}

	m_readerThread.start();
}


void CFileCollectionCompBase::OnReaderProgress(int progress)
{
	if (m_progressManagerListCompPtr.IsValid()){
		for (int i = 0; i < m_progressManagerListCompPtr.GetCount(); i++){
			m_progressManagerListCompPtr[i]->OnProgress(0, progress);
		}
	}
}


void CFileCollectionCompBase::OnReaderFinished()
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


void CFileCollectionCompBase::OnReaderInterrupted()
{
}


bool CFileCollectionCompBase::IsObjectIdLocked(const QByteArray& resourceId)
{
	QMutexLocker locker(&m_lockedObjectInfoMutex);

	return m_lockedObjectIds.contains(resourceId);
}


bool CFileCollectionCompBase::IsObjectNameLocked(const QString& resourceName)
{
	QMutexLocker locker(&m_lockedObjectInfoMutex);

	return m_lockedObjectNames.contains(resourceName);
}


bool CFileCollectionCompBase::IsObjectIdUsed(const QByteArray& objectId)
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


bool CFileCollectionCompBase::FinishInsertFileTransaction(
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
				ElementInsertInfo info;
				changes.SetChangeInfo(CN_ELEMENT_INSERTED, fileId);
				istd::CChangeNotifier changeNotifier(this, &changes);

				QWriteLocker locker(&m_filesLock);
				m_files.push_back(collectionItem);
				locker.unlock();
			}
		}
		else{
			QDir(repositoryPath).removeRecursively();
		}
	}

	return result;
}


// reimplemented (icomp::CComponentBase)

void CFileCollectionCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_itemInfoProvider.UpdateItems();

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
			SendCriticalMessage(0, QObject::tr("Root folder for the file collection could not be created in '%1'").arg(path));
		}
	}
}


void CFileCollectionCompBase::OnComponentDestroyed()
{
	m_readerThread.requestInterruption();
	m_readerThread.wait();

	m_filesLock.lockForWrite();
	m_files.clear();
	m_filesLock.unlock();

	BaseClass::OnComponentDestroyed();
}


// public methods of the embedded class RepositoryItemInfo

void CFileCollectionCompBase::RepositoryItemInfo::SetRepositoryItemFilePath(RepositoryFileType fileId, const QString& filePath)
{
	m_files[int(fileId)] = filePath;
}


// reimplemented (IRepositoryItemInfo)

IRepositoryItemInfo::RepositoryFileTypes CFileCollectionCompBase::RepositoryItemInfo::GetRepositoryItemFileTypes() const
{
	IRepositoryItemInfo::RepositoryFileTypes types;

	for (int type : m_files.keys()){
		types.insert((RepositoryFileType)type);
	}

	return types;
}


QString CFileCollectionCompBase::RepositoryItemInfo::GetRepositoryItemFilePath(RepositoryFileType fileId) const
{
	if (m_files.contains(fileId)){
		return m_files[fileId];
	}

	return QString();
}


// public methods of the embedded class RepositoryItemInfoProvider

CFileCollectionCompBase::RepositoryItemInfoProvider::RepositoryItemInfoProvider(CFileCollectionCompBase& parent)
	:m_parent(parent)
{
}


bool CFileCollectionCompBase::RepositoryItemInfoProvider::UpdateItems()
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

const imtbase::ICollectionInfo& CFileCollectionCompBase::RepositoryItemInfoProvider::GetRepositoryItems()
{
	return *this;
}


const IRepositoryItemInfo* CFileCollectionCompBase::RepositoryItemInfoProvider::GetRepositoryItemInfo(const QByteArray& itemId) const
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

CFileCollectionCompBase::ResourceTypeConstraints::ResourceTypeConstraints()
	:m_parentPtr(nullptr)
{
}


void CFileCollectionCompBase::ResourceTypeConstraints::SetParent(CFileCollectionCompBase* parentPtr)
{
	m_parentPtr = parentPtr;
}


// reimplemented (IFileResourceTypeConstraints)

const ifile::IFileTypeInfo* CFileCollectionCompBase::ResourceTypeConstraints::GetFileTypeInfo(int typeIndex) const
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

int CFileCollectionCompBase::ResourceTypeConstraints::GetOptionsFlags() const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionsFlags();
}


int CFileCollectionCompBase::ResourceTypeConstraints::GetOptionsCount() const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionsCount();
}


QString CFileCollectionCompBase::ResourceTypeConstraints::GetOptionName(int index) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionName(index);
}


QString CFileCollectionCompBase::ResourceTypeConstraints::GetOptionDescription(int index) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionDescription(index);
}


QByteArray CFileCollectionCompBase::ResourceTypeConstraints::GetOptionId(int index) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionId(index);
}


bool CFileCollectionCompBase::ResourceTypeConstraints::IsOptionEnabled(int index) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->IsOptionEnabled(index);
}


// public methods of embedded class QResourceLocker

CFileCollectionCompBase::ResourceLocker::ResourceLocker(
			CFileCollectionCompBase& collection,
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


CFileCollectionCompBase::ResourceLocker::~ResourceLocker()
{
	QMutexLocker locker(&m_collection.m_lockedObjectInfoMutex);

	if (!m_resourceId.isEmpty()){
		m_collection.m_lockedObjectIds.removeOne(m_resourceId);
	}

	if (!m_resourceName.isEmpty()){
		m_collection.m_lockedObjectNames.removeOne(m_resourceName);
	}
}


// public methods of embedded class ReaderThread

CFileCollectionCompBase::ReaderThread::ReaderThread(CFileCollectionCompBase* parentPtr)
	:m_parentPtr(parentPtr)
{
}


// private methods of embedded class ReaderThread

// reimplemented (QThread)

void CFileCollectionCompBase::ReaderThread::run()
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
