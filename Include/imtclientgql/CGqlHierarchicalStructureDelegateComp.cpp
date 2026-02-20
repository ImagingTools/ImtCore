// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
#include <imtbase/ICollectionFilter.h>
#include <imtgql/CGqlRequest.h>
#include <imtbase/COperationContext.h>


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

		imtgql::CGqlParamObject input;
		input.InsertParam("name", name);
		input.InsertParam("description", description);
		input.InsertParam("proposedNodeId", QVariant(proposedNodeId));
		input.InsertParam("parentNodeId", QVariant(parentNodeId));
		requestPtr->AddParam("input", input);

		imtgql::CGqlFieldObject query;
		query.InsertField("successfull");
		query.InsertField("id");
		requestPtr->AddField("query", query);
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

		imtgql::CGqlParamObject input;
		input.InsertParam("nodeId", QVariant(nodeId));
		input.InsertParam("name", name);
		requestPtr->AddParam("input", input);

		imtgql::CGqlFieldObject query;
		query.InsertField("successfull");
		requestPtr->AddField("query", query);
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

		imtgql::CGqlParamObject input;
		input.InsertParam("nodeId", QVariant(nodeId));
		input.InsertParam("description", description);
		requestPtr->AddParam("input", input);

		imtgql::CGqlFieldObject query;
		query.InsertField("successfull");
		requestPtr->AddField("query", query);
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

		imtgql::CGqlParamObject input;
		input.InsertParam("nodeId", QVariant(nodeId));
		input.InsertParam("parentNodeId", QVariant(parentNodeId));
		requestPtr->AddParam("input", input);

		imtgql::CGqlFieldObject query;
		query.InsertField("successfull");
		requestPtr->AddField("query", query);
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

		imtgql::CGqlParamObject input;
		input.InsertParam("nodeId", QVariant(nodeId));
		requestPtr->AddParam("input", input);

		imtgql::CGqlFieldObject query;
		query.InsertField("successfull");
		requestPtr->AddField("query", query);
	}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlHierarchicalStructureDelegateComp::CreateInsertLeafRequest(
			const Id& leafId,
			const Id& nodeId,
			const QString& /*leafName*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!leafId.isEmpty()){
		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "InsertNewLeaf");

		imtgql::CGqlParamObject input;
		input.InsertParam("leafId", QVariant(leafId));
		input.InsertParam("nodeId", QVariant(nodeId));
		requestPtr->AddParam("input", input);

		imtgql::CGqlFieldObject query;
		query.InsertField("successfull");
		requestPtr->AddField("query", query);
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

		imtgql::CGqlParamObject input;
		input.InsertParam("objectId", QVariant(leafId));
		input.InsertParam("sourceNodeId", QVariant(sourceNodeId));
		input.InsertParam("targetNodeId", QVariant(targetNodeId));
		requestPtr->AddParam("input", input);

		imtgql::CGqlFieldObject query;
		query.InsertField("successfull");
		requestPtr->AddField("query", query);
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

		imtgql::CGqlParamObject input;
		input.InsertParam("leafId", QVariant(leafId));
		input.InsertParam("nodeId", QVariant(nodeId));
		requestPtr->AddParam("input", input);

		imtgql::CGqlFieldObject query;
		query.InsertField("successfull");
		requestPtr->AddField("query", query);
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

	//		imtgql::CGqlParamObject input("input");
	//		input.InsertParam("nodeId", QVariant(nodeIdPtr->GetId()));
	//		input.InsertParam("filter", filterPtr->);
	//		requestPtr->AddParam(input);

	//		imtgql::CGqlParamObject query("query");
	//		query.InsertParam("count");
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

	//		imtgql::CGqlParamObject input("input");
	//		input.InsertParam("nodeId", QVariant(nodeIdPtr->GetId()));
	//		input.InsertParam("filter", filterPtr->);
	//		requestPtr->AddParam(input);

	//		imtgql::CGqlParamObject query("query");
	//		query.InsertParam("count");
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

		//imtgql::CGqlParamObject input("input");
		//input.InsertParam("nodeId", QVariant(nodeId));
		//requestPtr->AddParam(input);

		//imtgql::CGqlParamObject query("query");
		//query.InsertParam("info");
		//requestPtr->AddField(query);
	}

	return requestPtr;
}


imtgql::IGqlRequest* CGqlHierarchicalStructureDelegateComp::CreateGetItemPathRequest(const Id& itemId) const
{
	imtgql::CGqlRequest* requestPtr = nullptr;

	if (!itemId.isEmpty()){
		requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, "GetItemPath");

		imtgql::CGqlParamObject input;
		input.InsertParam("itemId", QVariant(itemId));
		requestPtr->AddParam("input", input);

		imtgql::CGqlFieldObject query;
		query.InsertField("ItemInfos");
		requestPtr->AddField("query", query);
	}

	return requestPtr;
}


imtbase::IHierarchicalStructureIterator* CGqlHierarchicalStructureDelegateComp::CreateHierarchicalStructureIterator(
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	return nullptr;
}


bool CGqlHierarchicalStructureDelegateComp::IsValid(imtgql::IGqlResponse& /*response*/) const
{
	return false;
}


bool CGqlHierarchicalStructureDelegateComp::GetNodeId(imtgql::IGqlResponse& /*response*/, Id& /*out*/) const
{
	return false;
}


bool CGqlHierarchicalStructureDelegateComp::GetOperationResult(imtgql::IGqlResponse& /*response*/, bool& /*out*/) const
{
	return false;
}


bool CGqlHierarchicalStructureDelegateComp::GetItemCount(imtgql::IGqlResponse& /*response*/, bool /*out*/) const
{
	return false;
}


bool CGqlHierarchicalStructureDelegateComp::GetItemIds(imtgql::IGqlResponse& /*response*/, Ids& /*out*/) const
{
	return false;
}


bool CGqlHierarchicalStructureDelegateComp::GetItemPath(imtgql::IGqlResponse& /*response*/, Ids& /*out*/) const
{
	return false;
}


bool CGqlHierarchicalStructureDelegateComp::GetItemInfos(imtgql::IGqlResponse& /*response*/, imtbase::IHierarchicalStructureInfo::ItemInfoList& /*out*/) const
{
	return false;
}


//m_data = replyData;
//m_commandId = request.GetCommandId();
//
//m_json = QJsonDocument::fromJson(replyData);
//
//QJsonObject data = m_json.object().value("data").toObject();
//
//if (data.contains(m_commandId)){
//	data = data.value(m_commandId).toObject();
//}
//
//if (data.contains("itemsCount")){
//	m_variant = data.value("itemsCount").toVariant();
//}
//
//if (data.contains("itemIds")){
//	QStringList itemIdsData = data.value("itemIds").toString().split(";");
//	//		ElementInfo itemInfo;
//
//	//		for (QString id: itemIdsData){
//	//			itemInfo.isNode = false;
//	//			itemInfo.id = id.toLatin1();
//	//			// m_elementList.push_back(itemInfo);
//	//		}
//			// m_isElementListPresent = true;
//}
//
//if (data.contains("objectData")){
//	m_variant = data.value("objectData").toVariant();
//	qDebug() << m_variant;
//}
//
//if (data.contains("renameNotification") || data.contains("setDescriptionNotification")){
//	m_variant = true;
//}
//
//QByteArrayList keys;
//keys << "info";
//keys << "metaInfo";
//keys << "dataMetaInfo";
//keys << "addedNotification";
//keys << "removedNotification";
//keys << "updatedNotification";
//keys << "updatedCollectionNotification";
//keys << "items";
//
//for (QByteArray key : keys){
//	if (data.contains(key)){
//		m_variant = data;
//
//		break;
//	}
//}

} // namespace imtclientgql


