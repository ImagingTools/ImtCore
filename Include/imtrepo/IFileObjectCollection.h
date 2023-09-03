#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <ifile/IFileMetaInfoProvider.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtrepo/IFileCollectionInfo.h>


namespace imtrepo
{


/**
	Common interface for an object collection, where the objects are stored in the file system.
	\ingroup Collection
	\ingroup FileRepository
*/
class IFileObjectCollection: virtual public imtbase::IObjectCollection, virtual public IFileCollectionInfo
{
public:
	/**
		Get file from the collection.
		\param objectId				ID of the file object.
		\param targetFilePath		Path to the location where the file should be stored. If the path is empty, the implementation should define the location of the file automatically.
		\return Path to the file on the local system or an empty string if the operation has been failed.
	*/
	virtual QString GetFile(const QByteArray& objectId, const QString& targetFilePath = QString()) const = 0;

	/**
		Add a file to the collection 
		\param filePath				Path to the file.
		\param objectTypeId			Type-ID of the file object. \sa GetFileTypeConstraints
		\param objectName			Name of the file object used for human readable representation. If no name was set, the file name will be used.
		\param objectDescription	Description of the file object.
		\param proposedObjectId		User-defined object-ID. If this parameter is not set, the implementation should generate a ID automatically.
		\return ID of the file object. If the operation was not successful, the method returns an empty collection ID.
	*/
	virtual QByteArray InsertFile(
				const QString& filePath,
				const QByteArray& objectTypeId = QByteArray(),
				const QString& objectName = QString(),
				const QString& objectDescription = QString(),
				const QByteArray& proposedObjectId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr
			) = 0;

	/**
		Update file in the collection.
		\param filePath		Path to the file containing the new data content.
		\param objectId		ID of the object to be updated in the collection.
		\return \c true if the file object was successfully updated or \c false otherwise.
	*/
	virtual bool UpdateFile(const QString& filePath, const QByteArray& objectId) = 0;
};


} // namespace imtrepo


