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
#include <imtbase/ICollectionDataController.h>
#include <imtbase/CObjectCollectionMetaInfo.h>
#include <imtfile/IFileCompression.h>
#include <imtrepo/IFileCollectionInfo.h>
#include <imtrepo/IFileObjectCollection.h>
#include <imtrepo/IRepositoryItemInfoProvider.h>
#include <imtrepo/CFileCollectionItem.h>


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
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", true, "VersionInfo");
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

	/**
		Provider of the version information for the entire system.
	*/
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
};


/**
	Base class of the file collection. General implementation
*/
class CFileCollectionCompBase:
			public CFileCollectionCompBaseAttr,
			virtual public IFileObjectCollection,
			virtual public imtbase::IObjectCollection::IDataFactory,
			virtual public IRepositoryItemInfoProvider,
			virtual protected imtbase::ICollectionDataController
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
		I_ASSIGN(m_loginProviderCompPtr, "Login", "Provider of login data used for revision management", false, "Login");
		I_ASSIGN(m_repositoryPathCompPtr, "RepositoryPath", "Path to the file collection folder", false, "RepositoryPath");
		I_ASSIGN(m_compressorCompPtr, "FileCompressor", "File compressor", false, "FileCompressor");
		I_ASSIGN(m_resourceTypesCompPtr, "ResourceTypes", "List of supported resource types", false, "ResourceTypes");
		I_ASSIGN(m_helperFilesPersistenceCompPtr, "HelperFilesPersistence", "Persistence component used for loading / saving helper (item and meta) files. Replaced by default persistence engine if not specified", false, "HelperFilesPersistence");
		I_ASSIGN_MULTI_0(m_progressManagerListCompPtr, "ProgressManagerList", "List of progress manager components", false);
		I_ASSIGN_MULTI_0(m_objectPersistenceListCompPtr, "ObjectPersistenceList", "List of persistence components used for data object loading", false);
		I_ASSIGN_MULTI_0(m_resourceFileTypesCompPtr, "FileTypeInfos", "List of file type infos for corresponding resource type", false);
		I_ASSIGN_MULTI_0(m_metaInfoCreatorListCompPtr, "MetaInfoCreatorList", "List of meta-info creators related to the object types", false);
		I_ASSIGN_MULTI_0(m_objectFactoryListCompPtr, "ObjectFactoryList", "List of factories used for data object instance creation", false);
	I_END_COMPONENT;

public:
	CFileCollectionCompBase();

	// reimplemented (IRepositoryItemInfoProvider)
	virtual const imtbase::ICollectionInfo& GetRepositoryItems() override;
	virtual const IRepositoryItemInfo* GetRepositoryItemInfo(const QByteArray& itemId) const override;

	// reimplemented (IFileCollectionInfo)
	virtual const ifile::IFileResourceTypeConstraints* GetFileTypeConstraints() const override;
	virtual bool GetFileInfo(const QByteArray& objectId, IFileCollectionItem& fileCollectionItem) const override;
	virtual QString GetCollectionRootFolder() const override;
	virtual int GetRepositoryRevision() const override;

	// reimplemented (IFileObjectCollection)
	virtual QString GetFile(const QByteArray& objectId, const QString& targetFilePath = QString()) const override;
	virtual QByteArray InsertFile(
				const QString& filePath,
				const QByteArray& objectTypeId = QByteArray(),
				const QString& objectName = QString(),
				const QString& objectDescription = QString(),
				const QByteArray& proposedObjectId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr) override;
	virtual bool UpdateFile(const QString& filePath, const QByteArray& objectId) override;

	// reimplemented (ICollectionDataController)
	virtual const ifile::IFilePersistence* GetPersistenceForObjectType(const QByteArray& typeId) const override;
	virtual bool ExportFile(const imtbase::IObjectCollection& collection, const QByteArray& objectId, const QString& targetFilePath = QString()) const override;
	virtual Id ImportFile(imtbase::IObjectCollection& collection, const QByteArray& typeId, const QString& sourceFilePath = QString()) const override;

	// reimplemented (IObjectCollection)
	virtual const imtbase::IRevisionController* GetRevisionController() const override;
	virtual const imtbase::ICollectionDataController* GetDataController() const override;
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				DataPtr defaultValuePtr = DataPtr(),
				const QByteArray& proposedObjectId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveElement(const Id& elementId, const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual const istd::IChangeable* GetObjectPtr(const QByteArray& objectId) const override;
	virtual bool GetObjectData(const QByteArray& objectId, DataPtr& dataPtr) const override;
	virtual bool SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS, const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual imtbase::IObjectCollection* CreateSubCollection(int offset = 0, int count = -1, const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtbase::IObjectCollectionIterator* CreateObjectCollectionIterator(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;

	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual Id GetObjectTypeId(const QByteArray& objectId) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo(const Id& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual int GetElementsCount(
				const iprm::IParamsSet* selectionParamPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool GetSubsetInfo(
				imtbase::ICollectionInfo& subsetInfo,
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true, ilog::IMessageConsumer* logPtr = nullptr) override;

	// reimplemented (IObjectCollection::IDataFactory)
	virtual DataPtr CreateInstance(const QByteArray& keyId = "") const override;
	virtual istd::IFactoryInfo::KeyList GetFactoryKeys() const override;

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
		virtual int GetElementsCount(
					const iprm::IParamsSet* selectionParamPtr = nullptr,
					ilog::IMessageConsumer* logPtr = nullptr) const override;
		virtual Ids GetElementIds(
					int offset = 0,
					int count = -1,
					const iprm::IParamsSet* selectionParamsPtr = nullptr,
					ilog::IMessageConsumer* logPtr = nullptr) const override;
		virtual bool GetSubsetInfo(
					imtbase::ICollectionInfo& subsetInfo,
					int offset = 0,
					int count = -1,
					const iprm::IParamsSet* selectionParamsPtr = nullptr,
					ilog::IMessageConsumer* logPtr = nullptr) const override;
		virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType, ilog::IMessageConsumer* logPtr = nullptr) const override;
		virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr = nullptr) const override;
		virtual bool SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr = nullptr) override;
		virtual bool SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr = nullptr) override;
		virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true, ilog::IMessageConsumer* logPtr = nullptr) override;

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


	typedef QList<CFileCollectionItem> Files;

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


protected:
	static RepositoryInfo GetRepositoryInfo();

	void EnumerateRepositoryItems(QFileInfoList& fileList) const;
	QString CreateWorkingDir() const;
	QString GetWorkingExt(
				const ifile::IFilePersistence* persistencePtr,
				const istd::IChangeable* dataObjectPtr,
				const QString& fileName);

	/**
		Create data object for the given type.
	*/
	virtual DataPtr CreateDataObject(const QByteArray& typeId) const;

	/**
		Create object data from the file.
	*/
	virtual DataPtr CreateObjectFromFile(const QString& filePath, const QByteArray& typeId) const;

	/**
		Write a file collection item to file system.
		\param repositoryItem collection item
		\param dataFilePath target file path for the data item. If this parameter is empty file path defined in \c repositoryItem will be used.
		\todo Refactor this method --> liquid shit in the brain of the programmer!
	*/
	QString SaveCollectionItem(const CFileCollectionItem& repositoryItem, const QString& dataFilePath = QString()) const;
	QString GetDataItemFilePath(const CFileCollectionItem& repositoryFile) const;
	QString GetMetaInfoFilePath(const CFileCollectionItem& repositoryFile) const;
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

	/**
		Check if the path to the file is inside of repository folder.
	*/
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
	virtual idoc::MetaInfoPtr CreateItemMetaInfo(const QString& dataObjectFilePath, const QByteArray& typeId) const;

	/**
		Create the meta information file.
	*/
	bool CreateItemMetaInfoFile(const QString& dataObjectFilePath, const QByteArray& typeId, const QString& metaInfoFilePath) const;

	/**
		Update the meta informations for the existing item.
	*/
	void UpdateItemMetaInfo(CFileCollectionItem& item) const;

	void ReadRepositoryItems();
	void ReadItem(Files& files, const QString& itemFilePath) const;
	bool ReadItemFile(CFileCollectionItem& collectionItem, const QString& itemFilePath) const;

	void StartRepositoryLoader();
	Q_INVOKABLE void OnReaderProgress(int progress);
	Q_INVOKABLE void OnReaderFinished();
	Q_INVOKABLE void OnReaderInterrupted();

	bool IsObjectIdLocked(const QByteArray& resourceId);
	bool IsObjectNameLocked(const QString& resourceName);
	bool IsObjectIdUsed(const QByteArray& objectId);

	/**
		Calculate path in file collection for the local file path
	*/
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
				const CFileCollectionItem& collectionItem);

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

	typedef QMap<QByteArray, DataPtr> ObjectCache;
	mutable ObjectCache m_objectCache;
	mutable QReadWriteLock m_objectCacheLock;


protected:
	/**
		Provider of the logged user.
	*/
	I_REF(iauth::ILogin, m_loginProviderCompPtr);

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
		Persistence component for helper (item & meta) files
	*/
	I_REF(ifile::IFilePersistence, m_helperFilesPersistenceCompPtr);

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


