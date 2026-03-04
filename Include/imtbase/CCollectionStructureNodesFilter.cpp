// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CCollectionStructureNodesFilter.h>


namespace imtbase
{


CCollectionNodesFilter::CCollectionNodesFilter(
			FilterMode filterMode,
			SortingAttribute sortingAttribute,
			SortingDirection sortingDirection)
	:m_mode(filterMode),
	m_sortingAttribute(sortingAttribute),
	m_sortingDirection(sortingDirection)
{
}


CCollectionNodesFilter::FilterMode CCollectionNodesFilter::GetFilterMode()
{
	return m_mode;
}



CCollectionNodesFilter::SortingAttribute CCollectionNodesFilter::GetSortingAttribute()
{
	return m_sortingAttribute;
}


CCollectionNodesFilter::SortingDirection CCollectionNodesFilter::GetSortingDirection()
{
	return m_sortingDirection;
}


} // namespace imtbase


