#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtgql/CGqlObject.h>
#include <ifile/IFilePersistence.h>

// GmgCore includes
#include <imtgql/CGqlObjectCollectionInfo.h>
#include <imtgql/IGqlObjectCollectionDelegate.h>


namespace imtgql
{


class CGqlObjectCollectionDelegateCompBase:
			public ilog::CLoggerComponentBase,
			virtual public IGqlObjectCollectionDelegate
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CGqlObjectCollectionDelegateCompBase);
		I_REGISTER_INTERFACE(IGqlObjectCollectionDelegate);
		I_ASSIGN_MULTI_0(m_objectTypeIdsAttrPtr, "ObjectTypeIds", "ID of the supported object type for this delegate", true);
		I_ASSIGN(m_clientCompPtr, "ApiClient", "GraphQL API client", true, "ApiClient");
	I_END_COMPONENT;

	// reimplemented (IGqlObjectCollectionDelegate)
	virtual QByteArrayList GetSupportedObjectTypeIds() const override;

	/**
		Infos
	*/
	virtual imtbase::IStructuredObjectCollectionInfo::ElementType GetElementType(const QByteArray& elementId) const override;
	virtual bool GetNodeInfo(const QByteArray& nodeId, imtgql::IGqlStructuredCollectionResponse::NodeInfo& outInfo) const override;
	virtual bool GetObjectInfo(const QByteArray& objectId, imtgql::IGqlStructuredCollectionResponse::ObjectInfo& outInfo) const override;

	/**
		Structure manipulations
	*/
	virtual QByteArray InsertNode(
				const QString& name,
				const QString& description,
				const QByteArray& proposedNodeId = QByteArray(),
				const QByteArray& parentNodeId = QByteArray(),
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual QSharedPointer<imtbase::IStructuredObjectCollectionInfo> GetNodeContent(const QByteArray& nodeId) const override;
	virtual bool SetNodeName(
				const QByteArray& nodeId,
				const QString& name,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual bool SetNodeDescription(
				const QByteArray& nodeId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual bool SetNodeMetaInfo(
				const QByteArray& nodeId,
				const idoc::IDocumentMetaInfo& metaInfo,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual bool MoveNode(
				const QByteArray& nodeId,
				const QByteArray& parentNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual bool RemoveNode(
				const QByteArray& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

	virtual bool AddObjectToNode(
				const QByteArray& objectId,
				const QByteArray& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool MoveObjectToNode(
				const QByteArray& objectId,
				const QByteArray& nodeId,
				const QByteArray& newNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveObjectFromNode(
				const QByteArray& objectId,
				const QByteArray& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

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
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual bool SetObjectName(
				const QByteArray& objectId,
				const QString& name,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual bool SetObjectDescription(
				const QByteArray& objectId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual bool SetObjectMetaInfo(
				const QByteArray& objectId,
				const idoc::IDocumentMetaInfo& dataMetaInfo,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual bool GetObject(
				const QByteArray& objectId,
				imtbase::IObjectCollection::DataPtr) const override;
	virtual bool SetObject(
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual bool RemoveObject(
				const QByteArray& objectId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

	/**
		Enumeration
	*/
	virtual bool GetElementCount(
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtgql::IGqlStructuredCollectionResponse::ElementList GetElementList(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;

protected:
	virtual IGqlRequest* CreateGetElementType(const QByteArray& elementId) const;
	virtual IGqlRequest* CreateGetNodeInfoRequest(const QByteArray& nodeId) const;
	virtual IGqlRequest* CreateGetObjectInfoRequest(const QByteArray& objectId) const;

	virtual IGqlRequest* CreateInsertNodeRequest(
				const QString& name,
				const QString& description,
				const QByteArray& proposedNodeId = QByteArray(),
				const QByteArray& parentNodeId = QByteArray(),
				const idoc::IDocumentMetaInfo* metaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const;
	virtual IGqlRequest* CreateSetNodeNameRequest(
				const QByteArray& nodeId,
				const QString& name,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const;
	virtual IGqlRequest* CreateSetNodeDescriptionRequest(
				const QByteArray& nodeId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const;
	virtual IGqlRequest* CreateSetNodeMetaInfoRequest(
				const QByteArray& nodeId,
				const idoc::IDocumentMetaInfo& metaInfo,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const;
	virtual IGqlRequest* CreateMoveNodeRequest(
				const QByteArray& nodeId,
				const QByteArray& parentNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const;
	virtual IGqlRequest* CreateRemoveNodeRequest(
				const QByteArray& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const;

	virtual IGqlRequest* CreateAddObjectToNodeRequest(
				const QByteArray& objectId,
				const QByteArray& nodeId,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr);
	virtual IGqlRequest* CreateMoveObjectToNodeRequest(
				const QByteArray& objectId,
				const QByteArray& nodeId,
				const QByteArray& newNodeId,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr);
	virtual IGqlRequest* CreateRemoveObjectFromNodeRequest(
				const QByteArray& objectId,
				const QByteArray& nodeId,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const;

	virtual IGqlRequest* CreateInsertObjectRequest(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* objectPtr,
				const QString& uploadUrl = QString(),
				const QByteArray& proposedObjectId = QByteArray(),
				const QByteArray& nodeId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const;
	virtual IGqlRequest* CreateSetObjectNameRequest(
				const QByteArray& objectId,
				const QString& name,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const;
	virtual IGqlRequest* CreateSetObjectDescriptionRequest(
				const QByteArray& objectId,
				const QString& description,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const;
	virtual IGqlRequest* CreateSetObjectMetaInfoRequest(
				const QByteArray& objectId,
				const idoc::IDocumentMetaInfo& metaInfo,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const;
	virtual IGqlRequest* CreateGetObjectRequest(const QByteArray& objectId) const;
	virtual IGqlRequest* CreateSetObjectRequest(
				const QByteArray& objectId,
				const istd::IChangeable* objectPtr,
				const QString& uploadUrl = QString(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const;
	virtual IGqlRequest* CreateRemoveObjectRequest(
				const QByteArray& objectId,
				int clientElementVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const;

	virtual IGqlResponse* CreateResponse(const IGqlRequest& request) const = 0;

protected:
	I_MULTIATTR(QByteArray, m_objectTypeIdsAttrPtr);
	I_REF(imtgql::IGqlClient, m_clientCompPtr);
};


} // namespace gmgaws


