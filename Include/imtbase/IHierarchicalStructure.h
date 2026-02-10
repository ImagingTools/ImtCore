// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IHierarchicalStructureInfo.h>


namespace imtbase
{


class IOperationContext;


class IHierarchicalStructure: virtual public IHierarchicalStructureInfo
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
				const idoc::IDocumentMetaInfo& metaInfo,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool MoveNode(
				const Id& nodeId,
				const Id& parentNodeId,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool RemoveNode(
				const Id& nodeId,
				const IOperationContext* operationContextPtr = nullptr) = 0;

	/**
		Insert a reference to an item into the given node.
		This method creates a leaf inside of the given parent node.
	*/
	virtual bool InsertLeaf(
				const Id& leafId,
				const Id& nodeId,
				const QString& leafName,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool MoveLeaf(
				const Id& leafId,
				const Id& sourceNodeId,
				const Id& targetNodeId,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool RemoveLeaf(
				const Id& leafId,
				const Id& nodeId,
				const IOperationContext* operationContextPtr = nullptr) = 0;
};


} // namespace imtbase


