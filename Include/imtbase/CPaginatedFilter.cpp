// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CPaginatedFilter.h>


// Qt includes
#include <QtCore/QtGlobal>


namespace imtbase
{


CPaginatedFilter::CPaginatedFilter()
{
    SyncFilterPagination();
}


CPaginatedFilter::CPaginatedFilter(const CFilter& filter, int page, int pageSize)
    : m_filter(filter)
    , m_page(qMax(1, page))
    , m_pageSize(qMax(1, pageSize))
{
    SyncFilterPagination();
}


const CFilter& CPaginatedFilter::GetFilter() const
{
    return m_filter;
}


void CPaginatedFilter::SetFilter(const CFilter& filter)
{
    m_filter = filter;
    SyncFilterPagination();
}


int CPaginatedFilter::GetPage() const
{
    return m_page;
}


void CPaginatedFilter::SetPage(int page)
{
    m_page = qMax(1, page);
    SyncFilterPagination();
}


int CPaginatedFilter::GetPageSize() const
{
    return m_pageSize;
}


void CPaginatedFilter::SetPageSize(int pageSize)
{
    m_pageSize = qMax(1, pageSize);
    SyncFilterPagination();
}


int CPaginatedFilter::GetOffset() const
{
    return (m_page - 1) * m_pageSize;
}


int CPaginatedFilter::GetLimit() const
{
    return m_pageSize;
}


int CPaginatedFilter::GetTotalCount() const
{
    return m_totalCount;
}


void CPaginatedFilter::SetTotalCount(int count)
{
    m_totalCount = count;
}


int CPaginatedFilter::GetTotalPages() const
{
    if (m_totalCount < 0){
        return -1;
    }
    return (m_totalCount + m_pageSize - 1) / m_pageSize;
}


bool CPaginatedFilter::HasNextPage() const
{
    const int totalPages = GetTotalPages();
    return totalPages >= 0 && m_page < totalPages;
}


bool CPaginatedFilter::HasPreviousPage() const
{
    return m_page > 1;
}


void CPaginatedFilter::SyncFilterPagination()
{
    m_filter.SetWindow(GetOffset(), GetLimit());
}


} // namespace imtbase


