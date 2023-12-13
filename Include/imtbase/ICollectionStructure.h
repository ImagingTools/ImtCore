#pragma once


// ImtCore includes
#include <imtbase/ICollectionStructureInfo.h>


namespace imtbase
{


class IOperationContext;
class ICollectionStructureController;


class ICollectionStructure: virtual public ICollectionStructureInfo
{
public:
	virtual Id InsertNewNode(
				const QString& name,
				const QString& description = QString(),
				const Id& proposedNodeId = Id(),
				const Id& parentNodeId = Id(),
				const idoc::IDocumentMetaInfo* metaInfoPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool SetNodeName(
				const Id& nodeId,
				const QString& name,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool SetNodeDescription(
				const Id& nodeId,
				const QString& description,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool SetNodeMetaInfo(
				const Id& nodeId,
				const idoc::IDocumentMetaInfo &metaInfo,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool MoveNode(
				const Id& nodeId,
				const Id& parentNodeId,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool RemoveNode(
				const Id& nodeId,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool AssignObject(
				const Id& objectId,
				const Id& nodeId,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool MoveObject(
				const Id& objectId,
				const Id& sourceNodeId,
				const Id& targetNodeId,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool RemoveObject(
				const Id& objectId,
				const Id& nodeId,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual ICollectionStructureController* GetHierarchicalStructureController() = 0;
};


} // namespace imtbase


