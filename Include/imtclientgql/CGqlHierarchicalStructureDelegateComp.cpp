#include <imtclientgql/CGqlHierarchicalStructureDelegateComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imod/TModelWrap.h>
#include <iprm/IIdParam.h>
#include <iprm/TParamsPtr.h>
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/ICollectionFilter.h>
#include <imtgql/CGqlRequest.h>
#include <imtclientgql/CGqlObjectCollectionResponse.h>
#include <imtbase/CFilterCollectionProxy.h>
#include <imtbase/COperationContext.h>
#include <imtclientgql/CGqlHierarchicalStructureResponse.h>


namespace imtclientgql
{


// protected methods

imtgql::IGqlRequest* CGqlHierarchicalStructureDelegateComp::CreateInsertNewNodeRequest(
			const QString& name,
			const QString& description,
			const Id& proposedNodeId,
			const Id& parentNodeId,
			const idoc::IDocumentMetaInfo* /*metaInfoPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!name.isEmpty()){
		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "InsertNewNode");

		imtgql::CGqlObject input("input");
		input.InsertField("name", name);
		input.InsertField("description", description);
		input.InsertField("proposedNodeId", QVariant(proposedNodeId));
		input.InsertField("parentNodeId", QVariant(parentNodeId));
		requestPtr->AddParam(input);

		imtgql::CGqlObject query("query");
		query.InsertField("successfull");
		query.InsertField("id");
		requestPtr->AddField(query);
	}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlHierarchicalStructureDelegateComp::CreateSetNodeNameRequest(
			const Id& nodeId,
			const QString& name,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!nodeId.isEmpty() && !name.isEmpty()){
		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "SetNodeName");

		imtgql::CGqlObject input("input");
		input.InsertField("nodeId", QVariant(nodeId));
		input.InsertField("name", name);
		requestPtr->AddParam(input);

		imtgql::CGqlObject query("query");
		query.InsertField("successfull");
		requestPtr->AddField(query);
	}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlHierarchicalStructureDelegateComp::CreateSetNodeDescriptionRequest(
			const Id& nodeId,
			const QString& description,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!nodeId.isEmpty()){
		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "SetNodeDescription");

		imtgql::CGqlObject input("input");
		input.InsertField("nodeId", QVariant(nodeId));
		input.InsertField("description", description);
		requestPtr->AddParam(input);

		imtgql::CGqlObject query("query");
		query.InsertField("successfull");
		requestPtr->AddField(query);
	}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlHierarchicalStructureDelegateComp::CreateSetNodeMetaInfoRequest(
			const Id& /*nodeId*/,
			const idoc::IDocumentMetaInfo& /*metaInfo*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlHierarchicalStructureDelegateComp::CreateMoveNodeRequest(
			const Id& nodeId,
			const Id& parentNodeId,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!nodeId.isEmpty()){
		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "MoveNode");

		imtgql::CGqlObject input("input");
		input.InsertField("nodeId", QVariant(nodeId));
		input.InsertField("parentNodeId", QVariant(parentNodeId));
		requestPtr->AddParam(input);

		imtgql::CGqlObject query("query");
		query.InsertField("successfull");
		requestPtr->AddField(query);
	}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlHierarchicalStructureDelegateComp::CreateRemoveNodeRequest(
			const Id& nodeId,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!nodeId.isEmpty()){
		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "RemoveNode");

		imtgql::CGqlObject input("input");
		input.InsertField("nodeId", QVariant(nodeId));
		requestPtr->AddParam(input);

		imtgql::CGqlObject query("query");
		query.InsertField("successfull");
		requestPtr->AddField(query);
	}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlHierarchicalStructureDelegateComp::CreateInsertLeafRequest(
			const Id& leafId,
			const Id& nodeId,
			const QString& leafName,
			const imtbase::IOperationContext* operationContextPtr) const
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!leafId.isEmpty()){
		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "InsertNewLeaf");

		imtgql::CGqlObject input("input");
		input.InsertField("leafId", QVariant(leafId));
		input.InsertField("nodeId", QVariant(nodeId));
		requestPtr->AddParam(input);

		imtgql::CGqlObject query("query");
		query.InsertField("successfull");
		requestPtr->AddField(query);
	}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlHierarchicalStructureDelegateComp::CreateMoveLeafRequest(
			const Id& leafId,
			const Id& sourceNodeId,
			const Id& targetNodeId,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!leafId.isEmpty()){
		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "MoveLeaf");

		imtgql::CGqlObject input("input");
		input.InsertField("objectId", QVariant(leafId));
		input.InsertField("sourceNodeId", QVariant(sourceNodeId));
		input.InsertField("targetNodeId", QVariant(targetNodeId));
		requestPtr->AddParam(input);

		imtgql::CGqlObject query("query");
		query.InsertField("successfull");
		requestPtr->AddField(query);
	}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlHierarchicalStructureDelegateComp::CreateRemoveLeafRequest(
			const Id& leafId,
			const Id& nodeId,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!leafId.isEmpty()){
		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "RemoveLeaf");

		imtgql::CGqlObject input("input");
		input.InsertField("leafId", QVariant(leafId));
		input.InsertField("nodeId", QVariant(nodeId));
		requestPtr->AddParam(input);

		imtgql::CGqlObject query("query");
		query.InsertField("successfull");
		requestPtr->AddField(query);
	}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlHierarchicalStructureDelegateComp::CreateGetItemCountRequest(
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	//QByteArray nodeId;
	//if (selectionParamsPtr != nullptr){
	//	iprm::TParamsPtr<iprm::IIdParam> nodeIdPtr(selectionParamsPtr, "nodeId");
	//	iprm::TParamsPtr<imtbase::ICollectionFilter> filterPtr(selectionParamsPtr, "filter");

	//	if (nodeIdPtr.IsValid() && filterPtr.IsValid()){
	//		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "GetNodeCount");

	//		imtgql::CGqlObject input("input");
	//		input.InsertField("nodeId", QVariant(nodeIdPtr->GetId()));
	//		input.InsertField("filter", filterPtr->);
	//		requestPtr->AddParam(input);

	//		imtgql::CGqlObject query("query");
	//		query.InsertField("count");
	//		requestPtr->AddField(query);
	//	}
	//}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlHierarchicalStructureDelegateComp::CreateGetItemIdsRequest(
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	//QByteArray nodeId;
	//if (selectionParamsPtr != nullptr){
	//	iprm::TParamsPtr<iprm::IIdParam> nodeIdPtr(selectionParamsPtr, "nodeId");
	//	iprm::TParamsPtr<imtbase::ICollectionFilter> filterPtr(selectionParamsPtr, "filter");

	//	if (nodeIdPtr.IsValid() && filterPtr.IsValid()){
	//		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "GetNodeIds");

	//		imtgql::CGqlObject input("input");
	//		input.InsertField("nodeId", QVariant(nodeIdPtr->GetId()));
	//		input.InsertField("filter", filterPtr->);
	//		requestPtr->AddParam(input);

	//		imtgql::CGqlObject query("query");
	//		query.InsertField("count");
	//		requestPtr->AddField(query);
	//	}
	//}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlHierarchicalStructureDelegateComp::CreateGetItemInfosRequest(const Ids& itemIds) const
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!itemIds.isEmpty()){
		//requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "GetItemInfos");

		//imtgql::CGqlObject input("input");
		//input.InsertField("nodeId", QVariant(nodeId));
		//requestPtr->AddParam(input);

		//imtgql::CGqlObject query("query");
		//query.InsertField("info");
		//requestPtr->AddField(query);
	}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlHierarchicalStructureDelegateComp::CreateGetItemPathRequest(const Id& itemId) const
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!itemId.isEmpty()){
		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "GetItemPath");

		imtgql::CGqlObject input("input");
		input.InsertField("itemId", QVariant(itemId));
		requestPtr->AddParam(input);

		imtgql::CGqlObject query("query");
		query.InsertField("ItemInfos");
		requestPtr->AddField(query);
	}

	return requestPtr;
}


imtbase::IHierarchicalStructureIterator* CGqlHierarchicalStructureDelegateComp::CreateHierarchicalStructureIterator(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	return nullptr;
}


// reimplemented (IGqlResponseCreator)

imtgql::IGqlResponse* CGqlHierarchicalStructureDelegateComp::CreateResponse(const imtgql::IGqlRequest& request) const
{
	const imtgql::CGqlRequest* requestPtr = dynamic_cast<const imtgql::CGqlRequest*>(&request);
	if (requestPtr != nullptr){
		QByteArray commandId = request.GetCommandId();

		if (commandId == "InsertNewNode" ||
			commandId == "SetNodeName" ||
			commandId == "SetNodeDescription" ||
			commandId == "SetNodeMetaInfo" ||
			commandId == "MoveNode" ||
			commandId == "RemoveNode" ||
			commandId == "InsertNewLeaf" ||
			commandId == "MoveLeaf" ||
			commandId == "RemoveLeaf" ||
			commandId == "GetItemCount" ||
			commandId == "GetItemIds" ||
			commandId == "GetItemInfos" ||
			commandId == "GetItemPath"){

			CGqlHierarchicalStructureResponse* responsePtr = new CGqlHierarchicalStructureResponse();
			responsePtr->SetRequest(*requestPtr);

			return responsePtr;
		}
	}

	return nullptr;
}


} // namespace imtclientgql


