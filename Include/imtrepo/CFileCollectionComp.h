#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QDateTime>
#include <QtCore/QThread>
#include <QtCore/QReadWriteLock>
#include <QtCore/QFileInfo>
#include <QtCore/QTimer>

// ACF includes
#include <ibase/IProgressManager.h>
#include <istd/TSmartPtr.h>
#include <iser/ISerializable.h>
#include <imod/TModelWrap.h>
#include <ilog/TLoggerCompWrap.h>
#include <iauth/ILogin.h>
#include <ifile/IFileNameParam.h>
#include <ifile/IFilePersistence.h>
#include <ifile/IFileResourceTypeConstraints.h>
#include <iauth/IRightsProvider.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>
#include <imtrepo/IRevisionController.h>
#include <imtrepo/IFileCollectionInfo.h>
#include <imtrepo/IFileObjectCollection.h>
#include <imtfile/IFileCompression.h>


namespace imtrepo
{


/**
	Implementation of a file-based data object collection.
	Each object will be placed in the file system according to the configured settings:
	*<b>Plain</b>:In this mode the data file and related description files will be simply placed into the file collection folder.
	*<b>Subfolder</b>: Each file will be placed into a separate folder. This folder contains the data file itself, file collection description file, and the meta-info file.
	\ingroup Collection
*/
class CFileCollectionCompBase:
			public QObject,
			public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFileCollectionCompBase)
		I_ASSIGN_MULTI_0(m_resourceFileTypesCompPtr, "FileTypeInfos", "List of file type infos for corresponding resource type", false);
		I_ASSIGN_MULTI_0(m_objectPersistenceListCompPtr, "ObjectPersistenceList", "List of persistence components used for data object loading", false);
		I_ASSIGN_MULTI_0(m_objectFactoryListCompPtr, "ObjectFactoryList", "List of factories used for data object instance creation", false);
		I_ASSIGN_MULTI_0(m_metaInfoCreatorListCompPtr, "MetaInfoCreatorList", "List of meta-info creators related to the object types", false);
		I_ASSIGN_MULTI_0(m_progressManagerListCompPtr, "ProgressManagerList", "List of progress manager components", false);
		I_ASSIGN_MULTI_0(m_eventHandlerListCompPtr, "EventHandlerList", "List of event handler components", false);
		I_ASSIGN(m_compressorCompPtr, "FileCompressor", "File compressor", false, "FileCompressor");
	I_END_COMPONENT;

	/**
		List of file type infos related to registered resource types.
	*/
	I_MULTIREF(ifile::IFileTypeInfo, m_resourceFileTypesCompPtr);

	/**
		List of file persistence components related to registered resource types.
	*/
	I_MULTIREF(ifile::IFilePersistence, m_objectPersistenceListCompPtr);

	/**
		List of data object factories related to registered resource types.
	*/
	I_MULTIFACT(istd::IChangeable, m_objectFactoryListCompPtr);

	/**
		List of meta-info creators related to registered resource types.
	*/
	I_MULTIREF(imtbase::IMetaInfoCreator, m_metaInfoCreatorListCompPtr);

	/**
		Consumers of the progress information
	*/
	I_MULTIREF(ibase::IProgressManager, m_progressManagerListCompPtr);

	/**
		Event handlers
	*/
	I_MULTIREF(imtbase::IObjectCollectionEventHandler, m_eventHandlerListCompPtr);

	/**
		File compressor.
	*/
	I_REF(imtfile::IFileCompression, m_compressorCompPtr);
};


class CFileCollectionComp:
			public CFileCollectionCompBase,
			virtual public IFileObjectCollection,
			virtual public IRevisionController
{
	Q_OBJECT
public:
	typedef CFileCollectionCompBase BaseClass;

	I_BEGIN_COMPONENT(CFileCollectionComp)
		I_REGISTER_INTERFACE(IFileObjectCollection);
		I_REGISTER_INTERFACE(IFileCollectionInfo);
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(IObjectCollectionInfo);
		I_REGISTER_INTERFACE(ICollectionInfo);
		I_ASSIGN(m_repositoryPathCompPtr, "RepositoryPath", "Path to the file collection folder", false, "RepositoryPath");
		I_ASSIGN(m_isCalculateCheckSumAttrPtr, "IsCalculateCheckSum", "Calculate check sums for the file", true, true);
		I_ASSIGN(m_resourceTypesCompPtr, "ResourceTypes", "List of supported resource types", false, "ResourceTypes");
		I_ASSIGN(m_useSubfolderAttrPtr, "UseSubfolder", "If set, for each input file a subfolder with the corresponding file name will be created", true, false);
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", true, "VersionInfo");
		I_ASSIGN(m_pollFileSystemAttrPtr, "PollFileSystem", "If enabled, the collection folder will be observed and the items will be re-read on changes in the folder structure", true, false);
		I_ASSIGN(m_pollingPeriodAttrPtr, "PollingPeriod", "Period of file system polling (seconds)", true, 1);
		I_ASSIGN(m_asynchronousReadingAttrPtr, "AsynchronousReading", "If enabled, the collection will reading asynchronously", true, false);
		I_ASSIGN(m_isEnableRevisionHistoryAttrPtr, "IsEnableRevisionHistory", "Allow saving item revisions", true, false);
		I_ASSIGN(m_rightsProviderCompPtr, "RightsProvider", "Rights provider", false, "RightsProvider");
		I_ASSIGN(m_restoreRevisionRightIdAttrPtr, "RestoreRevisionRightId", "Restore to revision right id", true, "RestoreRevision");
		I_ASSIGN(m_loginProviderCompPtr, "Login", "Login component", false, "Login");
	I_END_COMPONENT;

	CFileCollectionComp();

	// reimplemented (IRevisionController)
	virtual bool IsRevisionHistoryEnabled() const override;
	virtual RevisionInfoList GetRevisionInfoList(const QByteArray& objectId) const override;
	virtual bool RestoreRevision(const QByteArray& objectId, int revision) override;

	// reimplemented (IFileObjectCollection)
	virtual const ifile::IFileResourceTypeConstraints* GetFileTypeConstraints() const override;
	virtual FileInfo GetFileInfo(const QByteArray& resourceId) const override;
	virtual QString GetFile(
				const QByteArray& resourceId,
				const QString& targetFilePath) const override;
	virtual QByteArray InsertFile(
				const QString& localFilePath,
				const QByteArray& typeId = QByteArray(),
				const QString& resourceName = QString(),
				const QString& resourceDescription = QString(),
				const QByteArray& resourceId = QByteArray()) override;
	virtual bool UpdateFile(
				const QString& localFilePath,
				const QByteArray& resourceId) override;

	// reimplemented (IFileCollectionInfo)
	virtual QString GetCollectionRootFolder() const override;

	// reimplemented (IObjectCollection)
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;
	virtual bool GetDataMetaInfo(const QByteArray& objectId, ifile::IFileMetaInfoProvider::MetaInfoPtr& metaInfoPtr) const override;
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable * defaultValuePtr = nullptr,
				const QByteArray& proposedObjectId = QByteArray()) override;
	virtual bool RemoveObject(const QByteArray& objectId) override;
	virtual const istd::IChangeable* GetObjectPtr(const QByteArray& objectId) const override;
	virtual bool GetObjectData( const QByteArray& objectId, DataPtr& dataPtr) const override;
	virtual bool SetObjectData( const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual void SetObjectName(const QByteArray& objectId, const QString& objectName) override;
	virtual void SetObjectDescription(const QByteArray& objectId, const QString& objectDescription) override;
	virtual void SetObjectEnabled(const QByteArray& objectId, bool isEnabled = true) override;
	virtual bool RegisterEventHandler(imtbase::IObjectCollectionEventHandler* eventHandler) override;
	virtual bool UnregisterEventHandler(imtbase::IObjectCollectionEventHandler* eventHandler) override;

	// reimplemented (IObjectCollectionInfo)
	virtual bool GetCollectionItemMetaInfo(const QByteArray& objectId, idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual Id GetObjectTypeId(const QByteArray& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	/**
		Internal structure representing the file item in the collection.
	*/
	class CollectionItem: virtual public iser::ISerializable
	{
	public:
		CollectionItem(const QString& repositoryFolderPath)
			:m_repositoryFolderPath(repositoryFolderPath)
		{
		}

		// reimplement (iser::ISerializable)
		virtual bool Serialize(iser::IArchive& archive) override;

		// reimplement (istd::IChangeable)
		virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;

	public:
		/**
			Path to the root folder of the repository.
		*/
		QString m_repositoryFolderPath;

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
			Meta-informations for the file item in the collection.
		*/
		idoc::CStandardDocumentMetaInfo metaInfo;

		/**
			Meta-informations for the file contents.
		*/
		imtbase::IMetaInfoCreator::MetaInfoPtr contentsMetaInfoPtr;
	};

	class RevisionMetaInfo: public RevisionInfo, virtual public iser::ISerializable
	{
	public:
		// reimplement (iser::ISerializable)
		virtual bool Serialize(iser::IArchive& archive) override;

		// reimplement (istd::IChangeable)
		virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	};

	struct RepositoryInfo
	{
		QString metaInfoFileSuffix;
		QString dataFileSuffix;
	};

	static RepositoryInfo GetRepositoryInfo();

	/**
		Create data object for the given type.
	*/
	virtual istd::IChangeable* CreateDataObject(const QByteArray& typeId) const;

	/**
		Get persistence object for the related resource type.
	*/
	virtual const ifile::IFilePersistence* GetPersistenceForResource(const QByteArray& typeId) const;

	/**
		Get the index of the file by file ID.
		If the file was not found, the method will return a negative number.
	*/
	int GetFileIndexById(const QByteArray& fileId) const;

	/**
		Get file collection item by ID.
		If the file was not found, the method will return a negative number.
	*/
	int GetCollectionItemById(const QByteArray& fileId, CollectionItem& item) const;

	/**
		Get the index of the file by file (resource) name.
		If the file was not found, the method will return a negative number.
	*/
	int GetFileIndexByName(const QString& fileName) const;

	/**
		Create object data from the file.
	*/
	virtual istd::IChangeable* CreateObjectFromFile(const QString& filePath, const QByteArray& typeId) const;

	/**
		Create meta-info object for a given file in the file collection.
		\return The instance of the meta-information object, or \c NULL of no meta-info was created for the given file.
	*/
	virtual imtbase::IMetaInfoCreator::MetaInfoPtr CreateFileContentsMetaInfo(const QString& filePath, const QByteArray& typeId) const;

	/**
		Update the meta informations for the existing item.
	*/
	void UpdateItemMetaInfo(CollectionItem& item) const;

	/**
		Save file's meta info.
		\param metaInfo	Meta info of the resource file.
		\param filePath	Optionally defined name of the meta info file. If not specified, the file path will be calculated automatically.
		\return \c true of the operation was sucessfull, or \c false otherwise.
	*/
	virtual bool SaveMetaInfo(const idoc::IDocumentMetaInfo& metaInfo, const QString& filePath = QString()) const;

	/**
		Load file's meta info.
		\param metaInfo	Meta info of the resource file.
		\param filePath	Path to the meta info file.
		\return \c true of the operation was sucessfull, or \c false otherwise.
	*/
	virtual bool LoadFileMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QString& filePath) const;

	/**
		Calculate path in file collection for the local file path
	*/
	virtual QString CalculateFolderPathInRepository(
				const QString& localFilePath,
				const QString& resourceName,
				const QByteArray& typeId,
				bool useSubfolder,
				bool useNameCounting,
				ilog::IMessageConsumer* messageConsumerPtr) const;

	/**
		Calculate full file path for the item in the file collection.
	*/
	virtual QString CalculateTargetFilePath(
				const QString& targetFolderPath,
				const QString& localFilePath,
				bool useSubfolder) const;

	virtual QStringList DecomressRevisions(const QByteArray& objectId, const QString& path) const;
	virtual bool CreateRevision(const QByteArray& objectId);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	class ResourceTypeConstraints: public ifile::IFileResourceTypeConstraints
	{
	public:
		ResourceTypeConstraints();

		void SetParent(CFileCollectionComp* parentPtr);

		// reimplemented (ifile::IFileResourceTypeConstraints)
		virtual const ifile::IFileTypeInfo* GetFileTypeInfo(int resourceTypeIndex) const override;

		// reimplemented (iprm::IOptionsList)
		virtual int GetOptionsFlags() const override;
		virtual int GetOptionsCount() const override;
		virtual QString GetOptionName(int index) const override;
		virtual QString GetOptionDescription(int index) const override;
		virtual QByteArray GetOptionId(int index) const override;
		virtual bool IsOptionEnabled(int index) const override;

	private:
		CFileCollectionComp* m_parentPtr;
	};

	typedef imod::TModelWrap<ResourceTypeConstraints> ResourceTypeConstraintsModel;

	class ReaderThread: public QThread
	{
	public:
		explicit ReaderThread(CFileCollectionComp* parentPtr);

	private:
		// reimplemented (QThread)
		virtual void run() override;

	private:
		CFileCollectionComp* m_parentPtr;
	};

private:
	typedef QList<CollectionItem> Files;
	typedef QList<imtbase::IObjectCollectionEventHandler*> EventHandlerList;

	bool InsertFileIntoCollection(
				const QString& filePath,
				const QString& resourceName,
				const QByteArray& typeId,
				ilog::IMessageConsumer* messageConsumerPtr,
				QString& filePathInRepository);

	/**
		Write a file collection item to file system
	*/
	QString SaveCollectionItem(const CollectionItem& repositoryItem) const;
	QString GetTempDirectory() const;
	QString GetDataItemFilePath(const CollectionItem& repositoryFile) const;
	QString GetMetaInfoFilePath(const CollectionItem& repositoryFile) const;
	QString CalculateShortFileName(const QString& fileName, const QFileInfo& fileInfo, const QString& prefix) const;

	void SyncRead();
	void GetRepositoryFileList(QFileInfoList& fileList);
	void ReadItem(Files& filesPtr, const QString& itemFilePath);

	void StartReader();
	Q_INVOKABLE void OnReaderProgress(int progress);
	Q_INVOKABLE void OnReaderFinished();
	Q_INVOKABLE void OnReaderInterrupted();

private Q_SLOTS:
	void OnSync();

private:
	QTimer m_syncTimer;
	mutable bool m_directoryBlocked;
	ReaderThread m_readerThread;
	Files m_readerFiles;
	
	class DirectoryBlocker
	{
	public:
		DirectoryBlocker(const CFileCollectionComp& parent)
			:m_parent(parent)
		{
			parent.m_directoryBlocked = true;
		}

		~DirectoryBlocker()
		{
			m_parent.m_directoryBlocked = false;
		}

	private:
		const CFileCollectionComp& m_parent;
	};

	/**
		Collection data.
	*/
	mutable Files m_files;

	EventHandlerList m_eventHandlerList;

	/**
		Supported types of the objects in the collection.
	*/
	ResourceTypeConstraintsModel m_resourceTypeConstraints;

	/**
		Mutex used for locking the collectio items.
	*/
	mutable QReadWriteLock m_collectionLock;

	typedef QMap<QByteArray, const imtbase::IMetaInfoCreator*> MetaInfoCreatorMap;
	MetaInfoCreatorMap m_metaInfoCreatorMap;

	/**
		Path to the directory where the file file collection is located.
		If the path is set, the incomming file will be copied to this location,
		otherwise only a link to the file wil be created.
	*/
	I_REF(ifile::IFileNameParam, m_repositoryPathCompPtr);

	/**
		Calculate checkSum for files
	*/
	I_ATTR(bool, m_isCalculateCheckSumAttrPtr);

	/**
		List of supported resource types.
	*/
	I_REF(iprm::IOptionsList, m_resourceTypesCompPtr);
	
	/**
		Provider of the version information for the entire system.
	*/
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);

	/**
		If set, for each input file a subfolder with the corresponding file name will be created.
	*/
	I_ATTR(bool, m_useSubfolderAttrPtr);

	/**
		Poll changes in file system to get automatic updates if an external process do some changes in the file collection.
		Polling is disabled if asynchronous loading is enabled
	*/
	I_ATTR(bool, m_pollFileSystemAttrPtr);

	/**
		Polling period (seconds).
	*/
	I_ATTR(int, m_pollingPeriodAttrPtr);

	/**
		Asynchronous collection loading on dedicated thread
	*/
	I_ATTR(bool, m_asynchronousReadingAttrPtr);

	/**
		Enable items changing history
	*/
	I_ATTR(bool, m_isEnableRevisionHistoryAttrPtr);

	/**
		Rights provider
	*/
	I_REF(iauth::IRightsProvider, m_rightsProviderCompPtr);
	I_ATTR(QByteArray, m_restoreRevisionRightIdAttrPtr);

	/**
		Provider of logged in user.
	*/
	I_REF(iauth::ILogin, m_loginProviderCompPtr);
};


} // namespace imtrepo


