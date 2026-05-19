// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/CFilter.h>


namespace imtbase
{


/**
 * @brief Standalone pagination wrapper for CFilter.
 */
class CPaginatedFilter
{
public:
    CPaginatedFilter();
    explicit CPaginatedFilter(const CFilter& filter, int page = 1, int pageSize = 50);

    const CFilter& GetFilter() const;
    void SetFilter(const CFilter& filter);

    int GetPage() const;
    void SetPage(int page);

    int GetPageSize() const;
    void SetPageSize(int pageSize);

    int GetOffset() const;
    int GetLimit() const;

    int GetTotalCount() const;
    void SetTotalCount(int count);
    int GetTotalPages() const;
    bool HasNextPage() const;
    bool HasPreviousPage() const;

private:
    void SyncFilterPagination();

private:
    CFilter m_filter;
    int m_page = 1;
    int m_pageSize = 50;
    int m_totalCount = -1;
};


} // namespace imtbase


