#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/TIStructuredCollectionInfo.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/ICollectionStructure.h>
#include <imtbase/IOperationContext.h>
#include <imtclientgql/IGqlClient.h>
#include <imtgql/IGqlPrimitiveTypeResponse.h>
#include <imtgql/IGqlStructuredCollectionResponse.h>
//#include <imtgql/IGqlStructurereq.h>


namespace imtclientgql
{


class IGqlStructureDelegate:
			virtual public imtbase::ICollectionStructure//,
			//virtual public IGqlStructureRequestDelegate
{
public:
	//**
	//	Structure infos
	//*/
	//virtual int GetElementsCount(const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	//virtual QByteArrayList GetElementIds(
	//			int offset = 0,
	//			int count = -1,
	//			const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	//virtual imtbase::ICollectionStructureInfo::ElementType GetElementType(const QByteArray& elementId) const = 0;
	//virtual QByteArrayList GetElementBasePath(const QByteArray& elementId) const = 0;
	//virtual QByteArrayList GetObjectParentNodes(const QByteArray& objectId) const = 0;

	//virtual imtbase::ICollectionStructureInfo::NodeInfo GetNodeInfo(const QByteArray& nodeId);

	///**
	//	Structure manipulations
	//*/
	//virtual QByteArray InsertNode(
	//			const QString& name,
	//			const QString& description,
	//			const QByteArray& proposedNodeId = QByteArray(),
	//			const QByteArray& parentNodeId = QByteArray(),
	//			const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
	//			const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	//virtual QSharedPointer<imtbase::IStructuredObjectCollectionInfo> GetNodeContent(const QByteArray& nodeId) const = 0;
	//virtual bool SetNodeName(
	//			const QByteArray& nodeId,
	//			const QString& name,
	//			const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	//virtual bool SetNodeDescription(
	//			const QByteArray& nodeId,
	//			const QString& description,
	//			const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	//virtual bool SetNodeMetaInfo(
	//			const QByteArray& nodeId,
	//			const idoc::IDocumentMetaInfo& metaInfo,
	//			const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	//virtual bool MoveNode(
	//			const QByteArray& nodeId,
	//			const QByteArray& parentNodeId,
	//			const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	//virtual bool RemoveNode(
	//			const QByteArray& nodeId,
	//			const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;

	//virtual bool AddObject(
	//			const QByteArray& objectId,
	//			const QByteArray& nodeId,
	//			const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	//virtual bool MoveObject(
	//			const QByteArray& objectId,
	//			const QByteArray& sourceNodeId,
	//			const QByteArray& targetNodeId,
	//			const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	//virtual bool RemoveObject(
	//			const QByteArray& objectId,
	//			const QByteArray& nodeId,
	//			const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
};


} // namespace imtclientgql


