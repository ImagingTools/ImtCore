#pragma once

 
// Qt includes
#include <QtCore/QReadWriteLock>
#include <QtSql/QtSql>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/CObjectCollection.h>
#include <imtbase/ICollectionDataController.h>
#include <imtbase/IMetaInfoCreator.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/ISqlDatabaseObjectDelegate.h>
#include <imtdb/IDatabaseLoginSettings.h>


namespace imtdb
{


/**
	Basic implementation of a Database based model.
*/
class CSqlDatabaseObjectCollectionComp:
			public QObject,
			virtual public imtbase::IObjectCollection,
			public ilog::CLoggerComponentBase
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSqlDatabaseObjectCollectionComp);
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(imtbase::IObjectCollectionInfo);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
		I_ASSIGN(m_objectFactoryCompPtr, "ObjectFactory", "List of factories used for object creation", false, "ObjectFactory");
		I_ASSIGN(m_typeIdAttrPtr, "TypeId", "Type-ID corresponding to the registered factory", true, "Default");
		I_ASSIGN(m_typeNameAttrPtr, "TypeName", "Type name corresponding to the registered factory", true, "Default");
		I_ASSIGN(m_dbEngineCompPtr, "DatabaseEngine", "Database engine used for low level SQL quering", true, "DatabaseEngine");
		I_ASSIGN(m_objectDelegateCompPtr, "ObjectDelegate", "Database object delegate used for creation of C++ objects from the SQL record", true, "ObjectDelegate");
		I_ASSIGN(m_metaInfoCreatorCompPtr, "MetaInfoCreator", "Meta-info creator", false, "MetaInfoCreator");
		I_ASSIGN(m_filterParamsCompPtr, "FilteringParams", "Parameter using for the filterering the table", false, "FilteringParams");
		I_ASSIGN(m_databaseAccessSettingsCompPtr, "DatabaseAccessSettings", "Database access settings", false, "DatabaseAccessSettings");
		I_ASSIGN(m_collectionDataControllerCompPtr, "CollectionDataController", "Data export/import controller for the collection", false, "DataController");
	I_END_COMPONENT;

	CSqlDatabaseObjectCollectionComp();

	// reimplemented (imtbase::IObjectCollection)
	virtual const imtbase::IRevisionController* GetRevisionController() const override;
	virtual const imtbase::ICollectionDataController* GetDataController() const override;
	virtual int GetOperationFlags(const QByteArray& objectId = QByteArray()) const override;
	virtual Id InsertNewBranch(
				const Id& parentId,
				const QString& name,
				const QString& description,
				const Id& proposedElementId = Id(),
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr) override;
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				DataPtr defaultValuePtr = DataPtr(),
				const QByteArray& proposedObjectId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const Id& parentId = Id()) override;
	virtual bool RemoveElement(const Id& elementId) override;
	virtual const istd::IChangeable* GetObjectPtr(const QByteArray& objectId) const override;
	virtual bool GetObjectData(const QByteArray& objectId, DataPtr& dataPtr) const override;
	virtual bool SetObjectData(const Id& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
    virtual imtbase::IObjectCollection* CreateSubCollection(int offset = 0, int count = -1, const iprm::IParamsSet* selectionParamsPtr = nullptr, const Id& parentId = Id(), int iterationFlags = IF_RECURSIVE | IF_LEAF_ONLY) const override;

	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual Id GetObjectTypeId(const QByteArray& objectId) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo(const Id& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual int GetElementsCount(
		const iprm::IParamsSet* selectionParamPtr = nullptr,
		const Id& parentId = Id(),
		int iterationFlags = IF_RECURSIVE | IF_LEAF_ONLY) const override;
	virtual Ids GetElementIds(
		int offset = 0,
		int count = -1,
		const iprm::IParamsSet* selectionParamsPtr = nullptr,
		const Id& parentId = Id(),
		int iterationFlags = IF_RECURSIVE | IF_LEAF_ONLY) const override;
	virtual bool GetSubsetInfo(
				imtbase::ICollectionInfo& subsetInfo,
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				const Id& parentId = Id(),
				int iterationFlags = IF_RECURSIVE | IF_LEAF_ONLY) const override;
	virtual Id GetParentId(const Id& elementId) const override;
	virtual Ids GetElementPath(const Id& elementId) const override;
	virtual bool IsBranch(const Id& elementId) const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId) const override;
	virtual bool SetElementName(const Id& objectId, const QString& name) override;
	virtual bool SetElementDescription(const Id& objectId, const QString& description) override;
	virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true) override;


	// reimplemented (istd::IChangeable)
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;
	
protected:
	struct ObjectInfo
	{
		QByteArray typeId;
		idoc::MetaInfoPtr metaInfoPtr;
		idoc::MetaInfoPtr collectionMetaInfoPtr;
	};

	typedef QMap<QByteArray, ObjectInfo> ObjectInfoMap;

protected:
	virtual bool ExecuteTransaction(const QByteArray& sqlQuery) const;
	QSqlRecord GetObjectRecord(const QByteArray& objectId) const;

	void OnFilterParamsChanged(const istd::IChangeable::ChangeSet& changeSet, const iprm::IParamsSet* filterParamsPtr);
	void OnDatabaseAccessChanged(const istd::IChangeable::ChangeSet& changeSet, const imtdb::IDatabaseLoginSettings* databaseAccessSettingsPtr);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	I_REF(IDatabaseEngine, m_dbEngineCompPtr);

private:
	I_FACT(istd::IChangeable, m_objectFactoryCompPtr);
	I_REF(ISqlDatabaseObjectDelegate, m_objectDelegateCompPtr);
	I_REF(imtbase::IMetaInfoCreator, m_metaInfoCreatorCompPtr);
	I_ATTR(QByteArray, m_typeIdAttrPtr);
	I_TEXTATTR(m_typeNameAttrPtr);
	I_REF(iprm::IParamsSet, m_filterParamsCompPtr);
	I_REF(imtdb::IDatabaseLoginSettings, m_databaseAccessSettingsCompPtr);
	I_REF(imtbase::ICollectionDataController, m_collectionDataControllerCompPtr);

	iprm::COptionsManager m_typesInfo;

	imtbase::TModelUpdateBinder<iprm::IParamsSet, CSqlDatabaseObjectCollectionComp> m_filterParamsObserver;
	imtbase::TModelUpdateBinder<imtdb::IDatabaseLoginSettings, CSqlDatabaseObjectCollectionComp> m_databaseAccessObserver;

	bool m_isInitialized;
};


} // namespace imtdb


