#include <imtrepo/CFileCollectionItem.h>


// public methods

CFileCollectionItem::CFileCollectionItem(const QString& repositoryFolderPath, int repositoryRev)
	:m_repositoryFolderPath(repositoryFolderPath),
	repositoryRevision(repositoryRev)
{
}


// reimplement (iser::ISerializable)

bool CFileCollectionItem::Serialize(iser::IArchive& archive)
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

bool CFileCollectionItem::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
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


} // namespace imtrepo


