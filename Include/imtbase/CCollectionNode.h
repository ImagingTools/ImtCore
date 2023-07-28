#pragma once


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtbase/ICollectionNode.h>
#include <imtbase/ICollectionInfo.h>


namespace imtbase
{


class CCollectionNode: virtual public ICollectionNode
{
public:
	CCollectionNode(
				const QByteArray& nodeId,
				const QByteArray& parentNodeId,
				ICollectionInfo& nodeCollection,
				ICollectionInfo& contentCollection);

	// reimplemented (ICollectionNode)
	virtual QByteArray GetNodeId() const override;
	virtual QByteArray GetParentNodeId() const override;
	virtual const ICollectionInfo& GetChildNodes() const override;
	virtual const ICollectionInfo& GetObjects() const override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	class CollectionInfoProxy: virtual public imtbase::ICollectionInfo
	{
	public:
		CollectionInfoProxy() = delete;
		CollectionInfoProxy(
					const ICollectionInfo& collectionInfo,
					const Id& nodeId);

		// reimplemented (imtbase::ICollectionInfo)
		virtual int GetElementsCount(const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
		virtual Ids GetElementIds(
					int offset = 0,
					int count = -1,
					const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
		virtual bool GetSubsetInfo(
					ICollectionInfo& subsetInfo,
					int offset = 0,
					int count = -1,
					const iprm::IParamsSet* selectionParamsPtr = nullptr) const override;
		virtual QVariant GetElementInfo(const Id& elementId, int infoType) const override;
		virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId) const override;
		virtual bool SetElementName(const Id& elementId, const QString& name) override;
		virtual bool SetElementDescription(const Id& elementId, const QString& description) override;
		virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true) override;

	private:
		const ICollectionInfo& m_collectionInfo;

		iprm::CParamsSet m_paramsSet;
		iprm::CIdParam m_nodeId;
	};

private:
	Id m_nodeId;

	CollectionInfoProxy m_nodeCollectioInfo;
	CollectionInfoProxy m_objectCollectionInfo;
};


} // namespace imtbase


