#pragma once


// ImtCore includes
#include <imtbase/CObjectCollectionMetaInfo.h>
#include <imtrepo/IFileCollectionItem.h>


namespace imtrepo
{


/**
	Internal structure representing the file item in the collection.
*/
class CFileCollectionItem: virtual public IFileCollectionItem
{
public:
	CFileCollectionItem();
	CFileCollectionItem(
				const QString& repositoryFolderPath,
				int repositoryRevision,
				const QByteArray& fileId = QByteArray(),
				const QByteArray& typeId = QByteArray(),
				const QString& filePath = QString(),
				const QString& name = QString(),
				idoc::MetaInfoPtr contentsMetaInfoPtr = idoc::MetaInfoPtr());

	bool SetCollectionMetaInfo(int metaInfoType, const QVariant& value);
	bool SetContentsMetaInfo(idoc::MetaInfoPtr metaInfoPtr);
	void SetObjectName(const QString& name);
	void SetPathInRepository(const QString& path);
	void SetFileId(const QByteArray& fileId);
	void SetRepositoryRevision(int revision);

	// reimplemented (IFileCollectionItem)
	virtual QByteArray GetFileId() const override;
	virtual QString GetFilePath() const override;
	virtual QString GetName() const override;
	virtual QByteArray GetTypeId() const override;
	virtual const idoc::IDocumentMetaInfo& GetCollectionMetaInfo() const override;
	virtual idoc::MetaInfoPtr GetContentsMetaInfo() const override;
	virtual int GetRepositoryRevision() const override;

	// reimplement (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplement (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	/**
		ID of the file in the file collection.
	*/
	QByteArray m_fileId;

	/**
		File path in the file collection.
	*/
	QString m_filePathInRepository;

	/**
		Name of the file object.
	*/
	QString m_objectName;

	/**
		Type ID of the file object.
	*/
	QByteArray m_typeId;

	/**
		Repository revision.
	*/
	int m_repositoryRevision;

	/**
		Meta-informations for the file item in the collection.
	*/
	imtbase::CObjectCollectionMetaInfo m_metaInfo;

	/**
		Meta-informations for the file contents.
	*/
	idoc::MetaInfoPtr m_contentsMetaInfoPtr;

	/**
		Path to the root folder of the repository.
	*/
	QString m_repositoryFolderPath;
};


} // namespace imtrepo


