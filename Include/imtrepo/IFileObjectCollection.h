#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <ifile/IFileResourceTypeConstraints.h>
#include <ifile/IFileMetaInfoProvider.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtrepo/IFileCollectionInfo.h>


namespace imtrepo
{


/**
	Common interface for a object collection where the objects are stored in the file system.
	\ingroup Collection
*/
class IFileObjectCollection: virtual public imtbase::IObjectCollection, virtual public IFileCollectionInfo
{
public:
	enum MetaInfoType
	{
		/**
			Name of the user who has added the element.
		*/
		MIT_REVISION = idoc::IDocumentMetaInfo::MIT_USER + 20000,
	};

	/**
		Information related to the file item in the file collection.
	*/
	struct FileInfo
	{
		/**
			Name of the file in the collection folder.
		*/
		QString fileName;

		/**
			Path to the file inside of the collection folder.
		*/
		QString filePath;

		/**
			Timestamp of the last file modification.
		*/
		QDateTime lastModified;
	};

	/**
		Get type contraints describing each supported type of a resource.
		\return	Type constraints, if available or \c nullptr, if no type constraints were defined.
	*/
	virtual const ifile::IFileResourceTypeConstraints* GetFileTypeConstraints() const = 0;

	/**
		Get file information related to a object
		\param objectId	ID of the file in the collection
		\return	Related informations \sa FileInfo
	*/
	virtual FileInfo GetFileInfo(const QByteArray& objectId) const = 0;

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
		\return ID of the file object. If the operation was not successful, the method returns an empty ID.
	*/
	virtual QByteArray InsertFile(
				const QString& filePath,
				const QByteArray& objectTypeId = QByteArray(),
				const QString& objectName = QString(),
				const QString& objectDescription = QString(),
				const QByteArray& proposedObjectId = QByteArray()) = 0;

	/**
		Update file in the collection.
		\param filePath		Path to the file containing the new data content.
		\param objectId		ID of the object to be updated in the collection.
		\return \c true if the file object was successfully updated or \c false otherwise.
	*/
	virtual bool UpdateFile(const QString& filePath, const QByteArray& objectId) = 0;
};


} // namespace imtrepo


