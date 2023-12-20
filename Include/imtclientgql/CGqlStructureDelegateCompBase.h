#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtgql/CGqlObject.h>
#include <ifile/IFilePersistence.h>

// GmgCore includes
#include <imtbase/IOperationContext.h>
#include <imtclientgql/IGqlStructureDelegate.h>
#include <imtclientgql/IGqlStructureResponse.h>


namespace imtclientgql
{


class CGqlStructureDelegateCompBase:
			public ilog::CLoggerComponentBase,
			virtual public IGqlStructureDelegate
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CGqlStructureDelegateCompBase);
		I_REGISTER_INTERFACE(IGqlStructureDelegate);
		I_ASSIGN(m_clientCompPtr, "ApiClient", "GraphQL API client", true, "ApiClient");
	I_END_COMPONENT;

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

	virtual int GetNodeCount(const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual Ids GetNodeIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtbase::ICollectionStructureIterator* CreateCollectionStructureIterator(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual NodeInfo GetNodeInfo(const Id& nodeId) override;
	virtual Ids GetObjectParentNodeIds(const Id& objectId) const override;
	virtual imtbase::ICollectionStructureController* GetHierarchicalStructureController() override;
	NodePath GetNodePath(const Id& nodeId) const override;

protected:
	virtual imtgql::IGqlRequest* CreateInsertNewNodeRequest(
				const QString& name,
				const QString& description = QString(),
				const Id& proposedNodeId = Id(),
				const Id& parentNodeId = Id(),
				const idoc::IDocumentMetaInfo* metaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual imtgql::IGqlRequest* CreateSetNodeNameRequest(
				const Id& nodeId,
				const QString& name,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual imtgql::IGqlRequest* CreateSetNodeDescriptionRequest(
				const Id& nodeId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual imtgql::IGqlRequest* CreateSetNodeMetaInfoRequest(
				const Id& nodeId,
				const idoc::IDocumentMetaInfo& metaInfo,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual imtgql::IGqlRequest* CreateMoveNodeRequest(
				const Id& nodeId,
				const Id& parentNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual imtgql::IGqlRequest* CreateRemoveNodeRequest(
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;

	virtual imtgql::IGqlRequest* CreateInsertNewObjectRequest(
				const Id& objectId,
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual imtgql::IGqlRequest* CreateMoveObjectRequest(
				const Id& objectId,
				const Id& sourceNodeId,
				const Id& targetNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual imtgql::IGqlRequest* CreateRemoveObjectRequest(
				const Id& objectId,
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;

	virtual imtgql::IGqlRequest* CreateGetNodeCountRequest(const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateGetNodeIdsRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateGetNodeInfoRequest(const Id& nodeId) = 0;

	virtual imtgql::IGqlRequest* CreateGetObjectParentNodeIdsRequest(const Id& objectId) const = 0;

	virtual imtgql::IGqlResponse* CreateResponse(const imtgql::IGqlRequest& request) const = 0;

protected:
	I_REF(IGqlClient, m_clientCompPtr);
};


} // namespace imtclientgql


