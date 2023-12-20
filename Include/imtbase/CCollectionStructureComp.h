#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <ilog/TLoggerCompWrap.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtclientgql/IGqlStructureDelegate.h>
#include <imtclientgql/IGqlClient.h>
#include <imtclientgql/IGqlSubscriptionClient.h>
#include <imtgql/IGqlStructuredCollectionResponse.h>
#include <imtbase/CCollectionInfo.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtbase/ICollectionStructureController.h>


namespace imtbase
{


class CCollectionStructureComp:
			public QObject,
			public ilog::TLoggerCompWrap<icomp::CComponentBase>,
			virtual public imtbase::ICollectionStructure,
			virtual public imtbase::ICollectionStructureController
{
	Q_OBJECT
public:
	typedef ilog::TLoggerCompWrap<icomp::CComponentBase> BaseClass;

    I_BEGIN_COMPONENT(CCollectionStructureComp);
		I_REGISTER_INTERFACE(imtbase::ICollectionStructure);
		I_REGISTER_INTERFACE(imtbase::ICollectionStructureInfo);
		I_REGISTER_INTERFACE(imtbase::ICollectionStructureController);
		I_ASSIGN(m_delegateCompPtr, "GqlStructureDelegate", "GraphQL-based document delegate for database CRUD oeprations", true, "GqlObjectCollectionDelegate");
	I_END_COMPONENT;

	using ICollectionStructure::Id;

	// reimpolemented (imtbase::ICollectionStructure)
	virtual Id InsertNewNode(
				const QString& name,
				const QString& description = QString(),
				const Id& proposedNodeId = Id(),
				const Id& parentNodeId = Id(),
				const idoc::IDocumentMetaInfo* metaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool SetNodeName(
				const Id& nodeId,
				const QString& name,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool SetNodeDescription(
				const Id& nodeId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool SetNodeMetaInfo(
				const Id& nodeId,
				const idoc::IDocumentMetaInfo& metaInfo,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool MoveNode(
				const Id& nodeId,
				const Id& parentNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveNode(
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool AssignObject(
				const Id& objectId,
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool MoveObject(
				const Id& objectId,
				const Id& sourceNodeId,
				const Id& targetNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveObject(
				const Id& objectId,
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual ICollectionStructureController* GetHierarchicalStructureController() override;

	// reimpolemented (imtbase::ICollectionStructureInfo)
	virtual int GetNodeCount(const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual Ids GetNodeIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtbase::ICollectionStructureIterator* CreateCollectionStructureIterator(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtbase::ICollectionStructureInfo::NodeInfo GetNodeInfo(const Id& nodeId) override;
	virtual Ids GetObjectParentNodeIds(const Id& objectId) const override;
	virtual NodePath GetNodePath(const Id& nodeId) const override;

	// reimplemented (imtbase::ICollectionStructureController)
	virtual Id InsertNewObjectIntoCollection(
		IObjectCollection*  objectCollectionPtr,
		const Id& nodeId,
		const QByteArray& typeId,
		const QString& name,
		const QString& description,
		IObjectCollection::DataPtr defaultValuePtr = IObjectCollection::DataPtr(),
		const Id& proposedElementId = Id(),
		const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
		const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
		const IOperationContext* operationContextPtr = nullptr) override;

protected:
	// reimplemented (icomp::CComponentBase)
    // virtual void OnComponentCreated() override;
    // virtual void OnComponentDestroyed() override;

protected:
	I_REF(ICollectionStructure, m_delegateCompPtr);
};


} //namespace imtbase


