// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CTimeFilterParamRepresentationController.h>


// ImtCore includes
#include <imtbase/ITimeFilterParam.h>


namespace imtserverapp
{


// public methods

CTimeFilterParamRepresentationController::CTimeFilterParamRepresentationController()
{
	m_modelId = QByteArrayLiteral("TimeFilter");
	m_name = QStringLiteral("Time Filter");
	m_description = QStringLiteral("Time filter");
}


CTimeFilterParamRepresentationController::CTimeFilterParamRepresentationController(
			const QByteArray& filterId,
			const QString& filterName,
			const QString& filterDescription)
{
	m_modelId = filterId;
	m_name = filterName;
	m_description = filterDescription;
}


// reimplemented (imtserverapp::TJsonRepresentationControllerWrap<sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0>)

QByteArray CTimeFilterParamRepresentationController::GetTypeId() const
{
	return QByteArrayLiteral("TimeFilter");
}


bool CTimeFilterParamRepresentationController::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtbase::ITimeFilterParam* timeFilterParamPtr = dynamic_cast<const imtbase::ITimeFilterParam*>(&dataModel);

	return timeFilterParamPtr != nullptr;
}


bool CTimeFilterParamRepresentationController::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtbase::ITimeFilterParam* timeFilterParamPtr = dynamic_cast<const imtbase::ITimeFilterParam*>(&dataModel);
	if (timeFilterParamPtr == nullptr){
		return false;
	}

	sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0 sdlTimeFilter;

	imtbase::CTimeRange timeRange = timeFilterParamPtr->GetTimeRange();
	sdlRepresentation.timeRange.emplace();
	sdlRepresentation.timeRange->Begin = timeRange.GetBeginTime().toString(Qt::ISODateWithMs);
	sdlRepresentation.timeRange->End = timeRange.GetEndTime().toString(Qt::ISODateWithMs);

	imtbase::ITimeFilterParam::TimeUnit timeUnit = timeFilterParamPtr->GetTimeUnit();
	switch (timeUnit){
	case imtbase::ITimeFilterParam::TU_CUSTOM:
		sdlRepresentation.timeUnit = "Custom";
		break;
	case imtbase::ITimeFilterParam::TU_HOUR:
		sdlRepresentation.timeUnit = "Hour";
		break;
	case imtbase::ITimeFilterParam::TU_DAY:
		sdlRepresentation.timeUnit = "Day";
		break;
	case imtbase::ITimeFilterParam::TU_WEEK:
		sdlRepresentation.timeUnit = "Week";
		break;
	case imtbase::ITimeFilterParam::TU_MONTH:
		sdlRepresentation.timeUnit = "Month";
		break;
	case imtbase::ITimeFilterParam::TU_YEAR:
		sdlRepresentation.timeUnit = "Year";
		break;
	}

	imtbase::ITimeFilterParam::InterpretationMode mode = timeFilterParamPtr->GetInterpretationMode();
	switch (mode){
	case imtbase::ITimeFilterParam::IM_FOR:
		sdlRepresentation.interpretationMode = "For";
		break;
	case imtbase::ITimeFilterParam::IM_CURRENT:
		sdlRepresentation.interpretationMode = "Current";
		break;
	case imtbase::ITimeFilterParam::IM_LAST:
		sdlRepresentation.interpretationMode = "Last";
		break;
	}

	sdlRepresentation.unitMultiplier = timeFilterParamPtr->GetUnitMultiplier();

	return true;
}


bool CTimeFilterParamRepresentationController::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0& sdlRepresentation) const
{
	imtbase::ITimeFilterParam* timeFilterParamPtr = dynamic_cast<imtbase::ITimeFilterParam*>(&dataModel);
	if (timeFilterParamPtr == nullptr){
		return false;
	}

	if (sdlRepresentation.timeRange){
		sdl::imtbase::ImtBaseTypes::CTimeRange::V1_0 timeRangeSdl = *sdlRepresentation.timeRange;

		QString startRange;
		if (timeRangeSdl.Begin){
			startRange = *timeRangeSdl.Begin;
		}

		QString endRange;
		if (timeRangeSdl.End){
			endRange = *timeRangeSdl.End;
		}

		timeFilterParamPtr->SetTimeRange(imtbase::CTimeRange(
											 QDateTime::fromString(startRange, Qt::ISODateWithMs),
											 QDateTime::fromString(endRange, Qt::ISODateWithMs)));
	}

	imtbase::ITimeFilterParam::TimeUnit timeUnit = imtbase::ITimeFilterParam::TU_CUSTOM;
	if (sdlRepresentation.timeUnit){
		QString timeUnitStr = *sdlRepresentation.timeUnit;

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
	if (sdlRepresentation.interpretationMode){
		QString interpretationModeStr = *sdlRepresentation.interpretationMode;

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
	if (sdlRepresentation.unitMultiplier){
		multiplier = *sdlRepresentation.unitMultiplier;
	}

	timeFilterParamPtr->SetTimeUnit(timeUnit, interpretationMode, multiplier);

	return true;
}


} // namespace imtserverapp


