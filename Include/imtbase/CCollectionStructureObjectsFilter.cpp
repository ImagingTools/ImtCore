// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


