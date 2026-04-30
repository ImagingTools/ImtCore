// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CCollectionFilterAdapter.h>


namespace imtbase
{


CComplexCollectionFilter CCollectionFilterAdapter::ToComplexFilter(const ICollectionFilter& legacyFilter)
{
    CComplexCollectionFilter result;

    // Text filter
    result.SetTextFilter(legacyFilter.GetTextFilter());

    IComplexCollectionFilter::Fields fields;

    // Text-filter column IDs → FieldInfo with SO_TEXT_FILTER flag
    const QByteArrayList filteringIds = legacyFilter.GetFilteringInfoIds();
    for (const QByteArray& id : filteringIds){
        IComplexCollectionFilter::FieldInfo info(id);
        info.metaInfo.flags = IComplexCollectionFilter::SO_TEXT_FILTER;
        fields << info;
    }

    // Sort column → FieldInfo with SO_SORT flag
    const QByteArrayList sortingIds = legacyFilter.GetSortingInfoIds();
    if (!sortingIds.isEmpty()){
        const QByteArray& sortId = sortingIds.first();

        IComplexCollectionFilter::SortingOrder order = IComplexCollectionFilter::SO_NO_ORDER;
        switch (legacyFilter.GetSortingOrder()){
        case ICollectionFilter::SO_ASC:
            order = IComplexCollectionFilter::SO_ASC;
            break;
        case ICollectionFilter::SO_DESC:
            order = IComplexCollectionFilter::SO_DESC;
            break;
        default:
            break;
        }

        // Merge into an existing FieldInfo if the id is already present
        bool merged = false;
        for (IComplexCollectionFilter::FieldInfo& existing : fields){
            if (existing.id == sortId){
                existing.metaInfo.flags |= IComplexCollectionFilter::SO_SORT;
                existing.metaInfo.sortingOrder = order;
                merged = true;
                break;
            }
        }
        if (!merged){
            IComplexCollectionFilter::FieldInfo info(sortId);
            info.metaInfo.flags = IComplexCollectionFilter::SO_SORT;
            info.metaInfo.sortingOrder = order;
            fields << info;
        }
    }

    result.SetFields(fields);

    return result;
}


void CCollectionFilterAdapter::ToLegacyFilter(
    const IComplexCollectionFilter& complexFilter,
    ICollectionFilter& legacyFilter)
{
    // Text filter
    legacyFilter.SetTextFilter(complexFilter.GetTextFilter());

    // Text-filter columns (all fields with SO_TEXT_FILTER flag)
    QByteArrayList filteringIds;
    for (const IComplexCollectionFilter::FieldInfo& info : complexFilter.GetFields()){
        if (info.metaInfo.flags & IComplexCollectionFilter::SO_TEXT_FILTER){
            filteringIds << info.id;
        }
    }
    legacyFilter.SetFilteringInfoIds(filteringIds);

    // Sort info (first SO_SORT field only — ICollectionFilter is single-column)
    QByteArrayList sortingIds;
    ICollectionFilter::SortingOrder legacySortOrder = ICollectionFilter::SO_NO_ORDER;
    for (const IComplexCollectionFilter::FieldInfo& info : complexFilter.GetFields()){
        if (!(info.metaInfo.flags & IComplexCollectionFilter::SO_SORT)){
            continue;
        }
        if (info.metaInfo.sortingOrder == IComplexCollectionFilter::SO_NO_ORDER){
            continue;
        }
        sortingIds << info.id;
        switch (info.metaInfo.sortingOrder){
        case IComplexCollectionFilter::SO_ASC:
            legacySortOrder = ICollectionFilter::SO_ASC;
            break;
        case IComplexCollectionFilter::SO_DESC:
            legacySortOrder = ICollectionFilter::SO_DESC;
            break;
        default:
            break;
        }
        break; // Only the first sort field
    }
    legacyFilter.SetSortingInfoIds(sortingIds);
    legacyFilter.SetSortingOrder(legacySortOrder);
}


} // namespace imtbase


