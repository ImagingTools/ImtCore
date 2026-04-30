// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CPaginatedFilter.h>


// Qt includes
#include <QtCore/QtGlobal>


namespace imtbase
{


// public methods

CPaginatedFilter::CPaginatedFilter()
    : m_page(1)
    , m_pageSize(50)
    , m_totalCount(-1)
{
}


CPaginatedFilter::CPaginatedFilter(const IComplexCollectionFilter& filter, int page, int pageSize)
    : m_page(qMax(1, page))
    , m_pageSize(qMax(1, pageSize))
    , m_totalCount(-1)
{
    m_filter.CopyFrom(filter);
}


const IComplexCollectionFilter& CPaginatedFilter::GetFilter() const
{
    return m_filter;
}


void CPaginatedFilter::SetFilter(const IComplexCollectionFilter& filter)
{
    m_filter.CopyFrom(filter);
}


int CPaginatedFilter::GetPage() const
{
    return m_page;
}


void CPaginatedFilter::SetPage(int page)
{
    m_page = qMax(1, page);
}


int CPaginatedFilter::GetPageSize() const
{
    return m_pageSize;
}


void CPaginatedFilter::SetPageSize(int pageSize)
{
    m_pageSize = qMax(1, pageSize);
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
    if (m_pageSize <= 0){
        return 0;
    }
    return (m_totalCount + m_pageSize - 1) / m_pageSize;
}


bool CPaginatedFilter::HasNextPage() const
{
    const int totalPages = GetTotalPages();
    if (totalPages < 0){
        return false;
    }
    return m_page < totalPages;
}


bool CPaginatedFilter::HasPreviousPage() const
{
    return m_page > 1;
}


} // namespace imtbase


