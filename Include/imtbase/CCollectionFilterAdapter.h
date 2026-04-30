// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/ICollectionFilter.h>
#include <imtbase/IComplexCollectionFilter.h>


namespace imtbase
{


/**
 * @brief Bridges legacy ICollectionFilter and IComplexCollectionFilter.
 *
 * CCollectionFilterAdapter provides static conversion helpers for code that
 * still operates on the old ICollectionFilter interface and code that
 * produces the richer IComplexCollectionFilter.
 *
 * Converting to the complex filter is lossless for the features exposed by
 * ICollectionFilter (text filter, sort column, sort order, object type ID).
 * Converting back is lossy because IComplexCollectionFilter may contain
 * structured field-filter trees and multiple sort columns that cannot be
 * represented in ICollectionFilter.
 *
 * Usage example:
 * @code{.cpp}
 * // Upgrade a legacy filter for use with new delegates
 * CComplexCollectionFilter complex =
 *     CCollectionFilterAdapter::ToComplexFilter(legacyFilter);
 *
 * // Downgrade for use with a legacy delegate
 * CCollectionFilter legacy;
 * CCollectionFilterAdapter::ToLegacyFilter(complexFilter, legacy);
 * @endcode
 */
class CCollectionFilterAdapter
{
public:
    /**
     * @brief Convert a legacy ICollectionFilter to a CComplexCollectionFilter.
     *
     * Mapping:
     * - GetTextFilter()       → SetTextFilter()
     * - GetFilteringInfoIds() → FieldInfo entries with SO_TEXT_FILTER flag
     * - GetSortingInfoIds()   → first entry becomes a FieldInfo with SO_SORT flag
     * - GetSortingOrder()     → sortingOrder of the sort FieldInfo
     *
     * @param legacyFilter Source legacy filter.
     * @return Populated CComplexCollectionFilter.
     */
    static CComplexCollectionFilter ToComplexFilter(const ICollectionFilter& legacyFilter);

    /**
     * @brief Convert an IComplexCollectionFilter back to a legacy filter.
     *
     * Only the first text-filter field and the first sort field are mapped
     * because ICollectionFilter's API does not support richer structures.
     * The object type ID is left empty as it is not part of
     * IComplexCollectionFilter.
     *
     * @param complexFilter Source complex filter.
     * @param legacyFilter  Output legacy filter (must be a concrete implementation,
     *                      e.g. CCollectionFilter).
     */
    static void ToLegacyFilter(
        const IComplexCollectionFilter& complexFilter,
        ICollectionFilter& legacyFilter);
};


} // namespace imtbase


