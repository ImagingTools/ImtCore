#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ACF includes
#include <istd/TIFactory.h>
#include <ibase/IProgressManager.h>
#include <ilog/IMessageConsumer.h>
#include <ifile/IFileResourcesManager.h>


namespace imtrepo
{


class IFileRepository: virtual public ifile::IFileResourcesManager
{
public:
	typedef istd::TIFactory<istd::IChangeable> IDataObjectFactory;

	enum ChangeFlags
	{
		CF_FILE_ADDED = 0xacf478,
		CF_FILE_REMOVED,
		CF_FILE_UPDATED
	};

	enum
	{
		MIT_CHECKSUM = idoc::IDocumentMetaInfo::MIT_USER + 10000,
		MIT_RESOURCE_TYPE_ID
	};

	/**
		Binary flags for describing possible operations for a given resource type.
	*/
	enum OperationFlags
	{
		OF_ADD = 1,
		OF_REMOVE = 2,
		OF_DUPLICATE = 4,
		OF_IMPORT = 8,
		OF_EXPORT = 16,
		OF_LOCK = 32,
		OF_EDIT = 64,
		OF_RENAME = 128,
		OF_UPDATE = 256,
		OF_ALL = 512
	};

	/**
		Information related to the file inside the repository
	*/
	struct FileInfo
	{
		/**
			Path to the file inside of the repository.
		*/
		QString filePath;

		/**
			Name to the file inside of the repository.
		*/
		QString fileName;

		/**
			Last modification time.
		*/
		QDateTime lastModified;
	};


	/**
		Get flags for supported operations for the given resource type
		\sa OperationFlags
	*/
	virtual int GetSupportedEditOperations(const QByteArray& resourceTypeId) const = 0;

	/**
		Get file information related to a resource
		\param resourceId	ID of the file in the repository
		\return	Related informations \sa FileInfo
	*/
	virtual FileInfo GetFileInfo(const QByteArray& resourceId) const = 0;

	/**
		Find a resource in the repository using resource name.
		\return ID of the file resource. If the operation was not successful, the method returns an empty ID.
	*/
	virtual QByteArray FindResourceByName(const QString& resourceName) const = 0;

	/**
		Get file from the repository.
		\param resourceId			ID of the file resource.
		\param targetFilePath		Path to the location where the file should be stored. If the path is empty, the implementation will define the location of the file automatically.
		\param progressManagerPtr	Optionally defined progress manager.
		\param messageConsumerPtr	Optionally defined message consumer used for cummulation of possible operation errors, warnings, etc.
		\return Path to the download file on the local system or an empty string if the operation has been failed.
	*/
	virtual QString GetFile(
				const QByteArray& resourceId,
				const QString& targetFilePath = QString(),
				ibase::IProgressManager* progressManagerPtr = NULL,
				ilog::IMessageConsumer* messageConsumerPtr = NULL) const = 0;

	/**
		Add a file to the repository. This method works synchronous, what means that the file must already exist on the file system before the operation can be executed. 
		In the event, that the repository is running on another machine adding of a file means an upload to the repository server.
		\param localFilePath		Location of the file.
		\param resourceTypeId		Type ID of the file resource. \sa GetResourceTypeConstraints
		\param resourceName			Name of the file resource used for human readable representation. If no name was set, the file name will be used.
		\param resourceDescription	Description of the file resource.
		\param resourceId			Externally defined resource's unique ID.
		\param progressManagerPtr	Optionally defined progress manager.
		\param messageConsumerPtr	Optionally defined message consumer used for cummulation of possible operation errors, warnings, etc.
		\return ID of the file resource. If the operation was not successful, the method returns an empty ID.
	*/
	virtual QByteArray AddFile(
				const QString& localFilePath,
				const QByteArray& resourceTypeId = QByteArray(),
				const QString& resourceName = QString(),
				const QString& resourceDescription = QString(),
				const QByteArray& resourceId = QByteArray(),
				ibase::IProgressManager* progressManagerPtr = NULL,
				ilog::IMessageConsumer* messageConsumerPtr = NULL) = 0;

	/**
		Update file in the repository.
		\param localFilePath		Location of the file containing the new content.
		\param resourceId			ID of the resource to be updated in the repository.
		\param progressManagerPtr	Optionally defined progress manager.
		\param messageConsumerPtr	Optionally defined message consumer used for cummulation of possible operation errors, warnings, etc.
		\return \c true if the file resource was successfully updated or \c false otherwise.
	*/
	virtual bool UpdateFile(
				const QString& localFilePath,
				const QByteArray& resourceId,
				ibase::IProgressManager* progressManagerPtr = NULL,
				ilog::IMessageConsumer* messageConsumerPtr = NULL) = 0;

	/**
		Update contents of the resource in the repository.
		\param fileContents			Data object represented the contents of the file on the server side.
		\param resourceId			ID of the resource to be updated in the repository.
		\param progressManagerPtr	Optionally defined prBogress manager.
		\param messageConsumerPtr	Optionally defined message consumer used for cummulation of possible operation errors, warnings, etc.
		\return \c true if the file resource was successfully updated or \c false otherwise.
	*/
	virtual bool UpdateContent(
				const istd::IChangeable& fileContents,
				const QByteArray& resourceId,
				ibase::IProgressManager* progressManagerPtr = NULL,
				ilog::IMessageConsumer* messageConsumerPtr = NULL) = 0;

	/**
		Acquire lock state of a file in the repository.
		\param resourceId			ID of the resource to be updated in the repository.
		\return \c true if the file is locked in the repository or \c false otherwise.
	*/
	virtual bool IsFileLocked(const QByteArray& resourceId) const = 0;

	/**
		Lock/Unlock the file in the repository. Locked resources cannot be edited or removed. Only read-only access to the resource is possible.
		\param resourceId			ID of the resource to be updated in the repository.
		\param isFileLocked			New lock status of the resource.
		\return \c true if the file resource was successfully locked/unlocked or \c false otherwise.
	*/
	virtual bool SetFileLocked(const QByteArray& resourceId, bool isFileLocked) = 0;
};


} // namespace imtrepo


