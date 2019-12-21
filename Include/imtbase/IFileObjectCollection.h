#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <ifile/IFileResourceTypeConstraints.h>
#include <ifile/IFileMetaInfoProvider.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


/**
	Common interface for a object collection stored in the of files.
	\ingroup Collection
*/
class IFileObjectCollection: virtual public IObjectCollection
{
public:
	/**
		Type of the meta-informations supported by the file collection.
	*/
	enum MetaInfoType
	{
		/**
			CRC-checksum of the file contents.
		*/
		MIT_CHECKSUM = idoc::IDocumentMetaInfo::MIT_USER + 10000,

		/**
			Name of the user who has added the file.
		*/
		MIT_INSERTION_USER,

		/**
			Timestamp of the file inserting into the collection.
		*/
		MIT_INSERTION_TIME,

		/**
			Name of the user who has modified the file.
		*/
		MIT_LAST_OPERATION_USER,

		/**
			Timestamp of the last operation on the file in the collection.
		*/
		MIT_LAST_OPERATION_TIME,

		/**
			Timestamp of the last operation on the file in the collection.
		*/
		MIT_ITEM_DESCRIPTION
	};

	/**
		Information related to the file inside the repository
	*/
	struct FileInfo
	{
		/**
			Path to the file inside of the repository folder.
		*/
		QString filePath;

		/**
			Name to the file inside of the repository folder.
		*/
		QString fileName;

		/**
			Last modification time.
		*/
		QDateTime lastModified;
	};

	/**
		Get type contraints describing each supported type of a resource.
		\return	Type constraints, if available or \c NULL, if no type constraints were defined.
	*/
	virtual const ifile::IFileResourceTypeConstraints* GetFileTypeConstraints() const = 0;

	/**
		Get meta-information of the item in the collection.
		\param objectId			ID of the object in the collection.
		\param metaInfo			Meta-info to be filled by this method.
		\return \c true if the operation was successful, and \c false if no information could be provided.
	*/
	virtual bool GetItemMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& metaInfo) const = 0;

	/**
		Get meta-information provided by the file itself.
		In difference to GetItemMetaInfo, this method get the meta-informations created from the file contents.
		\param objectId			ID of the object in the collection.
		\param metaInfoPtr		Meta-info pointer to be created and filled by this method.
		\return \c true if the operation was successful, and \c false if no information could be provided.
	*/
	virtual bool GetFileMetaInfo(const QByteArray& objectId, ifile::IFileMetaInfoProvider::MetaInfoPtr& metaInfoPtr) const = 0;

	/**
		Get file information related to a object
		\param objectId	ID of the file in the repository
		\return	Related informations \sa FileInfo
	*/
	virtual FileInfo GetFileInfo(const QByteArray& objectId) const = 0;

	/**
		Get file from the repository.
		\param objectId				ID of the file object.
		\param targetFilePath		Path to the location where the file should be stored. If the path is empty, the implementation should define the location of the file automatically.
		\return Path to the file on the local system or an empty string if the operation has been failed.
	*/
	virtual QString GetFile(const QByteArray& objectId, const QString& targetFilePath = QString()) const = 0;

	/**
		Add a file to the repository 
		\param filePath				Path to the file.
		\param objectTypeId			Type ID of the file object. \sa GetFileTypeConstraints
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
		Update file in the repository.
		\param filePath		Path to the file containing the new data content.
		\param objectId		ID of the object to be updated in the repository.
		\return \c true if the file object was successfully updated or \c false otherwise.
	*/
	virtual bool UpdateFile(const QString& filePath, const QByteArray& objectId) = 0;
};


} // namespace imtbase


