#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QDateTime>
#include <QtCore/QThread>
#include <QtCore/QReadWriteLock>
#include <QtCore/QFileInfo>

// ACF includes
#include <istd/TSmartPtr.h>
#include <iser/ISerializable.h>
#include <imod/TModelWrap.h>
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>
#include <ifile/IFilePersistence.h>
#include <ifile/IFileResourceTypeConstraints.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IFileObjectCollection.h>
#include <imtbase/IFileCollectionInfo.h>


namespace imtbase
{


/**
	Implementation of a file-based data object collection. Each object will be placed in the file system according to the configured settings.
	Two modes are supported:
	* Plain. In this mode the data file and related description files will be simply placed into the repository folder.
	* Subfolder. Each file will be placed into a separate folder. This folder contains the data file itself, repository description file, and the meta-info file.
	\ingroup Collection
*/
class CFileCollectionComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public IFileObjectCollection,
			virtual public IFileCollectionInfo
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFileCollectionComp)
		I_REGISTER_INTERFACE(IFileObjectCollection);
		I_REGISTER_INTERFACE(IFileCollectionInfo);
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(IObjectCollectionInfo);
		I_REGISTER_INTERFACE(ICollectionInfo);
		I_ASSIGN(m_repositoryPathCompPtr, "RepositoryPath", "Path to the repository folder", false, "RepositoryPath");
		I_ASSIGN(m_isCalculateCheckSumAttrPtr, "IsCalculateCheckSum", "Calculate check sums for the file", true, true);
		I_ASSIGN(m_resourceTypesCompPtr, "ResourceTypes", "List of supported resource types", false, "ResourceTypes");
		I_ASSIGN_MULTI_0(m_resourceFileTypesCompPtr, "FileTypeInfos", "List of file type infos for corresponding resource type", false);
		I_ASSIGN(m_useSubfolderAttrPtr, "UseSubfolder", "If set, for each input file a subfolder with the corresponding file name will be created", true, false);
		I_ASSIGN_MULTI_0(m_objectPersistenceListCompPtr, "ObjectPersistenceList", "List of persistence components used for data object loading", false);
		I_ASSIGN_MULTI_0(m_objectFactoryListCompPtr, "ObjectFactoryList", "List of factories used for data ojbect instance creation", false);
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", true, "VersionInfo");
		I_ASSIGN(m_createFolderOnStartAttrPtr, "CreateRepositoryFolder", "Ensure that the repository folder is created on the start", true, true);
	I_END_COMPONENT;

	CFileCollectionComp();

	// reimplemented (IFileObjectCollection)
	virtual const ifile::IFileResourceTypeConstraints* GetResourceTypeConstraints() const override;
	virtual ifile::IFileMetaInfoProvider::MetaInfoPtr GetFileMetaInfo(const QByteArray& objectId) const;
	virtual FileInfo GetFileInfo(const QByteArray& resourceId) const override;
	virtual QString GetFile(
				const QByteArray& resourceId,
				const QString& targetFilePath) const override;
	virtual QByteArray AddFile(
				const QString& localFilePath,
				const QByteArray& resourceTypeId = QByteArray(),
				const QString& resourceName = QString(),
				const QString& resourceDescription = QString(),
				const QByteArray& resourceId = QByteArray()) override;
	virtual bool UpdateFile(
				const QString& localFilePath,
				const QByteArray& resourceId) override;

	// reimplemented (IFileCollectionInfo)
	virtual QString GetRepositoryPath() const;

	// reimplemented (IObjectCollection)
	virtual int GetOperationFlags(const QByteArray & objectId = QByteArray()) const override;
	virtual QByteArray InsertNewObject(const QByteArray & typeId, const QString & name, const QString & description, const istd::IChangeable * defaultValuePtr = nullptr) override;
	virtual bool RemoveObject(const QByteArray & objectId) override;
	virtual const istd::IChangeable* GetObjectPtr(const QByteArray& objectId) const override;
	virtual bool GetObjectData( const QByteArray& objectId, DataPtr& dataPtr) const override;
	virtual bool SetObjectData( const QByteArray& objectId, const istd::IChangeable& object) override;
	virtual void SetObjectName(const QByteArray & objectId, const QString & objectName) override;
	virtual void SetObjectDescription(const QByteArray & objectId, const QString & objectDescription) override;
	virtual void SetObjectEnabled(const QByteArray & objectId, bool isEnabled = true) override;

	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual Id GetObjectTypeId(const QByteArray& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS);

protected:
	/**
		Internal structure representing the file item in the repository.
	*/
	class CollectionItem: virtual public iser::ISerializable
	{
	public:
		CollectionItem(const CFileCollectionComp& parent)
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
		ifile::IFileMetaInfoProvider::MetaInfoPtr metaInfoPtr;

		const CFileCollectionComp* m_parent;
	};

	static QString GetInfoFileExtention();
	static QString GetDataFileExtention();

	/**
		Create data object for the given resource type.
	*/
	virtual istd::IChangeable* CreateDataObject(const QByteArray& resourceTypeId) const;

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
	int GetCollectionItemById(const QByteArray& fileId, CollectionItem& item) const;

	/**
		Get the index of the file by file (resource) name.
		If the file was not found, the method will return a negative number.
	*/
	int GetFileIndexByName(const QString& fileName) const;

	/**
		Create object data from the file.
	*/
	virtual istd::IChangeable* CreateObjectFromFile(const QString& filePath, const QByteArray& resourceTypeId) const;

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
	void UpdateItemMetaInfo(CollectionItem& item);

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

		void SetParent(CFileCollectionComp* parentPtr);

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
		CFileCollectionComp* m_parentPtr;
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
	QString SaveCollectionItem(const CollectionItem& repositoryItem) const;

	/**
		Read repository contents.
	*/
	void ReadCollectionItems();

	QString GetTempDirectory() const;
	QString GetDataItemFilePath(const CollectionItem& repositoryFile) const;
	QString GetMetaInfoFilePath(const CollectionItem& repositoryFile) const;
	QString ShortenWindowsFilename(const QString& fileName, const QFileInfo& fileInfo, const QString& prefix) const;

private:
	typedef QList<CollectionItem> Files;

	/**
		Collection data.
	*/
	mutable Files m_files;

	/**
		Supported types of the objects in the collection.
	*/
	ResourceTypeConstraintsModel m_resourceTypeConstraints;

	/**
		Mutex used for locking the collectio items.
	*/
	mutable QReadWriteLock m_repositoryLock;

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
};


} // namespace imtbase


