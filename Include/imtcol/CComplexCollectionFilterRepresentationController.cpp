#include <imtcol/CComplexCollectionFilterRepresentationController.h>


// ACF includes
#include <ilog/CMessage.h>

// ImtCore includes
#include <imtbase/CTimeFilterParam.h>


namespace imtcol
{


const QMap<int, imtbase::IComplexCollectionFilter::FieldOperation> CComplexCollectionFilterRepresentationController::s_allowableFlagsCombination = {
	{FOF_EQUAL, imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL},
	{FOF_EQUAL | FOF_NOT, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_EQUAL},

	{FOF_GREATER, imtbase::IComplexCollectionFilter::FieldOperation::FO_GREATER},
	{FOF_GREATER | FOF_NOT, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_GREATER},
	{FOF_GREATER | FOF_EQUAL, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_LESS},

	{FOF_LESS, imtbase::IComplexCollectionFilter::FieldOperation::FO_LESS},
	{FOF_LESS | FOF_NOT, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_LESS},
	{FOF_LESS | FOF_EQUAL, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_GREATER},

	{FOF_CONTAINS, imtbase::IComplexCollectionFilter::FieldOperation::FO_CONTAINS}
};


QString CComplexCollectionFilterRepresentationController::GetFlagsAsString(int flags)
{
	QString retVal;

	if (flags & FOF_NOT){
		retVal = "NOT";
	}

	if (flags & FOF_EQUAL){
		retVal += retVal.isEmpty() ? "EQUAL" : ", EQUAL";
	}

	if (flags & FOF_GREATER){
		retVal += retVal.isEmpty() ? "GREATER" : ", GREATER";
	}

	if (flags & FOF_LESS){
		retVal += retVal.isEmpty() ? "LESS" : ", LESS";
	}

	if (flags & FOF_CONTAINS){
		retVal += retVal.isEmpty() ? "CONTAINS" : ", CONTAINS";
	}

	return retVal;
}


bool CComplexCollectionFilterRepresentationController::ProcessFieldFilter(
			const sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0& source,
			imtbase::IComplexCollectionFilter::FieldFilter& target,
			ilog::IMessageConsumer* messageConsumerPtr)
{
	namespace Filter = sdl::imtbase::ComplexCollectionFilter;

	bool isOk = true;
	bool retVal = true;

	if (!source.fieldId){
		SendErrorMessage("ComplexCollectionFilter: Filter field ID not available", messageConsumerPtr);
		return false;
	}
	target.fieldId = *source.fieldId;

	QString value;
	if (!source.filterValue){
		SendErrorMessage("ComplexCollectionFilter: Filter field value not available", messageConsumerPtr);
		return false;
	}
	value = *source.filterValue;

	if (!source.filterValueType){
		SendErrorMessage("ComplexCollectionFilter: Filter field value type not available", messageConsumerPtr);
		return false;
	}

	Filter::ValueType sourceFilterValueType = *source.filterValueType;
	switch (sourceFilterValueType){
	case Filter::ValueType::Integer:
		target.filterValue = value.toLongLong(&isOk);
		break;
	case Filter::ValueType::Number:
		target.filterValue = value.toDouble(&isOk);
		break;
	case Filter::ValueType::String:
		target.filterValue = value;
		break;
	case Filter::ValueType::Bool:
		if (value.compare("true", Qt::CaseInsensitive) == 0){
			target.filterValue = true;
		}
		else if (value.compare("false", Qt::CaseInsensitive) == 0){
			target.filterValue = false;
		}
		else{
			SendErrorMessage("ComplexCollectionFilter: Invalid filter bool value", messageConsumerPtr);
			return false;
		}
		break;
	default:
		SendErrorMessage("ComplexCollectionFilter: Invalid filter value type", messageConsumerPtr);
		return false;
	}

	retVal = retVal && isOk;

	if (retVal){
		if (!source.filterOperations || source.filterOperations->isEmpty()){
			SendErrorMessage("ComplexCollectionFilter: Filter operation not available", messageConsumerPtr);
			return false;
		}
		QList<Filter::FilterOperation> filterOperations = *source.filterOperations;

		int flags = 0;

		if (filterOperations.contains(Filter::FilterOperation::Not)){
			flags |= FOF_NOT;
		}
		if (filterOperations.contains(Filter::FilterOperation::Equal)){
			flags |= FOF_EQUAL;
		}
		if (filterOperations.contains(Filter::FilterOperation::Less)){
			flags |= FOF_LESS;
		}
		if (filterOperations.contains(Filter::FilterOperation::Greater)){
			flags |= FOF_GREATER;
		}
		if (filterOperations.contains(Filter::FilterOperation::Contains)){
			flags |= FOF_CONTAINS;
		}

		if (!s_allowableFlagsCombination.contains(flags)){
			SendErrorMessage(QString("ComplexCollectionFilter: invalid flags combination (%1)").arg(GetFlagsAsString(flags)), messageConsumerPtr);
			return false;
		}

		target.filterOperation = s_allowableFlagsCombination[flags];
	}

	return retVal;
}


bool CComplexCollectionFilterRepresentationController::ProcessGroupFilter(
			const sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0& source,
			imtbase::IComplexCollectionFilter::GroupFilter& target,
			ilog::IMessageConsumer* messageConsumerPtr)
{
	namespace Filter = sdl::imtbase::ComplexCollectionFilter;

	QList<Filter::CFieldFilter::V1_0> sourceFieldSubFilters;
	if (source.fieldFilters){
		sourceFieldSubFilters = *source.fieldFilters;
	}
	QList<Filter::CGroupFilter::V1_0> sourceGroupSubFilters;
	if (source.groupFilters){
		sourceGroupSubFilters = *source.groupFilters;
	}
	QVector<imtbase::IComplexCollectionFilter::FieldFilter> targetFieldSubFilters;
	QVector<imtbase::IComplexCollectionFilter::GroupFilter> targetGroupSubFilters;

	for (const Filter::CFieldFilter::V1_0& sourceFieldSubFilter : std::as_const(sourceFieldSubFilters)){
		imtbase::IComplexCollectionFilter::FieldFilter targetFieldSubFilter;

		if (!ProcessFieldFilter(sourceFieldSubFilter, targetFieldSubFilter, messageConsumerPtr)){
			return false;
		}

		targetFieldSubFilters.append(targetFieldSubFilter);
	}

	for (const Filter::CGroupFilter::V1_0& sourceGroupSubFilter : std::as_const(sourceGroupSubFilters)){
		imtbase::IComplexCollectionFilter::GroupFilter targetGroupSubFilter;

		if (!ProcessGroupFilter(sourceGroupSubFilter, targetGroupSubFilter, messageConsumerPtr)){
			return false;
		}

		targetGroupSubFilters.append(targetGroupSubFilter);
	}

	target.fieldFilters = targetFieldSubFilters;
	target.groupFilters = targetGroupSubFilters;

	if (!source.groupFilters.has_value() && !source.fieldFilters.has_value()){
		return true;
	}

	if (!source.logicalOperation){
		return false;
	}
	Filter::LogicalOperation sourceLogicalOperation = *source.logicalOperation;

	switch (sourceLogicalOperation){
	case Filter::LogicalOperation::And:
		target.logicalOperation = imtbase::IComplexCollectionFilter::LO_AND;
		break;
	case Filter::LogicalOperation::Or:
		target.logicalOperation = imtbase::IComplexCollectionFilter::LO_OR;
		break;
	default:
		SendErrorMessage("ComplexCollectionFilter: Logical group operation was not defined", messageConsumerPtr);
		return false;
	}

	return true;
}


bool CComplexCollectionFilterRepresentationController::ComplexCollectionFilterRepresentationToModel(
	sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0& filterRepresentaion,
	imtbase::IComplexCollectionFilter& filter,
	ilog::IMessageConsumer* messageConsumerPtr)
{
	namespace Filter = sdl::imtbase::ComplexCollectionFilter;

	QList<Filter::CFieldSortingInfo::V1_0> sourceSorting;
	if (filterRepresentaion.sortingInfo){
		sourceSorting = *filterRepresentaion.sortingInfo;
	}
	Filter::CGroupFilter::V1_0 sourceFilter;
	if (filterRepresentaion.fieldsFilter){
		sourceFilter = *filterRepresentaion.fieldsFilter;
	}
	Filter::CTimeFilter::V1_0 sourceTimeFilter;
	if (filterRepresentaion.timeFilter){
		sourceTimeFilter = *filterRepresentaion.timeFilter;
	}

	// ---
	imtbase::IComplexCollectionFilter::FieldSortingInfoList sorting;
	for (const Filter::CFieldSortingInfo::V1_0& sourceSortingItem  : std::as_const(sourceSorting)){
		imtbase::IComplexCollectionFilter::FieldSortingInfo fieldSorting;

		if (sourceSortingItem.fieldId){
			fieldSorting.fieldId = sourceSortingItem.fieldId->toLatin1();
		}

		if (sourceSortingItem.sortingOrder && (*sourceSortingItem.sortingOrder == "ASC")){
			fieldSorting.sortingOrder = imtbase::IComplexCollectionFilter::SO_ASC;
		}
		else if (sourceSortingItem.sortingOrder && (*sourceSortingItem.sortingOrder == "DESC")){
			fieldSorting.sortingOrder = imtbase::IComplexCollectionFilter::SO_DESC;
		}
		else{
			return false;
		}

		sorting.append(fieldSorting);
	}
	filter.SetSortingInfo(sorting);


	imtbase::IComplexCollectionFilter::GroupFilter targetFilter;
	if (!ProcessGroupFilter(sourceFilter, targetFilter, messageConsumerPtr)){
		return false;
	}
	filter.SetFieldsFilter(targetFilter);


	imtbase::CTimeFilterParam timeFilter;
	if (filterRepresentaion.timeFilter){
		Filter::CTimeFilter::V1_0 timeFilterSdl = *filterRepresentaion.timeFilter;
		if (timeFilterSdl.timeRange){
			sdl::imtbase::ImtBaseTypes::CTimeRange::V1_0 timeRangeSdl = *timeFilterSdl.timeRange;

			QString startRange;
			if (timeRangeSdl.Begin){
				startRange = *timeRangeSdl.Begin;
			}
			QString endRange;
			if (timeRangeSdl.End){
				endRange = *timeRangeSdl.End;
			}
			timeFilter.SetTimeRange(imtbase::CTimeRange(
				QDateTime::fromString(startRange, Qt::ISODateWithMs),
				QDateTime::fromString(endRange, Qt::ISODateWithMs)));
		}

		imtbase::ITimeFilterParam::TimeUnit timeUnit = imtbase::ITimeFilterParam::TU_CUSTOM;
		if (timeFilterSdl.timeUnit){
			QString timeUnitStr = *timeFilterSdl.timeUnit;

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
		if (timeFilterSdl.interpretationMode){
			QString interpretationModeStr = *timeFilterSdl.interpretationMode;

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
		if (timeFilterSdl.unitMultiplier){
			multiplier = *timeFilterSdl.unitMultiplier;
		}

		timeFilter.SetTimeUnit(timeUnit, interpretationMode, multiplier);
	}
	filter.SetTimeFilter(timeFilter);

	if (filterRepresentaion.distinctFields){
		filter.SetDistinctFieldsList(*filterRepresentaion.distinctFields);
	}

	return true;
}


void CComplexCollectionFilterRepresentationController::SendErrorMessage(const QString& message, ilog::IMessageConsumer* messageConsumerPtr)
{
	if (messageConsumerPtr != nullptr){
		ilog::IMessageConsumer::MessagePtr messagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, message, "CComplexCollectionFilterRepresentationController"));

		messageConsumerPtr->AddMessage(messagePtr);
	}
}


} // namespace imtcol


