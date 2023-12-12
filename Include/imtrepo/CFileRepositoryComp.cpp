#include <imtrepo/CFileRepositoryComp.h>


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

// ImtCore includes
#include <imtcore/Version.h>
#include <imtbase/CTempDir.h>


namespace imtrepo
{


// public methods of the class CFileRepositoryComp

CFileRepositoryComp::CFileRepositoryComp()
{
	m_resourceTypeConstraints.SetParent(this);
}


// reimplemented (IFileCollectionInfo)

const ifile::IFileResourceTypeConstraints* CFileRepositoryComp::GetFileTypeConstraints() const
{
	if (!m_resourceFileTypesCompPtr.IsValid() || !m_resourceTypesCompPtr.IsValid()){
		return nullptr;
	}

	return &m_resourceTypeConstraints;
}


bool CFileRepositoryComp::GetFileInfo(const QByteArray& objectId, IFileCollectionItem& item) const
{
	if (!m_documentInfoCollectionCompPtr.IsValid()){
		return false;
	}

	DataPtr fileInfoDataPtr;
	if (!m_documentInfoCollectionCompPtr->GetObjectData(objectId, fileInfoDataPtr)){
		return false;
	}

	const IFileCollectionItem* fileInfoItemPtr = dynamic_cast<const IFileCollectionItem*>(fileInfoDataPtr.GetPtr());
	if (fileInfoItemPtr != nullptr){
		return item.CopyFrom(*fileInfoItemPtr);
	}

	return false;
}


QString CFileRepositoryComp::GetCollectionRootFolder() const
{
	QString retVal;

	if (m_repositoryPathCompPtr.IsValid()){
		retVal = m_repositoryPathCompPtr->GetPath();
	}

	return retVal;
}


int CFileRepositoryComp::GetRepositoryRevision() const
{
	return *m_revisionAttrPtr;
}


IFileCollectionInfo::FileCollectionLayout CFileRepositoryComp::GetCollectionFileLayout() const
{
	return FCL_FLAT;
}


// reimplemented (IFileObjectCollection)

QString CFileRepositoryComp::GetFile(
			const QByteArray& objectId,
			const QString& targetFilePath) const
{
	CFileCollectionItem fileItemInfo;
	if (!GetFileInfo(objectId, fileItemInfo)){
		return QString();
	}

	QString outputFilePath = targetFilePath;

	QFileInfo fileInfo(fileItemInfo.GetFilePath());
	if (fileInfo.exists()){
		// If the output file path is empty, then create the file path automatically:
		if (outputFilePath.isEmpty()){
			QString fileExtension = fileInfo.suffix();

			outputFilePath = QDir::tempPath() + "/ImtCore/" + fileInfo.completeBaseName() + "_" + QUuid::createUuid().toString() + "." + fileExtension;
		}

		if (fileItemInfo.GetFilePath() == outputFilePath){
			return outputFilePath;
		}

		if (istd::CSystem::FileCopy(fileItemInfo.GetFilePath(), outputFilePath, true)){
			return outputFilePath;
		}
		else{
			SendErrorMessage(0, QObject::tr("File could not be copied from '%1' to '%2'").arg(fileItemInfo.GetFilePath()).arg(targetFilePath));
		}
	}
	else{
		SendErrorMessage(0, QObject::tr("File doesn't exist '%1'").arg(fileItemInfo.GetFilePath()));
	}

	return QString();
}


QByteArray CFileRepositoryComp::InsertFile(
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

	// Generate unique file-ID accroding to \c proposedObjectId variable provided by caller:
	QByteArray fileId = proposedObjectId.isEmpty() ? QUuid::createUuid().toByteArray() : proposedObjectId;

	// Generate teraget absolute file path for the data file:
	QString targetFilePath = CalculateTargetFilePath(localFilePath, objectName, typeId);
	if (targetFilePath.isEmpty()){
		return emptyId;
	}

	QFileInfo targetFileInfo(targetFilePath);
	QString targetName = targetFileInfo.completeBaseName();

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

			CFileCollectionItem collectionItem(GetCollectionRootFolder(), *m_revisionAttrPtr, fileId, typeId, targetFilePath, targetName);
			if (collectionItemMetaInfoPtr != nullptr){
				idoc::CStandardDocumentMetaInfo::MetaInfoTypes metaInfoTypes = collectionItemMetaInfoPtr->GetMetaInfoTypes();
#if QT_VERSION < 0x051500
				QList<int> metaInfoTypesList = metaInfoTypes.toList();
#else
				QList<int> metaInfoTypesList(metaInfoTypes.begin(), metaInfoTypes.end());
#endif
				for (int i = 0; i < metaInfoTypesList.size(); i++){
					QVariant valueMeta = collectionItemMetaInfoPtr->GetMetaInfo(metaInfoTypesList[i]);
					collectionItem.SetCollectionMetaInfo(metaInfoTypesList[i], valueMeta);
				}
			}
			else{
				collectionItem.SetCollectionMetaInfo(MIT_LAST_OPERATION_TIME, QDateTime::currentDateTime());
				collectionItem.SetCollectionMetaInfo(MIT_INSERTION_TIME, QDateTime::currentDateTime());
				collectionItem.SetCollectionMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION, objectDescription);
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

			bool metaInfoCreated = WriteFileMetaInfo(*metaInfoPtr);
			if (metaInfoCreated){
				collectionItem.SetContentsMetaInfo(metaInfoPtr);

				// Inserting new item into collection if the structure was created successfully:
				bool success = WriteItemInfo(collectionItem);
				if (success){
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
				SendErrorMessage(0, tr("Meta-information for the file '%1' could not be created").arg(workingFilePath));
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


bool CFileRepositoryComp::UpdateFile(
			const QString& localFilePath,
			const QByteArray& objectId)
{
	{
		QWriteLocker cacheLocker(&m_objectCacheLock);

		if (m_objectCache.contains(objectId)){
			m_objectCache.remove(objectId);
		}
	}

	CFileCollectionItem fileItemInfo;
	if (!GetFileInfo(objectId, fileItemInfo)){
		return false;
	}

	// Create meta-informations for the file contents:
	fileItemInfo.SetContentsMetaInfo(CreateItemMetaInfo(localFilePath, fileItemInfo.GetTypeId()));
	if (!fileItemInfo.GetContentsMetaInfo().IsValid()){
		SendErrorMessage(0, QObject::tr("Meta-informations could not be created for'%1'. File could not be updated").arg(localFilePath));

		return false;
	}

	QFileInfo inputFileInfo(localFilePath);

	QString targetFilePath = fileItemInfo.GetFilePath();

	if (istd::CSystem::FileCopy(localFilePath, targetFilePath, true)){
		if (!QFile::setPermissions(targetFilePath, QFile::WriteGroup)){
			SendErrorMessage(0, QObject::tr("Permissions for the file '%1' could not be set").arg(targetFilePath));
		}

		static ChangeSet changes(CF_UPDATED);
		changes.SetChangeInfo(CN_ELEMENT_UPDATED, objectId);
		istd::CChangeNotifier changeNotifier(this, &changes);

		if (!WriteItemInfo(fileItemInfo)){
			SendErrorMessage(0, QObject::tr("Meta information of the file '%1' could not be updated").arg(targetFilePath));

			return false;
		}

		if (!WriteFileMetaInfo(*fileItemInfo.GetContentsMetaInfo())){
			SendErrorMessage(0, QObject::tr("Meta information of the file '%1' could not be updated").arg(targetFilePath));

			return false;
		}

		return true;
	}

	SendErrorMessage(0, QObject::tr("File '%1' could not be copied to %2").arg(localFilePath).arg(targetFilePath));

	return false;
}


// reimplemented (IObjectCollection)

const imtbase::IRevisionController* CFileRepositoryComp::GetRevisionController() const
{
	return nullptr;
}


const imtbase::ICollectionDataController* CFileRepositoryComp::GetDataController() const
{
	return nullptr;
}


int CFileRepositoryComp::GetOperationFlags(const QByteArray& /*objectId*/) const
{
	return OF_SUPPORT_DELETE | OF_SUPPORT_EDIT | OF_SUPPORT_INSERT | OF_SUPPORT_RENAME;
}


QByteArray CFileRepositoryComp::InsertNewObject(
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
		const ifile::IFilePersistence* persistencePtr = GetObjectPersistence(typeId);
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


bool CFileRepositoryComp::RemoveElement(const Id& /*elementId*/, const imtbase::IOperationContext* /*operationContextPtr*/)
{
	return false;
}


const istd::IChangeable* CFileRepositoryComp::GetObjectPtr(const QByteArray& /*objectId*/) const
{
	return nullptr;
}


bool CFileRepositoryComp::GetObjectData(const QByteArray& objectId, DataPtr& dataPtr) const
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

	CFileCollectionItem fileItemInfo;
	if (!GetFileInfo(objectId, fileItemInfo)){
		return false;
	}


	if (!fileItemInfo.GetFilePath().isEmpty()){
		DataPtr dataObjectPtr = CreateObjectFromFile(fileItemInfo.GetFilePath(), typeId);
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

	return false;
}


bool CFileRepositoryComp::SetObjectData(
			const QByteArray& objectId,
			const istd::IChangeable& object,
			CompatibilityMode /*mode*/,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	const ifile::IFilePersistence* persistencePtr = GetObjectPersistence(GetObjectTypeId(objectId));
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


imtbase::IObjectCollection* CFileRepositoryComp::CreateSubCollection(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (m_documentInfoCollectionCompPtr.IsValid()){
		return m_documentInfoCollectionCompPtr->CreateSubCollection(offset, count, selectionParamsPtr);
	}

	return nullptr;
}


imtbase::IObjectCollectionIterator* CFileRepositoryComp::CreateObjectCollectionIterator(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	if (m_documentInfoCollectionCompPtr.IsValid()){
		return m_documentInfoCollectionCompPtr->CreateObjectCollectionIterator(offset, count, selectionParamsPtr);
	}

	return nullptr;
}


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CFileRepositoryComp::GetObjectTypesInfo() const
{
	return &m_resourceTypeConstraints;
}


imtbase::IObjectCollectionInfo::Id CFileRepositoryComp::GetObjectTypeId(const QByteArray& objectId) const
{
	CFileCollectionItem fileItemInfo;
	if (!GetFileInfo(objectId, fileItemInfo)){
		return QByteArray();
	}

	return fileItemInfo.GetTypeId();
}


idoc::MetaInfoPtr CFileRepositoryComp::GetDataMetaInfo(const Id& objectId) const
{
	CFileCollectionItem fileItemInfo;
	if (!GetFileInfo(objectId, fileItemInfo)){
		return idoc::MetaInfoPtr();
	}

	return fileItemInfo.GetContentsMetaInfo();
}


// reimplemented (ICollectionInfo)

int CFileRepositoryComp::GetElementsCount(const iprm::IParamsSet* selectionParamPtr, ilog::IMessageConsumer* logPtr) const
{
	if (m_documentInfoCollectionCompPtr.IsValid()){
		return m_documentInfoCollectionCompPtr->GetElementsCount(selectionParamPtr, logPtr);
	}

	return 0;
}


imtbase::ICollectionInfo::Ids CFileRepositoryComp::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* logPtr) const
{
	if (m_documentInfoCollectionCompPtr.IsValid()){
		return m_documentInfoCollectionCompPtr->GetElementIds(offset, count, selectionParamsPtr, logPtr);
	}

	return Ids();
}


bool CFileRepositoryComp::GetSubsetInfo(
			imtbase::ICollectionInfo& subsetInfo,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* logPtr) const
{
	if (m_documentInfoCollectionCompPtr.IsValid()){
		return m_documentInfoCollectionCompPtr->GetSubsetInfo(subsetInfo, offset, count, selectionParamsPtr, logPtr);
	}

	return false;
}


QVariant CFileRepositoryComp::GetElementInfo(const QByteArray& elementId, int infoType, ilog::IMessageConsumer* /*logPtr*/) const
{
	CFileCollectionItem fileItemInfo;
	if (!GetFileInfo(elementId, fileItemInfo)){
		return QVariant();
	}

	switch (infoType){
	case EIT_NAME:
		return fileItemInfo.GetName();

	case EIT_DESCRIPTION:
		return fileItemInfo.GetCollectionMetaInfo().GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION).toString();
	}

	return QVariant();
}


idoc::MetaInfoPtr CFileRepositoryComp::GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* /*logPtr*/) const
{
	idoc::MetaInfoPtr metaInfoPtr;

	CFileCollectionItem fileItemInfo;
	if (!GetFileInfo(elementId, fileItemInfo)){
		return idoc::MetaInfoPtr();
	}

	metaInfoPtr.SetCastedOrRemove(fileItemInfo.GetCollectionMetaInfo().CloneMe());

	return metaInfoPtr;
}


bool CFileRepositoryComp::SetElementName(const Id& /*elementId*/, const QString& /*name*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


bool CFileRepositoryComp::SetElementDescription(const Id& /*elementId*/, const QString& /*description*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


bool CFileRepositoryComp::SetElementEnabled(const Id& /*elementId*/, bool /*isEnabled*/, ilog::IMessageConsumer* /*logPtr*/)
{
	return false;
}


// reimplemented (IObjectCollection::IDataFactory)

CFileRepositoryComp::DataPtr CFileRepositoryComp::CreateInstance(const QByteArray& keyId) const
{
	return CreateDataObject(keyId);
}


// reimplemented (istd::IFactoryInfo)

istd::IFactoryInfo::KeyList CFileRepositoryComp::GetFactoryKeys() const
{
	istd::IFactoryInfo::KeyList retVal;

	for (int i = 0; i < m_resourceTypeConstraints.GetOptionsCount(); ++i){
		retVal.insert(m_resourceTypeConstraints.GetOptionId(i));
	}

	return retVal;
}


// protected methods

const ifile::IFilePersistence* CFileRepositoryComp::GetObjectPersistence(const QByteArray& typeId) const
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


QString CFileRepositoryComp::CreateWorkingDir() const
{
	QString workingPath = QDir::tempPath() + "/ImtCore/" + QUuid::createUuid().toString();

	if (istd::CSystem::EnsurePathExists(workingPath)){
		return workingPath;
	}

	return QString();
}


QString CFileRepositoryComp::GetWorkingExt(
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


CFileRepositoryComp::DataPtr CFileRepositoryComp::CreateDataObject(const QByteArray& typeId) const
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
		return DataPtr(
			DataPtr::RootObjectPtr(compPtr),
			[this, compPtr]() {
				return m_objectFactoryListCompPtr.ExtractInterface(compPtr);
			});
	}

	return DataPtr();
}


CFileRepositoryComp::DataPtr CFileRepositoryComp::CreateObjectFromFile(const QString& filePath, const QByteArray& typeId) const
{
	CFileRepositoryComp::DataPtr retVal = CreateDataObject(typeId);
	if (retVal.IsValid()){
		const ifile::IFilePersistence* filePersistenceCompPtr = GetObjectPersistence(typeId);
		if (filePersistenceCompPtr != nullptr){
			int loadState = filePersistenceCompPtr->LoadFromFile(*retVal, filePath);
			if (loadState == ifile::IFilePersistence::OS_OK){
				return retVal;
			}
		}
	}

	return CFileRepositoryComp::DataPtr();
}


bool CFileRepositoryComp::WriteItemInfo(const CFileCollectionItem& repositoryItem) const
{
	return false;
}


bool CFileRepositoryComp::WriteFileMetaInfo(const idoc::IDocumentMetaInfo& metaInfo) const
{
	return false;
}


bool CFileRepositoryComp::IsPathInsideRepository(const QString& filePath) const
{
	QString cleanFileDir = QDir::cleanPath(QFileInfo(filePath).absolutePath());
	QString cleanRepositoryDir = QDir::cleanPath(GetCollectionRootFolder());

	return cleanFileDir.contains(cleanRepositoryDir);
}


idoc::MetaInfoPtr CFileRepositoryComp::CreateItemMetaInfo(
			const QString& dataObjectFilePath,
			const QByteArray& typeId) const
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


QString CFileRepositoryComp::CalculateFolderPathInRepository(
			const QString& localFilePath,
			const QString& objectName,
			const QByteArray& typeId,
			ilog::IMessageConsumer* messageConsumerPtr) const
{
	QFileInfo inputFileInfo(localFilePath);
	if (!inputFileInfo.isFile()){
		SendErrorMessage(0 , QObject::tr("Path '%1' is not a valid path to an existing file").arg(localFilePath));

		return QString();
	}

	QString targetDirPath = inputFileInfo.absoluteDir().absolutePath();

	QString repositoryDirPath = GetCollectionRootFolder();
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

	QString subfolderName = objectName;
#ifdef Q_OS_WIN
	if (inputFileInfo.fileName() == objectName){
		subfolderName = inputFileInfo.completeBaseName();
	}
#endif
	targetDirPath += QString("/") + subfolderName;

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

	return targetDirPath;
}


QString CFileRepositoryComp::CalculateTargetFilePath(
			const QString& filePath,
			const QString& objectName,
			const QByteArray& typeId) const
{
	ilog::IMessageConsumer* logPtr = GetLogPtr();

	if (QFile(filePath).exists()){
		QString targetFolderPath = CalculateFolderPathInRepository(filePath, objectName, typeId, logPtr);
		if (!targetFolderPath.isEmpty()){
			return CalculateTargetFilePath(targetFolderPath, filePath);
		}
		else{
			SendErrorMessage(0, QObject::tr("Collection folder for the input file '%1' could not be calculated").arg(filePath));
		}
	}
	else{
		SendErrorMessage(0, QObject::tr("Input file '%1' doesn't exist").arg(filePath));
	}

	return QString();
}


QString CFileRepositoryComp::CalculateTargetFilePath(
			const QString& targetFolderPath,
			const QString& localFilePath) const
{
	QFileInfo inputFileInfo(localFilePath);

	QString targetFilePath;

	Q_ASSERT(inputFileInfo.fileName().size() <= 255); // would be an invalid filename!

	QString targetFileName = inputFileInfo.fileName();
	QString dirName = QDir(targetFolderPath).dirName();
	if (!dirName.isEmpty() && (dirName != ".")){
		targetFileName = dirName + "." + inputFileInfo.suffix();
	}

	targetFilePath = targetFolderPath + QString("/") + targetFileName;

	return targetFilePath;
}


bool CFileRepositoryComp::FinishInsertFileTransaction(
			const QString& workingPath,
			const QString& repositoryPath,
			const QByteArray& fileId,
			const CFileCollectionItem& collectionItem)
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

				QByteArray documentId = m_documentInfoCollectionCompPtr->InsertNewObject(
							"FileItemInfo",
							collectionItem.GetName(),
							"",
							DataPtr(&collectionItem),
							fileId);

				Q_ASSERT(documentId == fileId);

				result = !documentId.isEmpty() && (documentId == fileId);
			}
		}
		else{
			QDir(repositoryPath).removeRecursively();
		}
	}

	return result;
}


// reimplemented (icomp::CComponentBase)

void CFileRepositoryComp::OnComponentCreated()
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
			SendCriticalMessage(0, QObject::tr("Root folder for the file collection could not be created in '%1'").arg(path));
		}
	}
}


void CFileRepositoryComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


// public methods of the embedded class ResourceTypeConstraints

CFileRepositoryComp::ResourceTypeConstraints::ResourceTypeConstraints()
	:m_parentPtr(nullptr)
{
}


void CFileRepositoryComp::ResourceTypeConstraints::SetParent(CFileRepositoryComp* parentPtr)
{
	m_parentPtr = parentPtr;
}


// reimplemented (IFileResourceTypeConstraints)

const ifile::IFileTypeInfo* CFileRepositoryComp::ResourceTypeConstraints::GetFileTypeInfo(int typeIndex) const
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

int CFileRepositoryComp::ResourceTypeConstraints::GetOptionsFlags() const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionsFlags();
}


int CFileRepositoryComp::ResourceTypeConstraints::GetOptionsCount() const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionsCount();
}


QString CFileRepositoryComp::ResourceTypeConstraints::GetOptionName(int index) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionName(index);
}


QString CFileRepositoryComp::ResourceTypeConstraints::GetOptionDescription(int index) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionDescription(index);
}


QByteArray CFileRepositoryComp::ResourceTypeConstraints::GetOptionId(int index) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->GetOptionId(index);
}


bool CFileRepositoryComp::ResourceTypeConstraints::IsOptionEnabled(int index) const
{
	Q_ASSERT(m_parentPtr != nullptr);
	Q_ASSERT(m_parentPtr->m_resourceFileTypesCompPtr.IsValid());

	return m_parentPtr->m_resourceTypesCompPtr->IsOptionEnabled(index);
}


} // namespace imtrepo


