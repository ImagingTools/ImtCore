#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <ilog/TLoggerCompWrap.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtbase/ICollectionDataController.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/IMetaInfoCreator.h>
#include <imtbase/IHierarchicalStructure.h>
#include <imtgql/IGqlStructuredCollectionResponse.h>
#include <imtclientgql/IClientProtocolEngine.h>
#include <imtclientgql/IGqlObjectCollectionDelegate.h>
#include <imtclientgql/IGqlClient.h>
#include <imtclientgql/IGqlSubscriptionClient.h>


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
			virtual public IGqlSubscriptionClient
{
	Q_OBJECT
public:
	typedef ilog::TLoggerCompWrap<icomp::CComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CGqlObjectCollectionComp);
		I_REGISTER_INTERFACE(IGqlSubscriptionClient);
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(IObjectCollectionInfo);
		I_REGISTER_INTERFACE(ICollectionInfo);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_delegateCompPtr, "GqlDatabaseDelegate", "GraphQL-based document delegate for database CRUD operations", true, "GqlObjectCollectionDelegate");
		I_ASSIGN_MULTI_0(m_typeIdsAttrPtr, "TypeIds", "List of type-ID corresponding to the registered factories", false);
		I_ASSIGN_MULTI_0(m_typeNamesAttrPtr, "TypeNames", "List of type names corresponding to the registered factories", false);
		I_ASSIGN_MULTI_0(m_objectFactoriesCompPtr, "ObjectFactories", "List of factories used for object creation", false);
	I_END_COMPONENT;

	CGqlObjectCollectionComp();

	virtual QByteArray InsertNewObject(
		const QByteArray& typeId,
		const QString& name,
		const QString& description,
		IObjectCollection::DataPtr defaultValuePtr = IObjectCollection::DataPtr(),
		const Id& proposedObjectId = Id(),
		const Id& nodeId = Id(),
		const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
		const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
		const imtbase::IOperationContext* operationContextPtr = nullptr);

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
	// bool GetElementType(const QByteArray& elementId, ElementType& valueOut) const;
	bool GetNodeInfo(const QByteArray& nodeId, imtgql::IGqlStructuredCollectionResponse::NodeInfo& valueOut) const;
	bool GetObjectInfo(const QByteArray& objectId, imtgql::IGqlStructuredCollectionResponse::ObjectInfo& valueOut) const;
	bool GetObjectMetaInfo(const QByteArray& objectId, idoc::MetaInfoPtr& valueOut) const;
	bool GetObjectDataMetaInfo(const QByteArray& objectId, idoc::MetaInfoPtr& valueOut) const;

protected:
	I_REF(IGqlObjectCollectionDelegate, m_delegateCompPtr);
	I_MULTIATTR(QByteArray, m_typeIdsAttrPtr);
	I_MULTITEXTATTR(m_typeNamesAttrPtr);
	I_MULTIFACT(istd::IChangeable, m_objectFactoriesCompPtr);

	typedef QMap<QByteArray, IGqlObjectCollectionDelegate*> DelegatesMap;

	iprm::COptionsManager m_typesInfo;
};


} //namespace imtclientgql


