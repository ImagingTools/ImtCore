#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ImtCore includes
#include <imtclientgql/CGqlStructureDelegateCompBase.h>


namespace imtclientgql
{


class CGqlStructureDelegateComp:
			public CGqlStructureDelegateCompBase
{
public:
	typedef CGqlStructureDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlStructureDelegateComp);
	I_END_COMPONENT;

protected:
	virtual imtgql::IGqlRequest* CreateInsertNewNodeRequest(
				const QString& name,
				const QString& description = QString(),
				const Id& proposedNodeId = Id(),
				const Id& parentNodeId = Id(),
				const idoc::IDocumentMetaInfo* metaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual imtgql::IGqlRequest* CreateSetNodeNameRequest(
				const Id& nodeId,
				const QString& name,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual imtgql::IGqlRequest* CreateSetNodeDescriptionRequest(
				const Id& nodeId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual imtgql::IGqlRequest* CreateSetNodeMetaInfoRequest(
				const Id& nodeId,
				const idoc::IDocumentMetaInfo& metaInfo,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual imtgql::IGqlRequest* CreateMoveNodeRequest(
				const Id& nodeId,
				const Id& parentNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual imtgql::IGqlRequest* CreateRemoveNodeRequest(
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;

	virtual imtgql::IGqlRequest* CreateInsertNewObjectRequest(
				const Id& objectId,
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual imtgql::IGqlRequest* CreateMoveObjectRequest(
				const Id& objectId,
				const Id& sourceNodeId,
				const Id& targetNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual imtgql::IGqlRequest* CreateRemoveObjectRequest(
				const Id& objectId,
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;

	virtual imtgql::IGqlRequest* CreateGetNodeCountRequest(const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateGetNodeIdsRequest(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual imtgql::IGqlRequest* CreateGetNodeInfoRequest(const Id& nodeId) override;

	virtual imtgql::IGqlRequest* CreateGetObjectParentNodeIdsRequest(const Id& objectId) const override;

	virtual imtgql::IGqlResponse* CreateResponse(const imtgql::IGqlRequest& request) const override;
};


} // namespace imtclientgql


