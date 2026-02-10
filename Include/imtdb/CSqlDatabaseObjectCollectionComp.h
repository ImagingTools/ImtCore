// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


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
#include <imtbase/TModelUpdateBinder.h>
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/ISqlDatabaseObjectDelegate.h>
#include <imtdb/IDatabaseLoginSettings.h>


namespace imtdb
{


/**
	Component implementation of a SQL-database collection.
*/
class CSqlDatabaseObjectCollectionComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public imtdb::ISqlDatabaseObjectCollection
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSqlDatabaseObjectCollectionComp);
		I_REGISTER_INTERFACE(imtdb::ISqlDatabaseObjectCollection);
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(imtbase::IObjectCollectionInfo);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
		I_ASSIGN(m_objectCollectionFactoryCompPtr, "ObjectCollectionFactory", "Factory used for object collection creation", false, "ObjectCollectionFactory");
		I_ASSIGN(m_dbEngineCompPtr, "DatabaseEngine", "Database engine used for low level SQL quering", true, "DatabaseEngine");
		I_ASSIGN(m_objectDelegateCompPtr, "ObjectDelegate", "Database object delegate used for creation of C++ objects from the SQL record", true, "ObjectDelegate");
		I_ASSIGN(m_metaInfoCreatorCompPtr, "MetaInfoCreator", "Meta-info creator", false, "MetaInfoCreator");
		I_ASSIGN(m_databaseAccessSettingsCompPtr, "DatabaseAccessSettings", "Database access settings", false, "DatabaseAccessSettings");
		I_ASSIGN(m_collectionDataControllerCompPtr, "CollectionDataController", "Data export/import controller for the collection", false, "DataController");
	I_END_COMPONENT;

	CSqlDatabaseObjectCollectionComp();

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
	QSqlRecord GetObjectRecord(const QByteArray& objectId) const;
	void OnDatabaseAccessChanged(
				const istd::IChangeable::ChangeSet& changeSet,
				const imtdb::IDatabaseLoginSettings* databaseAccessSettingsPtr);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;
	
private:
	void AddOperationContextToChangeSet(const imtbase::IOperationContext& operationContext, istd::IChangeable::ChangeSet& changeSet) const;

protected:
	I_REF(IDatabaseEngine, m_dbEngineCompPtr);

private:
	I_FACT(imtbase::IObjectCollection, m_objectCollectionFactoryCompPtr);
	I_REF(ISqlDatabaseObjectDelegate, m_objectDelegateCompPtr);
	I_REF(imtbase::IMetaInfoCreator, m_metaInfoCreatorCompPtr);
	I_REF(imtdb::IDatabaseLoginSettings, m_databaseAccessSettingsCompPtr);
	I_REF(imtbase::ICollectionDataController, m_collectionDataControllerCompPtr);

	imtbase::TModelUpdateBinder<imtdb::IDatabaseLoginSettings, CSqlDatabaseObjectCollectionComp> m_databaseAccessObserver;

	bool m_isInitialized;

	mutable QRecursiveMutex m_transactionDisableCountersMutex;
	QMap<Qt::HANDLE, int> m_transactionDisableCounters;
};


} // namespace imtdb


