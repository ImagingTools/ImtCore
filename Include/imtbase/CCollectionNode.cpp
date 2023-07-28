#include <imtbase/CCollectionNode.h>


// ImtCore includes
#include <imtbase/CParamsSetJoiner.h>
#include <imtbase/CCollectionStructureNodesFilter.h>


namespace imtbase
{


CCollectionNode::CCollectionNode(
			const QByteArray& nodeId,
			const QByteArray& parentNodeId,
			ICollectionInfo& nodeCollection,
			ICollectionInfo& contentCollection)
	:m_nodeId(nodeId),
	m_nodeCollectioInfo(nodeCollection, nodeId),
	m_objectCollectionInfo(contentCollection, nodeId)
{
}


// reimplemented (imtbase::TIIdBasedHierarchical)

ICollectionNode::Id CCollectionNode::GetNodeId() const
{
	return m_nodeId;
}


ICollectionNode::Id CCollectionNode::GetParentNodeId() const
{
	CCollectionNodesFilter filter(
				CCollectionNodesFilter::FM_PARENT,
				CCollectionNodesFilter::SM_NAME,
				CCollectionNodesFilter::SD_ASCENDING);

	iprm::CParamsSet filterSet;
	filterSet.SetEditableParameter("Filter", &filter);

	Ids ids = m_nodeCollectioInfo.GetElementIds(0, -1, &filterSet);
	Q_ASSERT(ids.count() < 2);
	if (ids.count() == 1) {
		return ids[0];
	}

	return Id();
}


const ICollectionInfo& CCollectionNode::GetChildNodes() const
{
	return m_nodeCollectioInfo;
}


// reimplemented (ICollectionNode)

const ICollectionInfo& CCollectionNode::GetObjects() const
{
	return m_objectCollectionInfo;
}


bool CCollectionNode::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	return false;
}


// public methods of the embedded class CollectionInfoProxy

CCollectionNode::CollectionInfoProxy::CollectionInfoProxy(
			const ICollectionInfo& collectionInfo,
			const Id& nodeId)
	:m_collectionInfo(collectionInfo)
{
	m_nodeId.SetId(nodeId);
	m_paramsSet.SetEditableParameter("NodeId", &m_nodeId);
}


// reimplemented (imtbase::ICollectionInfo)

int CCollectionNode::CollectionInfoProxy::GetElementsCount(const iprm::IParamsSet* selectionParamsPtr) const
{
	CParamsSetJoiner joinedParamsSet(&m_paramsSet, selectionParamsPtr);

	return m_collectionInfo.GetElementsCount(&joinedParamsSet);
}


ICollectionInfo::Ids CCollectionNode::CollectionInfoProxy::GetElementIds(int offset, int count, const iprm::IParamsSet* selectionParamsPtr) const
{
	CParamsSetJoiner joinedParamsSet(&m_paramsSet, selectionParamsPtr);

	return m_collectionInfo.GetElementIds(
				offset,
				count,
				&joinedParamsSet);
}


bool CCollectionNode::CollectionInfoProxy::GetSubsetInfo(
				ICollectionInfo& subsetInfo,
				int offset,
				int count,
				const iprm::IParamsSet* selectionParamsPtr) const
{
	CParamsSetJoiner joinedParamsSet(&m_paramsSet, selectionParamsPtr);

	return m_collectionInfo.GetSubsetInfo(
				subsetInfo,
				offset,
				count,
				&joinedParamsSet);
}


QVariant CCollectionNode::CollectionInfoProxy::GetElementInfo(const Id& elementId, int infoType) const
{
	return m_collectionInfo.GetElementInfo(elementId, infoType);
}


idoc::MetaInfoPtr CCollectionNode::CollectionInfoProxy::GetElementMetaInfo(const Id& elementId) const
{
	return m_collectionInfo.GetElementMetaInfo(elementId);
}


bool CCollectionNode::CollectionInfoProxy::SetElementName(const Id& elementId, const QString& name)
{
	return false;
}


bool CCollectionNode::CollectionInfoProxy::SetElementDescription(const Id& elementId, const QString& description)
{
	return false;
}


bool CCollectionNode::CollectionInfoProxy::SetElementEnabled(const Id& elementId, bool isEnabled)
{
	return true;
}


} // namespace imtbase


