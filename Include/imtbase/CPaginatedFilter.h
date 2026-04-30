// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/IComplexCollectionFilter.h>


namespace imtbase
{


/**
 * @brief Wraps an IComplexCollectionFilter with pagination support.
 *
 * CPaginatedFilter combines a collection filter with page/page-size state so
 * that pagination information travels alongside the filter through the same
 * call chain without changing existing delegate or collection interfaces.
 *
 * The underlying filter is stored by value as a CComplexCollectionFilter and
 * populated from any IComplexCollectionFilter via CopyFrom.
 *
 * Usage example:
 * @code{.cpp}
 * CPaginatedFilter pager;
 * pager.SetFilter(complexFilter);
 * pager.SetPage(2);
 * pager.SetPageSize(50);
 *
 * // After executing the count query:
 * pager.SetTotalCount(312);
 * qDebug() << "Total pages:" << pager.GetTotalPages();    // 7
 * qDebug() << "Has next:"    << pager.HasNextPage();       // true
 * qDebug() << "LIMIT:"       << pager.GetLimit();          // 50
 * qDebug() << "OFFSET:"      << pager.GetOffset();         // 50
 * @endcode
 */
class CPaginatedFilter
{
public:
    /**
     * @brief Constructs a CPaginatedFilter with page 1 and page size 50.
     */
    CPaginatedFilter();

    /**
     * @brief Constructs a CPaginatedFilter by copying from an existing filter.
     *
     * @param filter   Source filter.
     * @param page     Current page (1-based). Clamped to >= 1.
     * @param pageSize Number of records per page. Clamped to >= 1.
     */
    explicit CPaginatedFilter(const IComplexCollectionFilter& filter, int page = 1, int pageSize = 50);

    // -----------------------------------------------------------------------
    // Filter access
    // -----------------------------------------------------------------------

    /**
     * @return The contained complex filter.
     */
    const IComplexCollectionFilter& GetFilter() const;

    /**
     * @brief Replace the contained filter with a copy of @p filter.
     */
    void SetFilter(const IComplexCollectionFilter& filter);

    // -----------------------------------------------------------------------
    // Pagination
    // -----------------------------------------------------------------------

    /**
     * @return Current page number (1-based).
     */
    int GetPage() const;

    /**
     * @brief Set the current page.  Values < 1 are clamped to 1.
     */
    void SetPage(int page);

    /**
     * @return Number of records per page.
     */
    int GetPageSize() const;

    /**
     * @brief Set the page size.  Values < 1 are clamped to 1.
     */
    void SetPageSize(int pageSize);

    /**
     * @return SQL OFFSET value: (page - 1) * pageSize.
     */
    int GetOffset() const;

    /**
     * @return SQL LIMIT value (alias for GetPageSize()).
     */
    int GetLimit() const;

    // -----------------------------------------------------------------------
    // Total-count tracking (set after executing the COUNT query)
    // -----------------------------------------------------------------------

    /**
     * @return Total number of records matching the filter, or -1 if not set.
     */
    int GetTotalCount() const;

    /**
     * @brief Store the total record count returned by the COUNT query.
     */
    void SetTotalCount(int count);

    /**
     * @return Total number of pages, or -1 if total count has not been set.
     */
    int GetTotalPages() const;

    /**
     * @return @c true when a next page exists.
     */
    bool HasNextPage() const;

    /**
     * @return @c true when a previous page exists.
     */
    bool HasPreviousPage() const;

private:
    CComplexCollectionFilter m_filter;
    int m_page     = 1;
    int m_pageSize = 50;
    int m_totalCount = -1;
};


} // namespace imtbase


