// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IHierarchicalStructureInfo.h>
#include <imtbase/IOperationContext.h>
#include <imtgql/IGqlRequest.h>
#include <imtgql/IGqlResponse.h>


namespace imtclientgql
{


class IGqlHierarchicalStructureDelegate: virtual public istd::IPolymorphic
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
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateSetNodeDescriptionRequest(
				const Id& nodeId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateSetNodeMetaInfoRequest(
				const Id& nodeId,
				const idoc::IDocumentMetaInfo& metaInfo,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateMoveNodeRequest(
				const Id& nodeId,
				const Id& parentNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateRemoveNodeRequest(
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;

	virtual imtgql::IGqlRequest* CreateInsertLeafRequest(
				const Id& leafId,
				const Id& nodeId,
				const QString& leafName,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateMoveLeafRequest(
				const Id& leafId,
				const Id& sourceNodeId,
				const Id& targetNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateRemoveLeafRequest(
				const Id& leafId,
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const = 0;

	virtual imtgql::IGqlRequest* CreateGetItemCountRequest(
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	virtual imtgql::IGqlRequest* CreateGetItemIdsRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;

	virtual imtgql::IGqlRequest* CreateGetItemInfosRequest(const Ids& itemIds) const = 0;
	virtual imtgql::IGqlRequest* CreateGetItemPathRequest(const Id& itemId) const = 0;

	virtual imtbase::IHierarchicalStructureIterator* CreateHierarchicalStructureIterator(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;

	virtual bool IsValid(imtgql::IGqlResponse& reqponse) const = 0;
	virtual bool GetNodeId(imtgql::IGqlResponse& reqponse, Id& out) const = 0;
	virtual bool GetOperationResult(imtgql::IGqlResponse& reqponse, bool& out) const = 0;
	virtual bool GetItemCount(imtgql::IGqlResponse& reqponse, bool out) const = 0;
	virtual bool GetItemIds(imtgql::IGqlResponse& reqponse, Ids& out) const = 0;
	virtual bool GetItemPath(imtgql::IGqlResponse& reqponse, Ids& out) const = 0;
	virtual bool GetItemInfos(imtgql::IGqlResponse& reqponse, imtbase::IHierarchicalStructureInfo::ItemInfoList& out) const = 0;
};


} // namespace imtclientgql


