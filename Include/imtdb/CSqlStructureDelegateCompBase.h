#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtgql/CGqlObject.h>
#include <ifile/IFilePersistence.h>

// GmgCore includes
#include <imtbase/IOperationContext.h>
#include <imtbase/ICollectionStructure.h>
#include <imtdb/IDatabaseEngine.h>
// #include <imtclientgql/IGqlStructureResponse.h>


namespace imtdb
{


class CSqlStructureDelegateCompBase:
			public ilog::CLoggerComponentBase,
			virtual public imtbase::ICollectionStructure
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CSqlStructureDelegateCompBase);
		I_REGISTER_INTERFACE(imtbase::ICollectionStructure);
		I_ASSIGN(m_dbEngineCompPtr, "DatabaseEngine", "Database engine used for low level SQL quering", true, "DatabaseEngine");
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
	virtual QList<PathElement> GetNodePath(const Id& nodeId) const override;
	virtual Ids GetObjectParentNodeIds(const Id& objectId) const override;
	virtual imtbase::ICollectionStructureController* GetHierarchicalStructureController() override;

protected:
	virtual QByteArray CreateInsertNewNodeQuery(
				const QString& name,
				const QString& description = QString(),
				const Id& proposedNodeId = Id(),
				const Id& parentNodeId = Id(),
				const idoc::IDocumentMetaInfo* metaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual QByteArray CreateSetNodeNameQuery(
				const Id& nodeId,
				const QString& name,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual QByteArray CreateSetNodeDescriptionQuery(
				const Id& nodeId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual QByteArray CreateSetNodeMetaInfoQuery(
				const Id& nodeId,
				const idoc::IDocumentMetaInfo* metaInfo,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual QByteArray CreateMoveNodeQuery(
				const Id& nodeId,
				const Id& parentNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual QByteArray CreateRemoveNodeQuery(
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;

	virtual QByteArray CreateInsertNewObjectQuery(
				const Id& objectId,
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual QByteArray CreateMoveObjectQuery(
				const Id& objectId,
				const Id& sourceNodeId,
				const Id& targetNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual QByteArray CreateRemoveObjectQuery(
				const Id& objectId,
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual QByteArray CreateGetNodeCountQuery(const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	virtual QByteArray CreateGetNodeIdsQuery(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	virtual QByteArray CreateGetObjectIdsQuery(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	virtual QByteArray CreateGetNodeInfoQuery(const Id& nodeId) const = 0;
	virtual QByteArray CreateGetNodePathQuery(const Id& nodeId) const = 0;
	virtual QByteArray CreateGetObjectParentNodeIdsQuery(const Id& objectId) const = 0;
	virtual bool ExecuteTransaction(const QByteArray& sqlQuery) const;

protected:
	I_REF(imtdb::IDatabaseEngine, m_dbEngineCompPtr);
};


} // namespace imtclientgql


