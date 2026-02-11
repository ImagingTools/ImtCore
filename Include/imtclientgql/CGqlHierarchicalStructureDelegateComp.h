// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IHierarchicalStructureIterator.h>
#include <imtclientgql/IGqlHierarchicalStructureDelegate.h>


namespace imtclientgql
{


class CGqlHierarchicalStructureDelegateComp:
			public icomp::CComponentBase,
			virtual public IGqlHierarchicalStructureDelegate
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlHierarchicalStructureDelegateComp);
	I_END_COMPONENT;

public:
	// reimplemented (IGqlHierarchicalStructureDelegate)
	virtual imtgql::IGqlRequest* CreateInsertNewNodeRequest(
				const QString& name,
				const QString& description = QString(),
				const Id& proposedNodeId = Id(),
				const Id& parentNodeId = Id(),
				const idoc::IDocumentMetaInfo* metaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateSetNodeNameRequest(
				const Id& nodeId,
				const QString& name,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateSetNodeDescriptionRequest(
				const Id& nodeId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateSetNodeMetaInfoRequest(
				const Id& nodeId,
				const idoc::IDocumentMetaInfo& metaInfo,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateMoveNodeRequest(
				const Id& nodeId,
				const Id& parentNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateRemoveNodeRequest(
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

	virtual imtgql::IGqlRequest* CreateInsertLeafRequest(
				const Id& leafId,
				const Id& nodeId,
				const QString& leafName,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateMoveLeafRequest(
				const Id& leafId,
				const Id& sourceNodeId,
				const Id& targetNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateRemoveLeafRequest(
				const Id& leafId,
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

	virtual imtgql::IGqlRequest* CreateGetItemCountRequest(
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateGetItemIdsRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;

	virtual imtgql::IGqlRequest* CreateGetItemInfosRequest(const Ids& itemIds) const override;
	virtual imtgql::IGqlRequest* CreateGetItemPathRequest(const Id& itemId) const override;

	virtual imtbase::IHierarchicalStructureIterator* CreateHierarchicalStructureIterator(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;

	virtual bool IsValid(imtgql::IGqlResponse& reqponse) const override;
	virtual bool GetNodeId(imtgql::IGqlResponse& reqponse, Id& out) const override;
	virtual bool GetOperationResult(imtgql::IGqlResponse& reqponse, bool& out) const override;
	virtual bool GetItemCount(imtgql::IGqlResponse& reqponse, bool out) const override;
	virtual bool GetItemIds(imtgql::IGqlResponse& reqponse, Ids& out) const override;
	virtual bool GetItemPath(imtgql::IGqlResponse& reqponse, Ids& out) const override;
	virtual bool GetItemInfos(imtgql::IGqlResponse& reqponse, imtbase::IHierarchicalStructureInfo::ItemInfoList& out) const override;
};


} // namespace imtclientgql


