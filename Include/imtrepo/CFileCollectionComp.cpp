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
#include <imtcore/Version.h>
#include <imtbase/CObjectCollectionInsertEvent.h>
#include <imtbase/CObjectCollectionUpdateEvent.h>
#include <imtbase/CObjectCollectionRemoveEvent.h>


namespace imtrepo
{


// public methods

CFileCollectionComp::CFileCollectionComp()
	:m_itemInfoProvider(*this)
{
}


// reimplemented (IRepositoryItemInfoProvider)

const imtbase::ICollectionInfo& CFileCollectionComp::GetRepositoryItems()
{
	return m_itemInfoProvider;
}


const IRepositoryItemInfo* CFileCollectionComp::GetRepositoryItemInfo(const QByteArray& itemId) const
{
	return m_itemInfoProvider.GetRepositoryItemInfo(itemId);
}


// reimplemented (imtbase::IRevisionController)

imtbase::IRevisionController::RevisionInfoList CFileCollectionComp::GetRevisionInfoList(const imtbase::IObjectCollection& collection, const QByteArray& objectId) const
{
	imtbase::IRevisionController::RevisionInfoList revisionInfoList;
	
	const IFileObjectCollection* collectionPtr = dynamic_cast<const IFileObjectCollection*>(&collection);
	if (collectionPtr == nullptr){
		return revisionInfoList;
	}

	RevisionsContents revisionsContents;
	if (!LoadRevisionsContents(*collectionPtr, objectId, revisionsContents)){
		return revisionInfoList;
	}

	QString revisionsPath = QFileInfo(collectionPtr->GetFileInfo(objectId).filePath).path() + "/Revisions";

	for (int key : revisionsContents.keys()){
		RevisionInfo revisionInfoListItem;
		RevisionsContentsItem revisionsContentsItem = revisionsContents.value(key);
		revisionInfoListItem = revisionsContentsItem;

		revisionInfoListItem.isRevisionAvailable = false;
		if (QFile::exists(revisionsPath + "/" + revisionsContents[key].path)){
			revisionInfoListItem.isRevisionAvailable = true;
		}

		revisionInfoList.append(revisionInfoListItem);
	}

	return revisionInfoList;
}


int CFileCollectionComp::BackupObject(imtbase::IObjectCollection& collection, const QByteArray& objectId, const QString& userComment) const
{
	IFileObjectCollection* collectionPtr = dynamic_cast<IFileObjectCollection*>(&collection);
	if (collectionPtr == nullptr || !m_compressorCompPtr.IsValid()){
		return -1;
	}

	QString revisionsPath = QFileInfo(collectionPtr->GetFileInfo(objectId).filePath).path() + "/Revisions";

	if (istd::CSystem::EnsurePathExists(revisionsPath)){
		int newRevision = 0;

		RevisionsContents revisionsContents;
		LoadRevisionsContents(*collectionPtr, objectId, revisionsContents);
		for (int revision : revisionsContents.keys()){
			if (newRevision < revision){
				newRevision = revision;
			}
		}

		idoc::CStandardDocumentMetaInfo metaInfo;
		collection.GetCollectionItemMetaInfo(objectId, metaInfo);
		QVariant currentRevision = metaInfo.GetMetaInfo(imtrepo::IFileObjectCollection::MIT_REVISION);
		if (currentRevision.isValid()){
			int revision = currentRevision.toInt();
			if (revision >= newRevision){
				newRevision = revision;
			}
		}

		newRevision++;

		QString tempPath = CreateWorkingDir();
		QDir tempDir(tempPath);

		if (!tempPath.isEmpty()){
			QString sourceFilePath = collectionPtr->GetFile(objectId, tempPath + "/object.data");
			QFile sourceFile(sourceFilePath);

			QString fileName = QString("revision.%1.zip").arg(newRevision);
			QString targetFilePath = revisionsPath + "/" + fileName;
			QFile targetFile(targetFilePath);

			if (sourceFile.exists()){
				if (m_compressorCompPtr->CompressFile(sourceFilePath, targetFilePath)){
					RevisionsContentsItem revisionItem;

					revisionItem.revision = newRevision;
					revisionItem.timestamp = QDateTime::currentDateTime();
					revisionItem.comment = userComment;
					revisionItem.repositoryRevision = collectionPtr->GetRepositoryRevision();
					revisionItem.path = fileName;

					if (m_loginProviderCompPtr.IsValid()){
						iauth::CUser* userPtr = m_loginProviderCompPtr->GetLoggedUser();
						if (userPtr != nullptr){
							revisionItem.user = userPtr->GetUserName();
						}
					}

					revisionsContents[newRevision] = revisionItem;

					if (SaveRevisionsContents(*collectionPtr, objectId, revisionsContents)){
						// TODO: remove dirty code

						QReadLocker readLocker(&m_filesLock);

						int fileIndex = GetFileIndexById(objectId);
						if (fileIndex < 0){
							return -1;
						}

						CollectionItem& collectionItem = m_files[fileIndex];

						readLocker.unlock();

						collectionItem.metaInfo.SetMetaInfo(imtrepo::IFileObjectCollection::MIT_REVISION, newRevision);
						collectionItem.metaInfo.SetMetaInfo(MIT_LAST_OPERATION_TIME, QDateTime::currentDateTime());

						QWriteLocker writeLocker(&m_filesLock);

						SaveCollectionItem(collectionItem);

						writeLocker.unlock();

						imtbase::CObjectCollectionUpdateEvent event(objectId, imtbase::CObjectCollectionUpdateEvent::UT_DATA);
						for (imtbase::IObjectCollectionEventHandler* eventHandlerPtr : m_eventHandlerList){
							eventHandlerPtr->OnObjectCollectionEvent(this, &event);
						}

						tempDir.removeRecursively();
						return newRevision;
					}

					QFile(targetFilePath).remove();
				}
			}
			else{
				SendErrorMessage(0, QString(QObject::tr("Target file '%1' could not be created")).arg(targetFilePath));
			}

			tempDir.removeRecursively();
		}
		else{
			SendErrorMessage(0, QString(QObject::tr("Target folder '%1' could not be created")).arg(tempPath));
		}
	}
	else{
		SendErrorMessage(0, QString(QObject::tr("Target folder '%1' could not be created")).arg(revisionsPath));
	}

	return -1;
}


bool CFileCollectionComp::RestoreObject(imtbase::IObjectCollection& collection, const QByteArray& objectId, int revision) const
{
	IFileObjectCollection* collectionPtr = dynamic_cast<IFileObjectCollection*>(&collection);
	if (collectionPtr == nullptr || !m_compressorCompPtr.IsValid()){
		return false;
	}

	RevisionsContents revisionsContents;
	if (LoadRevisionsContents(*collectionPtr, objectId, revisionsContents)){
		if (revisionsContents.contains(revision)){
			QString revisionsPath = QFileInfo(collectionPtr->GetFileInfo(objectId).filePath).path() + "/Revisions";
			QString revisionFilePath = revisionsPath + "/" + revisionsContents[revision].path;

			if (QFile(revisionFilePath).exists()){
				QString tempPath = CreateWorkingDir();
				QDir tempDir(tempPath);

				if (!tempPath.isEmpty()){
					if (m_compressorCompPtr->DecompressFolder(revisionFilePath, tempPath)){
						QString objectDataFilePath = tempPath + "/object.data";
						QFile objectDataFile(objectDataFilePath);
						if (objectDataFile.exists()){
							if (revisionsContents[revision].repositoryRevision != *m_revisionAttrPtr){
								RepositoryItemInfo repositoryItemInfo;
								repositoryItemInfo.SetRepositoryItemFilePath(IRepositoryItemInfo::RFT_DATA, objectDataFilePath);

								if (!TransformRepositoryItem(repositoryItemInfo, revisionsContents[revision].repositoryRevision, *m_revisionAttrPtr)){
									tempDir.removeRecursively();
									return false;
								}
							}

							QString fileFromCollectionPath = collectionPtr->GetFile(objectId);
							QFile fileFromCollection(fileFromCollectionPath);
							if (fileFromCollection.exists()){
								QString newObjectDataFilePath = objectDataFilePath + "." + QFileInfo(fileFromCollection).suffix();
								fileFromCollection.remove();

								if (objectDataFile.rename(newObjectDataFilePath)){
									if (collectionPtr->UpdateFile(newObjectDataFilePath, objectId)){
										// TODO: remove dirty code

										QReadLocker readLocker(&m_filesLock);

										int fileIndex = GetFileIndexById(objectId);
										if (fileIndex < 0){
											tempDir.removeRecursively();
											return false;
										}

										CollectionItem& collectionItem = m_files[fileIndex];

										readLocker.unlock();

										collectionItem.metaInfo.SetMetaInfo(imtrepo::IFileObjectCollection::MIT_REVISION, revision);
										collectionItem.metaInfo.SetMetaInfo(MIT_LAST_OPERATION_TIME, QDateTime::currentDateTime());

										QWriteLocker writeLocker(&m_filesLock);

										SaveCollectionItem(collectionItem);

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
					else{
						SendErrorMessage(0, QString(QObject::tr("Revision '%1' could not be decompressed")).arg(revisionFilePath));
					}

					tempDir.removeRecursively();
				}
				else{
					SendErrorMessage(0, QString(QObject::tr("Target folder '%1' could not be created")).arg(tempPath));
				}
			}
		}
	}

	return false;
}


bool CFileCollectionComp::ExportObject(const imtbase::IObjectCollection& collection, const QByteArray& objectId, int revision, const QString& filePath) const
{
	if (!m_compressorCompPtr.IsValid()){
		return false;
	}

	const IFileObjectCollection* collectionPtr = dynamic_cast<const IFileObjectCollection*>(&collection);
	if (collectionPtr == nullptr){
		return false;
	}

	RevisionsContents revisionsContents;
	if (LoadRevisionsContents(*collectionPtr, objectId, revisionsContents)){
		if (revisionsContents.contains(revision)){
			QString revisionsPath = QFileInfo(collectionPtr->GetFileInfo(objectId).filePath).path() + "/Revisions";
			QString revisionFilePath = revisionsPath + "/" + revisionsContents[revision].path;

			if (QFile(revisionFilePath).exists()){
				QString tempPath = CreateWorkingDir();
				QDir tempDir(tempPath);

				if (!tempPath.isEmpty()){
					if (m_compressorCompPtr->DecompressFolder(revisionFilePath, tempPath)){
						QString objectDataFilePath = tempPath + "/object.data";

						if (QFile::exists(objectDataFilePath)){
							if (revisionsContents[revision].repositoryRevision != *m_revisionAttrPtr){
								RepositoryItemInfo repositoryItemInfo;
								repositoryItemInfo.SetRepositoryItemFilePath(IRepositoryItemInfo::RFT_DATA, objectDataFilePath);

								if (!TransformRepositoryItem(repositoryItemInfo, revisionsContents[revision].repositoryRevision, *m_revisionAttrPtr)){
									tempDir.removeRecursively();
									return false;
								}
							}

							if (istd::CSystem::FileCopy(objectDataFilePath, filePath)){
								tempDir.removeRecursively();
								return true;
							}
						}
					}
					else{
						SendErrorMessage(0, QString(QObject::tr("Revision '%1' could not be decompressed")).arg(revisionFilePath));
					}

					tempDir.removeRecursively();
				}
				else{
					SendErrorMessage(0, QString(QObject::tr("Target folder '%1' could not be created")).arg(tempPath));
				}
			}
		}
	}

	return false;
}


// reimplemented (IFileObjectCollection)

QByteArray CFileCollectionComp::ImportFile(const QByteArray& typeId, const QString& sourceFilePath)
{
	int repositoryRevision = *m_revisionAttrPtr;

	if (!sourceFilePath.isEmpty()){
		if (!IsPathInsideRepository(sourceFilePath)){
			QFileInfo fileInfo(sourceFilePath);
			if (fileInfo.suffix() == "zip"){
				QString workingPath = CreateWorkingDir();
				QDir workingDir(workingPath);

				if (istd::CSystem::EnsurePathExists(workingPath)){
					if (m_compressorCompPtr->DecompressFolder(sourceFilePath, workingPath)){
						QString itemSuffix = GetRepositoryInfo().dataFileSuffix;
						workingDir.setFilter(QDir::Files);
						workingDir.setNameFilters({"*."+ GetRepositoryInfo().dataFileSuffix});
						QStringList list = workingDir.entryList();
						if (!list.isEmpty()){
							QString itemFile = workingDir.filePath(list[0]);
							CollectionItem collectionItem(GetCollectionRootFolder(), repositoryRevision);
							if (ReadItemFile(collectionItem, itemFile)){
								QString dataFile = workingDir.filePath(QFileInfo(collectionItem.filePathInRepository).fileName());
								if (QFile(dataFile).exists()){
									if (collectionItem.repositoryRevision != repositoryRevision){
										if (m_transformationStepsProviderCompPtr.IsValid()){
											RepositoryItemInfo itemInfo;
											itemInfo.SetRepositoryItemFilePath(IRepositoryItemInfo::RFT_INFO, itemFile);
											itemInfo.SetRepositoryItemFilePath(IRepositoryItemInfo::RFT_DATA, dataFile);

											if (!TransformRepositoryItem(itemInfo, collectionItem.repositoryRevision, repositoryRevision)){
												workingDir.removeRecursively();
												return QByteArray();
											}

											if (!ReadItemFile(collectionItem, itemFile)){
												workingDir.removeRecursively();
												return QByteArray();
											}
										}
									}

									QFileInfo dataFileInfo(dataFile);
									QFileInfo itemFileInfo(itemFile);

									QFile(dataFile + "." + GetRepositoryInfo().metaInfoFileSuffix).remove();

									QWriteLocker locker(&m_filesLock);

									QString targetDirPath = CalculateFolderPathInRepository(dataFile, dataFileInfo.completeBaseName(), typeId, nullptr);
									QString objectName = QDir(targetDirPath).dirName();
									ResourceLocker(*this, QByteArray(), objectName);

									locker.unlock();

									QString newDataFile = dataFileInfo.dir().path() + "/" + objectName + "." + dataFileInfo.suffix();
									QString newItemFile = newDataFile + "." + GetRepositoryInfo().dataFileSuffix;
									QString newMetaInfoFile = newDataFile + "." + GetRepositoryInfo().metaInfoFileSuffix;

									QFile(dataFile).rename(newDataFile);
									QFile(itemFile).rename(newItemFile);

									MetaInfoPtr metaInfoPtr = CreateItemMetaInfo(newDataFile, typeId);
									if (!metaInfoPtr.IsValid()){
										workingDir.removeRecursively();
										return QByteArray();
									}

									collectionItem.contentsMetaInfoPtr = metaInfoPtr;
									SaveMetaInfo(*metaInfoPtr, newMetaInfoFile);

									collectionItem.objectName = objectName;
									collectionItem.fileId = QUuid::createUuid().toByteArray();
									collectionItem.filePathInRepository = GetCollectionRootFolder() + "/" + objectName + "/" + QFileInfo(newDataFile).fileName();
									collectionItem.repositoryRevision = repositoryRevision;

									collectionItem.metaInfo.SetMetaInfo(MIT_LAST_OPERATION_TIME, QDateTime::currentDateTime());
									collectionItem.metaInfo.SetMetaInfo(MIT_INSERTION_TIME, QDateTime::currentDateTime());

									QString savedPath = SaveCollectionItem(collectionItem, newItemFile);
									if (!savedPath.isEmpty()){
										if (FinishInsertFileTransaction(workingPath, targetDirPath, collectionItem.fileId, collectionItem)){
											QDir(workingPath).removeRecursively();

											return collectionItem.fileId;
										}
										else{
											SendErrorMessage(0, tr("File could not be inserted into the repository"));
										}
									}
									else{
										SendErrorMessage(0, tr("Item file '%1' could not be saved").arg(newItemFile));
									}
								}
								else{
									SendErrorMessage(0, tr("Data file '%1' not fount").arg(dataFile));
								}
							}
							else{
								SendErrorMessage(0, tr("Unable read item file '%1'").arg(itemFile));
							}
						}
						else{
							SendErrorMessage(0, tr("Item file not found in '%1'").arg(workingPath));
						}
					}
					else{
						SendErrorMessage(0, tr("Unable to uncompress '%1'").arg(sourceFilePath));
					}

					workingDir.removeRecursively();
				}
				else{
					SendErrorMessage(0, tr("Target folder '%1' could not be created").arg(workingPath));
				}
			}
			else{
				istd::TDelPtr<istd::IChangeable> dataObjectPtr(CreateObjectFromFile(sourceFilePath, typeId));
				if (dataObjectPtr.IsValid()){
					QByteArray objectId = InsertFile(sourceFilePath, typeId, QFileInfo(sourceFilePath).completeBaseName());
					return objectId;
				}
			}
		}
	}

	return QByteArray();
}


// reimplemented (IFileCollectionInfo)

IFileCollectionInfo::FileCollectionLayout CFileCollectionComp::GetCollectionFileLayout() const
{
	return FCL_FOLDER;
}


// reimplemented (IObjectCollection)

const imtbase::IRevisionController* CFileCollectionComp::GetRevisionController() const
{
	return this;
}


bool CFileCollectionComp::RemoveObject(const QByteArray& objectId)
{
	if (objectId.isEmpty()){
		SendErrorMessage(0, "object-ID is empty. Unknown resource could not be removed");

		return false;
	}

	if (m_rightsProviderCompPtr.IsValid() && !m_rightsProviderCompPtr->HasRight(*m_restoreRevisionRightIdAttrPtr)){
		return false;
	}

	QWriteLocker repositoryDataLocker(&m_filesLock);

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

			if (!istd::CSystem::RemoveDirectory(parentDirectory)){
				SendErrorMessage(0, QString("Folder containing file '%1'could not be removed").arg(itemToRemove.filePathInRepository));

				return false;
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


void CFileCollectionComp::SetObjectName(const QByteArray& objectId, const QString& objectName)
{
	QWriteLocker locker(&m_filesLock);

	int fileIndex = GetFileIndexById(objectId);
	if (fileIndex >= 0){
		CollectionItem& item = m_files[fileIndex];

		if (item.objectName != objectName){
			int foundIndex = GetFileIndexByName(objectName);
			if (foundIndex >= 0){
				bool isSameResourceType = (m_files[fileIndex].typeId == m_files[foundIndex].typeId);

				if (isSameResourceType){
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
			QWriteLocker locker(&m_filesLock);

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


// protected methods

QString CFileCollectionComp::CalculateFolderPathInRepository(
			const QString& localFilePath,
			const QString& objectName,
			const QByteArray& typeId,
			ilog::IMessageConsumer* messageConsumerPtr) const
{
	QFileInfo inputFileInfo(localFilePath);
	if (!inputFileInfo.isFile()){
		SendErrorMessage(0 , QString("Path '%1' is not a valid path to an existing file").arg(localFilePath));

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


QString CFileCollectionComp::CalculateTargetFilePath(
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

#ifdef Q_OS_WIN
	targetFileName = CalculateShortFileName(targetFileName, inputFileInfo, "");
#endif
	targetFilePath = targetFolderPath + QString("/") + targetFileName;

	return targetFilePath;
}


bool CFileCollectionComp::LoadRevisionsContents(const IFileObjectCollection& collection, const QByteArray& objectId, RevisionsContents& revisionsContents) const
{
	QString objectName = collection.GetElementInfo(objectId, EIT_NAME).toString();
	QString revisionsContentsPath = QFileInfo(collection.GetFileInfo(objectId).filePath).path() + "/Revisions/contents.xml";

	ifile::CCompactXmlFileReadArchive archive(revisionsContentsPath, m_versionInfoCompPtr.GetPtr());
	return revisionsContents.Serialize(archive);
}


bool CFileCollectionComp::SaveRevisionsContents(const IFileObjectCollection& collection, const QByteArray& objectId, RevisionsContents& revisionsContents) const
{
	QString objectName = collection.GetElementInfo(objectId, EIT_NAME).toString();
	QString revisionsContentsPath = QFileInfo(collection.GetFileInfo(objectId).filePath).path() + "/Revisions/contents.xml";

	ifile::CCompactXmlFileWriteArchive archive(revisionsContentsPath, m_versionInfoCompPtr.GetPtr());
	return revisionsContents.Serialize(archive);
}


bool CFileCollectionComp::UpdateRepositoryFormat()
{
	int currentRevision = 0;
	int targetRevision = *m_revisionAttrPtr;
	QString revisionFilePath;

	QString path = GetCollectionRootFolder();
	if (!path.isEmpty()){
		if (!istd::CSystem::EnsurePathExists(path)){
			SendCriticalMessage(0, QString("Root folder for the file collection could not be created in '%1'").arg(path));
		}

		revisionFilePath = path + "/Revision";

		if (!QFileInfo(revisionFilePath).exists()){
			QFile revisionFile(revisionFilePath);
			QTextStream textStream(&revisionFile);

			if (revisionFile.open(QIODevice::Text | QIODevice::ReadWrite)){
				textStream << currentRevision;

				revisionFile.close();
			}
		}
		else{
			QFile revisionFile(revisionFilePath);
			QTextStream textStream(&revisionFile);

			if (revisionFile.open(QIODevice::Text | QIODevice::ReadWrite)){
				textStream >> currentRevision;

				revisionFile.close();
			}
		}
	}

	if (targetRevision != currentRevision){
		if (m_transformationControllerCompPtr.IsValid()){
			bool retVal = m_transformationControllerCompPtr->TransformRepository(*this, currentRevision, targetRevision);
			if (retVal){
				QFile revisionFile(revisionFilePath);
				QTextStream textStream(&revisionFile);

				if (revisionFile.open(QIODevice::Text | QIODevice::ReadWrite)){
					textStream << targetRevision;

					revisionFile.close();

					return true;
				}
			}
		}
	}

	return false;
}


bool CFileCollectionComp::TransformRepositoryItem(const RepositoryItemInfo& repositoryItemInfo, int fromRepositoryRevision, int toRepositoryRevision) const
{
	if (m_transformationStepsProviderCompPtr.IsValid()){
		IRepositoryFileTransformationStepsProvider::TransformationSteps steps =
					m_transformationStepsProviderCompPtr->GetTransformationSteps(fromRepositoryRevision, toRepositoryRevision);

		if (steps.isEmpty()){
			return false;
		}

		for (const IRepositoryFileTransformationStepsProvider::TransformationStep& step : steps){
			if (!step.transformationPtr->TransformFile(repositoryItemInfo, step.from, step.to)){
				return false;
			}
		}
	}

	return true;
}


// reimplemented (icomp::CComponentBase)

void CFileCollectionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_itemInfoProvider.UpdateItems();

	if (UpdateRepositoryFormat()){
		ReadRepositoryItems();

		const imtbase::IRevisionController* revisionControllerPtr = GetRevisionController();
		if (revisionControllerPtr != nullptr){
			imtbase::ICollectionInfo::Ids ids = GetElementIds();
			for (const QByteArray& id : ids){
				revisionControllerPtr->BackupObject(*this, id, tr("Revision number updated to %1").arg(*m_revisionAttrPtr));
			}
		}
	}
	else{
		if (*m_asynchronousReadingAttrPtr){
			StartRepositoryLoader();
		}
		else{
			ReadRepositoryItems();
		}
	}
 }


void CFileCollectionComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


QString CFileCollectionComp::CalculateTargetFilePath(
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
			SendErrorMessage(0, QString("Collection folder for the input file '%1' could not be calculated").arg(filePath));
		}
	}
	else{
		SendErrorMessage(0, QString("Input file '%1' doesn't exist").arg(filePath));
	}

	return QString();
}


// reimplemented (imtbase::ICollectionInfo)

imtbase::ICollectionInfo::Ids CFileCollectionComp::RepositoryItemInfoProvider::GetElementIds() const
{
	QReadLocker locker(&m_lock);

	imtbase::ICollectionInfo::Ids ids;
	for (const Item& item : m_repositoryItems){
		ids.append(item.id);
	}

	return ids;
}


QVariant CFileCollectionComp::RepositoryItemInfoProvider::GetElementInfo(const QByteArray& /*elementId*/, int /*infoType*/) const
{
	return QVariant();
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

	quint32 imtCoreVersion;
	bool imtCoreVersionExists = archive.GetVersionInfo().GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion);
	if (imtCoreVersionExists && imtCoreVersion >= 3068){
		static iser::CArchiveTag repositoryRevisionTag("RepositoryRevision", "Repository revision");
		retVal = retVal && archive.BeginTag(repositoryRevisionTag);
		retVal = retVal && archive.Process(repositoryRevision);
		retVal = retVal && archive.EndTag(repositoryRevisionTag);
	}
	else{
		if (!archive.IsStoring()){
			repositoryRevision = 0;
		}
	}

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

	QList<int> keysList(keys());

	for (int i = 0; i < revisionCount; i++){
		RevisionsContentsItem revisionsContentsItem;

		if (archive.IsStoring()){
			revisionsContentsItem = value(keysList[i]);
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

		quint32 imtCoreVersion = 0xffffffff;
		bool imtCoreVersionExists = archive.GetVersionInfo().GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion);
		if ((archive.IsStoring() || imtCoreVersionExists) && (imtCoreVersion >= 2776)){
			static iser::CArchiveTag repositoryRevisionTag("RepositoryRevision", "RepositoryRevision");
			retVal = retVal && archive.BeginTag(repositoryRevisionTag);
			retVal = retVal && archive.Process(revisionsContentsItem.repositoryRevision);
			retVal = retVal && archive.EndTag(repositoryRevisionTag);
		}

		static iser::CArchiveTag repositoryFileNameTag("Path", "Path");
		retVal = retVal && archive.BeginTag(repositoryFileNameTag);
		retVal = retVal && archive.Process(revisionsContentsItem.path);
		retVal = retVal && archive.EndTag(repositoryFileNameTag);

		if (retVal && !archive.IsStoring()){
			insert(revisionsContentsItem.revision, revisionsContentsItem);
		}

		retVal = retVal && archive.EndTag(revisionItemTag);
	}

	retVal = retVal && archive.EndTag(revisionListTag);

	return retVal;
}


} // namespace imtrepo


