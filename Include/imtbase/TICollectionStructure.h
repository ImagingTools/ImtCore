#pragma once


// ImtCore includes
#include <imtbase/ICollectionStructureInfo.h>


namespace imtbase
{


class IObjectCollectionStructure: virtual public IObjectCollectionStructureInfo
{
public:
	virtual QByteArray InsertNewNode(
				const QString& name,
				const QString& description = QString(),
				const QByteArray& proposedNodeId = QByteArray(),
				const QByteArray& parentNodeId = QByteArray(),
				const idoc::IDocumentMetaInfo* metaInfoPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool SetNodeName(
				const QByteArray& nodeId,
				const QString& name,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool SetNodeDescription(
				const QByteArray& nodeId,
				const QString& description,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool SetNodeMetaInfo(
				const QByteArray& nodeId,
				const idoc::IDocumentMetaInfo &metaInfo,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool MoveNode(
				const QByteArray& nodeId,
				const QByteArray& parentNodeId,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool RemoveNode(
				const QByteArray& nodeId,
				const IOperationContext* operationContextPtr = nullptr) = 0;

	virtual bool AddObjectToNode(
				const QByteArray& objectId,
				const QByteArray& nodeId,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool MoveObjectToNode(
				const QByteArray& objectId,
				const QByteArray& fromNodeId,
				const QByteArray& toNodeId,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool RemoveObjectFromNode(
				const QByteArray& objectId,
				const QByteArray& nodeId,
				const IOperationContext* operationContextPtr = nullptr) = 0;
};


} // namespace imtbase


