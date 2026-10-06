// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// std includes
#include <map>
#include <memory>

// Qt includes
#include <QtCore/QReadWriteLock>
#include <QtSql/QtSql>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/COptionsManager.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtdb/ISqlDatabaseObjectCollection.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/ICollectionDataController.h>
#include <imtbase/IMetaInfoCreator.h>
#include <imtbase/ITenantCollectionProvider.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/ISqlDatabaseObjectDelegate.h>
#include <imtdb/IDatabaseLoginSettings.h>


namespace imtdb
{


/**
	Component implementation of a SQL-database collection.
	The component addresses the data without organization; the data of a tenant is accessed
	via the collection bound to the tenant (see imtbase::ITenantCollectionProvider).
*/
class CSqlDatabaseObjectCollectionComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public imtdb::ISqlDatabaseObjectCollection,
			virtual public imtbase::ITenantCollectionProvider
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSqlDatabaseObjectCollectionComp);
		I_REGISTER_INTERFACE(imtdb::ISqlDatabaseObjectCollection);
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(imtbase::IObjectCollectionInfo);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
		I_REGISTER_INTERFACE(imtbase::ITenantCollectionProvider);
		I_ASSIGN(m_objectCollectionFactoryCompPtr, "ObjectCollectionFactory", "Factory used for object collection creation", false, "ObjectCollectionFactory");
		I_ASSIGN(m_dbEngineCompPtr, "DatabaseEngine", "Database engine used for low level SQL quering", true, "DatabaseEngine");
		I_ASSIGN(m_objectDelegateCompPtr, "ObjectDelegate", "Database object delegate used for creation of C++ objects from the SQL record", true, "ObjectDelegate");
		I_ASSIGN(m_metaInfoCreatorCompPtr, "MetaInfoCreator", "Meta-info creator", false, "MetaInfoCreator");
		I_ASSIGN(m_databaseAccessSettingsCompPtr, "DatabaseAccessSettings", "Database access settings", false, "DatabaseAccessSettings");
		I_ASSIGN(m_collectionDataControllerCompPtr, "CollectionDataController", "Data export/import controller for the collection", false, "DataController");
		I_ASSIGN(m_maxLengthCommentAttrPtr, "MaxLengthComment", "Maximum length of the document comment", true, 1000);
	I_END_COMPONENT;

	CSqlDatabaseObjectCollectionComp();

	// reimplemented (imtbase::ITenantCollectionProvider)
	virtual bool IsTenantSeparated() const override;
	virtual imtbase::ITenantObjectCollection* GetTenantCollection(const QByteArray& tenantId) const override;

	// reimplemented (ISqlDatabaseObjectCollection)
	virtual QByteArray GetDatabaseId() const override;
	virtual bool AreInternalTransactionsEnabled() const override;
	virtual bool SetInternalTransactionsEnabled(bool isEnabled) override;

	// reimplemented (imtbase::IObjectCollection)
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
	virtual bool SetObjectData(
				const Id& objectId,
				const istd::IChangeable& object,
				CompatibilityMode mode = CM_WITHOUT_REFS,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual imtbase::IObjectCollectionUniquePtr CreateSubCollection(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
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

	// reimplemented (istd::IChangeable)
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	virtual bool IsObjectTypeSupported(const QByteArray& typeId) const;
	virtual bool ExecuteTransaction(const QByteArray& sqlQuery) const;
	virtual bool ExecuteTransaction(const QByteArray& sqlQuery, const QVariantMap& bindValues) const;

	/**
		Operations of the collection on the data storage of the given tenant (empty for the data without organization).
		\param collection Collection the operation is called on; passed to the object delegate and notified about the changes.
	*/
	QByteArray DoInsertNewObject(
				imtbase::IObjectCollection& collection,
				const QByteArray& tenantId,
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr,
				const QByteArray& proposedObjectId,
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr,
				const imtbase::IOperationContext* operationContextPtr);
	bool DoRemoveElements(
				imtbase::IObjectCollection& collection,
				const QByteArray& tenantId,
				const Ids& elementIds,
				const imtbase::IOperationContext* operationContextPtr);
	bool DoRemoveElementSet(
				imtbase::IObjectCollection& collection,
				const QByteArray& tenantId,
				const iprm::IParamsSet* selectionParamsPtr,
				const imtbase::IOperationContext* operationContextPtr);
	bool DoRestoreObjects(
				imtbase::IObjectCollection& collection,
				const QByteArray& tenantId,
				const Ids& objectIds,
				const imtbase::IOperationContext* operationContextPtr);
	bool DoRestoreObjectSet(
				imtbase::IObjectCollection& collection,
				const QByteArray& tenantId,
				const iprm::IParamsSet* selectionParamsPtr,
				const imtbase::IOperationContext* operationContextPtr);
	bool DoGetObjectData(const QByteArray& tenantId, const QByteArray& objectId, DataPtr& dataPtr, const iprm::IParamsSet* dataConfigurationPtr) const;
	bool DoSetObjectData(
				imtbase::IObjectCollection& collection,
				const QByteArray& tenantId,
				const Id& objectId,
				const istd::IChangeable& object,
				const imtbase::IOperationContext* operationContextPtr);
	imtbase::IObjectCollectionUniquePtr DoCreateSubCollection(const QByteArray& tenantId, int offset, int count, const iprm::IParamsSet* selectionParamsPtr) const;
	imtbase::IObjectCollectionIterator* DoCreateObjectCollectionIterator(
				const QByteArray& tenantId,
				const QByteArray& objectId,
				int offset,
				int count,
				const iprm::IParamsSet* selectionParamsPtr) const;
	Id DoGetObjectTypeId(const QByteArray& tenantId, const QByteArray& objectId) const;
	idoc::MetaInfoPtr DoGetDataMetaInfo(const QByteArray& tenantId, const Id& objectId) const;
	int DoGetElementsCount(const QByteArray& tenantId, const iprm::IParamsSet* selectionParamPtr) const;
	Ids DoGetElementIds(const QByteArray& tenantId, int offset, int count, const iprm::IParamsSet* selectionParamsPtr) const;
	QVariant DoGetElementInfo(const QByteArray& tenantId, const QByteArray& elementId, int infoType) const;
	idoc::MetaInfoPtr DoGetElementMetaInfo(const QByteArray& tenantId, const Id& elementId) const;
	bool DoSetElementName(imtbase::IObjectCollection& collection, const QByteArray& tenantId, const Id& elementId, const QString& name);
	bool DoSetElementDescription(imtbase::IObjectCollection& collection, const QByteArray& tenantId, const Id& elementId, const QString& description);
	bool DoResetData(imtbase::IObjectCollection& collection, const QByteArray& tenantId);

	QSqlRecord GetObjectRecord(const QByteArray& tenantId, const QByteArray& objectId) const;

	/**
		Address the data storage of the tenant in a query built by the object delegate.
		\return \c false if the storage cannot be resolved or the operation context belongs to another tenant.
	*/
	bool ApplyTenant(QByteArray& query, const QByteArray& tenantId, const imtbase::IOperationContext* operationContextPtr = nullptr) const;

	void OnDatabaseAccessChanged(
				const istd::IChangeable::ChangeSet& changeSet,
				const imtdb::IDatabaseLoginSettings* databaseAccessSettingsPtr);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void AddOperationContextToChangeSet(const imtbase::IOperationContext& operationContext, istd::IChangeable::ChangeSet& changeSet) const;

	/**
		View of the collection bound to the data storage of a single tenant.
	*/
	class CTenantCollection:
				virtual public imtbase::ITenantObjectCollection,
				virtual public imtdb::ISqlDatabaseObjectCollection
	{
	public:
		CTenantCollection(CSqlDatabaseObjectCollectionComp& parent, const QByteArray& tenantId);

		// reimplemented (imtbase::ITenantObjectCollection)
		virtual QByteArray GetTenantId() const override;

		// reimplemented (ISqlDatabaseObjectCollection)
		virtual QByteArray GetDatabaseId() const override;
		virtual bool AreInternalTransactionsEnabled() const override;
		virtual bool SetInternalTransactionsEnabled(bool isEnabled) override;

		// reimplemented (imtbase::IObjectCollection)
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
		virtual bool SetObjectData(
					const Id& objectId,
					const istd::IChangeable& object,
					CompatibilityMode mode = CM_WITHOUT_REFS,
					const imtbase::IOperationContext* operationContextPtr = nullptr) override;
		virtual imtbase::IObjectCollectionUniquePtr CreateSubCollection(
					int offset = 0,
					int count = -1,
					const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
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

		// reimplemented (istd::IChangeable)
		virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;
		virtual void BeginChanges(const ChangeSet& changeSet) override;
		virtual void EndChanges(const ChangeSet& changeSet) override;
		virtual void BeginChangeGroup(const ChangeSet& changeSet) override;
		virtual void EndChangeGroup(const ChangeSet& changeSet) override;

	private:
		ChangeSet CreateTenantChangeSet(const ChangeSet& changeSet) const;

		CSqlDatabaseObjectCollectionComp& m_parent;
		QByteArray m_tenantId;
	};

protected:
	I_REF(IDatabaseEngine, m_dbEngineCompPtr);

private:
	I_FACT(imtbase::IObjectCollection, m_objectCollectionFactoryCompPtr);
	I_REF(ISqlDatabaseObjectDelegate, m_objectDelegateCompPtr);
	I_REF(imtbase::IMetaInfoCreator, m_metaInfoCreatorCompPtr);
	I_REF(imtdb::IDatabaseLoginSettings, m_databaseAccessSettingsCompPtr);
	I_REF(imtbase::ICollectionDataController, m_collectionDataControllerCompPtr);
	I_ATTR(int, m_maxLengthCommentAttrPtr);

	imtbase::TModelUpdateBinder<imtdb::IDatabaseLoginSettings, CSqlDatabaseObjectCollectionComp> m_databaseAccessObserver;

	bool m_isInitialized;

	mutable QRecursiveMutex m_transactionDisableCountersMutex;
	QMap<Qt::HANDLE, int> m_transactionDisableCounters;

	mutable QMutex m_tenantCollectionsMutex;
	mutable std::map<QByteArray, std::unique_ptr<CTenantCollection>> m_tenantCollections;
};


} // namespace imtdb


