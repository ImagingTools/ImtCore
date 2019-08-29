#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QDateTime>
#include <QtCore/QThread>
#include <QtCore/QReadWriteLock>
#include <QtCore/QFileInfo>
#include <QtCore/QWaitCondition>

// ACF includes
#include <istd/TSmartPtr.h>
#include <iser/ISerializable.h>
#include <imod/TModelWrap.h>
#include <ilog/TLoggerCompWrap.h>
#include <ilog/CMessageContainer.h>
#include <ifile/IFileNameParam.h>
#include <ifile/IFilePersistence.h>
#include <ifile/IFileResourceTypeConstraints.h>
#include <ifile/IFileResourceValidator.h>
#include <ifile/IFileMetaInfoProvider.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtrepo/IFileRepository.h>
#include <imtrepo/IFileRepositoryInfo.h>


namespace imtrepo
{


class CFileRepositoryCompAttributes: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFileRepositoryCompAttributes)
		I_ASSIGN(m_repositoryPathCompPtr, "RepositoryPath", "Path to the repository folder", false, "RepositoryPath");
		I_ASSIGN(m_isCalculateCheckSumAttrPtr, "IsCalculateCheckSum", "Calculate check sums for the file", true, true);
		I_ASSIGN(m_resourceTypesCompPtr, "ResourceTypes", "List of supported resource types", false, "ResourceTypes");
		I_ASSIGN_MULTI_0(m_resourceFileTypesCompPtr, "FileTypeInfos", "List of file type infos for corresponding resource type", false);
		I_ASSIGN(m_useSubfolderAttrPtr, "UseSubfolder", "If set, for each input file a subfolder with the corresponding file name will be created", true, false);
		I_ASSIGN_MULTI_0(m_objectPersistenceListCompPtr, "ObjectPersistenceList", "List of persistence components used for data object loading", false);
		I_ASSIGN_MULTI_0(m_objectFactoryListCompPtr, "ObjectFactoryList", "List of factories used for data ojbect instance creation", false);
		I_ASSIGN_MULTI_0(m_resourceValidatorListCompPtr, "ResourceValidatorList", "List of components used for validation of the resource", false);
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", true, "VersionInfo");
		I_ASSIGN(m_createFolderOnStartAttrPtr, "CreateRepositoryFolder", "Ensure that the repository folder is created on the start", true, true);
	I_END_COMPONENT;

protected:
	/**
		Path to the directory where the file repository is located.
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
		List of resource validators for each resource type.
	*/
	I_MULTIREF(ifile::IFileResourceValidator, m_resourceValidatorListCompPtr);

	/**
		Version info provider.
	*/
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);

	/**
		If set, for each input file a subfolder with the corresponding file name will be created.
	*/
	I_ATTR(bool, m_useSubfolderAttrPtr);

	/**
		Ensure that the repository folder is created on starting.
	*/
	I_ATTR(bool, m_createFolderOnStartAttrPtr);

	/**
		Uses name counting if a file with the same name exists in the repository.
	*/
	I_ATTR(bool, m_useNameCountingAttrPtr);
};


class CFileRepositoryComp:
			public QObject,
			public CFileRepositoryCompAttributes,
			virtual public IFileRepository,
			virtual public IFileRepositoryInfo,
			virtual public iser::ISerializable,
			virtual public IFileRepository::IDataObjectFactory,
			virtual public ifile::IFileResourceValidator
{
	Q_OBJECT
public:
	typedef CFileRepositoryCompAttributes BaseClass;

	I_BEGIN_COMPONENT(CFileRepositoryComp)
		I_REGISTER_INTERFACE(ifile::IFileResourcesManager);
		I_REGISTER_INTERFACE(ifile::IFileResourceValidator);
		I_REGISTER_INTERFACE(IFileRepository);
		I_REGISTER_INTERFACE(IFileRepositoryInfo);
		I_REGISTER_INTERFACE(IDataObjectFactory);
	I_END_COMPONENT;

	CFileRepositoryComp();

	// reimplemented (ifile::IFileResourceValidator)
	virtual istd::IInformationProvider::InformationCategory ValidateResource(
				const QByteArray& resourceId,
				const ifile::IFileResourcesManager* fileRepositoryPtr,
				ilog::IMessageConsumer* messageConsumerPtr = NULL) const override;

	// reimplemented (IDataObjectFactory)
	virtual istd::IChangeable* CreateInstance(const QByteArray& keyId = "") const override;

	// reimplemented (istd::IFactoryInfo)
	virtual KeyList GetFactoryKeys() const override;

	// reimplemented (ifile::IFileResourcesManager)
	virtual ifile::IFileResourcesManager::Ids GetFileIds() const override;
	virtual const ifile::IFileResourceTypeConstraints* GetResourceTypeConstraints() const override;
	virtual QByteArray GetResourceTypeId(const QByteArray& resourceId) const override;
	virtual QByteArray CreateNewFile(
				const QByteArray& fileTypeId,
				const istd::IChangeable* dataObjectPtr,
				const QString& filePath = QString(),
				const QString& resourceName = QString()) override;
	virtual bool RemoveFile(const QByteArray& fileId) override;
	virtual DataObjectPtr GetDataObject(const QByteArray& fileId) const override;
	virtual MetaInfoPtr GetFileMetaInfo(const QByteArray& resourceId) const override;
	virtual QString GetResourceName(const QByteArray& resourceId)  const override;
	virtual bool SetResourceName(const QByteArray& resourceId, const QString& name) override;
	virtual QString GetResourceDescription(const QByteArray& resourceId)  const override;
	virtual bool SetResourceDescription(const QByteArray& resourceId, const QString& description) override;
	virtual LicenseStatus GetLicenseStatus(const QByteArray& resourceId, const QByteArray& resourceTypeId = QByteArray(), ilog::IMessageConsumer* messageConsumerPtr = NULL) const override;

	// reimplemented (IFileRepository)
	virtual int GetSupportedEditOperations(const QByteArray& resourceTypeId) const override;
	virtual FileInfo GetFileInfo(const QByteArray& resourceId) const override;
	virtual QByteArray FindResourceByName(const QString& resourceName) const override;
	virtual QString GetFile(
				const QByteArray& resourceId,
				const QString& targetFilePath,
				ibase::IProgressManager* progressManagerPtr = NULL,
				ilog::IMessageConsumer* messageConsumerPtr = NULL) const override;
	virtual QByteArray AddFile(
				const QString& localFilePath,
				const QByteArray& resourceTypeId = QByteArray(),
				const QString& resourceName = QString(),
				const QString& resourceDescription = QString(),
				const QByteArray& resourceId = QByteArray(),
				ibase::IProgressManager* progressManagerPtr = NULL,
				ilog::IMessageConsumer* messageConsumerPtr = NULL) override;
	virtual bool UpdateFile(
				const QString& localFilePath,
				const QByteArray& resourceId,
				ibase::IProgressManager* progressManagerPtr = NULL,
				ilog::IMessageConsumer* messageConsumerPtr = NULL) override;
	virtual bool UpdateContent(
				const istd::IChangeable& fileContents,
				const QByteArray& resourceId,
				ibase::IProgressManager* progressManagerPtr = NULL,
				ilog::IMessageConsumer* messageConsumerPtr = NULL) override;

	// reimplemented (IFileRepositoryInfo)
	virtual QString GetRepositoryPath() const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive);

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS);

protected:
	/**
		Internal structure representing the file item in the repository.
	*/
	class RepositoryItem: virtual public iser::ISerializable
	{
	public:
		RepositoryItem(const CFileRepositoryComp& parent)
			:checkSum(0),
			m_parent(&parent)
		{
		}

		// reimplement (iser::ISerializable)
		virtual bool Serialize(iser::IArchive& archive);

		// reimplement (istd::IChangeable)
		virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS);

		/**
			ID of the file in the repository.
		*/
		QByteArray fileId;

		/**
			File path in the repository.
		*/
		QString filePathInRepository;

		/**
			Location of the source file.
		*/
		QString sourceFilePath;

		/**
			Name of the file resource.
		*/
		QString resourceName;

		/**
			Description of the file resource.
		*/
		QString resourceDescription;

		/**
			File was added at the time.
		*/
		QDateTime addedTime;

		/**
			Timestamp of the last file modification.
		*/
		QDateTime lastModificationTime;

		/**
			Type ID of the resource.
		*/
		QByteArray resourceTypeId;

		/**
			CRC-32 check sum of the file.
		*/
		quint32 checkSum;

		/**
			Meta-informations for the file item.
		*/
		ifile::IFileResourcesManager::MetaInfoPtr metaInfoPtr;

		const CFileRepositoryComp* m_parent;
	};

	static QString GetInfoFileExtention();
	static QString GetDataFileExtention();

	/**
		Get resource validator for the given resource type.
	*/
	virtual const ifile::IFileResourceValidator* GetResourceValidator(const QByteArray& resourceTypeId) const;

	/**
		Create data object for the given resource type.
	*/
	virtual CFileRepositoryComp::DataObjectPtr CreateDataObject(const QByteArray& resourceTypeId) const;

	/**
		Get persistence object for the related resource type.
	*/
	virtual const ifile::IFilePersistence* GetPersistenceForResource(const QByteArray& resourceTypeId) const;

	/**
		Get the index of the file by file ID.
		If the file was not found, the method will return a negative number.
	*/
	int GetFileIndexById(const QByteArray& fileId) const;

	/**
		Get repository item by ID.
		If the file was not found, the method will return a negative number.
	*/
	int GetRepositoryItemById(const QByteArray& fileId, RepositoryItem& item) const;

	/**
		Get the index of the file by file (resource) name.
		If the file was not found, the method will return a negative number.
	*/
	int GetFileIndexByName(const QString& fileName) const;

	/**
		Create object data from the file.
	*/
	virtual DataObjectPtr CreateObjectFromFile(const QString& filePath, const QByteArray& resourceTypeId) const;

    /**
		Create meta-info object for a given file in the repository.
		\return The instance of the meta-information object, or \c NULL of no meta-info was created for the given file.
	*/
	virtual ifile::IFileMetaInfoProvider::MetaInfoPtr CreateFileMetaInfo(const QString& filePath, const QByteArray& type, const quint32& checkSum) const;

	/**
		Try to load the meta-informations from the file.
	*/
	virtual ifile::IFileMetaInfoProvider::MetaInfoPtr TryLoadMetaInfoFromFile(const QString& filePath) const;

	/**
		Update the meta informations for the existing item.
	*/
	void UpdateItemMetaInfo(RepositoryItem& item);

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
		Calculate path in repository for the local file path
	*/
	virtual QString CalculateFolderPathInRepository(
				const QString& localFilePath,
				const QString& resourceName,
				const QByteArray& resourceTypeId,
				bool useSubfolder,
				bool useNameCounting,
				ilog::IMessageConsumer* messageConsumerPtr) const;

	/**
		Calculate full file path for the item in the repository.
	*/
	virtual QString CalculateTargetFilePath(
				const QString& targetFolderPath,
				const QString& localFilePath,
				bool useSubfolder) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();
	virtual void OnComponentDestroyed();

protected:
	class ResourceTypeConstraints: public ifile::IFileResourceTypeConstraints
	{
	public:
		ResourceTypeConstraints();

		void SetParent(CFileRepositoryComp* parentPtr);

		// reimplemented (ifile::IFileResourceTypeConstraints)
		virtual const ifile::IFileTypeInfo* GetFileTypeInfo(int resourceTypeIndex) const;

		// reimplemented (iprm::IOptionsList)
		virtual int GetOptionsFlags() const;
		virtual int GetOptionsCount() const;
		virtual QString GetOptionName(int index) const;
		virtual QString GetOptionDescription(int index) const;
		virtual QByteArray GetOptionId(int index) const;
		virtual bool IsOptionEnabled(int index) const;

	private:
		CFileRepositoryComp* m_parentPtr;
	};

	typedef imod::TModelWrap<ResourceTypeConstraints> ResourceTypeConstraintsModel;

private:
	bool InsertFileIntoRepository(
				const QString& filePath,
				const QString& resourceName,
				const QByteArray& resourceTypeId,
				ilog::IMessageConsumer* messageConsumerPtr,
				QString& filePathInRepository);

	/**
		Write a repository item to file system
	*/
	QString SaveRepositoryItem(const RepositoryItem& repositoryItem) const;

	/**
		Read repository contents.
	*/
	void ReadRepositoryItems();

	/**
		Get temp folder used by repository.
	*/
	QString GetTempDirectory() const;

	QString GetDataItemFilePath(const RepositoryItem& repositoryFile) const;
	QString GetMetaInfoFilePath(const RepositoryItem& repositoryFile) const;
	QString ShortenWindowsFilename(const QString& fileName, const QFileInfo& fileInfo, const QString& prefix) const;

	QStringList ConvertMessageContainer(ilog::CMessageContainer& messageContainer);

Q_SIGNALS:
	/**
		Queue remove-file-request.
	*/
	void EmitRemoveFile(const QByteArray& fileId);

private Q_SLOTS:
	void OnFileRemove(const QByteArray& fileId);

private:
	typedef QList<RepositoryItem> Files;

	/**
		Repository data.
	*/
	mutable Files m_files;

	/**
		Resource type desfinitions.
	*/
	ResourceTypeConstraintsModel m_resourceTypeConstraints;

	/**
		Mutex used for locking the complete repository to make possible repository access from different threads.
	*/
	mutable QReadWriteLock m_repositoryLock;

	/**
		Mutex used for locking the data cache.
	*/
	mutable QMutex m_cacheMutex;

	struct Data
	{
		QByteArray fileId;
		istd::TSmartPtr<istd::IChangeable> dataObjectPtr;
		QString tempFilePath;
	};

	typedef istd::TSmartPtr<Data> DataCachePtr;
	typedef QList<istd::TSmartPtr<Data>> DataCache;
	mutable DataCache m_dataCache;

	typedef QMap<QByteArray, ifile::IFileResourceValidator*> ValidatorMap;
	ValidatorMap m_validatorMap;

	typedef QMap<QByteArray, int> ResourceTypeToLicenseIdMap;
	ResourceTypeToLicenseIdMap m_resourceTypeToLicenseIdMap;
};


} // namespace imtrepo


