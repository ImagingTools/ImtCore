#include <imtclientgql/CGqlStructureDelegateComp.h>


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
#include <imtclientgql/CGqlStructureResponse.h>


namespace imtclientgql
{


// protected methods

imtgql::IGqlRequest* CGqlStructureDelegateComp::CreateInsertNewNodeRequest(
			const QString& name,
			const QString& description,
			const Id& proposedNodeId,
			const Id& parentNodeId,
			const idoc::IDocumentMetaInfo* /*metaInfoPtr*/,
			const imtbase::IOperationContext* /*operationContextPtr*/)
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


imtgql::IGqlRequest* CGqlStructureDelegateComp::CreateSetNodeNameRequest(
			const Id& nodeId,
			const QString& name,
			const imtbase::IOperationContext* /*operationContextPtr*/)
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


imtgql::IGqlRequest* CGqlStructureDelegateComp::CreateSetNodeDescriptionRequest(
			const Id& nodeId,
			const QString& description,
			const imtbase::IOperationContext* /*operationContextPtr*/)
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


imtgql::IGqlRequest* CGqlStructureDelegateComp::CreateSetNodeMetaInfoRequest(
			const Id& /*nodeId*/,
			const idoc::IDocumentMetaInfo& /*metaInfo*/,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlStructureDelegateComp::CreateMoveNodeRequest(
			const Id& nodeId,
			const Id& parentNodeId,
			const imtbase::IOperationContext* /*operationContextPtr*/)
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


imtgql::IGqlRequest* CGqlStructureDelegateComp::CreateRemoveNodeRequest(
			const Id& nodeId,
			const imtbase::IOperationContext* /*operationContextPtr*/)
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


imtgql::IGqlRequest* CGqlStructureDelegateComp::CreateInsertNewObjectRequest(
			const Id& objectId,
			const Id& nodeId,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!objectId.isEmpty()){
		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "InsertNewObject");

		imtgql::CGqlObject input("input");
		input.InsertField("objectId", QVariant(objectId));
		input.InsertField("nodeId", QVariant(nodeId));
		requestPtr->AddParam(input);

		imtgql::CGqlObject query("query");
		query.InsertField("successfull");
		requestPtr->AddField(query);
	}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlStructureDelegateComp::CreateMoveObjectRequest(
			const Id& objectId,
			const Id& sourceNodeId,
			const Id& targetNodeId,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!objectId.isEmpty()){
		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "MoveObject");

		imtgql::CGqlObject input("input");
		input.InsertField("objectId", QVariant(objectId));
		input.InsertField("sourceNodeId", QVariant(sourceNodeId));
		input.InsertField("targetNodeId", QVariant(targetNodeId));
		requestPtr->AddParam(input);

		imtgql::CGqlObject query("query");
		query.InsertField("successfull");
		requestPtr->AddField(query);
	}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlStructureDelegateComp::CreateRemoveObjectRequest(
			const Id& objectId,
			const Id& nodeId,
			const imtbase::IOperationContext* /*operationContextPtr*/)
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!objectId.isEmpty()){
		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "RemoveObject");

		imtgql::CGqlObject input("input");
		input.InsertField("objectId", QVariant(objectId));
		input.InsertField("nodeId", QVariant(nodeId));
		requestPtr->AddParam(input);

		imtgql::CGqlObject query("query");
		query.InsertField("successfull");
		requestPtr->AddField(query);
	}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlStructureDelegateComp::CreateGetNodeCountRequest(const iprm::IParamsSet* selectionParamsPtr) const
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


imtgql::IGqlRequest* CGqlStructureDelegateComp::CreateGetNodeIdsRequest(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
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


imtgql::IGqlRequest* CGqlStructureDelegateComp::CreateGetNodeInfoRequest(const Id& nodeId)
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!nodeId.isEmpty()){
		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "GetNodeInfo");

		imtgql::CGqlObject input("input");
		input.InsertField("nodeId", QVariant(nodeId));
		requestPtr->AddParam(input);

		imtgql::CGqlObject query("query");
		query.InsertField("info");
		requestPtr->AddField(query);
	}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlStructureDelegateComp::CreateGetObjectParentNodeIdsRequest(const Id& objectId) const
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!objectId.isEmpty()){
		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "GetObjectParentNodeIds");

		imtgql::CGqlObject input("input");
		input.InsertField("objectId", QVariant(objectId));
		requestPtr->AddParam(input);

		imtgql::CGqlObject query("query");
		query.InsertField("nodeIds");
		requestPtr->AddField(query);
	}

	return requestPtr;
}


imtgql::IGqlResponse* CGqlStructureDelegateComp::CreateResponse(const imtgql::IGqlRequest& request) const
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
			commandId == "InsertNewObject" ||
			commandId == "MoveObject" ||
			commandId == "RemoveObject" ||
			commandId == "GetNodeCount" ||
			commandId == "GetNodeIds" ||
			commandId == "GetNodeInfo" ||
			commandId == "GetObjectParentNodeIds"){

			CGqlStructureResponse* responsePtr = new CGqlStructureResponse();
			responsePtr->SetRequest(*requestPtr);

			return responsePtr;
		}
	}

	return nullptr;
}


} // namespace imtclientgql


