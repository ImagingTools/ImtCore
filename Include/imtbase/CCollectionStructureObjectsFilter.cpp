#include <imtbase/CCollectionStructureObjectsFilter.h>


namespace imtbase
{


CCollectionStructureObjectsFilter::CCollectionStructureObjectsFilter(
			const QByteArray& nodeId,
			SortingAttribute sortingAttribute,
			SortingDirection sortingDirection)
	:m_nodeId(nodeId),
	m_sortingAttribute(sortingAttribute),
	m_sortingDirection(sortingDirection)
{
}


QByteArray CCollectionStructureObjectsFilter::GetNodeId()
{
	return QByteArray();
}


CCollectionStructureObjectsFilter::SortingAttribute CCollectionStructureObjectsFilter::GetSortingAttribute()
{
	return m_sortingAttribute;
}


CCollectionStructureObjectsFilter::SortingDirection CCollectionStructureObjectsFilter::GetSortingDirection()
{
	return m_sortingDirection;
}


} // namespace imtbase


