#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/TIStructuredCollectionInfo.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/imtbase::IOperationContext.h>
#include <imtgql/IGqlRequest.h>
#include <imtgql/IGqlHierarchicalStructureResponse.h>
#include <imtclientgql/IGqlResponseCreator.h>


namespace imtclientgql
{


class IGqlHierarchicalStructureDelegate: virtual public IGqlResponseCreator
{
public:
	typedef QByteArray Id;
	typedef QByteArrayList Ids;

	virtual imtgql::IGqlRequest* CreateInsertNewNodeRequest(
				const QString& name,
				const QString& description = QString(),
				const Id& proposedNodeId = Id(),
				const Id& parentNodeId = Id(),
				const idoc::IDocumentMetaInfo* metaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
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

	virtual imtgql::IGqlRequest* CreateInsertLeafRequest(
				const Id& leafId,
				const Id& nodeId,
				const QString& leafName,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual imtgql::IGqlRequest* CreateMoveLeafRequest(
				const Id& leafId,
				const Id& sourceNodeId,
				const Id& targetNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;
	virtual imtgql::IGqlRequest* CreateRemoveLeafRequest(
				const Id& leafId,
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) = 0;

	virtual imtgql::IGqlRequest* CreateGetItemCountRequest(
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateGetItemIdsRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;

	virtual imtgql::IGqlRequest* CreateGetItemInfosRequest(const Ids& itemIds) = 0;
	virtual imtgql::IGqlRequest* CreateGetItemPathRequest(const Id& itemId) const = 0;

	virtual imtbase::IHierarchicalStructureIterator* CreateHierarchicalStructureIterator(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
};


} // namespace imtclientgql


