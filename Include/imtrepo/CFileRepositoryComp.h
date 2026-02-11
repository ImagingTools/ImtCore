// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QReadWriteLock>
#include <QtCore/QFileInfo>

// ACF includes
#include <istd/TSmartPtr.h>
#include <iser/ISerializable.h>
#include <imod/TModelWrap.h>
#include <ilog/TLoggerCompWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <ifile/IFileNameParam.h>
#include <ifile/IFilePersistence.h>
#include <ifile/IFileResourceTypeConstraints.h>
#include <iauth/IRightsProvider.h>

// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>
#include <imtbase/IRevisionController.h>
#include <imtbase/ICollectionDataController.h>
#include <imtbase/CObjectCollectionMetaInfo.h>
#include <imtrepo/IFileCollectionInfo.h>
#include <imtrepo/IFileObjectCollection.h>
#include <imtrepo/CFileCollectionItem.h>


namespace imtrepo
{


/**
	Base class of the file collection. General implementation
*/
class CFileRepositoryComp:
			public ilog::CLoggerComponentBase,
			virtual public IFileObjectCollection,
			virtual public imtbase::IObjectCollection::IDataFactory
{
	Q_DECLARE_TR_FUNCTIONS(CFileRepositoryComp);

public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFileRepositoryComp)
		I_REGISTER_INTERFACE(IFileObjectCollection);
		I_REGISTER_INTERFACE(IFileCollectionInfo);
		I_REGISTER_INTERFACE(IObjectCollection);
		I_ASSIGN(m_repositoryPathCompPtr, "RepositoryPath", "Path to the file collection folder", false, "RepositoryPath");
		I_ASSIGN(m_resourceTypesCompPtr, "ResourceTypes", "List of supported resource types", false, "ResourceTypes");
		I_ASSIGN(m_revisionAttrPtr, "RepositoryRevision", "Revision number of the whole repository", true, 0);
		I_ASSIGN(m_isCalculateCheckSumAttrPtr, "IsCalculateCheckSum", "Calculate check sums for the file", true, true);
		I_ASSIGN_MULTI_0(m_objectPersistenceListCompPtr, "ObjectPersistenceList", "List of persistence components used for data object loading", false);
		I_ASSIGN_MULTI_0(m_resourceFileTypesCompPtr, "FileTypeInfos", "List of file type infos for corresponding resource type", false);
		I_ASSIGN_MULTI_0(m_metaInfoCreatorListCompPtr, "MetaInfoCreatorList", "List of meta-info creators related to the object types", false);
		I_ASSIGN_MULTI_0(m_objectFactoryListCompPtr, "ObjectFactoryList", "List of factories used for data object instance creation", false);
		I_ASSIGN(m_documentInfoCollectionCompPtr, "DocumentInfoCollection", "Collection's table of contents", true, "DocumentInfoCollection");
		I_ASSIGN(m_infoItemTypeIdAttrPtr, "InfoTypeId", "Type-ID of the file info item", true, "FileItemInfo");
	I_END_COMPONENT;

public:
	CFileRepositoryComp();

	// reimplemented (IFileCollectionInfo)
	virtual const ifile::IFileResourceTypeConstraints* GetFileTypeConstraints() const override;
	virtual bool GetFileInfo(const QByteArray& objectId, IFileCollectionItem& item) const override;
	virtual QString GetCollectionRootFolder() const override;
	virtual int GetRepositoryRevision() const override;
	virtual FileCollectionLayout GetCollectionFileLayout() const override;

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

	// reimplemented (IObjectCollection)
	virtual const imtbase::IRevisionController* GetRevisionController() const override;
	virtual const imtbase::ICollectionDataController* GetDataController() const override;
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const QByteArray& proposedObjectId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveElements(const Ids& elementIds, const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveElementSet(
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RestoreObjects(
				const Ids& objectIds,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RestoreObjectSet(
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual const istd::IChangeable* GetObjectPtr(const QByteArray& objectId) const override;
	virtual bool GetObjectData(const QByteArray& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override;
	virtual bool SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS, const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual imtbase::IObjectCollectionUniquePtr CreateSubCollection(int offset = 0, int count = -1, const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtbase::IObjectCollectionIterator* CreateObjectCollectionIterator(
				const QByteArray& objectId = QByteArray(),
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
	virtual istd::IChangeableUniquePtr CreateInstance(const QByteArray& keyId = "") const override;
	virtual istd::IFactoryInfo::KeyList GetFactoryKeys() const override;

protected:
	class ResourceTypeConstraints: public ifile::IFileResourceTypeConstraints
	{
	public:
		ResourceTypeConstraints();
		void SetParent(CFileRepositoryComp* parentPtr);

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
		CFileRepositoryComp* m_parentPtr;
	};

	typedef imod::TModelWrap<ResourceTypeConstraints> ResourceTypeConstraintsModel;

protected:
	const ifile::IFilePersistence* GetObjectPersistence(const QByteArray& typeId) const;

	QString CreateWorkingDir() const;
	QString GetWorkingExt(
				const ifile::IFilePersistence* persistencePtr,
				const istd::IChangeable* dataObjectPtr,
				const QString& fileName);

	/**
		Create data object for the given type.
	*/
	virtual istd::IChangeableUniquePtr CreateDataObject(const QByteArray& typeId) const;

	/**
		Create object data from the file.
	*/
	virtual istd::IChangeableUniquePtr CreateObjectFromFile(const QString& filePath, const QByteArray& typeId) const;

	/**
		Check if the path to the file is inside of repository folder.
	*/
	bool IsPathInsideRepository(const QString& filePath) const;

	/**
		Create meta-info object for a given file in the file collection.
		\return The instance of the meta-information object, or \c NULL of no meta-info was created for the given file.
	*/
	virtual idoc::MetaInfoPtr CreateItemMetaInfo(const QString& dataObjectFilePath, const QByteArray& typeId) const;

	/**
		Calculate path in file collection for the local file path
	*/
	virtual QString CalculateFolderPathInRepository(
				const QString& localFilePath,
				const QString& resourceName,
				const QByteArray& typeId,
				ilog::IMessageConsumer* messageConsumerPtr) const;
	virtual QString CalculateTargetFilePath(
				const QString& filePath,
				const QString& objectName,
				const QByteArray& typeId) const;
	virtual QString CalculateTargetFilePath(
				const QString& targetFolderPath,
				const QString& localFilePath) const;
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
		Whole repository revision.
	*/
	I_ATTR(int, m_revisionAttrPtr);

	/**
		Calculate checkSum for files
	*/
	I_ATTR(bool, m_isCalculateCheckSumAttrPtr);

	/**
		Supported types of the objects in the collection.
	*/
	ResourceTypeConstraintsModel m_resourceTypeConstraints;

	/**
		Path to the directory where the file collection is located.
		If the path is set, the incomming file will be copied to this location,
		otherwise only a link to the file wil be created.
	*/
	I_REF(ifile::IFileNameParam, m_repositoryPathCompPtr);

	/**
		List of supported resource types.
	*/
	I_REF(iprm::IOptionsList, m_resourceTypesCompPtr);

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
		List of data object factories related to registered resource types.
	*/
	I_MULTIFACT(istd::IChangeable, m_objectFactoryListCompPtr);

	/**
		Repository's table of contents.
	*/
	I_REF(imtbase::IObjectCollection, m_documentInfoCollectionCompPtr);

	/**
		ID of the file item info type.
	*/
	I_ATTR(QByteArray, m_infoItemTypeIdAttrPtr);

private:
	typedef QMap<QByteArray, DataPtr> ObjectCache;
	mutable ObjectCache m_objectCache;
	mutable QReadWriteLock m_objectCacheLock;

	typedef QMap<QByteArray, const imtbase::IMetaInfoCreator*> MetaInfoCreatorMap;
	MetaInfoCreatorMap m_metaInfoCreatorMap;
};


} // namespace imtrepo


