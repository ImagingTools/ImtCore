#include <imtrest/CTimeFilterParamRepresentationController.h>


// ACF includes
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtbase/ITimeFilterParam.h>


namespace imtrest
{


// public methods

// reimplemented (IRepresentationController)

QByteArray CTimeFilterParamRepresentationController::GetModelId() const
{
	return QByteArray();
}


bool CTimeFilterParamRepresentationController::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtbase::ITimeFilterParam* timeFilterParamPtr = dynamic_cast<const imtbase::ITimeFilterParam*>(&dataModel);
	return timeFilterParamPtr != nullptr;
}


bool CTimeFilterParamRepresentationController::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	const imtbase::ITimeFilterParam* timeFilterParamPtr = dynamic_cast<const imtbase::ITimeFilterParam*>(&dataModel);
	Q_ASSERT(timeFilterParamPtr != nullptr);

	imtbase::CTreeItemModel* timeRangeModelPtr = representation.AddTreeModel("TimeRange");
	Q_ASSERT(timeRangeModelPtr != nullptr);

	imtbase::CTimeRange timeRange = timeFilterParamPtr->GetTimeRange();
	timeRangeModelPtr->SetData("Begin", timeRange.GetBeginTime().toString("dd.MM.yyyy hh:mm:ss"));
	timeRangeModelPtr->SetData("End", timeRange.GetEndTime().toString("dd.MM.yyyy hh:mm:ss"));

	QString timeUnitStr;
	imtbase::ITimeFilterParam::TimeUnit timeUnit = timeFilterParamPtr->GetTimeUnit();
	switch(timeUnit){
	case imtbase::ITimeFilterParam::TU_CUSTOM:
		timeUnitStr = "Custom";
		break;
	case imtbase::ITimeFilterParam::TU_HOUR:
		timeUnitStr = "Hour";
		break;
	case imtbase::ITimeFilterParam::TU_DAY:
		timeUnitStr = "Day";
		break;
	case imtbase::ITimeFilterParam::TU_WEEK:
		timeUnitStr = "Week";
		break;
	case imtbase::ITimeFilterParam::TU_MONTH:
		timeUnitStr = "Month";
		break;
	case imtbase::ITimeFilterParam::TU_YEAR:
		timeUnitStr = "Year";
		break;
	}

	representation.SetData("TimeUnit", timeUnitStr);

	QString interpretationModeStr;
	imtbase::ITimeFilterParam::InterpretationMode interpretationMode = timeFilterParamPtr->GetInterpretationMode();
	switch(interpretationMode){
	case imtbase::ITimeFilterParam::IM_FOR:
		timeUnitStr = "For";
		break;
	case imtbase::ITimeFilterParam::IM_CURRENT:
		timeUnitStr = "Current";
		break;
	case imtbase::ITimeFilterParam::IM_LAST:
		timeUnitStr = "Last";
		break;
	}

	representation.SetData("InterpretationMode", interpretationModeStr);
	representation.SetData("UnitMultiplier", timeFilterParamPtr->GetUnitMultiplier());

	return true;
}


bool CTimeFilterParamRepresentationController::GetDataModelFromRepresentation(
			const imtbase::CTreeItemModel& representation,
			istd::IChangeable& dataModel) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	imtbase::ITimeFilterParam* timeFilterParamPtr = dynamic_cast<imtbase::ITimeFilterParam*>(&dataModel);
	Q_ASSERT(timeFilterParamPtr != nullptr);

	if (representation.ContainsKey("TimeRange")){
		imtbase::CTreeItemModel* timeRangeModelPtr = representation.GetTreeItemModel("TimeRange");
		if (timeRangeModelPtr == nullptr){
			return false;
		}

		QString begin = timeRangeModelPtr->GetData("Begin").toString();
		QString end = timeRangeModelPtr->GetData("End").toString();

		imtbase::CTimeRange* timeRange = new imtbase::CTimeRange;

		QDateTime dateTime = QDateTime::fromString(begin, "dd.MM.yyyy hh:mm:ss");

		timeRange->SetTimeRange(QDateTime::fromString(begin, "dd.MM.yyyy hh:mm:ss"), QDateTime::fromString(end, "dd.MM.yyyy hh:mm:ss"));

		timeFilterParamPtr->SetTimeRange(*timeRange);
	}

	imtbase::ITimeFilterParam::TimeUnit timeUnit = imtbase::ITimeFilterParam::TU_CUSTOM;

	if (representation.ContainsKey("TimeUnit")){
		QString timeUnitStr = representation.GetData("TimeUnit").toString();

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
	if (representation.ContainsKey("InterpretationMode")){
		QString interpretationModeStr = representation.GetData("InterpretationMode").toString();

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
	if (representation.ContainsKey("UnitMultiplier")){
		multiplier = representation.GetData("UnitMultiplier").toInt();
	}

	timeFilterParamPtr->SetTimeUnit(timeUnit, interpretationMode, multiplier);

	return true;
}


} // namespace imtrest


