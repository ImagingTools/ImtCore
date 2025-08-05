#include "CTImpl.h"


#include "CTImpl.h"


#include "CTImpl.h"


#include "CTImpl.h"


/// \file CResult.cpp

namespace sdl::complextest::CTImpl
{


QByteArray CResult::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CResult::V1_0::operator==(const V1_0& other) const
{
	return 
				AreaResult == other.AreaResult &&
				ExpectedMinValue.has_value() == other.ExpectedMinValue.has_value() &&
				((ExpectedMinValue.has_value() && other.ExpectedMinValue.has_value()) ?
					qFuzzyCompare(*ExpectedMinValue, *other.ExpectedMinValue) : true) &&
				ExpectedMaxValue.has_value() == other.ExpectedMaxValue.has_value() &&
				((ExpectedMaxValue.has_value() && other.ExpectedMaxValue.has_value()) ?
					qFuzzyCompare(*ExpectedMaxValue, *other.ExpectedMaxValue) : true) &&
				MeasuredValue.has_value() == other.MeasuredValue.has_value() &&
				((MeasuredValue.has_value() && other.MeasuredValue.has_value()) ?
					qFuzzyCompare(*MeasuredValue, *other.MeasuredValue) : true) &&
				MinMeasuredValue.has_value() == other.MinMeasuredValue.has_value() &&
				((MinMeasuredValue.has_value() && other.MinMeasuredValue.has_value()) ?
					qFuzzyCompare(*MinMeasuredValue, *other.MinMeasuredValue) : true) &&
				MaxMeasuredValue.has_value() == other.MaxMeasuredValue.has_value() &&
				((MaxMeasuredValue.has_value() && other.MaxMeasuredValue.has_value()) ?
					qFuzzyCompare(*MaxMeasuredValue, *other.MaxMeasuredValue) : true) &&
				MeanMeasuredValue.has_value() == other.MeanMeasuredValue.has_value() &&
				((MeanMeasuredValue.has_value() && other.MeanMeasuredValue.has_value()) ?
					qFuzzyCompare(*MeanMeasuredValue, *other.MeanMeasuredValue) : true) &&
				MeasurementType == other.MeasurementType &&
				MeasurementUnit == other.MeasurementUnit &&
				Length.has_value() == other.Length.has_value() &&
				((Length.has_value() && other.Length.has_value()) ?
					qFuzzyCompare(*Length, *other.Length) : true) &&
				ErrorType == other.ErrorType &&
				Geometry == other.Geometry;
}


bool CResult::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (!AreaResult){
		return false;
	}
	QString areaResultStringValue;
	switch (*AreaResult){
	case ::sdl::complextest::CTTypes::StatusCode::NONE:
		areaResultStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::OK:
		areaResultStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::WARNING:
		areaResultStringValue = "WARNING";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::NOK:
		areaResultStringValue = "NOK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::FAILED:
		areaResultStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("AreaResult", areaResultStringValue, modelIndex);

	if (!ExpectedMinValue){
		return false;
	}
	model.SetData("ExpectedMinValue", *ExpectedMinValue, modelIndex);

	if (!ExpectedMaxValue){
		return false;
	}
	model.SetData("ExpectedMaxValue", *ExpectedMaxValue, modelIndex);

	if (!MeasuredValue){
		return false;
	}
	model.SetData("MeasuredValue", *MeasuredValue, modelIndex);

	if (!MinMeasuredValue){
		return false;
	}
	model.SetData("MinMeasuredValue", *MinMeasuredValue, modelIndex);

	if (!MaxMeasuredValue){
		return false;
	}
	model.SetData("MaxMeasuredValue", *MaxMeasuredValue, modelIndex);

	if (!MeanMeasuredValue){
		return false;
	}
	model.SetData("MeanMeasuredValue", *MeanMeasuredValue, modelIndex);

	if (!MeasurementType){
		return false;
	}
	QString measurementTypeStringValue;
	switch (*MeasurementType){
	case ::sdl::complextest::CTTypes::MeasurementType::NONE:
		measurementTypeStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::WIDTH:
		measurementTypeStringValue = "WIDTH";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::BRIGHTNESS:
		measurementTypeStringValue = "BRIGHTNESS";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::DISTANCE:
		measurementTypeStringValue = "DISTANCE";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::DISPLACEMENT:
		measurementTypeStringValue = "DISPLACEMENT";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::HEIGHT:
		measurementTypeStringValue = "HEIGHT";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::SYMMETRY:
		measurementTypeStringValue = "SYMMETRY";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::VOLUME:
		measurementTypeStringValue = "VOLUME";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::VOLUMERATE:
		measurementTypeStringValue = "VOLUMERATE";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::LENGTH:
		measurementTypeStringValue = "LENGTH";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::OUT_OF_BOUND:
		measurementTypeStringValue = "OUT_OF_BOUND";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("MeasurementType", measurementTypeStringValue, modelIndex);

	if (!MeasurementUnit){
		return false;
	}
	QString measurementUnitStringValue;
	switch (*MeasurementUnit){
	case ::sdl::complextest::CTTypes::MeasurementUnit::NONE:
		measurementUnitStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::MeasurementUnit::mm:
		measurementUnitStringValue = "mm";
		break;
	case ::sdl::complextest::CTTypes::MeasurementUnit::mm3:
		measurementUnitStringValue = "mm3";
		break;
	case ::sdl::complextest::CTTypes::MeasurementUnit::mm3_mm:
		measurementUnitStringValue = "mm3_mm";
		break;
	case ::sdl::complextest::CTTypes::MeasurementUnit::pixel:
		measurementUnitStringValue = "pixel";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("MeasurementUnit", measurementUnitStringValue, modelIndex);

	if (!Length){
		return false;
	}
	model.SetData("Length", *Length, modelIndex);

	if (!ErrorType){
		return false;
	}
	QString errorTypeStringValue;
	switch (*ErrorType){
	case ::sdl::complextest::CTTypes::ErrorCode::OK:
		errorTypeStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE:
		errorTypeStringValue = "TOO_WIDE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE:
		errorTypeStringValue = "TOLERANCE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW:
		errorTypeStringValue = "TOO_NARROW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP:
		errorTypeStringValue = "NO_DATA_GAP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::GAP:
		errorTypeStringValue = "GAP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH:
		errorTypeStringValue = "TOO_HIGH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW:
		errorTypeStringValue = "TOO_LOW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW:
		errorTypeStringValue = "TOO_FEW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH:
		errorTypeStringValue = "TOO_MUCH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER:
		errorTypeStringValue = "WRONG_DOT_DIAMETER";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION:
		errorTypeStringValue = "WRONG_DOT_POSITION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS:
		errorTypeStringValue = "MISSING_DOTS";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE:
		errorTypeStringValue = "WRONG_DOT_DISTANCE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE:
		errorTypeStringValue = "AREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE:
		errorTypeStringValue = "TOTALAREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE:
		errorTypeStringValue = "BORDERAREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA:
		errorTypeStringValue = "WRONG_DOT_AREA";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT:
		errorTypeStringValue = "WRONG_DOT_AREA_ONHEIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME:
		errorTypeStringValue = "WRONG_DOT_VOLUME";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT:
		errorTypeStringValue = "WRONG_DOT_HEIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR:
		errorTypeStringValue = "CONTOUR_MEASUREMENT_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT:
		errorTypeStringValue = "TOO_BRIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK:
		errorTypeStringValue = "TOO_DARK";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR:
		errorTypeStringValue = "COUNT_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH:
		errorTypeStringValue = "Z_DISTANCE_HIGH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW:
		errorTypeStringValue = "Z_DISTANCE_LOW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL:
		errorTypeStringValue = "NOT_SYMMETRICAL";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR:
		errorTypeStringValue = "REFERENCE_3D_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR:
		errorTypeStringValue = "COLOR_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT:
		errorTypeStringValue = "PATTERN_COUNT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION:
		errorTypeStringValue = "PATTERN_POSITION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION:
		errorTypeStringValue = "PATTERN_ROTATION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE:
		errorTypeStringValue = "CENTERLINE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP:
		errorTypeStringValue = "CALIB_ERROR_COMP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP:
		errorTypeStringValue = "CALIB_INVALID_COMP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED:
		errorTypeStringValue = "CALIB_NOZZLE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND:
		errorTypeStringValue = "CONTOUR_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE:
		errorTypeStringValue = "CONTOUR_TOO_WIDE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW:
		errorTypeStringValue = "CONTOUR_TOO_NARROW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED:
		errorTypeStringValue = "CONTOUR_GLUE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY:
		errorTypeStringValue = "CONTOUR_INVALID_POLY";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND:
		errorTypeStringValue = "REFERENCE_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF:
		errorTypeStringValue = "REFERENCE_INVALID_REF";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED:
		errorTypeStringValue = "REFERENCE_POS_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED:
		errorTypeStringValue = "REFERENCE_DIST_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND:
		errorTypeStringValue = "AREA_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND:
		errorTypeStringValue = "AREA_GLUE_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED:
		errorTypeStringValue = "AREA_GLUE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH:
		errorTypeStringValue = "AREA_GAP_LENGTH";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("ErrorType", errorTypeStringValue, modelIndex);


	if (!Geometry){
		return false;
	}
	::imtbase::CTreeItemModel* geometryNewModelPtr = model.AddTreeModel("Geometry", modelIndex);
	const bool isGeometryAdded = Geometry->WriteToModel(*geometryNewModelPtr, 0);
	if (!isGeometryAdded){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry");)

		return false;
	}


	return true;
}


bool CResult::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant areaResultData = model.GetData("AreaResult", modelIndex);
	if (areaResultData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "AreaResult");)

		return false;
	}
	QString areaResultStringValue = areaResultData.toString();
	if(areaResultStringValue == "NONE"){
		AreaResult = ::sdl::complextest::CTTypes::StatusCode::NONE;
	}
	else if(areaResultStringValue == "OK"){
		AreaResult = ::sdl::complextest::CTTypes::StatusCode::OK;
	}
	else if(areaResultStringValue == "WARNING"){
		AreaResult = ::sdl::complextest::CTTypes::StatusCode::WARNING;
	}
	else if(areaResultStringValue == "NOK"){
		AreaResult = ::sdl::complextest::CTTypes::StatusCode::NOK;
	}
	else if(areaResultStringValue == "FAILED"){
		AreaResult = ::sdl::complextest::CTTypes::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", areaResultStringValue);)

		return false;
	}

	QVariant expectedMinValueData = model.GetData("ExpectedMinValue", modelIndex);
	if (expectedMinValueData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMinValue");)

		return false;
	}
	ExpectedMinValue = expectedMinValueData.toFloat();

	QVariant expectedMaxValueData = model.GetData("ExpectedMaxValue", modelIndex);
	if (expectedMaxValueData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMaxValue");)

		return false;
	}
	ExpectedMaxValue = expectedMaxValueData.toFloat();

	QVariant measuredValueData = model.GetData("MeasuredValue", modelIndex);
	if (measuredValueData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MeasuredValue");)

		return false;
	}
	MeasuredValue = measuredValueData.toFloat();

	QVariant minMeasuredValueData = model.GetData("MinMeasuredValue", modelIndex);
	if (minMeasuredValueData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MinMeasuredValue");)

		return false;
	}
	MinMeasuredValue = minMeasuredValueData.toFloat();

	QVariant maxMeasuredValueData = model.GetData("MaxMeasuredValue", modelIndex);
	if (maxMeasuredValueData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MaxMeasuredValue");)

		return false;
	}
	MaxMeasuredValue = maxMeasuredValueData.toFloat();

	QVariant meanMeasuredValueData = model.GetData("MeanMeasuredValue", modelIndex);
	if (meanMeasuredValueData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MeanMeasuredValue");)

		return false;
	}
	MeanMeasuredValue = meanMeasuredValueData.toFloat();

	QVariant measurementTypeData = model.GetData("MeasurementType", modelIndex);
	if (measurementTypeData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementType");)

		return false;
	}
	QString measurementTypeStringValue = measurementTypeData.toString();
	if(measurementTypeStringValue == "NONE"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::NONE;
	}
	else if(measurementTypeStringValue == "WIDTH"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::WIDTH;
	}
	else if(measurementTypeStringValue == "BRIGHTNESS"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::BRIGHTNESS;
	}
	else if(measurementTypeStringValue == "DISTANCE"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::DISTANCE;
	}
	else if(measurementTypeStringValue == "DISPLACEMENT"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::DISPLACEMENT;
	}
	else if(measurementTypeStringValue == "HEIGHT"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::HEIGHT;
	}
	else if(measurementTypeStringValue == "SYMMETRY"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::SYMMETRY;
	}
	else if(measurementTypeStringValue == "VOLUME"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::VOLUME;
	}
	else if(measurementTypeStringValue == "VOLUMERATE"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::VOLUMERATE;
	}
	else if(measurementTypeStringValue == "LENGTH"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::LENGTH;
	}
	else if(measurementTypeStringValue == "OUT_OF_BOUND"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::OUT_OF_BOUND;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

		return false;
	}

	QVariant measurementUnitData = model.GetData("MeasurementUnit", modelIndex);
	if (measurementUnitData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit");)

		return false;
	}
	QString measurementUnitStringValue = measurementUnitData.toString();
	if(measurementUnitStringValue == "NONE"){
		MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::NONE;
	}
	else if(measurementUnitStringValue == "mm"){
		MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm;
	}
	else if(measurementUnitStringValue == "mm3"){
		MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm3;
	}
	else if(measurementUnitStringValue == "mm3_mm"){
		MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm3_mm;
	}
	else if(measurementUnitStringValue == "pixel"){
		MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::pixel;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit", measurementUnitStringValue);)

		return false;
	}

	QVariant lengthData = model.GetData("Length", modelIndex);
	if (lengthData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Length");)

		return false;
	}
	Length = lengthData.toFloat();

	QVariant errorTypeData = model.GetData("ErrorType", modelIndex);
	if (errorTypeData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType");)

		return false;
	}
	QString errorTypeStringValue = errorTypeData.toString();
	if(errorTypeStringValue == "OK"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::OK;
	}
	else if(errorTypeStringValue == "TOO_WIDE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE;
	}
	else if(errorTypeStringValue == "TOLERANCE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE;
	}
	else if(errorTypeStringValue == "TOO_NARROW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW;
	}
	else if(errorTypeStringValue == "NO_DATA_GAP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP;
	}
	else if(errorTypeStringValue == "GAP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::GAP;
	}
	else if(errorTypeStringValue == "TOO_HIGH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH;
	}
	else if(errorTypeStringValue == "TOO_LOW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW;
	}
	else if(errorTypeStringValue == "TOO_FEW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW;
	}
	else if(errorTypeStringValue == "TOO_MUCH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER;
	}
	else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION;
	}
	else if(errorTypeStringValue == "MISSING_DOTS"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE;
	}
	else if(errorTypeStringValue == "AREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
	}
	else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME;
	}
	else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT;
	}
	else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
	}
	else if(errorTypeStringValue == "TOO_BRIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT;
	}
	else if(errorTypeStringValue == "TOO_DARK"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK;
	}
	else if(errorTypeStringValue == "COUNT_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW;
	}
	else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL;
	}
	else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR;
	}
	else if(errorTypeStringValue == "COLOR_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR;
	}
	else if(errorTypeStringValue == "PATTERN_COUNT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT;
	}
	else if(errorTypeStringValue == "PATTERN_POSITION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION;
	}
	else if(errorTypeStringValue == "PATTERN_ROTATION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION;
	}
	else if(errorTypeStringValue == "CENTERLINE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE;
	}
	else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP;
	}
	else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP;
	}
	else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW;
	}
	else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY;
	}
	else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND;
	}
	else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF;
	}
	else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED;
	}
	else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED;
	}
	else if(errorTypeStringValue == "AREA_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* geometryDataModelPtr = model.GetTreeItemModel("Geometry", modelIndex);
	if (geometryDataModelPtr == nullptr){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is missing, but required").arg(__FILE__, QString::number(__LINE__), "Geometry");)

		return false;
	}
	Geometry = CTTypes::CGeometry::V1_0();
	const bool isGeometryReaded = Geometry->ReadFromModel(*geometryDataModelPtr, modelIndex);
	if (!isGeometryReaded){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry");)

		return false;
	}


	return true;
}


bool CResult::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant areaResultData = model.GetData("AreaResult", modelIndex);
	if (!areaResultData.isNull()){
		QString areaResultStringValue = areaResultData.toString();
		if(areaResultStringValue == "NONE"){
			AreaResult = ::sdl::complextest::CTTypes::StatusCode::NONE;
		}
		else if(areaResultStringValue == "OK"){
			AreaResult = ::sdl::complextest::CTTypes::StatusCode::OK;
		}
		else if(areaResultStringValue == "WARNING"){
			AreaResult = ::sdl::complextest::CTTypes::StatusCode::WARNING;
		}
		else if(areaResultStringValue == "NOK"){
			AreaResult = ::sdl::complextest::CTTypes::StatusCode::NOK;
		}
		else if(areaResultStringValue == "FAILED"){
			AreaResult = ::sdl::complextest::CTTypes::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", areaResultStringValue);)

			return false;
		}
	}

	QVariant expectedMinValueData = model.GetData("ExpectedMinValue", modelIndex);
	if (!expectedMinValueData.isNull()){
		ExpectedMinValue = expectedMinValueData.toFloat();
	}

	QVariant expectedMaxValueData = model.GetData("ExpectedMaxValue", modelIndex);
	if (!expectedMaxValueData.isNull()){
		ExpectedMaxValue = expectedMaxValueData.toFloat();
	}

	QVariant measuredValueData = model.GetData("MeasuredValue", modelIndex);
	if (!measuredValueData.isNull()){
		MeasuredValue = measuredValueData.toFloat();
	}

	QVariant minMeasuredValueData = model.GetData("MinMeasuredValue", modelIndex);
	if (!minMeasuredValueData.isNull()){
		MinMeasuredValue = minMeasuredValueData.toFloat();
	}

	QVariant maxMeasuredValueData = model.GetData("MaxMeasuredValue", modelIndex);
	if (!maxMeasuredValueData.isNull()){
		MaxMeasuredValue = maxMeasuredValueData.toFloat();
	}

	QVariant meanMeasuredValueData = model.GetData("MeanMeasuredValue", modelIndex);
	if (!meanMeasuredValueData.isNull()){
		MeanMeasuredValue = meanMeasuredValueData.toFloat();
	}

	QVariant measurementTypeData = model.GetData("MeasurementType", modelIndex);
	if (!measurementTypeData.isNull()){
		QString measurementTypeStringValue = measurementTypeData.toString();
		if(measurementTypeStringValue == "NONE"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::NONE;
		}
		else if(measurementTypeStringValue == "WIDTH"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::WIDTH;
		}
		else if(measurementTypeStringValue == "BRIGHTNESS"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::BRIGHTNESS;
		}
		else if(measurementTypeStringValue == "DISTANCE"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::DISTANCE;
		}
		else if(measurementTypeStringValue == "DISPLACEMENT"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::DISPLACEMENT;
		}
		else if(measurementTypeStringValue == "HEIGHT"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::HEIGHT;
		}
		else if(measurementTypeStringValue == "SYMMETRY"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::SYMMETRY;
		}
		else if(measurementTypeStringValue == "VOLUME"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::VOLUME;
		}
		else if(measurementTypeStringValue == "VOLUMERATE"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::VOLUMERATE;
		}
		else if(measurementTypeStringValue == "LENGTH"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::LENGTH;
		}
		else if(measurementTypeStringValue == "OUT_OF_BOUND"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::OUT_OF_BOUND;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

			return false;
		}
	}

	QVariant measurementUnitData = model.GetData("MeasurementUnit", modelIndex);
	if (!measurementUnitData.isNull()){
		QString measurementUnitStringValue = measurementUnitData.toString();
		if(measurementUnitStringValue == "NONE"){
			MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::NONE;
		}
		else if(measurementUnitStringValue == "mm"){
			MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm;
		}
		else if(measurementUnitStringValue == "mm3"){
			MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm3;
		}
		else if(measurementUnitStringValue == "mm3_mm"){
			MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm3_mm;
		}
		else if(measurementUnitStringValue == "pixel"){
			MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::pixel;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit", measurementUnitStringValue);)

			return false;
		}
	}

	QVariant lengthData = model.GetData("Length", modelIndex);
	if (!lengthData.isNull()){
		Length = lengthData.toFloat();
	}

	QVariant errorTypeData = model.GetData("ErrorType", modelIndex);
	if (!errorTypeData.isNull()){
		QString errorTypeStringValue = errorTypeData.toString();
		if(errorTypeStringValue == "OK"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::OK;
		}
		else if(errorTypeStringValue == "TOO_WIDE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE;
		}
		else if(errorTypeStringValue == "TOLERANCE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE;
		}
		else if(errorTypeStringValue == "TOO_NARROW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW;
		}
		else if(errorTypeStringValue == "NO_DATA_GAP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP;
		}
		else if(errorTypeStringValue == "GAP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::GAP;
		}
		else if(errorTypeStringValue == "TOO_HIGH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH;
		}
		else if(errorTypeStringValue == "TOO_LOW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW;
		}
		else if(errorTypeStringValue == "TOO_FEW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW;
		}
		else if(errorTypeStringValue == "TOO_MUCH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER;
		}
		else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION;
		}
		else if(errorTypeStringValue == "MISSING_DOTS"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE;
		}
		else if(errorTypeStringValue == "AREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
		}
		else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME;
		}
		else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT;
		}
		else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
		}
		else if(errorTypeStringValue == "TOO_BRIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT;
		}
		else if(errorTypeStringValue == "TOO_DARK"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK;
		}
		else if(errorTypeStringValue == "COUNT_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW;
		}
		else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL;
		}
		else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR;
		}
		else if(errorTypeStringValue == "COLOR_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR;
		}
		else if(errorTypeStringValue == "PATTERN_COUNT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT;
		}
		else if(errorTypeStringValue == "PATTERN_POSITION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION;
		}
		else if(errorTypeStringValue == "PATTERN_ROTATION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION;
		}
		else if(errorTypeStringValue == "CENTERLINE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE;
		}
		else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP;
		}
		else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP;
		}
		else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW;
		}
		else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY;
		}
		else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND;
		}
		else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF;
		}
		else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED;
		}
		else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED;
		}
		else if(errorTypeStringValue == "AREA_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* geometryDataModelPtr = model.GetTreeItemModel("Geometry", modelIndex);
	if (geometryDataModelPtr != nullptr){
		Geometry = CTTypes::CGeometry::V1_0();
		const bool isGeometryReaded = Geometry->ReadFromModel(*geometryDataModelPtr, modelIndex);
		if (!isGeometryReaded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry");)

			return false;
		}
	}

	return true;
}


bool CResult::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!AreaResult){
		return false;
	}
	QString areaResultStringValue;
	switch (*AreaResult){
	case ::sdl::complextest::CTTypes::StatusCode::NONE:
		areaResultStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::OK:
		areaResultStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::WARNING:
		areaResultStringValue = "WARNING";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::NOK:
		areaResultStringValue = "NOK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::FAILED:
		areaResultStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("AreaResult", QVariant(areaResultStringValue));

	if (!ExpectedMinValue){
		return false;
	}
	gqlObject.InsertParam("ExpectedMinValue", QVariant(*ExpectedMinValue));

	if (!ExpectedMaxValue){
		return false;
	}
	gqlObject.InsertParam("ExpectedMaxValue", QVariant(*ExpectedMaxValue));

	if (!MeasuredValue){
		return false;
	}
	gqlObject.InsertParam("MeasuredValue", QVariant(*MeasuredValue));

	if (!MinMeasuredValue){
		return false;
	}
	gqlObject.InsertParam("MinMeasuredValue", QVariant(*MinMeasuredValue));

	if (!MaxMeasuredValue){
		return false;
	}
	gqlObject.InsertParam("MaxMeasuredValue", QVariant(*MaxMeasuredValue));

	if (!MeanMeasuredValue){
		return false;
	}
	gqlObject.InsertParam("MeanMeasuredValue", QVariant(*MeanMeasuredValue));

	if (!MeasurementType){
		return false;
	}
	QString measurementTypeStringValue;
	switch (*MeasurementType){
	case ::sdl::complextest::CTTypes::MeasurementType::NONE:
		measurementTypeStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::WIDTH:
		measurementTypeStringValue = "WIDTH";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::BRIGHTNESS:
		measurementTypeStringValue = "BRIGHTNESS";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::DISTANCE:
		measurementTypeStringValue = "DISTANCE";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::DISPLACEMENT:
		measurementTypeStringValue = "DISPLACEMENT";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::HEIGHT:
		measurementTypeStringValue = "HEIGHT";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::SYMMETRY:
		measurementTypeStringValue = "SYMMETRY";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::VOLUME:
		measurementTypeStringValue = "VOLUME";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::VOLUMERATE:
		measurementTypeStringValue = "VOLUMERATE";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::LENGTH:
		measurementTypeStringValue = "LENGTH";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::OUT_OF_BOUND:
		measurementTypeStringValue = "OUT_OF_BOUND";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("MeasurementType", QVariant(measurementTypeStringValue));

	if (!MeasurementUnit){
		return false;
	}
	QString measurementUnitStringValue;
	switch (*MeasurementUnit){
	case ::sdl::complextest::CTTypes::MeasurementUnit::NONE:
		measurementUnitStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::MeasurementUnit::mm:
		measurementUnitStringValue = "mm";
		break;
	case ::sdl::complextest::CTTypes::MeasurementUnit::mm3:
		measurementUnitStringValue = "mm3";
		break;
	case ::sdl::complextest::CTTypes::MeasurementUnit::mm3_mm:
		measurementUnitStringValue = "mm3_mm";
		break;
	case ::sdl::complextest::CTTypes::MeasurementUnit::pixel:
		measurementUnitStringValue = "pixel";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("MeasurementUnit", QVariant(measurementUnitStringValue));

	if (!Length){
		return false;
	}
	gqlObject.InsertParam("Length", QVariant(*Length));

	if (!ErrorType){
		return false;
	}
	QString errorTypeStringValue;
	switch (*ErrorType){
	case ::sdl::complextest::CTTypes::ErrorCode::OK:
		errorTypeStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE:
		errorTypeStringValue = "TOO_WIDE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE:
		errorTypeStringValue = "TOLERANCE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW:
		errorTypeStringValue = "TOO_NARROW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP:
		errorTypeStringValue = "NO_DATA_GAP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::GAP:
		errorTypeStringValue = "GAP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH:
		errorTypeStringValue = "TOO_HIGH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW:
		errorTypeStringValue = "TOO_LOW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW:
		errorTypeStringValue = "TOO_FEW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH:
		errorTypeStringValue = "TOO_MUCH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER:
		errorTypeStringValue = "WRONG_DOT_DIAMETER";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION:
		errorTypeStringValue = "WRONG_DOT_POSITION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS:
		errorTypeStringValue = "MISSING_DOTS";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE:
		errorTypeStringValue = "WRONG_DOT_DISTANCE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE:
		errorTypeStringValue = "AREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE:
		errorTypeStringValue = "TOTALAREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE:
		errorTypeStringValue = "BORDERAREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA:
		errorTypeStringValue = "WRONG_DOT_AREA";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT:
		errorTypeStringValue = "WRONG_DOT_AREA_ONHEIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME:
		errorTypeStringValue = "WRONG_DOT_VOLUME";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT:
		errorTypeStringValue = "WRONG_DOT_HEIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR:
		errorTypeStringValue = "CONTOUR_MEASUREMENT_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT:
		errorTypeStringValue = "TOO_BRIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK:
		errorTypeStringValue = "TOO_DARK";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR:
		errorTypeStringValue = "COUNT_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH:
		errorTypeStringValue = "Z_DISTANCE_HIGH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW:
		errorTypeStringValue = "Z_DISTANCE_LOW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL:
		errorTypeStringValue = "NOT_SYMMETRICAL";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR:
		errorTypeStringValue = "REFERENCE_3D_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR:
		errorTypeStringValue = "COLOR_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT:
		errorTypeStringValue = "PATTERN_COUNT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION:
		errorTypeStringValue = "PATTERN_POSITION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION:
		errorTypeStringValue = "PATTERN_ROTATION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE:
		errorTypeStringValue = "CENTERLINE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP:
		errorTypeStringValue = "CALIB_ERROR_COMP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP:
		errorTypeStringValue = "CALIB_INVALID_COMP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED:
		errorTypeStringValue = "CALIB_NOZZLE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND:
		errorTypeStringValue = "CONTOUR_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE:
		errorTypeStringValue = "CONTOUR_TOO_WIDE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW:
		errorTypeStringValue = "CONTOUR_TOO_NARROW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED:
		errorTypeStringValue = "CONTOUR_GLUE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY:
		errorTypeStringValue = "CONTOUR_INVALID_POLY";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND:
		errorTypeStringValue = "REFERENCE_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF:
		errorTypeStringValue = "REFERENCE_INVALID_REF";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED:
		errorTypeStringValue = "REFERENCE_POS_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED:
		errorTypeStringValue = "REFERENCE_DIST_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND:
		errorTypeStringValue = "AREA_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND:
		errorTypeStringValue = "AREA_GLUE_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED:
		errorTypeStringValue = "AREA_GLUE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH:
		errorTypeStringValue = "AREA_GAP_LENGTH";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("ErrorType", QVariant(errorTypeStringValue));

	if (!Geometry){
		return false;
	}
	::imtgql::CGqlParamObject geometryGqlObject;
	const bool isGeometryAdded = Geometry->WriteToGraphQlObject(geometryGqlObject);
	if (!isGeometryAdded){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry");)

		return false;
	}
	gqlObject.InsertParam("Geometry", geometryGqlObject);

	return true;
}


bool CResult::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("AreaResult") || (gqlObject["AreaResult"].userType() != QMetaType::QString && gqlObject["AreaResult"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaResult");)

		return false;
	}
	const QString areaResultStringValue = gqlObject["AreaResult"].toString();
	if(areaResultStringValue == "NONE"){
		AreaResult = ::sdl::complextest::CTTypes::StatusCode::NONE;
	}
	else if(areaResultStringValue == "OK"){
		AreaResult = ::sdl::complextest::CTTypes::StatusCode::OK;
	}
	else if(areaResultStringValue == "WARNING"){
		AreaResult = ::sdl::complextest::CTTypes::StatusCode::WARNING;
	}
	else if(areaResultStringValue == "NOK"){
		AreaResult = ::sdl::complextest::CTTypes::StatusCode::NOK;
	}
	else if(areaResultStringValue == "FAILED"){
		AreaResult = ::sdl::complextest::CTTypes::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", areaResultStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("ExpectedMinValue") || (gqlObject["ExpectedMinValue"].userType() != QMetaType::Float && gqlObject["ExpectedMinValue"].userType() != QMetaType::Double && gqlObject["ExpectedMinValue"].userType() != QMetaType::Int && gqlObject["ExpectedMinValue"].userType() != QMetaType::UInt && gqlObject["ExpectedMinValue"].userType() != QMetaType::LongLong && gqlObject["ExpectedMinValue"].userType() != QMetaType::ULongLong && gqlObject["ExpectedMinValue"].userType() != QMetaType::Long && gqlObject["ExpectedMinValue"].userType() != QMetaType::Short && gqlObject["ExpectedMinValue"].userType() != QMetaType::ULong && gqlObject["ExpectedMinValue"].userType() != QMetaType::UShort && gqlObject["ExpectedMinValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMinValue");)

		return false;
	}
	ExpectedMinValue = gqlObject["ExpectedMinValue"].toDouble();

	if (!gqlObject.ContainsParam("ExpectedMaxValue") || (gqlObject["ExpectedMaxValue"].userType() != QMetaType::Float && gqlObject["ExpectedMaxValue"].userType() != QMetaType::Double && gqlObject["ExpectedMaxValue"].userType() != QMetaType::Int && gqlObject["ExpectedMaxValue"].userType() != QMetaType::UInt && gqlObject["ExpectedMaxValue"].userType() != QMetaType::LongLong && gqlObject["ExpectedMaxValue"].userType() != QMetaType::ULongLong && gqlObject["ExpectedMaxValue"].userType() != QMetaType::Long && gqlObject["ExpectedMaxValue"].userType() != QMetaType::Short && gqlObject["ExpectedMaxValue"].userType() != QMetaType::ULong && gqlObject["ExpectedMaxValue"].userType() != QMetaType::UShort && gqlObject["ExpectedMaxValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMaxValue");)

		return false;
	}
	ExpectedMaxValue = gqlObject["ExpectedMaxValue"].toDouble();

	if (!gqlObject.ContainsParam("MeasuredValue") || (gqlObject["MeasuredValue"].userType() != QMetaType::Float && gqlObject["MeasuredValue"].userType() != QMetaType::Double && gqlObject["MeasuredValue"].userType() != QMetaType::Int && gqlObject["MeasuredValue"].userType() != QMetaType::UInt && gqlObject["MeasuredValue"].userType() != QMetaType::LongLong && gqlObject["MeasuredValue"].userType() != QMetaType::ULongLong && gqlObject["MeasuredValue"].userType() != QMetaType::Long && gqlObject["MeasuredValue"].userType() != QMetaType::Short && gqlObject["MeasuredValue"].userType() != QMetaType::ULong && gqlObject["MeasuredValue"].userType() != QMetaType::UShort && gqlObject["MeasuredValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasuredValue");)

		return false;
	}
	MeasuredValue = gqlObject["MeasuredValue"].toDouble();

	if (!gqlObject.ContainsParam("MinMeasuredValue") || (gqlObject["MinMeasuredValue"].userType() != QMetaType::Float && gqlObject["MinMeasuredValue"].userType() != QMetaType::Double && gqlObject["MinMeasuredValue"].userType() != QMetaType::Int && gqlObject["MinMeasuredValue"].userType() != QMetaType::UInt && gqlObject["MinMeasuredValue"].userType() != QMetaType::LongLong && gqlObject["MinMeasuredValue"].userType() != QMetaType::ULongLong && gqlObject["MinMeasuredValue"].userType() != QMetaType::Long && gqlObject["MinMeasuredValue"].userType() != QMetaType::Short && gqlObject["MinMeasuredValue"].userType() != QMetaType::ULong && gqlObject["MinMeasuredValue"].userType() != QMetaType::UShort && gqlObject["MinMeasuredValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MinMeasuredValue");)

		return false;
	}
	MinMeasuredValue = gqlObject["MinMeasuredValue"].toDouble();

	if (!gqlObject.ContainsParam("MaxMeasuredValue") || (gqlObject["MaxMeasuredValue"].userType() != QMetaType::Float && gqlObject["MaxMeasuredValue"].userType() != QMetaType::Double && gqlObject["MaxMeasuredValue"].userType() != QMetaType::Int && gqlObject["MaxMeasuredValue"].userType() != QMetaType::UInt && gqlObject["MaxMeasuredValue"].userType() != QMetaType::LongLong && gqlObject["MaxMeasuredValue"].userType() != QMetaType::ULongLong && gqlObject["MaxMeasuredValue"].userType() != QMetaType::Long && gqlObject["MaxMeasuredValue"].userType() != QMetaType::Short && gqlObject["MaxMeasuredValue"].userType() != QMetaType::ULong && gqlObject["MaxMeasuredValue"].userType() != QMetaType::UShort && gqlObject["MaxMeasuredValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MaxMeasuredValue");)

		return false;
	}
	MaxMeasuredValue = gqlObject["MaxMeasuredValue"].toDouble();

	if (!gqlObject.ContainsParam("MeanMeasuredValue") || (gqlObject["MeanMeasuredValue"].userType() != QMetaType::Float && gqlObject["MeanMeasuredValue"].userType() != QMetaType::Double && gqlObject["MeanMeasuredValue"].userType() != QMetaType::Int && gqlObject["MeanMeasuredValue"].userType() != QMetaType::UInt && gqlObject["MeanMeasuredValue"].userType() != QMetaType::LongLong && gqlObject["MeanMeasuredValue"].userType() != QMetaType::ULongLong && gqlObject["MeanMeasuredValue"].userType() != QMetaType::Long && gqlObject["MeanMeasuredValue"].userType() != QMetaType::Short && gqlObject["MeanMeasuredValue"].userType() != QMetaType::ULong && gqlObject["MeanMeasuredValue"].userType() != QMetaType::UShort && gqlObject["MeanMeasuredValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeanMeasuredValue");)

		return false;
	}
	MeanMeasuredValue = gqlObject["MeanMeasuredValue"].toDouble();

	if (!gqlObject.ContainsParam("MeasurementType") || (gqlObject["MeasurementType"].userType() != QMetaType::QString && gqlObject["MeasurementType"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementType");)

		return false;
	}
	const QString measurementTypeStringValue = gqlObject["MeasurementType"].toString();
	if(measurementTypeStringValue == "NONE"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::NONE;
	}
	else if(measurementTypeStringValue == "WIDTH"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::WIDTH;
	}
	else if(measurementTypeStringValue == "BRIGHTNESS"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::BRIGHTNESS;
	}
	else if(measurementTypeStringValue == "DISTANCE"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::DISTANCE;
	}
	else if(measurementTypeStringValue == "DISPLACEMENT"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::DISPLACEMENT;
	}
	else if(measurementTypeStringValue == "HEIGHT"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::HEIGHT;
	}
	else if(measurementTypeStringValue == "SYMMETRY"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::SYMMETRY;
	}
	else if(measurementTypeStringValue == "VOLUME"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::VOLUME;
	}
	else if(measurementTypeStringValue == "VOLUMERATE"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::VOLUMERATE;
	}
	else if(measurementTypeStringValue == "LENGTH"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::LENGTH;
	}
	else if(measurementTypeStringValue == "OUT_OF_BOUND"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::OUT_OF_BOUND;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("MeasurementUnit") || (gqlObject["MeasurementUnit"].userType() != QMetaType::QString && gqlObject["MeasurementUnit"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit");)

		return false;
	}
	const QString measurementUnitStringValue = gqlObject["MeasurementUnit"].toString();
	if(measurementUnitStringValue == "NONE"){
		MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::NONE;
	}
	else if(measurementUnitStringValue == "mm"){
		MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm;
	}
	else if(measurementUnitStringValue == "mm3"){
		MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm3;
	}
	else if(measurementUnitStringValue == "mm3_mm"){
		MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm3_mm;
	}
	else if(measurementUnitStringValue == "pixel"){
		MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::pixel;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit", measurementUnitStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("Length") || (gqlObject["Length"].userType() != QMetaType::Float && gqlObject["Length"].userType() != QMetaType::Double && gqlObject["Length"].userType() != QMetaType::Int && gqlObject["Length"].userType() != QMetaType::UInt && gqlObject["Length"].userType() != QMetaType::LongLong && gqlObject["Length"].userType() != QMetaType::ULongLong && gqlObject["Length"].userType() != QMetaType::Long && gqlObject["Length"].userType() != QMetaType::Short && gqlObject["Length"].userType() != QMetaType::ULong && gqlObject["Length"].userType() != QMetaType::UShort && gqlObject["Length"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Length");)

		return false;
	}
	Length = gqlObject["Length"].toDouble();

	if (!gqlObject.ContainsParam("ErrorType") || (gqlObject["ErrorType"].userType() != QMetaType::QString && gqlObject["ErrorType"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType");)

		return false;
	}
	const QString errorTypeStringValue = gqlObject["ErrorType"].toString();
	if(errorTypeStringValue == "OK"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::OK;
	}
	else if(errorTypeStringValue == "TOO_WIDE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE;
	}
	else if(errorTypeStringValue == "TOLERANCE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE;
	}
	else if(errorTypeStringValue == "TOO_NARROW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW;
	}
	else if(errorTypeStringValue == "NO_DATA_GAP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP;
	}
	else if(errorTypeStringValue == "GAP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::GAP;
	}
	else if(errorTypeStringValue == "TOO_HIGH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH;
	}
	else if(errorTypeStringValue == "TOO_LOW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW;
	}
	else if(errorTypeStringValue == "TOO_FEW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW;
	}
	else if(errorTypeStringValue == "TOO_MUCH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER;
	}
	else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION;
	}
	else if(errorTypeStringValue == "MISSING_DOTS"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE;
	}
	else if(errorTypeStringValue == "AREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
	}
	else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME;
	}
	else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT;
	}
	else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
	}
	else if(errorTypeStringValue == "TOO_BRIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT;
	}
	else if(errorTypeStringValue == "TOO_DARK"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK;
	}
	else if(errorTypeStringValue == "COUNT_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW;
	}
	else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL;
	}
	else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR;
	}
	else if(errorTypeStringValue == "COLOR_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR;
	}
	else if(errorTypeStringValue == "PATTERN_COUNT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT;
	}
	else if(errorTypeStringValue == "PATTERN_POSITION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION;
	}
	else if(errorTypeStringValue == "PATTERN_ROTATION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION;
	}
	else if(errorTypeStringValue == "CENTERLINE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE;
	}
	else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP;
	}
	else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP;
	}
	else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW;
	}
	else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY;
	}
	else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND;
	}
	else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF;
	}
	else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED;
	}
	else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED;
	}
	else if(errorTypeStringValue == "AREA_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("Geometry") || (gqlObject.GetParamArgumentObjectPtr("Geometry") == nullptr)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Geometry");)

		return false;
	}
	Geometry = CTTypes::CGeometry::V1_0();
	const bool isGeometryRead = Geometry->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("Geometry"));
	if (!isGeometryRead){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry");)

		return false;
	}

	return true;
}


bool CResult::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("AreaResult") && (gqlObject["AreaResult"].userType() == QMetaType::QString || gqlObject["AreaResult"].userType() == QMetaType::QByteArray)){
		const QString areaResultStringValue = gqlObject["AreaResult"].toString();
		if(areaResultStringValue == "NONE"){
			AreaResult = ::sdl::complextest::CTTypes::StatusCode::NONE;
		}
		else if(areaResultStringValue == "OK"){
			AreaResult = ::sdl::complextest::CTTypes::StatusCode::OK;
		}
		else if(areaResultStringValue == "WARNING"){
			AreaResult = ::sdl::complextest::CTTypes::StatusCode::WARNING;
		}
		else if(areaResultStringValue == "NOK"){
			AreaResult = ::sdl::complextest::CTTypes::StatusCode::NOK;
		}
		else if(areaResultStringValue == "FAILED"){
			AreaResult = ::sdl::complextest::CTTypes::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", areaResultStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("ExpectedMinValue") && (gqlObject["ExpectedMinValue"].userType() == QMetaType::Float || gqlObject["ExpectedMinValue"].userType() == QMetaType::Double || gqlObject["ExpectedMinValue"].userType() == QMetaType::Int || gqlObject["ExpectedMinValue"].userType() == QMetaType::UInt || gqlObject["ExpectedMinValue"].userType() == QMetaType::LongLong || gqlObject["ExpectedMinValue"].userType() == QMetaType::ULongLong || gqlObject["ExpectedMinValue"].userType() == QMetaType::Long || gqlObject["ExpectedMinValue"].userType() == QMetaType::Short || gqlObject["ExpectedMinValue"].userType() == QMetaType::ULong || gqlObject["ExpectedMinValue"].userType() == QMetaType::UShort || gqlObject["ExpectedMinValue"].userType() == QMetaType::UChar)){
		ExpectedMinValue = gqlObject["ExpectedMinValue"].toDouble();
	}

	if (gqlObject.ContainsParam("ExpectedMaxValue") && (gqlObject["ExpectedMaxValue"].userType() == QMetaType::Float || gqlObject["ExpectedMaxValue"].userType() == QMetaType::Double || gqlObject["ExpectedMaxValue"].userType() == QMetaType::Int || gqlObject["ExpectedMaxValue"].userType() == QMetaType::UInt || gqlObject["ExpectedMaxValue"].userType() == QMetaType::LongLong || gqlObject["ExpectedMaxValue"].userType() == QMetaType::ULongLong || gqlObject["ExpectedMaxValue"].userType() == QMetaType::Long || gqlObject["ExpectedMaxValue"].userType() == QMetaType::Short || gqlObject["ExpectedMaxValue"].userType() == QMetaType::ULong || gqlObject["ExpectedMaxValue"].userType() == QMetaType::UShort || gqlObject["ExpectedMaxValue"].userType() == QMetaType::UChar)){
		ExpectedMaxValue = gqlObject["ExpectedMaxValue"].toDouble();
	}

	if (gqlObject.ContainsParam("MeasuredValue") && (gqlObject["MeasuredValue"].userType() == QMetaType::Float || gqlObject["MeasuredValue"].userType() == QMetaType::Double || gqlObject["MeasuredValue"].userType() == QMetaType::Int || gqlObject["MeasuredValue"].userType() == QMetaType::UInt || gqlObject["MeasuredValue"].userType() == QMetaType::LongLong || gqlObject["MeasuredValue"].userType() == QMetaType::ULongLong || gqlObject["MeasuredValue"].userType() == QMetaType::Long || gqlObject["MeasuredValue"].userType() == QMetaType::Short || gqlObject["MeasuredValue"].userType() == QMetaType::ULong || gqlObject["MeasuredValue"].userType() == QMetaType::UShort || gqlObject["MeasuredValue"].userType() == QMetaType::UChar)){
		MeasuredValue = gqlObject["MeasuredValue"].toDouble();
	}

	if (gqlObject.ContainsParam("MinMeasuredValue") && (gqlObject["MinMeasuredValue"].userType() == QMetaType::Float || gqlObject["MinMeasuredValue"].userType() == QMetaType::Double || gqlObject["MinMeasuredValue"].userType() == QMetaType::Int || gqlObject["MinMeasuredValue"].userType() == QMetaType::UInt || gqlObject["MinMeasuredValue"].userType() == QMetaType::LongLong || gqlObject["MinMeasuredValue"].userType() == QMetaType::ULongLong || gqlObject["MinMeasuredValue"].userType() == QMetaType::Long || gqlObject["MinMeasuredValue"].userType() == QMetaType::Short || gqlObject["MinMeasuredValue"].userType() == QMetaType::ULong || gqlObject["MinMeasuredValue"].userType() == QMetaType::UShort || gqlObject["MinMeasuredValue"].userType() == QMetaType::UChar)){
		MinMeasuredValue = gqlObject["MinMeasuredValue"].toDouble();
	}

	if (gqlObject.ContainsParam("MaxMeasuredValue") && (gqlObject["MaxMeasuredValue"].userType() == QMetaType::Float || gqlObject["MaxMeasuredValue"].userType() == QMetaType::Double || gqlObject["MaxMeasuredValue"].userType() == QMetaType::Int || gqlObject["MaxMeasuredValue"].userType() == QMetaType::UInt || gqlObject["MaxMeasuredValue"].userType() == QMetaType::LongLong || gqlObject["MaxMeasuredValue"].userType() == QMetaType::ULongLong || gqlObject["MaxMeasuredValue"].userType() == QMetaType::Long || gqlObject["MaxMeasuredValue"].userType() == QMetaType::Short || gqlObject["MaxMeasuredValue"].userType() == QMetaType::ULong || gqlObject["MaxMeasuredValue"].userType() == QMetaType::UShort || gqlObject["MaxMeasuredValue"].userType() == QMetaType::UChar)){
		MaxMeasuredValue = gqlObject["MaxMeasuredValue"].toDouble();
	}

	if (gqlObject.ContainsParam("MeanMeasuredValue") && (gqlObject["MeanMeasuredValue"].userType() == QMetaType::Float || gqlObject["MeanMeasuredValue"].userType() == QMetaType::Double || gqlObject["MeanMeasuredValue"].userType() == QMetaType::Int || gqlObject["MeanMeasuredValue"].userType() == QMetaType::UInt || gqlObject["MeanMeasuredValue"].userType() == QMetaType::LongLong || gqlObject["MeanMeasuredValue"].userType() == QMetaType::ULongLong || gqlObject["MeanMeasuredValue"].userType() == QMetaType::Long || gqlObject["MeanMeasuredValue"].userType() == QMetaType::Short || gqlObject["MeanMeasuredValue"].userType() == QMetaType::ULong || gqlObject["MeanMeasuredValue"].userType() == QMetaType::UShort || gqlObject["MeanMeasuredValue"].userType() == QMetaType::UChar)){
		MeanMeasuredValue = gqlObject["MeanMeasuredValue"].toDouble();
	}

	if (gqlObject.ContainsParam("MeasurementType") && (gqlObject["MeasurementType"].userType() == QMetaType::QString || gqlObject["MeasurementType"].userType() == QMetaType::QByteArray)){
		const QString measurementTypeStringValue = gqlObject["MeasurementType"].toString();
		if(measurementTypeStringValue == "NONE"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::NONE;
		}
		else if(measurementTypeStringValue == "WIDTH"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::WIDTH;
		}
		else if(measurementTypeStringValue == "BRIGHTNESS"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::BRIGHTNESS;
		}
		else if(measurementTypeStringValue == "DISTANCE"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::DISTANCE;
		}
		else if(measurementTypeStringValue == "DISPLACEMENT"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::DISPLACEMENT;
		}
		else if(measurementTypeStringValue == "HEIGHT"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::HEIGHT;
		}
		else if(measurementTypeStringValue == "SYMMETRY"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::SYMMETRY;
		}
		else if(measurementTypeStringValue == "VOLUME"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::VOLUME;
		}
		else if(measurementTypeStringValue == "VOLUMERATE"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::VOLUMERATE;
		}
		else if(measurementTypeStringValue == "LENGTH"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::LENGTH;
		}
		else if(measurementTypeStringValue == "OUT_OF_BOUND"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::OUT_OF_BOUND;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("MeasurementUnit") && (gqlObject["MeasurementUnit"].userType() == QMetaType::QString || gqlObject["MeasurementUnit"].userType() == QMetaType::QByteArray)){
		const QString measurementUnitStringValue = gqlObject["MeasurementUnit"].toString();
		if(measurementUnitStringValue == "NONE"){
			MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::NONE;
		}
		else if(measurementUnitStringValue == "mm"){
			MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm;
		}
		else if(measurementUnitStringValue == "mm3"){
			MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm3;
		}
		else if(measurementUnitStringValue == "mm3_mm"){
			MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm3_mm;
		}
		else if(measurementUnitStringValue == "pixel"){
			MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::pixel;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit", measurementUnitStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("Length") && (gqlObject["Length"].userType() == QMetaType::Float || gqlObject["Length"].userType() == QMetaType::Double || gqlObject["Length"].userType() == QMetaType::Int || gqlObject["Length"].userType() == QMetaType::UInt || gqlObject["Length"].userType() == QMetaType::LongLong || gqlObject["Length"].userType() == QMetaType::ULongLong || gqlObject["Length"].userType() == QMetaType::Long || gqlObject["Length"].userType() == QMetaType::Short || gqlObject["Length"].userType() == QMetaType::ULong || gqlObject["Length"].userType() == QMetaType::UShort || gqlObject["Length"].userType() == QMetaType::UChar)){
		Length = gqlObject["Length"].toDouble();
	}

	if (gqlObject.ContainsParam("ErrorType") && (gqlObject["ErrorType"].userType() == QMetaType::QString || gqlObject["ErrorType"].userType() == QMetaType::QByteArray)){
		const QString errorTypeStringValue = gqlObject["ErrorType"].toString();
		if(errorTypeStringValue == "OK"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::OK;
		}
		else if(errorTypeStringValue == "TOO_WIDE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE;
		}
		else if(errorTypeStringValue == "TOLERANCE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE;
		}
		else if(errorTypeStringValue == "TOO_NARROW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW;
		}
		else if(errorTypeStringValue == "NO_DATA_GAP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP;
		}
		else if(errorTypeStringValue == "GAP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::GAP;
		}
		else if(errorTypeStringValue == "TOO_HIGH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH;
		}
		else if(errorTypeStringValue == "TOO_LOW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW;
		}
		else if(errorTypeStringValue == "TOO_FEW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW;
		}
		else if(errorTypeStringValue == "TOO_MUCH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER;
		}
		else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION;
		}
		else if(errorTypeStringValue == "MISSING_DOTS"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE;
		}
		else if(errorTypeStringValue == "AREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
		}
		else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME;
		}
		else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT;
		}
		else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
		}
		else if(errorTypeStringValue == "TOO_BRIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT;
		}
		else if(errorTypeStringValue == "TOO_DARK"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK;
		}
		else if(errorTypeStringValue == "COUNT_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW;
		}
		else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL;
		}
		else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR;
		}
		else if(errorTypeStringValue == "COLOR_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR;
		}
		else if(errorTypeStringValue == "PATTERN_COUNT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT;
		}
		else if(errorTypeStringValue == "PATTERN_POSITION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION;
		}
		else if(errorTypeStringValue == "PATTERN_ROTATION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION;
		}
		else if(errorTypeStringValue == "CENTERLINE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE;
		}
		else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP;
		}
		else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP;
		}
		else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW;
		}
		else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY;
		}
		else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND;
		}
		else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF;
		}
		else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED;
		}
		else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED;
		}
		else if(errorTypeStringValue == "AREA_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("Geometry") && (gqlObject.GetParamArgumentObjectPtr("Geometry") != nullptr)){
		Geometry = CTTypes::CGeometry::V1_0();
		const bool isGeometryRead = Geometry->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("Geometry"));
		if (!isGeometryRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry");)

			return false;
		}
	}

	return true;
}


bool CResult::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!AreaResult){
		return false;
	}
	QString areaResultStringValue;
	switch (*AreaResult){
	case ::sdl::complextest::CTTypes::StatusCode::NONE:
		areaResultStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::OK:
		areaResultStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::WARNING:
		areaResultStringValue = "WARNING";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::NOK:
		areaResultStringValue = "NOK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::FAILED:
		areaResultStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["AreaResult"] = QJsonValue::fromVariant(areaResultStringValue);

	if (!ExpectedMinValue){
		return false;
	}
	jsonObject["ExpectedMinValue"] = QJsonValue::fromVariant(*ExpectedMinValue);

	if (!ExpectedMaxValue){
		return false;
	}
	jsonObject["ExpectedMaxValue"] = QJsonValue::fromVariant(*ExpectedMaxValue);

	if (!MeasuredValue){
		return false;
	}
	jsonObject["MeasuredValue"] = QJsonValue::fromVariant(*MeasuredValue);

	if (!MinMeasuredValue){
		return false;
	}
	jsonObject["MinMeasuredValue"] = QJsonValue::fromVariant(*MinMeasuredValue);

	if (!MaxMeasuredValue){
		return false;
	}
	jsonObject["MaxMeasuredValue"] = QJsonValue::fromVariant(*MaxMeasuredValue);

	if (!MeanMeasuredValue){
		return false;
	}
	jsonObject["MeanMeasuredValue"] = QJsonValue::fromVariant(*MeanMeasuredValue);

	if (!MeasurementType){
		return false;
	}
	QString measurementTypeStringValue;
	switch (*MeasurementType){
	case ::sdl::complextest::CTTypes::MeasurementType::NONE:
		measurementTypeStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::WIDTH:
		measurementTypeStringValue = "WIDTH";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::BRIGHTNESS:
		measurementTypeStringValue = "BRIGHTNESS";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::DISTANCE:
		measurementTypeStringValue = "DISTANCE";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::DISPLACEMENT:
		measurementTypeStringValue = "DISPLACEMENT";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::HEIGHT:
		measurementTypeStringValue = "HEIGHT";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::SYMMETRY:
		measurementTypeStringValue = "SYMMETRY";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::VOLUME:
		measurementTypeStringValue = "VOLUME";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::VOLUMERATE:
		measurementTypeStringValue = "VOLUMERATE";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::LENGTH:
		measurementTypeStringValue = "LENGTH";
		break;
	case ::sdl::complextest::CTTypes::MeasurementType::OUT_OF_BOUND:
		measurementTypeStringValue = "OUT_OF_BOUND";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["MeasurementType"] = QJsonValue::fromVariant(measurementTypeStringValue);

	if (!MeasurementUnit){
		return false;
	}
	QString measurementUnitStringValue;
	switch (*MeasurementUnit){
	case ::sdl::complextest::CTTypes::MeasurementUnit::NONE:
		measurementUnitStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::MeasurementUnit::mm:
		measurementUnitStringValue = "mm";
		break;
	case ::sdl::complextest::CTTypes::MeasurementUnit::mm3:
		measurementUnitStringValue = "mm3";
		break;
	case ::sdl::complextest::CTTypes::MeasurementUnit::mm3_mm:
		measurementUnitStringValue = "mm3_mm";
		break;
	case ::sdl::complextest::CTTypes::MeasurementUnit::pixel:
		measurementUnitStringValue = "pixel";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["MeasurementUnit"] = QJsonValue::fromVariant(measurementUnitStringValue);

	if (!Length){
		return false;
	}
	jsonObject["Length"] = QJsonValue::fromVariant(*Length);

	if (!ErrorType){
		return false;
	}
	QString errorTypeStringValue;
	switch (*ErrorType){
	case ::sdl::complextest::CTTypes::ErrorCode::OK:
		errorTypeStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE:
		errorTypeStringValue = "TOO_WIDE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE:
		errorTypeStringValue = "TOLERANCE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW:
		errorTypeStringValue = "TOO_NARROW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP:
		errorTypeStringValue = "NO_DATA_GAP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::GAP:
		errorTypeStringValue = "GAP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH:
		errorTypeStringValue = "TOO_HIGH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW:
		errorTypeStringValue = "TOO_LOW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW:
		errorTypeStringValue = "TOO_FEW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH:
		errorTypeStringValue = "TOO_MUCH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER:
		errorTypeStringValue = "WRONG_DOT_DIAMETER";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION:
		errorTypeStringValue = "WRONG_DOT_POSITION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS:
		errorTypeStringValue = "MISSING_DOTS";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE:
		errorTypeStringValue = "WRONG_DOT_DISTANCE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE:
		errorTypeStringValue = "AREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE:
		errorTypeStringValue = "TOTALAREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE:
		errorTypeStringValue = "BORDERAREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA:
		errorTypeStringValue = "WRONG_DOT_AREA";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT:
		errorTypeStringValue = "WRONG_DOT_AREA_ONHEIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME:
		errorTypeStringValue = "WRONG_DOT_VOLUME";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT:
		errorTypeStringValue = "WRONG_DOT_HEIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR:
		errorTypeStringValue = "CONTOUR_MEASUREMENT_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT:
		errorTypeStringValue = "TOO_BRIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK:
		errorTypeStringValue = "TOO_DARK";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR:
		errorTypeStringValue = "COUNT_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH:
		errorTypeStringValue = "Z_DISTANCE_HIGH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW:
		errorTypeStringValue = "Z_DISTANCE_LOW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL:
		errorTypeStringValue = "NOT_SYMMETRICAL";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR:
		errorTypeStringValue = "REFERENCE_3D_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR:
		errorTypeStringValue = "COLOR_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT:
		errorTypeStringValue = "PATTERN_COUNT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION:
		errorTypeStringValue = "PATTERN_POSITION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION:
		errorTypeStringValue = "PATTERN_ROTATION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE:
		errorTypeStringValue = "CENTERLINE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP:
		errorTypeStringValue = "CALIB_ERROR_COMP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP:
		errorTypeStringValue = "CALIB_INVALID_COMP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED:
		errorTypeStringValue = "CALIB_NOZZLE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND:
		errorTypeStringValue = "CONTOUR_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE:
		errorTypeStringValue = "CONTOUR_TOO_WIDE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW:
		errorTypeStringValue = "CONTOUR_TOO_NARROW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED:
		errorTypeStringValue = "CONTOUR_GLUE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY:
		errorTypeStringValue = "CONTOUR_INVALID_POLY";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND:
		errorTypeStringValue = "REFERENCE_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF:
		errorTypeStringValue = "REFERENCE_INVALID_REF";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED:
		errorTypeStringValue = "REFERENCE_POS_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED:
		errorTypeStringValue = "REFERENCE_DIST_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND:
		errorTypeStringValue = "AREA_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND:
		errorTypeStringValue = "AREA_GLUE_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED:
		errorTypeStringValue = "AREA_GLUE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH:
		errorTypeStringValue = "AREA_GAP_LENGTH";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["ErrorType"] = QJsonValue::fromVariant(errorTypeStringValue);

	if (!Geometry){
		return false;
	}
	QJsonObject geometryJsonObject;
	const bool isGeometryAdded = Geometry->WriteToJsonObject(geometryJsonObject);
	if (!isGeometryAdded){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry");)

		return false;
	}
	jsonObject["Geometry"] = geometryJsonObject;

	return true;
}


bool CResult::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("AreaResult") || ! jsonObject["AreaResult"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaResult");)

		return false;
	}
	const QString areaResultStringValue = jsonObject["AreaResult"].toString();
	if(areaResultStringValue == "NONE"){
		AreaResult = ::sdl::complextest::CTTypes::StatusCode::NONE;
	}
	else if(areaResultStringValue == "OK"){
		AreaResult = ::sdl::complextest::CTTypes::StatusCode::OK;
	}
	else if(areaResultStringValue == "WARNING"){
		AreaResult = ::sdl::complextest::CTTypes::StatusCode::WARNING;
	}
	else if(areaResultStringValue == "NOK"){
		AreaResult = ::sdl::complextest::CTTypes::StatusCode::NOK;
	}
	else if(areaResultStringValue == "FAILED"){
		AreaResult = ::sdl::complextest::CTTypes::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", areaResultStringValue);)

		return false;
	}

	if (!jsonObject.contains("ExpectedMinValue") || ! jsonObject["ExpectedMinValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMinValue");)

		return false;
	}
	ExpectedMinValue = jsonObject["ExpectedMinValue"].toDouble();

	if (!jsonObject.contains("ExpectedMaxValue") || ! jsonObject["ExpectedMaxValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMaxValue");)

		return false;
	}
	ExpectedMaxValue = jsonObject["ExpectedMaxValue"].toDouble();

	if (!jsonObject.contains("MeasuredValue") || ! jsonObject["MeasuredValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasuredValue");)

		return false;
	}
	MeasuredValue = jsonObject["MeasuredValue"].toDouble();

	if (!jsonObject.contains("MinMeasuredValue") || ! jsonObject["MinMeasuredValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MinMeasuredValue");)

		return false;
	}
	MinMeasuredValue = jsonObject["MinMeasuredValue"].toDouble();

	if (!jsonObject.contains("MaxMeasuredValue") || ! jsonObject["MaxMeasuredValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MaxMeasuredValue");)

		return false;
	}
	MaxMeasuredValue = jsonObject["MaxMeasuredValue"].toDouble();

	if (!jsonObject.contains("MeanMeasuredValue") || ! jsonObject["MeanMeasuredValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeanMeasuredValue");)

		return false;
	}
	MeanMeasuredValue = jsonObject["MeanMeasuredValue"].toDouble();

	if (!jsonObject.contains("MeasurementType") || ! jsonObject["MeasurementType"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementType");)

		return false;
	}
	const QString measurementTypeStringValue = jsonObject["MeasurementType"].toString();
	if(measurementTypeStringValue == "NONE"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::NONE;
	}
	else if(measurementTypeStringValue == "WIDTH"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::WIDTH;
	}
	else if(measurementTypeStringValue == "BRIGHTNESS"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::BRIGHTNESS;
	}
	else if(measurementTypeStringValue == "DISTANCE"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::DISTANCE;
	}
	else if(measurementTypeStringValue == "DISPLACEMENT"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::DISPLACEMENT;
	}
	else if(measurementTypeStringValue == "HEIGHT"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::HEIGHT;
	}
	else if(measurementTypeStringValue == "SYMMETRY"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::SYMMETRY;
	}
	else if(measurementTypeStringValue == "VOLUME"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::VOLUME;
	}
	else if(measurementTypeStringValue == "VOLUMERATE"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::VOLUMERATE;
	}
	else if(measurementTypeStringValue == "LENGTH"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::LENGTH;
	}
	else if(measurementTypeStringValue == "OUT_OF_BOUND"){
		MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::OUT_OF_BOUND;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

		return false;
	}

	if (!jsonObject.contains("MeasurementUnit") || ! jsonObject["MeasurementUnit"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit");)

		return false;
	}
	const QString measurementUnitStringValue = jsonObject["MeasurementUnit"].toString();
	if(measurementUnitStringValue == "NONE"){
		MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::NONE;
	}
	else if(measurementUnitStringValue == "mm"){
		MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm;
	}
	else if(measurementUnitStringValue == "mm3"){
		MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm3;
	}
	else if(measurementUnitStringValue == "mm3_mm"){
		MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm3_mm;
	}
	else if(measurementUnitStringValue == "pixel"){
		MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::pixel;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit", measurementUnitStringValue);)

		return false;
	}

	if (!jsonObject.contains("Length") || ! jsonObject["Length"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Length");)

		return false;
	}
	Length = jsonObject["Length"].toDouble();

	if (!jsonObject.contains("ErrorType") || ! jsonObject["ErrorType"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType");)

		return false;
	}
	const QString errorTypeStringValue = jsonObject["ErrorType"].toString();
	if(errorTypeStringValue == "OK"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::OK;
	}
	else if(errorTypeStringValue == "TOO_WIDE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE;
	}
	else if(errorTypeStringValue == "TOLERANCE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE;
	}
	else if(errorTypeStringValue == "TOO_NARROW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW;
	}
	else if(errorTypeStringValue == "NO_DATA_GAP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP;
	}
	else if(errorTypeStringValue == "GAP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::GAP;
	}
	else if(errorTypeStringValue == "TOO_HIGH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH;
	}
	else if(errorTypeStringValue == "TOO_LOW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW;
	}
	else if(errorTypeStringValue == "TOO_FEW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW;
	}
	else if(errorTypeStringValue == "TOO_MUCH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER;
	}
	else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION;
	}
	else if(errorTypeStringValue == "MISSING_DOTS"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE;
	}
	else if(errorTypeStringValue == "AREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
	}
	else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME;
	}
	else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT;
	}
	else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
	}
	else if(errorTypeStringValue == "TOO_BRIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT;
	}
	else if(errorTypeStringValue == "TOO_DARK"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK;
	}
	else if(errorTypeStringValue == "COUNT_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW;
	}
	else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL;
	}
	else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR;
	}
	else if(errorTypeStringValue == "COLOR_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR;
	}
	else if(errorTypeStringValue == "PATTERN_COUNT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT;
	}
	else if(errorTypeStringValue == "PATTERN_POSITION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION;
	}
	else if(errorTypeStringValue == "PATTERN_ROTATION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION;
	}
	else if(errorTypeStringValue == "CENTERLINE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE;
	}
	else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP;
	}
	else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP;
	}
	else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW;
	}
	else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY;
	}
	else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND;
	}
	else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF;
	}
	else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED;
	}
	else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED;
	}
	else if(errorTypeStringValue == "AREA_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	if (!jsonObject.contains("Geometry") || ! jsonObject["Geometry"].isObject()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Geometry");)

		return false;
	}
	Geometry = CTTypes::CGeometry::V1_0();
	const bool isGeometryRead = Geometry->ReadFromJsonObject(jsonObject["Geometry"].toObject());
	if (!isGeometryRead){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry");)

		return false;
	}

	return true;
}


bool CResult::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("AreaResult") && jsonObject["AreaResult"].isString()){
		const QString areaResultStringValue = jsonObject["AreaResult"].toString();
		if(areaResultStringValue == "NONE"){
			AreaResult = ::sdl::complextest::CTTypes::StatusCode::NONE;
		}
		else if(areaResultStringValue == "OK"){
			AreaResult = ::sdl::complextest::CTTypes::StatusCode::OK;
		}
		else if(areaResultStringValue == "WARNING"){
			AreaResult = ::sdl::complextest::CTTypes::StatusCode::WARNING;
		}
		else if(areaResultStringValue == "NOK"){
			AreaResult = ::sdl::complextest::CTTypes::StatusCode::NOK;
		}
		else if(areaResultStringValue == "FAILED"){
			AreaResult = ::sdl::complextest::CTTypes::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", areaResultStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("ExpectedMinValue") && jsonObject["ExpectedMinValue"].isDouble()){
		ExpectedMinValue = jsonObject["ExpectedMinValue"].toDouble();
	}

	if (jsonObject.contains("ExpectedMaxValue") && jsonObject["ExpectedMaxValue"].isDouble()){
		ExpectedMaxValue = jsonObject["ExpectedMaxValue"].toDouble();
	}

	if (jsonObject.contains("MeasuredValue") && jsonObject["MeasuredValue"].isDouble()){
		MeasuredValue = jsonObject["MeasuredValue"].toDouble();
	}

	if (jsonObject.contains("MinMeasuredValue") && jsonObject["MinMeasuredValue"].isDouble()){
		MinMeasuredValue = jsonObject["MinMeasuredValue"].toDouble();
	}

	if (jsonObject.contains("MaxMeasuredValue") && jsonObject["MaxMeasuredValue"].isDouble()){
		MaxMeasuredValue = jsonObject["MaxMeasuredValue"].toDouble();
	}

	if (jsonObject.contains("MeanMeasuredValue") && jsonObject["MeanMeasuredValue"].isDouble()){
		MeanMeasuredValue = jsonObject["MeanMeasuredValue"].toDouble();
	}

	if (jsonObject.contains("MeasurementType") && jsonObject["MeasurementType"].isString()){
		const QString measurementTypeStringValue = jsonObject["MeasurementType"].toString();
		if(measurementTypeStringValue == "NONE"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::NONE;
		}
		else if(measurementTypeStringValue == "WIDTH"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::WIDTH;
		}
		else if(measurementTypeStringValue == "BRIGHTNESS"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::BRIGHTNESS;
		}
		else if(measurementTypeStringValue == "DISTANCE"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::DISTANCE;
		}
		else if(measurementTypeStringValue == "DISPLACEMENT"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::DISPLACEMENT;
		}
		else if(measurementTypeStringValue == "HEIGHT"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::HEIGHT;
		}
		else if(measurementTypeStringValue == "SYMMETRY"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::SYMMETRY;
		}
		else if(measurementTypeStringValue == "VOLUME"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::VOLUME;
		}
		else if(measurementTypeStringValue == "VOLUMERATE"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::VOLUMERATE;
		}
		else if(measurementTypeStringValue == "LENGTH"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::LENGTH;
		}
		else if(measurementTypeStringValue == "OUT_OF_BOUND"){
			MeasurementType = ::sdl::complextest::CTTypes::MeasurementType::OUT_OF_BOUND;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("MeasurementUnit") && jsonObject["MeasurementUnit"].isString()){
		const QString measurementUnitStringValue = jsonObject["MeasurementUnit"].toString();
		if(measurementUnitStringValue == "NONE"){
			MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::NONE;
		}
		else if(measurementUnitStringValue == "mm"){
			MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm;
		}
		else if(measurementUnitStringValue == "mm3"){
			MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm3;
		}
		else if(measurementUnitStringValue == "mm3_mm"){
			MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::mm3_mm;
		}
		else if(measurementUnitStringValue == "pixel"){
			MeasurementUnit = ::sdl::complextest::CTTypes::MeasurementUnit::pixel;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit", measurementUnitStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("Length") && jsonObject["Length"].isDouble()){
		Length = jsonObject["Length"].toDouble();
	}

	if (jsonObject.contains("ErrorType") && jsonObject["ErrorType"].isString()){
		const QString errorTypeStringValue = jsonObject["ErrorType"].toString();
		if(errorTypeStringValue == "OK"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::OK;
		}
		else if(errorTypeStringValue == "TOO_WIDE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE;
		}
		else if(errorTypeStringValue == "TOLERANCE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE;
		}
		else if(errorTypeStringValue == "TOO_NARROW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW;
		}
		else if(errorTypeStringValue == "NO_DATA_GAP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP;
		}
		else if(errorTypeStringValue == "GAP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::GAP;
		}
		else if(errorTypeStringValue == "TOO_HIGH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH;
		}
		else if(errorTypeStringValue == "TOO_LOW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW;
		}
		else if(errorTypeStringValue == "TOO_FEW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW;
		}
		else if(errorTypeStringValue == "TOO_MUCH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER;
		}
		else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION;
		}
		else if(errorTypeStringValue == "MISSING_DOTS"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE;
		}
		else if(errorTypeStringValue == "AREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
		}
		else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME;
		}
		else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT;
		}
		else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
		}
		else if(errorTypeStringValue == "TOO_BRIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT;
		}
		else if(errorTypeStringValue == "TOO_DARK"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK;
		}
		else if(errorTypeStringValue == "COUNT_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW;
		}
		else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL;
		}
		else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR;
		}
		else if(errorTypeStringValue == "COLOR_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR;
		}
		else if(errorTypeStringValue == "PATTERN_COUNT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT;
		}
		else if(errorTypeStringValue == "PATTERN_POSITION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION;
		}
		else if(errorTypeStringValue == "PATTERN_ROTATION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION;
		}
		else if(errorTypeStringValue == "CENTERLINE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE;
		}
		else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP;
		}
		else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP;
		}
		else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW;
		}
		else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY;
		}
		else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND;
		}
		else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF;
		}
		else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED;
		}
		else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED;
		}
		else if(errorTypeStringValue == "AREA_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("Geometry") && jsonObject["Geometry"].isObject()){
		Geometry = CTTypes::CGeometry::V1_0();
		const bool isGeometryRead = Geometry->OptReadFromJsonObject(jsonObject["Geometry"].toObject());
		if (!isGeometryRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry");)

			return false;
		}
	}

	return true;
}


// serialize methods

bool CResult::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CResult::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CResult::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CResult::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CResult::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CResult::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CResult::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CResult::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CResult::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


CResultObject::CResultObject(QObject* parent): ::imtbase::CItemModelBase(parent)			, m_geometryQObjectPtr(nullptr)
{
	Version_1_0.emplace();

	QObject::connect(this, &CResultObject::areaResultChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::expectedMinValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::expectedMaxValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::measuredValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::minMeasuredValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::maxMeasuredValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::meanMeasuredValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::measurementTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::measurementUnitChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::lengthChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::errorTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultObject::geometryChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString CResultObject::GetAreaResult()
{
	if (Version_1_0->AreaResult.has_value()){
		sdl::complextest::CTTypes::StatusCode valueType = Version_1_0->AreaResult.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::StatusCode>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QString();
}


void CResultObject::SetAreaResult(QString v)
{
	Version_1_0->AreaResult.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::StatusCode>();
	int key = metaEnum.keyToValue(v.toUtf8());
	if (key > -1){
		Version_1_0->AreaResult = (sdl::complextest::CTTypes::StatusCode)key;
	}
	areaResultChanged();
}


bool CResultObject::hasAreaResult()
{
	 return Version_1_0->AreaResult.HasValue();
}


double CResultObject::GetExpectedMinValue()
{
	if (Version_1_0->ExpectedMinValue.has_value()){
		return Version_1_0->ExpectedMinValue.value();
	}

	return 0;
}


void CResultObject::SetExpectedMinValue(double v)
{
	Version_1_0->ExpectedMinValue = v;
	expectedMinValueChanged();
}


bool CResultObject::hasExpectedMinValue()
{
	 return Version_1_0->ExpectedMinValue.HasValue();
}


double CResultObject::GetExpectedMaxValue()
{
	if (Version_1_0->ExpectedMaxValue.has_value()){
		return Version_1_0->ExpectedMaxValue.value();
	}

	return 0;
}


void CResultObject::SetExpectedMaxValue(double v)
{
	Version_1_0->ExpectedMaxValue = v;
	expectedMaxValueChanged();
}


bool CResultObject::hasExpectedMaxValue()
{
	 return Version_1_0->ExpectedMaxValue.HasValue();
}


double CResultObject::GetMeasuredValue()
{
	if (Version_1_0->MeasuredValue.has_value()){
		return Version_1_0->MeasuredValue.value();
	}

	return 0;
}


void CResultObject::SetMeasuredValue(double v)
{
	Version_1_0->MeasuredValue = v;
	measuredValueChanged();
}


bool CResultObject::hasMeasuredValue()
{
	 return Version_1_0->MeasuredValue.HasValue();
}


double CResultObject::GetMinMeasuredValue()
{
	if (Version_1_0->MinMeasuredValue.has_value()){
		return Version_1_0->MinMeasuredValue.value();
	}

	return 0;
}


void CResultObject::SetMinMeasuredValue(double v)
{
	Version_1_0->MinMeasuredValue = v;
	minMeasuredValueChanged();
}


bool CResultObject::hasMinMeasuredValue()
{
	 return Version_1_0->MinMeasuredValue.HasValue();
}


double CResultObject::GetMaxMeasuredValue()
{
	if (Version_1_0->MaxMeasuredValue.has_value()){
		return Version_1_0->MaxMeasuredValue.value();
	}

	return 0;
}


void CResultObject::SetMaxMeasuredValue(double v)
{
	Version_1_0->MaxMeasuredValue = v;
	maxMeasuredValueChanged();
}


bool CResultObject::hasMaxMeasuredValue()
{
	 return Version_1_0->MaxMeasuredValue.HasValue();
}


double CResultObject::GetMeanMeasuredValue()
{
	if (Version_1_0->MeanMeasuredValue.has_value()){
		return Version_1_0->MeanMeasuredValue.value();
	}

	return 0;
}


void CResultObject::SetMeanMeasuredValue(double v)
{
	Version_1_0->MeanMeasuredValue = v;
	meanMeasuredValueChanged();
}


bool CResultObject::hasMeanMeasuredValue()
{
	 return Version_1_0->MeanMeasuredValue.HasValue();
}


QString CResultObject::GetMeasurementType()
{
	if (Version_1_0->MeasurementType.has_value()){
		sdl::complextest::CTTypes::MeasurementType valueType = Version_1_0->MeasurementType.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::MeasurementType>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QString();
}


void CResultObject::SetMeasurementType(QString v)
{
	Version_1_0->MeasurementType.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::MeasurementType>();
	int key = metaEnum.keyToValue(v.toUtf8());
	if (key > -1){
		Version_1_0->MeasurementType = (sdl::complextest::CTTypes::MeasurementType)key;
	}
	measurementTypeChanged();
}


bool CResultObject::hasMeasurementType()
{
	 return Version_1_0->MeasurementType.HasValue();
}


QString CResultObject::GetMeasurementUnit()
{
	if (Version_1_0->MeasurementUnit.has_value()){
		sdl::complextest::CTTypes::MeasurementUnit valueType = Version_1_0->MeasurementUnit.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::MeasurementUnit>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QString();
}


void CResultObject::SetMeasurementUnit(QString v)
{
	Version_1_0->MeasurementUnit.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::MeasurementUnit>();
	int key = metaEnum.keyToValue(v.toUtf8());
	if (key > -1){
		Version_1_0->MeasurementUnit = (sdl::complextest::CTTypes::MeasurementUnit)key;
	}
	measurementUnitChanged();
}


bool CResultObject::hasMeasurementUnit()
{
	 return Version_1_0->MeasurementUnit.HasValue();
}


double CResultObject::GetLength()
{
	if (Version_1_0->Length.has_value()){
		return Version_1_0->Length.value();
	}

	return 0;
}


void CResultObject::SetLength(double v)
{
	Version_1_0->Length = v;
	lengthChanged();
}


bool CResultObject::hasLength()
{
	 return Version_1_0->Length.HasValue();
}


QString CResultObject::GetErrorType()
{
	if (Version_1_0->ErrorType.has_value()){
		sdl::complextest::CTTypes::ErrorCode valueType = Version_1_0->ErrorType.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::ErrorCode>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QString();
}


void CResultObject::SetErrorType(QString v)
{
	Version_1_0->ErrorType.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::ErrorCode>();
	int key = metaEnum.keyToValue(v.toUtf8());
	if (key > -1){
		Version_1_0->ErrorType = (sdl::complextest::CTTypes::ErrorCode)key;
	}
	errorTypeChanged();
}


bool CResultObject::hasErrorType()
{
	 return Version_1_0->ErrorType.HasValue();
}


sdl::complextest::CTTypes::CGeometryObject* CResultObject::GetGeometry()
{
	if (Version_1_0->Geometry.has_value()){
		if (!m_geometryQObjectPtr){
			m_geometryQObjectPtr = dynamic_cast<sdl::complextest::CTTypes::CGeometryObject*>(CreateObject("Geometry"));
			m_geometryQObjectPtr->Version_1_0 = Version_1_0->Geometry;
		}
		return m_geometryQObjectPtr;
	}

	return nullptr;
}


void CResultObject::SetGeometry(sdl::complextest::CTTypes::CGeometryObject* v)
{
	if (v){
		Version_1_0->Geometry = v->Version_1_0;
		m_geometryQObjectPtr = v;
	}
	else {
		Version_1_0->Geometry = nullptr;
	}

	geometryChanged();
}


bool CResultObject::hasGeometry()
{
	 return Version_1_0->Geometry.HasValue();
}


void CResultObject::createGeometry()
{	Version_1_0->Geometry.emplace();

}


QString CResultObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CResultObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CResultObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CResultObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QObject* CResultObject::CreateObject(const QString& key)
{
	if (key == "Geometry"){
		return new sdl::complextest::CTTypes::CGeometryObject(this);
	}
	return nullptr;
}


QString CResultObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "areaResult")){
		return "AreaResult";
	}
	if (propertyName == (QString("m_") + "expectedMinValue")){
		return "ExpectedMinValue";
	}
	if (propertyName == (QString("m_") + "expectedMaxValue")){
		return "ExpectedMaxValue";
	}
	if (propertyName == (QString("m_") + "measuredValue")){
		return "MeasuredValue";
	}
	if (propertyName == (QString("m_") + "minMeasuredValue")){
		return "MinMeasuredValue";
	}
	if (propertyName == (QString("m_") + "maxMeasuredValue")){
		return "MaxMeasuredValue";
	}
	if (propertyName == (QString("m_") + "meanMeasuredValue")){
		return "MeanMeasuredValue";
	}
	if (propertyName == (QString("m_") + "measurementType")){
		return "MeasurementType";
	}
	if (propertyName == (QString("m_") + "measurementUnit")){
		return "MeasurementUnit";
	}
	if (propertyName == (QString("m_") + "length")){
		return "Length";
	}
	if (propertyName == (QString("m_") + "errorType")){
		return "ErrorType";
	}
	if (propertyName == (QString("m_") + "geometry")){
		return "Geometry";
	}

	return propertyName;
}


} // namespace sdl::complextest::CTImpl




/// \file CArea.cpp

namespace sdl::complextest::CTImpl
{


QByteArray CArea::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CArea::V1_0::operator==(const V1_0& other) const
{
	return 
				AreaName == other.AreaName &&
				Status == other.Status &&
				IconPosition == other.IconPosition &&
				ErrorType == other.ErrorType &&
				Results == other.Results;
}


bool CArea::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (!AreaName){
		return false;
	}
	model.SetData("AreaName", *AreaName, modelIndex);

	if (!Status){
		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::complextest::CTTypes::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::FAILED:
		statusStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("Status", statusStringValue, modelIndex);


	if (!IconPosition){
		return false;
	}
	::imtbase::CTreeItemModel* iconPositionNewModelPtr = model.AddTreeModel("IconPosition", modelIndex);
	const bool isIconPositionAdded = IconPosition->WriteToModel(*iconPositionNewModelPtr, 0);
	if (!isIconPositionAdded){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition");)

		return false;
	}

	if (!ErrorType){
		return false;
	}
	QString errorTypeStringValue;
	switch (*ErrorType){
	case ::sdl::complextest::CTTypes::ErrorCode::OK:
		errorTypeStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE:
		errorTypeStringValue = "TOO_WIDE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE:
		errorTypeStringValue = "TOLERANCE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW:
		errorTypeStringValue = "TOO_NARROW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP:
		errorTypeStringValue = "NO_DATA_GAP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::GAP:
		errorTypeStringValue = "GAP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH:
		errorTypeStringValue = "TOO_HIGH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW:
		errorTypeStringValue = "TOO_LOW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW:
		errorTypeStringValue = "TOO_FEW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH:
		errorTypeStringValue = "TOO_MUCH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER:
		errorTypeStringValue = "WRONG_DOT_DIAMETER";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION:
		errorTypeStringValue = "WRONG_DOT_POSITION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS:
		errorTypeStringValue = "MISSING_DOTS";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE:
		errorTypeStringValue = "WRONG_DOT_DISTANCE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE:
		errorTypeStringValue = "AREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE:
		errorTypeStringValue = "TOTALAREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE:
		errorTypeStringValue = "BORDERAREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA:
		errorTypeStringValue = "WRONG_DOT_AREA";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT:
		errorTypeStringValue = "WRONG_DOT_AREA_ONHEIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME:
		errorTypeStringValue = "WRONG_DOT_VOLUME";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT:
		errorTypeStringValue = "WRONG_DOT_HEIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR:
		errorTypeStringValue = "CONTOUR_MEASUREMENT_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT:
		errorTypeStringValue = "TOO_BRIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK:
		errorTypeStringValue = "TOO_DARK";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR:
		errorTypeStringValue = "COUNT_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH:
		errorTypeStringValue = "Z_DISTANCE_HIGH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW:
		errorTypeStringValue = "Z_DISTANCE_LOW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL:
		errorTypeStringValue = "NOT_SYMMETRICAL";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR:
		errorTypeStringValue = "REFERENCE_3D_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR:
		errorTypeStringValue = "COLOR_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT:
		errorTypeStringValue = "PATTERN_COUNT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION:
		errorTypeStringValue = "PATTERN_POSITION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION:
		errorTypeStringValue = "PATTERN_ROTATION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE:
		errorTypeStringValue = "CENTERLINE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP:
		errorTypeStringValue = "CALIB_ERROR_COMP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP:
		errorTypeStringValue = "CALIB_INVALID_COMP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED:
		errorTypeStringValue = "CALIB_NOZZLE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND:
		errorTypeStringValue = "CONTOUR_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE:
		errorTypeStringValue = "CONTOUR_TOO_WIDE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW:
		errorTypeStringValue = "CONTOUR_TOO_NARROW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED:
		errorTypeStringValue = "CONTOUR_GLUE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY:
		errorTypeStringValue = "CONTOUR_INVALID_POLY";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND:
		errorTypeStringValue = "REFERENCE_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF:
		errorTypeStringValue = "REFERENCE_INVALID_REF";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED:
		errorTypeStringValue = "REFERENCE_POS_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED:
		errorTypeStringValue = "REFERENCE_DIST_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND:
		errorTypeStringValue = "AREA_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND:
		errorTypeStringValue = "AREA_GLUE_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED:
		errorTypeStringValue = "AREA_GLUE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH:
		errorTypeStringValue = "AREA_GAP_LENGTH";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("ErrorType", errorTypeStringValue, modelIndex);

	if (!Results){
		return false;
	}
	::imtbase::CTreeItemModel* newResultsModelPtr = model.AddTreeModel("Results", modelIndex);
	newResultsModelPtr->setIsArray(true);
	for (qsizetype resultsIndex = 0; resultsIndex < Results->size(); ++resultsIndex){
		newResultsModelPtr->InsertNewItem();
		if (!(Results->at(resultsIndex).WriteToModel(*newResultsModelPtr, resultsIndex))){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Results");)

			return false;
		}
	}

	return true;
}


bool CArea::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant areaNameData = model.GetData("AreaName", modelIndex);
	if (areaNameData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "AreaName");)

		return false;
	}
	AreaName = areaNameData.toString();

	QVariant statusData = model.GetData("Status", modelIndex);
	if (statusData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Status");)

		return false;
	}
	QString statusStringValue = statusData.toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* iconPositionDataModelPtr = model.GetTreeItemModel("IconPosition", modelIndex);
	if (iconPositionDataModelPtr == nullptr){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is missing, but required").arg(__FILE__, QString::number(__LINE__), "IconPosition");)

		return false;
	}
	IconPosition = CTTypes::CPoint::V1_0();
	const bool isIconPositionReaded = IconPosition->ReadFromModel(*iconPositionDataModelPtr, modelIndex);
	if (!isIconPositionReaded){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition");)

		return false;
	}


	QVariant errorTypeData = model.GetData("ErrorType", modelIndex);
	if (errorTypeData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType");)

		return false;
	}
	QString errorTypeStringValue = errorTypeData.toString();
	if(errorTypeStringValue == "OK"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::OK;
	}
	else if(errorTypeStringValue == "TOO_WIDE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE;
	}
	else if(errorTypeStringValue == "TOLERANCE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE;
	}
	else if(errorTypeStringValue == "TOO_NARROW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW;
	}
	else if(errorTypeStringValue == "NO_DATA_GAP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP;
	}
	else if(errorTypeStringValue == "GAP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::GAP;
	}
	else if(errorTypeStringValue == "TOO_HIGH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH;
	}
	else if(errorTypeStringValue == "TOO_LOW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW;
	}
	else if(errorTypeStringValue == "TOO_FEW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW;
	}
	else if(errorTypeStringValue == "TOO_MUCH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER;
	}
	else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION;
	}
	else if(errorTypeStringValue == "MISSING_DOTS"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE;
	}
	else if(errorTypeStringValue == "AREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
	}
	else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME;
	}
	else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT;
	}
	else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
	}
	else if(errorTypeStringValue == "TOO_BRIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT;
	}
	else if(errorTypeStringValue == "TOO_DARK"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK;
	}
	else if(errorTypeStringValue == "COUNT_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW;
	}
	else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL;
	}
	else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR;
	}
	else if(errorTypeStringValue == "COLOR_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR;
	}
	else if(errorTypeStringValue == "PATTERN_COUNT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT;
	}
	else if(errorTypeStringValue == "PATTERN_POSITION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION;
	}
	else if(errorTypeStringValue == "PATTERN_ROTATION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION;
	}
	else if(errorTypeStringValue == "CENTERLINE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE;
	}
	else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP;
	}
	else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP;
	}
	else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW;
	}
	else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY;
	}
	else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND;
	}
	else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF;
	}
	else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED;
	}
	else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED;
	}
	else if(errorTypeStringValue == "AREA_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* resultsModel = model.GetTreeItemModel("Results", modelIndex);
	if (resultsModel == nullptr){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is missing, but required.").arg(__FILE__, QString::number(__LINE__), "Results");)

		return false;
	}
	int resultsCount = resultsModel->GetItemsCount();
	QList<CResult::V1_0> resultsList;
	for (int resultsIndex = 0; resultsIndex < resultsCount; ++resultsIndex){
		CResult::V1_0 results;
		if (!results.ReadFromModel(*resultsModel, resultsIndex)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results");)

			return false;
		}
		resultsList << results;
	}
	Results = resultsList;


	return true;
}


bool CArea::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant areaNameData = model.GetData("AreaName", modelIndex);
	if (!areaNameData.isNull()){
		AreaName = areaNameData.toString();
	}

	QVariant statusData = model.GetData("Status", modelIndex);
	if (!statusData.isNull()){
		QString statusStringValue = statusData.toString();
		if(statusStringValue == "NONE"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* iconPositionDataModelPtr = model.GetTreeItemModel("IconPosition", modelIndex);
	if (iconPositionDataModelPtr != nullptr){
		IconPosition = CTTypes::CPoint::V1_0();
		const bool isIconPositionReaded = IconPosition->ReadFromModel(*iconPositionDataModelPtr, modelIndex);
		if (!isIconPositionReaded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition");)

			return false;
		}
	}

	QVariant errorTypeData = model.GetData("ErrorType", modelIndex);
	if (!errorTypeData.isNull()){
		QString errorTypeStringValue = errorTypeData.toString();
		if(errorTypeStringValue == "OK"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::OK;
		}
		else if(errorTypeStringValue == "TOO_WIDE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE;
		}
		else if(errorTypeStringValue == "TOLERANCE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE;
		}
		else if(errorTypeStringValue == "TOO_NARROW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW;
		}
		else if(errorTypeStringValue == "NO_DATA_GAP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP;
		}
		else if(errorTypeStringValue == "GAP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::GAP;
		}
		else if(errorTypeStringValue == "TOO_HIGH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH;
		}
		else if(errorTypeStringValue == "TOO_LOW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW;
		}
		else if(errorTypeStringValue == "TOO_FEW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW;
		}
		else if(errorTypeStringValue == "TOO_MUCH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER;
		}
		else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION;
		}
		else if(errorTypeStringValue == "MISSING_DOTS"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE;
		}
		else if(errorTypeStringValue == "AREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
		}
		else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME;
		}
		else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT;
		}
		else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
		}
		else if(errorTypeStringValue == "TOO_BRIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT;
		}
		else if(errorTypeStringValue == "TOO_DARK"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK;
		}
		else if(errorTypeStringValue == "COUNT_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW;
		}
		else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL;
		}
		else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR;
		}
		else if(errorTypeStringValue == "COLOR_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR;
		}
		else if(errorTypeStringValue == "PATTERN_COUNT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT;
		}
		else if(errorTypeStringValue == "PATTERN_POSITION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION;
		}
		else if(errorTypeStringValue == "PATTERN_ROTATION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION;
		}
		else if(errorTypeStringValue == "CENTERLINE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE;
		}
		else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP;
		}
		else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP;
		}
		else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW;
		}
		else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY;
		}
		else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND;
		}
		else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF;
		}
		else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED;
		}
		else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED;
		}
		else if(errorTypeStringValue == "AREA_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultsModel = model.GetTreeItemModel("Results", modelIndex);
	if (resultsModel != nullptr){
		int resultsCount = resultsModel->GetItemsCount();
		QList<CResult::V1_0> resultsList;
		for (int resultsIndex = 0; resultsIndex < resultsCount; ++resultsIndex){
			CResult::V1_0 results;
			if (!results.OptReadFromModel(*resultsModel, resultsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results");)

				return false;
			}
			resultsList << results;
		}
		Results = resultsList;

	}

	return true;
}


bool CArea::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!AreaName){
		return false;
	}
	gqlObject.InsertParam("AreaName", QVariant(*AreaName));

	if (!Status){
		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::complextest::CTTypes::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::FAILED:
		statusStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("Status", QVariant(statusStringValue));

	if (!IconPosition){
		return false;
	}
	::imtgql::CGqlParamObject iconPositionGqlObject;
	const bool isIconPositionAdded = IconPosition->WriteToGraphQlObject(iconPositionGqlObject);
	if (!isIconPositionAdded){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition");)

		return false;
	}
	gqlObject.InsertParam("IconPosition", iconPositionGqlObject);

	if (!ErrorType){
		return false;
	}
	QString errorTypeStringValue;
	switch (*ErrorType){
	case ::sdl::complextest::CTTypes::ErrorCode::OK:
		errorTypeStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE:
		errorTypeStringValue = "TOO_WIDE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE:
		errorTypeStringValue = "TOLERANCE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW:
		errorTypeStringValue = "TOO_NARROW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP:
		errorTypeStringValue = "NO_DATA_GAP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::GAP:
		errorTypeStringValue = "GAP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH:
		errorTypeStringValue = "TOO_HIGH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW:
		errorTypeStringValue = "TOO_LOW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW:
		errorTypeStringValue = "TOO_FEW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH:
		errorTypeStringValue = "TOO_MUCH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER:
		errorTypeStringValue = "WRONG_DOT_DIAMETER";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION:
		errorTypeStringValue = "WRONG_DOT_POSITION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS:
		errorTypeStringValue = "MISSING_DOTS";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE:
		errorTypeStringValue = "WRONG_DOT_DISTANCE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE:
		errorTypeStringValue = "AREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE:
		errorTypeStringValue = "TOTALAREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE:
		errorTypeStringValue = "BORDERAREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA:
		errorTypeStringValue = "WRONG_DOT_AREA";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT:
		errorTypeStringValue = "WRONG_DOT_AREA_ONHEIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME:
		errorTypeStringValue = "WRONG_DOT_VOLUME";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT:
		errorTypeStringValue = "WRONG_DOT_HEIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR:
		errorTypeStringValue = "CONTOUR_MEASUREMENT_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT:
		errorTypeStringValue = "TOO_BRIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK:
		errorTypeStringValue = "TOO_DARK";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR:
		errorTypeStringValue = "COUNT_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH:
		errorTypeStringValue = "Z_DISTANCE_HIGH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW:
		errorTypeStringValue = "Z_DISTANCE_LOW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL:
		errorTypeStringValue = "NOT_SYMMETRICAL";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR:
		errorTypeStringValue = "REFERENCE_3D_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR:
		errorTypeStringValue = "COLOR_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT:
		errorTypeStringValue = "PATTERN_COUNT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION:
		errorTypeStringValue = "PATTERN_POSITION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION:
		errorTypeStringValue = "PATTERN_ROTATION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE:
		errorTypeStringValue = "CENTERLINE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP:
		errorTypeStringValue = "CALIB_ERROR_COMP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP:
		errorTypeStringValue = "CALIB_INVALID_COMP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED:
		errorTypeStringValue = "CALIB_NOZZLE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND:
		errorTypeStringValue = "CONTOUR_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE:
		errorTypeStringValue = "CONTOUR_TOO_WIDE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW:
		errorTypeStringValue = "CONTOUR_TOO_NARROW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED:
		errorTypeStringValue = "CONTOUR_GLUE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY:
		errorTypeStringValue = "CONTOUR_INVALID_POLY";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND:
		errorTypeStringValue = "REFERENCE_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF:
		errorTypeStringValue = "REFERENCE_INVALID_REF";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED:
		errorTypeStringValue = "REFERENCE_POS_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED:
		errorTypeStringValue = "REFERENCE_DIST_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND:
		errorTypeStringValue = "AREA_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND:
		errorTypeStringValue = "AREA_GLUE_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED:
		errorTypeStringValue = "AREA_GLUE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH:
		errorTypeStringValue = "AREA_GAP_LENGTH";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("ErrorType", QVariant(errorTypeStringValue));

	if (!Results){
		return false;
	}
	QList<::imtgql::CGqlParamObject> resultsDataObjectList;
	for (qsizetype resultsIndex = 0; resultsIndex < Results->size(); ++resultsIndex){
		::imtgql::CGqlParamObject newResultsGqlObject;
		if (!Results->at(resultsIndex).WriteToGraphQlObject(newResultsGqlObject)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Results");)

			return false;
		}
		resultsDataObjectList << newResultsGqlObject;
	}
	gqlObject.InsertParam("Results", resultsDataObjectList);

	return true;
}


bool CArea::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("AreaName") || (gqlObject["AreaName"].userType() != QMetaType::QString && gqlObject["AreaName"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaName");)

		return false;
	}
	AreaName = gqlObject["AreaName"].toString();

	if (!gqlObject.ContainsParam("Status") || (gqlObject["Status"].userType() != QMetaType::QString && gqlObject["Status"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status");)

		return false;
	}
	const QString statusStringValue = gqlObject["Status"].toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("IconPosition") || (gqlObject.GetParamArgumentObjectPtr("IconPosition") == nullptr)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "IconPosition");)

		return false;
	}
	IconPosition = CTTypes::CPoint::V1_0();
	const bool isIconPositionRead = IconPosition->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("IconPosition"));
	if (!isIconPositionRead){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition");)

		return false;
	}

	if (!gqlObject.ContainsParam("ErrorType") || (gqlObject["ErrorType"].userType() != QMetaType::QString && gqlObject["ErrorType"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType");)

		return false;
	}
	const QString errorTypeStringValue = gqlObject["ErrorType"].toString();
	if(errorTypeStringValue == "OK"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::OK;
	}
	else if(errorTypeStringValue == "TOO_WIDE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE;
	}
	else if(errorTypeStringValue == "TOLERANCE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE;
	}
	else if(errorTypeStringValue == "TOO_NARROW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW;
	}
	else if(errorTypeStringValue == "NO_DATA_GAP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP;
	}
	else if(errorTypeStringValue == "GAP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::GAP;
	}
	else if(errorTypeStringValue == "TOO_HIGH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH;
	}
	else if(errorTypeStringValue == "TOO_LOW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW;
	}
	else if(errorTypeStringValue == "TOO_FEW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW;
	}
	else if(errorTypeStringValue == "TOO_MUCH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER;
	}
	else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION;
	}
	else if(errorTypeStringValue == "MISSING_DOTS"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE;
	}
	else if(errorTypeStringValue == "AREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
	}
	else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME;
	}
	else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT;
	}
	else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
	}
	else if(errorTypeStringValue == "TOO_BRIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT;
	}
	else if(errorTypeStringValue == "TOO_DARK"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK;
	}
	else if(errorTypeStringValue == "COUNT_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW;
	}
	else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL;
	}
	else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR;
	}
	else if(errorTypeStringValue == "COLOR_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR;
	}
	else if(errorTypeStringValue == "PATTERN_COUNT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT;
	}
	else if(errorTypeStringValue == "PATTERN_POSITION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION;
	}
	else if(errorTypeStringValue == "PATTERN_ROTATION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION;
	}
	else if(errorTypeStringValue == "CENTERLINE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE;
	}
	else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP;
	}
	else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP;
	}
	else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW;
	}
	else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY;
	}
	else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND;
	}
	else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF;
	}
	else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED;
	}
	else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED;
	}
	else if(errorTypeStringValue == "AREA_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("Results") || (gqlObject.GetObjectsCount("Results") <= 0)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Results");)

		return false;
	}
	const qsizetype resultsElementsCount = gqlObject.GetObjectsCount("results");
	Results = QList<CResult::V1_0>();
	for (qsizetype resultsIndex = 0; resultsIndex < resultsElementsCount; ++resultsIndex){
		const ::imtgql::CGqlParamObject* resultsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Results", resultsIndex);
		if (resultsDataObjectPtr == nullptr){
			qDebug() << "invalid type" << resultsDataObjectPtr;
			return false;
		}
		CResult::V1_0 tempResults;
		if (!tempResults.ReadFromGraphQlObject(*resultsDataObjectPtr)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results");)

			return false;
		}
		Results->append(tempResults);
	}

	return true;
}


bool CArea::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("AreaName") && (gqlObject["AreaName"].userType() == QMetaType::QString || gqlObject["AreaName"].userType() == QMetaType::QByteArray)){
		AreaName = gqlObject["AreaName"].toString();
	}

	if (gqlObject.ContainsParam("Status") && (gqlObject["Status"].userType() == QMetaType::QString || gqlObject["Status"].userType() == QMetaType::QByteArray)){
		const QString statusStringValue = gqlObject["Status"].toString();
		if(statusStringValue == "NONE"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("IconPosition") && (gqlObject.GetParamArgumentObjectPtr("IconPosition") != nullptr)){
		IconPosition = CTTypes::CPoint::V1_0();
		const bool isIconPositionRead = IconPosition->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("IconPosition"));
		if (!isIconPositionRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition");)

			return false;
		}
	}

	if (gqlObject.ContainsParam("ErrorType") && (gqlObject["ErrorType"].userType() == QMetaType::QString || gqlObject["ErrorType"].userType() == QMetaType::QByteArray)){
		const QString errorTypeStringValue = gqlObject["ErrorType"].toString();
		if(errorTypeStringValue == "OK"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::OK;
		}
		else if(errorTypeStringValue == "TOO_WIDE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE;
		}
		else if(errorTypeStringValue == "TOLERANCE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE;
		}
		else if(errorTypeStringValue == "TOO_NARROW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW;
		}
		else if(errorTypeStringValue == "NO_DATA_GAP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP;
		}
		else if(errorTypeStringValue == "GAP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::GAP;
		}
		else if(errorTypeStringValue == "TOO_HIGH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH;
		}
		else if(errorTypeStringValue == "TOO_LOW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW;
		}
		else if(errorTypeStringValue == "TOO_FEW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW;
		}
		else if(errorTypeStringValue == "TOO_MUCH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER;
		}
		else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION;
		}
		else if(errorTypeStringValue == "MISSING_DOTS"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE;
		}
		else if(errorTypeStringValue == "AREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
		}
		else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME;
		}
		else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT;
		}
		else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
		}
		else if(errorTypeStringValue == "TOO_BRIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT;
		}
		else if(errorTypeStringValue == "TOO_DARK"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK;
		}
		else if(errorTypeStringValue == "COUNT_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW;
		}
		else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL;
		}
		else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR;
		}
		else if(errorTypeStringValue == "COLOR_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR;
		}
		else if(errorTypeStringValue == "PATTERN_COUNT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT;
		}
		else if(errorTypeStringValue == "PATTERN_POSITION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION;
		}
		else if(errorTypeStringValue == "PATTERN_ROTATION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION;
		}
		else if(errorTypeStringValue == "CENTERLINE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE;
		}
		else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP;
		}
		else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP;
		}
		else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW;
		}
		else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY;
		}
		else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND;
		}
		else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF;
		}
		else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED;
		}
		else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED;
		}
		else if(errorTypeStringValue == "AREA_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("Results") && (gqlObject.GetObjectsCount("Results") > 0)){
		const qsizetype resultsElementsCount = gqlObject.GetObjectsCount("results");
		Results = QList<CResult::V1_0>();
		for (qsizetype resultsIndex = 0; resultsIndex < resultsElementsCount; ++resultsIndex){
			const ::imtgql::CGqlParamObject* resultsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Results", resultsIndex);
			if (resultsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << resultsDataObjectPtr;
				return false;
			}
			CResult::V1_0 tempResults;
			if (!tempResults.OptReadFromGraphQlObject(*resultsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results");)

				return false;
			}
			Results->append(tempResults);
		}
	}

	return true;
}


bool CArea::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!AreaName){
		return false;
	}
	jsonObject["AreaName"] = QJsonValue::fromVariant(*AreaName);

	if (!Status){
		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::complextest::CTTypes::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::FAILED:
		statusStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["Status"] = QJsonValue::fromVariant(statusStringValue);

	if (!IconPosition){
		return false;
	}
	QJsonObject iconPositionJsonObject;
	const bool isIconPositionAdded = IconPosition->WriteToJsonObject(iconPositionJsonObject);
	if (!isIconPositionAdded){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition");)

		return false;
	}
	jsonObject["IconPosition"] = iconPositionJsonObject;

	if (!ErrorType){
		return false;
	}
	QString errorTypeStringValue;
	switch (*ErrorType){
	case ::sdl::complextest::CTTypes::ErrorCode::OK:
		errorTypeStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE:
		errorTypeStringValue = "TOO_WIDE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE:
		errorTypeStringValue = "TOLERANCE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW:
		errorTypeStringValue = "TOO_NARROW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP:
		errorTypeStringValue = "NO_DATA_GAP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::GAP:
		errorTypeStringValue = "GAP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH:
		errorTypeStringValue = "TOO_HIGH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW:
		errorTypeStringValue = "TOO_LOW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW:
		errorTypeStringValue = "TOO_FEW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH:
		errorTypeStringValue = "TOO_MUCH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER:
		errorTypeStringValue = "WRONG_DOT_DIAMETER";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION:
		errorTypeStringValue = "WRONG_DOT_POSITION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS:
		errorTypeStringValue = "MISSING_DOTS";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE:
		errorTypeStringValue = "WRONG_DOT_DISTANCE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE:
		errorTypeStringValue = "AREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE:
		errorTypeStringValue = "TOTALAREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE:
		errorTypeStringValue = "BORDERAREA_TOO_LARGE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA:
		errorTypeStringValue = "WRONG_DOT_AREA";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT:
		errorTypeStringValue = "WRONG_DOT_AREA_ONHEIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME:
		errorTypeStringValue = "WRONG_DOT_VOLUME";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT:
		errorTypeStringValue = "WRONG_DOT_HEIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR:
		errorTypeStringValue = "CONTOUR_MEASUREMENT_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT:
		errorTypeStringValue = "TOO_BRIGHT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK:
		errorTypeStringValue = "TOO_DARK";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR:
		errorTypeStringValue = "COUNT_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH:
		errorTypeStringValue = "Z_DISTANCE_HIGH";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW:
		errorTypeStringValue = "Z_DISTANCE_LOW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL:
		errorTypeStringValue = "NOT_SYMMETRICAL";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR:
		errorTypeStringValue = "REFERENCE_3D_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR:
		errorTypeStringValue = "COLOR_ERROR";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT:
		errorTypeStringValue = "PATTERN_COUNT";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION:
		errorTypeStringValue = "PATTERN_POSITION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION:
		errorTypeStringValue = "PATTERN_ROTATION";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE:
		errorTypeStringValue = "CENTERLINE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP:
		errorTypeStringValue = "CALIB_ERROR_COMP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP:
		errorTypeStringValue = "CALIB_INVALID_COMP";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED:
		errorTypeStringValue = "CALIB_NOZZLE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND:
		errorTypeStringValue = "CONTOUR_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE:
		errorTypeStringValue = "CONTOUR_TOO_WIDE";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW:
		errorTypeStringValue = "CONTOUR_TOO_NARROW";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED:
		errorTypeStringValue = "CONTOUR_GLUE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY:
		errorTypeStringValue = "CONTOUR_INVALID_POLY";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND:
		errorTypeStringValue = "REFERENCE_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF:
		errorTypeStringValue = "REFERENCE_INVALID_REF";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED:
		errorTypeStringValue = "REFERENCE_POS_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED:
		errorTypeStringValue = "REFERENCE_DIST_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND:
		errorTypeStringValue = "AREA_NOT_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND:
		errorTypeStringValue = "AREA_GLUE_FOUND";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED:
		errorTypeStringValue = "AREA_GLUE_MOVED";
		break;
	case ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH:
		errorTypeStringValue = "AREA_GAP_LENGTH";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["ErrorType"] = QJsonValue::fromVariant(errorTypeStringValue);

	if (!Results){
		return false;
	}
	QJsonArray newResultsArray;
	for (qsizetype resultsIndex = 0; resultsIndex < Results->size(); ++resultsIndex){
		QJsonObject newResultsJsonObject;
		if (!Results->at(resultsIndex).WriteToJsonObject(newResultsJsonObject)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Results");)

			return false;
		}
		newResultsArray << newResultsJsonObject;
	}
	jsonObject["Results"] = newResultsArray;

	return true;
}


bool CArea::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("AreaName") || ! jsonObject["AreaName"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaName");)

		return false;
	}
	AreaName = jsonObject["AreaName"].toString();

	if (!jsonObject.contains("Status") || ! jsonObject["Status"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status");)

		return false;
	}
	const QString statusStringValue = jsonObject["Status"].toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (!jsonObject.contains("IconPosition") || ! jsonObject["IconPosition"].isObject()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "IconPosition");)

		return false;
	}
	IconPosition = CTTypes::CPoint::V1_0();
	const bool isIconPositionRead = IconPosition->ReadFromJsonObject(jsonObject["IconPosition"].toObject());
	if (!isIconPositionRead){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition");)

		return false;
	}

	if (!jsonObject.contains("ErrorType") || ! jsonObject["ErrorType"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType");)

		return false;
	}
	const QString errorTypeStringValue = jsonObject["ErrorType"].toString();
	if(errorTypeStringValue == "OK"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::OK;
	}
	else if(errorTypeStringValue == "TOO_WIDE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE;
	}
	else if(errorTypeStringValue == "TOLERANCE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE;
	}
	else if(errorTypeStringValue == "TOO_NARROW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW;
	}
	else if(errorTypeStringValue == "NO_DATA_GAP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP;
	}
	else if(errorTypeStringValue == "GAP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::GAP;
	}
	else if(errorTypeStringValue == "TOO_HIGH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH;
	}
	else if(errorTypeStringValue == "TOO_LOW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW;
	}
	else if(errorTypeStringValue == "TOO_FEW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW;
	}
	else if(errorTypeStringValue == "TOO_MUCH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER;
	}
	else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION;
	}
	else if(errorTypeStringValue == "MISSING_DOTS"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE;
	}
	else if(errorTypeStringValue == "AREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
	}
	else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME;
	}
	else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT;
	}
	else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
	}
	else if(errorTypeStringValue == "TOO_BRIGHT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT;
	}
	else if(errorTypeStringValue == "TOO_DARK"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK;
	}
	else if(errorTypeStringValue == "COUNT_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW;
	}
	else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL;
	}
	else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR;
	}
	else if(errorTypeStringValue == "COLOR_ERROR"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR;
	}
	else if(errorTypeStringValue == "PATTERN_COUNT"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT;
	}
	else if(errorTypeStringValue == "PATTERN_POSITION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION;
	}
	else if(errorTypeStringValue == "PATTERN_ROTATION"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION;
	}
	else if(errorTypeStringValue == "CENTERLINE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE;
	}
	else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP;
	}
	else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP;
	}
	else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW;
	}
	else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY;
	}
	else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND;
	}
	else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF;
	}
	else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED;
	}
	else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED;
	}
	else if(errorTypeStringValue == "AREA_NOT_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
		ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	if (!jsonObject.contains("Results") || ! jsonObject["Results"].isArray()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Results");)

		return false;
	}
	const QJsonArray resultsJsonArray = jsonObject["Results"].toArray();
	const qsizetype resultsArrayCount = resultsJsonArray.size();
	Results = QList<CResult::V1_0>();
	for (qsizetype resultsIndex = 0; resultsIndex < resultsArrayCount; ++resultsIndex){
		CResult::V1_0 tempResults;
		if (!tempResults.ReadFromJsonObject(resultsJsonArray[resultsIndex].toObject())){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results");)

			return false;
		}
		Results->append(tempResults);
	}

	return true;
}


bool CArea::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("AreaName") && jsonObject["AreaName"].isString()){
		AreaName = jsonObject["AreaName"].toString();
	}

	if (jsonObject.contains("Status") && jsonObject["Status"].isString()){
		const QString statusStringValue = jsonObject["Status"].toString();
		if(statusStringValue == "NONE"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("IconPosition") && jsonObject["IconPosition"].isObject()){
		IconPosition = CTTypes::CPoint::V1_0();
		const bool isIconPositionRead = IconPosition->OptReadFromJsonObject(jsonObject["IconPosition"].toObject());
		if (!isIconPositionRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition");)

			return false;
		}
	}

	if (jsonObject.contains("ErrorType") && jsonObject["ErrorType"].isString()){
		const QString errorTypeStringValue = jsonObject["ErrorType"].toString();
		if(errorTypeStringValue == "OK"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::OK;
		}
		else if(errorTypeStringValue == "TOO_WIDE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_WIDE;
		}
		else if(errorTypeStringValue == "TOLERANCE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOLERANCE;
		}
		else if(errorTypeStringValue == "TOO_NARROW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_NARROW;
		}
		else if(errorTypeStringValue == "NO_DATA_GAP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NO_DATA_GAP;
		}
		else if(errorTypeStringValue == "GAP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::GAP;
		}
		else if(errorTypeStringValue == "TOO_HIGH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_HIGH;
		}
		else if(errorTypeStringValue == "TOO_LOW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_LOW;
		}
		else if(errorTypeStringValue == "TOO_FEW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_FEW;
		}
		else if(errorTypeStringValue == "TOO_MUCH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_MUCH;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DIAMETER;
		}
		else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_POSITION;
		}
		else if(errorTypeStringValue == "MISSING_DOTS"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::MISSING_DOTS;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_DISTANCE;
		}
		else if(errorTypeStringValue == "AREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOTALAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::BORDERAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
		}
		else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_VOLUME;
		}
		else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::WRONG_DOT_HEIGHT;
		}
		else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
		}
		else if(errorTypeStringValue == "TOO_BRIGHT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_BRIGHT;
		}
		else if(errorTypeStringValue == "TOO_DARK"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::TOO_DARK;
		}
		else if(errorTypeStringValue == "COUNT_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COUNT_ERROR;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_HIGH;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::Z_DISTANCE_LOW;
		}
		else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::NOT_SYMMETRICAL;
		}
		else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_3D_ERROR;
		}
		else if(errorTypeStringValue == "COLOR_ERROR"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::COLOR_ERROR;
		}
		else if(errorTypeStringValue == "PATTERN_COUNT"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_COUNT;
		}
		else if(errorTypeStringValue == "PATTERN_POSITION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_POSITION;
		}
		else if(errorTypeStringValue == "PATTERN_ROTATION"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::PATTERN_ROTATION;
		}
		else if(errorTypeStringValue == "CENTERLINE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CENTERLINE;
		}
		else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_ERROR_COMP;
		}
		else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_INVALID_COMP;
		}
		else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CALIB_NOZZLE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_NOT_FOUND;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_WIDE;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_TOO_NARROW;
		}
		else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::CONTOUR_INVALID_POLY;
		}
		else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_NOT_FOUND;
		}
		else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_INVALID_REF;
		}
		else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_POS_MOVED;
		}
		else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::REFERENCE_DIST_MOVED;
		}
		else if(errorTypeStringValue == "AREA_NOT_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_NOT_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
			ErrorType = ::sdl::complextest::CTTypes::ErrorCode::AREA_GAP_LENGTH;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("Results") && jsonObject["Results"].isArray()){
		const QJsonArray resultsJsonArray = jsonObject["Results"].toArray();
		const qsizetype resultsArrayCount = resultsJsonArray.size();
		Results = QList<CResult::V1_0>();
		for (qsizetype resultsIndex = 0; resultsIndex < resultsArrayCount; ++resultsIndex){
			CResult::V1_0 tempResults;
			if (!tempResults.OptReadFromJsonObject(resultsJsonArray[resultsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results");)

				return false;
			}
			Results->append(tempResults);
		}
	}

	return true;
}


// serialize methods

bool CArea::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CArea::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CArea::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CArea::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CArea::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CArea::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CArea::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CArea::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CArea::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


CAreaObject::CAreaObject(QObject* parent): ::imtbase::CItemModelBase(parent)			, m_iconPositionQObjectPtr(nullptr)
			, m_resultsQObjectPtr(nullptr)
{
	Version_1_0.emplace();

	QObject::connect(this, &CAreaObject::areaNameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAreaObject::statusChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAreaObject::iconPositionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAreaObject::errorTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAreaObject::resultsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString CAreaObject::GetAreaName()
{
	if (Version_1_0->AreaName.has_value()){
		return Version_1_0->AreaName.value();
	}

	return QString();
}


void CAreaObject::SetAreaName(QString v)
{
	Version_1_0->AreaName = v;
	areaNameChanged();
}


bool CAreaObject::hasAreaName()
{
	 return Version_1_0->AreaName.HasValue();
}


QString CAreaObject::GetStatus()
{
	if (Version_1_0->Status.has_value()){
		sdl::complextest::CTTypes::StatusCode valueType = Version_1_0->Status.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::StatusCode>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QString();
}


void CAreaObject::SetStatus(QString v)
{
	Version_1_0->Status.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::StatusCode>();
	int key = metaEnum.keyToValue(v.toUtf8());
	if (key > -1){
		Version_1_0->Status = (sdl::complextest::CTTypes::StatusCode)key;
	}
	statusChanged();
}


bool CAreaObject::hasStatus()
{
	 return Version_1_0->Status.HasValue();
}


sdl::complextest::CTTypes::CPointObject* CAreaObject::GetIconPosition()
{
	if (Version_1_0->IconPosition.has_value()){
		if (!m_iconPositionQObjectPtr){
			m_iconPositionQObjectPtr = dynamic_cast<sdl::complextest::CTTypes::CPointObject*>(CreateObject("IconPosition"));
			m_iconPositionQObjectPtr->Version_1_0 = Version_1_0->IconPosition;
		}
		return m_iconPositionQObjectPtr;
	}

	return nullptr;
}


void CAreaObject::SetIconPosition(sdl::complextest::CTTypes::CPointObject* v)
{
	if (v){
		Version_1_0->IconPosition = v->Version_1_0;
		m_iconPositionQObjectPtr = v;
	}
	else {
		Version_1_0->IconPosition = nullptr;
	}

	iconPositionChanged();
}


bool CAreaObject::hasIconPosition()
{
	 return Version_1_0->IconPosition.HasValue();
}


void CAreaObject::createIconPosition()
{	Version_1_0->IconPosition.emplace();

}


QString CAreaObject::GetErrorType()
{
	if (Version_1_0->ErrorType.has_value()){
		sdl::complextest::CTTypes::ErrorCode valueType = Version_1_0->ErrorType.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::ErrorCode>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QString();
}


void CAreaObject::SetErrorType(QString v)
{
	Version_1_0->ErrorType.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::ErrorCode>();
	int key = metaEnum.keyToValue(v.toUtf8());
	if (key > -1){
		Version_1_0->ErrorType = (sdl::complextest::CTTypes::ErrorCode)key;
	}
	errorTypeChanged();
}


bool CAreaObject::hasErrorType()
{
	 return Version_1_0->ErrorType.HasValue();
}


sdl::complextest::CTImpl::CResultObjectList* CAreaObject::GetResults()
{
	if (Version_1_0->Results.has_value()){
		if (!m_resultsQObjectPtr){
			m_resultsQObjectPtr = dynamic_cast<sdl::complextest::CTImpl::CResultObjectList*>(CreateObject("Results"));
			m_resultsQObjectPtr->Version_1_0 = Version_1_0->Results;
		}
		return m_resultsQObjectPtr;
	}

	return nullptr;
}


void CAreaObject::SetResults(sdl::complextest::CTImpl::CResultObjectList* v)
{
	if (v){
		Version_1_0->Results = v->Version_1_0;
		m_resultsQObjectPtr = v;
	}
	else {
		Version_1_0->Results = nullptr;
	}

	resultsChanged();
}


bool CAreaObject::hasResults()
{
	 return Version_1_0->Results.HasValue();
}


void CAreaObject::createResults()
{	Version_1_0->Results.emplace();

}


QString CAreaObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CAreaObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CAreaObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CAreaObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QObject* CAreaObject::CreateObject(const QString& key)
{
	if (key == "IconPosition"){
		return new sdl::complextest::CTTypes::CPointObject(this);
	}
	if (key == "Results"){
		return new sdl::complextest::CTImpl::CResultObjectList(this);
	}
	return nullptr;
}


QString CAreaObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "areaName")){
		return "AreaName";
	}
	if (propertyName == (QString("m_") + "status")){
		return "Status";
	}
	if (propertyName == (QString("m_") + "iconPosition")){
		return "IconPosition";
	}
	if (propertyName == (QString("m_") + "errorType")){
		return "ErrorType";
	}
	if (propertyName == (QString("m_") + "results")){
		return "Results";
	}

	return propertyName;
}


} // namespace sdl::complextest::CTImpl




/// \file CAnalyzer.cpp

namespace sdl::complextest::CTImpl
{


QByteArray CAnalyzer::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CAnalyzer::V1_0::operator==(const V1_0& other) const
{
	return 
				AnalyzerName == other.AnalyzerName &&
				AnalyzerResult == other.AnalyzerResult &&
				Areas == other.Areas;
}


bool CAnalyzer::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (!AnalyzerName){
		return false;
	}
	model.SetData("AnalyzerName", *AnalyzerName, modelIndex);

	if (!AnalyzerResult){
		return false;
	}
	QString analyzerResultStringValue;
	switch (*AnalyzerResult){
	case ::sdl::complextest::CTTypes::StatusCode::NONE:
		analyzerResultStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::OK:
		analyzerResultStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::WARNING:
		analyzerResultStringValue = "WARNING";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::NOK:
		analyzerResultStringValue = "NOK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::FAILED:
		analyzerResultStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("AnalyzerResult", analyzerResultStringValue, modelIndex);

	if (Areas){
		::imtbase::CTreeItemModel* newAreasModelPtr = model.AddTreeModel("Areas", modelIndex);
		newAreasModelPtr->setIsArray(true);
		for (qsizetype areasIndex = 0; areasIndex < Areas->size(); ++areasIndex){
			newAreasModelPtr->InsertNewItem();
			if (!(Areas->at(areasIndex).WriteToModel(*newAreasModelPtr, areasIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas");)

				return false;
			}
		}
	}

	return true;
}


bool CAnalyzer::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant analyzerNameData = model.GetData("AnalyzerName", modelIndex);
	if (analyzerNameData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerName");)

		return false;
	}
	AnalyzerName = analyzerNameData.toString();

	QVariant analyzerResultData = model.GetData("AnalyzerResult", modelIndex);
	if (analyzerResultData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerResult");)

		return false;
	}
	QString analyzerResultStringValue = analyzerResultData.toString();
	if(analyzerResultStringValue == "NONE"){
		AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::NONE;
	}
	else if(analyzerResultStringValue == "OK"){
		AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::OK;
	}
	else if(analyzerResultStringValue == "WARNING"){
		AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::WARNING;
	}
	else if(analyzerResultStringValue == "NOK"){
		AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::NOK;
	}
	else if(analyzerResultStringValue == "FAILED"){
		AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* areasModel = model.GetTreeItemModel("Areas", modelIndex);
	if (areasModel != nullptr){
		int areasCount = areasModel->GetItemsCount();
		QList<CArea::V1_0> areasList;
		for (int areasIndex = 0; areasIndex < areasCount; ++areasIndex){
			CArea::V1_0 areas;
			if (!areas.ReadFromModel(*areasModel, areasIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas");)

				return false;
			}
			areasList << areas;
		}
		Areas = areasList;

	}

	return true;
}


bool CAnalyzer::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant analyzerNameData = model.GetData("AnalyzerName", modelIndex);
	if (!analyzerNameData.isNull()){
		AnalyzerName = analyzerNameData.toString();
	}

	QVariant analyzerResultData = model.GetData("AnalyzerResult", modelIndex);
	if (!analyzerResultData.isNull()){
		QString analyzerResultStringValue = analyzerResultData.toString();
		if(analyzerResultStringValue == "NONE"){
			AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::NONE;
		}
		else if(analyzerResultStringValue == "OK"){
			AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::OK;
		}
		else if(analyzerResultStringValue == "WARNING"){
			AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::WARNING;
		}
		else if(analyzerResultStringValue == "NOK"){
			AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::NOK;
		}
		else if(analyzerResultStringValue == "FAILED"){
			AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* areasModel = model.GetTreeItemModel("Areas", modelIndex);
	if (areasModel != nullptr){
		int areasCount = areasModel->GetItemsCount();
		QList<CArea::V1_0> areasList;
		for (int areasIndex = 0; areasIndex < areasCount; ++areasIndex){
			CArea::V1_0 areas;
			if (!areas.OptReadFromModel(*areasModel, areasIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas");)

				return false;
			}
			areasList << areas;
		}
		Areas = areasList;

	}

	return true;
}


bool CAnalyzer::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!AnalyzerName){
		return false;
	}
	gqlObject.InsertParam("AnalyzerName", QVariant(*AnalyzerName));

	if (!AnalyzerResult){
		return false;
	}
	QString analyzerResultStringValue;
	switch (*AnalyzerResult){
	case ::sdl::complextest::CTTypes::StatusCode::NONE:
		analyzerResultStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::OK:
		analyzerResultStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::WARNING:
		analyzerResultStringValue = "WARNING";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::NOK:
		analyzerResultStringValue = "NOK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::FAILED:
		analyzerResultStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("AnalyzerResult", QVariant(analyzerResultStringValue));

	if (Areas){
		QList<::imtgql::CGqlParamObject> areasDataObjectList;
		for (qsizetype areasIndex = 0; areasIndex < Areas->size(); ++areasIndex){
			::imtgql::CGqlParamObject newAreasGqlObject;
			if (!Areas->at(areasIndex).WriteToGraphQlObject(newAreasGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Areas");)

				return false;
			}
			areasDataObjectList << newAreasGqlObject;
		}
		gqlObject.InsertParam("Areas", areasDataObjectList);
	}

	return true;
}


bool CAnalyzer::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("AnalyzerName") || (gqlObject["AnalyzerName"].userType() != QMetaType::QString && gqlObject["AnalyzerName"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerName");)

		return false;
	}
	AnalyzerName = gqlObject["AnalyzerName"].toString();

	if (!gqlObject.ContainsParam("AnalyzerResult") || (gqlObject["AnalyzerResult"].userType() != QMetaType::QString && gqlObject["AnalyzerResult"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerResult");)

		return false;
	}
	const QString analyzerResultStringValue = gqlObject["AnalyzerResult"].toString();
	if(analyzerResultStringValue == "NONE"){
		AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::NONE;
	}
	else if(analyzerResultStringValue == "OK"){
		AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::OK;
	}
	else if(analyzerResultStringValue == "WARNING"){
		AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::WARNING;
	}
	else if(analyzerResultStringValue == "NOK"){
		AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::NOK;
	}
	else if(analyzerResultStringValue == "FAILED"){
		AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

		return false;
	}

	if (gqlObject.ContainsParam("Areas") && (gqlObject.GetObjectsCount("Areas") > 0)){
		const qsizetype areasElementsCount = gqlObject.GetObjectsCount("areas");
		Areas = QList<CArea::V1_0>();
		for (qsizetype areasIndex = 0; areasIndex < areasElementsCount; ++areasIndex){
			const ::imtgql::CGqlParamObject* areasDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Areas", areasIndex);
			if (areasDataObjectPtr == nullptr){
				qDebug() << "invalid type" << areasDataObjectPtr;
				return false;
			}
			CArea::V1_0 tempAreas;
			if (!tempAreas.ReadFromGraphQlObject(*areasDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas");)

				return false;
			}
			Areas->append(tempAreas);
		}
	}

	return true;
}


bool CAnalyzer::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("AnalyzerName") && (gqlObject["AnalyzerName"].userType() == QMetaType::QString || gqlObject["AnalyzerName"].userType() == QMetaType::QByteArray)){
		AnalyzerName = gqlObject["AnalyzerName"].toString();
	}

	if (gqlObject.ContainsParam("AnalyzerResult") && (gqlObject["AnalyzerResult"].userType() == QMetaType::QString || gqlObject["AnalyzerResult"].userType() == QMetaType::QByteArray)){
		const QString analyzerResultStringValue = gqlObject["AnalyzerResult"].toString();
		if(analyzerResultStringValue == "NONE"){
			AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::NONE;
		}
		else if(analyzerResultStringValue == "OK"){
			AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::OK;
		}
		else if(analyzerResultStringValue == "WARNING"){
			AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::WARNING;
		}
		else if(analyzerResultStringValue == "NOK"){
			AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::NOK;
		}
		else if(analyzerResultStringValue == "FAILED"){
			AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("Areas") && (gqlObject.GetObjectsCount("Areas") > 0)){
		const qsizetype areasElementsCount = gqlObject.GetObjectsCount("areas");
		Areas = QList<CArea::V1_0>();
		for (qsizetype areasIndex = 0; areasIndex < areasElementsCount; ++areasIndex){
			const ::imtgql::CGqlParamObject* areasDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Areas", areasIndex);
			if (areasDataObjectPtr == nullptr){
				qDebug() << "invalid type" << areasDataObjectPtr;
				return false;
			}
			CArea::V1_0 tempAreas;
			if (!tempAreas.OptReadFromGraphQlObject(*areasDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas");)

				return false;
			}
			Areas->append(tempAreas);
		}
	}

	return true;
}


bool CAnalyzer::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!AnalyzerName){
		return false;
	}
	jsonObject["AnalyzerName"] = QJsonValue::fromVariant(*AnalyzerName);

	if (!AnalyzerResult){
		return false;
	}
	QString analyzerResultStringValue;
	switch (*AnalyzerResult){
	case ::sdl::complextest::CTTypes::StatusCode::NONE:
		analyzerResultStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::OK:
		analyzerResultStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::WARNING:
		analyzerResultStringValue = "WARNING";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::NOK:
		analyzerResultStringValue = "NOK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::FAILED:
		analyzerResultStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["AnalyzerResult"] = QJsonValue::fromVariant(analyzerResultStringValue);

	if (Areas){
		QJsonArray newAreasArray;
		for (qsizetype areasIndex = 0; areasIndex < Areas->size(); ++areasIndex){
			QJsonObject newAreasJsonObject;
			if (!Areas->at(areasIndex).WriteToJsonObject(newAreasJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Areas");)

				return false;
			}
			newAreasArray << newAreasJsonObject;
		}
		jsonObject["Areas"] = newAreasArray;
	}

	return true;
}


bool CAnalyzer::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("AnalyzerName") || ! jsonObject["AnalyzerName"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerName");)

		return false;
	}
	AnalyzerName = jsonObject["AnalyzerName"].toString();

	if (!jsonObject.contains("AnalyzerResult") || ! jsonObject["AnalyzerResult"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerResult");)

		return false;
	}
	const QString analyzerResultStringValue = jsonObject["AnalyzerResult"].toString();
	if(analyzerResultStringValue == "NONE"){
		AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::NONE;
	}
	else if(analyzerResultStringValue == "OK"){
		AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::OK;
	}
	else if(analyzerResultStringValue == "WARNING"){
		AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::WARNING;
	}
	else if(analyzerResultStringValue == "NOK"){
		AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::NOK;
	}
	else if(analyzerResultStringValue == "FAILED"){
		AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

		return false;
	}

	if (jsonObject.contains("Areas") && jsonObject["Areas"].isArray()){
		const QJsonArray areasJsonArray = jsonObject["Areas"].toArray();
		const qsizetype areasArrayCount = areasJsonArray.size();
		Areas = QList<CArea::V1_0>();
		for (qsizetype areasIndex = 0; areasIndex < areasArrayCount; ++areasIndex){
			CArea::V1_0 tempAreas;
			if (!tempAreas.ReadFromJsonObject(areasJsonArray[areasIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas");)

				return false;
			}
			Areas->append(tempAreas);
		}
	}

	return true;
}


bool CAnalyzer::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("AnalyzerName") && jsonObject["AnalyzerName"].isString()){
		AnalyzerName = jsonObject["AnalyzerName"].toString();
	}

	if (jsonObject.contains("AnalyzerResult") && jsonObject["AnalyzerResult"].isString()){
		const QString analyzerResultStringValue = jsonObject["AnalyzerResult"].toString();
		if(analyzerResultStringValue == "NONE"){
			AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::NONE;
		}
		else if(analyzerResultStringValue == "OK"){
			AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::OK;
		}
		else if(analyzerResultStringValue == "WARNING"){
			AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::WARNING;
		}
		else if(analyzerResultStringValue == "NOK"){
			AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::NOK;
		}
		else if(analyzerResultStringValue == "FAILED"){
			AnalyzerResult = ::sdl::complextest::CTTypes::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("Areas") && jsonObject["Areas"].isArray()){
		const QJsonArray areasJsonArray = jsonObject["Areas"].toArray();
		const qsizetype areasArrayCount = areasJsonArray.size();
		Areas = QList<CArea::V1_0>();
		for (qsizetype areasIndex = 0; areasIndex < areasArrayCount; ++areasIndex){
			CArea::V1_0 tempAreas;
			if (!tempAreas.OptReadFromJsonObject(areasJsonArray[areasIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas");)

				return false;
			}
			Areas->append(tempAreas);
		}
	}

	return true;
}


// serialize methods

bool CAnalyzer::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CAnalyzer::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CAnalyzer::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CAnalyzer::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CAnalyzer::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CAnalyzer::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CAnalyzer::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CAnalyzer::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CAnalyzer::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


CAnalyzerObject::CAnalyzerObject(QObject* parent): ::imtbase::CItemModelBase(parent)			, m_areasQObjectPtr(nullptr)
{
	Version_1_0.emplace();

	QObject::connect(this, &CAnalyzerObject::analyzerNameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAnalyzerObject::analyzerResultChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAnalyzerObject::areasChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString CAnalyzerObject::GetAnalyzerName()
{
	if (Version_1_0->AnalyzerName.has_value()){
		return Version_1_0->AnalyzerName.value();
	}

	return QString();
}


void CAnalyzerObject::SetAnalyzerName(QString v)
{
	Version_1_0->AnalyzerName = v;
	analyzerNameChanged();
}


bool CAnalyzerObject::hasAnalyzerName()
{
	 return Version_1_0->AnalyzerName.HasValue();
}


QString CAnalyzerObject::GetAnalyzerResult()
{
	if (Version_1_0->AnalyzerResult.has_value()){
		sdl::complextest::CTTypes::StatusCode valueType = Version_1_0->AnalyzerResult.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::StatusCode>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QString();
}


void CAnalyzerObject::SetAnalyzerResult(QString v)
{
	Version_1_0->AnalyzerResult.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::StatusCode>();
	int key = metaEnum.keyToValue(v.toUtf8());
	if (key > -1){
		Version_1_0->AnalyzerResult = (sdl::complextest::CTTypes::StatusCode)key;
	}
	analyzerResultChanged();
}


bool CAnalyzerObject::hasAnalyzerResult()
{
	 return Version_1_0->AnalyzerResult.HasValue();
}


sdl::complextest::CTImpl::CAreaObjectList* CAnalyzerObject::GetAreas()
{
	if (Version_1_0->Areas.has_value()){
		if (!m_areasQObjectPtr){
			m_areasQObjectPtr = dynamic_cast<sdl::complextest::CTImpl::CAreaObjectList*>(CreateObject("Areas"));
			m_areasQObjectPtr->Version_1_0 = Version_1_0->Areas;
		}
		return m_areasQObjectPtr;
	}

	return nullptr;
}


void CAnalyzerObject::SetAreas(sdl::complextest::CTImpl::CAreaObjectList* v)
{
	if (v){
		Version_1_0->Areas = v->Version_1_0;
		m_areasQObjectPtr = v;
	}
	else {
		Version_1_0->Areas = nullptr;
	}

	areasChanged();
}


bool CAnalyzerObject::hasAreas()
{
	 return Version_1_0->Areas.HasValue();
}


void CAnalyzerObject::createAreas()
{	Version_1_0->Areas.emplace();

}


QString CAnalyzerObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CAnalyzerObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CAnalyzerObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CAnalyzerObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QObject* CAnalyzerObject::CreateObject(const QString& key)
{
	if (key == "Areas"){
		return new sdl::complextest::CTImpl::CAreaObjectList(this);
	}
	return nullptr;
}


QString CAnalyzerObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "analyzerName")){
		return "AnalyzerName";
	}
	if (propertyName == (QString("m_") + "analyzerResult")){
		return "AnalyzerResult";
	}
	if (propertyName == (QString("m_") + "areas")){
		return "Areas";
	}

	return propertyName;
}


} // namespace sdl::complextest::CTImpl




/// \file CInspection.cpp

namespace sdl::complextest::CTImpl
{


QByteArray CInspection::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CInspection::V1_0::operator==(const V1_0& other) const
{
	return 
				ID == other.ID &&
				TypeID == other.TypeID &&
				Name == other.Name &&
				Status == other.Status &&
				Analyzers == other.Analyzers;
}


bool CInspection::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (!ID){
		return false;
	}
	model.SetData("ID", *ID, modelIndex);

	if (!TypeID){
		return false;
	}
	model.SetData("TypeID", *TypeID, modelIndex);

	if (!Name){
		return false;
	}
	model.SetData("Name", *Name, modelIndex);

	if (!Status){
		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::complextest::CTTypes::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::FAILED:
		statusStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("Status", statusStringValue, modelIndex);

	if (Analyzers){
		::imtbase::CTreeItemModel* newAnalyzersModelPtr = model.AddTreeModel("Analyzers", modelIndex);
		newAnalyzersModelPtr->setIsArray(true);
		for (qsizetype analyzersIndex = 0; analyzersIndex < Analyzers->size(); ++analyzersIndex){
			newAnalyzersModelPtr->InsertNewItem();
			if (!(Analyzers->at(analyzersIndex).WriteToModel(*newAnalyzersModelPtr, analyzersIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers");)

				return false;
			}
		}
	}

	return true;
}


bool CInspection::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant iDData = model.GetData("ID", modelIndex);
	if (iDData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ID");)

		return false;
	}
	ID = iDData.toString();

	QVariant typeIDData = model.GetData("TypeID", modelIndex);
	if (typeIDData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "TypeID");)

		return false;
	}
	TypeID = typeIDData.toString();

	QVariant nameData = model.GetData("Name", modelIndex);
	if (nameData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Name");)

		return false;
	}
	Name = nameData.toString();

	QVariant statusData = model.GetData("Status", modelIndex);
	if (statusData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Status");)

		return false;
	}
	QString statusStringValue = statusData.toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* analyzersModel = model.GetTreeItemModel("Analyzers", modelIndex);
	if (analyzersModel != nullptr){
		int analyzersCount = analyzersModel->GetItemsCount();
		QList<CAnalyzer::V1_0> analyzersList;
		for (int analyzersIndex = 0; analyzersIndex < analyzersCount; ++analyzersIndex){
			CAnalyzer::V1_0 analyzers;
			if (!analyzers.ReadFromModel(*analyzersModel, analyzersIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers");)

				return false;
			}
			analyzersList << analyzers;
		}
		Analyzers = analyzersList;

	}

	return true;
}


bool CInspection::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant iDData = model.GetData("ID", modelIndex);
	if (!iDData.isNull()){
		ID = iDData.toString();
	}

	QVariant typeIDData = model.GetData("TypeID", modelIndex);
	if (!typeIDData.isNull()){
		TypeID = typeIDData.toString();
	}

	QVariant nameData = model.GetData("Name", modelIndex);
	if (!nameData.isNull()){
		Name = nameData.toString();
	}

	QVariant statusData = model.GetData("Status", modelIndex);
	if (!statusData.isNull()){
		QString statusStringValue = statusData.toString();
		if(statusStringValue == "NONE"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* analyzersModel = model.GetTreeItemModel("Analyzers", modelIndex);
	if (analyzersModel != nullptr){
		int analyzersCount = analyzersModel->GetItemsCount();
		QList<CAnalyzer::V1_0> analyzersList;
		for (int analyzersIndex = 0; analyzersIndex < analyzersCount; ++analyzersIndex){
			CAnalyzer::V1_0 analyzers;
			if (!analyzers.OptReadFromModel(*analyzersModel, analyzersIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers");)

				return false;
			}
			analyzersList << analyzers;
		}
		Analyzers = analyzersList;

	}

	return true;
}


bool CInspection::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!ID){
		return false;
	}
	gqlObject.InsertParam("ID", QVariant(*ID));

	if (!TypeID){
		return false;
	}
	gqlObject.InsertParam("TypeID", QVariant(*TypeID));

	if (!Name){
		return false;
	}
	gqlObject.InsertParam("Name", QVariant(*Name));

	if (!Status){
		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::complextest::CTTypes::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::FAILED:
		statusStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("Status", QVariant(statusStringValue));

	if (Analyzers){
		QList<::imtgql::CGqlParamObject> analyzersDataObjectList;
		for (qsizetype analyzersIndex = 0; analyzersIndex < Analyzers->size(); ++analyzersIndex){
			::imtgql::CGqlParamObject newAnalyzersGqlObject;
			if (!Analyzers->at(analyzersIndex).WriteToGraphQlObject(newAnalyzersGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers");)

				return false;
			}
			analyzersDataObjectList << newAnalyzersGqlObject;
		}
		gqlObject.InsertParam("Analyzers", analyzersDataObjectList);
	}

	return true;
}


bool CInspection::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("ID") || (gqlObject["ID"].userType() != QMetaType::QString && gqlObject["ID"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ID");)

		return false;
	}
	ID = gqlObject["ID"].toString();

	if (!gqlObject.ContainsParam("TypeID") || (gqlObject["TypeID"].userType() != QMetaType::QString && gqlObject["TypeID"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "TypeID");)

		return false;
	}
	TypeID = gqlObject["TypeID"].toString();

	if (!gqlObject.ContainsParam("Name") || (gqlObject["Name"].userType() != QMetaType::QString && gqlObject["Name"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name");)

		return false;
	}
	Name = gqlObject["Name"].toString();

	if (!gqlObject.ContainsParam("Status") || (gqlObject["Status"].userType() != QMetaType::QString && gqlObject["Status"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status");)

		return false;
	}
	const QString statusStringValue = gqlObject["Status"].toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (gqlObject.ContainsParam("Analyzers") && (gqlObject.GetObjectsCount("Analyzers") > 0)){
		const qsizetype analyzersElementsCount = gqlObject.GetObjectsCount("analyzers");
		Analyzers = QList<CAnalyzer::V1_0>();
		for (qsizetype analyzersIndex = 0; analyzersIndex < analyzersElementsCount; ++analyzersIndex){
			const ::imtgql::CGqlParamObject* analyzersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Analyzers", analyzersIndex);
			if (analyzersDataObjectPtr == nullptr){
				qDebug() << "invalid type" << analyzersDataObjectPtr;
				return false;
			}
			CAnalyzer::V1_0 tempAnalyzers;
			if (!tempAnalyzers.ReadFromGraphQlObject(*analyzersDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers");)

				return false;
			}
			Analyzers->append(tempAnalyzers);
		}
	}

	return true;
}


bool CInspection::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("ID") && (gqlObject["ID"].userType() == QMetaType::QString || gqlObject["ID"].userType() == QMetaType::QByteArray)){
		ID = gqlObject["ID"].toString();
	}

	if (gqlObject.ContainsParam("TypeID") && (gqlObject["TypeID"].userType() == QMetaType::QString || gqlObject["TypeID"].userType() == QMetaType::QByteArray)){
		TypeID = gqlObject["TypeID"].toString();
	}

	if (gqlObject.ContainsParam("Name") && (gqlObject["Name"].userType() == QMetaType::QString || gqlObject["Name"].userType() == QMetaType::QByteArray)){
		Name = gqlObject["Name"].toString();
	}

	if (gqlObject.ContainsParam("Status") && (gqlObject["Status"].userType() == QMetaType::QString || gqlObject["Status"].userType() == QMetaType::QByteArray)){
		const QString statusStringValue = gqlObject["Status"].toString();
		if(statusStringValue == "NONE"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("Analyzers") && (gqlObject.GetObjectsCount("Analyzers") > 0)){
		const qsizetype analyzersElementsCount = gqlObject.GetObjectsCount("analyzers");
		Analyzers = QList<CAnalyzer::V1_0>();
		for (qsizetype analyzersIndex = 0; analyzersIndex < analyzersElementsCount; ++analyzersIndex){
			const ::imtgql::CGqlParamObject* analyzersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Analyzers", analyzersIndex);
			if (analyzersDataObjectPtr == nullptr){
				qDebug() << "invalid type" << analyzersDataObjectPtr;
				return false;
			}
			CAnalyzer::V1_0 tempAnalyzers;
			if (!tempAnalyzers.OptReadFromGraphQlObject(*analyzersDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers");)

				return false;
			}
			Analyzers->append(tempAnalyzers);
		}
	}

	return true;
}


bool CInspection::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!ID){
		return false;
	}
	jsonObject["ID"] = QJsonValue::fromVariant(*ID);

	if (!TypeID){
		return false;
	}
	jsonObject["TypeID"] = QJsonValue::fromVariant(*TypeID);

	if (!Name){
		return false;
	}
	jsonObject["Name"] = QJsonValue::fromVariant(*Name);

	if (!Status){
		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::complextest::CTTypes::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::FAILED:
		statusStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["Status"] = QJsonValue::fromVariant(statusStringValue);

	if (Analyzers){
		QJsonArray newAnalyzersArray;
		for (qsizetype analyzersIndex = 0; analyzersIndex < Analyzers->size(); ++analyzersIndex){
			QJsonObject newAnalyzersJsonObject;
			if (!Analyzers->at(analyzersIndex).WriteToJsonObject(newAnalyzersJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers");)

				return false;
			}
			newAnalyzersArray << newAnalyzersJsonObject;
		}
		jsonObject["Analyzers"] = newAnalyzersArray;
	}

	return true;
}


bool CInspection::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("ID") || ! jsonObject["ID"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ID");)

		return false;
	}
	ID = jsonObject["ID"].toString();

	if (!jsonObject.contains("TypeID") || ! jsonObject["TypeID"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "TypeID");)

		return false;
	}
	TypeID = jsonObject["TypeID"].toString();

	if (!jsonObject.contains("Name") || ! jsonObject["Name"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name");)

		return false;
	}
	Name = jsonObject["Name"].toString();

	if (!jsonObject.contains("Status") || ! jsonObject["Status"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status");)

		return false;
	}
	const QString statusStringValue = jsonObject["Status"].toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (jsonObject.contains("Analyzers") && jsonObject["Analyzers"].isArray()){
		const QJsonArray analyzersJsonArray = jsonObject["Analyzers"].toArray();
		const qsizetype analyzersArrayCount = analyzersJsonArray.size();
		Analyzers = QList<CAnalyzer::V1_0>();
		for (qsizetype analyzersIndex = 0; analyzersIndex < analyzersArrayCount; ++analyzersIndex){
			CAnalyzer::V1_0 tempAnalyzers;
			if (!tempAnalyzers.ReadFromJsonObject(analyzersJsonArray[analyzersIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers");)

				return false;
			}
			Analyzers->append(tempAnalyzers);
		}
	}

	return true;
}


bool CInspection::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("ID") && jsonObject["ID"].isString()){
		ID = jsonObject["ID"].toString();
	}

	if (jsonObject.contains("TypeID") && jsonObject["TypeID"].isString()){
		TypeID = jsonObject["TypeID"].toString();
	}

	if (jsonObject.contains("Name") && jsonObject["Name"].isString()){
		Name = jsonObject["Name"].toString();
	}

	if (jsonObject.contains("Status") && jsonObject["Status"].isString()){
		const QString statusStringValue = jsonObject["Status"].toString();
		if(statusStringValue == "NONE"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("Analyzers") && jsonObject["Analyzers"].isArray()){
		const QJsonArray analyzersJsonArray = jsonObject["Analyzers"].toArray();
		const qsizetype analyzersArrayCount = analyzersJsonArray.size();
		Analyzers = QList<CAnalyzer::V1_0>();
		for (qsizetype analyzersIndex = 0; analyzersIndex < analyzersArrayCount; ++analyzersIndex){
			CAnalyzer::V1_0 tempAnalyzers;
			if (!tempAnalyzers.OptReadFromJsonObject(analyzersJsonArray[analyzersIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers");)

				return false;
			}
			Analyzers->append(tempAnalyzers);
		}
	}

	return true;
}


// serialize methods

bool CInspection::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CInspection::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CInspection::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CInspection::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CInspection::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CInspection::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CInspection::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CInspection::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CInspection::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


CInspectionObject::CInspectionObject(QObject* parent): ::imtbase::CItemModelBase(parent)			, m_analyzersQObjectPtr(nullptr)
{
	Version_1_0.emplace();

	QObject::connect(this, &CInspectionObject::iDChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CInspectionObject::typeIDChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CInspectionObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CInspectionObject::statusChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CInspectionObject::analyzersChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString CInspectionObject::GetID()
{
	if (Version_1_0->ID.has_value()){
		return Version_1_0->ID.value();
	}

	return QString();
}


void CInspectionObject::SetID(QString v)
{
	Version_1_0->ID = v;
	iDChanged();
}


bool CInspectionObject::hasID()
{
	 return Version_1_0->ID.HasValue();
}


QString CInspectionObject::GetTypeID()
{
	if (Version_1_0->TypeID.has_value()){
		return Version_1_0->TypeID.value();
	}

	return QString();
}


void CInspectionObject::SetTypeID(QString v)
{
	Version_1_0->TypeID = v;
	typeIDChanged();
}


bool CInspectionObject::hasTypeID()
{
	 return Version_1_0->TypeID.HasValue();
}


QString CInspectionObject::GetName()
{
	if (Version_1_0->Name.has_value()){
		return Version_1_0->Name.value();
	}

	return QString();
}


void CInspectionObject::SetName(QString v)
{
	Version_1_0->Name = v;
	nameChanged();
}


bool CInspectionObject::hasName()
{
	 return Version_1_0->Name.HasValue();
}


QString CInspectionObject::GetStatus()
{
	if (Version_1_0->Status.has_value()){
		sdl::complextest::CTTypes::StatusCode valueType = Version_1_0->Status.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::StatusCode>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QString();
}


void CInspectionObject::SetStatus(QString v)
{
	Version_1_0->Status.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::StatusCode>();
	int key = metaEnum.keyToValue(v.toUtf8());
	if (key > -1){
		Version_1_0->Status = (sdl::complextest::CTTypes::StatusCode)key;
	}
	statusChanged();
}


bool CInspectionObject::hasStatus()
{
	 return Version_1_0->Status.HasValue();
}


sdl::complextest::CTImpl::CAnalyzerObjectList* CInspectionObject::GetAnalyzers()
{
	if (Version_1_0->Analyzers.has_value()){
		if (!m_analyzersQObjectPtr){
			m_analyzersQObjectPtr = dynamic_cast<sdl::complextest::CTImpl::CAnalyzerObjectList*>(CreateObject("Analyzers"));
			m_analyzersQObjectPtr->Version_1_0 = Version_1_0->Analyzers;
		}
		return m_analyzersQObjectPtr;
	}

	return nullptr;
}


void CInspectionObject::SetAnalyzers(sdl::complextest::CTImpl::CAnalyzerObjectList* v)
{
	if (v){
		Version_1_0->Analyzers = v->Version_1_0;
		m_analyzersQObjectPtr = v;
	}
	else {
		Version_1_0->Analyzers = nullptr;
	}

	analyzersChanged();
}


bool CInspectionObject::hasAnalyzers()
{
	 return Version_1_0->Analyzers.HasValue();
}


void CInspectionObject::createAnalyzers()
{	Version_1_0->Analyzers.emplace();

}


QString CInspectionObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CInspectionObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CInspectionObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CInspectionObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QObject* CInspectionObject::CreateObject(const QString& key)
{
	if (key == "Analyzers"){
		return new sdl::complextest::CTImpl::CAnalyzerObjectList(this);
	}
	return nullptr;
}


QString CInspectionObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "iD")){
		return "ID";
	}
	if (propertyName == (QString("m_") + "typeID")){
		return "TypeID";
	}
	if (propertyName == (QString("m_") + "name")){
		return "Name";
	}
	if (propertyName == (QString("m_") + "status")){
		return "Status";
	}
	if (propertyName == (QString("m_") + "analyzers")){
		return "Analyzers";
	}

	return propertyName;
}


} // namespace sdl::complextest::CTImpl




/// \file CProductOverview.cpp

namespace sdl::complextest::CTImpl
{


QByteArray CProductOverview::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CProductOverview::V1_0::operator==(const V1_0& other) const
{
	return 
				ProductID == other.ProductID &&
				Name == other.Name &&
				Status == other.Status &&
				PartSerialNumber == other.PartSerialNumber &&
				Timestamp == other.Timestamp &&
				ProductImage == other.ProductImage &&
				Inspections == other.Inspections;
}


bool CProductOverview::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (!ProductID){
		return false;
	}
	model.SetData("ProductID", *ProductID, modelIndex);

	if (!Name){
		return false;
	}
	model.SetData("Name", *Name, modelIndex);

	if (!Status){
		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::complextest::CTTypes::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::FAILED:
		statusStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("Status", statusStringValue, modelIndex);

	if (!PartSerialNumber){
		return false;
	}
	model.SetData("PartSerialNumber", *PartSerialNumber, modelIndex);

	if (!Timestamp){
		return false;
	}
	model.SetData("Timestamp", *Timestamp, modelIndex);

	if (!ProductImage){
		return false;
	}
	model.SetData("ProductImage", *ProductImage, modelIndex);

	if (Inspections){
		::imtbase::CTreeItemModel* newInspectionsModelPtr = model.AddTreeModel("Inspections", modelIndex);
		newInspectionsModelPtr->setIsArray(true);
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < Inspections->size(); ++inspectionsIndex){
			newInspectionsModelPtr->InsertNewItem();
			if (!(Inspections->at(inspectionsIndex).WriteToModel(*newInspectionsModelPtr, inspectionsIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections");)

				return false;
			}
		}
	}

	return true;
}


bool CProductOverview::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant productIDData = model.GetData("ProductID", modelIndex);
	if (productIDData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ProductID");)

		return false;
	}
	ProductID = productIDData.toInt();

	QVariant nameData = model.GetData("Name", modelIndex);
	if (nameData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Name");)

		return false;
	}
	Name = nameData.toString();

	QVariant statusData = model.GetData("Status", modelIndex);
	if (statusData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Status");)

		return false;
	}
	QString statusStringValue = statusData.toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	QVariant partSerialNumberData = model.GetData("PartSerialNumber", modelIndex);
	if (partSerialNumberData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "PartSerialNumber");)

		return false;
	}
	PartSerialNumber = partSerialNumberData.toString();

	QVariant timestampData = model.GetData("Timestamp", modelIndex);
	if (timestampData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Timestamp");)

		return false;
	}
	Timestamp = timestampData.toString();

	QVariant productImageData = model.GetData("ProductImage", modelIndex);
	if (productImageData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ProductImage");)

		return false;
	}
	ProductImage = productImageData.toString();

	::imtbase::CTreeItemModel* inspectionsModel = model.GetTreeItemModel("Inspections", modelIndex);
	if (inspectionsModel != nullptr){
		int inspectionsCount = inspectionsModel->GetItemsCount();
		QList<CInspection::V1_0> inspectionsList;
		for (int inspectionsIndex = 0; inspectionsIndex < inspectionsCount; ++inspectionsIndex){
			CInspection::V1_0 inspections;
			if (!inspections.ReadFromModel(*inspectionsModel, inspectionsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections");)

				return false;
			}
			inspectionsList << inspections;
		}
		Inspections = inspectionsList;

	}

	return true;
}


bool CProductOverview::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant productIDData = model.GetData("ProductID", modelIndex);
	if (!productIDData.isNull()){
		ProductID = productIDData.toInt();
	}

	QVariant nameData = model.GetData("Name", modelIndex);
	if (!nameData.isNull()){
		Name = nameData.toString();
	}

	QVariant statusData = model.GetData("Status", modelIndex);
	if (!statusData.isNull()){
		QString statusStringValue = statusData.toString();
		if(statusStringValue == "NONE"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	QVariant partSerialNumberData = model.GetData("PartSerialNumber", modelIndex);
	if (!partSerialNumberData.isNull()){
		PartSerialNumber = partSerialNumberData.toString();
	}

	QVariant timestampData = model.GetData("Timestamp", modelIndex);
	if (!timestampData.isNull()){
		Timestamp = timestampData.toString();
	}

	QVariant productImageData = model.GetData("ProductImage", modelIndex);
	if (!productImageData.isNull()){
		ProductImage = productImageData.toString();
	}

	::imtbase::CTreeItemModel* inspectionsModel = model.GetTreeItemModel("Inspections", modelIndex);
	if (inspectionsModel != nullptr){
		int inspectionsCount = inspectionsModel->GetItemsCount();
		QList<CInspection::V1_0> inspectionsList;
		for (int inspectionsIndex = 0; inspectionsIndex < inspectionsCount; ++inspectionsIndex){
			CInspection::V1_0 inspections;
			if (!inspections.OptReadFromModel(*inspectionsModel, inspectionsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections");)

				return false;
			}
			inspectionsList << inspections;
		}
		Inspections = inspectionsList;

	}

	return true;
}


bool CProductOverview::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!ProductID){
		return false;
	}
	gqlObject.InsertParam("ProductID", QVariant(*ProductID));

	if (!Name){
		return false;
	}
	gqlObject.InsertParam("Name", QVariant(*Name));

	if (!Status){
		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::complextest::CTTypes::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::FAILED:
		statusStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("Status", QVariant(statusStringValue));

	if (!PartSerialNumber){
		return false;
	}
	gqlObject.InsertParam("PartSerialNumber", QVariant(*PartSerialNumber));

	if (!Timestamp){
		return false;
	}
	gqlObject.InsertParam("Timestamp", QVariant(*Timestamp));

	if (!ProductImage){
		return false;
	}
	gqlObject.InsertParam("ProductImage", QVariant(*ProductImage));

	if (Inspections){
		QList<::imtgql::CGqlParamObject> inspectionsDataObjectList;
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < Inspections->size(); ++inspectionsIndex){
			::imtgql::CGqlParamObject newInspectionsGqlObject;
			if (!Inspections->at(inspectionsIndex).WriteToGraphQlObject(newInspectionsGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections");)

				return false;
			}
			inspectionsDataObjectList << newInspectionsGqlObject;
		}
		gqlObject.InsertParam("Inspections", inspectionsDataObjectList);
	}

	return true;
}


bool CProductOverview::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("ProductID") || (gqlObject["ProductID"].userType() != QMetaType::Int && gqlObject["ProductID"].userType() != QMetaType::UInt && gqlObject["ProductID"].userType() != QMetaType::LongLong && gqlObject["ProductID"].userType() != QMetaType::ULongLong && gqlObject["ProductID"].userType() != QMetaType::Long && gqlObject["ProductID"].userType() != QMetaType::Short && gqlObject["ProductID"].userType() != QMetaType::ULong && gqlObject["ProductID"].userType() != QMetaType::UShort && gqlObject["ProductID"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductID");)

		return false;
	}
	ProductID = gqlObject["ProductID"].toInt();

	if (!gqlObject.ContainsParam("Name") || (gqlObject["Name"].userType() != QMetaType::QString && gqlObject["Name"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name");)

		return false;
	}
	Name = gqlObject["Name"].toString();

	if (!gqlObject.ContainsParam("Status") || (gqlObject["Status"].userType() != QMetaType::QString && gqlObject["Status"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status");)

		return false;
	}
	const QString statusStringValue = gqlObject["Status"].toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("PartSerialNumber") || (gqlObject["PartSerialNumber"].userType() != QMetaType::QString && gqlObject["PartSerialNumber"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "PartSerialNumber");)

		return false;
	}
	PartSerialNumber = gqlObject["PartSerialNumber"].toString();

	if (!gqlObject.ContainsParam("Timestamp") || (gqlObject["Timestamp"].userType() != QMetaType::QString && gqlObject["Timestamp"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Timestamp");)

		return false;
	}
	Timestamp = gqlObject["Timestamp"].toString();

	if (!gqlObject.ContainsParam("ProductImage") || (gqlObject["ProductImage"].userType() != QMetaType::QString && gqlObject["ProductImage"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductImage");)

		return false;
	}
	ProductImage = gqlObject["ProductImage"].toString();

	if (gqlObject.ContainsParam("Inspections") && (gqlObject.GetObjectsCount("Inspections") > 0)){
		const qsizetype inspectionsElementsCount = gqlObject.GetObjectsCount("inspections");
		Inspections = QList<CInspection::V1_0>();
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < inspectionsElementsCount; ++inspectionsIndex){
			const ::imtgql::CGqlParamObject* inspectionsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Inspections", inspectionsIndex);
			if (inspectionsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << inspectionsDataObjectPtr;
				return false;
			}
			CInspection::V1_0 tempInspections;
			if (!tempInspections.ReadFromGraphQlObject(*inspectionsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections");)

				return false;
			}
			Inspections->append(tempInspections);
		}
	}

	return true;
}


bool CProductOverview::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("ProductID") && (gqlObject["ProductID"].userType() == QMetaType::Int || gqlObject["ProductID"].userType() == QMetaType::UInt || gqlObject["ProductID"].userType() == QMetaType::LongLong || gqlObject["ProductID"].userType() == QMetaType::ULongLong || gqlObject["ProductID"].userType() == QMetaType::Long || gqlObject["ProductID"].userType() == QMetaType::Short || gqlObject["ProductID"].userType() == QMetaType::ULong || gqlObject["ProductID"].userType() == QMetaType::UShort || gqlObject["ProductID"].userType() == QMetaType::UChar)){
		ProductID = gqlObject["ProductID"].toInt();
	}

	if (gqlObject.ContainsParam("Name") && (gqlObject["Name"].userType() == QMetaType::QString || gqlObject["Name"].userType() == QMetaType::QByteArray)){
		Name = gqlObject["Name"].toString();
	}

	if (gqlObject.ContainsParam("Status") && (gqlObject["Status"].userType() == QMetaType::QString || gqlObject["Status"].userType() == QMetaType::QByteArray)){
		const QString statusStringValue = gqlObject["Status"].toString();
		if(statusStringValue == "NONE"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("PartSerialNumber") && (gqlObject["PartSerialNumber"].userType() == QMetaType::QString || gqlObject["PartSerialNumber"].userType() == QMetaType::QByteArray)){
		PartSerialNumber = gqlObject["PartSerialNumber"].toString();
	}

	if (gqlObject.ContainsParam("Timestamp") && (gqlObject["Timestamp"].userType() == QMetaType::QString || gqlObject["Timestamp"].userType() == QMetaType::QByteArray)){
		Timestamp = gqlObject["Timestamp"].toString();
	}

	if (gqlObject.ContainsParam("ProductImage") && (gqlObject["ProductImage"].userType() == QMetaType::QString || gqlObject["ProductImage"].userType() == QMetaType::QByteArray)){
		ProductImage = gqlObject["ProductImage"].toString();
	}

	if (gqlObject.ContainsParam("Inspections") && (gqlObject.GetObjectsCount("Inspections") > 0)){
		const qsizetype inspectionsElementsCount = gqlObject.GetObjectsCount("inspections");
		Inspections = QList<CInspection::V1_0>();
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < inspectionsElementsCount; ++inspectionsIndex){
			const ::imtgql::CGqlParamObject* inspectionsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Inspections", inspectionsIndex);
			if (inspectionsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << inspectionsDataObjectPtr;
				return false;
			}
			CInspection::V1_0 tempInspections;
			if (!tempInspections.OptReadFromGraphQlObject(*inspectionsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections");)

				return false;
			}
			Inspections->append(tempInspections);
		}
	}

	return true;
}


bool CProductOverview::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!ProductID){
		return false;
	}
	jsonObject["ProductID"] = QJsonValue::fromVariant(*ProductID);

	if (!Name){
		return false;
	}
	jsonObject["Name"] = QJsonValue::fromVariant(*Name);

	if (!Status){
		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::complextest::CTTypes::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::complextest::CTTypes::StatusCode::FAILED:
		statusStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["Status"] = QJsonValue::fromVariant(statusStringValue);

	if (!PartSerialNumber){
		return false;
	}
	jsonObject["PartSerialNumber"] = QJsonValue::fromVariant(*PartSerialNumber);

	if (!Timestamp){
		return false;
	}
	jsonObject["Timestamp"] = QJsonValue::fromVariant(*Timestamp);

	if (!ProductImage){
		return false;
	}
	jsonObject["ProductImage"] = QJsonValue::fromVariant(*ProductImage);

	if (Inspections){
		QJsonArray newInspectionsArray;
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < Inspections->size(); ++inspectionsIndex){
			QJsonObject newInspectionsJsonObject;
			if (!Inspections->at(inspectionsIndex).WriteToJsonObject(newInspectionsJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections");)

				return false;
			}
			newInspectionsArray << newInspectionsJsonObject;
		}
		jsonObject["Inspections"] = newInspectionsArray;
	}

	return true;
}


bool CProductOverview::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("ProductID") || ! jsonObject["ProductID"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductID");)

		return false;
	}
	ProductID = jsonObject["ProductID"].toInt();

	if (!jsonObject.contains("Name") || ! jsonObject["Name"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name");)

		return false;
	}
	Name = jsonObject["Name"].toString();

	if (!jsonObject.contains("Status") || ! jsonObject["Status"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status");)

		return false;
	}
	const QString statusStringValue = jsonObject["Status"].toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (!jsonObject.contains("PartSerialNumber") || ! jsonObject["PartSerialNumber"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "PartSerialNumber");)

		return false;
	}
	PartSerialNumber = jsonObject["PartSerialNumber"].toString();

	if (!jsonObject.contains("Timestamp") || ! jsonObject["Timestamp"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Timestamp");)

		return false;
	}
	Timestamp = jsonObject["Timestamp"].toString();

	if (!jsonObject.contains("ProductImage") || ! jsonObject["ProductImage"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductImage");)

		return false;
	}
	ProductImage = jsonObject["ProductImage"].toString();

	if (jsonObject.contains("Inspections") && jsonObject["Inspections"].isArray()){
		const QJsonArray inspectionsJsonArray = jsonObject["Inspections"].toArray();
		const qsizetype inspectionsArrayCount = inspectionsJsonArray.size();
		Inspections = QList<CInspection::V1_0>();
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < inspectionsArrayCount; ++inspectionsIndex){
			CInspection::V1_0 tempInspections;
			if (!tempInspections.ReadFromJsonObject(inspectionsJsonArray[inspectionsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections");)

				return false;
			}
			Inspections->append(tempInspections);
		}
	}

	return true;
}


bool CProductOverview::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("ProductID") && jsonObject["ProductID"].isDouble()){
		ProductID = jsonObject["ProductID"].toInt();
	}

	if (jsonObject.contains("Name") && jsonObject["Name"].isString()){
		Name = jsonObject["Name"].toString();
	}

	if (jsonObject.contains("Status") && jsonObject["Status"].isString()){
		const QString statusStringValue = jsonObject["Status"].toString();
		if(statusStringValue == "NONE"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::complextest::CTTypes::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::complextest::CTTypes::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::complextest::CTTypes::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("PartSerialNumber") && jsonObject["PartSerialNumber"].isString()){
		PartSerialNumber = jsonObject["PartSerialNumber"].toString();
	}

	if (jsonObject.contains("Timestamp") && jsonObject["Timestamp"].isString()){
		Timestamp = jsonObject["Timestamp"].toString();
	}

	if (jsonObject.contains("ProductImage") && jsonObject["ProductImage"].isString()){
		ProductImage = jsonObject["ProductImage"].toString();
	}

	if (jsonObject.contains("Inspections") && jsonObject["Inspections"].isArray()){
		const QJsonArray inspectionsJsonArray = jsonObject["Inspections"].toArray();
		const qsizetype inspectionsArrayCount = inspectionsJsonArray.size();
		Inspections = QList<CInspection::V1_0>();
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < inspectionsArrayCount; ++inspectionsIndex){
			CInspection::V1_0 tempInspections;
			if (!tempInspections.OptReadFromJsonObject(inspectionsJsonArray[inspectionsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections");)

				return false;
			}
			Inspections->append(tempInspections);
		}
	}

	return true;
}


// serialize methods

bool CProductOverview::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CProductOverview::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CProductOverview::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CProductOverview::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CProductOverview::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CProductOverview::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CProductOverview::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CProductOverview::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CProductOverview::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


CProductOverviewObject::CProductOverviewObject(QObject* parent): ::imtbase::CItemModelBase(parent)			, m_inspectionsQObjectPtr(nullptr)
{
	Version_1_0.emplace();

	QObject::connect(this, &CProductOverviewObject::productIDChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::statusChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::partSerialNumberChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::timestampChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::productImageChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::inspectionsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


int CProductOverviewObject::GetProductID()
{
	if (Version_1_0->ProductID.has_value()){
		return Version_1_0->ProductID.value();
	}

	return 0;
}


void CProductOverviewObject::SetProductID(int v)
{
	Version_1_0->ProductID = v;
	productIDChanged();
}


bool CProductOverviewObject::hasProductID()
{
	 return Version_1_0->ProductID.HasValue();
}


QString CProductOverviewObject::GetName()
{
	if (Version_1_0->Name.has_value()){
		return Version_1_0->Name.value();
	}

	return QString();
}


void CProductOverviewObject::SetName(QString v)
{
	Version_1_0->Name = v;
	nameChanged();
}


bool CProductOverviewObject::hasName()
{
	 return Version_1_0->Name.HasValue();
}


QString CProductOverviewObject::GetStatus()
{
	if (Version_1_0->Status.has_value()){
		sdl::complextest::CTTypes::StatusCode valueType = Version_1_0->Status.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::StatusCode>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QString();
}


void CProductOverviewObject::SetStatus(QString v)
{
	Version_1_0->Status.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::CTTypes::StatusCode>();
	int key = metaEnum.keyToValue(v.toUtf8());
	if (key > -1){
		Version_1_0->Status = (sdl::complextest::CTTypes::StatusCode)key;
	}
	statusChanged();
}


bool CProductOverviewObject::hasStatus()
{
	 return Version_1_0->Status.HasValue();
}


QString CProductOverviewObject::GetPartSerialNumber()
{
	if (Version_1_0->PartSerialNumber.has_value()){
		return Version_1_0->PartSerialNumber.value();
	}

	return QString();
}


void CProductOverviewObject::SetPartSerialNumber(QString v)
{
	Version_1_0->PartSerialNumber = v;
	partSerialNumberChanged();
}


bool CProductOverviewObject::hasPartSerialNumber()
{
	 return Version_1_0->PartSerialNumber.HasValue();
}


QString CProductOverviewObject::GetTimestamp()
{
	if (Version_1_0->Timestamp.has_value()){
		return Version_1_0->Timestamp.value();
	}

	return QString();
}


void CProductOverviewObject::SetTimestamp(QString v)
{
	Version_1_0->Timestamp = v;
	timestampChanged();
}


bool CProductOverviewObject::hasTimestamp()
{
	 return Version_1_0->Timestamp.HasValue();
}


QString CProductOverviewObject::GetProductImage()
{
	if (Version_1_0->ProductImage.has_value()){
		return Version_1_0->ProductImage.value();
	}

	return QString();
}


void CProductOverviewObject::SetProductImage(QString v)
{
	Version_1_0->ProductImage = v;
	productImageChanged();
}


bool CProductOverviewObject::hasProductImage()
{
	 return Version_1_0->ProductImage.HasValue();
}


sdl::complextest::CTImpl::CInspectionObjectList* CProductOverviewObject::GetInspections()
{
	if (Version_1_0->Inspections.has_value()){
		if (!m_inspectionsQObjectPtr){
			m_inspectionsQObjectPtr = dynamic_cast<sdl::complextest::CTImpl::CInspectionObjectList*>(CreateObject("Inspections"));
			m_inspectionsQObjectPtr->Version_1_0 = Version_1_0->Inspections;
		}
		return m_inspectionsQObjectPtr;
	}

	return nullptr;
}


void CProductOverviewObject::SetInspections(sdl::complextest::CTImpl::CInspectionObjectList* v)
{
	if (v){
		Version_1_0->Inspections = v->Version_1_0;
		m_inspectionsQObjectPtr = v;
	}
	else {
		Version_1_0->Inspections = nullptr;
	}

	inspectionsChanged();
}


bool CProductOverviewObject::hasInspections()
{
	 return Version_1_0->Inspections.HasValue();
}


void CProductOverviewObject::createInspections()
{	Version_1_0->Inspections.emplace();

}


QString CProductOverviewObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CProductOverviewObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CProductOverviewObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CProductOverviewObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QObject* CProductOverviewObject::CreateObject(const QString& key)
{
	if (key == "Inspections"){
		return new sdl::complextest::CTImpl::CInspectionObjectList(this);
	}
	return nullptr;
}


QString CProductOverviewObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "productID")){
		return "ProductID";
	}
	if (propertyName == (QString("m_") + "name")){
		return "Name";
	}
	if (propertyName == (QString("m_") + "status")){
		return "Status";
	}
	if (propertyName == (QString("m_") + "partSerialNumber")){
		return "PartSerialNumber";
	}
	if (propertyName == (QString("m_") + "timestamp")){
		return "Timestamp";
	}
	if (propertyName == (QString("m_") + "productImage")){
		return "ProductImage";
	}
	if (propertyName == (QString("m_") + "inspections")){
		return "Inspections";
	}

	return propertyName;
}


} // namespace sdl::complextest::CTImpl




/// \file CLocalizedText.cpp

namespace sdl::complextest::CTImpl
{


QByteArray CLocalizedText::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CLocalizedText::V1_0::operator==(const V1_0& other) const
{
	return 
				text == other.text &&
				locale == other.locale;
}


bool CLocalizedText::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (!text){
		return false;
	}
	model.SetData("text", *text, modelIndex);

	if (!locale){
		return false;
	}
	model.SetData("locale", *locale, modelIndex);


	return true;
}


bool CLocalizedText::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant textData = model.GetData("text", modelIndex);
	if (textData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "text");)

		return false;
	}
	text = textData.toString();

	QVariant localeData = model.GetData("locale", modelIndex);
	if (localeData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "locale");)

		return false;
	}
	locale = localeData.toString();

	return true;
}


bool CLocalizedText::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant textData = model.GetData("text", modelIndex);
	if (!textData.isNull()){
		text = textData.toString();
	}

	QVariant localeData = model.GetData("locale", modelIndex);
	if (!localeData.isNull()){
		locale = localeData.toString();
	}

	return true;
}


bool CLocalizedText::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!text){
		return false;
	}
	gqlObject.InsertParam("text", QVariant(*text));

	if (!locale){
		return false;
	}
	gqlObject.InsertParam("locale", QVariant(*locale));

	return true;
}


bool CLocalizedText::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("text") || (gqlObject["text"].userType() != QMetaType::QString && gqlObject["text"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "text");)

		return false;
	}
	text = gqlObject["text"].toString();

	if (!gqlObject.ContainsParam("locale") || (gqlObject["locale"].userType() != QMetaType::QString && gqlObject["locale"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "locale");)

		return false;
	}
	locale = gqlObject["locale"].toString();

	return true;
}


bool CLocalizedText::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("text") && (gqlObject["text"].userType() == QMetaType::QString || gqlObject["text"].userType() == QMetaType::QByteArray)){
		text = gqlObject["text"].toString();
	}

	if (gqlObject.ContainsParam("locale") && (gqlObject["locale"].userType() == QMetaType::QString || gqlObject["locale"].userType() == QMetaType::QByteArray)){
		locale = gqlObject["locale"].toString();
	}

	return true;
}


bool CLocalizedText::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!text){
		return false;
	}
	jsonObject["text"] = QJsonValue::fromVariant(*text);

	if (!locale){
		return false;
	}
	jsonObject["locale"] = QJsonValue::fromVariant(*locale);

	return true;
}


bool CLocalizedText::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("text") || ! jsonObject["text"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "text");)

		return false;
	}
	text = jsonObject["text"].toString();

	if (!jsonObject.contains("locale") || ! jsonObject["locale"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "locale");)

		return false;
	}
	locale = jsonObject["locale"].toString();

	return true;
}


bool CLocalizedText::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("text") && jsonObject["text"].isString()){
		text = jsonObject["text"].toString();
	}

	if (jsonObject.contains("locale") && jsonObject["locale"].isString()){
		locale = jsonObject["locale"].toString();
	}

	return true;
}


// serialize methods

bool CLocalizedText::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CLocalizedText::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CLocalizedText::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CLocalizedText::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CLocalizedText::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CLocalizedText::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CLocalizedText::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CLocalizedText::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CLocalizedText::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


CLocalizedTextObject::CLocalizedTextObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CLocalizedTextObject::textChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CLocalizedTextObject::localeChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString CLocalizedTextObject::GetText()
{
	if (Version_1_0->text.has_value()){
		return Version_1_0->text.value();
	}

	return QString();
}


void CLocalizedTextObject::SetText(QString v)
{
	Version_1_0->text = v;
	textChanged();
}


bool CLocalizedTextObject::hasText()
{
	 return Version_1_0->text.HasValue();
}


QString CLocalizedTextObject::GetLocale()
{
	if (Version_1_0->locale.has_value()){
		return Version_1_0->locale.value();
	}

	return QString();
}


void CLocalizedTextObject::SetLocale(QString v)
{
	Version_1_0->locale = v;
	localeChanged();
}


bool CLocalizedTextObject::hasLocale()
{
	 return Version_1_0->locale.HasValue();
}


QString CLocalizedTextObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CLocalizedTextObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CLocalizedTextObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CLocalizedTextObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QObject* CLocalizedTextObject::CreateObject(const QString& key)
{
	return nullptr;
}


QString CLocalizedTextObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "text")){
		return "text";
	}
	if (propertyName == (QString("m_") + "locale")){
		return "locale";
	}

	return propertyName;
}


} // namespace sdl::complextest::CTImpl




/// \file CCoordinates.cpp

namespace sdl::complextest::CTImpl
{


QByteArray CCoordinates::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CCoordinates::V1_0::operator==(const V1_0& other) const
{
	return 
				x.has_value() == other.x.has_value() &&
				((x.has_value() && other.x.has_value()) ?
					qFuzzyCompare(*x, *other.x) : true) &&
				y.has_value() == other.y.has_value() &&
				((y.has_value() && other.y.has_value()) ?
					qFuzzyCompare(*y, *other.y) : true) &&
				z.has_value() == other.z.has_value() &&
				((z.has_value() && other.z.has_value()) ?
					qFuzzyCompare(*z, *other.z) : true);
}


bool CCoordinates::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (!x){
		return false;
	}
	model.SetData("x", *x, modelIndex);

	if (!y){
		return false;
	}
	model.SetData("y", *y, modelIndex);

	if (!z){
		return false;
	}
	model.SetData("z", *z, modelIndex);


	return true;
}


bool CCoordinates::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant xData = model.GetData("x", modelIndex);
	if (xData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "x");)

		return false;
	}
	x = xData.toFloat();

	QVariant yData = model.GetData("y", modelIndex);
	if (yData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "y");)

		return false;
	}
	y = yData.toFloat();

	QVariant zData = model.GetData("z", modelIndex);
	if (zData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "z");)

		return false;
	}
	z = zData.toFloat();

	return true;
}


bool CCoordinates::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant xData = model.GetData("x", modelIndex);
	if (!xData.isNull()){
		x = xData.toFloat();
	}

	QVariant yData = model.GetData("y", modelIndex);
	if (!yData.isNull()){
		y = yData.toFloat();
	}

	QVariant zData = model.GetData("z", modelIndex);
	if (!zData.isNull()){
		z = zData.toFloat();
	}

	return true;
}


bool CCoordinates::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!x){
		return false;
	}
	gqlObject.InsertParam("x", QVariant(*x));

	if (!y){
		return false;
	}
	gqlObject.InsertParam("y", QVariant(*y));

	if (!z){
		return false;
	}
	gqlObject.InsertParam("z", QVariant(*z));

	return true;
}


bool CCoordinates::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("x") || (gqlObject["x"].userType() != QMetaType::Float && gqlObject["x"].userType() != QMetaType::Double && gqlObject["x"].userType() != QMetaType::Int && gqlObject["x"].userType() != QMetaType::UInt && gqlObject["x"].userType() != QMetaType::LongLong && gqlObject["x"].userType() != QMetaType::ULongLong && gqlObject["x"].userType() != QMetaType::Long && gqlObject["x"].userType() != QMetaType::Short && gqlObject["x"].userType() != QMetaType::ULong && gqlObject["x"].userType() != QMetaType::UShort && gqlObject["x"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "x");)

		return false;
	}
	x = gqlObject["x"].toDouble();

	if (!gqlObject.ContainsParam("y") || (gqlObject["y"].userType() != QMetaType::Float && gqlObject["y"].userType() != QMetaType::Double && gqlObject["y"].userType() != QMetaType::Int && gqlObject["y"].userType() != QMetaType::UInt && gqlObject["y"].userType() != QMetaType::LongLong && gqlObject["y"].userType() != QMetaType::ULongLong && gqlObject["y"].userType() != QMetaType::Long && gqlObject["y"].userType() != QMetaType::Short && gqlObject["y"].userType() != QMetaType::ULong && gqlObject["y"].userType() != QMetaType::UShort && gqlObject["y"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "y");)

		return false;
	}
	y = gqlObject["y"].toDouble();

	if (!gqlObject.ContainsParam("z") || (gqlObject["z"].userType() != QMetaType::Float && gqlObject["z"].userType() != QMetaType::Double && gqlObject["z"].userType() != QMetaType::Int && gqlObject["z"].userType() != QMetaType::UInt && gqlObject["z"].userType() != QMetaType::LongLong && gqlObject["z"].userType() != QMetaType::ULongLong && gqlObject["z"].userType() != QMetaType::Long && gqlObject["z"].userType() != QMetaType::Short && gqlObject["z"].userType() != QMetaType::ULong && gqlObject["z"].userType() != QMetaType::UShort && gqlObject["z"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "z");)

		return false;
	}
	z = gqlObject["z"].toDouble();

	return true;
}


bool CCoordinates::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("x") && (gqlObject["x"].userType() == QMetaType::Float || gqlObject["x"].userType() == QMetaType::Double || gqlObject["x"].userType() == QMetaType::Int || gqlObject["x"].userType() == QMetaType::UInt || gqlObject["x"].userType() == QMetaType::LongLong || gqlObject["x"].userType() == QMetaType::ULongLong || gqlObject["x"].userType() == QMetaType::Long || gqlObject["x"].userType() == QMetaType::Short || gqlObject["x"].userType() == QMetaType::ULong || gqlObject["x"].userType() == QMetaType::UShort || gqlObject["x"].userType() == QMetaType::UChar)){
		x = gqlObject["x"].toDouble();
	}

	if (gqlObject.ContainsParam("y") && (gqlObject["y"].userType() == QMetaType::Float || gqlObject["y"].userType() == QMetaType::Double || gqlObject["y"].userType() == QMetaType::Int || gqlObject["y"].userType() == QMetaType::UInt || gqlObject["y"].userType() == QMetaType::LongLong || gqlObject["y"].userType() == QMetaType::ULongLong || gqlObject["y"].userType() == QMetaType::Long || gqlObject["y"].userType() == QMetaType::Short || gqlObject["y"].userType() == QMetaType::ULong || gqlObject["y"].userType() == QMetaType::UShort || gqlObject["y"].userType() == QMetaType::UChar)){
		y = gqlObject["y"].toDouble();
	}

	if (gqlObject.ContainsParam("z") && (gqlObject["z"].userType() == QMetaType::Float || gqlObject["z"].userType() == QMetaType::Double || gqlObject["z"].userType() == QMetaType::Int || gqlObject["z"].userType() == QMetaType::UInt || gqlObject["z"].userType() == QMetaType::LongLong || gqlObject["z"].userType() == QMetaType::ULongLong || gqlObject["z"].userType() == QMetaType::Long || gqlObject["z"].userType() == QMetaType::Short || gqlObject["z"].userType() == QMetaType::ULong || gqlObject["z"].userType() == QMetaType::UShort || gqlObject["z"].userType() == QMetaType::UChar)){
		z = gqlObject["z"].toDouble();
	}

	return true;
}


bool CCoordinates::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!x){
		return false;
	}
	jsonObject["x"] = QJsonValue::fromVariant(*x);

	if (!y){
		return false;
	}
	jsonObject["y"] = QJsonValue::fromVariant(*y);

	if (!z){
		return false;
	}
	jsonObject["z"] = QJsonValue::fromVariant(*z);

	return true;
}


bool CCoordinates::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("x") || ! jsonObject["x"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "x");)

		return false;
	}
	x = jsonObject["x"].toDouble();

	if (!jsonObject.contains("y") || ! jsonObject["y"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "y");)

		return false;
	}
	y = jsonObject["y"].toDouble();

	if (!jsonObject.contains("z") || ! jsonObject["z"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "z");)

		return false;
	}
	z = jsonObject["z"].toDouble();

	return true;
}


bool CCoordinates::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("x") && jsonObject["x"].isDouble()){
		x = jsonObject["x"].toDouble();
	}

	if (jsonObject.contains("y") && jsonObject["y"].isDouble()){
		y = jsonObject["y"].toDouble();
	}

	if (jsonObject.contains("z") && jsonObject["z"].isDouble()){
		z = jsonObject["z"].toDouble();
	}

	return true;
}


// serialize methods

bool CCoordinates::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCoordinates::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCoordinates::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCoordinates::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCoordinates::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCoordinates::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCoordinates::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCoordinates::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCoordinates::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


CCoordinatesObject::CCoordinatesObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CCoordinatesObject::xChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCoordinatesObject::yChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCoordinatesObject::zChanged, this, &CItemModelBase::OnInternalModelChanged);
}


double CCoordinatesObject::GetX()
{
	if (Version_1_0->x.has_value()){
		return Version_1_0->x.value();
	}

	return 0;
}


void CCoordinatesObject::SetX(double v)
{
	Version_1_0->x = v;
	xChanged();
}


bool CCoordinatesObject::hasX()
{
	 return Version_1_0->x.HasValue();
}


double CCoordinatesObject::GetY()
{
	if (Version_1_0->y.has_value()){
		return Version_1_0->y.value();
	}

	return 0;
}


void CCoordinatesObject::SetY(double v)
{
	Version_1_0->y = v;
	yChanged();
}


bool CCoordinatesObject::hasY()
{
	 return Version_1_0->y.HasValue();
}


double CCoordinatesObject::GetZ()
{
	if (Version_1_0->z.has_value()){
		return Version_1_0->z.value();
	}

	return 0;
}


void CCoordinatesObject::SetZ(double v)
{
	Version_1_0->z = v;
	zChanged();
}


bool CCoordinatesObject::hasZ()
{
	 return Version_1_0->z.HasValue();
}


QString CCoordinatesObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CCoordinatesObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CCoordinatesObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CCoordinatesObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QObject* CCoordinatesObject::CreateObject(const QString& key)
{
	return nullptr;
}


QString CCoordinatesObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "x")){
		return "x";
	}
	if (propertyName == (QString("m_") + "y")){
		return "y";
	}
	if (propertyName == (QString("m_") + "z")){
		return "z";
	}

	return propertyName;
}


} // namespace sdl::complextest::CTImpl




/// \file CExtendedMetaData.cpp

namespace sdl::complextest::CTImpl
{


QByteArray CExtendedMetaData::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CExtendedMetaData::V1_0::operator==(const V1_0& other) const
{
	return 
				key == other.key &&
				value == other.value;
}


bool CExtendedMetaData::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (key){
		model.SetData("key", *key, modelIndex);
	}

	if (value){
		QVariant valueVariantValue;
		if (const CCoordinates* val = std::get_if<CCoordinates>((*value).get())){
			if (!val->WriteToModel(*(model.AddTreeModel("value", modelIndex)), 0)){
				return false;
			}
		}
		else if (const double* val = std::get_if<double>((*value).get())){
			valueVariantValue = *val;
			model.SetData("value", valueVariantValue, modelIndex);
		}
		else if (const bool* val = std::get_if<bool>((*value).get())){
			valueVariantValue = *val;
			model.SetData("value", valueVariantValue, modelIndex);
		}
		else if (const QString* val = std::get_if<QString>((*value).get())){
			valueVariantValue = *val;
			model.SetData("value", valueVariantValue, modelIndex);
		}

	}


	return true;
}


bool CExtendedMetaData::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant keyData = model.GetData("key", modelIndex);
	if (!keyData.isNull()){
		key = keyData.toString();
	}

	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
	QString itemTypename = valueData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();

		if (itemTypename == "Coordinates") {
			CCoordinates valueConvert;
			const bool isvalueRead = valueConvert.ReadFromModel(*model.GetTreeItemModel("value", modelIndex)); 
			if (!isvalueRead){
				return false;
			}
			value = std::make_shared<ExtendedMetaDataUnionType>(valueConvert);
		}
		else if (valueData.canConvert<double>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueData.value<double>());
		}
		else if (valueData.canConvert<bool>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueData.value<bool>());
		}
		else if (valueData.canConvert<QString>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueData.value<QString>());
		}
	}

	return true;
}


bool CExtendedMetaData::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant keyData = model.GetData("key", modelIndex);
	if (!keyData.isNull()){
		key = keyData.toString();
	}

	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
	QString itemTypename = valueData.value<::imtbase::CTreeItemModel*>()->GetData("__typename").toString();

		if (itemTypename == "Coordinates") {
			CCoordinates valueConvert;
			const bool isvalueRead = valueConvert.ReadFromModel(*model.GetTreeItemModel("value", modelIndex)); 
			if (!isvalueRead){
				return false;
			}
			value = std::make_shared<ExtendedMetaDataUnionType>(valueConvert);
		}
		else if (valueData.canConvert<double>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueData.value<double>());
		}
		else if (valueData.canConvert<bool>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueData.value<bool>());
		}
		else if (valueData.canConvert<QString>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueData.value<QString>());
		}
	}

	return true;
}


bool CExtendedMetaData::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (key){
		gqlObject.InsertParam("key", QVariant(*key));
	}

	if (value){
		::imtgql::CGqlParamObject valueDataObject;
		if (const CCoordinates* val = std::get_if<CCoordinates>((*value).get())){
			if (!val->WriteToGraphQlObject(valueDataObject)){
				return false;
			}
		}
		else if (const double* val = std::get_if<double>((*value).get())){
			valueDataObject.InsertParam("value", *val);
		}
		else if (const bool* val = std::get_if<bool>((*value).get())){
			valueDataObject.InsertParam("value", *val);
		}
		else if (const QString* val = std::get_if<QString>((*value).get())){
			valueDataObject.InsertParam("value", *val);
		}
		gqlObject.InsertParam("value", valueDataObject);
	}

	return true;
}


bool CExtendedMetaData::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("key") && (gqlObject["key"].userType() == QMetaType::QString || gqlObject["key"].userType() == QMetaType::QByteArray)){
		key = gqlObject["key"].toString();
	}

	if (gqlObject.ContainsParam("value") && (!((gqlObject.IsObject("item") && gqlObject.GetParamArgumentObjectPtr("value") == nullptr) || (!gqlObject.IsObject("item") && gqlObject["value"].isNull())) && gqlObject.GetParamArgumentObjectPtr("value") != nullptr)){
			const ::imtgql::CGqlParamObject* itemDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("item");
	if (!itemDataObjectPtr->ContainsParam("__typename")) {
		qDebug() << "invalid typename for: value";
		return false;
	}

	QString itemTypename = itemDataObjectPtr->GetParamArgumentValue("__typename").toString();;
		if (itemTypename == "Coordinates") {
			CCoordinates valueConvert;
			const ::imtgql::CGqlParamObject* valueDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("value");
			const bool isValueRead = valueConvert.ReadFromGraphQlObject(*valueDataObjectPtr);
			if (!isValueRead){
				return false;
			}
			value = std::make_shared<ExtendedMetaDataUnionType>(valueConvert);
		}
		else if (valueVariantValue.canConvert<double>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueVariantValue.value<double>());
		}
		else if (valueVariantValue.canConvert<bool>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueVariantValue.value<bool>());
		}
		else if (valueVariantValue.canConvert<QString>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueVariantValue.value<QString>());
		}
	}

	return true;
}


bool CExtendedMetaData::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("key") && (gqlObject["key"].userType() == QMetaType::QString || gqlObject["key"].userType() == QMetaType::QByteArray)){
		key = gqlObject["key"].toString();
	}

	if (gqlObject.ContainsParam("value") && (!((gqlObject.IsObject("item") && gqlObject.GetParamArgumentObjectPtr("value") == nullptr) || (!gqlObject.IsObject("item") && gqlObject["value"].isNull())) && gqlObject.GetParamArgumentObjectPtr("value") != nullptr)){
			const ::imtgql::CGqlParamObject* itemDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("item");
	if (!itemDataObjectPtr->ContainsParam("__typename")) {
		qDebug() << "invalid typename for: value";
		return false;
	}

	QString itemTypename = itemDataObjectPtr->GetParamArgumentValue("__typename").toString();;
		if (itemTypename == "Coordinates") {
			CCoordinates valueConvert;
			const ::imtgql::CGqlParamObject* valueDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("value");
			const bool isValueRead = valueConvert.ReadFromGraphQlObject(*valueDataObjectPtr);
			if (!isValueRead){
				return false;
			}
			value = std::make_shared<ExtendedMetaDataUnionType>(valueConvert);
		}
		else if (valueVariantValue.canConvert<double>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueVariantValue.value<double>());
		}
		else if (valueVariantValue.canConvert<bool>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueVariantValue.value<bool>());
		}
		else if (valueVariantValue.canConvert<QString>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueVariantValue.value<QString>());
		}
	}

	return true;
}


bool CExtendedMetaData::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (key){
		jsonObject["key"] = QJsonValue::fromVariant(*key);
	}

	if (value){
		if (const CCoordinates* val = std::get_if<CCoordinates>((*value).get())){
			QJsonObject valueJsonObject;
			const bool isvalueAdded = val->WriteToJsonObject(valueJsonObject);
			if (!isvalueAdded){
				return false;
			}
			jsonObject["value"] = valueJsonObject;
		}
		else if (const double* val = std::get_if<double>((*value).get())){
			if (!value){
				return false;
			}
			jsonObject["value"] = QJsonValue::fromVariant(*val);
		}
		else if (const bool* val = std::get_if<bool>((*value).get())){
			if (!value){
				return false;
			}
			jsonObject["value"] = QJsonValue::fromVariant(*val);
		}
		else if (const QString* val = std::get_if<QString>((*value).get())){
			if (!value){
				return false;
			}
			jsonObject["value"] = QJsonValue::fromVariant(*val);
		}

	}

	return true;
}


bool CExtendedMetaData::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("key") && jsonObject["key"].isString()){
		key = jsonObject["key"].toString();
	}

	if (jsonObject.contains("value") && jsonObject["value"].isObject()){
			QJsonObject itemObject = jsonObject.value("item").toObject();
	if (!itemObject.contains("__typename")){
		qDebug() << "invalid typename for: value";
		return false;
	}

	QString itemTypename = itemObject.value("__typename").toString();;
		if (itemTypename == "Coordinates") {
			CCoordinates valueConvert;
			const bool isvalueRead = valueConvert.ReadFromJsonObject(jsonObject["value"].toObject());
			if (!isvalueRead){
				return false;
			}
			value = std::make_shared<ExtendedMetaDataUnionType>(valueConvert);
		}
		else if (valueVariantValue.canConvert<double>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueVariantValue.value<double>());
		}
		else if (valueVariantValue.canConvert<bool>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueVariantValue.value<bool>());
		}
		else if (valueVariantValue.canConvert<QString>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueVariantValue.value<QString>());
		}
	}

	return true;
}


bool CExtendedMetaData::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("key") && jsonObject["key"].isString()){
		key = jsonObject["key"].toString();
	}

	if (jsonObject.contains("value") && jsonObject["value"].isObject()){
			QJsonObject itemObject = jsonObject.value("item").toObject();
	if (!itemObject.contains("__typename")){
		qDebug() << "invalid typename for: value";
		return false;
	}

	QString itemTypename = itemObject.value("__typename").toString();;
		if (itemTypename == "Coordinates") {
			CCoordinates valueConvert;
			const bool isvalueRead = valueConvert.ReadFromJsonObject(jsonObject["value"].toObject());
			if (!isvalueRead){
				return false;
			}
			value = std::make_shared<ExtendedMetaDataUnionType>(valueConvert);
		}
		else if (valueVariantValue.canConvert<double>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueVariantValue.value<double>());
		}
		else if (valueVariantValue.canConvert<bool>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueVariantValue.value<bool>());
		}
		else if (valueVariantValue.canConvert<QString>()){
			value = std::make_shared<ExtendedMetaDataUnionType>(valueVariantValue.value<QString>());
		}
	}

	return true;
}


// serialize methods

bool CExtendedMetaData::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CExtendedMetaData::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CExtendedMetaData::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CExtendedMetaData::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CExtendedMetaData::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CExtendedMetaData::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CExtendedMetaData::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CExtendedMetaData::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CExtendedMetaData::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


CExtendedMetaDataObject::CExtendedMetaDataObject(QObject* parent): ::imtbase::CItemModelBase(parent)			, m_valueQObjectPtr(nullptr)
{
	Version_1_0.emplace();

	QObject::connect(this, &CExtendedMetaDataObject::keyChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CExtendedMetaDataObject::valueChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString CExtendedMetaDataObject::GetKey()
{
	if (Version_1_0->key.has_value()){
		return Version_1_0->key.value();
	}

	return QString();
}


void CExtendedMetaDataObject::SetKey(QString v)
{
	Version_1_0->key = v;
	keyChanged();
}


bool CExtendedMetaDataObject::hasKey()
{
	 return Version_1_0->key.HasValue();
}


sdl::complextest::CTImpl::CExtendedMetaDataUnionTypeObject* CExtendedMetaDataObject::GetValue()
{
	if (Version_1_0->value.has_value()){
		if (!m_valueQObjectPtr){
			m_valueQObjectPtr = dynamic_cast<sdl::complextest::CTImpl::CExtendedMetaDataUnionTypeObject*>(CreateObject("value"));
			m_valueQObjectPtr->Version_1_0 = Version_1_0->value;
		}
		return m_valueQObjectPtr;
	}

	return nullptr;
}


void CExtendedMetaDataObject::SetValue(sdl::complextest::CTImpl::CExtendedMetaDataUnionTypeObject* v)
{
	if (v){
		Version_1_0->value = v->Version_1_0;
		m_valueQObjectPtr = v;
	}
	else {
		Version_1_0->value = nullptr;
	}

	valueChanged();
}


bool CExtendedMetaDataObject::hasValue()
{
	 return Version_1_0->value.HasValue();
}


void CExtendedMetaDataObject::createValue()
{	Version_1_0->value.emplace();

}


QString CExtendedMetaDataObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CExtendedMetaDataObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CExtendedMetaDataObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CExtendedMetaDataObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QObject* CExtendedMetaDataObject::CreateObject(const QString& key)
{
	if (key == "value"){
		return new sdl::complextest::CTImpl::CExtendedMetaDataUnionTypeObject(this);
	}
	return nullptr;
}


QString CExtendedMetaDataObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "key")){
		return "key";
	}
	if (propertyName == (QString("m_") + "value")){
		return "value";
	}

	return propertyName;
}


} // namespace sdl::complextest::CTImpl




/// \file CResultMetaData.cpp

namespace sdl::complextest::CTImpl
{


QByteArray CResultMetaData::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CResultMetaData::V1_0::operator==(const V1_0& other) const
{
	return 
				resultId == other.resultId &&
				creationTime == other.creationTime &&
				partId == other.partId &&
				productId == other.productId &&
				name == other.name &&
				classification == other.classification &&
				description == other.description &&
				extendedMetaData == other.extendedMetaData;
}


bool CResultMetaData::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (!resultId){
		return false;
	}
	model.SetData("resultId", *resultId, modelIndex);

	if (creationTime){
		model.SetData("creationTime", *creationTime, modelIndex);
	}

	if (partId){
		model.SetData("partId", *partId, modelIndex);
	}

	if (productId){
		model.SetData("productId", *productId, modelIndex);
	}

	if (name){
		model.SetData("name", *name, modelIndex);
	}

	if (classification){
		model.SetData("classification", *classification, modelIndex);
	}


	if (description){
		::imtbase::CTreeItemModel* descriptionNewModelPtr = model.AddTreeModel("description", modelIndex);
		const bool isDescriptionAdded = description->WriteToModel(*descriptionNewModelPtr, 0);
		if (!isDescriptionAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "description");)

			return false;
		}

	}
	if (extendedMetaData){
		::imtbase::CTreeItemModel* newExtendedMetaDataModelPtr = model.AddTreeModel("extendedMetaData", modelIndex);
		newExtendedMetaDataModelPtr->setIsArray(true);
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaData->size(); ++extendedMetaDataIndex){
			newExtendedMetaDataModelPtr->InsertNewItem();
			if (!(extendedMetaData->at(extendedMetaDataIndex).WriteToModel(*newExtendedMetaDataModelPtr, extendedMetaDataIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData");)

				return false;
			}
		}
	}

	return true;
}


bool CResultMetaData::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant resultIdData = model.GetData("resultId", modelIndex);
	if (resultIdData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "resultId");)

		return false;
	}
	resultId = resultIdData.toByteArray();

	QVariant creationTimeData = model.GetData("creationTime", modelIndex);
	if (!creationTimeData.isNull()){
		creationTime = creationTimeData.toString();
	}

	QVariant partIdData = model.GetData("partId", modelIndex);
	if (!partIdData.isNull()){
		partId = partIdData.toString();
	}

	QVariant productIdData = model.GetData("productId", modelIndex);
	if (!productIdData.isNull()){
		productId = productIdData.toString();
	}

	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	QVariant classificationData = model.GetData("classification", modelIndex);
	if (!classificationData.isNull()){
		classification = classificationData.toInt();
	}

	::imtbase::CTreeItemModel* descriptionDataModelPtr = model.GetTreeItemModel("description", modelIndex);
	if (descriptionDataModelPtr != nullptr){
		description = CLocalizedText::V1_0();
		const bool isDescriptionReaded = description->ReadFromModel(*descriptionDataModelPtr, modelIndex);
		if (!isDescriptionReaded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "description");)

			return false;
		}
	}

	::imtbase::CTreeItemModel* extendedMetaDataModel = model.GetTreeItemModel("extendedMetaData", modelIndex);
	if (extendedMetaDataModel != nullptr){
		int extendedMetaDataCount = extendedMetaDataModel->GetItemsCount();
		QList<CExtendedMetaData::V1_0> extendedMetaDataList;
		for (int extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataCount; ++extendedMetaDataIndex){
			CExtendedMetaData::V1_0 extendedMetaData;
			if (!extendedMetaData.ReadFromModel(*extendedMetaDataModel, extendedMetaDataIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData");)

				return false;
			}
			extendedMetaDataList << extendedMetaData;
		}
		extendedMetaData = extendedMetaDataList;

	}

	return true;
}


bool CResultMetaData::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant resultIdData = model.GetData("resultId", modelIndex);
	if (!resultIdData.isNull()){
		resultId = resultIdData.toByteArray();
	}

	QVariant creationTimeData = model.GetData("creationTime", modelIndex);
	if (!creationTimeData.isNull()){
		creationTime = creationTimeData.toString();
	}

	QVariant partIdData = model.GetData("partId", modelIndex);
	if (!partIdData.isNull()){
		partId = partIdData.toString();
	}

	QVariant productIdData = model.GetData("productId", modelIndex);
	if (!productIdData.isNull()){
		productId = productIdData.toString();
	}

	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	QVariant classificationData = model.GetData("classification", modelIndex);
	if (!classificationData.isNull()){
		classification = classificationData.toInt();
	}

	::imtbase::CTreeItemModel* descriptionDataModelPtr = model.GetTreeItemModel("description", modelIndex);
	if (descriptionDataModelPtr != nullptr){
		description = CLocalizedText::V1_0();
		const bool isDescriptionReaded = description->ReadFromModel(*descriptionDataModelPtr, modelIndex);
		if (!isDescriptionReaded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "description");)

			return false;
		}
	}

	::imtbase::CTreeItemModel* extendedMetaDataModel = model.GetTreeItemModel("extendedMetaData", modelIndex);
	if (extendedMetaDataModel != nullptr){
		int extendedMetaDataCount = extendedMetaDataModel->GetItemsCount();
		QList<CExtendedMetaData::V1_0> extendedMetaDataList;
		for (int extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataCount; ++extendedMetaDataIndex){
			CExtendedMetaData::V1_0 extendedMetaData;
			if (!extendedMetaData.OptReadFromModel(*extendedMetaDataModel, extendedMetaDataIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData");)

				return false;
			}
			extendedMetaDataList << extendedMetaData;
		}
		extendedMetaData = extendedMetaDataList;

	}

	return true;
}


bool CResultMetaData::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!resultId){
		return false;
	}
	gqlObject.InsertParam("resultId", QVariant(*resultId));

	if (creationTime){
		gqlObject.InsertParam("creationTime", QVariant(*creationTime));
	}

	if (partId){
		gqlObject.InsertParam("partId", QVariant(*partId));
	}

	if (productId){
		gqlObject.InsertParam("productId", QVariant(*productId));
	}

	if (name){
		gqlObject.InsertParam("name", QVariant(*name));
	}

	if (classification){
		gqlObject.InsertParam("classification", QVariant(*classification));
	}

	if (description){
		::imtgql::CGqlParamObject descriptionGqlObject;
		const bool isDescriptionAdded = description->WriteToGraphQlObject(descriptionGqlObject);
		if (!isDescriptionAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "description");)

			return false;
		}
		gqlObject.InsertParam("description", descriptionGqlObject);
	}

	if (extendedMetaData){
		QList<::imtgql::CGqlParamObject> extendedMetaDataDataObjectList;
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaData->size(); ++extendedMetaDataIndex){
			::imtgql::CGqlParamObject newExtendedMetaDataGqlObject;
			if (!extendedMetaData->at(extendedMetaDataIndex).WriteToGraphQlObject(newExtendedMetaDataGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData");)

				return false;
			}
			extendedMetaDataDataObjectList << newExtendedMetaDataGqlObject;
		}
		gqlObject.InsertParam("extendedMetaData", extendedMetaDataDataObjectList);
	}

	return true;
}


bool CResultMetaData::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("resultId") || (gqlObject["resultId"].userType() != QMetaType::QString && gqlObject["resultId"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "resultId");)

		return false;
	}
	resultId = gqlObject["resultId"].toByteArray();

	if (gqlObject.ContainsParam("creationTime") && (gqlObject["creationTime"].userType() == QMetaType::QString || gqlObject["creationTime"].userType() == QMetaType::QByteArray)){
		creationTime = gqlObject["creationTime"].toString();
	}

	if (gqlObject.ContainsParam("partId") && (gqlObject["partId"].userType() == QMetaType::QString || gqlObject["partId"].userType() == QMetaType::QByteArray)){
		partId = gqlObject["partId"].toString();
	}

	if (gqlObject.ContainsParam("productId") && (gqlObject["productId"].userType() == QMetaType::QString || gqlObject["productId"].userType() == QMetaType::QByteArray)){
		productId = gqlObject["productId"].toString();
	}

	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (gqlObject.ContainsParam("classification") && (gqlObject["classification"].userType() == QMetaType::Int || gqlObject["classification"].userType() == QMetaType::UInt || gqlObject["classification"].userType() == QMetaType::LongLong || gqlObject["classification"].userType() == QMetaType::ULongLong || gqlObject["classification"].userType() == QMetaType::Long || gqlObject["classification"].userType() == QMetaType::Short || gqlObject["classification"].userType() == QMetaType::ULong || gqlObject["classification"].userType() == QMetaType::UShort || gqlObject["classification"].userType() == QMetaType::UChar)){
		classification = gqlObject["classification"].toInt();
	}

	if (gqlObject.ContainsParam("description") && (gqlObject.GetParamArgumentObjectPtr("description") != nullptr)){
		description = CLocalizedText::V1_0();
		const bool isDescriptionRead = description->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("description"));
		if (!isDescriptionRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "description");)

			return false;
		}
	}

	if (gqlObject.ContainsParam("extendedMetaData") && (gqlObject.GetObjectsCount("extendedMetaData") > 0)){
		const qsizetype extendedMetaDataElementsCount = gqlObject.GetObjectsCount("extendedMetaData");
		extendedMetaData = QList<CExtendedMetaData::V1_0>();
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataElementsCount; ++extendedMetaDataIndex){
			const ::imtgql::CGqlParamObject* extendedMetaDataDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("extendedMetaData", extendedMetaDataIndex);
			if (extendedMetaDataDataObjectPtr == nullptr){
				qDebug() << "invalid type" << extendedMetaDataDataObjectPtr;
				return false;
			}
			CExtendedMetaData::V1_0 tempExtendedMetaData;
			if (!tempExtendedMetaData.ReadFromGraphQlObject(*extendedMetaDataDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData");)

				return false;
			}
			extendedMetaData->append(tempExtendedMetaData);
		}
	}

	return true;
}


bool CResultMetaData::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("resultId") && (gqlObject["resultId"].userType() == QMetaType::QString || gqlObject["resultId"].userType() == QMetaType::QByteArray)){
		resultId = gqlObject["resultId"].toByteArray();
	}

	if (gqlObject.ContainsParam("creationTime") && (gqlObject["creationTime"].userType() == QMetaType::QString || gqlObject["creationTime"].userType() == QMetaType::QByteArray)){
		creationTime = gqlObject["creationTime"].toString();
	}

	if (gqlObject.ContainsParam("partId") && (gqlObject["partId"].userType() == QMetaType::QString || gqlObject["partId"].userType() == QMetaType::QByteArray)){
		partId = gqlObject["partId"].toString();
	}

	if (gqlObject.ContainsParam("productId") && (gqlObject["productId"].userType() == QMetaType::QString || gqlObject["productId"].userType() == QMetaType::QByteArray)){
		productId = gqlObject["productId"].toString();
	}

	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (gqlObject.ContainsParam("classification") && (gqlObject["classification"].userType() == QMetaType::Int || gqlObject["classification"].userType() == QMetaType::UInt || gqlObject["classification"].userType() == QMetaType::LongLong || gqlObject["classification"].userType() == QMetaType::ULongLong || gqlObject["classification"].userType() == QMetaType::Long || gqlObject["classification"].userType() == QMetaType::Short || gqlObject["classification"].userType() == QMetaType::ULong || gqlObject["classification"].userType() == QMetaType::UShort || gqlObject["classification"].userType() == QMetaType::UChar)){
		classification = gqlObject["classification"].toInt();
	}

	if (gqlObject.ContainsParam("description") && (gqlObject.GetParamArgumentObjectPtr("description") != nullptr)){
		description = CLocalizedText::V1_0();
		const bool isDescriptionRead = description->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("description"));
		if (!isDescriptionRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "description");)

			return false;
		}
	}

	if (gqlObject.ContainsParam("extendedMetaData") && (gqlObject.GetObjectsCount("extendedMetaData") > 0)){
		const qsizetype extendedMetaDataElementsCount = gqlObject.GetObjectsCount("extendedMetaData");
		extendedMetaData = QList<CExtendedMetaData::V1_0>();
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataElementsCount; ++extendedMetaDataIndex){
			const ::imtgql::CGqlParamObject* extendedMetaDataDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("extendedMetaData", extendedMetaDataIndex);
			if (extendedMetaDataDataObjectPtr == nullptr){
				qDebug() << "invalid type" << extendedMetaDataDataObjectPtr;
				return false;
			}
			CExtendedMetaData::V1_0 tempExtendedMetaData;
			if (!tempExtendedMetaData.OptReadFromGraphQlObject(*extendedMetaDataDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData");)

				return false;
			}
			extendedMetaData->append(tempExtendedMetaData);
		}
	}

	return true;
}


bool CResultMetaData::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!resultId){
		return false;
	}
	jsonObject["resultId"] = QJsonValue::fromVariant(*resultId);

	if (creationTime){
		jsonObject["creationTime"] = QJsonValue::fromVariant(*creationTime);
	}

	if (partId){
		jsonObject["partId"] = QJsonValue::fromVariant(*partId);
	}

	if (productId){
		jsonObject["productId"] = QJsonValue::fromVariant(*productId);
	}

	if (name){
		jsonObject["name"] = QJsonValue::fromVariant(*name);
	}

	if (classification){
		jsonObject["classification"] = QJsonValue::fromVariant(*classification);
	}

	if (description){
		QJsonObject descriptionJsonObject;
		const bool isDescriptionAdded = description->WriteToJsonObject(descriptionJsonObject);
		if (!isDescriptionAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "description");)

			return false;
		}
		jsonObject["description"] = descriptionJsonObject;
	}

	if (extendedMetaData){
		QJsonArray newExtendedMetaDataArray;
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaData->size(); ++extendedMetaDataIndex){
			QJsonObject newExtendedMetaDataJsonObject;
			if (!extendedMetaData->at(extendedMetaDataIndex).WriteToJsonObject(newExtendedMetaDataJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData");)

				return false;
			}
			newExtendedMetaDataArray << newExtendedMetaDataJsonObject;
		}
		jsonObject["extendedMetaData"] = newExtendedMetaDataArray;
	}

	return true;
}


bool CResultMetaData::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("resultId") || ! jsonObject["resultId"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "resultId");)

		return false;
	}
	resultId = jsonObject["resultId"].toString().toUtf8();

	if (jsonObject.contains("creationTime") && jsonObject["creationTime"].isString()){
		creationTime = jsonObject["creationTime"].toString();
	}

	if (jsonObject.contains("partId") && jsonObject["partId"].isString()){
		partId = jsonObject["partId"].toString();
	}

	if (jsonObject.contains("productId") && jsonObject["productId"].isString()){
		productId = jsonObject["productId"].toString();
	}

	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (jsonObject.contains("classification") && jsonObject["classification"].isDouble()){
		classification = jsonObject["classification"].toInt();
	}

	if (jsonObject.contains("description") && jsonObject["description"].isObject()){
		description = CLocalizedText::V1_0();
		const bool isDescriptionRead = description->ReadFromJsonObject(jsonObject["description"].toObject());
		if (!isDescriptionRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "description");)

			return false;
		}
	}

	if (jsonObject.contains("extendedMetaData") && jsonObject["extendedMetaData"].isArray()){
		const QJsonArray extendedMetaDataJsonArray = jsonObject["extendedMetaData"].toArray();
		const qsizetype extendedMetaDataArrayCount = extendedMetaDataJsonArray.size();
		extendedMetaData = QList<CExtendedMetaData::V1_0>();
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataArrayCount; ++extendedMetaDataIndex){
			CExtendedMetaData::V1_0 tempExtendedMetaData;
			if (!tempExtendedMetaData.ReadFromJsonObject(extendedMetaDataJsonArray[extendedMetaDataIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData");)

				return false;
			}
			extendedMetaData->append(tempExtendedMetaData);
		}
	}

	return true;
}


bool CResultMetaData::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("resultId") && jsonObject["resultId"].isString()){
		resultId = jsonObject["resultId"].toString().toUtf8();
	}

	if (jsonObject.contains("creationTime") && jsonObject["creationTime"].isString()){
		creationTime = jsonObject["creationTime"].toString();
	}

	if (jsonObject.contains("partId") && jsonObject["partId"].isString()){
		partId = jsonObject["partId"].toString();
	}

	if (jsonObject.contains("productId") && jsonObject["productId"].isString()){
		productId = jsonObject["productId"].toString();
	}

	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (jsonObject.contains("classification") && jsonObject["classification"].isDouble()){
		classification = jsonObject["classification"].toInt();
	}

	if (jsonObject.contains("description") && jsonObject["description"].isObject()){
		description = CLocalizedText::V1_0();
		const bool isDescriptionRead = description->OptReadFromJsonObject(jsonObject["description"].toObject());
		if (!isDescriptionRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "description");)

			return false;
		}
	}

	if (jsonObject.contains("extendedMetaData") && jsonObject["extendedMetaData"].isArray()){
		const QJsonArray extendedMetaDataJsonArray = jsonObject["extendedMetaData"].toArray();
		const qsizetype extendedMetaDataArrayCount = extendedMetaDataJsonArray.size();
		extendedMetaData = QList<CExtendedMetaData::V1_0>();
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataArrayCount; ++extendedMetaDataIndex){
			CExtendedMetaData::V1_0 tempExtendedMetaData;
			if (!tempExtendedMetaData.OptReadFromJsonObject(extendedMetaDataJsonArray[extendedMetaDataIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData");)

				return false;
			}
			extendedMetaData->append(tempExtendedMetaData);
		}
	}

	return true;
}


// serialize methods

bool CResultMetaData::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CResultMetaData::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CResultMetaData::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CResultMetaData::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CResultMetaData::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CResultMetaData::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CResultMetaData::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CResultMetaData::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CResultMetaData::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


CResultMetaDataObject::CResultMetaDataObject(QObject* parent): ::imtbase::CItemModelBase(parent)			, m_descriptionQObjectPtr(nullptr)
			, m_extendedMetaDataQObjectPtr(nullptr)
{
	Version_1_0.emplace();

	QObject::connect(this, &CResultMetaDataObject::resultIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::creationTimeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::partIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::productIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::classificationChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::descriptionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::extendedMetaDataChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString CResultMetaDataObject::GetResultId()
{
	if (Version_1_0->resultId.has_value()){
		return Version_1_0->resultId.value();
	}

	return QString();
}


void CResultMetaDataObject::SetResultId(QString v)
{
	Version_1_0->resultId = v.toUtf8();
	resultIdChanged();
}


bool CResultMetaDataObject::hasResultId()
{
	 return Version_1_0->resultId.HasValue();
}


QString CResultMetaDataObject::GetCreationTime()
{
	if (Version_1_0->creationTime.has_value()){
		return Version_1_0->creationTime.value();
	}

	return QString();
}


void CResultMetaDataObject::SetCreationTime(QString v)
{
	Version_1_0->creationTime = v;
	creationTimeChanged();
}


bool CResultMetaDataObject::hasCreationTime()
{
	 return Version_1_0->creationTime.HasValue();
}


QString CResultMetaDataObject::GetPartId()
{
	if (Version_1_0->partId.has_value()){
		return Version_1_0->partId.value();
	}

	return QString();
}


void CResultMetaDataObject::SetPartId(QString v)
{
	Version_1_0->partId = v;
	partIdChanged();
}


bool CResultMetaDataObject::hasPartId()
{
	 return Version_1_0->partId.HasValue();
}


QString CResultMetaDataObject::GetProductId()
{
	if (Version_1_0->productId.has_value()){
		return Version_1_0->productId.value();
	}

	return QString();
}


void CResultMetaDataObject::SetProductId(QString v)
{
	Version_1_0->productId = v;
	productIdChanged();
}


bool CResultMetaDataObject::hasProductId()
{
	 return Version_1_0->productId.HasValue();
}


QString CResultMetaDataObject::GetName()
{
	if (Version_1_0->name.has_value()){
		return Version_1_0->name.value();
	}

	return QString();
}


void CResultMetaDataObject::SetName(QString v)
{
	Version_1_0->name = v;
	nameChanged();
}


bool CResultMetaDataObject::hasName()
{
	 return Version_1_0->name.HasValue();
}


int CResultMetaDataObject::GetClassification()
{
	if (Version_1_0->classification.has_value()){
		return Version_1_0->classification.value();
	}

	return 0;
}


void CResultMetaDataObject::SetClassification(int v)
{
	Version_1_0->classification = v;
	classificationChanged();
}


bool CResultMetaDataObject::hasClassification()
{
	 return Version_1_0->classification.HasValue();
}


sdl::complextest::CTImpl::CLocalizedTextObject* CResultMetaDataObject::GetDescription()
{
	if (Version_1_0->description.has_value()){
		if (!m_descriptionQObjectPtr){
			m_descriptionQObjectPtr = dynamic_cast<sdl::complextest::CTImpl::CLocalizedTextObject*>(CreateObject("description"));
			m_descriptionQObjectPtr->Version_1_0 = Version_1_0->description;
		}
		return m_descriptionQObjectPtr;
	}

	return nullptr;
}


void CResultMetaDataObject::SetDescription(sdl::complextest::CTImpl::CLocalizedTextObject* v)
{
	if (v){
		Version_1_0->description = v->Version_1_0;
		m_descriptionQObjectPtr = v;
	}
	else {
		Version_1_0->description = nullptr;
	}

	descriptionChanged();
}


bool CResultMetaDataObject::hasDescription()
{
	 return Version_1_0->description.HasValue();
}


void CResultMetaDataObject::createDescription()
{	Version_1_0->description.emplace();

}


sdl::complextest::CTImpl::CExtendedMetaDataObjectList* CResultMetaDataObject::GetExtendedMetaData()
{
	if (Version_1_0->extendedMetaData.has_value()){
		if (!m_extendedMetaDataQObjectPtr){
			m_extendedMetaDataQObjectPtr = dynamic_cast<sdl::complextest::CTImpl::CExtendedMetaDataObjectList*>(CreateObject("extendedMetaData"));
			m_extendedMetaDataQObjectPtr->Version_1_0 = Version_1_0->extendedMetaData;
		}
		return m_extendedMetaDataQObjectPtr;
	}

	return nullptr;
}


void CResultMetaDataObject::SetExtendedMetaData(sdl::complextest::CTImpl::CExtendedMetaDataObjectList* v)
{
	if (v){
		Version_1_0->extendedMetaData = v->Version_1_0;
		m_extendedMetaDataQObjectPtr = v;
	}
	else {
		Version_1_0->extendedMetaData = nullptr;
	}

	extendedMetaDataChanged();
}


bool CResultMetaDataObject::hasExtendedMetaData()
{
	 return Version_1_0->extendedMetaData.HasValue();
}


void CResultMetaDataObject::createExtendedMetaData()
{	Version_1_0->extendedMetaData.emplace();

}


QString CResultMetaDataObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CResultMetaDataObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CResultMetaDataObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CResultMetaDataObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QObject* CResultMetaDataObject::CreateObject(const QString& key)
{
	if (key == "description"){
		return new sdl::complextest::CTImpl::CLocalizedTextObject(this);
	}
	if (key == "extendedMetaData"){
		return new sdl::complextest::CTImpl::CExtendedMetaDataObjectList(this);
	}
	return nullptr;
}


QString CResultMetaDataObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "resultId")){
		return "resultId";
	}
	if (propertyName == (QString("m_") + "creationTime")){
		return "creationTime";
	}
	if (propertyName == (QString("m_") + "partId")){
		return "partId";
	}
	if (propertyName == (QString("m_") + "productId")){
		return "productId";
	}
	if (propertyName == (QString("m_") + "name")){
		return "name";
	}
	if (propertyName == (QString("m_") + "classification")){
		return "classification";
	}
	if (propertyName == (QString("m_") + "description")){
		return "description";
	}
	if (propertyName == (QString("m_") + "extendedMetaData")){
		return "extendedMetaData";
	}

	return propertyName;
}


} // namespace sdl::complextest::CTImpl




/// \file COverallResultValues.cpp

namespace sdl::complextest::CTImpl
{


QByteArray COverallResultValues::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool COverallResultValues::V1_0::operator==(const V1_0& other) const
{
	return 
				name == other.name &&
				valueId == other.valueId &&
				measuredValue.has_value() == other.measuredValue.has_value() &&
				((measuredValue.has_value() && other.measuredValue.has_value()) ?
					qFuzzyCompare(*measuredValue, *other.measuredValue) : true) &&
				lowLimit.has_value() == other.lowLimit.has_value() &&
				((lowLimit.has_value() && other.lowLimit.has_value()) ?
					qFuzzyCompare(*lowLimit, *other.lowLimit) : true) &&
				highLimit.has_value() == other.highLimit.has_value() &&
				((highLimit.has_value() && other.highLimit.has_value()) ?
					qFuzzyCompare(*highLimit, *other.highLimit) : true);
}


bool COverallResultValues::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (name){
		model.SetData("name", *name, modelIndex);
	}

	if (!valueId){
		return false;
	}
	model.SetData("valueId", *valueId, modelIndex);

	if (!measuredValue){
		return false;
	}
	model.SetData("measuredValue", *measuredValue, modelIndex);

	if (lowLimit){
		model.SetData("lowLimit", *lowLimit, modelIndex);
	}

	if (highLimit){
		model.SetData("highLimit", *highLimit, modelIndex);
	}


	return true;
}


bool COverallResultValues::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	QVariant valueIdData = model.GetData("valueId", modelIndex);
	if (valueIdData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "valueId");)

		return false;
	}
	valueId = valueIdData.toString();

	QVariant measuredValueData = model.GetData("measuredValue", modelIndex);
	if (measuredValueData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "measuredValue");)

		return false;
	}
	measuredValue = measuredValueData.toFloat();

	QVariant lowLimitData = model.GetData("lowLimit", modelIndex);
	if (!lowLimitData.isNull()){
		lowLimit = lowLimitData.toFloat();
	}

	QVariant highLimitData = model.GetData("highLimit", modelIndex);
	if (!highLimitData.isNull()){
		highLimit = highLimitData.toFloat();
	}

	return true;
}


bool COverallResultValues::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	QVariant valueIdData = model.GetData("valueId", modelIndex);
	if (!valueIdData.isNull()){
		valueId = valueIdData.toString();
	}

	QVariant measuredValueData = model.GetData("measuredValue", modelIndex);
	if (!measuredValueData.isNull()){
		measuredValue = measuredValueData.toFloat();
	}

	QVariant lowLimitData = model.GetData("lowLimit", modelIndex);
	if (!lowLimitData.isNull()){
		lowLimit = lowLimitData.toFloat();
	}

	QVariant highLimitData = model.GetData("highLimit", modelIndex);
	if (!highLimitData.isNull()){
		highLimit = highLimitData.toFloat();
	}

	return true;
}


bool COverallResultValues::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (name){
		gqlObject.InsertParam("name", QVariant(*name));
	}

	if (!valueId){
		return false;
	}
	gqlObject.InsertParam("valueId", QVariant(*valueId));

	if (!measuredValue){
		return false;
	}
	gqlObject.InsertParam("measuredValue", QVariant(*measuredValue));

	if (lowLimit){
		gqlObject.InsertParam("lowLimit", QVariant(*lowLimit));
	}

	if (highLimit){
		gqlObject.InsertParam("highLimit", QVariant(*highLimit));
	}

	return true;
}


bool COverallResultValues::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (!gqlObject.ContainsParam("valueId") || (gqlObject["valueId"].userType() != QMetaType::QString && gqlObject["valueId"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "valueId");)

		return false;
	}
	valueId = gqlObject["valueId"].toString();

	if (!gqlObject.ContainsParam("measuredValue") || (gqlObject["measuredValue"].userType() != QMetaType::Float && gqlObject["measuredValue"].userType() != QMetaType::Double && gqlObject["measuredValue"].userType() != QMetaType::Int && gqlObject["measuredValue"].userType() != QMetaType::UInt && gqlObject["measuredValue"].userType() != QMetaType::LongLong && gqlObject["measuredValue"].userType() != QMetaType::ULongLong && gqlObject["measuredValue"].userType() != QMetaType::Long && gqlObject["measuredValue"].userType() != QMetaType::Short && gqlObject["measuredValue"].userType() != QMetaType::ULong && gqlObject["measuredValue"].userType() != QMetaType::UShort && gqlObject["measuredValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "measuredValue");)

		return false;
	}
	measuredValue = gqlObject["measuredValue"].toDouble();

	if (gqlObject.ContainsParam("lowLimit") && (gqlObject["lowLimit"].userType() == QMetaType::Float || gqlObject["lowLimit"].userType() == QMetaType::Double || gqlObject["lowLimit"].userType() == QMetaType::Int || gqlObject["lowLimit"].userType() == QMetaType::UInt || gqlObject["lowLimit"].userType() == QMetaType::LongLong || gqlObject["lowLimit"].userType() == QMetaType::ULongLong || gqlObject["lowLimit"].userType() == QMetaType::Long || gqlObject["lowLimit"].userType() == QMetaType::Short || gqlObject["lowLimit"].userType() == QMetaType::ULong || gqlObject["lowLimit"].userType() == QMetaType::UShort || gqlObject["lowLimit"].userType() == QMetaType::UChar)){
		lowLimit = gqlObject["lowLimit"].toDouble();
	}

	if (gqlObject.ContainsParam("highLimit") && (gqlObject["highLimit"].userType() == QMetaType::Float || gqlObject["highLimit"].userType() == QMetaType::Double || gqlObject["highLimit"].userType() == QMetaType::Int || gqlObject["highLimit"].userType() == QMetaType::UInt || gqlObject["highLimit"].userType() == QMetaType::LongLong || gqlObject["highLimit"].userType() == QMetaType::ULongLong || gqlObject["highLimit"].userType() == QMetaType::Long || gqlObject["highLimit"].userType() == QMetaType::Short || gqlObject["highLimit"].userType() == QMetaType::ULong || gqlObject["highLimit"].userType() == QMetaType::UShort || gqlObject["highLimit"].userType() == QMetaType::UChar)){
		highLimit = gqlObject["highLimit"].toDouble();
	}

	return true;
}


bool COverallResultValues::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (gqlObject.ContainsParam("valueId") && (gqlObject["valueId"].userType() == QMetaType::QString || gqlObject["valueId"].userType() == QMetaType::QByteArray)){
		valueId = gqlObject["valueId"].toString();
	}

	if (gqlObject.ContainsParam("measuredValue") && (gqlObject["measuredValue"].userType() == QMetaType::Float || gqlObject["measuredValue"].userType() == QMetaType::Double || gqlObject["measuredValue"].userType() == QMetaType::Int || gqlObject["measuredValue"].userType() == QMetaType::UInt || gqlObject["measuredValue"].userType() == QMetaType::LongLong || gqlObject["measuredValue"].userType() == QMetaType::ULongLong || gqlObject["measuredValue"].userType() == QMetaType::Long || gqlObject["measuredValue"].userType() == QMetaType::Short || gqlObject["measuredValue"].userType() == QMetaType::ULong || gqlObject["measuredValue"].userType() == QMetaType::UShort || gqlObject["measuredValue"].userType() == QMetaType::UChar)){
		measuredValue = gqlObject["measuredValue"].toDouble();
	}

	if (gqlObject.ContainsParam("lowLimit") && (gqlObject["lowLimit"].userType() == QMetaType::Float || gqlObject["lowLimit"].userType() == QMetaType::Double || gqlObject["lowLimit"].userType() == QMetaType::Int || gqlObject["lowLimit"].userType() == QMetaType::UInt || gqlObject["lowLimit"].userType() == QMetaType::LongLong || gqlObject["lowLimit"].userType() == QMetaType::ULongLong || gqlObject["lowLimit"].userType() == QMetaType::Long || gqlObject["lowLimit"].userType() == QMetaType::Short || gqlObject["lowLimit"].userType() == QMetaType::ULong || gqlObject["lowLimit"].userType() == QMetaType::UShort || gqlObject["lowLimit"].userType() == QMetaType::UChar)){
		lowLimit = gqlObject["lowLimit"].toDouble();
	}

	if (gqlObject.ContainsParam("highLimit") && (gqlObject["highLimit"].userType() == QMetaType::Float || gqlObject["highLimit"].userType() == QMetaType::Double || gqlObject["highLimit"].userType() == QMetaType::Int || gqlObject["highLimit"].userType() == QMetaType::UInt || gqlObject["highLimit"].userType() == QMetaType::LongLong || gqlObject["highLimit"].userType() == QMetaType::ULongLong || gqlObject["highLimit"].userType() == QMetaType::Long || gqlObject["highLimit"].userType() == QMetaType::Short || gqlObject["highLimit"].userType() == QMetaType::ULong || gqlObject["highLimit"].userType() == QMetaType::UShort || gqlObject["highLimit"].userType() == QMetaType::UChar)){
		highLimit = gqlObject["highLimit"].toDouble();
	}

	return true;
}


bool COverallResultValues::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (name){
		jsonObject["name"] = QJsonValue::fromVariant(*name);
	}

	if (!valueId){
		return false;
	}
	jsonObject["valueId"] = QJsonValue::fromVariant(*valueId);

	if (!measuredValue){
		return false;
	}
	jsonObject["measuredValue"] = QJsonValue::fromVariant(*measuredValue);

	if (lowLimit){
		jsonObject["lowLimit"] = QJsonValue::fromVariant(*lowLimit);
	}

	if (highLimit){
		jsonObject["highLimit"] = QJsonValue::fromVariant(*highLimit);
	}

	return true;
}


bool COverallResultValues::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (!jsonObject.contains("valueId") || ! jsonObject["valueId"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "valueId");)

		return false;
	}
	valueId = jsonObject["valueId"].toString();

	if (!jsonObject.contains("measuredValue") || ! jsonObject["measuredValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "measuredValue");)

		return false;
	}
	measuredValue = jsonObject["measuredValue"].toDouble();

	if (jsonObject.contains("lowLimit") && jsonObject["lowLimit"].isDouble()){
		lowLimit = jsonObject["lowLimit"].toDouble();
	}

	if (jsonObject.contains("highLimit") && jsonObject["highLimit"].isDouble()){
		highLimit = jsonObject["highLimit"].toDouble();
	}

	return true;
}


bool COverallResultValues::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (jsonObject.contains("valueId") && jsonObject["valueId"].isString()){
		valueId = jsonObject["valueId"].toString();
	}

	if (jsonObject.contains("measuredValue") && jsonObject["measuredValue"].isDouble()){
		measuredValue = jsonObject["measuredValue"].toDouble();
	}

	if (jsonObject.contains("lowLimit") && jsonObject["lowLimit"].isDouble()){
		lowLimit = jsonObject["lowLimit"].toDouble();
	}

	if (jsonObject.contains("highLimit") && jsonObject["highLimit"].isDouble()){
		highLimit = jsonObject["highLimit"].toDouble();
	}

	return true;
}


// serialize methods

bool COverallResultValues::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool COverallResultValues::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool COverallResultValues::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool COverallResultValues::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool COverallResultValues::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool COverallResultValues::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool COverallResultValues::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool COverallResultValues::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool COverallResultValues::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


COverallResultValuesObject::COverallResultValuesObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &COverallResultValuesObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COverallResultValuesObject::valueIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COverallResultValuesObject::measuredValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COverallResultValuesObject::lowLimitChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COverallResultValuesObject::highLimitChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString COverallResultValuesObject::GetName()
{
	if (Version_1_0->name.has_value()){
		return Version_1_0->name.value();
	}

	return QString();
}


void COverallResultValuesObject::SetName(QString v)
{
	Version_1_0->name = v;
	nameChanged();
}


bool COverallResultValuesObject::hasName()
{
	 return Version_1_0->name.HasValue();
}


QString COverallResultValuesObject::GetValueId()
{
	if (Version_1_0->valueId.has_value()){
		return Version_1_0->valueId.value();
	}

	return QString();
}


void COverallResultValuesObject::SetValueId(QString v)
{
	Version_1_0->valueId = v;
	valueIdChanged();
}


bool COverallResultValuesObject::hasValueId()
{
	 return Version_1_0->valueId.HasValue();
}


double COverallResultValuesObject::GetMeasuredValue()
{
	if (Version_1_0->measuredValue.has_value()){
		return Version_1_0->measuredValue.value();
	}

	return 0;
}


void COverallResultValuesObject::SetMeasuredValue(double v)
{
	Version_1_0->measuredValue = v;
	measuredValueChanged();
}


bool COverallResultValuesObject::hasMeasuredValue()
{
	 return Version_1_0->measuredValue.HasValue();
}


double COverallResultValuesObject::GetLowLimit()
{
	if (Version_1_0->lowLimit.has_value()){
		return Version_1_0->lowLimit.value();
	}

	return 0;
}


void COverallResultValuesObject::SetLowLimit(double v)
{
	Version_1_0->lowLimit = v;
	lowLimitChanged();
}


bool COverallResultValuesObject::hasLowLimit()
{
	 return Version_1_0->lowLimit.HasValue();
}


double COverallResultValuesObject::GetHighLimit()
{
	if (Version_1_0->highLimit.has_value()){
		return Version_1_0->highLimit.value();
	}

	return 0;
}


void COverallResultValuesObject::SetHighLimit(double v)
{
	Version_1_0->highLimit = v;
	highLimitChanged();
}


bool COverallResultValuesObject::hasHighLimit()
{
	 return Version_1_0->highLimit.HasValue();
}


QString COverallResultValuesObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool COverallResultValuesObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool COverallResultValuesObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString COverallResultValuesObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QObject* COverallResultValuesObject::CreateObject(const QString& key)
{
	return nullptr;
}


QString COverallResultValuesObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "name")){
		return "name";
	}
	if (propertyName == (QString("m_") + "valueId")){
		return "valueId";
	}
	if (propertyName == (QString("m_") + "measuredValue")){
		return "measuredValue";
	}
	if (propertyName == (QString("m_") + "lowLimit")){
		return "lowLimit";
	}
	if (propertyName == (QString("m_") + "highLimit")){
		return "highLimit";
	}

	return propertyName;
}


} // namespace sdl::complextest::CTImpl




/// \file CCDMResultVarRecursive.cpp

namespace sdl::complextest::CTImpl
{


QByteArray CCDMResultVarRecursive::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CCDMResultVarRecursive::V1_0::operator==(const V1_0& other) const
{
	return 
				dataModelTitle == other.dataModelTitle &&
				dataModelVersion == other.dataModelVersion &&
				resultMetaData == other.resultMetaData &&
				resultContent == other.resultContent &&
				overallResultValues == other.overallResultValues;
}


bool CCDMResultVarRecursive::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (!dataModelTitle){
		return false;
	}
	model.SetData("dataModelTitle", *dataModelTitle, modelIndex);

	if (!dataModelVersion){
		return false;
	}
	model.SetData("dataModelVersion", *dataModelVersion, modelIndex);


	if (resultMetaData){
		::imtbase::CTreeItemModel* resultMetaDataNewModelPtr = model.AddTreeModel("resultMetaData", modelIndex);
		const bool isResultMetaDataAdded = resultMetaData->WriteToModel(*resultMetaDataNewModelPtr, 0);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}

	}
	if (resultContent){
		::imtbase::CTreeItemModel* newResultContentModelPtr = model.AddTreeModel("resultContent", modelIndex);
		newResultContentModelPtr->setIsArray(true);
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			QVariant resultContentVariantValue;
			if (const CCDMResultVarString* val = std::get_if<CCDMResultVarString>((resultContent->at(resultContentIndex)).get())){
				newResultContentModelPtr->InsertNewItem();
				if(!val->WriteToModel(*newResultContentModelPtr, resultContentIndex)){
					return false;
				}
			}
			else if (const CCDMResultVarRecursive* val = std::get_if<CCDMResultVarRecursive>((resultContent->at(resultContentIndex)).get())){
				newResultContentModelPtr->InsertNewItem();
				if(!val->WriteToModel(*newResultContentModelPtr, resultContentIndex)){
					return false;
				}
			}
		}
	}

	if (overallResultValues){
		::imtbase::CTreeItemModel* newOverallResultValuesModelPtr = model.AddTreeModel("overallResultValues", modelIndex);
		newOverallResultValuesModelPtr->setIsArray(true);
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			newOverallResultValuesModelPtr->InsertNewItem();
			if (!(overallResultValues->at(overallResultValuesIndex).WriteToModel(*newOverallResultValuesModelPtr, overallResultValuesIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
		}
	}

	return true;
}


bool CCDMResultVarRecursive::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant dataModelTitleData = model.GetData("dataModelTitle", modelIndex);
	if (dataModelTitleData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle");)

		return false;
	}
	dataModelTitle = dataModelTitleData.toString();

	QVariant dataModelVersionData = model.GetData("dataModelVersion", modelIndex);
	if (dataModelVersionData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion");)

		return false;
	}
	dataModelVersion = dataModelVersionData.toString();

	::imtbase::CTreeItemModel* resultMetaDataDataModelPtr = model.GetTreeItemModel("resultMetaData", modelIndex);
	if (resultMetaDataDataModelPtr != nullptr){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataReaded = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataReaded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		QList<std::shared_ptr<CDMResultUnionType>> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			std::shared_ptr<CDMResultUnionType> resultContentData;
			QVariant resultContentVariantValue = resultContentModel->GetData(QByteArray(), resultContentIndex);
			if (itemTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = std::make_shared<CDMResultUnionType>(resultContentDataConvert);
			}
			else if (itemTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = std::make_shared<CDMResultUnionType>(resultContentDataConvert);
			}
			else{
				return false;
			}

			resultContentList << resultContentData;
		}
		resultContent = resultContentList;

	}

	::imtbase::CTreeItemModel* overallResultValuesModel = model.GetTreeItemModel("overallResultValues", modelIndex);
	if (overallResultValuesModel != nullptr){
		int overallResultValuesCount = overallResultValuesModel->GetItemsCount();
		QList<COverallResultValues::V1_0> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			COverallResultValues::V1_0 overallResultValues;
			if (!overallResultValues.ReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValuesList << overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool CCDMResultVarRecursive::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant dataModelTitleData = model.GetData("dataModelTitle", modelIndex);
	if (!dataModelTitleData.isNull()){
		dataModelTitle = dataModelTitleData.toString();
	}

	QVariant dataModelVersionData = model.GetData("dataModelVersion", modelIndex);
	if (!dataModelVersionData.isNull()){
		dataModelVersion = dataModelVersionData.toString();
	}

	::imtbase::CTreeItemModel* resultMetaDataDataModelPtr = model.GetTreeItemModel("resultMetaData", modelIndex);
	if (resultMetaDataDataModelPtr != nullptr){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataReaded = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataReaded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		QList<std::shared_ptr<CDMResultUnionType>> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			std::shared_ptr<CDMResultUnionType> resultContentData;
			QVariant resultContentVariantValue = resultContentModel->GetData(QByteArray(), resultContentIndex);
			if (itemTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = std::make_shared<CDMResultUnionType>(resultContentDataConvert);
			}
			else if (itemTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = std::make_shared<CDMResultUnionType>(resultContentDataConvert);
			}
			else{
				return false;
			}

			resultContentList << resultContentData;
		}
		resultContent = resultContentList;

	}

	::imtbase::CTreeItemModel* overallResultValuesModel = model.GetTreeItemModel("overallResultValues", modelIndex);
	if (overallResultValuesModel != nullptr){
		int overallResultValuesCount = overallResultValuesModel->GetItemsCount();
		QList<COverallResultValues::V1_0> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			COverallResultValues::V1_0 overallResultValues;
			if (!overallResultValues.OptReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValuesList << overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool CCDMResultVarRecursive::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!dataModelTitle){
		return false;
	}
	gqlObject.InsertParam("dataModelTitle", QVariant(*dataModelTitle));

	if (!dataModelVersion){
		return false;
	}
	gqlObject.InsertParam("dataModelVersion", QVariant(*dataModelVersion));

	if (resultMetaData){
		::imtgql::CGqlParamObject resultMetaDataGqlObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToGraphQlObject(resultMetaDataGqlObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
		gqlObject.InsertParam("resultMetaData", resultMetaDataGqlObject);
	}

	if (resultContent){
		QList<::imtgql::CGqlParamObject> resultContentDataObjectList;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			::imtgql::CGqlParamObject resultContentDataObject;
			if (const CCDMResultVarString* val = std::get_if<CCDMResultVarString>((resultContent->at(resultContentIndex)).get())){
				if (!val->WriteToGraphQlObject(resultContentDataObject)){
					return false;
				}
			}
			else if (const CCDMResultVarRecursive* val = std::get_if<CCDMResultVarRecursive>((resultContent->at(resultContentIndex)).get())){
				if (!val->WriteToGraphQlObject(resultContentDataObject)){
					return false;
				}
			}
			resultContentDataObjectList << resultContentDataObject;
		}
		gqlObject.InsertParam("resultContent", resultContentDataObjectList);
	}

	if (overallResultValues){
		QList<::imtgql::CGqlParamObject> overallResultValuesDataObjectList;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			::imtgql::CGqlParamObject newOverallResultValuesGqlObject;
			if (!overallResultValues->at(overallResultValuesIndex).WriteToGraphQlObject(newOverallResultValuesGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValuesDataObjectList << newOverallResultValuesGqlObject;
		}
		gqlObject.InsertParam("overallResultValues", overallResultValuesDataObjectList);
	}

	return true;
}


bool CCDMResultVarRecursive::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("dataModelTitle") || (gqlObject["dataModelTitle"].userType() != QMetaType::QString && gqlObject["dataModelTitle"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle");)

		return false;
	}
	dataModelTitle = gqlObject["dataModelTitle"].toString();

	if (!gqlObject.ContainsParam("dataModelVersion") || (gqlObject["dataModelVersion"].userType() != QMetaType::QString && gqlObject["dataModelVersion"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion");)

		return false;
	}
	dataModelVersion = gqlObject["dataModelVersion"].toString();

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataRead = resultMetaData->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent") && (!gqlObject["resultContent"].isNull())){
		const QVariant resultContentData = gqlObject["resultContent"];
		const QVariantList resultContentDataList = resultContentData.toList();
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = QList<std::shared_ptr<complextest::CTImpl::CDMResultUnionType>>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			QVariant tempResultContent = resultContentDataList[resultContentIndex];
			std::shared_ptr<complextest::CTImpl::CDMResultUnionType> resultContentDataValue;
			if (itemTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataValueConvert;
				const ::imtgql::CGqlParamObject* resultContentDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("resultContent");
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*resultContentDataObjectPtr);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = std::make_shared<CDMResultUnionType>(resultContentDataValueConvert);
			}
			else if (itemTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataValueConvert;
				const ::imtgql::CGqlParamObject* resultContentDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("resultContent");
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*resultContentDataObjectPtr);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = std::make_shared<CDMResultUnionType>(resultContentDataValueConvert);
			}
			else{
				return false;
			}
			resultContent->append(resultContentDataValue);
		}
	}

	if (gqlObject.ContainsParam("overallResultValues") && (gqlObject.GetObjectsCount("overallResultValues") > 0)){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = QList<COverallResultValues::V1_0>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr){
				qDebug() << "invalid type" << overallResultValuesDataObjectPtr;
				return false;
			}
			COverallResultValues::V1_0 tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResultVarRecursive::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("dataModelTitle") && (gqlObject["dataModelTitle"].userType() == QMetaType::QString || gqlObject["dataModelTitle"].userType() == QMetaType::QByteArray)){
		dataModelTitle = gqlObject["dataModelTitle"].toString();
	}

	if (gqlObject.ContainsParam("dataModelVersion") && (gqlObject["dataModelVersion"].userType() == QMetaType::QString || gqlObject["dataModelVersion"].userType() == QMetaType::QByteArray)){
		dataModelVersion = gqlObject["dataModelVersion"].toString();
	}

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent") && (!gqlObject["resultContent"].isNull())){
		const QVariant resultContentData = gqlObject["resultContent"];
		const QVariantList resultContentDataList = resultContentData.toList();
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = QList<std::shared_ptr<complextest::CTImpl::CDMResultUnionType>>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			QVariant tempResultContent = resultContentDataList[resultContentIndex];
			std::shared_ptr<complextest::CTImpl::CDMResultUnionType> resultContentDataValue;
			if (itemTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataValueConvert;
				const ::imtgql::CGqlParamObject* resultContentDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("resultContent");
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*resultContentDataObjectPtr);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = std::make_shared<CDMResultUnionType>(resultContentDataValueConvert);
			}
			else if (itemTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataValueConvert;
				const ::imtgql::CGqlParamObject* resultContentDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("resultContent");
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*resultContentDataObjectPtr);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = std::make_shared<CDMResultUnionType>(resultContentDataValueConvert);
			}
			else{
				return false;
			}
			resultContent->append(resultContentDataValue);
		}
	}

	if (gqlObject.ContainsParam("overallResultValues") && (gqlObject.GetObjectsCount("overallResultValues") > 0)){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = QList<COverallResultValues::V1_0>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr){
				qDebug() << "invalid type" << overallResultValuesDataObjectPtr;
				return false;
			}
			COverallResultValues::V1_0 tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResultVarRecursive::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!dataModelTitle){
		return false;
	}
	jsonObject["dataModelTitle"] = QJsonValue::fromVariant(*dataModelTitle);

	if (!dataModelVersion){
		return false;
	}
	jsonObject["dataModelVersion"] = QJsonValue::fromVariant(*dataModelVersion);

	if (resultMetaData){
		QJsonObject resultMetaDataJsonObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToJsonObject(resultMetaDataJsonObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
		jsonObject["resultMetaData"] = resultMetaDataJsonObject;
	}

	if (resultContent){
		QJsonArray newResultContentArray;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			if (const CCDMResultVarString* val = std::get_if<CCDMResultVarString>((resultContent->at(resultContentIndex)).get())){
				QJsonObject resultContentJsonObject;
				const bool isresultContentAdded = val->WriteToJsonObject(resultContentJsonObject);
				if (!isresultContentAdded){
					return false;
				}
				newResultContentArray << resultContentJsonObject;
			}
			else if (const CCDMResultVarRecursive* val = std::get_if<CCDMResultVarRecursive>((resultContent->at(resultContentIndex)).get())){
				QJsonObject resultContentJsonObject;
				const bool isresultContentAdded = val->WriteToJsonObject(resultContentJsonObject);
				if (!isresultContentAdded){
					return false;
				}
				newResultContentArray << resultContentJsonObject;
			}
		}
		jsonObject["resultContent"] = newResultContentArray;
	}

	if (overallResultValues){
		QJsonArray newOverallResultValuesArray;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			QJsonObject newOverallResultValuesJsonObject;
			if (!overallResultValues->at(overallResultValuesIndex).WriteToJsonObject(newOverallResultValuesJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			newOverallResultValuesArray << newOverallResultValuesJsonObject;
		}
		jsonObject["overallResultValues"] = newOverallResultValuesArray;
	}

	return true;
}


bool CCDMResultVarRecursive::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("dataModelTitle") || ! jsonObject["dataModelTitle"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle");)

		return false;
	}
	dataModelTitle = jsonObject["dataModelTitle"].toString();

	if (!jsonObject.contains("dataModelVersion") || ! jsonObject["dataModelVersion"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion");)

		return false;
	}
	dataModelVersion = jsonObject["dataModelVersion"].toString();

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataRead = resultMetaData->ReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	if (jsonObject.contains("resultContent") && jsonObject["resultContent"].isArray()){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = QList<std::shared_ptr<complextest::CTImpl::CDMResultUnionType>>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			QVariant tempResultContent = resultContentJsonArray[resultContentIndex].toVariant();
			std::shared_ptr<complextest::CTImpl::CDMResultUnionType> resultContentDataValue;
			if (itemTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = std::make_shared<CDMResultUnionType>(resultContentDataValueConvert);
			}
			else if (itemTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = std::make_shared<CDMResultUnionType>(resultContentDataValueConvert);
			}
			else{
				return false;
			}
			resultContent->append(resultContentDataValue);
		}
	}

	if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isArray()){
		const QJsonArray overallResultValuesJsonArray = jsonObject["overallResultValues"].toArray();
		const qsizetype overallResultValuesArrayCount = overallResultValuesJsonArray.size();
		overallResultValues = QList<COverallResultValues::V1_0>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			COverallResultValues::V1_0 tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResultVarRecursive::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("dataModelTitle") && jsonObject["dataModelTitle"].isString()){
		dataModelTitle = jsonObject["dataModelTitle"].toString();
	}

	if (jsonObject.contains("dataModelVersion") && jsonObject["dataModelVersion"].isString()){
		dataModelVersion = jsonObject["dataModelVersion"].toString();
	}

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	if (jsonObject.contains("resultContent") && jsonObject["resultContent"].isArray()){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = QList<std::shared_ptr<complextest::CTImpl::CDMResultUnionType>>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			QVariant tempResultContent = resultContentJsonArray[resultContentIndex].toVariant();
			std::shared_ptr<complextest::CTImpl::CDMResultUnionType> resultContentDataValue;
			if (itemTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = std::make_shared<CDMResultUnionType>(resultContentDataValueConvert);
			}
			else if (itemTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = std::make_shared<CDMResultUnionType>(resultContentDataValueConvert);
			}
			else{
				return false;
			}
			resultContent->append(resultContentDataValue);
		}
	}

	if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isArray()){
		const QJsonArray overallResultValuesJsonArray = jsonObject["overallResultValues"].toArray();
		const qsizetype overallResultValuesArrayCount = overallResultValuesJsonArray.size();
		overallResultValues = QList<COverallResultValues::V1_0>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			COverallResultValues::V1_0 tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


// serialize methods

bool CCDMResultVarRecursive::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResultVarRecursive::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResultVarRecursive::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResultVarRecursive::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResultVarRecursive::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResultVarRecursive::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResultVarRecursive::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResultVarRecursive::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResultVarRecursive::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


CCDMResultVarRecursiveObject::CCDMResultVarRecursiveObject(QObject* parent): ::imtbase::CItemModelBase(parent)			, m_resultMetaDataQObjectPtr(nullptr)
			, m_resultContentQObjectPtr(nullptr)
			, m_overallResultValuesQObjectPtr(nullptr)
{
	Version_1_0.emplace();

	QObject::connect(this, &CCDMResultVarRecursiveObject::dataModelTitleChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarRecursiveObject::dataModelVersionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarRecursiveObject::resultMetaDataChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarRecursiveObject::resultContentChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarRecursiveObject::overallResultValuesChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString CCDMResultVarRecursiveObject::GetDataModelTitle()
{
	if (Version_1_0->dataModelTitle.has_value()){
		return Version_1_0->dataModelTitle.value();
	}

	return QString();
}


void CCDMResultVarRecursiveObject::SetDataModelTitle(QString v)
{
	Version_1_0->dataModelTitle = v;
	dataModelTitleChanged();
}


bool CCDMResultVarRecursiveObject::hasDataModelTitle()
{
	 return Version_1_0->dataModelTitle.HasValue();
}


QString CCDMResultVarRecursiveObject::GetDataModelVersion()
{
	if (Version_1_0->dataModelVersion.has_value()){
		return Version_1_0->dataModelVersion.value();
	}

	return QString();
}


void CCDMResultVarRecursiveObject::SetDataModelVersion(QString v)
{
	Version_1_0->dataModelVersion = v;
	dataModelVersionChanged();
}


bool CCDMResultVarRecursiveObject::hasDataModelVersion()
{
	 return Version_1_0->dataModelVersion.HasValue();
}


sdl::complextest::CTImpl::CResultMetaDataObject* CCDMResultVarRecursiveObject::GetResultMetaData()
{
	if (Version_1_0->resultMetaData.has_value()){
		if (!m_resultMetaDataQObjectPtr){
			m_resultMetaDataQObjectPtr = dynamic_cast<sdl::complextest::CTImpl::CResultMetaDataObject*>(CreateObject("resultMetaData"));
			m_resultMetaDataQObjectPtr->Version_1_0 = Version_1_0->resultMetaData;
		}
		return m_resultMetaDataQObjectPtr;
	}

	return nullptr;
}


void CCDMResultVarRecursiveObject::SetResultMetaData(sdl::complextest::CTImpl::CResultMetaDataObject* v)
{
	if (v){
		Version_1_0->resultMetaData = v->Version_1_0;
		m_resultMetaDataQObjectPtr = v;
	}
	else {
		Version_1_0->resultMetaData = nullptr;
	}

	resultMetaDataChanged();
}


bool CCDMResultVarRecursiveObject::hasResultMetaData()
{
	 return Version_1_0->resultMetaData.HasValue();
}


void CCDMResultVarRecursiveObject::createResultMetaData()
{	Version_1_0->resultMetaData.emplace();

}


sdl::complextest::CTImpl::CCDMResultUnionTypeObjectList* CCDMResultVarRecursiveObject::GetResultContent()
{
	if (Version_1_0->resultContent.has_value()){
		if (!m_resultContentQObjectPtr){
			m_resultContentQObjectPtr = dynamic_cast<sdl::complextest::CTImpl::CCDMResultUnionTypeObjectList*>(CreateObject("resultContent"));
			m_resultContentQObjectPtr->Version_1_0 = Version_1_0->resultContent;
		}
		return m_resultContentQObjectPtr;
	}

	return nullptr;
}


void CCDMResultVarRecursiveObject::SetResultContent(sdl::complextest::CTImpl::CCDMResultUnionTypeObjectList* v)
{
	if (v){
		Version_1_0->resultContent = v->Version_1_0;
		m_resultContentQObjectPtr = v;
	}
	else {
		Version_1_0->resultContent = nullptr;
	}

	resultContentChanged();
}


bool CCDMResultVarRecursiveObject::hasResultContent()
{
	 return Version_1_0->resultContent.HasValue();
}


void CCDMResultVarRecursiveObject::createResultContent()
{	Version_1_0->resultContent.emplace();

}


sdl::complextest::CTImpl::COverallResultValuesObjectList* CCDMResultVarRecursiveObject::GetOverallResultValues()
{
	if (Version_1_0->overallResultValues.has_value()){
		if (!m_overallResultValuesQObjectPtr){
			m_overallResultValuesQObjectPtr = dynamic_cast<sdl::complextest::CTImpl::COverallResultValuesObjectList*>(CreateObject("overallResultValues"));
			m_overallResultValuesQObjectPtr->Version_1_0 = Version_1_0->overallResultValues;
		}
		return m_overallResultValuesQObjectPtr;
	}

	return nullptr;
}


void CCDMResultVarRecursiveObject::SetOverallResultValues(sdl::complextest::CTImpl::COverallResultValuesObjectList* v)
{
	if (v){
		Version_1_0->overallResultValues = v->Version_1_0;
		m_overallResultValuesQObjectPtr = v;
	}
	else {
		Version_1_0->overallResultValues = nullptr;
	}

	overallResultValuesChanged();
}


bool CCDMResultVarRecursiveObject::hasOverallResultValues()
{
	 return Version_1_0->overallResultValues.HasValue();
}


void CCDMResultVarRecursiveObject::createOverallResultValues()
{	Version_1_0->overallResultValues.emplace();

}


QString CCDMResultVarRecursiveObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CCDMResultVarRecursiveObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CCDMResultVarRecursiveObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CCDMResultVarRecursiveObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QObject* CCDMResultVarRecursiveObject::CreateObject(const QString& key)
{
	if (key == "resultMetaData"){
		return new sdl::complextest::CTImpl::CResultMetaDataObject(this);
	}
	if (key == "resultContent"){
		return new sdl::complextest::CTImpl::CCDMResultUnionTypeObjectList(this);
	}
	if (key == "overallResultValues"){
		return new sdl::complextest::CTImpl::COverallResultValuesObjectList(this);
	}
	return nullptr;
}


QString CCDMResultVarRecursiveObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "dataModelTitle")){
		return "dataModelTitle";
	}
	if (propertyName == (QString("m_") + "dataModelVersion")){
		return "dataModelVersion";
	}
	if (propertyName == (QString("m_") + "resultMetaData")){
		return "resultMetaData";
	}
	if (propertyName == (QString("m_") + "resultContent")){
		return "resultContent";
	}
	if (propertyName == (QString("m_") + "overallResultValues")){
		return "overallResultValues";
	}

	return propertyName;
}


} // namespace sdl::complextest::CTImpl




/// \file CCDMResultVarString.cpp

namespace sdl::complextest::CTImpl
{


QByteArray CCDMResultVarString::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CCDMResultVarString::V1_0::operator==(const V1_0& other) const
{
	return 
				dataModelTitle == other.dataModelTitle &&
				dataModelVersion == other.dataModelVersion &&
				resultMetaData == other.resultMetaData &&
				resultContent == other.resultContent &&
				overallResultValues == other.overallResultValues;
}


bool CCDMResultVarString::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (!dataModelTitle){
		return false;
	}
	model.SetData("dataModelTitle", *dataModelTitle, modelIndex);

	if (!dataModelVersion){
		return false;
	}
	model.SetData("dataModelVersion", *dataModelVersion, modelIndex);


	if (resultMetaData){
		::imtbase::CTreeItemModel* resultMetaDataNewModelPtr = model.AddTreeModel("resultMetaData", modelIndex);
		const bool isResultMetaDataAdded = resultMetaData->WriteToModel(*resultMetaDataNewModelPtr, 0);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}

	}
	if (resultContent){
		::imtbase::CTreeItemModel* newResultContentModelPtr = model.AddTreeModel("resultContent", modelIndex);
		newResultContentModelPtr->setIsArray(true);
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			newResultContentModelPtr->InsertNewItem();
			newResultContentModelPtr->SetData(QByteArray(), resultContent->at(resultContentIndex), resultContentIndex);
		}
	}

	if (overallResultValues){
		::imtbase::CTreeItemModel* newOverallResultValuesModelPtr = model.AddTreeModel("overallResultValues", modelIndex);
		newOverallResultValuesModelPtr->setIsArray(true);
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			newOverallResultValuesModelPtr->InsertNewItem();
			if (!(overallResultValues->at(overallResultValuesIndex).WriteToModel(*newOverallResultValuesModelPtr, overallResultValuesIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
		}
	}

	return true;
}


bool CCDMResultVarString::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant dataModelTitleData = model.GetData("dataModelTitle", modelIndex);
	if (dataModelTitleData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle");)

		return false;
	}
	dataModelTitle = dataModelTitleData.toString();

	QVariant dataModelVersionData = model.GetData("dataModelVersion", modelIndex);
	if (dataModelVersionData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion");)

		return false;
	}
	dataModelVersion = dataModelVersionData.toString();

	::imtbase::CTreeItemModel* resultMetaDataDataModelPtr = model.GetTreeItemModel("resultMetaData", modelIndex);
	if (resultMetaDataDataModelPtr != nullptr){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataReaded = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataReaded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		QList<QString> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			QString resultContent = resultContentModel->GetData(QByteArray(), resultContentIndex).toString();
			resultContentList << resultContent;
		}
		resultContent = resultContentList;

	}

	::imtbase::CTreeItemModel* overallResultValuesModel = model.GetTreeItemModel("overallResultValues", modelIndex);
	if (overallResultValuesModel != nullptr){
		int overallResultValuesCount = overallResultValuesModel->GetItemsCount();
		QList<COverallResultValues::V1_0> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			COverallResultValues::V1_0 overallResultValues;
			if (!overallResultValues.ReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValuesList << overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool CCDMResultVarString::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant dataModelTitleData = model.GetData("dataModelTitle", modelIndex);
	if (!dataModelTitleData.isNull()){
		dataModelTitle = dataModelTitleData.toString();
	}

	QVariant dataModelVersionData = model.GetData("dataModelVersion", modelIndex);
	if (!dataModelVersionData.isNull()){
		dataModelVersion = dataModelVersionData.toString();
	}

	::imtbase::CTreeItemModel* resultMetaDataDataModelPtr = model.GetTreeItemModel("resultMetaData", modelIndex);
	if (resultMetaDataDataModelPtr != nullptr){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataReaded = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataReaded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		QList<QString> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			QString resultContent = resultContentModel->GetData(QByteArray(), resultContentIndex).toString();
			resultContentList << resultContent;
		}
		resultContent = resultContentList;

	}

	::imtbase::CTreeItemModel* overallResultValuesModel = model.GetTreeItemModel("overallResultValues", modelIndex);
	if (overallResultValuesModel != nullptr){
		int overallResultValuesCount = overallResultValuesModel->GetItemsCount();
		QList<COverallResultValues::V1_0> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			COverallResultValues::V1_0 overallResultValues;
			if (!overallResultValues.OptReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValuesList << overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool CCDMResultVarString::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!dataModelTitle){
		return false;
	}
	gqlObject.InsertParam("dataModelTitle", QVariant(*dataModelTitle));

	if (!dataModelVersion){
		return false;
	}
	gqlObject.InsertParam("dataModelVersion", QVariant(*dataModelVersion));

	if (resultMetaData){
		::imtgql::CGqlParamObject resultMetaDataGqlObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToGraphQlObject(resultMetaDataGqlObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
		gqlObject.InsertParam("resultMetaData", resultMetaDataGqlObject);
	}

	if (resultContent){
		QVariantList resultContentDataObjectList;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			resultContentDataObjectList << resultContent->at(resultContentIndex);
		}
		gqlObject.InsertParam("resultContent", resultContentDataObjectList);
	}

	if (overallResultValues){
		QList<::imtgql::CGqlParamObject> overallResultValuesDataObjectList;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			::imtgql::CGqlParamObject newOverallResultValuesGqlObject;
			if (!overallResultValues->at(overallResultValuesIndex).WriteToGraphQlObject(newOverallResultValuesGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValuesDataObjectList << newOverallResultValuesGqlObject;
		}
		gqlObject.InsertParam("overallResultValues", overallResultValuesDataObjectList);
	}

	return true;
}


bool CCDMResultVarString::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("dataModelTitle") || (gqlObject["dataModelTitle"].userType() != QMetaType::QString && gqlObject["dataModelTitle"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle");)

		return false;
	}
	dataModelTitle = gqlObject["dataModelTitle"].toString();

	if (!gqlObject.ContainsParam("dataModelVersion") || (gqlObject["dataModelVersion"].userType() != QMetaType::QString && gqlObject["dataModelVersion"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion");)

		return false;
	}
	dataModelVersion = gqlObject["dataModelVersion"].toString();

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataRead = resultMetaData->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent") && (!gqlObject["resultContent"].isNull())){
		const QVariant resultContentData = gqlObject["resultContent"];
		const QVariantList resultContentDataList = resultContentData.toList();
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = QList<QString>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			QString tempResultContent = resultContentDataList[resultContentIndex].toString();
			resultContent->append(tempResultContent);
		}
	}

	if (gqlObject.ContainsParam("overallResultValues") && (gqlObject.GetObjectsCount("overallResultValues") > 0)){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = QList<COverallResultValues::V1_0>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr){
				qDebug() << "invalid type" << overallResultValuesDataObjectPtr;
				return false;
			}
			COverallResultValues::V1_0 tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResultVarString::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("dataModelTitle") && (gqlObject["dataModelTitle"].userType() == QMetaType::QString || gqlObject["dataModelTitle"].userType() == QMetaType::QByteArray)){
		dataModelTitle = gqlObject["dataModelTitle"].toString();
	}

	if (gqlObject.ContainsParam("dataModelVersion") && (gqlObject["dataModelVersion"].userType() == QMetaType::QString || gqlObject["dataModelVersion"].userType() == QMetaType::QByteArray)){
		dataModelVersion = gqlObject["dataModelVersion"].toString();
	}

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent") && (!gqlObject["resultContent"].isNull())){
		const QVariant resultContentData = gqlObject["resultContent"];
		const QVariantList resultContentDataList = resultContentData.toList();
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = QList<QString>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			QString tempResultContent = resultContentDataList[resultContentIndex].toString();
			resultContent->append(tempResultContent);
		}
	}

	if (gqlObject.ContainsParam("overallResultValues") && (gqlObject.GetObjectsCount("overallResultValues") > 0)){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = QList<COverallResultValues::V1_0>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr){
				qDebug() << "invalid type" << overallResultValuesDataObjectPtr;
				return false;
			}
			COverallResultValues::V1_0 tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResultVarString::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!dataModelTitle){
		return false;
	}
	jsonObject["dataModelTitle"] = QJsonValue::fromVariant(*dataModelTitle);

	if (!dataModelVersion){
		return false;
	}
	jsonObject["dataModelVersion"] = QJsonValue::fromVariant(*dataModelVersion);

	if (resultMetaData){
		QJsonObject resultMetaDataJsonObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToJsonObject(resultMetaDataJsonObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
		jsonObject["resultMetaData"] = resultMetaDataJsonObject;
	}

	if (resultContent){
		QJsonArray newResultContentArray;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			newResultContentArray << resultContent->at(resultContentIndex);
		}
		jsonObject["resultContent"] = newResultContentArray;
	}

	if (overallResultValues){
		QJsonArray newOverallResultValuesArray;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			QJsonObject newOverallResultValuesJsonObject;
			if (!overallResultValues->at(overallResultValuesIndex).WriteToJsonObject(newOverallResultValuesJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			newOverallResultValuesArray << newOverallResultValuesJsonObject;
		}
		jsonObject["overallResultValues"] = newOverallResultValuesArray;
	}

	return true;
}


bool CCDMResultVarString::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("dataModelTitle") || ! jsonObject["dataModelTitle"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle");)

		return false;
	}
	dataModelTitle = jsonObject["dataModelTitle"].toString();

	if (!jsonObject.contains("dataModelVersion") || ! jsonObject["dataModelVersion"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion");)

		return false;
	}
	dataModelVersion = jsonObject["dataModelVersion"].toString();

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataRead = resultMetaData->ReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	if (jsonObject.contains("resultContent") && jsonObject["resultContent"].isArray()){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = QList<QString>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			QString tempResultContent = resultContentJsonArray[resultContentIndex].toString();
			resultContent->append(tempResultContent);
		}
	}

	if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isArray()){
		const QJsonArray overallResultValuesJsonArray = jsonObject["overallResultValues"].toArray();
		const qsizetype overallResultValuesArrayCount = overallResultValuesJsonArray.size();
		overallResultValues = QList<COverallResultValues::V1_0>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			COverallResultValues::V1_0 tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResultVarString::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("dataModelTitle") && jsonObject["dataModelTitle"].isString()){
		dataModelTitle = jsonObject["dataModelTitle"].toString();
	}

	if (jsonObject.contains("dataModelVersion") && jsonObject["dataModelVersion"].isString()){
		dataModelVersion = jsonObject["dataModelVersion"].toString();
	}

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	if (jsonObject.contains("resultContent") && jsonObject["resultContent"].isArray()){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = QList<QString>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			QString tempResultContent = resultContentJsonArray[resultContentIndex].toString();
			resultContent->append(tempResultContent);
		}
	}

	if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isArray()){
		const QJsonArray overallResultValuesJsonArray = jsonObject["overallResultValues"].toArray();
		const qsizetype overallResultValuesArrayCount = overallResultValuesJsonArray.size();
		overallResultValues = QList<COverallResultValues::V1_0>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			COverallResultValues::V1_0 tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


// serialize methods

bool CCDMResultVarString::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResultVarString::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResultVarString::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResultVarString::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResultVarString::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResultVarString::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResultVarString::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResultVarString::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResultVarString::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


CCDMResultVarStringObject::CCDMResultVarStringObject(QObject* parent): ::imtbase::CItemModelBase(parent)			, m_resultMetaDataQObjectPtr(nullptr)
			, m_overallResultValuesQObjectPtr(nullptr)
{
	Version_1_0.emplace();

	QObject::connect(this, &CCDMResultVarStringObject::dataModelTitleChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarStringObject::dataModelVersionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarStringObject::resultMetaDataChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarStringObject::resultContentChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarStringObject::overallResultValuesChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString CCDMResultVarStringObject::GetDataModelTitle()
{
	if (Version_1_0->dataModelTitle.has_value()){
		return Version_1_0->dataModelTitle.value();
	}

	return QString();
}


void CCDMResultVarStringObject::SetDataModelTitle(QString v)
{
	Version_1_0->dataModelTitle = v;
	dataModelTitleChanged();
}


bool CCDMResultVarStringObject::hasDataModelTitle()
{
	 return Version_1_0->dataModelTitle.HasValue();
}


QString CCDMResultVarStringObject::GetDataModelVersion()
{
	if (Version_1_0->dataModelVersion.has_value()){
		return Version_1_0->dataModelVersion.value();
	}

	return QString();
}


void CCDMResultVarStringObject::SetDataModelVersion(QString v)
{
	Version_1_0->dataModelVersion = v;
	dataModelVersionChanged();
}


bool CCDMResultVarStringObject::hasDataModelVersion()
{
	 return Version_1_0->dataModelVersion.HasValue();
}


sdl::complextest::CTImpl::CResultMetaDataObject* CCDMResultVarStringObject::GetResultMetaData()
{
	if (Version_1_0->resultMetaData.has_value()){
		if (!m_resultMetaDataQObjectPtr){
			m_resultMetaDataQObjectPtr = dynamic_cast<sdl::complextest::CTImpl::CResultMetaDataObject*>(CreateObject("resultMetaData"));
			m_resultMetaDataQObjectPtr->Version_1_0 = Version_1_0->resultMetaData;
		}
		return m_resultMetaDataQObjectPtr;
	}

	return nullptr;
}


void CCDMResultVarStringObject::SetResultMetaData(sdl::complextest::CTImpl::CResultMetaDataObject* v)
{
	if (v){
		Version_1_0->resultMetaData = v->Version_1_0;
		m_resultMetaDataQObjectPtr = v;
	}
	else {
		Version_1_0->resultMetaData = nullptr;
	}

	resultMetaDataChanged();
}


bool CCDMResultVarStringObject::hasResultMetaData()
{
	 return Version_1_0->resultMetaData.HasValue();
}


void CCDMResultVarStringObject::createResultMetaData()
{	Version_1_0->resultMetaData.emplace();

}


QList<QString> CCDMResultVarStringObject::GetResultContent()
{
	if (Version_1_0->resultContent.has_value()){
		return Version_1_0->resultContent.value();
	}

	return QStringList();
}


void CCDMResultVarStringObject::SetResultContent(QList<QString> v)
{
	Version_1_0->resultContent = v;
	resultContentChanged();
}


bool CCDMResultVarStringObject::hasResultContent()
{
	 return Version_1_0->resultContent.HasValue();
}


sdl::complextest::CTImpl::COverallResultValuesObjectList* CCDMResultVarStringObject::GetOverallResultValues()
{
	if (Version_1_0->overallResultValues.has_value()){
		if (!m_overallResultValuesQObjectPtr){
			m_overallResultValuesQObjectPtr = dynamic_cast<sdl::complextest::CTImpl::COverallResultValuesObjectList*>(CreateObject("overallResultValues"));
			m_overallResultValuesQObjectPtr->Version_1_0 = Version_1_0->overallResultValues;
		}
		return m_overallResultValuesQObjectPtr;
	}

	return nullptr;
}


void CCDMResultVarStringObject::SetOverallResultValues(sdl::complextest::CTImpl::COverallResultValuesObjectList* v)
{
	if (v){
		Version_1_0->overallResultValues = v->Version_1_0;
		m_overallResultValuesQObjectPtr = v;
	}
	else {
		Version_1_0->overallResultValues = nullptr;
	}

	overallResultValuesChanged();
}


bool CCDMResultVarStringObject::hasOverallResultValues()
{
	 return Version_1_0->overallResultValues.HasValue();
}


void CCDMResultVarStringObject::createOverallResultValues()
{	Version_1_0->overallResultValues.emplace();

}


QString CCDMResultVarStringObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CCDMResultVarStringObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CCDMResultVarStringObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CCDMResultVarStringObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QObject* CCDMResultVarStringObject::CreateObject(const QString& key)
{
	if (key == "resultMetaData"){
		return new sdl::complextest::CTImpl::CResultMetaDataObject(this);
	}
	if (key == "overallResultValues"){
		return new sdl::complextest::CTImpl::COverallResultValuesObjectList(this);
	}
	return nullptr;
}


QString CCDMResultVarStringObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "dataModelTitle")){
		return "dataModelTitle";
	}
	if (propertyName == (QString("m_") + "dataModelVersion")){
		return "dataModelVersion";
	}
	if (propertyName == (QString("m_") + "resultMetaData")){
		return "resultMetaData";
	}
	if (propertyName == (QString("m_") + "resultContent")){
		return "resultContent";
	}
	if (propertyName == (QString("m_") + "overallResultValues")){
		return "overallResultValues";
	}

	return propertyName;
}


} // namespace sdl::complextest::CTImpl




/// \file CCDMResult.cpp

namespace sdl::complextest::CTImpl
{


QByteArray CCDMResult::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CCDMResult::V1_0::operator==(const V1_0& other) const
{
	return 
				dataModelTitle == other.dataModelTitle &&
				dataModelVersion == other.dataModelVersion &&
				resultMetaData == other.resultMetaData &&
				resultContent == other.resultContent &&
				overallResultValues == other.overallResultValues;
}


bool CCDMResult::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (!dataModelTitle){
		return false;
	}
	model.SetData("dataModelTitle", *dataModelTitle, modelIndex);

	if (!dataModelVersion){
		return false;
	}
	model.SetData("dataModelVersion", *dataModelVersion, modelIndex);


	if (resultMetaData){
		::imtbase::CTreeItemModel* resultMetaDataNewModelPtr = model.AddTreeModel("resultMetaData", modelIndex);
		const bool isResultMetaDataAdded = resultMetaData->WriteToModel(*resultMetaDataNewModelPtr, 0);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}

	}
	if (resultContent){
		::imtbase::CTreeItemModel* newResultContentModelPtr = model.AddTreeModel("resultContent", modelIndex);
		newResultContentModelPtr->setIsArray(true);
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			QVariant resultContentVariantValue;
			if (const CCDMResultVarString* val = std::get_if<CCDMResultVarString>((resultContent->at(resultContentIndex)).get())){
				newResultContentModelPtr->InsertNewItem();
				if(!val->WriteToModel(*newResultContentModelPtr, resultContentIndex)){
					return false;
				}
			}
			else if (const CCDMResultVarRecursive* val = std::get_if<CCDMResultVarRecursive>((resultContent->at(resultContentIndex)).get())){
				newResultContentModelPtr->InsertNewItem();
				if(!val->WriteToModel(*newResultContentModelPtr, resultContentIndex)){
					return false;
				}
			}
		}
	}

	if (overallResultValues){
		::imtbase::CTreeItemModel* newOverallResultValuesModelPtr = model.AddTreeModel("overallResultValues", modelIndex);
		newOverallResultValuesModelPtr->setIsArray(true);
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			newOverallResultValuesModelPtr->InsertNewItem();
			if (!(overallResultValues->at(overallResultValuesIndex).WriteToModel(*newOverallResultValuesModelPtr, overallResultValuesIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
		}
	}

	return true;
}


bool CCDMResult::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant dataModelTitleData = model.GetData("dataModelTitle", modelIndex);
	if (dataModelTitleData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle");)

		return false;
	}
	dataModelTitle = dataModelTitleData.toString();

	QVariant dataModelVersionData = model.GetData("dataModelVersion", modelIndex);
	if (dataModelVersionData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion");)

		return false;
	}
	dataModelVersion = dataModelVersionData.toString();

	::imtbase::CTreeItemModel* resultMetaDataDataModelPtr = model.GetTreeItemModel("resultMetaData", modelIndex);
	if (resultMetaDataDataModelPtr != nullptr){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataReaded = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataReaded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		QList<std::shared_ptr<CDMResultUnionType>> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			std::shared_ptr<CDMResultUnionType> resultContentData;
			QVariant resultContentVariantValue = resultContentModel->GetData(QByteArray(), resultContentIndex);
			if (itemTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = std::make_shared<CDMResultUnionType>(resultContentDataConvert);
			}
			else if (itemTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = std::make_shared<CDMResultUnionType>(resultContentDataConvert);
			}
			else{
				return false;
			}

			resultContentList << resultContentData;
		}
		resultContent = resultContentList;

	}

	::imtbase::CTreeItemModel* overallResultValuesModel = model.GetTreeItemModel("overallResultValues", modelIndex);
	if (overallResultValuesModel != nullptr){
		int overallResultValuesCount = overallResultValuesModel->GetItemsCount();
		QList<COverallResultValues::V1_0> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			COverallResultValues::V1_0 overallResultValues;
			if (!overallResultValues.ReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValuesList << overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool CCDMResult::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant dataModelTitleData = model.GetData("dataModelTitle", modelIndex);
	if (!dataModelTitleData.isNull()){
		dataModelTitle = dataModelTitleData.toString();
	}

	QVariant dataModelVersionData = model.GetData("dataModelVersion", modelIndex);
	if (!dataModelVersionData.isNull()){
		dataModelVersion = dataModelVersionData.toString();
	}

	::imtbase::CTreeItemModel* resultMetaDataDataModelPtr = model.GetTreeItemModel("resultMetaData", modelIndex);
	if (resultMetaDataDataModelPtr != nullptr){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataReaded = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataReaded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		QList<std::shared_ptr<CDMResultUnionType>> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			std::shared_ptr<CDMResultUnionType> resultContentData;
			QVariant resultContentVariantValue = resultContentModel->GetData(QByteArray(), resultContentIndex);
			if (itemTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = std::make_shared<CDMResultUnionType>(resultContentDataConvert);
			}
			else if (itemTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = std::make_shared<CDMResultUnionType>(resultContentDataConvert);
			}
			else{
				return false;
			}

			resultContentList << resultContentData;
		}
		resultContent = resultContentList;

	}

	::imtbase::CTreeItemModel* overallResultValuesModel = model.GetTreeItemModel("overallResultValues", modelIndex);
	if (overallResultValuesModel != nullptr){
		int overallResultValuesCount = overallResultValuesModel->GetItemsCount();
		QList<COverallResultValues::V1_0> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			COverallResultValues::V1_0 overallResultValues;
			if (!overallResultValues.OptReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValuesList << overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool CCDMResult::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!dataModelTitle){
		return false;
	}
	gqlObject.InsertParam("dataModelTitle", QVariant(*dataModelTitle));

	if (!dataModelVersion){
		return false;
	}
	gqlObject.InsertParam("dataModelVersion", QVariant(*dataModelVersion));

	if (resultMetaData){
		::imtgql::CGqlParamObject resultMetaDataGqlObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToGraphQlObject(resultMetaDataGqlObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
		gqlObject.InsertParam("resultMetaData", resultMetaDataGqlObject);
	}

	if (resultContent){
		QList<::imtgql::CGqlParamObject> resultContentDataObjectList;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			::imtgql::CGqlParamObject resultContentDataObject;
			if (const CCDMResultVarString* val = std::get_if<CCDMResultVarString>((resultContent->at(resultContentIndex)).get())){
				if (!val->WriteToGraphQlObject(resultContentDataObject)){
					return false;
				}
			}
			else if (const CCDMResultVarRecursive* val = std::get_if<CCDMResultVarRecursive>((resultContent->at(resultContentIndex)).get())){
				if (!val->WriteToGraphQlObject(resultContentDataObject)){
					return false;
				}
			}
			resultContentDataObjectList << resultContentDataObject;
		}
		gqlObject.InsertParam("resultContent", resultContentDataObjectList);
	}

	if (overallResultValues){
		QList<::imtgql::CGqlParamObject> overallResultValuesDataObjectList;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			::imtgql::CGqlParamObject newOverallResultValuesGqlObject;
			if (!overallResultValues->at(overallResultValuesIndex).WriteToGraphQlObject(newOverallResultValuesGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValuesDataObjectList << newOverallResultValuesGqlObject;
		}
		gqlObject.InsertParam("overallResultValues", overallResultValuesDataObjectList);
	}

	return true;
}


bool CCDMResult::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("dataModelTitle") || (gqlObject["dataModelTitle"].userType() != QMetaType::QString && gqlObject["dataModelTitle"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle");)

		return false;
	}
	dataModelTitle = gqlObject["dataModelTitle"].toString();

	if (!gqlObject.ContainsParam("dataModelVersion") || (gqlObject["dataModelVersion"].userType() != QMetaType::QString && gqlObject["dataModelVersion"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion");)

		return false;
	}
	dataModelVersion = gqlObject["dataModelVersion"].toString();

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataRead = resultMetaData->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent") && (!gqlObject["resultContent"].isNull())){
		const QVariant resultContentData = gqlObject["resultContent"];
		const QVariantList resultContentDataList = resultContentData.toList();
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = QList<std::shared_ptr<complextest::CTImpl::CDMResultUnionType>>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			QVariant tempResultContent = resultContentDataList[resultContentIndex];
			std::shared_ptr<complextest::CTImpl::CDMResultUnionType> resultContentDataValue;
			if (itemTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataValueConvert;
				const ::imtgql::CGqlParamObject* resultContentDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("resultContent");
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*resultContentDataObjectPtr);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = std::make_shared<CDMResultUnionType>(resultContentDataValueConvert);
			}
			else if (itemTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataValueConvert;
				const ::imtgql::CGqlParamObject* resultContentDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("resultContent");
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*resultContentDataObjectPtr);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = std::make_shared<CDMResultUnionType>(resultContentDataValueConvert);
			}
			else{
				return false;
			}
			resultContent->append(resultContentDataValue);
		}
	}

	if (gqlObject.ContainsParam("overallResultValues") && (gqlObject.GetObjectsCount("overallResultValues") > 0)){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = QList<COverallResultValues::V1_0>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr){
				qDebug() << "invalid type" << overallResultValuesDataObjectPtr;
				return false;
			}
			COverallResultValues::V1_0 tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResult::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("dataModelTitle") && (gqlObject["dataModelTitle"].userType() == QMetaType::QString || gqlObject["dataModelTitle"].userType() == QMetaType::QByteArray)){
		dataModelTitle = gqlObject["dataModelTitle"].toString();
	}

	if (gqlObject.ContainsParam("dataModelVersion") && (gqlObject["dataModelVersion"].userType() == QMetaType::QString || gqlObject["dataModelVersion"].userType() == QMetaType::QByteArray)){
		dataModelVersion = gqlObject["dataModelVersion"].toString();
	}

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent") && (!gqlObject["resultContent"].isNull())){
		const QVariant resultContentData = gqlObject["resultContent"];
		const QVariantList resultContentDataList = resultContentData.toList();
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = QList<std::shared_ptr<complextest::CTImpl::CDMResultUnionType>>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			QVariant tempResultContent = resultContentDataList[resultContentIndex];
			std::shared_ptr<complextest::CTImpl::CDMResultUnionType> resultContentDataValue;
			if (itemTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataValueConvert;
				const ::imtgql::CGqlParamObject* resultContentDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("resultContent");
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*resultContentDataObjectPtr);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = std::make_shared<CDMResultUnionType>(resultContentDataValueConvert);
			}
			else if (itemTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataValueConvert;
				const ::imtgql::CGqlParamObject* resultContentDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("resultContent");
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*resultContentDataObjectPtr);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = std::make_shared<CDMResultUnionType>(resultContentDataValueConvert);
			}
			else{
				return false;
			}
			resultContent->append(resultContentDataValue);
		}
	}

	if (gqlObject.ContainsParam("overallResultValues") && (gqlObject.GetObjectsCount("overallResultValues") > 0)){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = QList<COverallResultValues::V1_0>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr){
				qDebug() << "invalid type" << overallResultValuesDataObjectPtr;
				return false;
			}
			COverallResultValues::V1_0 tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResult::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!dataModelTitle){
		return false;
	}
	jsonObject["dataModelTitle"] = QJsonValue::fromVariant(*dataModelTitle);

	if (!dataModelVersion){
		return false;
	}
	jsonObject["dataModelVersion"] = QJsonValue::fromVariant(*dataModelVersion);

	if (resultMetaData){
		QJsonObject resultMetaDataJsonObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToJsonObject(resultMetaDataJsonObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
		jsonObject["resultMetaData"] = resultMetaDataJsonObject;
	}

	if (resultContent){
		QJsonArray newResultContentArray;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			if (const CCDMResultVarString* val = std::get_if<CCDMResultVarString>((resultContent->at(resultContentIndex)).get())){
				QJsonObject resultContentJsonObject;
				const bool isresultContentAdded = val->WriteToJsonObject(resultContentJsonObject);
				if (!isresultContentAdded){
					return false;
				}
				newResultContentArray << resultContentJsonObject;
			}
			else if (const CCDMResultVarRecursive* val = std::get_if<CCDMResultVarRecursive>((resultContent->at(resultContentIndex)).get())){
				QJsonObject resultContentJsonObject;
				const bool isresultContentAdded = val->WriteToJsonObject(resultContentJsonObject);
				if (!isresultContentAdded){
					return false;
				}
				newResultContentArray << resultContentJsonObject;
			}
		}
		jsonObject["resultContent"] = newResultContentArray;
	}

	if (overallResultValues){
		QJsonArray newOverallResultValuesArray;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			QJsonObject newOverallResultValuesJsonObject;
			if (!overallResultValues->at(overallResultValuesIndex).WriteToJsonObject(newOverallResultValuesJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			newOverallResultValuesArray << newOverallResultValuesJsonObject;
		}
		jsonObject["overallResultValues"] = newOverallResultValuesArray;
	}

	return true;
}


bool CCDMResult::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("dataModelTitle") || ! jsonObject["dataModelTitle"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle");)

		return false;
	}
	dataModelTitle = jsonObject["dataModelTitle"].toString();

	if (!jsonObject.contains("dataModelVersion") || ! jsonObject["dataModelVersion"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion");)

		return false;
	}
	dataModelVersion = jsonObject["dataModelVersion"].toString();

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataRead = resultMetaData->ReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	if (jsonObject.contains("resultContent") && jsonObject["resultContent"].isArray()){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = QList<std::shared_ptr<complextest::CTImpl::CDMResultUnionType>>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			QVariant tempResultContent = resultContentJsonArray[resultContentIndex].toVariant();
			std::shared_ptr<complextest::CTImpl::CDMResultUnionType> resultContentDataValue;
			if (itemTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = std::make_shared<CDMResultUnionType>(resultContentDataValueConvert);
			}
			else if (itemTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = std::make_shared<CDMResultUnionType>(resultContentDataValueConvert);
			}
			else{
				return false;
			}
			resultContent->append(resultContentDataValue);
		}
	}

	if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isArray()){
		const QJsonArray overallResultValuesJsonArray = jsonObject["overallResultValues"].toArray();
		const qsizetype overallResultValuesArrayCount = overallResultValuesJsonArray.size();
		overallResultValues = QList<COverallResultValues::V1_0>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			COverallResultValues::V1_0 tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResult::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("dataModelTitle") && jsonObject["dataModelTitle"].isString()){
		dataModelTitle = jsonObject["dataModelTitle"].toString();
	}

	if (jsonObject.contains("dataModelVersion") && jsonObject["dataModelVersion"].isString()){
		dataModelVersion = jsonObject["dataModelVersion"].toString();
	}

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = CResultMetaData::V1_0();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData");)

			return false;
		}
	}

	if (jsonObject.contains("resultContent") && jsonObject["resultContent"].isArray()){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = QList<std::shared_ptr<complextest::CTImpl::CDMResultUnionType>>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			QVariant tempResultContent = resultContentJsonArray[resultContentIndex].toVariant();
			std::shared_ptr<complextest::CTImpl::CDMResultUnionType> resultContentDataValue;
			if (itemTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = std::make_shared<CDMResultUnionType>(resultContentDataValueConvert);
			}
			else if (itemTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = std::make_shared<CDMResultUnionType>(resultContentDataValueConvert);
			}
			else{
				return false;
			}
			resultContent->append(resultContentDataValue);
		}
	}

	if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isArray()){
		const QJsonArray overallResultValuesJsonArray = jsonObject["overallResultValues"].toArray();
		const qsizetype overallResultValuesArrayCount = overallResultValuesJsonArray.size();
		overallResultValues = QList<COverallResultValues::V1_0>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			COverallResultValues::V1_0 tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues");)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


// serialize methods

bool CCDMResult::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResult::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResult::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResult::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResult::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResult::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResult::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResult::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CCDMResult::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


CCDMResultObject::CCDMResultObject(QObject* parent): ::imtbase::CItemModelBase(parent)			, m_resultMetaDataQObjectPtr(nullptr)
			, m_resultContentQObjectPtr(nullptr)
			, m_overallResultValuesQObjectPtr(nullptr)
{
	Version_1_0.emplace();

	QObject::connect(this, &CCDMResultObject::dataModelTitleChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultObject::dataModelVersionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultObject::resultMetaDataChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultObject::resultContentChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultObject::overallResultValuesChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QString CCDMResultObject::GetDataModelTitle()
{
	if (Version_1_0->dataModelTitle.has_value()){
		return Version_1_0->dataModelTitle.value();
	}

	return QString();
}


void CCDMResultObject::SetDataModelTitle(QString v)
{
	Version_1_0->dataModelTitle = v;
	dataModelTitleChanged();
}


bool CCDMResultObject::hasDataModelTitle()
{
	 return Version_1_0->dataModelTitle.HasValue();
}


QString CCDMResultObject::GetDataModelVersion()
{
	if (Version_1_0->dataModelVersion.has_value()){
		return Version_1_0->dataModelVersion.value();
	}

	return QString();
}


void CCDMResultObject::SetDataModelVersion(QString v)
{
	Version_1_0->dataModelVersion = v;
	dataModelVersionChanged();
}


bool CCDMResultObject::hasDataModelVersion()
{
	 return Version_1_0->dataModelVersion.HasValue();
}


sdl::complextest::CTImpl::CResultMetaDataObject* CCDMResultObject::GetResultMetaData()
{
	if (Version_1_0->resultMetaData.has_value()){
		if (!m_resultMetaDataQObjectPtr){
			m_resultMetaDataQObjectPtr = dynamic_cast<sdl::complextest::CTImpl::CResultMetaDataObject*>(CreateObject("resultMetaData"));
			m_resultMetaDataQObjectPtr->Version_1_0 = Version_1_0->resultMetaData;
		}
		return m_resultMetaDataQObjectPtr;
	}

	return nullptr;
}


void CCDMResultObject::SetResultMetaData(sdl::complextest::CTImpl::CResultMetaDataObject* v)
{
	if (v){
		Version_1_0->resultMetaData = v->Version_1_0;
		m_resultMetaDataQObjectPtr = v;
	}
	else {
		Version_1_0->resultMetaData = nullptr;
	}

	resultMetaDataChanged();
}


bool CCDMResultObject::hasResultMetaData()
{
	 return Version_1_0->resultMetaData.HasValue();
}


void CCDMResultObject::createResultMetaData()
{	Version_1_0->resultMetaData.emplace();

}


sdl::complextest::CTImpl::CCDMResultUnionTypeObjectList* CCDMResultObject::GetResultContent()
{
	if (Version_1_0->resultContent.has_value()){
		if (!m_resultContentQObjectPtr){
			m_resultContentQObjectPtr = dynamic_cast<sdl::complextest::CTImpl::CCDMResultUnionTypeObjectList*>(CreateObject("resultContent"));
			m_resultContentQObjectPtr->Version_1_0 = Version_1_0->resultContent;
		}
		return m_resultContentQObjectPtr;
	}

	return nullptr;
}


void CCDMResultObject::SetResultContent(sdl::complextest::CTImpl::CCDMResultUnionTypeObjectList* v)
{
	if (v){
		Version_1_0->resultContent = v->Version_1_0;
		m_resultContentQObjectPtr = v;
	}
	else {
		Version_1_0->resultContent = nullptr;
	}

	resultContentChanged();
}


bool CCDMResultObject::hasResultContent()
{
	 return Version_1_0->resultContent.HasValue();
}


void CCDMResultObject::createResultContent()
{	Version_1_0->resultContent.emplace();

}


sdl::complextest::CTImpl::COverallResultValuesObjectList* CCDMResultObject::GetOverallResultValues()
{
	if (Version_1_0->overallResultValues.has_value()){
		if (!m_overallResultValuesQObjectPtr){
			m_overallResultValuesQObjectPtr = dynamic_cast<sdl::complextest::CTImpl::COverallResultValuesObjectList*>(CreateObject("overallResultValues"));
			m_overallResultValuesQObjectPtr->Version_1_0 = Version_1_0->overallResultValues;
		}
		return m_overallResultValuesQObjectPtr;
	}

	return nullptr;
}


void CCDMResultObject::SetOverallResultValues(sdl::complextest::CTImpl::COverallResultValuesObjectList* v)
{
	if (v){
		Version_1_0->overallResultValues = v->Version_1_0;
		m_overallResultValuesQObjectPtr = v;
	}
	else {
		Version_1_0->overallResultValues = nullptr;
	}

	overallResultValuesChanged();
}


bool CCDMResultObject::hasOverallResultValues()
{
	 return Version_1_0->overallResultValues.HasValue();
}


void CCDMResultObject::createOverallResultValues()
{	Version_1_0->overallResultValues.emplace();

}


QString CCDMResultObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CCDMResultObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CCDMResultObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CCDMResultObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QObject* CCDMResultObject::CreateObject(const QString& key)
{
	if (key == "resultMetaData"){
		return new sdl::complextest::CTImpl::CResultMetaDataObject(this);
	}
	if (key == "resultContent"){
		return new sdl::complextest::CTImpl::CCDMResultUnionTypeObjectList(this);
	}
	if (key == "overallResultValues"){
		return new sdl::complextest::CTImpl::COverallResultValuesObjectList(this);
	}
	return nullptr;
}


QString CCDMResultObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "dataModelTitle")){
		return "dataModelTitle";
	}
	if (propertyName == (QString("m_") + "dataModelVersion")){
		return "dataModelVersion";
	}
	if (propertyName == (QString("m_") + "resultMetaData")){
		return "resultMetaData";
	}
	if (propertyName == (QString("m_") + "resultContent")){
		return "resultContent";
	}
	if (propertyName == (QString("m_") + "overallResultValues")){
		return "overallResultValues";
	}

	return propertyName;
}


} // namespace sdl::complextest::CTImpl




/// \file CPointsInCoordinateFormat.cpp

namespace sdl::complextest::CTImpl
{


QByteArray CPointsInCoordinateFormat::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPointsInCoordinateFormat::V1_0::operator==(const V1_0& other) const
{
	return 
				pointAsCoordinates == other.pointAsCoordinates;
}


bool CPointsInCoordinateFormat::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	if (pointAsCoordinates){
		model.SetData("pointAsCoordinates", *pointAsCoordinates, modelIndex);
	}


	return true;
}


bool CPointsInCoordinateFormat::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant pointAsCoordinatesData = model.GetData("pointAsCoordinates", modelIndex);
	if (!pointAsCoordinatesData.isNull()){
		pointAsCoordinates = pointAsCoordinatesData.toBool();
	}

	return true;
}


bool CPointsInCoordinateFormat::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant pointAsCoordinatesData = model.GetData("pointAsCoordinates", modelIndex);
	if (!pointAsCoordinatesData.isNull()){
		pointAsCoordinates = pointAsCoordinatesData.toBool();
	}

	return true;
}


bool CPointsInCoordinateFormat::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (pointAsCoordinates){
		gqlObject.InsertParam("pointAsCoordinates", QVariant(*pointAsCoordinates));
	}

	return true;
}


bool CPointsInCoordinateFormat::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("pointAsCoordinates") && (gqlObject["pointAsCoordinates"].userType() == QMetaType::Bool)){
		pointAsCoordinates = gqlObject["pointAsCoordinates"].toBool();
	}

	return true;
}


bool CPointsInCoordinateFormat::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("pointAsCoordinates") && (gqlObject["pointAsCoordinates"].userType() == QMetaType::Bool)){
		pointAsCoordinates = gqlObject["pointAsCoordinates"].toBool();
	}

	return true;
}


bool CPointsInCoordinateFormat::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (pointAsCoordinates){
		jsonObject["pointAsCoordinates"] = QJsonValue::fromVariant(*pointAsCoordinates);
	}

	return true;
}


bool CPointsInCoordinateFormat::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("pointAsCoordinates") && jsonObject["pointAsCoordinates"].isBool()){
		pointAsCoordinates = jsonObject["pointAsCoordinates"].toBool();
	}

	return true;
}


bool CPointsInCoordinateFormat::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("pointAsCoordinates") && jsonObject["pointAsCoordinates"].isBool()){
		pointAsCoordinates = jsonObject["pointAsCoordinates"].toBool();
	}

	return true;
}


// serialize methods

bool CPointsInCoordinateFormat::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPointsInCoordinateFormat::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPointsInCoordinateFormat::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPointsInCoordinateFormat::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPointsInCoordinateFormat::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPointsInCoordinateFormat::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPointsInCoordinateFormat::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version object not initialized.").arg(__FILE__, QString::number(__LINE__));)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPointsInCoordinateFormat::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CPointsInCoordinateFormat::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


CPointsInCoordinateFormatObject::CPointsInCoordinateFormatObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CPointsInCoordinateFormatObject::pointAsCoordinatesChanged, this, &CItemModelBase::OnInternalModelChanged);
}


bool CPointsInCoordinateFormatObject::GetPointAsCoordinates()
{
	if (Version_1_0->pointAsCoordinates.has_value()){
		return Version_1_0->pointAsCoordinates.value();
	}

	return false;
}


void CPointsInCoordinateFormatObject::SetPointAsCoordinates(bool v)
{
	Version_1_0->pointAsCoordinates = v;
	pointAsCoordinatesChanged();
}


bool CPointsInCoordinateFormatObject::hasPointAsCoordinates()
{
	 return Version_1_0->pointAsCoordinates.HasValue();
}


QString CPointsInCoordinateFormatObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CPointsInCoordinateFormatObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CPointsInCoordinateFormatObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CPointsInCoordinateFormatObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QObject* CPointsInCoordinateFormatObject::CreateObject(const QString& key)
{
	return nullptr;
}


QString CPointsInCoordinateFormatObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "pointAsCoordinates")){
		return "pointAsCoordinates";
	}

	return propertyName;
}


} // namespace sdl::complextest::CTImpl




/// \file CGetLastProductionResultsGqlRequest.cpp


namespace sdl::complextest::CTImpl
{


QByteArray CGetLastProductionResultsGqlRequest::GetCommandId()
{
	return QByteArrayLiteral("GetLastProductionResults");
}


bool CGetLastProductionResultsGqlRequest::SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const GetLastProductionResultsRequestArguments& requestArguments, const GetLastProductionResultsRequestInfo& /*requestInfo*/)
{
	gqlRequest.SetCommandId(GetCommandId());

	// writting input arguments

	return true;
}


CGetLastProductionResultsGqlRequest::CGetLastProductionResultsGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead)
	: m_isValid(true)
{
	Q_UNUSED(optRead)
	m_isValid = true;
	m_gqlContextPtr = gqlRequest.GetRequestContext();

	const QByteArray protocolVersion = gqlRequest.GetProtocolVersion();

	// reading input arguments

	// reading requested fields
	const imtgql::CGqlFieldObject* requestedFieldsObjectPtr = nullptr;
	if (!gqlRequest.GetFields().GetFieldIds().isEmpty()){
		requestedFieldsObjectPtr = gqlRequest.GetFields().GetFieldArgumentObjectPtr(gqlRequest.GetFields().GetFieldIds().constFirst());
		if (requestedFieldsObjectPtr != nullptr){
			const QByteArrayList requestedIds = requestedFieldsObjectPtr->GetFieldIds();
			if (!requestedIds.isEmpty()){
				m_requestInfo.isProductIDRequested = true;
				m_requestInfo.isNameRequested = true;
				m_requestInfo.isStatusRequested = true;
				m_requestInfo.isPartSerialNumberRequested = true;
				m_requestInfo.isTimestampRequested = true;
				m_requestInfo.isProductImageRequested = true;
				m_requestInfo.isInspectionsRequested = requestedIds.contains("Inspections");
				const imtgql::CGqlFieldObject* inspectionsRequestedFieldsPtr = requestedFieldsObjectPtr->GetFieldArgumentObjectPtr("Inspections");
				if (inspectionsRequestedFieldsPtr != nullptr){
					const QByteArrayList inspectionsRequestedIds = inspectionsRequestedFieldsPtr->GetFieldIds();
					if (!inspectionsRequestedIds.isEmpty()){
						m_requestInfo.Inspections.isIDRequested = true;
						m_requestInfo.Inspections.isTypeIDRequested = true;
						m_requestInfo.Inspections.isNameRequested = true;
						m_requestInfo.Inspections.isStatusRequested = true;
						m_requestInfo.Inspections.isAnalyzersRequested = inspectionsRequestedIds.contains("Analyzers");
						const imtgql::CGqlFieldObject* analyzersRequestedFieldsPtr = inspectionsRequestedFieldsPtr->GetFieldArgumentObjectPtr("Analyzers");
						if (analyzersRequestedFieldsPtr != nullptr){
							const QByteArrayList analyzersRequestedIds = analyzersRequestedFieldsPtr->GetFieldIds();
							if (!analyzersRequestedIds.isEmpty()){
								m_requestInfo.Inspections.Analyzers.isAnalyzerNameRequested = true;
								m_requestInfo.Inspections.Analyzers.isAnalyzerResultRequested = true;
								m_requestInfo.Inspections.Analyzers.isAreasRequested = analyzersRequestedIds.contains("Areas");
								const imtgql::CGqlFieldObject* areasRequestedFieldsPtr = analyzersRequestedFieldsPtr->GetFieldArgumentObjectPtr("Areas");
								if (areasRequestedFieldsPtr != nullptr){
									const QByteArrayList areasRequestedIds = areasRequestedFieldsPtr->GetFieldIds();
									if (!areasRequestedIds.isEmpty()){
										m_requestInfo.Inspections.Analyzers.Areas.isAreaNameRequested = true;
										m_requestInfo.Inspections.Analyzers.Areas.isStatusRequested = true;
										m_requestInfo.Inspections.Analyzers.Areas.isIconPositionRequested = true;
										m_requestInfo.Inspections.Analyzers.Areas.isErrorTypeRequested = true;
										m_requestInfo.Inspections.Analyzers.Areas.isResultsRequested = true;
										const imtgql::CGqlFieldObject* iconPositionRequestedFieldsPtr = areasRequestedFieldsPtr->GetFieldArgumentObjectPtr("IconPosition");
										if (iconPositionRequestedFieldsPtr != nullptr){
											const QByteArrayList iconPositionRequestedIds = iconPositionRequestedFieldsPtr->GetFieldIds();
											if (!iconPositionRequestedIds.isEmpty()){
												m_requestInfo.Inspections.Analyzers.Areas.IconPosition.isXRequested = true;
												m_requestInfo.Inspections.Analyzers.Areas.IconPosition.isYRequested = true;
											}
										}
										const imtgql::CGqlFieldObject* resultsRequestedFieldsPtr = areasRequestedFieldsPtr->GetFieldArgumentObjectPtr("Results");
										if (resultsRequestedFieldsPtr != nullptr){
											const QByteArrayList resultsRequestedIds = resultsRequestedFieldsPtr->GetFieldIds();
											if (!resultsRequestedIds.isEmpty()){
												m_requestInfo.Inspections.Analyzers.Areas.Results.isAreaResultRequested = true;
												m_requestInfo.Inspections.Analyzers.Areas.Results.isExpectedMinValueRequested = true;
												m_requestInfo.Inspections.Analyzers.Areas.Results.isExpectedMaxValueRequested = true;
												m_requestInfo.Inspections.Analyzers.Areas.Results.isMeasuredValueRequested = true;
												m_requestInfo.Inspections.Analyzers.Areas.Results.isMinMeasuredValueRequested = true;
												m_requestInfo.Inspections.Analyzers.Areas.Results.isMaxMeasuredValueRequested = true;
												m_requestInfo.Inspections.Analyzers.Areas.Results.isMeanMeasuredValueRequested = true;
												m_requestInfo.Inspections.Analyzers.Areas.Results.isMeasurementTypeRequested = true;
												m_requestInfo.Inspections.Analyzers.Areas.Results.isMeasurementUnitRequested = true;
												m_requestInfo.Inspections.Analyzers.Areas.Results.isLengthRequested = true;
												m_requestInfo.Inspections.Analyzers.Areas.Results.isErrorTypeRequested = true;
												m_requestInfo.Inspections.Analyzers.Areas.Results.isGeometryRequested = true;
												const imtgql::CGqlFieldObject* geometryRequestedFieldsPtr = resultsRequestedFieldsPtr->GetFieldArgumentObjectPtr("Geometry");
												if (geometryRequestedFieldsPtr != nullptr){
													const QByteArrayList geometryRequestedIds = geometryRequestedFieldsPtr->GetFieldIds();
													if (!geometryRequestedIds.isEmpty()){
														m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.isGeometryTypeRequested = true;
														m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.isRadiusRequested = geometryRequestedIds.contains("Radius");
														m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.isPointsRequested = true;
														const imtgql::CGqlFieldObject* pointsRequestedFieldsPtr = geometryRequestedFieldsPtr->GetFieldArgumentObjectPtr("Points");
														if (pointsRequestedFieldsPtr != nullptr){
															const QByteArrayList pointsRequestedIds = pointsRequestedFieldsPtr->GetFieldIds();
															if (!pointsRequestedIds.isEmpty()){
																m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.Points.isXRequested = true;
																m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.Points.isYRequested = true;
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}


bool CGetLastProductionResultsGqlRequest::IsValid() const
{
	return m_isValid;
}


const ::imtgql::IGqlContext* CGetLastProductionResultsGqlRequest::GetRequestContext() const
{
	return m_gqlContextPtr;
}


const GetLastProductionResultsRequestArguments& CGetLastProductionResultsGqlRequest::GetRequestedArguments() const
{
	return m_requestedArguments;
}


GetLastProductionResultsRequestInfo CGetLastProductionResultsGqlRequest::GetRequestInfo() const
{
	return m_requestInfo;
}


} // namespace sdl::complextest::CTImpl



/// \file CGetLastProductionResultsCDMGqlRequest.cpp


namespace sdl::complextest::CTImpl
{


QByteArray CGetLastProductionResultsCDMGqlRequest::GetCommandId()
{
	return QByteArrayLiteral("GetLastProductionResultsCDM");
}


bool CGetLastProductionResultsCDMGqlRequest::SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const GetLastProductionResultsCDMRequestArguments& requestArguments, const GetLastProductionResultsCDMRequestInfo& /*requestInfo*/)
{
	gqlRequest.SetCommandId(GetCommandId());

	// writting input arguments
	::imtgql::CGqlParamObject pointAsCoordinatesDataObject;
	if (!requestArguments.pointAsCoordinates.WriteToGraphQlObject(pointAsCoordinatesDataObject)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to create GQL Object").arg(__FILE__, QString::number(__LINE__));)

		return false;
	}
	gqlRequest.AddParam("pointAsCoordinates", pointAsCoordinatesDataObject);

	return true;
}


CGetLastProductionResultsCDMGqlRequest::CGetLastProductionResultsCDMGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead)
	: m_isValid(true)
{
	m_gqlContextPtr = gqlRequest.GetRequestContext();

	const QByteArray protocolVersion = gqlRequest.GetProtocolVersion();

	// reading input arguments
	const ::imtgql::CGqlParamObject* pointAsCoordinatesDataObjectPtr = gqlRequest.GetParamObject("pointAsCoordinates");
	if (pointAsCoordinatesDataObjectPtr != nullptr){
		if (!protocolVersion.isEmpty()){
			if (protocolVersion == "1.0"){
				bool isPointAsCoordinatesRead;
				if (optRead){
					isPointAsCoordinatesRead = m_requestedArguments.pointAsCoordinates.OptReadFromGraphQlObject(*pointAsCoordinatesDataObjectPtr, CPointsInCoordinateFormat::PV_1_0);
				}
				else {
					isPointAsCoordinatesRead = m_requestedArguments.pointAsCoordinates.ReadFromGraphQlObject(*pointAsCoordinatesDataObjectPtr, CPointsInCoordinateFormat::PV_1_0);
				}
				m_isValid = isPointAsCoordinatesRead;
				if (!isPointAsCoordinatesRead){
					return;
				}
			}
			else {
				qWarning() << QString("Bad request. Version %1 is not supported").arg(qPrintable(protocolVersion));
				m_isValid = false;

				return;
			}
		}
		else {
			bool isPointAsCoordinatesRead;
			if (optRead){
				isPointAsCoordinatesRead = m_requestedArguments.pointAsCoordinates.OptReadFromGraphQlObject(*pointAsCoordinatesDataObjectPtr);
			}
			else {
				isPointAsCoordinatesRead = m_requestedArguments.pointAsCoordinates.ReadFromGraphQlObject(*pointAsCoordinatesDataObjectPtr);
			}
			m_isValid = isPointAsCoordinatesRead;
			if (!isPointAsCoordinatesRead){
				return;
			}
		}
	}

	// reading requested fields
	const imtgql::CGqlFieldObject* requestedFieldsObjectPtr = nullptr;
	if (!gqlRequest.GetFields().GetFieldIds().isEmpty()){
		requestedFieldsObjectPtr = gqlRequest.GetFields().GetFieldArgumentObjectPtr(gqlRequest.GetFields().GetFieldIds().constFirst());
		if (requestedFieldsObjectPtr != nullptr){
			const QByteArrayList requestedIds = requestedFieldsObjectPtr->GetFieldIds();
			if (!requestedIds.isEmpty()){
				m_requestInfo.isDataModelTitleRequested = true;
				m_requestInfo.isDataModelVersionRequested = true;
				m_requestInfo.isResultMetaDataRequested = requestedIds.contains("resultMetaData");
				m_requestInfo.isResultContentRequested = requestedIds.contains("resultContent");
				m_requestInfo.isOverallResultValuesRequested = requestedIds.contains("overallResultValues");
				const imtgql::CGqlFieldObject* resultMetaDataRequestedFieldsPtr = requestedFieldsObjectPtr->GetFieldArgumentObjectPtr("resultMetaData");
				if (resultMetaDataRequestedFieldsPtr != nullptr){
					const QByteArrayList resultMetaDataRequestedIds = resultMetaDataRequestedFieldsPtr->GetFieldIds();
					if (!resultMetaDataRequestedIds.isEmpty()){
						m_requestInfo.resultMetaData.isResultIdRequested = true;
						m_requestInfo.resultMetaData.isCreationTimeRequested = resultMetaDataRequestedIds.contains("creationTime");
						m_requestInfo.resultMetaData.isPartIdRequested = resultMetaDataRequestedIds.contains("partId");
						m_requestInfo.resultMetaData.isProductIdRequested = resultMetaDataRequestedIds.contains("productId");
						m_requestInfo.resultMetaData.isNameRequested = resultMetaDataRequestedIds.contains("name");
						m_requestInfo.resultMetaData.isClassificationRequested = resultMetaDataRequestedIds.contains("classification");
						m_requestInfo.resultMetaData.isDescriptionRequested = resultMetaDataRequestedIds.contains("description");
						m_requestInfo.resultMetaData.isExtendedMetaDataRequested = resultMetaDataRequestedIds.contains("extendedMetaData");
						const imtgql::CGqlFieldObject* descriptionRequestedFieldsPtr = resultMetaDataRequestedFieldsPtr->GetFieldArgumentObjectPtr("description");
						if (descriptionRequestedFieldsPtr != nullptr){
							const QByteArrayList descriptionRequestedIds = descriptionRequestedFieldsPtr->GetFieldIds();
							if (!descriptionRequestedIds.isEmpty()){
								m_requestInfo.resultMetaData.description.isTextRequested = true;
								m_requestInfo.resultMetaData.description.isLocaleRequested = true;
							}
						}
						const imtgql::CGqlFieldObject* extendedMetaDataRequestedFieldsPtr = resultMetaDataRequestedFieldsPtr->GetFieldArgumentObjectPtr("extendedMetaData");
						if (extendedMetaDataRequestedFieldsPtr != nullptr){
							const QByteArrayList extendedMetaDataRequestedIds = extendedMetaDataRequestedFieldsPtr->GetFieldIds();
							if (!extendedMetaDataRequestedIds.isEmpty()){
								m_requestInfo.resultMetaData.extendedMetaData.isKeyRequested = extendedMetaDataRequestedIds.contains("key");
								m_requestInfo.resultMetaData.extendedMetaData.isValueRequested = extendedMetaDataRequestedIds.contains("value");
							}
						}
					}
				}
				const imtgql::CGqlFieldObject* overallResultValuesRequestedFieldsPtr = requestedFieldsObjectPtr->GetFieldArgumentObjectPtr("overallResultValues");
				if (overallResultValuesRequestedFieldsPtr != nullptr){
					const QByteArrayList overallResultValuesRequestedIds = overallResultValuesRequestedFieldsPtr->GetFieldIds();
					if (!overallResultValuesRequestedIds.isEmpty()){
						m_requestInfo.overallResultValues.isNameRequested = overallResultValuesRequestedIds.contains("name");
						m_requestInfo.overallResultValues.isValueIdRequested = true;
						m_requestInfo.overallResultValues.isMeasuredValueRequested = true;
						m_requestInfo.overallResultValues.isLowLimitRequested = overallResultValuesRequestedIds.contains("lowLimit");
						m_requestInfo.overallResultValues.isHighLimitRequested = overallResultValuesRequestedIds.contains("highLimit");
					}
				}
			}
		}
	}
}


bool CGetLastProductionResultsCDMGqlRequest::IsValid() const
{
	return m_isValid;
}


const ::imtgql::IGqlContext* CGetLastProductionResultsCDMGqlRequest::GetRequestContext() const
{
	return m_gqlContextPtr;
}


const GetLastProductionResultsCDMRequestArguments& CGetLastProductionResultsCDMGqlRequest::GetRequestedArguments() const
{
	return m_requestedArguments;
}


GetLastProductionResultsCDMRequestInfo CGetLastProductionResultsCDMGqlRequest::GetRequestInfo() const
{
	return m_requestInfo;
}


} // namespace sdl::complextest::CTImpl



/// \file CGraphQlHandlerCompBase.cpp


namespace sdl::complextest::CTImpl
{


bool CGraphQlHandlerCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	if (commandId == CGetLastProductionResultsGqlRequest::GetCommandId()){
		return true;
	}
	else if (commandId == CGetLastProductionResultsCDMGqlRequest::GetCommandId()){
		return true;
	}

	return BaseClass::IsRequestSupported(gqlRequest);
}


::imtbase::CTreeItemModel* CGraphQlHandlerCompBase::CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	istd::TDelPtr<::imtbase::CTreeItemModel> modelPtr(new ::imtbase::CTreeItemModel);
	::imtbase::CTreeItemModel* dataModelPtr = modelPtr->AddTreeModel("data");

	// GetLastProductionResults
	if (commandId == CGetLastProductionResultsGqlRequest::GetCommandId()){
		CGetLastProductionResultsGqlRequest getLastProductionResultsGqlRequest(gqlRequest, false);
		if (!getLastProductionResultsGqlRequest.IsValid()){
			errorMessage = QString("Bad request. Unexpected request for command-ID: '%1'").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage);

			return nullptr;
		}

		CProductOverview replyPayload = OnGetLastProductionResults(getLastProductionResultsGqlRequest, gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			SendErrorMessage(0, QString("The derived call [OnGetLastProductionResults] returned an error: %1").arg(errorMessage));

			return nullptr;
		}

		const bool isModelCreated = replyPayload.WriteToModel(*dataModelPtr);
		if (!isModelCreated){
			errorMessage = QString("Internal error. Unable to create response for command-ID: '%1'").arg(qPrintable(commandId));
			SendCriticalMessage(0, errorMessage);

			return nullptr;
		}

		return modelPtr.PopPtr();
	}

	// GetLastProductionResultsCDM
	if (commandId == CGetLastProductionResultsCDMGqlRequest::GetCommandId()){
		CGetLastProductionResultsCDMGqlRequest getLastProductionResultsCDMGqlRequest(gqlRequest, false);
		if (!getLastProductionResultsCDMGqlRequest.IsValid()){
			errorMessage = QString("Bad request. Unexpected request for command-ID: '%1'").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage);

			return nullptr;
		}

		CCDMResult replyPayload = OnGetLastProductionResultsCDM(getLastProductionResultsCDMGqlRequest, gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			SendErrorMessage(0, QString("The derived call [OnGetLastProductionResultsCDM] returned an error: %1").arg(errorMessage));

			return nullptr;
		}

		const bool isModelCreated = replyPayload.WriteToModel(*dataModelPtr);
		if (!isModelCreated){
			errorMessage = QString("Internal error. Unable to create response for command-ID: '%1'").arg(qPrintable(commandId));
			SendCriticalMessage(0, errorMessage);

			return nullptr;
		}

		return modelPtr.PopPtr();
	}

	errorMessage = QString("Bad request. Unexpected command-ID: '%1'").arg(qPrintable(commandId));
	SendErrorMessage(0, errorMessage);

	return nullptr;
}




} // namespace sdl::complextest::CTImpl



