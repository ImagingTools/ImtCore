// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CComplexCollectionFilterHelper.h>



namespace imtbase
{


IComplexCollectionFilter::SortingOrder CComplexCollectionFilterHelper::GetSortingOrder(const IComplexCollectionFilter& filter, const QByteArrayList& fieldIds)
{
	for (const IComplexCollectionFilter::FieldInfo& info : filter.GetFields()){
		if (fieldIds.contains(info.id) || fieldIds.isEmpty()){
			return info.metaInfo.sortingOrder;
		}
	}

	return IComplexCollectionFilter::SO_NO_ORDER;
}


void CComplexCollectionFilterHelper::SetSortingOrder(IComplexCollectionFilter& filter, const QByteArrayList& fieldIds, IComplexCollectionFilter::SortingOrder order)
{
	for (const QByteArray& id : fieldIds){
		IComplexCollectionFilter::FieldInfo* infoPtr = filter.GetEditableFieldInfo(id);
		if (infoPtr != nullptr){
			if (!(infoPtr->metaInfo.flags & IComplexCollectionFilter::SO_SORT)){
				infoPtr->metaInfo.flags &= IComplexCollectionFilter::SO_SORT;
			}

			infoPtr->metaInfo.sortingOrder = order;
		}
	}
}


QSet<QByteArray> CComplexCollectionFilterHelper::GetSortingFieldIds(const IComplexCollectionFilter& filter)
{
	QSet<QByteArray> retVal;

	for (const IComplexCollectionFilter::FieldInfo& info : filter.GetFields()){
		if (info.metaInfo.flags & IComplexCollectionFilter::SO_SORT && info.metaInfo.sortingOrder != IComplexCollectionFilter::SO_NO_ORDER){
			retVal += info.id;
		}
	}

	return retVal;
}


QSet<QByteArray> CComplexCollectionFilterHelper::GetFilteringFieldIds(const IComplexCollectionFilter::FilterExpression& filter)
{
	QSet<QByteArray> retVal;

	for (const IComplexCollectionFilter::FieldFilter& fieldFilter : filter.fieldFilters){
		retVal += fieldFilter.fieldId;
	}

	for (const IComplexCollectionFilter::FilterExpression& groupFilter : filter.filterExpressions){
		retVal += GetFilteringFieldIds(groupFilter);
	}

	return retVal;
}


QString CComplexCollectionFilterHelper::GetTextFilter(const imtbase::IComplexCollectionFilter::FilterExpression& filter)
{
	for (const IComplexCollectionFilter::FilterExpression& groupFilter : filter.filterExpressions){
		QString retVal = GetTextFilter(groupFilter);

		if (!retVal.isEmpty()){
			return retVal;
		}
	}

	return QString();
}


void CComplexCollectionFilterHelper::SetTextFilter(imtbase::IComplexCollectionFilter& filter, const QByteArrayList& fieldIds, const QString& text)
{
	for (const QByteArray& id : fieldIds){
		IComplexCollectionFilter::FieldInfo* infoPtr = filter.GetEditableFieldInfo(id);
		if (infoPtr != nullptr){
			if (!(infoPtr->metaInfo.flags & IComplexCollectionFilter::SO_TEXT_FILTER)){
				infoPtr->metaInfo.flags &= IComplexCollectionFilter::SO_TEXT_FILTER;
			}
		}
	}

	filter.SetTextFilter(text);
}


void CComplexCollectionFilterHelper::FillTextFilter(imtbase::IComplexCollectionFilter::FilterExpression& groupFilter, const QByteArrayList& fieldIds, const QString& text)
{
	QVector<imtbase::IComplexCollectionFilter::FieldFilter> fieldFilters;
	for (const QByteArray& fieldId : fieldIds){
		imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;

		fieldFilter.fieldId = fieldId;
		fieldFilter.filterValue = text;
		fieldFilter.filterOperation = IComplexCollectionFilter::FO_CONTAINS;

		fieldFilters.append(fieldFilter);
	}

	groupFilter.fieldFilters = fieldFilters;
	groupFilter.logicalOperation = imtbase::IComplexCollectionFilter::LO_OR;
}

} // namespace imtbase


