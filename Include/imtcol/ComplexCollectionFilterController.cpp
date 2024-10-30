#include <imtcol/ComplexCollectionFilterController.h>



// ImtCore includes
#include <imtbase/CTimeFilterParam.h>


namespace imtcol
{


bool ProcessFieldFilter(
	const sdl::imtbase::ComplexCollectionFilter::V1_0::CFieldFilter& source,
	imtbase::IComplexCollectionFilter::FieldFilter& target)
{
	bool retVal = true;
	bool isOk = true;

	target.fieldId = source.GetFieldId();
	QString value = source.GetFilterValue();
	if (source.GetFilterValueType() == "signed"){
		target.filterValue = value.toLongLong(&isOk);
	}
	else if (source.GetFilterValueType() == "unsigned"){
		target.filterValue = value.toULongLong(&isOk);
	}
	else if (source.GetFilterValueType() == "floatingPoint"){
		target.filterValue = value.toDouble(&isOk);
	}
	else if (source.GetFilterValueType() == "string"){
		target.filterValue = value;
	}
	else if (source.GetFilterValueType() == "bool"){
		if (value == "true"){
			target.filterValue = true;
		}
		else if (value == "false"){
			target.filterValue = false;
		}
		else{
			isOk = false;
		}
	}
	else{
		retVal = false;
	}

	retVal = retVal && isOk;

	if (retVal){
		if (source.GetFilterOperation() == "=="){
			target.filterOperation = imtbase::IComplexCollectionFilter::FO_EQUAL;
		}
		else if (source.GetFilterOperation() == "!="){
			target.filterOperation = imtbase::IComplexCollectionFilter::FO_NOT_EQUAL;
		}
		else if (source.GetFilterOperation() == "<"){
			target.filterOperation = imtbase::IComplexCollectionFilter::FO_LESS;
		}
		else if (source.GetFilterOperation() == ">"){
			target.filterOperation = imtbase::IComplexCollectionFilter::FO_GREATER;
		}
		else if (source.GetFilterOperation() == "<="){
			target.filterOperation = imtbase::IComplexCollectionFilter::FO_NOT_GREATER;
		}
		else if (source.GetFilterOperation() == ">="){
			target.filterOperation = imtbase::IComplexCollectionFilter::FO_NOT_LESS;
		}
		else if (source.GetFilterOperation() == "CONTAINS"){
			target.filterOperation = imtbase::IComplexCollectionFilter::FO_CONTAINS;
		}
		else{
			retVal = false;
		}
	}

	return retVal;
}


bool ProcessGroupFilter(
	const sdl::imtbase::ComplexCollectionFilter::V1_0::CGroupFilter& source,
	imtbase::IComplexCollectionFilter::GroupFilter& target)
{
	QList<sdl::imtbase::ComplexCollectionFilter::V1_0::CFieldFilter> sourceFieldSubFilters = source.GetFieldFilters();
	QList<sdl::imtbase::ComplexCollectionFilter::V1_0::CGroupFilter> sourceGroupSubFilters = source.GetGroupFilters();
	QList<imtbase::IComplexCollectionFilter::FieldFilter> targetFieldSubFilters;
	QList<imtbase::IComplexCollectionFilter::GroupFilter> targetGroupSubFilters;

	for (const sdl::imtbase::ComplexCollectionFilter::V1_0::CFieldFilter& sourceFieldSubFilter : sourceFieldSubFilters){
		imtbase::IComplexCollectionFilter::FieldFilter targetFieldSubFilter;

		if (!ProcessFieldFilter(sourceFieldSubFilter, targetFieldSubFilter)){
			return false;
		}

		targetFieldSubFilters.append(targetFieldSubFilter);
	}

	for (const sdl::imtbase::ComplexCollectionFilter::V1_0::CGroupFilter& sourceGroupSubFilter : sourceGroupSubFilters){
		imtbase::IComplexCollectionFilter::GroupFilter targetGroupSubFilter;

		if (!ProcessGroupFilter(sourceGroupSubFilter, targetGroupSubFilter)){
			return false;
		}

		targetGroupSubFilters.append(targetGroupSubFilter);
	}

	target.fieldFilters = targetFieldSubFilters;
	target.groupFilters = targetGroupSubFilters;

	if (source.GetLogicalOperation() == "AND"){
		target.logicalOperation = imtbase::IComplexCollectionFilter::LO_AND;
	}
	else if (source.GetLogicalOperation() == "OR"){
		target.logicalOperation = imtbase::IComplexCollectionFilter::LO_OR;
	}
	else{
		return false;
	}

	return true;
}


bool ComplexCollectionFilterRepresentationToModel(
	sdl::imtbase::ComplexCollectionFilter::V1_0::CComplexCollectionFilter& filterRepresentaion,
	imtbase::IComplexCollectionFilter& filter)
{
	QList<sdl::imtbase::ComplexCollectionFilter::V1_0::CFieldSortingInfo> sourceSorting = filterRepresentaion.GetSortingInfo();
	sdl::imtbase::ComplexCollectionFilter::V1_0::CGroupFilter sourceFilter = filterRepresentaion.GetFieldsFilter();
	sdl::imtbase::ComplexCollectionFilter::V1_0::CTimeFilter sourceTimeFilter = filterRepresentaion.GetTimeFilter();

	// ---
	imtbase::IComplexCollectionFilter::FieldSortingInfoList sorting;
	for (const sdl::imtbase::ComplexCollectionFilter::V1_0::CFieldSortingInfo& sourceSortingItem  :sourceSorting){
		imtbase::IComplexCollectionFilter::FieldSortingInfo fieldSorting;

		fieldSorting.fieldId = sourceSortingItem.GetFieldId().toLatin1();

		if (sourceSortingItem.GetSortingOrder() == "ASC"){
			fieldSorting.sortingOrder = imtbase::IComplexCollectionFilter::SO_ASC;
		}
		else if (sourceSortingItem.GetSortingOrder() == "DESC"){
			fieldSorting.sortingOrder = imtbase::IComplexCollectionFilter::SO_DESC;
		}
		else{
			return false;
		}

		sorting.append(fieldSorting);
	}
	filter.SetSortingInfo(sorting);

	// ---
	imtbase::IComplexCollectionFilter::GroupFilter targetFilter;
	if (!ProcessGroupFilter(sourceFilter, targetFilter)){
		return false;
	}
	filter.SetFieldsFilter(targetFilter);

	// ---
	imtbase::CTimeFilterParam timeFilter;
	if (filterRepresentaion.HasTimeFilter()){
		sdl::imtbase::ComplexCollectionFilter::V1_0::CTimeFilter timeFilterSdl = filterRepresentaion.GetTimeFilter();
		if (timeFilterSdl.HasTimeRange()){
			sdl::imtbase::ComplexCollectionFilter::V1_0::CTimeRange timeRangeSdl = timeFilterSdl.GetTimeRange();

			timeFilter.SetTimeRange(imtbase::CTimeRange(
				QDateTime::fromString(timeRangeSdl.GetBegin(), "dd.MM.yyyy hh:mm:ss"),
				QDateTime::fromString(timeRangeSdl.GetEnd(), "dd.MM.yyyy hh:mm:ss")));
		}

		imtbase::ITimeFilterParam::TimeUnit timeUnit = imtbase::ITimeFilterParam::TU_CUSTOM;
		if (timeFilterSdl.HasTimeUnit()){
			QString timeUnitStr = timeFilterSdl.GetTimeUnit();

			if (timeUnitStr == "Custom"){
				timeUnit = imtbase::ITimeFilterParam::TU_CUSTOM;
			}
			else if (timeUnitStr == "Hour"){
				timeUnit = imtbase::ITimeFilterParam::TU_HOUR;
			}
			else if (timeUnitStr == "Day"){
				timeUnit = imtbase::ITimeFilterParam::TU_DAY;
			}
			else if (timeUnitStr == "Week"){
				timeUnit = imtbase::ITimeFilterParam::TU_WEEK;
			}
			else if (timeUnitStr == "Month"){
				timeUnit = imtbase::ITimeFilterParam::TU_MONTH;
			}
			else if (timeUnitStr == "Year"){
				timeUnit = imtbase::ITimeFilterParam::TU_YEAR;
			}
		}

		imtbase::ITimeFilterParam::InterpretationMode interpretationMode = imtbase::ITimeFilterParam::IM_FOR;
		if (timeFilterSdl.HasInterpretationMode()){
			QString interpretationModeStr = timeFilterSdl.GetInterpretationMode();

			if (interpretationModeStr == "For"){
				interpretationMode = imtbase::ITimeFilterParam::IM_FOR;
			}
			else if (interpretationModeStr == "Current"){
				interpretationMode = imtbase::ITimeFilterParam::IM_CURRENT;
			}
			else if (interpretationModeStr == "Last"){
				interpretationMode = imtbase::ITimeFilterParam::IM_LAST;
			}
		}

		int multiplier = 1;
		if (timeFilterSdl.HasUnitMultiplier()){
			multiplier = timeFilterSdl.GetUnitMultiplier();
		}

		timeFilter.SetTimeUnit(timeUnit, interpretationMode, multiplier);
	}
	filter.SetTimeFilter(timeFilter);

	return true;
}


} // namespace imtcol


