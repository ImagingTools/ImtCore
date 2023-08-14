#pragma once


// ACF includes
#include <istd/TSmartPtr.h>
#include <iprm/IParamsSet.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/TIStructuredCollectionInfo.h>
#include <imtbase/IOperationContext.h>
#include <imtcom/IFileTransfer.h>
#include <imtgql/IGqlResponse.h>
#include <imtgql/IGqlClient.h>


namespace imtgql
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
	virtual IGqlRequest* CreateGetElementType(const QByteArray& elementId) const = 0;
	virtual IGqlRequest* CreateGetNodeInfoRequest(const QByteArray& nodeId) const = 0;
	virtual IGqlRequest* CreateGetObjectInfoRequest(const QByteArray& objectId) const = 0;

	/**
		Structure manipulations
	*/
	virtual IGqlRequest* CreateInsertNodeRequest(
				const QString& name,
				const QString& description,
				const QByteArray& proposedNodeId = QByteArray(),
				const QByteArray& parentNodeId = QByteArray(),
				const idoc::IDocumentMetaInfo* metaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual IGqlRequest* CreateSetNodeNameRequest(
				const QByteArray& nodeId,
				const QString& name,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual IGqlRequest* CreateSetNodeDescriptionRequest(
				const QByteArray& nodeId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual IGqlRequest* CreateSetNodeMetaInfoRequest(
				const QByteArray& nodeId,
				const idoc::IDocumentMetaInfo& metaInfo,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual IGqlRequest* CreateMoveNodeRequest(
				const QByteArray& nodeId,
				const QByteArray& parentNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual IGqlRequest* CreateRemoveNodeRequest(
				const QByteArray& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;

	virtual IGqlRequest* CreateAddObjectToNodeRequest(
				const QByteArray& objectId,
				const QByteArray& nodeId,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual IGqlRequest* CreateMoveObjectToNodeRequest(
				const QByteArray& objectId,
				const QByteArray& nodeId,
				const QByteArray& newNodeId,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual IGqlRequest* CreateRemoveObjectFromNodeRequest(
				const QByteArray& objectId,
				const QByteArray& nodeId,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;

	/**
		Object manipulations
	*/
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
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual IGqlRequest* CreateSetObjectNameRequest(
				const QByteArray& objectId,
				const QString& name,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual IGqlRequest* CreateSetObjectDescriptionRequest(
				const QByteArray& objectId,
				const QString& description,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual IGqlRequest* CreateSetObjectMetaInfoRequest(
				const QByteArray& objectId,
				const idoc::IDocumentMetaInfo& metaInfo,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual IGqlRequest* CreateGetObjectRequest(const QByteArray& objectId) const = 0;
	virtual IGqlRequest* CreateSetObjectRequest(
				const QByteArray& objectId,
				const istd::IChangeable* objectPtr,
				const QString& uploadUrl = QString(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
				int clientVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual IGqlRequest* CreateRemoveObjectRequest(
				const QByteArray& objectId,
				int clientElementVersion = -1,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;

	/**
		Enumeration
	*/
	virtual IGqlRequest* CreateGetElementCountRequest(
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	virtual IGqlRequest* CreateGetElementListRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;

	/**
		File Transfer Management
	*/
	virtual IGqlRequest* CreateUploadUrlsRequest(const QStringList& fileNames, const QByteArray& nodeId) const = 0;
	virtual IGqlRequest* CreateDownloadUrlsRequest(const QByteArrayList& objectIds) const = 0;
	virtual imtcom::IFileTransfer* GetFileTransfer() const = 0;

	/**
		Create response
	*/
	virtual IGqlResponse* CreateResponse(const IGqlRequest& request) const = 0;
};


} // namespace imtgql


