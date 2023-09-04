#include <imtrepo/CFileCollectionItem.h>


// Qt includes
#include <QtCore/QDir>

// ACF includes
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>

// ImtCore includes
#include <imtcore/Version.h>


namespace imtrepo
{


// public methods
CFileCollectionItem::CFileCollectionItem()
	:m_repositoryRevision(0)
{
}


CFileCollectionItem::CFileCollectionItem(
			const QString& repositoryFolderPath,
			int repositoryRevision,
			const QByteArray& fileId,
			const QByteArray& typeId,
			const QString& filePath,
			const QString& name,
			idoc::MetaInfoPtr contentsMetaInfoPtr)
	:m_repositoryFolderPath(repositoryFolderPath),
	m_repositoryRevision(repositoryRevision),
	m_fileId(fileId),
	m_typeId(typeId),
	m_filePathInRepository(filePath),
	m_objectName(name),
	m_contentsMetaInfoPtr(contentsMetaInfoPtr)
{
}


bool CFileCollectionItem::SetCollectionMetaInfo(int metaInfoType, const QVariant& value)
{
	return m_metaInfo.SetMetaInfo(metaInfoType, value);
}

bool CFileCollectionItem::SetContentsMetaInfo(idoc::MetaInfoPtr metaInfoPtr)
{
	m_contentsMetaInfoPtr = metaInfoPtr;

	return true;
}


void CFileCollectionItem::SetObjectName(const QString& name)
{
	m_objectName = name;
}


void CFileCollectionItem::SetPathInRepository(const QString& path)
{
	m_filePathInRepository = path;
}


void CFileCollectionItem::SetFileId(const QByteArray& fileId)
{
	m_fileId = fileId;
}


void CFileCollectionItem::SetRepositoryRevision(int revision)
{
	m_repositoryRevision = revision;
}


// reimplemented (IFileCollectionItem)

QByteArray CFileCollectionItem::GetFileId() const
{
	return m_fileId;
}


QString CFileCollectionItem::GetFilePath() const
{
	return m_filePathInRepository;
}


QString CFileCollectionItem::GetName() const
{
	return m_objectName;
}


QByteArray CFileCollectionItem::GetTypeId() const
{
	return m_typeId;
}


const idoc::IDocumentMetaInfo& CFileCollectionItem::GetCollectionMetaInfo() const
{
	return m_metaInfo;
}


idoc::MetaInfoPtr CFileCollectionItem::GetContentsMetaInfo() const
{
	return m_contentsMetaInfoPtr;
}

int CFileCollectionItem::GetRepositoryRevision() const
{
	return m_repositoryRevision;
}


// reimplement (iser::ISerializable)

bool CFileCollectionItem::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	static iser::CArchiveTag fileIdTag("FileID", "ID of the file in the repository");
	retVal = retVal && archive.BeginTag(fileIdTag);
	retVal = retVal && archive.Process(m_fileId);
	retVal = retVal && archive.EndTag(fileIdTag);

	// Change file path in the repository to a relative path (relvative to repository's folder):
	QDir repositoryDir(m_repositoryFolderPath);
	QString relativeFilePathInRepository = repositoryDir.relativeFilePath(m_filePathInRepository);

	static iser::CArchiveTag filePathInrepositoryItemsTag("RepositoryFilePath", "File path in the repository");
	retVal = retVal && archive.BeginTag(filePathInrepositoryItemsTag);
	retVal = retVal && archive.Process(relativeFilePathInRepository);
	retVal = retVal && archive.EndTag(filePathInrepositoryItemsTag);

	if (retVal && !archive.IsStoring()){
		m_filePathInRepository = QDir::cleanPath(repositoryDir.absoluteFilePath(relativeFilePathInRepository));
	}

	static iser::CArchiveTag resourceNameTag("Name", "Name of the file resource");
	retVal = retVal && archive.BeginTag(resourceNameTag);
	retVal = retVal && archive.Process(m_objectName);
	retVal = retVal && archive.EndTag(resourceNameTag);

	static iser::CArchiveTag resourceTypeIdTag("TypeID", "Type ID of the file resource");
	retVal = retVal && archive.BeginTag(resourceTypeIdTag);
	retVal = retVal && archive.Process(m_typeId);
	retVal = retVal && archive.EndTag(resourceTypeIdTag);

	quint32 imtCoreVersion;
	bool imtCoreVersionExists = archive.GetVersionInfo().GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion);
	if (imtCoreVersionExists && imtCoreVersion >= 3068){
		static iser::CArchiveTag repositoryRevisionTag("RepositoryRevision", "Repository revision");
		retVal = retVal && archive.BeginTag(repositoryRevisionTag);
		retVal = retVal && archive.Process(m_repositoryRevision);
		retVal = retVal && archive.EndTag(repositoryRevisionTag);
	}
	else{
		if (!archive.IsStoring()){
			m_repositoryRevision = 0;
		}
	}

	static iser::CArchiveTag metaInfoTag("MetaInfo", "Collection related meta-informations", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(metaInfoTag);
	retVal = retVal && m_metaInfo.Serialize(archive);
	retVal = retVal && archive.EndTag(metaInfoTag);

	return retVal;
}


// reimplement (istd::IChangeable)

bool CFileCollectionItem::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CFileCollectionItem* sourceItemPtr = dynamic_cast<const CFileCollectionItem*>(&object);
	if (sourceItemPtr != nullptr){
		m_fileId = sourceItemPtr->m_fileId;
		m_filePathInRepository = sourceItemPtr->m_filePathInRepository;
		m_objectName = sourceItemPtr->m_objectName;
		m_typeId = sourceItemPtr->m_typeId;

		if (sourceItemPtr->m_contentsMetaInfoPtr.IsValid()){
			m_contentsMetaInfoPtr.SetCastedOrRemove(sourceItemPtr->m_contentsMetaInfoPtr->CloneMe());
			if (!m_contentsMetaInfoPtr.IsValid()){
				return false;
			}
		}

		return m_metaInfo.CopyFrom(sourceItemPtr->m_metaInfo);
	}

	return false;
}


} // namespace imtrepo


