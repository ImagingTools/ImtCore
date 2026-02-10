// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IHierarchicalStructure.h>
#include <imtclientgql/IGqlHierarchicalStructureDelegate.h>
#include <imtclientgql/IGqlClient.h>


namespace imtbase
{
	class IRevisionController;
}


namespace imtclientgql
{


class CGqlHierarchicalStructureComp:
			public QObject,
			public ilog::TLoggerCompWrap<icomp::CComponentBase>,
			virtual public imtbase::IHierarchicalStructure
{
	Q_OBJECT
public:
	typedef ilog::TLoggerCompWrap<icomp::CComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CGqlHierarchicalStructureComp);
		I_REGISTER_INTERFACE(IHierarchicalStructure);
		I_REGISTER_INTERFACE(IHierarchicalStructureInfo);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_clientCompPtr, "ApiClient", "GraphQL API client", true, "ApiClient");
		I_ASSIGN(m_delegateCompPtr, "GqlHierarchicalStructureDelegate", "GraphQL-based hierarchical structure delegate for database CRUD operations", true, "GqlObjectCollectionDelegate");
	I_END_COMPONENT;

	// reimplemented (IHierarchicalStructure)
	virtual Id InsertNewNode(
				const QString& name,
				const QString& description = QString(),
				const Id& proposedNodeId = Id(),
				const Id& parentNodeId = Id(),
				const idoc::IDocumentMetaInfo* metaInfoPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool SetNodeName(
				const Id& nodeId,
				const QString& name,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool SetNodeDescription(
				const Id& nodeId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool SetNodeMetaInfo(
				const Id& nodeId,
				const idoc::IDocumentMetaInfo& metaInfo,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool MoveNode(
				const Id& nodeId,
				const Id& parentNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveNode(
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;

	virtual bool InsertLeaf(
				const Id& leafId,
				const Id& nodeId,
				const QString& leafName,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool MoveLeaf(
				const Id& leafId,
				const Id& sourceNodeId,
				const Id& targetNodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;
	virtual bool RemoveLeaf(
				const Id& leafId,
				const Id& nodeId,
				const imtbase::IOperationContext* operationContextPtr = nullptr) override;

	// reimplemented (IHierarchicalStructureInfo)
	virtual int GetItemCount(const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual Ids GetItemIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
	virtual ItemInfoList GetItemInfos(const Ids& itemIds) override;
	virtual Ids GetItemPath(const Id& itemId) const override;
	virtual imtbase::IHierarchicalStructureIterator* CreateHierarchicalStructureIterator(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	// imtbase::IObjectCollection::DataPtr GetObject(const QByteArray& objectId, const QByteArray& typeId) const;
	// // bool GetElementType(const QByteArray& elementId, ElementType& valueOut) const;
	// bool GetItemInfo(const QByteArray& nodeId, imtbase::IHierarchicalStructureInfo::ItemInfo& valueOut) const;
	// bool GetObjectInfo(const QByteArray& objectId, imtgql::IGqlStructuredCollectionResponse::ObjectInfo& valueOut) const;
	// bool GetObjectMetaInfo(const QByteArray& objectId, idoc::MetaInfoPtr& valueOut) const;
	// bool GetObjectDataMetaInfo(const QByteArray& objectId, idoc::MetaInfoPtr& valueOut) const;

protected:
	I_REF(IGqlClient, m_clientCompPtr);
	I_REF(imtclientgql::IGqlHierarchicalStructureDelegate, m_delegateCompPtr);
};


} //namespace imtclientgql


