#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ImtCore includes
#include <imtdb/CSqlStructureDelegateCompBase.h>
#include <imtbase/ICollectionFilter.h>
#include <imtbase/IMetaInfoCreator.h>


namespace imtdb
{


class CSqlStructureDelegateComp:
			public CSqlStructureDelegateCompBase
{
public:
	typedef CSqlStructureDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CSqlStructureDelegateComp);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
		I_ASSIGN(m_metaInfoCreatorCompPtr, "MetaInfoCreator", "Meta-info creator", true, "MetaInfoCreator");
	I_END_COMPONENT;

protected:
	virtual QByteArray CreateInsertNewNodeQuery(
				const QString& name,
				const QString& description = QString(),
				const Id& proposedNodeId = Id(),
				const Id& parentNodeId = Id(),
				const idoc::IDocumentMetaInfo* metaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual QByteArray CreateSetNodeNameQuery(
				const Id& nodeId,
				const QString& name,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual QByteArray CreateSetNodeDescriptionQuery(
				const Id& nodeId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual QByteArray CreateSetNodeMetaInfoQuery(
				const Id& nodeId,
				const idoc::IDocumentMetaInfo *metaInfoPtr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual QByteArray CreateMoveNodeQuery(
				const Id& nodeId,
				const Id& parentNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual QByteArray CreateRemoveNodeQuery(
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;

	virtual QByteArray CreateInsertNewObjectQuery(
				const Id& objectId,
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual QByteArray CreateMoveObjectQuery(
				const Id& objectId,
				const Id& sourceNodeId,
				const Id& targetNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual QByteArray CreateRemoveObjectQuery(
				const Id& objectId,
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;

	virtual QByteArray CreateGetNodeCountQuery(const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual QByteArray CreateGetNodeIdsQuery(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual QByteArray CreateGetObjectIdsQuery(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual QByteArray CreateGetNodeInfoQuery(const Id& nodeId) const override;
	virtual QByteArray CreateGetNodePathQuery(const Id& nodeId) const override;
	virtual QByteArray CreateGetObjectParentNodeIdsQuery(const Id& objectId) const override;

private:
	virtual QJsonArray CtreateJsonFromMetaInfo(const idoc::IDocumentMetaInfo* metaInfoPtr);
	virtual bool CreatePaginationQuery(int offset, int count, QByteArray& paginationQuery) const;
	virtual bool CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const;
	virtual bool CreateObjectFilterQuery(
				const iprm::IParamsSet& filterParams,
				QString& filterQuery) const;
	virtual bool CreateTextFilterQuery(
				const imtbase::ICollectionFilter& collectionFilter,
				QString& textFilterQuery) const;
	virtual bool CreateSortQuery(
				const imtbase::ICollectionFilter& collectionFilter,
				QString& sortQuery) const;
protected:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(imtbase::IMetaInfoCreator, m_metaInfoCreatorCompPtr);

};


} // namespace imtdb


