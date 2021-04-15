#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>
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
#include <idoc/CStandardDocumentMetaInfo.h>
#include <ifile/IFileNameParam.h>
#include <ifile/IFilePersistence.h>
#include <ifile/IFileResourceTypeConstraints.h>
#include <iauth/ILogin.h>
#include <iauth/IRightsProvider.h>

// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>
#include <imtbase/IRevisionController.h>
#include <imtfile/IFileCompression.h>
#include <imtrepo/IFileCollectionInfo.h>
#include <imtrepo/IFileObjectCollection.h>
#include <imtrepo/IRepositoryItemInfoProvider.h>


namespace imtrepo
{


class CFileCollectionCompBaseAttr:
			public QObject,
			public ilog::CLoggerComponentBase
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CFileCollectionCompBaseAttr)
		I_ASSIGN(m_revisionAttrPtr, "RepositoryRevision", "Revision number of the whole repository", true, 0);
		I_ASSIGN(m_isCalculateCheckSumAttrPtr, "IsCalculateCheckSum", "Calculate check sums for the file", true, true);
		I_ASSIGN(m_asynchronousReadingAttrPtr, "AsynchronousReading", "If enabled, the collection will reading asynchronously", true, false);
	I_END_COMPONENT;

protected:
	/**
		Whole repository revision.
	*/
	I_ATTR(int, m_revisionAttrPtr);

	/**
		Calculate checkSum for files
	*/
	I_ATTR(bool, m_isCalculateCheckSumAttrPtr);

	/**
		Asynchronous collection loading on dedicated thread
	*/
	I_ATTR(bool, m_asynchronousReadingAttrPtr);
};


/**
	Base class of the file collection. General implementation
*/
class CFileCollectionCompBase:
			public CFileCollectionCompBaseAttr,
			virtual public IFileObjectCollection,
			virtual public IRepositoryItemInfoProvider
{
	Q_OBJECT
public:
	typedef CFileCollectionCompBaseAttr BaseClass;

	I_BEGIN_BASE_COMPONENT(CFileCollectionCompBase)
		I_REGISTER_INTERFACE(IFileObjectCollection);
		I_REGISTER_INTERFACE(IFileCollectionInfo);
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(IObjectCollectionInfo);
		I_REGISTER_INTERFACE(ICollectionInfo);
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", true, "VersionInfo");
		I_ASSIGN(m_loginProviderCompPtr, "Login", "Provider of login data used for revision management", false, "Login");
		I_ASSIGN(m_rightsProviderCompPtr, "RightsProvider", "Rights provider", false, "RightsProvider");
		I_ASSIGN(m_repositoryPathCompPtr, "RepositoryPath", "Path to the file collection folder", false, "RepositoryPath");
		I_ASSIGN(m_compressorCompPtr, "FileCompressor", "File compressor", false, "FileCompressor");
		I_ASSIGN(m_resourceTypesCompPtr, "ResourceTypes", "List of supported resource types", false, "ResourceTypes");
		I_ASSIGN_MULTI_0(m_progressManagerListCompPtr, "ProgressManagerList", "List of progress manager components", false);
		I_ASSIGN_MULTI_0(m_objectPersistenceListCompPtr, "ObjectPersistenceList", "List of persistence components used for data object loading", false);
		I_ASSIGN_MULTI_0(m_resourceFileTypesCompPtr, "FileTypeInfos", "List of file type infos for corresponding resource type", false);
		I_ASSIGN_MULTI_0(m_metaInfoCreatorListCompPtr, "MetaInfoCreatorList", "List of meta-info creators related to the object types", false);
		I_ASSIGN_MULTI_0(m_eventHandlerListCompPtr, "EventHandlerList", "List of event handler components", false);
		I_ASSIGN_MULTI_0(m_objectFactoryListCompPtr, "ObjectFactoryList", "List of factories used for data object instance creation", false);
	I_END_COMPONENT;

public:
	CFileCollectionCompBase();

	// reimplemented (IRepositoryItemInfoProvider)
	virtual const imtbase::ICollectionInfo& GetRepositoryItems() override;
	virtual const IRepositoryItemInfo* GetRepositoryItemInfo(const QByteArray& itemId) const override;

	// reimplemented (IFileCollectionInfo)
	virtual QString GetCollectionRootFolder() const override;
	virtual int GetRepositoryRevision() const override;

	// reimplemented (IFileObjectCollection)
	virtual const ifile::IFileResourceTypeConstraints* GetFileTypeConstraints() const override;
	virtual FileInfo GetFileInfo(const QByteArray& objectId) const override;
	virtual QString GetFile(const QByteArray& objectId, const QString& targetFilePath = QString()) const override;
	virtual QByteArray InsertFile(
				const QString& filePath,
				const QByteArray& objectTypeId = QByteArray(),
				const QString& objectName = QString(),
				const QString& objectDescription = QString(),
				const QByteArray& proposedObjectId = QByteArray()) override;
	virtual bool UpdateFile(const QString& filePath, const QByteArray& objectId) override;
	virtual bool ExportFile(const QByteArray& objectId, const QString& targetFilePath = QString()) const override;
	virtual QByteArray ImportFile(const QByteArray& typeId, const QString& sourceFilePath = QString()) override;

	// reimplemented (IObjectCollection)
	virtual const imtbase::IRevisionController* GetRevisionController() const override;
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;
	virtual bool GetDataMetaInfo(const QByteArray& objectId, MetaInfoPtr& metaInfoPtr) const override;
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const QByteArray& proposedObjectId = QByteArray()) override;
	virtual bool RemoveObject(const QByteArray& objectId) override;
	virtual const istd::IChangeable* GetObjectPtr(const QByteArray& objectId) const override;
	virtual bool GetObjectData(const QByteArray& objectId, DataPtr& dataPtr) const override;
	virtual bool SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
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

protected:
	class ResourceLocker
	{
	public:
		explicit ResourceLocker(
			CFileCollectionCompBase& collection,
			const QByteArray& resourceId,
			const QString& resourceName);
		~ResourceLocker();

	private:
		CFileCollectionCompBase& m_collection;
		QByteArray m_resourceId;
		QString m_resourceName;
	};

	struct RepositoryInfo
	{
		QString metaInfoFileSuffix;
		QString dataFileSuffix;
	};

	class RepositoryItemInfo: virtual public IRepositoryItemInfo
	{
	public:
		virtual void SetRepositoryItemFilePath(RepositoryFileType fileId, const QString& filePath);

		// reimplemented (IRepositoryItemInfo)
		virtual RepositoryFileTypes GetRepositoryItemFileTypes() const override;
		virtual QString GetRepositoryItemFilePath(RepositoryFileType fileId) const override;

	private:
		QMap<int, QString> m_files;
	};

	class RepositoryItemInfoProvider:
		virtual public imtbase::ICollectionInfo,
		virtual public IRepositoryItemInfoProvider
	{
	public:
		RepositoryItemInfoProvider(CFileCollectionCompBase& parent);

		bool UpdateItems();

		// reimplemented (IRepositoryItemInfoProvider)
		virtual const imtbase::ICollectionInfo& GetRepositoryItems() override;
		virtual const IRepositoryItemInfo* GetRepositoryItemInfo(const QByteArray& itemId) const override;

		// reimplemented (imtbase::ICollectionInfo)
		virtual Ids GetElementIds() const override;
		virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

	private:
		struct Item
		{
			QByteArray id;
			RepositoryItemInfo itemInfo;
		};

	private:
		CFileCollectionCompBase& m_parent;
		QList<Item> m_repositoryItems;

		mutable QReadWriteLock m_lock;
	};

	/**
		Internal structure representing the file item in the collection.
	*/
	class CollectionItem: virtual public iser::ISerializable
	{
	public:
		CollectionItem(const QString& repositoryFolderPath, int repositoryRev)
			:m_repositoryFolderPath(repositoryFolderPath),
			repositoryRevision(repositoryRev)
		{
		}

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
		idoc::CStandardDocumentMetaInfo metaInfo;

		/**
			Meta-informations for the file contents.
		*/
		imtbase::IMetaInfoCreator::MetaInfoPtr contentsMetaInfoPtr;

	private:
		/**
			Path to the root folder of the repository.
		*/
		QString m_repositoryFolderPath;
	};

	typedef QList<CollectionItem> Files;

	class ResourceTypeConstraints: public ifile::IFileResourceTypeConstraints
	{
	public:
		ResourceTypeConstraints();
		void SetParent(CFileCollectionCompBase* parentPtr);

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
		CFileCollectionCompBase* m_parentPtr;
	};

	typedef imod::TModelWrap<ResourceTypeConstraints> ResourceTypeConstraintsModel;

	typedef QList<imtbase::IObjectCollectionEventHandler*> EventHandlerList;

protected:
	static RepositoryInfo GetRepositoryInfo();

	void EnumerateRepositoryItems(QFileInfoList& fileList) const;
	QString CreateWorkingDir() const;

	/**
		Create data object for the given type.
	*/
	virtual istd::IChangeable* CreateDataObject(const QByteArray& typeId) const;

	/**
		Create object data from the file.
	*/
	virtual istd::IChangeable* CreateObjectFromFile(const QString& filePath, const QByteArray& typeId) const;

	/**
		Get persistence object for the related resource type.
	*/
	virtual const ifile::IFilePersistence* GetPersistenceForResource(const QByteArray& typeId) const;

	/**
		Write a file collection item to file system.
		\param repositoryItem collection item
		\param dataFilePath target file path for the data item. If this parameter is empty file path defined in \c repositoryItem will be used.
		\todo Refactor this method --> liquid shit in the brain of the programmer!
	*/
	QString SaveCollectionItem(const CollectionItem& repositoryItem, const QString& dataFilePath = QString()) const;
	QString GetDataItemFilePath(const CollectionItem& repositoryFile) const;
	QString GetMetaInfoFilePath(const CollectionItem& repositoryFile) const;
	QString CalculateShortFileName(const QString& fileName, const QFileInfo& fileInfo, const QString& prefix) const;

	/**
		Save file's meta info.
		\param metaInfo	Meta info of the resource file.
		\param filePath	Optionally defined name of the meta info file. If not specified, the file path will be calculated automatically.
		\return \c true of the operation was sucessfull, or \c false otherwise.
	*/
	virtual bool SaveMetaInfo(const idoc::IDocumentMetaInfo& metaInfo, const QString& metaInfoFilePath) const;

	/**
		Load file's meta info.
		\param metaInfo	Meta info of the resource file.
		\param filePath	Path to the meta info file.
		\return \c true of the operation was sucessfull, or \c false otherwise.
	*/
	virtual bool LoadMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QString& metaInfoFilePath) const;

	bool IsPathInsideRepository(const QString& filePath) const;

	/**
		Get the index of the file by file ID.
		If the file was not found, the method will return a negative number.
	*/
	int GetFileIndexById(const QByteArray& fileId) const;

	/**
		Get the index of the file by file (resource) name.
		If the file was not found, the method will return a negative number.
	*/
	int GetFileIndexByName(const QString& fileName) const;

	/**
	Create meta-info object for a given file in the file collection.
	\return The instance of the meta-information object, or \c NULL of no meta-info was created for the given file.
*/
	virtual IFileObjectCollection::MetaInfoPtr CreateItemMetaInfo(const QString& dataObjectFilePath, const QByteArray& typeId) const;

	/**
		Create the meta information file.
	*/
	bool CreateItemMetaInfoFile(const QString& dataObjectFilePath, const QByteArray& typeId, const QString& metaInfoFilePath) const;

	/**
		Update the meta informations for the existing item.
	*/
	void UpdateItemMetaInfo(CollectionItem& item) const;

	void ReadRepositoryItems();
	void ReadItem(Files& filesPtr, const QString& itemFilePath);
	bool ReadItemFile(CollectionItem& collectionItem, const QString& itemFilePath);

	void StartRepositoryLoader();
	Q_INVOKABLE void OnReaderProgress(int progress);
	Q_INVOKABLE void OnReaderFinished();
	Q_INVOKABLE void OnReaderInterrupted();

	bool IsObjectIdLocked(const QByteArray& resourceId);
	bool IsObjectNameLocked(const QString& resourceName);
	bool IsObjectIdUsed(const QByteArray& objectId);

	virtual QString CalculateFolderPathInRepository(
				const QString& localFilePath,
				const QString& resourceName,
				const QByteArray& typeId,
				ilog::IMessageConsumer* messageConsumerPtr) const = 0;

	virtual QString CalculateTargetFilePath(
				const QString& filePath,
				const QString& objectName,
				const QByteArray& typeId) const = 0;

	virtual QString CalculateTargetFilePath(
				const QString& targetFolderPath,
				const QString& localFilePath) const = 0;

	bool FinishInsertFileTransaction(
				const QString& workingPath,
				const QString& repositoryPath,
				const QByteArray& fileId,
				const CollectionItem& collectionItem);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	/**
		Supported types of the objects in the collection.
	*/
	ResourceTypeConstraintsModel m_resourceTypeConstraints;

	/**
		Collection data.
		Mutex used for locking the collection items.
	*/
	mutable Files m_files;
	mutable QReadWriteLock m_filesLock;

	EventHandlerList m_eventHandlerList;

protected:
	/**
	Provider of the version information for the entire system.
*/
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);

	/**
		Provider of the logged user.
	*/
	I_REF(iauth::ILogin, m_loginProviderCompPtr);

	/**
		Rights provider
	*/
	I_REF(iauth::IRightsProvider, m_rightsProviderCompPtr);

	/**
		Path to the directory where the file file collection is located.
		If the path is set, the incomming file will be copied to this location,
		otherwise only a link to the file wil be created.
	*/
	I_REF(ifile::IFileNameParam, m_repositoryPathCompPtr);

	/**
		File compressor.
	*/
	I_REF(imtfile::IFileCompression, m_compressorCompPtr);

	/**
		List of supported resource types.
	*/
	I_REF(iprm::IOptionsList, m_resourceTypesCompPtr);

	/**
		Consumers of the progress information
	*/
	I_MULTIREF(ibase::IProgressManager, m_progressManagerListCompPtr);

	/**
		List of file persistence components related to registered resource types.
	*/
	I_MULTIREF(ifile::IFilePersistence, m_objectPersistenceListCompPtr);

	/**
		List of file type infos related to registered resource types.
	*/
	I_MULTIREF(ifile::IFileTypeInfo, m_resourceFileTypesCompPtr);

	/**
		List of meta-info creators related to registered resource types.
	*/
	I_MULTIREF(imtbase::IMetaInfoCreator, m_metaInfoCreatorListCompPtr);

	/**
		Event handlers
	*/
	I_MULTIREF(imtbase::IObjectCollectionEventHandler, m_eventHandlerListCompPtr);

	/**
		List of data object factories related to registered resource types.
	*/
	I_MULTIFACT(istd::IChangeable, m_objectFactoryListCompPtr);

private:
	class ReaderThread: public QThread
	{
	public:
		explicit ReaderThread(CFileCollectionCompBase* parentPtr);

	private:
		// reimplemented (QThread)
		virtual void run() override;

	private:
		CFileCollectionCompBase* m_parentPtr;
	};

private:
	ReaderThread m_readerThread;
	Files m_readerFiles;

	typedef QMap<QByteArray, const imtbase::IMetaInfoCreator*> MetaInfoCreatorMap;
	MetaInfoCreatorMap m_metaInfoCreatorMap;

	QList<QByteArray> m_lockedObjectIds;
	QList<QString> m_lockedObjectNames;
	QMutex m_lockedObjectInfoMutex;

	RepositoryItemInfoProvider m_itemInfoProvider;
};


} // namespace imtrepo


