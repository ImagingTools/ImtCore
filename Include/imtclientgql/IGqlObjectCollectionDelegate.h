#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/TIStructuredCollectionInfo.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/IOperationContext.h>
#include <imtclientgql/IGqlClient.h>
#include <imtgql/IGqlPrimitiveTypeResponse.h>
#include <imtgql/IGqlStructuredCollectionResponse.h>


namespace imtclientgql
{


/**
	Common interface for the GraphQL-based document database delegate.
	The job of the document delegate is the realization of the GraphQL requests for the common database operations (CRUD/L)
*/
class IGqlObjectCollectionDelegate: virtual public istd::IPolymorphic
{
public:
	enum MetaInfoType
	{
		MIT_KEYWORDS = idoc::IDocumentMetaInfo::MIT_USER + 30000,
		MIT_VERSION
	};

	/**
		Get the IDs of the supported document types.
	*/
	virtual QByteArrayList GetSupportedObjectTypeIds() const = 0;

	/**
		Infos
	*/
	virtual imtbase::IStructuredObjectCollectionInfo::ElementType GetElementType(const QByteArray& elementId) const = 0;
	virtual bool GetNodeInfo(const QByteArray& nodeId, imtgql::IGqlStructuredCollectionResponse::NodeInfo& outInfo) const = 0;
	virtual bool GetObjectInfo(const QByteArray& objectId, imtgql::IGqlStructuredCollectionResponse::ObjectInfo& outInfo) const = 0;
	virtual bool GetObjectMetaInfo(const QByteArray& objectId, idoc::MetaInfoPtr& outInfo) const = 0;
	virtual bool GetObjectDataMetaInfo(const QByteArray& objectId, idoc::MetaInfoPtr& outInfo) const = 0;
	virtual QByteArray GetObjectTypeId(const QByteArray& objectId) const = 0;

	/**
		Structure manipulations
	*/
	virtual QByteArray InsertNode(
				const QString& name,
				const QString& description,
				const QByteArray& proposedNodeId = QByteArray(),
				const QByteArray& parentNodeId = QByteArray(),
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual QSharedPointer<imtbase::IStructuredObjectCollectionInfo> GetNodeContent(const QByteArray& nodeId) const = 0;
	virtual bool SetNodeName(
				const QByteArray& nodeId,
				const QString& name,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual bool SetNodeDescription(
				const QByteArray& nodeId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual bool SetNodeMetaInfo(
				const QByteArray& nodeId,
				const idoc::IDocumentMetaInfo& metaInfo,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual bool MoveNode(
				const QByteArray& nodeId,
				const QByteArray& parentNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual bool RemoveNode(
				const QByteArray& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;

	virtual bool AddObjectToNode(
				const QByteArray& objectId,
				const QByteArray& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool MoveObjectToNode(
				const QByteArray& objectId,
				const QByteArray& nodeId,
				const QByteArray& newNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual bool RemoveObjectFromNode(
				const QByteArray& objectId,
				const QByteArray& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;

	/**
		Object manipulations
	*/
	virtual QByteArray InsertObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable& object,
				const QByteArray& proposedObjectId = QByteArray(),
				const QByteArray& nodeId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual bool SetObjectName(
				const QByteArray& objectId,
				const QString& name,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual bool SetObjectDescription(
				const QByteArray& objectId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual bool SetObjectMetaInfo(
				const QByteArray& objectId,
				const idoc::IDocumentMetaInfo& dataMetaInfo,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual bool GetObjectData(
				const QByteArray& objectId,
				const QByteArray& typeId,
				imtbase::IObjectCollection::DataPtr objectPtr) const = 0;
	virtual bool SetObjectData(
				const QByteArray& objectId,
				const QByteArray& typeId,
				const istd::IChangeable& object,
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual bool RemoveObject(
				const QByteArray& objectId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;

	/**
		Enumeration
	*/
	virtual int GetElementCount(
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	virtual imtgql::IGqlStructuredCollectionResponse::ElementList GetElementList(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	virtual bool GetSubCollection(
				imtbase::IObjectCollection* subcollection,
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
};


} // namespace imtclientgql


