#pragma once

 
// Qt includes
#include <QtCore/QReadWriteLock>
#include <QtSql/QtSql>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/IMetaInfoCreator.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/IDatabaseObjectDelegate.h>


namespace imtdb
{


/**
	Basic implementation of a Database based model.
*/
class CSqlDatabaseObjectCollectionComp:
			virtual public imtbase::IObjectCollection,
			public ilog::CLoggerComponentBase
{
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
	I_END_COMPONENT;

	CSqlDatabaseObjectCollectionComp();

	// reimplemented (ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

	// reimplemented (IObjectCollectionInfo)
	virtual bool GetCollectionItemMetaInfo(const QByteArray & objectId, idoc::IDocumentMetaInfo & metaInfo) const override;
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual Id GetObjectTypeId(const QByteArray& objectId) const override;

	// reimplemented (imtbase::IObjectCollection)
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const QByteArray& proposedObjectId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr) override;
	virtual bool RemoveObject(const QByteArray& objectId) override;
	virtual bool SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual void SetObjectName(const QByteArray& objectId, const QString& objectName) override;
	virtual void SetObjectDescription(const QByteArray& objectId, const QString& objectDescription) override;
	virtual const imtbase::IRevisionController * GetRevisionController() const override;
	virtual const imtbase::ICollectionDataController * GetDataController() const override;
	virtual int GetOperationFlags(const QByteArray & objectId = QByteArray()) const override;
	virtual bool GetDataMetaInfo(const QByteArray & objectId, MetaInfoPtr & metaInfoPtr) const override;
	virtual const istd::IChangeable * GetObjectPtr(const QByteArray & objectId) const override;
	virtual bool GetObjectData(const QByteArray & objectId, DataPtr & dataPtr) const override;
	virtual void SetObjectEnabled(const QByteArray & objectId, bool isEnabled = true) override;
	virtual bool RegisterEventHandler(imtbase::IObjectCollectionEventHandler * eventHandler) override;
	virtual bool UnregisterEventHandler(imtbase::IObjectCollectionEventHandler * eventHandler) override;
	
protected:
	virtual bool ExecuteTransaction(const QByteArray& sqlQuery) const;
	virtual void CreateCollectionFromDatabase();

	void OnFilterParamsChanged(const istd::IChangeable::ChangeSet& changeSet, const iprm::IParamsSet* filterParamsPtr);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	I_REF(IDatabaseEngine, m_dbEngineCompPtr);

private:
	I_FACT(istd::IChangeable, m_objectFactoryCompPtr);
	I_REF(IDatabaseObjectDelegate, m_objectDelegateCompPtr);
	I_REF(imtbase::IMetaInfoCreator, m_metaInfoCreatorCompPtr);
	I_ATTR(QByteArray, m_typeIdAttrPtr);
	I_TEXTATTR(m_typeNameAttrPtr);
	I_REF(iprm::IParamsSet, m_filterParamsCompPtr);


	iprm::COptionsManager m_typesInfo;

	struct ObjectInfo
	{
		QByteArray typeId;
		imtbase::IMetaInfoCreator::MetaInfoPtr metaInfoPtr;
		imtbase::IMetaInfoCreator::MetaInfoPtr collectionMetaInfoPtr;
	};

	typedef QMap<QByteArray, ObjectInfo> ObjectInfoMap;

	ObjectInfoMap m_objectInfoMap;

	mutable QReadWriteLock m_objectInfoMapMutex;

	imtbase::TModelUpdateBinder<iprm::IParamsSet, CSqlDatabaseObjectCollectionComp> m_filterParamsObserver;
};


} // namespace imtdb


