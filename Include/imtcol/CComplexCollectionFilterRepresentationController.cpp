#include <imtcol/CComplexCollectionFilterRepresentationController.h>


// ACF includes
#include <ilog/CMessage.h>

// ImtCore includes
#include <imtbase/CTimeFilterParam.h>


namespace imtcol
{


bool CComplexCollectionFilterRepresentationController::ProcessFieldFilter(
			const sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0& source,
			imtbase::IComplexCollectionFilter::FieldFilter& target,
			ilog::IMessageConsumer* messageConsumerPtr)
{
	bool retVal = true;
	bool isOk = true;

	QByteArray targetFieldId;
	if (!source.fieldId){
		return false;
	}

	target.fieldId = *source.fieldId;

	QString value;
	if (source.filterValue){
		value = *source.filterValue;
	}

	sdl::imtbase::ComplexCollectionFilter::CValueType::V1_0 sourceFilterValueType;
	if (source.filterValueType){
		sourceFilterValueType = *source.filterValueType;
	}

	if (sourceFilterValueType.signedNumber && *sourceFilterValueType.signedNumber){
		target.filterValue = value.toLongLong(&isOk);
	}
	else if (sourceFilterValueType.unsignedNumber && *sourceFilterValueType.unsignedNumber){
		target.filterValue = value.toULongLong(&isOk);
	}
	else if (sourceFilterValueType.floatingPoint && *sourceFilterValueType.floatingPoint){
		target.filterValue = value.toDouble(&isOk);
	}
	else if (sourceFilterValueType.stringValue && *sourceFilterValueType.stringValue){
		target.filterValue = value;
	}
	else if (sourceFilterValueType.boolValue && *sourceFilterValueType.boolValue){
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
		sdl::imtbase::ComplexCollectionFilter::CFilterOperations::V1_0 filterOperations;
		if (source.filterOperation){
			filterOperations = *source.filterOperation;
		}

		int flags = 0;

		if (filterOperations.equalOp && *filterOperations.equalOp){
			if (flags & FOF_CONTAINS){
				SendErrorMessage("Equal and Contains flags can't be combined", messageConsumerPtr);

				return false;
			}

			flags |= FOF_EQUAL;
		}
		else if (filterOperations.notOp && *filterOperations.notOp){
			flags |= FOF_NOT;
		}
		else if (filterOperations.greaterOp && *filterOperations.greaterOp){
			if (flags & FOF_CONTAINS){
				SendErrorMessage("Greater and Contains flags can't be combined", messageConsumerPtr);

				return false;
			}

			flags |= FOF_GREATER;
		}
		else if (filterOperations.lessOp && *filterOperations.lessOp){
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
		else if (filterOperations.containsOp && *filterOperations.containsOp){
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
			const sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0& source,
			imtbase::IComplexCollectionFilter::GroupFilter& target,
			ilog::IMessageConsumer* messageConsumerPtr)
{
	QList<sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0> sourceFieldSubFilters;
	if (source.fieldFilters){
		sourceFieldSubFilters = *source.fieldFilters;
	}
	QList<sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0> sourceGroupSubFilters;
	if (source.groupFilters){
		sourceGroupSubFilters = *source.groupFilters;
	}
	QList<imtbase::IComplexCollectionFilter::FieldFilter> targetFieldSubFilters;
	QList<imtbase::IComplexCollectionFilter::GroupFilter> targetGroupSubFilters;

	for (const sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0& sourceFieldSubFilter : sourceFieldSubFilters){
		imtbase::IComplexCollectionFilter::FieldFilter targetFieldSubFilter;

		if (!ProcessFieldFilter(sourceFieldSubFilter, targetFieldSubFilter, messageConsumerPtr)){
			return false;
		}

		targetFieldSubFilters.append(targetFieldSubFilter);
	}

	for (const sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0& sourceGroupSubFilter : sourceGroupSubFilters){
		imtbase::IComplexCollectionFilter::GroupFilter targetGroupSubFilter;

		if (!ProcessGroupFilter(sourceGroupSubFilter, targetGroupSubFilter, messageConsumerPtr)){
			return false;
		}

		targetGroupSubFilters.append(targetGroupSubFilter);
	}

	target.fieldFilters = targetFieldSubFilters;
	target.groupFilters = targetGroupSubFilters;

	sdl::imtbase::ComplexCollectionFilter::CLogicalOperation::V1_0 sourceLogicalOperation;
	if (source.logicalOperation){
		sourceLogicalOperation = *source.logicalOperation;
	}

	if (sourceLogicalOperation.andOp && *sourceLogicalOperation.andOp){
		target.logicalOperation = imtbase::IComplexCollectionFilter::LO_AND;
	}
	else if (sourceLogicalOperation.orOp && *sourceLogicalOperation.orOp){
		target.logicalOperation = imtbase::IComplexCollectionFilter::LO_OR;
	}
	else{
		SendErrorMessage("Logical group operation was not defined", messageConsumerPtr);

		return false;
	}

	return true;
}


bool CComplexCollectionFilterRepresentationController::ComplexCollectionFilterRepresentationToModel(
			sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0& filterRepresentaion,
			imtbase::IComplexCollectionFilter& filter,
			ilog::IMessageConsumer* messageConsumerPtr)
{
	QList<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfo::V1_0> sourceSorting;
	if (filterRepresentaion.sortingInfo){
		sourceSorting = *filterRepresentaion.sortingInfo;
	}
	sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0 sourceFilter;
	if (filterRepresentaion.fieldsFilter){
		sourceFilter = *filterRepresentaion.fieldsFilter;
	}
	sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0 sourceTimeFilter;
	if (filterRepresentaion.timeFilter){
		sourceTimeFilter = *filterRepresentaion.timeFilter;
	}

	// ---
	imtbase::IComplexCollectionFilter::FieldSortingInfoList sorting;
	for (const sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfo::V1_0& sourceSortingItem  :sourceSorting){
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
		sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0 timeFilterSdl = *filterRepresentaion.timeFilter;
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
				QDateTime::fromString(startRange, "dd.MM.yyyy hh:mm:ss"),
				QDateTime::fromString(endRange, "dd.MM.yyyy hh:mm:ss")));
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


