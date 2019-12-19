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
			Type-ID of the file object in the collection.
		*/
		MIT_RESOURCE_TYPE_ID
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
	virtual const ifile::IFileResourceTypeConstraints* GetResourceTypeConstraints() const = 0;

	/**
		Get meta info of an existing file resource.
		\param objectId			ID of the file resource.
		\return Pointer to the meta-info instance or \c NULL, if no information could be provided.
	*/
	virtual ifile::IFileMetaInfoProvider::MetaInfoPtr GetFileMetaInfo(const QByteArray& objectId) const = 0;

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
	virtual QString GetFile(
				const QByteArray& objectId,
				const QString& targetFilePath = QString()) const = 0;

	/**
		Add a file to the repository. This method works synchronous, what means that the file must already exist on the file system before the operation can be executed. 
		In the event, that the repository is running on another machine adding of a file means an upload to the repository server.
		\param filePath				Location of the file.
		\param objectTypeId			Type ID of the file object. \sa GetResourceTypeConstraints
		\param objectName			Name of the file object used for human readable representation. If no name was set, the file name will be used.
		\param objectDescription	Description of the file object.
		\param objectId				Externally defined object's unique ID.
		\return ID of the file object. If the operation was not successful, the method returns an empty ID.
	*/
	virtual QByteArray AddFile(
				const QString& filePath,
				const QByteArray& objectTypeId = QByteArray(),
				const QString& objectName = QString(),
				const QString& objectDescription = QString(),
				const QByteArray& objectId = QByteArray()) = 0;

	/**
		Update file in the repository.
		\param filePath		Location of the file containing the new content.
		\param objectId		ID of the object to be updated in the repository.
		\return \c true if the file object was successfully updated or \c false otherwise.
	*/
	virtual bool UpdateFile(
				const QString& filePath,
				const QByteArray& objectId) = 0;
};


} // namespace imtbase


