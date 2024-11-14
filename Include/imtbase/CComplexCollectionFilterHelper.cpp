#include <imtbase/CComplexCollectionFilterHelper.h>



namespace imtbase
{


IComplexCollectionFilter::SortingOrder CComplexCollectionFilterHelper::GetSortingOrder(const IComplexCollectionFilter& filter, const QByteArrayList& fieldIds)
{
	for (const IComplexCollectionFilter::FieldSortingInfo& info : filter.GetSortingInfo()){
		if (fieldIds.contains(info.fieldId) || fieldIds.isEmpty()){
			return info.sortingOrder;
		}
	}

	return IComplexCollectionFilter::SO_NO_ORDER;
}


void CComplexCollectionFilterHelper::SetSortingOrder(IComplexCollectionFilter& filter, const QByteArrayList& fieldIds, IComplexCollectionFilter::SortingOrder order)
{
	IComplexCollectionFilter::FieldSortingInfoList infoList;

	for (const QByteArray& id : fieldIds){
		IComplexCollectionFilter::FieldSortingInfo info;

		info.fieldId = id;
		info.sortingOrder = order;

		infoList.append(info);
	}

	filter.SetSortingInfo(infoList);
}


QSet<QByteArray> CComplexCollectionFilterHelper::GetSortingFieldIds(const IComplexCollectionFilter& filter)
{
	QSet<QByteArray> retVal;

	for (const IComplexCollectionFilter::FieldSortingInfo& info : filter.GetSortingInfo()){
		retVal += info.fieldId;
	}

	return retVal;
}


QSet<QByteArray> CComplexCollectionFilterHelper::GetFilteringFieldIds(const IComplexCollectionFilter::GroupFilter& filter)
{
	QSet<QByteArray> retVal;

	for (const IComplexCollectionFilter::FieldFilter& fieldFilter : filter.fieldFilters){
		retVal += fieldFilter.fieldId;
	}

	for (const IComplexCollectionFilter::GroupFilter& groupFilter : filter.groupFilters){
		retVal += GetFilteringFieldIds(groupFilter);
	}

	return retVal;
}


QString CComplexCollectionFilterHelper::GetTextFilter(const imtbase::IComplexCollectionFilter::GroupFilter& filter)
{
	for (const IComplexCollectionFilter::FieldFilter& fieldFilter : filter.fieldFilters){
		return fieldFilter.filterValue.toString();
	}

	for (const IComplexCollectionFilter::GroupFilter& groupFilter : filter.groupFilters){
		QString retVal = GetTextFilter(groupFilter);

		if (!retVal.isEmpty()){
			return retVal;
		}
	}

	return QString();
}


void CComplexCollectionFilterHelper::SetTextFilter(imtbase::IComplexCollectionFilter& filter, const QByteArrayList& fieldIds, const QString& text)
{
	imtbase::IComplexCollectionFilter::GroupFilter groupFilter;

	QVector<imtbase::IComplexCollectionFilter::FieldFilter> fieldFilters;
	for (const QByteArray& fieldId : fieldIds){
		imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;

		fieldFilter.fieldId = fieldId;
		fieldFilter.filterValue = text;
		fieldFilter.filterOperation = IComplexCollectionFilter::FO_CONTAINS;

		fieldFilters.append(fieldFilter);
	}
	groupFilter.fieldFilters = fieldFilters;

	filter.SetFieldsFilter(groupFilter);
}


} // namespace imtbase


