#pragma once


// ACF includes
#include <istd/TSmartPtr.h>
#include <iser/ISerializable.h>
#include <idoc/IDocumentMetaInfo.h>


namespace imtrepo
{


/**
	Internal structure representing the file item in the collection.
*/
class CFileCollectionItem: virtual public iser::ISerializable
{
public:
	CFileCollectionItem(const QString& repositoryFolderPath, int repositoryRev);

	// reimplement (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplement (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;

public:
	/**
		ID of the file in the file collection.
	*/
	QByteArray fileId;

	/**
		File path in the file collection.
	*/
	QString filePathInRepository;

	/**
		Location of the source file.
	*/
	QString sourceFilePath;

	/**
		Name of the file object.
	*/
	QString objectName;

	/**
		Type ID of the file object.
	*/
	QByteArray typeId;

	/**
		Repository revision.
	*/
	int repositoryRevision;

	/**
		Meta-informations for the file item in the collection.
	*/
	imtbase::CObjectCollectionMetaInfo metaInfo;

	/**
		Meta-informations for the file contents.
	*/
	idoc::MetaInfoPtr contentsMetaInfoPtr;

private:
	/**
		Path to the root folder of the repository.
	*/
	QString m_repositoryFolderPath;
};


} // namespace imtrepo


