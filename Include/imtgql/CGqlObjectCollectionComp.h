#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <ilog/TLoggerCompWrap.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtgql/IClientProtocolEngine.h>
#include <imtgql/IGqlObjectCollectionDelegate.h>
#include <imtgql/IGqlSubscriptionManager.h>
#include <imtgql/IGqlClient.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtbase/ICollectionDataController.h>
#include <imtbase/IObjectCollection.h>


namespace imtbase
{
	class IRevisionController;
}


namespace imtgql
{


class CGqlObjectCollectionComp:
			public QObject,
			public ilog::TLoggerCompWrap<icomp::CComponentBase>,
			virtual public imtbase::IObjectCollection,
			virtual public imtbase::ICollectionDataController,
			virtual public IGqlSubscriptionClient
{
	Q_OBJECT
public:
	typedef ilog::TLoggerCompWrap<icomp::CComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CGqlObjectCollectionComp);
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(IObjectCollectionInfo);
		I_REGISTER_INTERFACE(ICollectionInfo);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_gqlClientCompPtr, "ApiClient", "GraphQL API client", true, "ApiClient");
		I_ASSIGN(m_subscriptionManagerCompPtr, "SubscriptionManager", "GraphQL subscription manager", true, "SubscriptionManager");
		I_ASSIGN_MULTI_0(m_gqlDatabaseDelegatesCompPtr, "GqlDatabaseDelegates", "GraphQL-based document delegate for database CRUD oeprations", true);
		I_ASSIGN_MULTI_0(m_typeIdsAttrPtr, "TypeIds", "List of type-ID corresponding to the registered factories", false);
		I_ASSIGN_MULTI_0(m_typeNamesAttrPtr, "TypeNames", "List of type names corresponding to the registered factories", false);
		I_ASSIGN_MULTI_0(m_objectFactoriesCompPtr, "ObjectFactories", "List of factories used for object creation", false);
		I_ASSIGN_MULTI_0(m_objectPersistenceListCompPtr, "ObjectPersistenceList", "List of persistence components used for object data persistence", false);
	I_END_COMPONENT;

	CGqlObjectCollectionComp();

	// reimplemented (IObjectCollection)
	virtual const imtbase::IRevisionController* GetRevisionController() const override;
	virtual const imtbase::ICollectionDataController* GetDataController() const override;
	virtual int GetOperationFlags(const Id& elementId = Id()) const ;
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				DataPtr defaultValuePtr = nullptr,
				const QByteArray& proposedObjectId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveElement(const Id& elementId, const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual const istd::IChangeable* GetObjectPtr(const Id& objectId) const override;
	virtual bool GetObjectData(const Id& objectId, DataPtr& dataPtr) const override;
	virtual bool SetObjectData(const Id& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS, const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual imtbase::IObjectCollection* CreateSubCollection(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtbase::IObjectCollectionIterator* CreateObjectCollectionIterator(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;

	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual QByteArray GetObjectTypeId(const Id& objectId) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo(const Id& objectId) const override;

	// reimplemented (ICollectionInfo)
	virtual int GetElementsCount(const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual bool GetSubsetInfo(
				ICollectionInfo& subsetInfo,
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual QVariant GetElementInfo(const Id& elementId, int infoType) const override;
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId) const override;
	virtual bool SetElementName(const Id& elementId, const QString& name) override;
	virtual bool SetElementDescription(const Id& elementId, const QString& description) override;
	virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true) override;

	// reimplemented (imtbase::ICollectionDataController)
	virtual const ifile::IFilePersistence* GetPersistenceForObjectType(const QByteArray& typeId) const override;
	virtual bool ExportFile(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& targetFilePath = QString()) const override;
	virtual QByteArray ImportFile(
				imtbase::IObjectCollection& collection,
				const QByteArray& typeId,
				const QString& sourceFilePath = QString()) const override;

	// reimplemented (imtgql::IGqlSubscriptionClient)
	virtual void OnResponseReceived(const QByteArray& subscriptionId, const QByteArray& subscriptionData);
	virtual void OnSubscriptionStatusChanged(const QByteArray& subscriptionId, const SubscriptionStatus& status, const QString& message);

protected:
	DataPtr CreateObjectInstance(const QByteArray& typeId) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	QString GetDocumentExtension(const QByteArray& typeId) const;
	IGqlObjectCollectionDelegate* GetDelegateForType(const QByteArray& typeId) const;

protected:
	IObjectCollection::DataPtr GetDocument(const QByteArray& typeId, const QByteArray& documentId) const;

protected:
	I_REF(imtgql::IGqlClient, m_gqlClientCompPtr);
	I_REF(imtgql::IGqlSubscriptionManager, m_subscriptionManagerCompPtr);
	I_MULTIREF(IGqlObjectCollectionDelegate, m_gqlDatabaseDelegatesCompPtr);
	I_MULTIATTR(QByteArray, m_typeIdsAttrPtr);
	I_MULTITEXTATTR(m_typeNamesAttrPtr);
	I_MULTIFACT(istd::IChangeable, m_objectFactoriesCompPtr);
	I_MULTIREF(ifile::IFilePersistence, m_objectPersistenceListCompPtr);

	typedef QMap<QByteArray, IGqlObjectCollectionDelegate*> DelegatesMap;
	DelegatesMap m_delegatesMap;

	iprm::COptionsManager m_typesInfo;

	struct Item
	{
		QByteArray orgId;
		QByteArray parentId;
		bool isBranch;
		QString name;
		int version;
	};
	mutable QMap<QByteArray, Item> m_items;
	QByteArray m_addMeasurementSubsriptionId;
};


} //namespace imtgql


