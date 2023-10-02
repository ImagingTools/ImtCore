#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <ilog/TLoggerCompWrap.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtclientgql/IClientProtocolEngine.h>
#include <imtclientgql/IGqlObjectCollectionDelegate.h>
#include <imtclientgql/IGqlSubscriptionManager.h>
#include <imtclientgql/IGqlClient.h>
#include <imtgql/IGqlStructuredCollectionResponse.h>
#include <imtbase/CCollectionInfo.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtbase/ICollectionDataController.h>
#include <imtbase/IObjectCollection.h>

#include <imtbase/TICollectionStructure.h>
#include <imtbase/TIStructuredCollectionInfo.h>
#include <imtbase/IStructuredCollectionInserter.h>


namespace imtbase
{
	class IRevisionController;
}


namespace imtclientgql
{


class CGqlObjectCollectionComp:
			public QObject,
			public ilog::TLoggerCompWrap<icomp::CComponentBase>,
			virtual public imtbase::IObjectCollection,
			virtual public imtbase::IObjectCollectionStructure,
			virtual public imtbase::IStructuredObjectCollectionInfo,
			virtual public imtbase::IStructuredCollectionInserter,
			virtual public imtbase::IStructuredCollectionFinder,
			virtual public IGqlSubscriptionClient
{
	Q_OBJECT
public:
	typedef ilog::TLoggerCompWrap<icomp::CComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CGqlObjectCollectionComp);
		I_REGISTER_INTERFACE(imtbase::IObjectCollectionStructure);
		I_REGISTER_INTERFACE(imtbase::IObjectCollectionStructureInfo);
		I_REGISTER_INTERFACE(imtbase::IStructuredCollectionInserter);
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(IObjectCollectionInfo);
		I_REGISTER_INTERFACE(ICollectionInfo);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_subscriptionManagerCompPtr, "SubscriptionManager", "GraphQL subscription manager", true, "SubscriptionManager");
		I_ASSIGN(m_delegateCompPtr, "GqlDatabaseDelegate", "GraphQL-based document delegate for database CRUD oeprations", true, "GqlObjectCollectionDelegate");
		I_ASSIGN_MULTI_0(m_typeIdsAttrPtr, "TypeIds", "List of type-ID corresponding to the registered factories", false);
		I_ASSIGN_MULTI_0(m_typeNamesAttrPtr, "TypeNames", "List of type names corresponding to the registered factories", false);
		I_ASSIGN_MULTI_0(m_objectFactoriesCompPtr, "ObjectFactories", "List of factories used for object creation", false);
	I_END_COMPONENT;

	CGqlObjectCollectionComp();

	// reimplemented (imtbase::IStructuredCollectionFinder)
	virtual QByteArrayList FindObjectParentNodes(const QByteArray& objectId) const override;

	// reimpolemented (imtbase::IStructuredCollectionInserter)
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				IObjectCollection::DataPtr defaultValuePtr = IObjectCollection::DataPtr(),
				const Id& proposedObjectId = Id(),
				const Id& nodeId = Id(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;

	// reimpolemented (imtbase::TIStructuredCollectionInfo)
	virtual ElementType GetElementType(const QByteArray& elementId) const override;
	virtual QByteArrayList GetElementBasePath(const QByteArray& elementId) const override;

	// reimpolemented (imtbase::TICollectionStructure)
	virtual QByteArray InsertNewNode(
				const QString& name,
				const QString& description = QString(),
				const QByteArray& proposedNodeId = QByteArray(),
				const QByteArray& parentNodeId = QByteArray(),
				const idoc::IDocumentMetaInfo* metaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool SetNodeName(
				const QByteArray& nodeId,
				const QString& name,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool SetNodeDescription(
				const QByteArray& nodeId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool SetNodeMetaInfo(
				const QByteArray& nodeId,
				const idoc::IDocumentMetaInfo& metaInfo,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool MoveNode(
				const QByteArray& nodeId,
				const QByteArray& parentNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveNode(
				const QByteArray& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;

	virtual bool AddObjectToNode(
				const QByteArray& objectId,
				const QByteArray& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool MoveObjectToNode(
				const QByteArray& objectId,
				const QByteArray& fromNodeId,
				const QByteArray& toNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveObjectFromNode(
				const QByteArray& objectId,
				const QByteArray& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;

	// reimpolemented (imtbase::IObjectCollectionStructureInfo)
	virtual QByteArrayList GetNodePath(const QByteArray& nodeId) const override;
	virtual QSharedPointer<imtbase::IStructuredObjectCollectionInfo> GetNodeContent(const QByteArray& nodeId) const override;
	virtual const imtbase::IStructuredCollectionFinder* GetCollectionFinder() const override;

	// reimplemented (IObjectCollection)
	virtual const imtbase::IRevisionController* GetRevisionController() const override;
	virtual const imtbase::ICollectionDataController* GetDataController() const override;
	virtual int GetOperationFlags(const Id& elementId = Id()) const override;
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

	// reimplemented (imtgql::IGqlSubscriptionClient)
	virtual void OnResponseReceived(const QByteArray& subscriptionId, const QByteArray& subscriptionData) override;
	virtual void OnSubscriptionStatusChanged(const QByteArray& subscriptionId, const SubscriptionStatus& status, const QString& message) override;

protected:
	DataPtr CreateObjectInstance(const QByteArray& typeId) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	IObjectCollection::DataPtr GetObject(const QByteArray& objectId, const QByteArray& typeId) const;
	bool GetElementType(const QByteArray& elementId, ElementType& valueOut) const;
	bool GetNodeInfo(const QByteArray& nodeId, imtgql::IGqlStructuredCollectionResponse::NodeInfo& valueOut) const;
	bool GetObjectInfo(const QByteArray& objectId, imtgql::IGqlStructuredCollectionResponse::ObjectInfo& valueOut) const;
	bool GetObjectMetaInfo(const QByteArray& objectId, imtgql::IGqlStructuredCollectionResponse::ObjectInfo& valueOut) const;
	bool GetObjectDataMetaInfo(const QByteArray& objectId, imtgql::IGqlStructuredCollectionResponse::ObjectInfo& valueOut) const;

protected:
	I_REF(IGqlSubscriptionManager, m_subscriptionManagerCompPtr);
	I_REF(IGqlObjectCollectionDelegate, m_delegateCompPtr);
	I_MULTIATTR(QByteArray, m_typeIdsAttrPtr);
	I_MULTITEXTATTR(m_typeNamesAttrPtr);
	I_MULTIFACT(istd::IChangeable, m_objectFactoriesCompPtr);

	typedef QMap<QByteArray, IGqlObjectCollectionDelegate*> DelegatesMap;

	iprm::COptionsManager m_typesInfo;

	QByteArray m_addMeasurementSubsriptionId;
};


} //namespace imtclientgql


