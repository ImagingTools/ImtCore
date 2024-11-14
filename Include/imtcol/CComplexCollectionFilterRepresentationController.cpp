#include <imtcol/CComplexCollectionFilterRepresentationController.h>


// ACF includes
#include <ilog/CMessage.h>

// ImtCore includes
#include <imtbase/CTimeFilterParam.h>


namespace imtcol
{


bool CComplexCollectionFilterRepresentationController::ProcessFieldFilter(
	const sdl::imtbase::ComplexCollectionFilter::V1_0::CFieldFilter& source,
	imtbase::IComplexCollectionFilter::FieldFilter& target,
	ilog::IMessageConsumer* messageConsumerPtr)
{
	bool retVal = true;
	bool isOk = true;

	target.fieldId = source.GetFieldId();
	QString value = source.GetFilterValue();
	if (source.GetFilterValueType().GetSignedNumber()){
		target.filterValue = value.toLongLong(&isOk);
	}
	else if (source.GetFilterValueType().GetUnsignedNumber()){
		target.filterValue = value.toULongLong(&isOk);
	}
	else if (source.GetFilterValueType().GetFloatingPoint()){
		target.filterValue = value.toDouble(&isOk);
	}
	else if (source.GetFilterValueType().GetStringValue()){
		target.filterValue = value;
	}
	else if (source.GetFilterValueType().GetBoolValue()){
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
		sdl::imtbase::ComplexCollectionFilter::V1_0::CFilterOperations filterOperations = source.GetFilterOperation();

		int flags = 0;

		if (filterOperations.GetEqualOp()){
			if (flags & FOF_CONTAINS){
				SendErrorMessage("Equal and Contains flags can't be combined", messageConsumerPtr);

				return false;
			}

			flags |= FOF_EQUAL;
		}
		else if (filterOperations.GetNotOp()){
			flags |= FOF_NOT;
		}
		else if (filterOperations.GetGreaterOp()){
			if (flags & FOF_CONTAINS){
				SendErrorMessage("Greater and Contains flags can't be combined", messageConsumerPtr);

				return false;
			}

			flags |= FOF_GREATER;
		}
		else if (filterOperations.GetLessOp()){
			if (flags & FOF_GREATER){
				SendErrorMessage("Less and Greater flags can't be combined", messageConsumerPtr);

				return false;
			}

			if (flags & FOF_CONTAINS){
				SendErrorMessage("Less and Contains flags can't be combined", messageConsumerPtr);

				return false;
			}

			flags |= FOF_LESS;
		}
		else if (filterOperations.GetContainsOp()){
			if (flags != 0){
				SendErrorMessage("Contains flag can't be combined with any other flag", messageConsumerPtr);

				return false;
			}

			flags |= FOF_CONTAINS;
		}
		else{
			retVal = false;
		}

		if (flags & FOF_CONTAINS){
			Q_ASSERT(flags == FOF_CONTAINS);
			target.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_CONTAINS;
		}
		else if (flags & FOF_EQUAL){
			if (flags & FOF_NOT){
				target.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_EQUAL;
			}
			else{
				target.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL;
			}
		}
		else if (flags & FOF_LESS){
			if (flags & FOF_NOT){
				target.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_LESS;
			}
			else{
				target.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_LESS;
			}
		}
		else if (flags & FOF_GREATER){
			if (flags & FOF_NOT){
				target.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_GREATER;
			}
			else{
				target.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_GREATER;
			}
		}
	}

	return retVal;
}


bool CComplexCollectionFilterRepresentationController::ProcessGroupFilter(
	const sdl::imtbase::ComplexCollectionFilter::V1_0::CGroupFilter& source,
	imtbase::IComplexCollectionFilter::GroupFilter& target,
	ilog::IMessageConsumer* messageConsumerPtr)
{
	QList<sdl::imtbase::ComplexCollectionFilter::V1_0::CFieldFilter> sourceFieldSubFilters = source.GetFieldFilters();
	QList<sdl::imtbase::ComplexCollectionFilter::V1_0::CGroupFilter> sourceGroupSubFilters = source.GetGroupFilters();
	QList<imtbase::IComplexCollectionFilter::FieldFilter> targetFieldSubFilters;
	QList<imtbase::IComplexCollectionFilter::GroupFilter> targetGroupSubFilters;

	for (const sdl::imtbase::ComplexCollectionFilter::V1_0::CFieldFilter& sourceFieldSubFilter : sourceFieldSubFilters){
		imtbase::IComplexCollectionFilter::FieldFilter targetFieldSubFilter;

		if (!ProcessFieldFilter(sourceFieldSubFilter, targetFieldSubFilter, messageConsumerPtr)){
			return false;
		}

		targetFieldSubFilters.append(targetFieldSubFilter);
	}

	for (const sdl::imtbase::ComplexCollectionFilter::V1_0::CGroupFilter& sourceGroupSubFilter : sourceGroupSubFilters){
		imtbase::IComplexCollectionFilter::GroupFilter targetGroupSubFilter;

		if (!ProcessGroupFilter(sourceGroupSubFilter, targetGroupSubFilter, messageConsumerPtr)){
			return false;
		}

		targetGroupSubFilters.append(targetGroupSubFilter);
	}

	target.fieldFilters = targetFieldSubFilters;
	target.groupFilters = targetGroupSubFilters;

	if (source.GetLogicalOperation().GetAndOp()){
		target.logicalOperation = imtbase::IComplexCollectionFilter::LO_AND;
	}
	else if (source.GetLogicalOperation().GetOrOp()){
		target.logicalOperation = imtbase::IComplexCollectionFilter::LO_OR;
	}
	else{
		SendErrorMessage("Logical group operation was not defined", messageConsumerPtr);

		return false;
	}

	return true;
}


bool CComplexCollectionFilterRepresentationController::ComplexCollectionFilterRepresentationToModel(
	sdl::imtbase::ComplexCollectionFilter::V1_0::CComplexCollectionFilter& filterRepresentaion,
	imtbase::IComplexCollectionFilter& filter,
	ilog::IMessageConsumer* messageConsumerPtr)
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


	imtbase::IComplexCollectionFilter::GroupFilter targetFilter;
	if (!ProcessGroupFilter(sourceFilter, targetFilter, messageConsumerPtr)){
		return false;
	}
	filter.SetFieldsFilter(targetFilter);


	imtbase::CTimeFilterParam timeFilter;
	if (filterRepresentaion.HasTimeFilter()){
		sdl::imtbase::ComplexCollectionFilter::V1_0::CTimeFilter timeFilterSdl = filterRepresentaion.GetTimeFilter();
		if (timeFilterSdl.HasTimeRange()){
			sdl::imtbase::ImtBaseTypes::V1_0::CTimeRange timeRangeSdl = timeFilterSdl.GetTimeRange();

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


void CComplexCollectionFilterRepresentationController::SendErrorMessage(const QString& message, ilog::IMessageConsumer* messageConsumerPtr)
{
	if (messageConsumerPtr != nullptr){
		ilog::IMessageConsumer::MessagePtr messagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, message, "CComplexCollectionFilterRepresentationController"));

		messageConsumerPtr->AddMessage(messagePtr);
	}
}


} // namespace imtcol


