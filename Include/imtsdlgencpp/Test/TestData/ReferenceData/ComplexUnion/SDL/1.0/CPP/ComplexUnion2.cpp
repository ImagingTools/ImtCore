#include "ComplexUnion2.h"
#include "ComplexUnion2_fwd.h"


namespace sdl::V1_0::complextest
{


QByteArray CResult::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CResult::operator==(const CResult& other) const
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


bool CResult::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Result", modelIndex);

	if (!AreaResult){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaResult").toLocal8Bit().constData();)

		return false;
	}
	QString areaResultStringValue;
	switch (*AreaResult){
	case ::sdl::V1_0::complextest::StatusCode::NONE:
		areaResultStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::StatusCode::OK:
		areaResultStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::WARNING:
		areaResultStringValue = "WARNING";
		break;
	case ::sdl::V1_0::complextest::StatusCode::NOK:
		areaResultStringValue = "NOK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::FAILED:
		areaResultStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("AreaResult", areaResultStringValue, modelIndex);

	if (!ExpectedMinValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMinValue").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("ExpectedMinValue", *ExpectedMinValue, modelIndex);

	if (!ExpectedMaxValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMaxValue").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("ExpectedMaxValue", *ExpectedMaxValue, modelIndex);

	if (!MeasuredValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("MeasuredValue", *MeasuredValue, modelIndex);

	if (!MinMeasuredValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MinMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("MinMeasuredValue", *MinMeasuredValue, modelIndex);

	if (!MaxMeasuredValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MaxMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("MaxMeasuredValue", *MaxMeasuredValue, modelIndex);

	if (!MeanMeasuredValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeanMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("MeanMeasuredValue", *MeanMeasuredValue, modelIndex);

	if (!MeasurementType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementType").toLocal8Bit().constData();)

		return false;
	}
	QString measurementTypeStringValue;
	switch (*MeasurementType){
	case ::sdl::V1_0::complextest::MeasurementType::NONE:
		measurementTypeStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::WIDTH:
		measurementTypeStringValue = "WIDTH";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::BRIGHTNESS:
		measurementTypeStringValue = "BRIGHTNESS";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::DISTANCE:
		measurementTypeStringValue = "DISTANCE";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::DISPLACEMENT:
		measurementTypeStringValue = "DISPLACEMENT";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::HEIGHT:
		measurementTypeStringValue = "HEIGHT";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::SYMMETRY:
		measurementTypeStringValue = "SYMMETRY";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::VOLUME:
		measurementTypeStringValue = "VOLUME";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::VOLUMERATE:
		measurementTypeStringValue = "VOLUMERATE";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::LENGTH:
		measurementTypeStringValue = "LENGTH";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::OUT_OF_BOUND:
		measurementTypeStringValue = "OUT_OF_BOUND";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("MeasurementType", measurementTypeStringValue, modelIndex);

	if (!MeasurementUnit){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit").toLocal8Bit().constData();)

		return false;
	}
	QString measurementUnitStringValue;
	switch (*MeasurementUnit){
	case ::sdl::V1_0::complextest::MeasurementUnit::NONE:
		measurementUnitStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::MeasurementUnit::mm:
		measurementUnitStringValue = "mm";
		break;
	case ::sdl::V1_0::complextest::MeasurementUnit::mm3:
		measurementUnitStringValue = "mm3";
		break;
	case ::sdl::V1_0::complextest::MeasurementUnit::mm3_mm:
		measurementUnitStringValue = "mm3_mm";
		break;
	case ::sdl::V1_0::complextest::MeasurementUnit::pixel:
		measurementUnitStringValue = "pixel";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("MeasurementUnit", measurementUnitStringValue, modelIndex);

	if (!Length){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Length").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("Length", *Length, modelIndex);

	if (!ErrorType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

		return false;
	}
	QString errorTypeStringValue;
	switch (*ErrorType){
	case ::sdl::V1_0::complextest::ErrorCode::OK:
		errorTypeStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE:
		errorTypeStringValue = "TOO_WIDE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOLERANCE:
		errorTypeStringValue = "TOLERANCE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW:
		errorTypeStringValue = "TOO_NARROW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP:
		errorTypeStringValue = "NO_DATA_GAP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::GAP:
		errorTypeStringValue = "GAP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH:
		errorTypeStringValue = "TOO_HIGH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_LOW:
		errorTypeStringValue = "TOO_LOW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_FEW:
		errorTypeStringValue = "TOO_FEW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH:
		errorTypeStringValue = "TOO_MUCH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER:
		errorTypeStringValue = "WRONG_DOT_DIAMETER";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION:
		errorTypeStringValue = "WRONG_DOT_POSITION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS:
		errorTypeStringValue = "MISSING_DOTS";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE:
		errorTypeStringValue = "WRONG_DOT_DISTANCE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE:
		errorTypeStringValue = "AREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE:
		errorTypeStringValue = "TOTALAREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE:
		errorTypeStringValue = "BORDERAREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA:
		errorTypeStringValue = "WRONG_DOT_AREA";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT:
		errorTypeStringValue = "WRONG_DOT_AREA_ONHEIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME:
		errorTypeStringValue = "WRONG_DOT_VOLUME";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT:
		errorTypeStringValue = "WRONG_DOT_HEIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR:
		errorTypeStringValue = "CONTOUR_MEASUREMENT_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT:
		errorTypeStringValue = "TOO_BRIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_DARK:
		errorTypeStringValue = "TOO_DARK";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR:
		errorTypeStringValue = "COUNT_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH:
		errorTypeStringValue = "Z_DISTANCE_HIGH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW:
		errorTypeStringValue = "Z_DISTANCE_LOW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL:
		errorTypeStringValue = "NOT_SYMMETRICAL";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR:
		errorTypeStringValue = "REFERENCE_3D_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR:
		errorTypeStringValue = "COLOR_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT:
		errorTypeStringValue = "PATTERN_COUNT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION:
		errorTypeStringValue = "PATTERN_POSITION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION:
		errorTypeStringValue = "PATTERN_ROTATION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CENTERLINE:
		errorTypeStringValue = "CENTERLINE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP:
		errorTypeStringValue = "CALIB_ERROR_COMP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP:
		errorTypeStringValue = "CALIB_INVALID_COMP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED:
		errorTypeStringValue = "CALIB_NOZZLE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND:
		errorTypeStringValue = "CONTOUR_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE:
		errorTypeStringValue = "CONTOUR_TOO_WIDE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW:
		errorTypeStringValue = "CONTOUR_TOO_NARROW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED:
		errorTypeStringValue = "CONTOUR_GLUE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY:
		errorTypeStringValue = "CONTOUR_INVALID_POLY";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND:
		errorTypeStringValue = "REFERENCE_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF:
		errorTypeStringValue = "REFERENCE_INVALID_REF";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED:
		errorTypeStringValue = "REFERENCE_POS_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED:
		errorTypeStringValue = "REFERENCE_DIST_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND:
		errorTypeStringValue = "AREA_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND:
		errorTypeStringValue = "AREA_GLUE_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED:
		errorTypeStringValue = "AREA_GLUE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH:
		errorTypeStringValue = "AREA_GAP_LENGTH";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("ErrorType", errorTypeStringValue, modelIndex);


	if (!Geometry){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}
	::imtbase::CTreeItemModel* geometryNewModelPtr = model.AddTreeModel("Geometry", modelIndex);
	const bool isGeometryAdded = Geometry->WriteToModel(*geometryNewModelPtr, 0);
	if (!isGeometryAdded){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}


	return true;
}


bool CResult::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant areaResultData = model.GetData("AreaResult", modelIndex);
	if (areaResultData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "AreaResult").toLocal8Bit().constData();)

		return false;
	}
	QString areaResultStringValue = areaResultData.toString();
	if(areaResultStringValue == "NONE"){
		AreaResult = ::sdl::V1_0::complextest::StatusCode::NONE;
	}
	else if(areaResultStringValue == "OK"){
		AreaResult = ::sdl::V1_0::complextest::StatusCode::OK;
	}
	else if(areaResultStringValue == "WARNING"){
		AreaResult = ::sdl::V1_0::complextest::StatusCode::WARNING;
	}
	else if(areaResultStringValue == "NOK"){
		AreaResult = ::sdl::V1_0::complextest::StatusCode::NOK;
	}
	else if(areaResultStringValue == "FAILED"){
		AreaResult = ::sdl::V1_0::complextest::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", areaResultStringValue);)

		return false;
	}

	QVariant expectedMinValueData = model.GetData("ExpectedMinValue", modelIndex);
	if (expectedMinValueData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMinValue").toLocal8Bit().constData();)

		return false;
	}
	ExpectedMinValue = expectedMinValueData.toFloat();

	QVariant expectedMaxValueData = model.GetData("ExpectedMaxValue", modelIndex);
	if (expectedMaxValueData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMaxValue").toLocal8Bit().constData();)

		return false;
	}
	ExpectedMaxValue = expectedMaxValueData.toFloat();

	QVariant measuredValueData = model.GetData("MeasuredValue", modelIndex);
	if (measuredValueData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MeasuredValue = measuredValueData.toFloat();

	QVariant minMeasuredValueData = model.GetData("MinMeasuredValue", modelIndex);
	if (minMeasuredValueData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MinMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MinMeasuredValue = minMeasuredValueData.toFloat();

	QVariant maxMeasuredValueData = model.GetData("MaxMeasuredValue", modelIndex);
	if (maxMeasuredValueData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MaxMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MaxMeasuredValue = maxMeasuredValueData.toFloat();

	QVariant meanMeasuredValueData = model.GetData("MeanMeasuredValue", modelIndex);
	if (meanMeasuredValueData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MeanMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MeanMeasuredValue = meanMeasuredValueData.toFloat();

	QVariant measurementTypeData = model.GetData("MeasurementType", modelIndex);
	if (measurementTypeData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementType").toLocal8Bit().constData();)

		return false;
	}
	QString measurementTypeStringValue = measurementTypeData.toString();
	if(measurementTypeStringValue == "NONE"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::NONE;
	}
	else if(measurementTypeStringValue == "WIDTH"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::WIDTH;
	}
	else if(measurementTypeStringValue == "BRIGHTNESS"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::BRIGHTNESS;
	}
	else if(measurementTypeStringValue == "DISTANCE"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::DISTANCE;
	}
	else if(measurementTypeStringValue == "DISPLACEMENT"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::DISPLACEMENT;
	}
	else if(measurementTypeStringValue == "HEIGHT"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::HEIGHT;
	}
	else if(measurementTypeStringValue == "SYMMETRY"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::SYMMETRY;
	}
	else if(measurementTypeStringValue == "VOLUME"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::VOLUME;
	}
	else if(measurementTypeStringValue == "VOLUMERATE"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::VOLUMERATE;
	}
	else if(measurementTypeStringValue == "LENGTH"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::LENGTH;
	}
	else if(measurementTypeStringValue == "OUT_OF_BOUND"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::OUT_OF_BOUND;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

		return false;
	}

	QVariant measurementUnitData = model.GetData("MeasurementUnit", modelIndex);
	if (measurementUnitData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit").toLocal8Bit().constData();)

		return false;
	}
	QString measurementUnitStringValue = measurementUnitData.toString();
	if(measurementUnitStringValue == "NONE"){
		MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::NONE;
	}
	else if(measurementUnitStringValue == "mm"){
		MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm;
	}
	else if(measurementUnitStringValue == "mm3"){
		MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm3;
	}
	else if(measurementUnitStringValue == "mm3_mm"){
		MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm3_mm;
	}
	else if(measurementUnitStringValue == "pixel"){
		MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::pixel;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit", measurementUnitStringValue);)

		return false;
	}

	QVariant lengthData = model.GetData("Length", modelIndex);
	if (lengthData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Length").toLocal8Bit().constData();)

		return false;
	}
	Length = lengthData.toFloat();

	QVariant errorTypeData = model.GetData("ErrorType", modelIndex);
	if (errorTypeData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

		return false;
	}
	QString errorTypeStringValue = errorTypeData.toString();
	if(errorTypeStringValue == "OK"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::OK;
	}
	else if(errorTypeStringValue == "TOO_WIDE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE;
	}
	else if(errorTypeStringValue == "TOLERANCE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOLERANCE;
	}
	else if(errorTypeStringValue == "TOO_NARROW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW;
	}
	else if(errorTypeStringValue == "NO_DATA_GAP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP;
	}
	else if(errorTypeStringValue == "GAP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::GAP;
	}
	else if(errorTypeStringValue == "TOO_HIGH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH;
	}
	else if(errorTypeStringValue == "TOO_LOW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_LOW;
	}
	else if(errorTypeStringValue == "TOO_FEW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_FEW;
	}
	else if(errorTypeStringValue == "TOO_MUCH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER;
	}
	else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION;
	}
	else if(errorTypeStringValue == "MISSING_DOTS"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE;
	}
	else if(errorTypeStringValue == "AREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
	}
	else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME;
	}
	else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT;
	}
	else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
	}
	else if(errorTypeStringValue == "TOO_BRIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT;
	}
	else if(errorTypeStringValue == "TOO_DARK"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_DARK;
	}
	else if(errorTypeStringValue == "COUNT_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW;
	}
	else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL;
	}
	else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR;
	}
	else if(errorTypeStringValue == "COLOR_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR;
	}
	else if(errorTypeStringValue == "PATTERN_COUNT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT;
	}
	else if(errorTypeStringValue == "PATTERN_POSITION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION;
	}
	else if(errorTypeStringValue == "PATTERN_ROTATION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION;
	}
	else if(errorTypeStringValue == "CENTERLINE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CENTERLINE;
	}
	else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP;
	}
	else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP;
	}
	else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW;
	}
	else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY;
	}
	else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND;
	}
	else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF;
	}
	else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED;
	}
	else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED;
	}
	else if(errorTypeStringValue == "AREA_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* geometryDataModelPtr = model.GetTreeItemModel("Geometry", modelIndex);
	if (geometryDataModelPtr == nullptr){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is missing, but required").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}
	Geometry = CGeometry();
	const bool isGeometryRead = Geometry->ReadFromModel(*geometryDataModelPtr, modelIndex);
	if (!isGeometryRead){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}


	return true;
}


bool CResult::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant areaResultData = model.GetData("AreaResult", modelIndex);
	if (!areaResultData.isNull()){
		QString areaResultStringValue = areaResultData.toString();
		if(areaResultStringValue == "NONE"){
			AreaResult = ::sdl::V1_0::complextest::StatusCode::NONE;
		}
		else if(areaResultStringValue == "OK"){
			AreaResult = ::sdl::V1_0::complextest::StatusCode::OK;
		}
		else if(areaResultStringValue == "WARNING"){
			AreaResult = ::sdl::V1_0::complextest::StatusCode::WARNING;
		}
		else if(areaResultStringValue == "NOK"){
			AreaResult = ::sdl::V1_0::complextest::StatusCode::NOK;
		}
		else if(areaResultStringValue == "FAILED"){
			AreaResult = ::sdl::V1_0::complextest::StatusCode::FAILED;
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
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::NONE;
		}
		else if(measurementTypeStringValue == "WIDTH"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::WIDTH;
		}
		else if(measurementTypeStringValue == "BRIGHTNESS"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::BRIGHTNESS;
		}
		else if(measurementTypeStringValue == "DISTANCE"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::DISTANCE;
		}
		else if(measurementTypeStringValue == "DISPLACEMENT"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::DISPLACEMENT;
		}
		else if(measurementTypeStringValue == "HEIGHT"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::HEIGHT;
		}
		else if(measurementTypeStringValue == "SYMMETRY"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::SYMMETRY;
		}
		else if(measurementTypeStringValue == "VOLUME"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::VOLUME;
		}
		else if(measurementTypeStringValue == "VOLUMERATE"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::VOLUMERATE;
		}
		else if(measurementTypeStringValue == "LENGTH"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::LENGTH;
		}
		else if(measurementTypeStringValue == "OUT_OF_BOUND"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::OUT_OF_BOUND;
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
			MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::NONE;
		}
		else if(measurementUnitStringValue == "mm"){
			MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm;
		}
		else if(measurementUnitStringValue == "mm3"){
			MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm3;
		}
		else if(measurementUnitStringValue == "mm3_mm"){
			MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm3_mm;
		}
		else if(measurementUnitStringValue == "pixel"){
			MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::pixel;
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
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::OK;
		}
		else if(errorTypeStringValue == "TOO_WIDE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE;
		}
		else if(errorTypeStringValue == "TOLERANCE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOLERANCE;
		}
		else if(errorTypeStringValue == "TOO_NARROW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW;
		}
		else if(errorTypeStringValue == "NO_DATA_GAP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP;
		}
		else if(errorTypeStringValue == "GAP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::GAP;
		}
		else if(errorTypeStringValue == "TOO_HIGH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH;
		}
		else if(errorTypeStringValue == "TOO_LOW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_LOW;
		}
		else if(errorTypeStringValue == "TOO_FEW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_FEW;
		}
		else if(errorTypeStringValue == "TOO_MUCH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER;
		}
		else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION;
		}
		else if(errorTypeStringValue == "MISSING_DOTS"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE;
		}
		else if(errorTypeStringValue == "AREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
		}
		else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME;
		}
		else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT;
		}
		else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
		}
		else if(errorTypeStringValue == "TOO_BRIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT;
		}
		else if(errorTypeStringValue == "TOO_DARK"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_DARK;
		}
		else if(errorTypeStringValue == "COUNT_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW;
		}
		else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL;
		}
		else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR;
		}
		else if(errorTypeStringValue == "COLOR_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR;
		}
		else if(errorTypeStringValue == "PATTERN_COUNT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT;
		}
		else if(errorTypeStringValue == "PATTERN_POSITION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION;
		}
		else if(errorTypeStringValue == "PATTERN_ROTATION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION;
		}
		else if(errorTypeStringValue == "CENTERLINE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CENTERLINE;
		}
		else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP;
		}
		else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP;
		}
		else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW;
		}
		else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY;
		}
		else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND;
		}
		else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF;
		}
		else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED;
		}
		else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED;
		}
		else if(errorTypeStringValue == "AREA_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* geometryDataModelPtr = model.GetTreeItemModel("Geometry", modelIndex);
	if (geometryDataModelPtr != nullptr){
		Geometry = CGeometry();
		const bool isGeometryRead = Geometry->ReadFromModel(*geometryDataModelPtr, modelIndex);
		if (!isGeometryRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CResult::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!AreaResult){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaResult").toLocal8Bit().constData();)

		return false;
	}
	QString areaResultStringValue;
	switch (*AreaResult){
	case ::sdl::V1_0::complextest::StatusCode::NONE:
		areaResultStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::StatusCode::OK:
		areaResultStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::WARNING:
		areaResultStringValue = "WARNING";
		break;
	case ::sdl::V1_0::complextest::StatusCode::NOK:
		areaResultStringValue = "NOK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::FAILED:
		areaResultStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("AreaResult", QVariant(areaResultStringValue));

	if (!ExpectedMinValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMinValue").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("ExpectedMinValue", QVariant(*ExpectedMinValue));

	if (!ExpectedMaxValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMaxValue").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("ExpectedMaxValue", QVariant(*ExpectedMaxValue));

	if (!MeasuredValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("MeasuredValue", QVariant(*MeasuredValue));

	if (!MinMeasuredValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MinMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("MinMeasuredValue", QVariant(*MinMeasuredValue));

	if (!MaxMeasuredValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MaxMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("MaxMeasuredValue", QVariant(*MaxMeasuredValue));

	if (!MeanMeasuredValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeanMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("MeanMeasuredValue", QVariant(*MeanMeasuredValue));

	if (!MeasurementType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementType").toLocal8Bit().constData();)

		return false;
	}
	QString measurementTypeStringValue;
	switch (*MeasurementType){
	case ::sdl::V1_0::complextest::MeasurementType::NONE:
		measurementTypeStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::WIDTH:
		measurementTypeStringValue = "WIDTH";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::BRIGHTNESS:
		measurementTypeStringValue = "BRIGHTNESS";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::DISTANCE:
		measurementTypeStringValue = "DISTANCE";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::DISPLACEMENT:
		measurementTypeStringValue = "DISPLACEMENT";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::HEIGHT:
		measurementTypeStringValue = "HEIGHT";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::SYMMETRY:
		measurementTypeStringValue = "SYMMETRY";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::VOLUME:
		measurementTypeStringValue = "VOLUME";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::VOLUMERATE:
		measurementTypeStringValue = "VOLUMERATE";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::LENGTH:
		measurementTypeStringValue = "LENGTH";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::OUT_OF_BOUND:
		measurementTypeStringValue = "OUT_OF_BOUND";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("MeasurementType", QVariant(measurementTypeStringValue));

	if (!MeasurementUnit){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit").toLocal8Bit().constData();)

		return false;
	}
	QString measurementUnitStringValue;
	switch (*MeasurementUnit){
	case ::sdl::V1_0::complextest::MeasurementUnit::NONE:
		measurementUnitStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::MeasurementUnit::mm:
		measurementUnitStringValue = "mm";
		break;
	case ::sdl::V1_0::complextest::MeasurementUnit::mm3:
		measurementUnitStringValue = "mm3";
		break;
	case ::sdl::V1_0::complextest::MeasurementUnit::mm3_mm:
		measurementUnitStringValue = "mm3_mm";
		break;
	case ::sdl::V1_0::complextest::MeasurementUnit::pixel:
		measurementUnitStringValue = "pixel";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("MeasurementUnit", QVariant(measurementUnitStringValue));

	if (!Length){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Length").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("Length", QVariant(*Length));

	if (!ErrorType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

		return false;
	}
	QString errorTypeStringValue;
	switch (*ErrorType){
	case ::sdl::V1_0::complextest::ErrorCode::OK:
		errorTypeStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE:
		errorTypeStringValue = "TOO_WIDE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOLERANCE:
		errorTypeStringValue = "TOLERANCE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW:
		errorTypeStringValue = "TOO_NARROW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP:
		errorTypeStringValue = "NO_DATA_GAP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::GAP:
		errorTypeStringValue = "GAP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH:
		errorTypeStringValue = "TOO_HIGH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_LOW:
		errorTypeStringValue = "TOO_LOW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_FEW:
		errorTypeStringValue = "TOO_FEW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH:
		errorTypeStringValue = "TOO_MUCH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER:
		errorTypeStringValue = "WRONG_DOT_DIAMETER";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION:
		errorTypeStringValue = "WRONG_DOT_POSITION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS:
		errorTypeStringValue = "MISSING_DOTS";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE:
		errorTypeStringValue = "WRONG_DOT_DISTANCE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE:
		errorTypeStringValue = "AREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE:
		errorTypeStringValue = "TOTALAREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE:
		errorTypeStringValue = "BORDERAREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA:
		errorTypeStringValue = "WRONG_DOT_AREA";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT:
		errorTypeStringValue = "WRONG_DOT_AREA_ONHEIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME:
		errorTypeStringValue = "WRONG_DOT_VOLUME";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT:
		errorTypeStringValue = "WRONG_DOT_HEIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR:
		errorTypeStringValue = "CONTOUR_MEASUREMENT_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT:
		errorTypeStringValue = "TOO_BRIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_DARK:
		errorTypeStringValue = "TOO_DARK";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR:
		errorTypeStringValue = "COUNT_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH:
		errorTypeStringValue = "Z_DISTANCE_HIGH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW:
		errorTypeStringValue = "Z_DISTANCE_LOW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL:
		errorTypeStringValue = "NOT_SYMMETRICAL";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR:
		errorTypeStringValue = "REFERENCE_3D_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR:
		errorTypeStringValue = "COLOR_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT:
		errorTypeStringValue = "PATTERN_COUNT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION:
		errorTypeStringValue = "PATTERN_POSITION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION:
		errorTypeStringValue = "PATTERN_ROTATION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CENTERLINE:
		errorTypeStringValue = "CENTERLINE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP:
		errorTypeStringValue = "CALIB_ERROR_COMP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP:
		errorTypeStringValue = "CALIB_INVALID_COMP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED:
		errorTypeStringValue = "CALIB_NOZZLE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND:
		errorTypeStringValue = "CONTOUR_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE:
		errorTypeStringValue = "CONTOUR_TOO_WIDE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW:
		errorTypeStringValue = "CONTOUR_TOO_NARROW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED:
		errorTypeStringValue = "CONTOUR_GLUE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY:
		errorTypeStringValue = "CONTOUR_INVALID_POLY";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND:
		errorTypeStringValue = "REFERENCE_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF:
		errorTypeStringValue = "REFERENCE_INVALID_REF";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED:
		errorTypeStringValue = "REFERENCE_POS_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED:
		errorTypeStringValue = "REFERENCE_DIST_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND:
		errorTypeStringValue = "AREA_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND:
		errorTypeStringValue = "AREA_GLUE_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED:
		errorTypeStringValue = "AREA_GLUE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH:
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
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("Geometry", geometryGqlObject);

	gqlObject.InsertParam("__typename", QVariant("Result"));

	return true;
}


bool CResult::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("AreaResult") || (gqlObject["AreaResult"].userType() != QMetaType::QString && gqlObject["AreaResult"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaResult").toLocal8Bit().constData();)

		return false;
	}
	const QString areaResultStringValue = gqlObject["AreaResult"].toString();
	if(areaResultStringValue == "NONE"){
		AreaResult = ::sdl::V1_0::complextest::StatusCode::NONE;
	}
	else if(areaResultStringValue == "OK"){
		AreaResult = ::sdl::V1_0::complextest::StatusCode::OK;
	}
	else if(areaResultStringValue == "WARNING"){
		AreaResult = ::sdl::V1_0::complextest::StatusCode::WARNING;
	}
	else if(areaResultStringValue == "NOK"){
		AreaResult = ::sdl::V1_0::complextest::StatusCode::NOK;
	}
	else if(areaResultStringValue == "FAILED"){
		AreaResult = ::sdl::V1_0::complextest::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", areaResultStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("ExpectedMinValue") || (gqlObject["ExpectedMinValue"].userType() != QMetaType::Float && gqlObject["ExpectedMinValue"].userType() != QMetaType::Double && gqlObject["ExpectedMinValue"].userType() != QMetaType::Int && gqlObject["ExpectedMinValue"].userType() != QMetaType::UInt && gqlObject["ExpectedMinValue"].userType() != QMetaType::LongLong && gqlObject["ExpectedMinValue"].userType() != QMetaType::ULongLong && gqlObject["ExpectedMinValue"].userType() != QMetaType::Long && gqlObject["ExpectedMinValue"].userType() != QMetaType::Short && gqlObject["ExpectedMinValue"].userType() != QMetaType::ULong && gqlObject["ExpectedMinValue"].userType() != QMetaType::UShort && gqlObject["ExpectedMinValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMinValue").toLocal8Bit().constData();)

		return false;
	}
	ExpectedMinValue = gqlObject["ExpectedMinValue"].toDouble();

	if (!gqlObject.ContainsParam("ExpectedMaxValue") || (gqlObject["ExpectedMaxValue"].userType() != QMetaType::Float && gqlObject["ExpectedMaxValue"].userType() != QMetaType::Double && gqlObject["ExpectedMaxValue"].userType() != QMetaType::Int && gqlObject["ExpectedMaxValue"].userType() != QMetaType::UInt && gqlObject["ExpectedMaxValue"].userType() != QMetaType::LongLong && gqlObject["ExpectedMaxValue"].userType() != QMetaType::ULongLong && gqlObject["ExpectedMaxValue"].userType() != QMetaType::Long && gqlObject["ExpectedMaxValue"].userType() != QMetaType::Short && gqlObject["ExpectedMaxValue"].userType() != QMetaType::ULong && gqlObject["ExpectedMaxValue"].userType() != QMetaType::UShort && gqlObject["ExpectedMaxValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMaxValue").toLocal8Bit().constData();)

		return false;
	}
	ExpectedMaxValue = gqlObject["ExpectedMaxValue"].toDouble();

	if (!gqlObject.ContainsParam("MeasuredValue") || (gqlObject["MeasuredValue"].userType() != QMetaType::Float && gqlObject["MeasuredValue"].userType() != QMetaType::Double && gqlObject["MeasuredValue"].userType() != QMetaType::Int && gqlObject["MeasuredValue"].userType() != QMetaType::UInt && gqlObject["MeasuredValue"].userType() != QMetaType::LongLong && gqlObject["MeasuredValue"].userType() != QMetaType::ULongLong && gqlObject["MeasuredValue"].userType() != QMetaType::Long && gqlObject["MeasuredValue"].userType() != QMetaType::Short && gqlObject["MeasuredValue"].userType() != QMetaType::ULong && gqlObject["MeasuredValue"].userType() != QMetaType::UShort && gqlObject["MeasuredValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MeasuredValue = gqlObject["MeasuredValue"].toDouble();

	if (!gqlObject.ContainsParam("MinMeasuredValue") || (gqlObject["MinMeasuredValue"].userType() != QMetaType::Float && gqlObject["MinMeasuredValue"].userType() != QMetaType::Double && gqlObject["MinMeasuredValue"].userType() != QMetaType::Int && gqlObject["MinMeasuredValue"].userType() != QMetaType::UInt && gqlObject["MinMeasuredValue"].userType() != QMetaType::LongLong && gqlObject["MinMeasuredValue"].userType() != QMetaType::ULongLong && gqlObject["MinMeasuredValue"].userType() != QMetaType::Long && gqlObject["MinMeasuredValue"].userType() != QMetaType::Short && gqlObject["MinMeasuredValue"].userType() != QMetaType::ULong && gqlObject["MinMeasuredValue"].userType() != QMetaType::UShort && gqlObject["MinMeasuredValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MinMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MinMeasuredValue = gqlObject["MinMeasuredValue"].toDouble();

	if (!gqlObject.ContainsParam("MaxMeasuredValue") || (gqlObject["MaxMeasuredValue"].userType() != QMetaType::Float && gqlObject["MaxMeasuredValue"].userType() != QMetaType::Double && gqlObject["MaxMeasuredValue"].userType() != QMetaType::Int && gqlObject["MaxMeasuredValue"].userType() != QMetaType::UInt && gqlObject["MaxMeasuredValue"].userType() != QMetaType::LongLong && gqlObject["MaxMeasuredValue"].userType() != QMetaType::ULongLong && gqlObject["MaxMeasuredValue"].userType() != QMetaType::Long && gqlObject["MaxMeasuredValue"].userType() != QMetaType::Short && gqlObject["MaxMeasuredValue"].userType() != QMetaType::ULong && gqlObject["MaxMeasuredValue"].userType() != QMetaType::UShort && gqlObject["MaxMeasuredValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MaxMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MaxMeasuredValue = gqlObject["MaxMeasuredValue"].toDouble();

	if (!gqlObject.ContainsParam("MeanMeasuredValue") || (gqlObject["MeanMeasuredValue"].userType() != QMetaType::Float && gqlObject["MeanMeasuredValue"].userType() != QMetaType::Double && gqlObject["MeanMeasuredValue"].userType() != QMetaType::Int && gqlObject["MeanMeasuredValue"].userType() != QMetaType::UInt && gqlObject["MeanMeasuredValue"].userType() != QMetaType::LongLong && gqlObject["MeanMeasuredValue"].userType() != QMetaType::ULongLong && gqlObject["MeanMeasuredValue"].userType() != QMetaType::Long && gqlObject["MeanMeasuredValue"].userType() != QMetaType::Short && gqlObject["MeanMeasuredValue"].userType() != QMetaType::ULong && gqlObject["MeanMeasuredValue"].userType() != QMetaType::UShort && gqlObject["MeanMeasuredValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeanMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MeanMeasuredValue = gqlObject["MeanMeasuredValue"].toDouble();

	if (!gqlObject.ContainsParam("MeasurementType") || (gqlObject["MeasurementType"].userType() != QMetaType::QString && gqlObject["MeasurementType"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementType").toLocal8Bit().constData();)

		return false;
	}
	const QString measurementTypeStringValue = gqlObject["MeasurementType"].toString();
	if(measurementTypeStringValue == "NONE"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::NONE;
	}
	else if(measurementTypeStringValue == "WIDTH"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::WIDTH;
	}
	else if(measurementTypeStringValue == "BRIGHTNESS"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::BRIGHTNESS;
	}
	else if(measurementTypeStringValue == "DISTANCE"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::DISTANCE;
	}
	else if(measurementTypeStringValue == "DISPLACEMENT"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::DISPLACEMENT;
	}
	else if(measurementTypeStringValue == "HEIGHT"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::HEIGHT;
	}
	else if(measurementTypeStringValue == "SYMMETRY"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::SYMMETRY;
	}
	else if(measurementTypeStringValue == "VOLUME"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::VOLUME;
	}
	else if(measurementTypeStringValue == "VOLUMERATE"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::VOLUMERATE;
	}
	else if(measurementTypeStringValue == "LENGTH"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::LENGTH;
	}
	else if(measurementTypeStringValue == "OUT_OF_BOUND"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::OUT_OF_BOUND;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("MeasurementUnit") || (gqlObject["MeasurementUnit"].userType() != QMetaType::QString && gqlObject["MeasurementUnit"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit").toLocal8Bit().constData();)

		return false;
	}
	const QString measurementUnitStringValue = gqlObject["MeasurementUnit"].toString();
	if(measurementUnitStringValue == "NONE"){
		MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::NONE;
	}
	else if(measurementUnitStringValue == "mm"){
		MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm;
	}
	else if(measurementUnitStringValue == "mm3"){
		MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm3;
	}
	else if(measurementUnitStringValue == "mm3_mm"){
		MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm3_mm;
	}
	else if(measurementUnitStringValue == "pixel"){
		MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::pixel;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit", measurementUnitStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("Length") || (gqlObject["Length"].userType() != QMetaType::Float && gqlObject["Length"].userType() != QMetaType::Double && gqlObject["Length"].userType() != QMetaType::Int && gqlObject["Length"].userType() != QMetaType::UInt && gqlObject["Length"].userType() != QMetaType::LongLong && gqlObject["Length"].userType() != QMetaType::ULongLong && gqlObject["Length"].userType() != QMetaType::Long && gqlObject["Length"].userType() != QMetaType::Short && gqlObject["Length"].userType() != QMetaType::ULong && gqlObject["Length"].userType() != QMetaType::UShort && gqlObject["Length"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Length").toLocal8Bit().constData();)

		return false;
	}
	Length = gqlObject["Length"].toDouble();

	if (!gqlObject.ContainsParam("ErrorType") || (gqlObject["ErrorType"].userType() != QMetaType::QString && gqlObject["ErrorType"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

		return false;
	}
	const QString errorTypeStringValue = gqlObject["ErrorType"].toString();
	if(errorTypeStringValue == "OK"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::OK;
	}
	else if(errorTypeStringValue == "TOO_WIDE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE;
	}
	else if(errorTypeStringValue == "TOLERANCE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOLERANCE;
	}
	else if(errorTypeStringValue == "TOO_NARROW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW;
	}
	else if(errorTypeStringValue == "NO_DATA_GAP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP;
	}
	else if(errorTypeStringValue == "GAP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::GAP;
	}
	else if(errorTypeStringValue == "TOO_HIGH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH;
	}
	else if(errorTypeStringValue == "TOO_LOW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_LOW;
	}
	else if(errorTypeStringValue == "TOO_FEW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_FEW;
	}
	else if(errorTypeStringValue == "TOO_MUCH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER;
	}
	else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION;
	}
	else if(errorTypeStringValue == "MISSING_DOTS"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE;
	}
	else if(errorTypeStringValue == "AREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
	}
	else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME;
	}
	else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT;
	}
	else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
	}
	else if(errorTypeStringValue == "TOO_BRIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT;
	}
	else if(errorTypeStringValue == "TOO_DARK"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_DARK;
	}
	else if(errorTypeStringValue == "COUNT_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW;
	}
	else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL;
	}
	else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR;
	}
	else if(errorTypeStringValue == "COLOR_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR;
	}
	else if(errorTypeStringValue == "PATTERN_COUNT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT;
	}
	else if(errorTypeStringValue == "PATTERN_POSITION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION;
	}
	else if(errorTypeStringValue == "PATTERN_ROTATION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION;
	}
	else if(errorTypeStringValue == "CENTERLINE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CENTERLINE;
	}
	else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP;
	}
	else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP;
	}
	else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW;
	}
	else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY;
	}
	else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND;
	}
	else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF;
	}
	else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED;
	}
	else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED;
	}
	else if(errorTypeStringValue == "AREA_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("Geometry") || (gqlObject.GetParamArgumentObjectPtr("Geometry") == nullptr)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}
	Geometry = CGeometry();
	const bool isGeometryRead = Geometry->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("Geometry"));
	if (!isGeometryRead){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}

	return true;
}


bool CResult::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("AreaResult") && (gqlObject["AreaResult"].userType() == QMetaType::QString || gqlObject["AreaResult"].userType() == QMetaType::QByteArray)){
		const QString areaResultStringValue = gqlObject["AreaResult"].toString();
		if(areaResultStringValue == "NONE"){
			AreaResult = ::sdl::V1_0::complextest::StatusCode::NONE;
		}
		else if(areaResultStringValue == "OK"){
			AreaResult = ::sdl::V1_0::complextest::StatusCode::OK;
		}
		else if(areaResultStringValue == "WARNING"){
			AreaResult = ::sdl::V1_0::complextest::StatusCode::WARNING;
		}
		else if(areaResultStringValue == "NOK"){
			AreaResult = ::sdl::V1_0::complextest::StatusCode::NOK;
		}
		else if(areaResultStringValue == "FAILED"){
			AreaResult = ::sdl::V1_0::complextest::StatusCode::FAILED;
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
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::NONE;
		}
		else if(measurementTypeStringValue == "WIDTH"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::WIDTH;
		}
		else if(measurementTypeStringValue == "BRIGHTNESS"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::BRIGHTNESS;
		}
		else if(measurementTypeStringValue == "DISTANCE"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::DISTANCE;
		}
		else if(measurementTypeStringValue == "DISPLACEMENT"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::DISPLACEMENT;
		}
		else if(measurementTypeStringValue == "HEIGHT"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::HEIGHT;
		}
		else if(measurementTypeStringValue == "SYMMETRY"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::SYMMETRY;
		}
		else if(measurementTypeStringValue == "VOLUME"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::VOLUME;
		}
		else if(measurementTypeStringValue == "VOLUMERATE"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::VOLUMERATE;
		}
		else if(measurementTypeStringValue == "LENGTH"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::LENGTH;
		}
		else if(measurementTypeStringValue == "OUT_OF_BOUND"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::OUT_OF_BOUND;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("MeasurementUnit") && (gqlObject["MeasurementUnit"].userType() == QMetaType::QString || gqlObject["MeasurementUnit"].userType() == QMetaType::QByteArray)){
		const QString measurementUnitStringValue = gqlObject["MeasurementUnit"].toString();
		if(measurementUnitStringValue == "NONE"){
			MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::NONE;
		}
		else if(measurementUnitStringValue == "mm"){
			MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm;
		}
		else if(measurementUnitStringValue == "mm3"){
			MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm3;
		}
		else if(measurementUnitStringValue == "mm3_mm"){
			MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm3_mm;
		}
		else if(measurementUnitStringValue == "pixel"){
			MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::pixel;
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
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::OK;
		}
		else if(errorTypeStringValue == "TOO_WIDE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE;
		}
		else if(errorTypeStringValue == "TOLERANCE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOLERANCE;
		}
		else if(errorTypeStringValue == "TOO_NARROW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW;
		}
		else if(errorTypeStringValue == "NO_DATA_GAP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP;
		}
		else if(errorTypeStringValue == "GAP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::GAP;
		}
		else if(errorTypeStringValue == "TOO_HIGH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH;
		}
		else if(errorTypeStringValue == "TOO_LOW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_LOW;
		}
		else if(errorTypeStringValue == "TOO_FEW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_FEW;
		}
		else if(errorTypeStringValue == "TOO_MUCH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER;
		}
		else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION;
		}
		else if(errorTypeStringValue == "MISSING_DOTS"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE;
		}
		else if(errorTypeStringValue == "AREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
		}
		else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME;
		}
		else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT;
		}
		else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
		}
		else if(errorTypeStringValue == "TOO_BRIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT;
		}
		else if(errorTypeStringValue == "TOO_DARK"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_DARK;
		}
		else if(errorTypeStringValue == "COUNT_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW;
		}
		else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL;
		}
		else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR;
		}
		else if(errorTypeStringValue == "COLOR_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR;
		}
		else if(errorTypeStringValue == "PATTERN_COUNT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT;
		}
		else if(errorTypeStringValue == "PATTERN_POSITION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION;
		}
		else if(errorTypeStringValue == "PATTERN_ROTATION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION;
		}
		else if(errorTypeStringValue == "CENTERLINE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CENTERLINE;
		}
		else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP;
		}
		else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP;
		}
		else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW;
		}
		else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY;
		}
		else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND;
		}
		else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF;
		}
		else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED;
		}
		else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED;
		}
		else if(errorTypeStringValue == "AREA_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("Geometry") && (gqlObject.GetParamArgumentObjectPtr("Geometry") != nullptr)){
		Geometry = CGeometry();
		const bool isGeometryRead = Geometry->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("Geometry"));
		if (!isGeometryRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CResult::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!AreaResult){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaResult").toLocal8Bit().constData();)

		return false;
	}
	QString areaResultStringValue;
	switch (*AreaResult){
	case ::sdl::V1_0::complextest::StatusCode::NONE:
		areaResultStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::StatusCode::OK:
		areaResultStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::WARNING:
		areaResultStringValue = "WARNING";
		break;
	case ::sdl::V1_0::complextest::StatusCode::NOK:
		areaResultStringValue = "NOK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::FAILED:
		areaResultStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["AreaResult"] = QJsonValue::fromVariant(areaResultStringValue);

	if (!ExpectedMinValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMinValue").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["ExpectedMinValue"] = QJsonValue::fromVariant(*ExpectedMinValue);

	if (!ExpectedMaxValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMaxValue").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["ExpectedMaxValue"] = QJsonValue::fromVariant(*ExpectedMaxValue);

	if (!MeasuredValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["MeasuredValue"] = QJsonValue::fromVariant(*MeasuredValue);

	if (!MinMeasuredValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MinMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["MinMeasuredValue"] = QJsonValue::fromVariant(*MinMeasuredValue);

	if (!MaxMeasuredValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MaxMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["MaxMeasuredValue"] = QJsonValue::fromVariant(*MaxMeasuredValue);

	if (!MeanMeasuredValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeanMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["MeanMeasuredValue"] = QJsonValue::fromVariant(*MeanMeasuredValue);

	if (!MeasurementType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementType").toLocal8Bit().constData();)

		return false;
	}
	QString measurementTypeStringValue;
	switch (*MeasurementType){
	case ::sdl::V1_0::complextest::MeasurementType::NONE:
		measurementTypeStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::WIDTH:
		measurementTypeStringValue = "WIDTH";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::BRIGHTNESS:
		measurementTypeStringValue = "BRIGHTNESS";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::DISTANCE:
		measurementTypeStringValue = "DISTANCE";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::DISPLACEMENT:
		measurementTypeStringValue = "DISPLACEMENT";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::HEIGHT:
		measurementTypeStringValue = "HEIGHT";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::SYMMETRY:
		measurementTypeStringValue = "SYMMETRY";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::VOLUME:
		measurementTypeStringValue = "VOLUME";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::VOLUMERATE:
		measurementTypeStringValue = "VOLUMERATE";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::LENGTH:
		measurementTypeStringValue = "LENGTH";
		break;
	case ::sdl::V1_0::complextest::MeasurementType::OUT_OF_BOUND:
		measurementTypeStringValue = "OUT_OF_BOUND";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["MeasurementType"] = QJsonValue::fromVariant(measurementTypeStringValue);

	if (!MeasurementUnit){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit").toLocal8Bit().constData();)

		return false;
	}
	QString measurementUnitStringValue;
	switch (*MeasurementUnit){
	case ::sdl::V1_0::complextest::MeasurementUnit::NONE:
		measurementUnitStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::MeasurementUnit::mm:
		measurementUnitStringValue = "mm";
		break;
	case ::sdl::V1_0::complextest::MeasurementUnit::mm3:
		measurementUnitStringValue = "mm3";
		break;
	case ::sdl::V1_0::complextest::MeasurementUnit::mm3_mm:
		measurementUnitStringValue = "mm3_mm";
		break;
	case ::sdl::V1_0::complextest::MeasurementUnit::pixel:
		measurementUnitStringValue = "pixel";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["MeasurementUnit"] = QJsonValue::fromVariant(measurementUnitStringValue);

	if (!Length){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Length").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["Length"] = QJsonValue::fromVariant(*Length);

	if (!ErrorType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

		return false;
	}
	QString errorTypeStringValue;
	switch (*ErrorType){
	case ::sdl::V1_0::complextest::ErrorCode::OK:
		errorTypeStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE:
		errorTypeStringValue = "TOO_WIDE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOLERANCE:
		errorTypeStringValue = "TOLERANCE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW:
		errorTypeStringValue = "TOO_NARROW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP:
		errorTypeStringValue = "NO_DATA_GAP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::GAP:
		errorTypeStringValue = "GAP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH:
		errorTypeStringValue = "TOO_HIGH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_LOW:
		errorTypeStringValue = "TOO_LOW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_FEW:
		errorTypeStringValue = "TOO_FEW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH:
		errorTypeStringValue = "TOO_MUCH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER:
		errorTypeStringValue = "WRONG_DOT_DIAMETER";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION:
		errorTypeStringValue = "WRONG_DOT_POSITION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS:
		errorTypeStringValue = "MISSING_DOTS";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE:
		errorTypeStringValue = "WRONG_DOT_DISTANCE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE:
		errorTypeStringValue = "AREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE:
		errorTypeStringValue = "TOTALAREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE:
		errorTypeStringValue = "BORDERAREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA:
		errorTypeStringValue = "WRONG_DOT_AREA";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT:
		errorTypeStringValue = "WRONG_DOT_AREA_ONHEIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME:
		errorTypeStringValue = "WRONG_DOT_VOLUME";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT:
		errorTypeStringValue = "WRONG_DOT_HEIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR:
		errorTypeStringValue = "CONTOUR_MEASUREMENT_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT:
		errorTypeStringValue = "TOO_BRIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_DARK:
		errorTypeStringValue = "TOO_DARK";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR:
		errorTypeStringValue = "COUNT_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH:
		errorTypeStringValue = "Z_DISTANCE_HIGH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW:
		errorTypeStringValue = "Z_DISTANCE_LOW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL:
		errorTypeStringValue = "NOT_SYMMETRICAL";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR:
		errorTypeStringValue = "REFERENCE_3D_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR:
		errorTypeStringValue = "COLOR_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT:
		errorTypeStringValue = "PATTERN_COUNT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION:
		errorTypeStringValue = "PATTERN_POSITION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION:
		errorTypeStringValue = "PATTERN_ROTATION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CENTERLINE:
		errorTypeStringValue = "CENTERLINE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP:
		errorTypeStringValue = "CALIB_ERROR_COMP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP:
		errorTypeStringValue = "CALIB_INVALID_COMP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED:
		errorTypeStringValue = "CALIB_NOZZLE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND:
		errorTypeStringValue = "CONTOUR_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE:
		errorTypeStringValue = "CONTOUR_TOO_WIDE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW:
		errorTypeStringValue = "CONTOUR_TOO_NARROW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED:
		errorTypeStringValue = "CONTOUR_GLUE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY:
		errorTypeStringValue = "CONTOUR_INVALID_POLY";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND:
		errorTypeStringValue = "REFERENCE_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF:
		errorTypeStringValue = "REFERENCE_INVALID_REF";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED:
		errorTypeStringValue = "REFERENCE_POS_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED:
		errorTypeStringValue = "REFERENCE_DIST_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND:
		errorTypeStringValue = "AREA_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND:
		errorTypeStringValue = "AREA_GLUE_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED:
		errorTypeStringValue = "AREA_GLUE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH:
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
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["Geometry"] = geometryJsonObject;

	jsonObject["__typename"] = "Result";

	return true;
}


bool CResult::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("AreaResult") || ! jsonObject["AreaResult"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaResult").toLocal8Bit().constData();)

		return false;
	}
	const QString areaResultStringValue = jsonObject["AreaResult"].toString();
	if(areaResultStringValue == "NONE"){
		AreaResult = ::sdl::V1_0::complextest::StatusCode::NONE;
	}
	else if(areaResultStringValue == "OK"){
		AreaResult = ::sdl::V1_0::complextest::StatusCode::OK;
	}
	else if(areaResultStringValue == "WARNING"){
		AreaResult = ::sdl::V1_0::complextest::StatusCode::WARNING;
	}
	else if(areaResultStringValue == "NOK"){
		AreaResult = ::sdl::V1_0::complextest::StatusCode::NOK;
	}
	else if(areaResultStringValue == "FAILED"){
		AreaResult = ::sdl::V1_0::complextest::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", areaResultStringValue);)

		return false;
	}

	if (!jsonObject.contains("ExpectedMinValue") || ! jsonObject["ExpectedMinValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMinValue").toLocal8Bit().constData();)

		return false;
	}
	ExpectedMinValue = jsonObject["ExpectedMinValue"].toDouble();

	if (!jsonObject.contains("ExpectedMaxValue") || ! jsonObject["ExpectedMaxValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMaxValue").toLocal8Bit().constData();)

		return false;
	}
	ExpectedMaxValue = jsonObject["ExpectedMaxValue"].toDouble();

	if (!jsonObject.contains("MeasuredValue") || ! jsonObject["MeasuredValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MeasuredValue = jsonObject["MeasuredValue"].toDouble();

	if (!jsonObject.contains("MinMeasuredValue") || ! jsonObject["MinMeasuredValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MinMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MinMeasuredValue = jsonObject["MinMeasuredValue"].toDouble();

	if (!jsonObject.contains("MaxMeasuredValue") || ! jsonObject["MaxMeasuredValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MaxMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MaxMeasuredValue = jsonObject["MaxMeasuredValue"].toDouble();

	if (!jsonObject.contains("MeanMeasuredValue") || ! jsonObject["MeanMeasuredValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeanMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MeanMeasuredValue = jsonObject["MeanMeasuredValue"].toDouble();

	if (!jsonObject.contains("MeasurementType") || ! jsonObject["MeasurementType"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementType").toLocal8Bit().constData();)

		return false;
	}
	const QString measurementTypeStringValue = jsonObject["MeasurementType"].toString();
	if(measurementTypeStringValue == "NONE"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::NONE;
	}
	else if(measurementTypeStringValue == "WIDTH"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::WIDTH;
	}
	else if(measurementTypeStringValue == "BRIGHTNESS"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::BRIGHTNESS;
	}
	else if(measurementTypeStringValue == "DISTANCE"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::DISTANCE;
	}
	else if(measurementTypeStringValue == "DISPLACEMENT"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::DISPLACEMENT;
	}
	else if(measurementTypeStringValue == "HEIGHT"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::HEIGHT;
	}
	else if(measurementTypeStringValue == "SYMMETRY"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::SYMMETRY;
	}
	else if(measurementTypeStringValue == "VOLUME"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::VOLUME;
	}
	else if(measurementTypeStringValue == "VOLUMERATE"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::VOLUMERATE;
	}
	else if(measurementTypeStringValue == "LENGTH"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::LENGTH;
	}
	else if(measurementTypeStringValue == "OUT_OF_BOUND"){
		MeasurementType = ::sdl::V1_0::complextest::MeasurementType::OUT_OF_BOUND;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

		return false;
	}

	if (!jsonObject.contains("MeasurementUnit") || ! jsonObject["MeasurementUnit"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit").toLocal8Bit().constData();)

		return false;
	}
	const QString measurementUnitStringValue = jsonObject["MeasurementUnit"].toString();
	if(measurementUnitStringValue == "NONE"){
		MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::NONE;
	}
	else if(measurementUnitStringValue == "mm"){
		MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm;
	}
	else if(measurementUnitStringValue == "mm3"){
		MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm3;
	}
	else if(measurementUnitStringValue == "mm3_mm"){
		MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm3_mm;
	}
	else if(measurementUnitStringValue == "pixel"){
		MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::pixel;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit", measurementUnitStringValue);)

		return false;
	}

	if (!jsonObject.contains("Length") || ! jsonObject["Length"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Length").toLocal8Bit().constData();)

		return false;
	}
	Length = jsonObject["Length"].toDouble();

	if (!jsonObject.contains("ErrorType") || ! jsonObject["ErrorType"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

		return false;
	}
	const QString errorTypeStringValue = jsonObject["ErrorType"].toString();
	if(errorTypeStringValue == "OK"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::OK;
	}
	else if(errorTypeStringValue == "TOO_WIDE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE;
	}
	else if(errorTypeStringValue == "TOLERANCE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOLERANCE;
	}
	else if(errorTypeStringValue == "TOO_NARROW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW;
	}
	else if(errorTypeStringValue == "NO_DATA_GAP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP;
	}
	else if(errorTypeStringValue == "GAP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::GAP;
	}
	else if(errorTypeStringValue == "TOO_HIGH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH;
	}
	else if(errorTypeStringValue == "TOO_LOW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_LOW;
	}
	else if(errorTypeStringValue == "TOO_FEW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_FEW;
	}
	else if(errorTypeStringValue == "TOO_MUCH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER;
	}
	else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION;
	}
	else if(errorTypeStringValue == "MISSING_DOTS"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE;
	}
	else if(errorTypeStringValue == "AREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
	}
	else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME;
	}
	else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT;
	}
	else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
	}
	else if(errorTypeStringValue == "TOO_BRIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT;
	}
	else if(errorTypeStringValue == "TOO_DARK"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_DARK;
	}
	else if(errorTypeStringValue == "COUNT_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW;
	}
	else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL;
	}
	else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR;
	}
	else if(errorTypeStringValue == "COLOR_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR;
	}
	else if(errorTypeStringValue == "PATTERN_COUNT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT;
	}
	else if(errorTypeStringValue == "PATTERN_POSITION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION;
	}
	else if(errorTypeStringValue == "PATTERN_ROTATION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION;
	}
	else if(errorTypeStringValue == "CENTERLINE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CENTERLINE;
	}
	else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP;
	}
	else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP;
	}
	else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW;
	}
	else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY;
	}
	else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND;
	}
	else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF;
	}
	else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED;
	}
	else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED;
	}
	else if(errorTypeStringValue == "AREA_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	if (!jsonObject.contains("Geometry") || ! jsonObject["Geometry"].isObject()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}
	Geometry = CGeometry();
	const bool isGeometryRead = Geometry->ReadFromJsonObject(jsonObject["Geometry"].toObject());
	if (!isGeometryRead){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}

	return true;
}


bool CResult::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("AreaResult") && jsonObject["AreaResult"].isString()){
		const QString areaResultStringValue = jsonObject["AreaResult"].toString();
		if(areaResultStringValue == "NONE"){
			AreaResult = ::sdl::V1_0::complextest::StatusCode::NONE;
		}
		else if(areaResultStringValue == "OK"){
			AreaResult = ::sdl::V1_0::complextest::StatusCode::OK;
		}
		else if(areaResultStringValue == "WARNING"){
			AreaResult = ::sdl::V1_0::complextest::StatusCode::WARNING;
		}
		else if(areaResultStringValue == "NOK"){
			AreaResult = ::sdl::V1_0::complextest::StatusCode::NOK;
		}
		else if(areaResultStringValue == "FAILED"){
			AreaResult = ::sdl::V1_0::complextest::StatusCode::FAILED;
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
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::NONE;
		}
		else if(measurementTypeStringValue == "WIDTH"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::WIDTH;
		}
		else if(measurementTypeStringValue == "BRIGHTNESS"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::BRIGHTNESS;
		}
		else if(measurementTypeStringValue == "DISTANCE"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::DISTANCE;
		}
		else if(measurementTypeStringValue == "DISPLACEMENT"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::DISPLACEMENT;
		}
		else if(measurementTypeStringValue == "HEIGHT"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::HEIGHT;
		}
		else if(measurementTypeStringValue == "SYMMETRY"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::SYMMETRY;
		}
		else if(measurementTypeStringValue == "VOLUME"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::VOLUME;
		}
		else if(measurementTypeStringValue == "VOLUMERATE"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::VOLUMERATE;
		}
		else if(measurementTypeStringValue == "LENGTH"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::LENGTH;
		}
		else if(measurementTypeStringValue == "OUT_OF_BOUND"){
			MeasurementType = ::sdl::V1_0::complextest::MeasurementType::OUT_OF_BOUND;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("MeasurementUnit") && jsonObject["MeasurementUnit"].isString()){
		const QString measurementUnitStringValue = jsonObject["MeasurementUnit"].toString();
		if(measurementUnitStringValue == "NONE"){
			MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::NONE;
		}
		else if(measurementUnitStringValue == "mm"){
			MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm;
		}
		else if(measurementUnitStringValue == "mm3"){
			MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm3;
		}
		else if(measurementUnitStringValue == "mm3_mm"){
			MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::mm3_mm;
		}
		else if(measurementUnitStringValue == "pixel"){
			MeasurementUnit = ::sdl::V1_0::complextest::MeasurementUnit::pixel;
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
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::OK;
		}
		else if(errorTypeStringValue == "TOO_WIDE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE;
		}
		else if(errorTypeStringValue == "TOLERANCE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOLERANCE;
		}
		else if(errorTypeStringValue == "TOO_NARROW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW;
		}
		else if(errorTypeStringValue == "NO_DATA_GAP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP;
		}
		else if(errorTypeStringValue == "GAP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::GAP;
		}
		else if(errorTypeStringValue == "TOO_HIGH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH;
		}
		else if(errorTypeStringValue == "TOO_LOW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_LOW;
		}
		else if(errorTypeStringValue == "TOO_FEW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_FEW;
		}
		else if(errorTypeStringValue == "TOO_MUCH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER;
		}
		else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION;
		}
		else if(errorTypeStringValue == "MISSING_DOTS"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE;
		}
		else if(errorTypeStringValue == "AREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
		}
		else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME;
		}
		else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT;
		}
		else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
		}
		else if(errorTypeStringValue == "TOO_BRIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT;
		}
		else if(errorTypeStringValue == "TOO_DARK"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_DARK;
		}
		else if(errorTypeStringValue == "COUNT_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW;
		}
		else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL;
		}
		else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR;
		}
		else if(errorTypeStringValue == "COLOR_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR;
		}
		else if(errorTypeStringValue == "PATTERN_COUNT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT;
		}
		else if(errorTypeStringValue == "PATTERN_POSITION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION;
		}
		else if(errorTypeStringValue == "PATTERN_ROTATION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION;
		}
		else if(errorTypeStringValue == "CENTERLINE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CENTERLINE;
		}
		else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP;
		}
		else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP;
		}
		else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW;
		}
		else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY;
		}
		else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND;
		}
		else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF;
		}
		else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED;
		}
		else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED;
		}
		else if(errorTypeStringValue == "AREA_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("Geometry") && jsonObject["Geometry"].isObject()){
		Geometry = CGeometry();
		const bool isGeometryRead = Geometry->OptReadFromJsonObject(jsonObject["Geometry"].toObject());
		if (!isGeometryRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


// serialize methods

QByteArray CArea::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CArea::operator==(const CArea& other) const
{
	return 
				AreaName == other.AreaName &&
				Status == other.Status &&
				IconPosition == other.IconPosition &&
				ErrorType == other.ErrorType &&
				Results == other.Results;
}


bool CArea::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Area", modelIndex);

	if (!AreaName){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaName").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("AreaName", *AreaName, modelIndex);

	if (!Status){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::V1_0::complextest::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::V1_0::complextest::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::FAILED:
		statusStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("Status", statusStringValue, modelIndex);


	if (!IconPosition){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}
	::imtbase::CTreeItemModel* iconPositionNewModelPtr = model.AddTreeModel("IconPosition", modelIndex);
	const bool isIconPositionAdded = IconPosition->WriteToModel(*iconPositionNewModelPtr, 0);
	if (!isIconPositionAdded){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}

	if (!ErrorType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

		return false;
	}
	QString errorTypeStringValue;
	switch (*ErrorType){
	case ::sdl::V1_0::complextest::ErrorCode::OK:
		errorTypeStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE:
		errorTypeStringValue = "TOO_WIDE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOLERANCE:
		errorTypeStringValue = "TOLERANCE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW:
		errorTypeStringValue = "TOO_NARROW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP:
		errorTypeStringValue = "NO_DATA_GAP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::GAP:
		errorTypeStringValue = "GAP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH:
		errorTypeStringValue = "TOO_HIGH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_LOW:
		errorTypeStringValue = "TOO_LOW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_FEW:
		errorTypeStringValue = "TOO_FEW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH:
		errorTypeStringValue = "TOO_MUCH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER:
		errorTypeStringValue = "WRONG_DOT_DIAMETER";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION:
		errorTypeStringValue = "WRONG_DOT_POSITION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS:
		errorTypeStringValue = "MISSING_DOTS";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE:
		errorTypeStringValue = "WRONG_DOT_DISTANCE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE:
		errorTypeStringValue = "AREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE:
		errorTypeStringValue = "TOTALAREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE:
		errorTypeStringValue = "BORDERAREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA:
		errorTypeStringValue = "WRONG_DOT_AREA";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT:
		errorTypeStringValue = "WRONG_DOT_AREA_ONHEIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME:
		errorTypeStringValue = "WRONG_DOT_VOLUME";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT:
		errorTypeStringValue = "WRONG_DOT_HEIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR:
		errorTypeStringValue = "CONTOUR_MEASUREMENT_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT:
		errorTypeStringValue = "TOO_BRIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_DARK:
		errorTypeStringValue = "TOO_DARK";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR:
		errorTypeStringValue = "COUNT_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH:
		errorTypeStringValue = "Z_DISTANCE_HIGH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW:
		errorTypeStringValue = "Z_DISTANCE_LOW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL:
		errorTypeStringValue = "NOT_SYMMETRICAL";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR:
		errorTypeStringValue = "REFERENCE_3D_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR:
		errorTypeStringValue = "COLOR_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT:
		errorTypeStringValue = "PATTERN_COUNT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION:
		errorTypeStringValue = "PATTERN_POSITION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION:
		errorTypeStringValue = "PATTERN_ROTATION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CENTERLINE:
		errorTypeStringValue = "CENTERLINE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP:
		errorTypeStringValue = "CALIB_ERROR_COMP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP:
		errorTypeStringValue = "CALIB_INVALID_COMP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED:
		errorTypeStringValue = "CALIB_NOZZLE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND:
		errorTypeStringValue = "CONTOUR_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE:
		errorTypeStringValue = "CONTOUR_TOO_WIDE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW:
		errorTypeStringValue = "CONTOUR_TOO_NARROW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED:
		errorTypeStringValue = "CONTOUR_GLUE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY:
		errorTypeStringValue = "CONTOUR_INVALID_POLY";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND:
		errorTypeStringValue = "REFERENCE_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF:
		errorTypeStringValue = "REFERENCE_INVALID_REF";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED:
		errorTypeStringValue = "REFERENCE_POS_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED:
		errorTypeStringValue = "REFERENCE_DIST_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND:
		errorTypeStringValue = "AREA_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND:
		errorTypeStringValue = "AREA_GLUE_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED:
		errorTypeStringValue = "AREA_GLUE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH:
		errorTypeStringValue = "AREA_GAP_LENGTH";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("ErrorType", errorTypeStringValue, modelIndex);

	if (!Results){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

		return false;
	}
	::imtbase::CTreeItemModel* newResultsModelPtr = model.AddTreeModel("Results", modelIndex);
	newResultsModelPtr->setIsArray(true);
	for (qsizetype resultsIndex = 0; resultsIndex < Results->size(); ++resultsIndex){
		newResultsModelPtr->InsertNewItem();
		if (!(Results->at(resultsIndex)->WriteToModel(*newResultsModelPtr, resultsIndex))){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CArea::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant areaNameData = model.GetData("AreaName", modelIndex);
	if (areaNameData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "AreaName").toLocal8Bit().constData();)

		return false;
	}
	AreaName = areaNameData.toString();

	QVariant statusData = model.GetData("Status", modelIndex);
	if (statusData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	QString statusStringValue = statusData.toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::V1_0::complextest::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::V1_0::complextest::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::V1_0::complextest::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* iconPositionDataModelPtr = model.GetTreeItemModel("IconPosition", modelIndex);
	if (iconPositionDataModelPtr == nullptr){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is missing, but required").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}
	IconPosition = CPoint();
	const bool isIconPositionRead = IconPosition->ReadFromModel(*iconPositionDataModelPtr, modelIndex);
	if (!isIconPositionRead){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}


	QVariant errorTypeData = model.GetData("ErrorType", modelIndex);
	if (errorTypeData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

		return false;
	}
	QString errorTypeStringValue = errorTypeData.toString();
	if(errorTypeStringValue == "OK"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::OK;
	}
	else if(errorTypeStringValue == "TOO_WIDE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE;
	}
	else if(errorTypeStringValue == "TOLERANCE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOLERANCE;
	}
	else if(errorTypeStringValue == "TOO_NARROW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW;
	}
	else if(errorTypeStringValue == "NO_DATA_GAP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP;
	}
	else if(errorTypeStringValue == "GAP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::GAP;
	}
	else if(errorTypeStringValue == "TOO_HIGH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH;
	}
	else if(errorTypeStringValue == "TOO_LOW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_LOW;
	}
	else if(errorTypeStringValue == "TOO_FEW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_FEW;
	}
	else if(errorTypeStringValue == "TOO_MUCH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER;
	}
	else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION;
	}
	else if(errorTypeStringValue == "MISSING_DOTS"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE;
	}
	else if(errorTypeStringValue == "AREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
	}
	else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME;
	}
	else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT;
	}
	else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
	}
	else if(errorTypeStringValue == "TOO_BRIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT;
	}
	else if(errorTypeStringValue == "TOO_DARK"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_DARK;
	}
	else if(errorTypeStringValue == "COUNT_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW;
	}
	else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL;
	}
	else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR;
	}
	else if(errorTypeStringValue == "COLOR_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR;
	}
	else if(errorTypeStringValue == "PATTERN_COUNT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT;
	}
	else if(errorTypeStringValue == "PATTERN_POSITION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION;
	}
	else if(errorTypeStringValue == "PATTERN_ROTATION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION;
	}
	else if(errorTypeStringValue == "CENTERLINE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CENTERLINE;
	}
	else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP;
	}
	else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP;
	}
	else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW;
	}
	else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY;
	}
	else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND;
	}
	else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF;
	}
	else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED;
	}
	else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED;
	}
	else if(errorTypeStringValue == "AREA_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* resultsModel = model.GetTreeItemModel("Results", modelIndex);
	if (resultsModel == nullptr){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is missing, but required.").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

		return false;
	}
	int resultsCount = resultsModel->GetItemsCount();
	imtsdl::TElementList<CResult> resultsList;
	for (int resultsIndex = 0; resultsIndex < resultsCount; ++resultsIndex){
		CResult t_results;
		if (!t_results.ReadFromModel(*resultsModel, resultsIndex)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

			return false;
		}
		resultsList << t_results;
	}
	Results = resultsList;


	return true;
}


bool CArea::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant areaNameData = model.GetData("AreaName", modelIndex);
	if (!areaNameData.isNull()){
		AreaName = areaNameData.toString();
	}

	QVariant statusData = model.GetData("Status", modelIndex);
	if (!statusData.isNull()){
		QString statusStringValue = statusData.toString();
		if(statusStringValue == "NONE"){
			Status = ::sdl::V1_0::complextest::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::V1_0::complextest::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::V1_0::complextest::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* iconPositionDataModelPtr = model.GetTreeItemModel("IconPosition", modelIndex);
	if (iconPositionDataModelPtr != nullptr){
		IconPosition = CPoint();
		const bool isIconPositionRead = IconPosition->ReadFromModel(*iconPositionDataModelPtr, modelIndex);
		if (!isIconPositionRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

			return false;
		}
	}

	QVariant errorTypeData = model.GetData("ErrorType", modelIndex);
	if (!errorTypeData.isNull()){
		QString errorTypeStringValue = errorTypeData.toString();
		if(errorTypeStringValue == "OK"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::OK;
		}
		else if(errorTypeStringValue == "TOO_WIDE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE;
		}
		else if(errorTypeStringValue == "TOLERANCE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOLERANCE;
		}
		else if(errorTypeStringValue == "TOO_NARROW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW;
		}
		else if(errorTypeStringValue == "NO_DATA_GAP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP;
		}
		else if(errorTypeStringValue == "GAP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::GAP;
		}
		else if(errorTypeStringValue == "TOO_HIGH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH;
		}
		else if(errorTypeStringValue == "TOO_LOW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_LOW;
		}
		else if(errorTypeStringValue == "TOO_FEW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_FEW;
		}
		else if(errorTypeStringValue == "TOO_MUCH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER;
		}
		else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION;
		}
		else if(errorTypeStringValue == "MISSING_DOTS"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE;
		}
		else if(errorTypeStringValue == "AREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
		}
		else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME;
		}
		else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT;
		}
		else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
		}
		else if(errorTypeStringValue == "TOO_BRIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT;
		}
		else if(errorTypeStringValue == "TOO_DARK"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_DARK;
		}
		else if(errorTypeStringValue == "COUNT_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW;
		}
		else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL;
		}
		else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR;
		}
		else if(errorTypeStringValue == "COLOR_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR;
		}
		else if(errorTypeStringValue == "PATTERN_COUNT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT;
		}
		else if(errorTypeStringValue == "PATTERN_POSITION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION;
		}
		else if(errorTypeStringValue == "PATTERN_ROTATION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION;
		}
		else if(errorTypeStringValue == "CENTERLINE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CENTERLINE;
		}
		else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP;
		}
		else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP;
		}
		else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW;
		}
		else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY;
		}
		else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND;
		}
		else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF;
		}
		else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED;
		}
		else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED;
		}
		else if(errorTypeStringValue == "AREA_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultsModel = model.GetTreeItemModel("Results", modelIndex);
	if (resultsModel != nullptr){
		int resultsCount = resultsModel->GetItemsCount();
		imtsdl::TElementList<CResult> resultsList;
		for (int resultsIndex = 0; resultsIndex < resultsCount; ++resultsIndex){
			CResult t_results;
			if (!t_results.OptReadFromModel(*resultsModel, resultsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

				return false;
			}
			resultsList << t_results;
		}
		Results = resultsList;

	}

	return true;
}


bool CArea::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!AreaName){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaName").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("AreaName", QVariant(*AreaName));

	if (!Status){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::V1_0::complextest::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::V1_0::complextest::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::FAILED:
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
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("IconPosition", iconPositionGqlObject);

	if (!ErrorType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

		return false;
	}
	QString errorTypeStringValue;
	switch (*ErrorType){
	case ::sdl::V1_0::complextest::ErrorCode::OK:
		errorTypeStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE:
		errorTypeStringValue = "TOO_WIDE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOLERANCE:
		errorTypeStringValue = "TOLERANCE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW:
		errorTypeStringValue = "TOO_NARROW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP:
		errorTypeStringValue = "NO_DATA_GAP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::GAP:
		errorTypeStringValue = "GAP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH:
		errorTypeStringValue = "TOO_HIGH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_LOW:
		errorTypeStringValue = "TOO_LOW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_FEW:
		errorTypeStringValue = "TOO_FEW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH:
		errorTypeStringValue = "TOO_MUCH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER:
		errorTypeStringValue = "WRONG_DOT_DIAMETER";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION:
		errorTypeStringValue = "WRONG_DOT_POSITION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS:
		errorTypeStringValue = "MISSING_DOTS";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE:
		errorTypeStringValue = "WRONG_DOT_DISTANCE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE:
		errorTypeStringValue = "AREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE:
		errorTypeStringValue = "TOTALAREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE:
		errorTypeStringValue = "BORDERAREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA:
		errorTypeStringValue = "WRONG_DOT_AREA";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT:
		errorTypeStringValue = "WRONG_DOT_AREA_ONHEIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME:
		errorTypeStringValue = "WRONG_DOT_VOLUME";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT:
		errorTypeStringValue = "WRONG_DOT_HEIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR:
		errorTypeStringValue = "CONTOUR_MEASUREMENT_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT:
		errorTypeStringValue = "TOO_BRIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_DARK:
		errorTypeStringValue = "TOO_DARK";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR:
		errorTypeStringValue = "COUNT_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH:
		errorTypeStringValue = "Z_DISTANCE_HIGH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW:
		errorTypeStringValue = "Z_DISTANCE_LOW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL:
		errorTypeStringValue = "NOT_SYMMETRICAL";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR:
		errorTypeStringValue = "REFERENCE_3D_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR:
		errorTypeStringValue = "COLOR_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT:
		errorTypeStringValue = "PATTERN_COUNT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION:
		errorTypeStringValue = "PATTERN_POSITION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION:
		errorTypeStringValue = "PATTERN_ROTATION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CENTERLINE:
		errorTypeStringValue = "CENTERLINE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP:
		errorTypeStringValue = "CALIB_ERROR_COMP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP:
		errorTypeStringValue = "CALIB_INVALID_COMP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED:
		errorTypeStringValue = "CALIB_NOZZLE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND:
		errorTypeStringValue = "CONTOUR_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE:
		errorTypeStringValue = "CONTOUR_TOO_WIDE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW:
		errorTypeStringValue = "CONTOUR_TOO_NARROW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED:
		errorTypeStringValue = "CONTOUR_GLUE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY:
		errorTypeStringValue = "CONTOUR_INVALID_POLY";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND:
		errorTypeStringValue = "REFERENCE_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF:
		errorTypeStringValue = "REFERENCE_INVALID_REF";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED:
		errorTypeStringValue = "REFERENCE_POS_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED:
		errorTypeStringValue = "REFERENCE_DIST_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND:
		errorTypeStringValue = "AREA_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND:
		errorTypeStringValue = "AREA_GLUE_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED:
		errorTypeStringValue = "AREA_GLUE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH:
		errorTypeStringValue = "AREA_GAP_LENGTH";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("ErrorType", QVariant(errorTypeStringValue));

	if (!Results){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

		return false;
	}
	QList<::imtgql::CGqlParamObject> resultsDataObjectList;
	for (qsizetype resultsIndex = 0; resultsIndex < Results->size(); ++resultsIndex){
		::imtgql::CGqlParamObject newResultsGqlObject;
		if (!Results->at(resultsIndex)->WriteToGraphQlObject(newResultsGqlObject)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

			return false;
		}
		resultsDataObjectList << newResultsGqlObject;
	}
	gqlObject.InsertParam("Results", resultsDataObjectList);

	gqlObject.InsertParam("__typename", QVariant("Area"));

	return true;
}


bool CArea::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("AreaName") || (gqlObject["AreaName"].userType() != QMetaType::QString && gqlObject["AreaName"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaName").toLocal8Bit().constData();)

		return false;
	}
	AreaName = gqlObject["AreaName"].toString();

	if (!gqlObject.ContainsParam("Status") || (gqlObject["Status"].userType() != QMetaType::QString && gqlObject["Status"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	const QString statusStringValue = gqlObject["Status"].toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::V1_0::complextest::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::V1_0::complextest::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::V1_0::complextest::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("IconPosition") || (gqlObject.GetParamArgumentObjectPtr("IconPosition") == nullptr)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}
	IconPosition = CPoint();
	const bool isIconPositionRead = IconPosition->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("IconPosition"));
	if (!isIconPositionRead){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}

	if (!gqlObject.ContainsParam("ErrorType") || (gqlObject["ErrorType"].userType() != QMetaType::QString && gqlObject["ErrorType"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

		return false;
	}
	const QString errorTypeStringValue = gqlObject["ErrorType"].toString();
	if(errorTypeStringValue == "OK"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::OK;
	}
	else if(errorTypeStringValue == "TOO_WIDE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE;
	}
	else if(errorTypeStringValue == "TOLERANCE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOLERANCE;
	}
	else if(errorTypeStringValue == "TOO_NARROW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW;
	}
	else if(errorTypeStringValue == "NO_DATA_GAP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP;
	}
	else if(errorTypeStringValue == "GAP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::GAP;
	}
	else if(errorTypeStringValue == "TOO_HIGH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH;
	}
	else if(errorTypeStringValue == "TOO_LOW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_LOW;
	}
	else if(errorTypeStringValue == "TOO_FEW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_FEW;
	}
	else if(errorTypeStringValue == "TOO_MUCH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER;
	}
	else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION;
	}
	else if(errorTypeStringValue == "MISSING_DOTS"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE;
	}
	else if(errorTypeStringValue == "AREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
	}
	else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME;
	}
	else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT;
	}
	else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
	}
	else if(errorTypeStringValue == "TOO_BRIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT;
	}
	else if(errorTypeStringValue == "TOO_DARK"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_DARK;
	}
	else if(errorTypeStringValue == "COUNT_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW;
	}
	else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL;
	}
	else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR;
	}
	else if(errorTypeStringValue == "COLOR_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR;
	}
	else if(errorTypeStringValue == "PATTERN_COUNT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT;
	}
	else if(errorTypeStringValue == "PATTERN_POSITION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION;
	}
	else if(errorTypeStringValue == "PATTERN_ROTATION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION;
	}
	else if(errorTypeStringValue == "CENTERLINE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CENTERLINE;
	}
	else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP;
	}
	else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP;
	}
	else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW;
	}
	else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY;
	}
	else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND;
	}
	else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF;
	}
	else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED;
	}
	else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED;
	}
	else if(errorTypeStringValue == "AREA_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("Results")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

		return false;
	}
	const qsizetype resultsElementsCount = gqlObject.GetObjectsCount("Results");
	Results = imtsdl::TElementList<CResult>();
	for (qsizetype resultsIndex = 0; resultsIndex < resultsElementsCount; ++resultsIndex){
		const ::imtgql::CGqlParamObject* resultsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Results", resultsIndex);
		if (resultsDataObjectPtr == nullptr){
			qDebug() << "invalid type" << resultsDataObjectPtr;
			return false;
		}
		CResult tempResults;
		if (!tempResults.ReadFromGraphQlObject(*resultsDataObjectPtr)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

			return false;
		}
		Results->append(tempResults);
	}

	return true;
}


bool CArea::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("AreaName") && (gqlObject["AreaName"].userType() == QMetaType::QString || gqlObject["AreaName"].userType() == QMetaType::QByteArray)){
		AreaName = gqlObject["AreaName"].toString();
	}

	if (gqlObject.ContainsParam("Status") && (gqlObject["Status"].userType() == QMetaType::QString || gqlObject["Status"].userType() == QMetaType::QByteArray)){
		const QString statusStringValue = gqlObject["Status"].toString();
		if(statusStringValue == "NONE"){
			Status = ::sdl::V1_0::complextest::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::V1_0::complextest::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::V1_0::complextest::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("IconPosition") && (gqlObject.GetParamArgumentObjectPtr("IconPosition") != nullptr)){
		IconPosition = CPoint();
		const bool isIconPositionRead = IconPosition->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("IconPosition"));
		if (!isIconPositionRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("ErrorType") && (gqlObject["ErrorType"].userType() == QMetaType::QString || gqlObject["ErrorType"].userType() == QMetaType::QByteArray)){
		const QString errorTypeStringValue = gqlObject["ErrorType"].toString();
		if(errorTypeStringValue == "OK"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::OK;
		}
		else if(errorTypeStringValue == "TOO_WIDE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE;
		}
		else if(errorTypeStringValue == "TOLERANCE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOLERANCE;
		}
		else if(errorTypeStringValue == "TOO_NARROW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW;
		}
		else if(errorTypeStringValue == "NO_DATA_GAP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP;
		}
		else if(errorTypeStringValue == "GAP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::GAP;
		}
		else if(errorTypeStringValue == "TOO_HIGH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH;
		}
		else if(errorTypeStringValue == "TOO_LOW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_LOW;
		}
		else if(errorTypeStringValue == "TOO_FEW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_FEW;
		}
		else if(errorTypeStringValue == "TOO_MUCH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER;
		}
		else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION;
		}
		else if(errorTypeStringValue == "MISSING_DOTS"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE;
		}
		else if(errorTypeStringValue == "AREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
		}
		else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME;
		}
		else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT;
		}
		else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
		}
		else if(errorTypeStringValue == "TOO_BRIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT;
		}
		else if(errorTypeStringValue == "TOO_DARK"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_DARK;
		}
		else if(errorTypeStringValue == "COUNT_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW;
		}
		else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL;
		}
		else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR;
		}
		else if(errorTypeStringValue == "COLOR_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR;
		}
		else if(errorTypeStringValue == "PATTERN_COUNT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT;
		}
		else if(errorTypeStringValue == "PATTERN_POSITION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION;
		}
		else if(errorTypeStringValue == "PATTERN_ROTATION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION;
		}
		else if(errorTypeStringValue == "CENTERLINE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CENTERLINE;
		}
		else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP;
		}
		else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP;
		}
		else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW;
		}
		else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY;
		}
		else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND;
		}
		else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF;
		}
		else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED;
		}
		else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED;
		}
		else if(errorTypeStringValue == "AREA_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("Results")){
		Results.emplace();
	}
	if (gqlObject.ContainsParam("Results") && (gqlObject.GetObjectsCount("Results") > 0)){
		const qsizetype resultsElementsCount = gqlObject.GetObjectsCount("Results");
		Results = imtsdl::TElementList<CResult>();
		for (qsizetype resultsIndex = 0; resultsIndex < resultsElementsCount; ++resultsIndex){
			const ::imtgql::CGqlParamObject* resultsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Results", resultsIndex);
			if (resultsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << resultsDataObjectPtr;
				return false;
			}
			CResult tempResults;
			if (!tempResults.OptReadFromGraphQlObject(*resultsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

				return false;
			}
			Results->append(tempResults);
		}
	}

	return true;
}


bool CArea::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!AreaName){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaName").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["AreaName"] = QJsonValue::fromVariant(*AreaName);

	if (!Status){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::V1_0::complextest::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::V1_0::complextest::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::FAILED:
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
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["IconPosition"] = iconPositionJsonObject;

	if (!ErrorType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

		return false;
	}
	QString errorTypeStringValue;
	switch (*ErrorType){
	case ::sdl::V1_0::complextest::ErrorCode::OK:
		errorTypeStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE:
		errorTypeStringValue = "TOO_WIDE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOLERANCE:
		errorTypeStringValue = "TOLERANCE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW:
		errorTypeStringValue = "TOO_NARROW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP:
		errorTypeStringValue = "NO_DATA_GAP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::GAP:
		errorTypeStringValue = "GAP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH:
		errorTypeStringValue = "TOO_HIGH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_LOW:
		errorTypeStringValue = "TOO_LOW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_FEW:
		errorTypeStringValue = "TOO_FEW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH:
		errorTypeStringValue = "TOO_MUCH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER:
		errorTypeStringValue = "WRONG_DOT_DIAMETER";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION:
		errorTypeStringValue = "WRONG_DOT_POSITION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS:
		errorTypeStringValue = "MISSING_DOTS";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE:
		errorTypeStringValue = "WRONG_DOT_DISTANCE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE:
		errorTypeStringValue = "AREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE:
		errorTypeStringValue = "TOTALAREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE:
		errorTypeStringValue = "BORDERAREA_TOO_LARGE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA:
		errorTypeStringValue = "WRONG_DOT_AREA";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT:
		errorTypeStringValue = "WRONG_DOT_AREA_ONHEIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME:
		errorTypeStringValue = "WRONG_DOT_VOLUME";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT:
		errorTypeStringValue = "WRONG_DOT_HEIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR:
		errorTypeStringValue = "CONTOUR_MEASUREMENT_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT:
		errorTypeStringValue = "TOO_BRIGHT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::TOO_DARK:
		errorTypeStringValue = "TOO_DARK";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR:
		errorTypeStringValue = "COUNT_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH:
		errorTypeStringValue = "Z_DISTANCE_HIGH";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW:
		errorTypeStringValue = "Z_DISTANCE_LOW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL:
		errorTypeStringValue = "NOT_SYMMETRICAL";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR:
		errorTypeStringValue = "REFERENCE_3D_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR:
		errorTypeStringValue = "COLOR_ERROR";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT:
		errorTypeStringValue = "PATTERN_COUNT";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION:
		errorTypeStringValue = "PATTERN_POSITION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION:
		errorTypeStringValue = "PATTERN_ROTATION";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CENTERLINE:
		errorTypeStringValue = "CENTERLINE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP:
		errorTypeStringValue = "CALIB_ERROR_COMP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP:
		errorTypeStringValue = "CALIB_INVALID_COMP";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED:
		errorTypeStringValue = "CALIB_NOZZLE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND:
		errorTypeStringValue = "CONTOUR_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE:
		errorTypeStringValue = "CONTOUR_TOO_WIDE";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW:
		errorTypeStringValue = "CONTOUR_TOO_NARROW";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED:
		errorTypeStringValue = "CONTOUR_GLUE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY:
		errorTypeStringValue = "CONTOUR_INVALID_POLY";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND:
		errorTypeStringValue = "REFERENCE_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF:
		errorTypeStringValue = "REFERENCE_INVALID_REF";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED:
		errorTypeStringValue = "REFERENCE_POS_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED:
		errorTypeStringValue = "REFERENCE_DIST_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND:
		errorTypeStringValue = "AREA_NOT_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND:
		errorTypeStringValue = "AREA_GLUE_FOUND";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED:
		errorTypeStringValue = "AREA_GLUE_MOVED";
		break;
	case ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH:
		errorTypeStringValue = "AREA_GAP_LENGTH";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["ErrorType"] = QJsonValue::fromVariant(errorTypeStringValue);

	if (!Results){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

		return false;
	}
	QJsonArray newResultsArray;
	for (qsizetype resultsIndex = 0; resultsIndex < Results->size(); ++resultsIndex){
		QJsonObject newResultsJsonObject;
		if (!Results->at(resultsIndex)->WriteToJsonObject(newResultsJsonObject)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

			return false;
		}
		newResultsArray << newResultsJsonObject;
	}
	jsonObject["Results"] = newResultsArray;

	jsonObject["__typename"] = "Area";

	return true;
}


bool CArea::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("AreaName") || ! jsonObject["AreaName"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaName").toLocal8Bit().constData();)

		return false;
	}
	AreaName = jsonObject["AreaName"].toString();

	if (!jsonObject.contains("Status") || ! jsonObject["Status"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	const QString statusStringValue = jsonObject["Status"].toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::V1_0::complextest::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::V1_0::complextest::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::V1_0::complextest::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (!jsonObject.contains("IconPosition") || ! jsonObject["IconPosition"].isObject()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}
	IconPosition = CPoint();
	const bool isIconPositionRead = IconPosition->ReadFromJsonObject(jsonObject["IconPosition"].toObject());
	if (!isIconPositionRead){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}

	if (!jsonObject.contains("ErrorType") || ! jsonObject["ErrorType"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

		return false;
	}
	const QString errorTypeStringValue = jsonObject["ErrorType"].toString();
	if(errorTypeStringValue == "OK"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::OK;
	}
	else if(errorTypeStringValue == "TOO_WIDE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE;
	}
	else if(errorTypeStringValue == "TOLERANCE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOLERANCE;
	}
	else if(errorTypeStringValue == "TOO_NARROW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW;
	}
	else if(errorTypeStringValue == "NO_DATA_GAP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP;
	}
	else if(errorTypeStringValue == "GAP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::GAP;
	}
	else if(errorTypeStringValue == "TOO_HIGH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH;
	}
	else if(errorTypeStringValue == "TOO_LOW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_LOW;
	}
	else if(errorTypeStringValue == "TOO_FEW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_FEW;
	}
	else if(errorTypeStringValue == "TOO_MUCH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER;
	}
	else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION;
	}
	else if(errorTypeStringValue == "MISSING_DOTS"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS;
	}
	else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE;
	}
	else if(errorTypeStringValue == "AREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA;
	}
	else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
	}
	else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME;
	}
	else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT;
	}
	else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
	}
	else if(errorTypeStringValue == "TOO_BRIGHT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT;
	}
	else if(errorTypeStringValue == "TOO_DARK"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_DARK;
	}
	else if(errorTypeStringValue == "COUNT_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH;
	}
	else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW;
	}
	else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL;
	}
	else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR;
	}
	else if(errorTypeStringValue == "COLOR_ERROR"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR;
	}
	else if(errorTypeStringValue == "PATTERN_COUNT"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT;
	}
	else if(errorTypeStringValue == "PATTERN_POSITION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION;
	}
	else if(errorTypeStringValue == "PATTERN_ROTATION"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION;
	}
	else if(errorTypeStringValue == "CENTERLINE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CENTERLINE;
	}
	else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP;
	}
	else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP;
	}
	else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE;
	}
	else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW;
	}
	else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY;
	}
	else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND;
	}
	else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF;
	}
	else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED;
	}
	else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED;
	}
	else if(errorTypeStringValue == "AREA_NOT_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND;
	}
	else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED;
	}
	else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
		ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	if (!jsonObject.contains("Results") || ! jsonObject["Results"].isArray()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

		return false;
	}
	const QJsonArray resultsJsonArray = jsonObject["Results"].toArray();
	const qsizetype resultsArrayCount = resultsJsonArray.size();
	Results = imtsdl::TElementList<CResult>();
	for (qsizetype resultsIndex = 0; resultsIndex < resultsArrayCount; ++resultsIndex){
		CResult tempResults;
		if (!tempResults.ReadFromJsonObject(resultsJsonArray[resultsIndex].toObject())){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

			return false;
		}
		Results->append(tempResults);
	}

	return true;
}


bool CArea::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("AreaName") && jsonObject["AreaName"].isString()){
		AreaName = jsonObject["AreaName"].toString();
	}

	if (jsonObject.contains("Status") && jsonObject["Status"].isString()){
		const QString statusStringValue = jsonObject["Status"].toString();
		if(statusStringValue == "NONE"){
			Status = ::sdl::V1_0::complextest::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::V1_0::complextest::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::V1_0::complextest::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("IconPosition") && jsonObject["IconPosition"].isObject()){
		IconPosition = CPoint();
		const bool isIconPositionRead = IconPosition->OptReadFromJsonObject(jsonObject["IconPosition"].toObject());
		if (!isIconPositionRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("ErrorType") && jsonObject["ErrorType"].isString()){
		const QString errorTypeStringValue = jsonObject["ErrorType"].toString();
		if(errorTypeStringValue == "OK"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::OK;
		}
		else if(errorTypeStringValue == "TOO_WIDE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_WIDE;
		}
		else if(errorTypeStringValue == "TOLERANCE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOLERANCE;
		}
		else if(errorTypeStringValue == "TOO_NARROW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_NARROW;
		}
		else if(errorTypeStringValue == "NO_DATA_GAP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::NO_DATA_GAP;
		}
		else if(errorTypeStringValue == "GAP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::GAP;
		}
		else if(errorTypeStringValue == "TOO_HIGH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_HIGH;
		}
		else if(errorTypeStringValue == "TOO_LOW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_LOW;
		}
		else if(errorTypeStringValue == "TOO_FEW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_FEW;
		}
		else if(errorTypeStringValue == "TOO_MUCH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_MUCH;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DIAMETER"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DIAMETER;
		}
		else if(errorTypeStringValue == "WRONG_DOT_POSITION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_POSITION;
		}
		else if(errorTypeStringValue == "MISSING_DOTS"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::MISSING_DOTS;
		}
		else if(errorTypeStringValue == "WRONG_DOT_DISTANCE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_DISTANCE;
		}
		else if(errorTypeStringValue == "AREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "TOTALAREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOTALAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "BORDERAREA_TOO_LARGE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::BORDERAREA_TOO_LARGE;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA;
		}
		else if(errorTypeStringValue == "WRONG_DOT_AREA_ONHEIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_AREA_ONHEIGHT;
		}
		else if(errorTypeStringValue == "WRONG_DOT_VOLUME"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_VOLUME;
		}
		else if(errorTypeStringValue == "WRONG_DOT_HEIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::WRONG_DOT_HEIGHT;
		}
		else if(errorTypeStringValue == "CONTOUR_MEASUREMENT_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_MEASUREMENT_ERROR;
		}
		else if(errorTypeStringValue == "TOO_BRIGHT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_BRIGHT;
		}
		else if(errorTypeStringValue == "TOO_DARK"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::TOO_DARK;
		}
		else if(errorTypeStringValue == "COUNT_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::COUNT_ERROR;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_HIGH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_HIGH;
		}
		else if(errorTypeStringValue == "Z_DISTANCE_LOW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::Z_DISTANCE_LOW;
		}
		else if(errorTypeStringValue == "NOT_SYMMETRICAL"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::NOT_SYMMETRICAL;
		}
		else if(errorTypeStringValue == "REFERENCE_3D_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_3D_ERROR;
		}
		else if(errorTypeStringValue == "COLOR_ERROR"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::COLOR_ERROR;
		}
		else if(errorTypeStringValue == "PATTERN_COUNT"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_COUNT;
		}
		else if(errorTypeStringValue == "PATTERN_POSITION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_POSITION;
		}
		else if(errorTypeStringValue == "PATTERN_ROTATION"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::PATTERN_ROTATION;
		}
		else if(errorTypeStringValue == "CENTERLINE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CENTERLINE;
		}
		else if(errorTypeStringValue == "CALIB_ERROR_COMP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_ERROR_COMP;
		}
		else if(errorTypeStringValue == "CALIB_INVALID_COMP"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_INVALID_COMP;
		}
		else if(errorTypeStringValue == "CALIB_NOZZLE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CALIB_NOZZLE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_NOT_FOUND;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_WIDE"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_WIDE;
		}
		else if(errorTypeStringValue == "CONTOUR_TOO_NARROW"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_TOO_NARROW;
		}
		else if(errorTypeStringValue == "CONTOUR_GLUE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "CONTOUR_INVALID_POLY"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::CONTOUR_INVALID_POLY;
		}
		else if(errorTypeStringValue == "REFERENCE_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_NOT_FOUND;
		}
		else if(errorTypeStringValue == "REFERENCE_INVALID_REF"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_INVALID_REF;
		}
		else if(errorTypeStringValue == "REFERENCE_POS_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_POS_MOVED;
		}
		else if(errorTypeStringValue == "REFERENCE_DIST_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::REFERENCE_DIST_MOVED;
		}
		else if(errorTypeStringValue == "AREA_NOT_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_NOT_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_FOUND"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_FOUND;
		}
		else if(errorTypeStringValue == "AREA_GLUE_MOVED"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GLUE_MOVED;
		}
		else if(errorTypeStringValue == "AREA_GAP_LENGTH"){
			ErrorType = ::sdl::V1_0::complextest::ErrorCode::AREA_GAP_LENGTH;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("Results") && jsonObject["Results"].isArray()){
		const QJsonArray resultsJsonArray = jsonObject["Results"].toArray();
		const qsizetype resultsArrayCount = resultsJsonArray.size();
		Results = imtsdl::TElementList<CResult>();
		for (qsizetype resultsIndex = 0; resultsIndex < resultsArrayCount; ++resultsIndex){
			CResult tempResults;
			if (!tempResults.OptReadFromJsonObject(resultsJsonArray[resultsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

				return false;
			}
			Results->append(tempResults);
		}
	}

	return true;
}


// serialize methods

QByteArray CAnalyzer::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CAnalyzer::operator==(const CAnalyzer& other) const
{
	return 
				AnalyzerName == other.AnalyzerName &&
				AnalyzerResult == other.AnalyzerResult &&
				Areas == other.Areas;
}


bool CAnalyzer::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Analyzer", modelIndex);

	if (!AnalyzerName){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerName").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("AnalyzerName", *AnalyzerName, modelIndex);

	if (!AnalyzerResult){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerResult").toLocal8Bit().constData();)

		return false;
	}
	QString analyzerResultStringValue;
	switch (*AnalyzerResult){
	case ::sdl::V1_0::complextest::StatusCode::NONE:
		analyzerResultStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::StatusCode::OK:
		analyzerResultStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::WARNING:
		analyzerResultStringValue = "WARNING";
		break;
	case ::sdl::V1_0::complextest::StatusCode::NOK:
		analyzerResultStringValue = "NOK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::FAILED:
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
			if (!(Areas->at(areasIndex)->WriteToModel(*newAreasModelPtr, areasIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
		}
	}

	return true;
}


bool CAnalyzer::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant analyzerNameData = model.GetData("AnalyzerName", modelIndex);
	if (analyzerNameData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerName").toLocal8Bit().constData();)

		return false;
	}
	AnalyzerName = analyzerNameData.toString();

	QVariant analyzerResultData = model.GetData("AnalyzerResult", modelIndex);
	if (analyzerResultData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerResult").toLocal8Bit().constData();)

		return false;
	}
	QString analyzerResultStringValue = analyzerResultData.toString();
	if(analyzerResultStringValue == "NONE"){
		AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::NONE;
	}
	else if(analyzerResultStringValue == "OK"){
		AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::OK;
	}
	else if(analyzerResultStringValue == "WARNING"){
		AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::WARNING;
	}
	else if(analyzerResultStringValue == "NOK"){
		AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::NOK;
	}
	else if(analyzerResultStringValue == "FAILED"){
		AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* areasModel = model.GetTreeItemModel("Areas", modelIndex);
	if (areasModel != nullptr){
		int areasCount = areasModel->GetItemsCount();
		imtsdl::TElementList<CArea> areasList;
		for (int areasIndex = 0; areasIndex < areasCount; ++areasIndex){
			CArea t_areas;
			if (!t_areas.ReadFromModel(*areasModel, areasIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
			areasList << t_areas;
		}
		Areas = areasList;

	}

	return true;
}


bool CAnalyzer::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant analyzerNameData = model.GetData("AnalyzerName", modelIndex);
	if (!analyzerNameData.isNull()){
		AnalyzerName = analyzerNameData.toString();
	}

	QVariant analyzerResultData = model.GetData("AnalyzerResult", modelIndex);
	if (!analyzerResultData.isNull()){
		QString analyzerResultStringValue = analyzerResultData.toString();
		if(analyzerResultStringValue == "NONE"){
			AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::NONE;
		}
		else if(analyzerResultStringValue == "OK"){
			AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::OK;
		}
		else if(analyzerResultStringValue == "WARNING"){
			AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::WARNING;
		}
		else if(analyzerResultStringValue == "NOK"){
			AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::NOK;
		}
		else if(analyzerResultStringValue == "FAILED"){
			AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* areasModel = model.GetTreeItemModel("Areas", modelIndex);
	if (areasModel != nullptr){
		int areasCount = areasModel->GetItemsCount();
		imtsdl::TElementList<CArea> areasList;
		for (int areasIndex = 0; areasIndex < areasCount; ++areasIndex){
			CArea t_areas;
			if (!t_areas.OptReadFromModel(*areasModel, areasIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
			areasList << t_areas;
		}
		Areas = areasList;

	}

	return true;
}


bool CAnalyzer::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!AnalyzerName){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerName").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("AnalyzerName", QVariant(*AnalyzerName));

	if (!AnalyzerResult){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerResult").toLocal8Bit().constData();)

		return false;
	}
	QString analyzerResultStringValue;
	switch (*AnalyzerResult){
	case ::sdl::V1_0::complextest::StatusCode::NONE:
		analyzerResultStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::StatusCode::OK:
		analyzerResultStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::WARNING:
		analyzerResultStringValue = "WARNING";
		break;
	case ::sdl::V1_0::complextest::StatusCode::NOK:
		analyzerResultStringValue = "NOK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::FAILED:
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
			if (!Areas->at(areasIndex)->WriteToGraphQlObject(newAreasGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
			areasDataObjectList << newAreasGqlObject;
		}
		gqlObject.InsertParam("Areas", areasDataObjectList);
	}

	gqlObject.InsertParam("__typename", QVariant("Analyzer"));

	return true;
}


bool CAnalyzer::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("AnalyzerName") || (gqlObject["AnalyzerName"].userType() != QMetaType::QString && gqlObject["AnalyzerName"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerName").toLocal8Bit().constData();)

		return false;
	}
	AnalyzerName = gqlObject["AnalyzerName"].toString();

	if (!gqlObject.ContainsParam("AnalyzerResult") || (gqlObject["AnalyzerResult"].userType() != QMetaType::QString && gqlObject["AnalyzerResult"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerResult").toLocal8Bit().constData();)

		return false;
	}
	const QString analyzerResultStringValue = gqlObject["AnalyzerResult"].toString();
	if(analyzerResultStringValue == "NONE"){
		AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::NONE;
	}
	else if(analyzerResultStringValue == "OK"){
		AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::OK;
	}
	else if(analyzerResultStringValue == "WARNING"){
		AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::WARNING;
	}
	else if(analyzerResultStringValue == "NOK"){
		AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::NOK;
	}
	else if(analyzerResultStringValue == "FAILED"){
		AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

		return false;
	}

	if (gqlObject.ContainsParam("Areas")){
		Areas.emplace();
	}
	if (gqlObject.ContainsParam("Areas") && (gqlObject.GetObjectsCount("Areas") > 0)){
		const qsizetype areasElementsCount = gqlObject.GetObjectsCount("Areas");
		Areas = imtsdl::TElementList<CArea>();
		for (qsizetype areasIndex = 0; areasIndex < areasElementsCount; ++areasIndex){
			const ::imtgql::CGqlParamObject* areasDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Areas", areasIndex);
			if (areasDataObjectPtr == nullptr){
				qDebug() << "invalid type" << areasDataObjectPtr;
				return false;
			}
			CArea tempAreas;
			if (!tempAreas.ReadFromGraphQlObject(*areasDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
			Areas->append(tempAreas);
		}
	}

	return true;
}


bool CAnalyzer::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("AnalyzerName") && (gqlObject["AnalyzerName"].userType() == QMetaType::QString || gqlObject["AnalyzerName"].userType() == QMetaType::QByteArray)){
		AnalyzerName = gqlObject["AnalyzerName"].toString();
	}

	if (gqlObject.ContainsParam("AnalyzerResult") && (gqlObject["AnalyzerResult"].userType() == QMetaType::QString || gqlObject["AnalyzerResult"].userType() == QMetaType::QByteArray)){
		const QString analyzerResultStringValue = gqlObject["AnalyzerResult"].toString();
		if(analyzerResultStringValue == "NONE"){
			AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::NONE;
		}
		else if(analyzerResultStringValue == "OK"){
			AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::OK;
		}
		else if(analyzerResultStringValue == "WARNING"){
			AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::WARNING;
		}
		else if(analyzerResultStringValue == "NOK"){
			AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::NOK;
		}
		else if(analyzerResultStringValue == "FAILED"){
			AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("Areas")){
		Areas.emplace();
	}
	if (gqlObject.ContainsParam("Areas") && (gqlObject.GetObjectsCount("Areas") > 0)){
		const qsizetype areasElementsCount = gqlObject.GetObjectsCount("Areas");
		Areas = imtsdl::TElementList<CArea>();
		for (qsizetype areasIndex = 0; areasIndex < areasElementsCount; ++areasIndex){
			const ::imtgql::CGqlParamObject* areasDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Areas", areasIndex);
			if (areasDataObjectPtr == nullptr){
				qDebug() << "invalid type" << areasDataObjectPtr;
				return false;
			}
			CArea tempAreas;
			if (!tempAreas.OptReadFromGraphQlObject(*areasDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
			Areas->append(tempAreas);
		}
	}

	return true;
}


bool CAnalyzer::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!AnalyzerName){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerName").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["AnalyzerName"] = QJsonValue::fromVariant(*AnalyzerName);

	if (!AnalyzerResult){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerResult").toLocal8Bit().constData();)

		return false;
	}
	QString analyzerResultStringValue;
	switch (*AnalyzerResult){
	case ::sdl::V1_0::complextest::StatusCode::NONE:
		analyzerResultStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::StatusCode::OK:
		analyzerResultStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::WARNING:
		analyzerResultStringValue = "WARNING";
		break;
	case ::sdl::V1_0::complextest::StatusCode::NOK:
		analyzerResultStringValue = "NOK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::FAILED:
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
			if (!Areas->at(areasIndex)->WriteToJsonObject(newAreasJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
			newAreasArray << newAreasJsonObject;
		}
		jsonObject["Areas"] = newAreasArray;
	}

	jsonObject["__typename"] = "Analyzer";

	return true;
}


bool CAnalyzer::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("AnalyzerName") || ! jsonObject["AnalyzerName"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerName").toLocal8Bit().constData();)

		return false;
	}
	AnalyzerName = jsonObject["AnalyzerName"].toString();

	if (!jsonObject.contains("AnalyzerResult") || ! jsonObject["AnalyzerResult"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerResult").toLocal8Bit().constData();)

		return false;
	}
	const QString analyzerResultStringValue = jsonObject["AnalyzerResult"].toString();
	if(analyzerResultStringValue == "NONE"){
		AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::NONE;
	}
	else if(analyzerResultStringValue == "OK"){
		AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::OK;
	}
	else if(analyzerResultStringValue == "WARNING"){
		AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::WARNING;
	}
	else if(analyzerResultStringValue == "NOK"){
		AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::NOK;
	}
	else if(analyzerResultStringValue == "FAILED"){
		AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

		return false;
	}

	if (jsonObject.contains("Areas") && jsonObject["Areas"].isArray()){
		const QJsonArray areasJsonArray = jsonObject["Areas"].toArray();
		const qsizetype areasArrayCount = areasJsonArray.size();
		Areas = imtsdl::TElementList<CArea>();
		for (qsizetype areasIndex = 0; areasIndex < areasArrayCount; ++areasIndex){
			CArea tempAreas;
			if (!tempAreas.ReadFromJsonObject(areasJsonArray[areasIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
			Areas->append(tempAreas);
		}
	}

	return true;
}


bool CAnalyzer::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("AnalyzerName") && jsonObject["AnalyzerName"].isString()){
		AnalyzerName = jsonObject["AnalyzerName"].toString();
	}

	if (jsonObject.contains("AnalyzerResult") && jsonObject["AnalyzerResult"].isString()){
		const QString analyzerResultStringValue = jsonObject["AnalyzerResult"].toString();
		if(analyzerResultStringValue == "NONE"){
			AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::NONE;
		}
		else if(analyzerResultStringValue == "OK"){
			AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::OK;
		}
		else if(analyzerResultStringValue == "WARNING"){
			AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::WARNING;
		}
		else if(analyzerResultStringValue == "NOK"){
			AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::NOK;
		}
		else if(analyzerResultStringValue == "FAILED"){
			AnalyzerResult = ::sdl::V1_0::complextest::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("Areas") && jsonObject["Areas"].isArray()){
		const QJsonArray areasJsonArray = jsonObject["Areas"].toArray();
		const qsizetype areasArrayCount = areasJsonArray.size();
		Areas = imtsdl::TElementList<CArea>();
		for (qsizetype areasIndex = 0; areasIndex < areasArrayCount; ++areasIndex){
			CArea tempAreas;
			if (!tempAreas.OptReadFromJsonObject(areasJsonArray[areasIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
			Areas->append(tempAreas);
		}
	}

	return true;
}


// serialize methods

QByteArray CInspection::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CInspection::operator==(const CInspection& other) const
{
	return 
				ID == other.ID &&
				TypeID == other.TypeID &&
				Name == other.Name &&
				Status == other.Status &&
				Analyzers == other.Analyzers;
}


bool CInspection::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Inspection", modelIndex);

	if (!ID){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ID").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("ID", *ID, modelIndex);

	if (!TypeID){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "TypeID").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("TypeID", *TypeID, modelIndex);

	if (!Name){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("Name", *Name, modelIndex);

	if (!Status){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::V1_0::complextest::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::V1_0::complextest::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::FAILED:
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
			if (!(Analyzers->at(analyzersIndex)->WriteToModel(*newAnalyzersModelPtr, analyzersIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
		}
	}

	return true;
}


bool CInspection::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant iDData = model.GetData("ID", modelIndex);
	if (iDData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ID").toLocal8Bit().constData();)

		return false;
	}
	ID = iDData.toString();

	QVariant typeIDData = model.GetData("TypeID", modelIndex);
	if (typeIDData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "TypeID").toLocal8Bit().constData();)

		return false;
	}
	TypeID = typeIDData.toString();

	QVariant nameData = model.GetData("Name", modelIndex);
	if (nameData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	Name = nameData.toString();

	QVariant statusData = model.GetData("Status", modelIndex);
	if (statusData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	QString statusStringValue = statusData.toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::V1_0::complextest::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::V1_0::complextest::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::V1_0::complextest::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* analyzersModel = model.GetTreeItemModel("Analyzers", modelIndex);
	if (analyzersModel != nullptr){
		int analyzersCount = analyzersModel->GetItemsCount();
		imtsdl::TElementList<CAnalyzer> analyzersList;
		for (int analyzersIndex = 0; analyzersIndex < analyzersCount; ++analyzersIndex){
			CAnalyzer t_analyzers;
			if (!t_analyzers.ReadFromModel(*analyzersModel, analyzersIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
			analyzersList << t_analyzers;
		}
		Analyzers = analyzersList;

	}

	return true;
}


bool CInspection::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
			Status = ::sdl::V1_0::complextest::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::V1_0::complextest::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::V1_0::complextest::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* analyzersModel = model.GetTreeItemModel("Analyzers", modelIndex);
	if (analyzersModel != nullptr){
		int analyzersCount = analyzersModel->GetItemsCount();
		imtsdl::TElementList<CAnalyzer> analyzersList;
		for (int analyzersIndex = 0; analyzersIndex < analyzersCount; ++analyzersIndex){
			CAnalyzer t_analyzers;
			if (!t_analyzers.OptReadFromModel(*analyzersModel, analyzersIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
			analyzersList << t_analyzers;
		}
		Analyzers = analyzersList;

	}

	return true;
}


bool CInspection::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!ID){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ID").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("ID", QVariant(*ID));

	if (!TypeID){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "TypeID").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("TypeID", QVariant(*TypeID));

	if (!Name){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("Name", QVariant(*Name));

	if (!Status){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::V1_0::complextest::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::V1_0::complextest::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::FAILED:
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
			if (!Analyzers->at(analyzersIndex)->WriteToGraphQlObject(newAnalyzersGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
			analyzersDataObjectList << newAnalyzersGqlObject;
		}
		gqlObject.InsertParam("Analyzers", analyzersDataObjectList);
	}

	gqlObject.InsertParam("__typename", QVariant("Inspection"));

	return true;
}


bool CInspection::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("ID") || (gqlObject["ID"].userType() != QMetaType::QString && gqlObject["ID"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ID").toLocal8Bit().constData();)

		return false;
	}
	ID = gqlObject["ID"].toString();

	if (!gqlObject.ContainsParam("TypeID") || (gqlObject["TypeID"].userType() != QMetaType::QString && gqlObject["TypeID"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "TypeID").toLocal8Bit().constData();)

		return false;
	}
	TypeID = gqlObject["TypeID"].toString();

	if (!gqlObject.ContainsParam("Name") || (gqlObject["Name"].userType() != QMetaType::QString && gqlObject["Name"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	Name = gqlObject["Name"].toString();

	if (!gqlObject.ContainsParam("Status") || (gqlObject["Status"].userType() != QMetaType::QString && gqlObject["Status"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	const QString statusStringValue = gqlObject["Status"].toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::V1_0::complextest::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::V1_0::complextest::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::V1_0::complextest::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (gqlObject.ContainsParam("Analyzers")){
		Analyzers.emplace();
	}
	if (gqlObject.ContainsParam("Analyzers") && (gqlObject.GetObjectsCount("Analyzers") > 0)){
		const qsizetype analyzersElementsCount = gqlObject.GetObjectsCount("Analyzers");
		Analyzers = imtsdl::TElementList<CAnalyzer>();
		for (qsizetype analyzersIndex = 0; analyzersIndex < analyzersElementsCount; ++analyzersIndex){
			const ::imtgql::CGqlParamObject* analyzersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Analyzers", analyzersIndex);
			if (analyzersDataObjectPtr == nullptr){
				qDebug() << "invalid type" << analyzersDataObjectPtr;
				return false;
			}
			CAnalyzer tempAnalyzers;
			if (!tempAnalyzers.ReadFromGraphQlObject(*analyzersDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
			Analyzers->append(tempAnalyzers);
		}
	}

	return true;
}


bool CInspection::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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
			Status = ::sdl::V1_0::complextest::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::V1_0::complextest::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::V1_0::complextest::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("Analyzers")){
		Analyzers.emplace();
	}
	if (gqlObject.ContainsParam("Analyzers") && (gqlObject.GetObjectsCount("Analyzers") > 0)){
		const qsizetype analyzersElementsCount = gqlObject.GetObjectsCount("Analyzers");
		Analyzers = imtsdl::TElementList<CAnalyzer>();
		for (qsizetype analyzersIndex = 0; analyzersIndex < analyzersElementsCount; ++analyzersIndex){
			const ::imtgql::CGqlParamObject* analyzersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Analyzers", analyzersIndex);
			if (analyzersDataObjectPtr == nullptr){
				qDebug() << "invalid type" << analyzersDataObjectPtr;
				return false;
			}
			CAnalyzer tempAnalyzers;
			if (!tempAnalyzers.OptReadFromGraphQlObject(*analyzersDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
			Analyzers->append(tempAnalyzers);
		}
	}

	return true;
}


bool CInspection::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!ID){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ID").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["ID"] = QJsonValue::fromVariant(*ID);

	if (!TypeID){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "TypeID").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["TypeID"] = QJsonValue::fromVariant(*TypeID);

	if (!Name){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["Name"] = QJsonValue::fromVariant(*Name);

	if (!Status){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::V1_0::complextest::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::V1_0::complextest::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::FAILED:
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
			if (!Analyzers->at(analyzersIndex)->WriteToJsonObject(newAnalyzersJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
			newAnalyzersArray << newAnalyzersJsonObject;
		}
		jsonObject["Analyzers"] = newAnalyzersArray;
	}

	jsonObject["__typename"] = "Inspection";

	return true;
}


bool CInspection::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("ID") || ! jsonObject["ID"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ID").toLocal8Bit().constData();)

		return false;
	}
	ID = jsonObject["ID"].toString();

	if (!jsonObject.contains("TypeID") || ! jsonObject["TypeID"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "TypeID").toLocal8Bit().constData();)

		return false;
	}
	TypeID = jsonObject["TypeID"].toString();

	if (!jsonObject.contains("Name") || ! jsonObject["Name"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	Name = jsonObject["Name"].toString();

	if (!jsonObject.contains("Status") || ! jsonObject["Status"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	const QString statusStringValue = jsonObject["Status"].toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::V1_0::complextest::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::V1_0::complextest::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::V1_0::complextest::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (jsonObject.contains("Analyzers") && jsonObject["Analyzers"].isArray()){
		const QJsonArray analyzersJsonArray = jsonObject["Analyzers"].toArray();
		const qsizetype analyzersArrayCount = analyzersJsonArray.size();
		Analyzers = imtsdl::TElementList<CAnalyzer>();
		for (qsizetype analyzersIndex = 0; analyzersIndex < analyzersArrayCount; ++analyzersIndex){
			CAnalyzer tempAnalyzers;
			if (!tempAnalyzers.ReadFromJsonObject(analyzersJsonArray[analyzersIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
			Analyzers->append(tempAnalyzers);
		}
	}

	return true;
}


bool CInspection::OptReadFromJsonObject(const QJsonObject& jsonObject)
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
			Status = ::sdl::V1_0::complextest::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::V1_0::complextest::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::V1_0::complextest::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("Analyzers") && jsonObject["Analyzers"].isArray()){
		const QJsonArray analyzersJsonArray = jsonObject["Analyzers"].toArray();
		const qsizetype analyzersArrayCount = analyzersJsonArray.size();
		Analyzers = imtsdl::TElementList<CAnalyzer>();
		for (qsizetype analyzersIndex = 0; analyzersIndex < analyzersArrayCount; ++analyzersIndex){
			CAnalyzer tempAnalyzers;
			if (!tempAnalyzers.OptReadFromJsonObject(analyzersJsonArray[analyzersIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
			Analyzers->append(tempAnalyzers);
		}
	}

	return true;
}


// serialize methods

QByteArray CProductOverview::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CProductOverview::operator==(const CProductOverview& other) const
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


bool CProductOverview::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "ProductOverview", modelIndex);

	if (!ProductID){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductID").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("ProductID", *ProductID, modelIndex);

	if (!Name){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("Name", *Name, modelIndex);

	if (!Status){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::V1_0::complextest::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::V1_0::complextest::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::FAILED:
		statusStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("Status", statusStringValue, modelIndex);

	if (!PartSerialNumber){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "PartSerialNumber").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("PartSerialNumber", *PartSerialNumber, modelIndex);

	if (!Timestamp){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Timestamp").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("Timestamp", *Timestamp, modelIndex);

	if (!ProductImage){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductImage").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("ProductImage", *ProductImage, modelIndex);

	if (Inspections){
		::imtbase::CTreeItemModel* newInspectionsModelPtr = model.AddTreeModel("Inspections", modelIndex);
		newInspectionsModelPtr->setIsArray(true);
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < Inspections->size(); ++inspectionsIndex){
			newInspectionsModelPtr->InsertNewItem();
			if (!(Inspections->at(inspectionsIndex)->WriteToModel(*newInspectionsModelPtr, inspectionsIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
		}
	}

	return true;
}


bool CProductOverview::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant productIDData = model.GetData("ProductID", modelIndex);
	if (productIDData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ProductID").toLocal8Bit().constData();)

		return false;
	}
	ProductID = productIDData.toInt();

	QVariant nameData = model.GetData("Name", modelIndex);
	if (nameData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	Name = nameData.toString();

	QVariant statusData = model.GetData("Status", modelIndex);
	if (statusData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	QString statusStringValue = statusData.toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::V1_0::complextest::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::V1_0::complextest::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::V1_0::complextest::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	QVariant partSerialNumberData = model.GetData("PartSerialNumber", modelIndex);
	if (partSerialNumberData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "PartSerialNumber").toLocal8Bit().constData();)

		return false;
	}
	PartSerialNumber = partSerialNumberData.toString();

	QVariant timestampData = model.GetData("Timestamp", modelIndex);
	if (timestampData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Timestamp").toLocal8Bit().constData();)

		return false;
	}
	Timestamp = timestampData.toString();

	QVariant productImageData = model.GetData("ProductImage", modelIndex);
	if (productImageData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ProductImage").toLocal8Bit().constData();)

		return false;
	}
	ProductImage = productImageData.toString();

	::imtbase::CTreeItemModel* inspectionsModel = model.GetTreeItemModel("Inspections", modelIndex);
	if (inspectionsModel != nullptr){
		int inspectionsCount = inspectionsModel->GetItemsCount();
		imtsdl::TElementList<CInspection> inspectionsList;
		for (int inspectionsIndex = 0; inspectionsIndex < inspectionsCount; ++inspectionsIndex){
			CInspection t_inspections;
			if (!t_inspections.ReadFromModel(*inspectionsModel, inspectionsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
			inspectionsList << t_inspections;
		}
		Inspections = inspectionsList;

	}

	return true;
}


bool CProductOverview::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
			Status = ::sdl::V1_0::complextest::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::V1_0::complextest::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::V1_0::complextest::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
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
		imtsdl::TElementList<CInspection> inspectionsList;
		for (int inspectionsIndex = 0; inspectionsIndex < inspectionsCount; ++inspectionsIndex){
			CInspection t_inspections;
			if (!t_inspections.OptReadFromModel(*inspectionsModel, inspectionsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
			inspectionsList << t_inspections;
		}
		Inspections = inspectionsList;

	}

	return true;
}


bool CProductOverview::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!ProductID){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductID").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("ProductID", QVariant(*ProductID));

	if (!Name){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("Name", QVariant(*Name));

	if (!Status){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::V1_0::complextest::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::V1_0::complextest::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::FAILED:
		statusStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("Status", QVariant(statusStringValue));

	if (!PartSerialNumber){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "PartSerialNumber").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("PartSerialNumber", QVariant(*PartSerialNumber));

	if (!Timestamp){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Timestamp").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("Timestamp", QVariant(*Timestamp));

	if (!ProductImage){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductImage").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("ProductImage", QVariant(*ProductImage));

	if (Inspections){
		QList<::imtgql::CGqlParamObject> inspectionsDataObjectList;
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < Inspections->size(); ++inspectionsIndex){
			::imtgql::CGqlParamObject newInspectionsGqlObject;
			if (!Inspections->at(inspectionsIndex)->WriteToGraphQlObject(newInspectionsGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
			inspectionsDataObjectList << newInspectionsGqlObject;
		}
		gqlObject.InsertParam("Inspections", inspectionsDataObjectList);
	}

	gqlObject.InsertParam("__typename", QVariant("ProductOverview"));

	return true;
}


bool CProductOverview::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("ProductID") || (gqlObject["ProductID"].userType() != QMetaType::Int && gqlObject["ProductID"].userType() != QMetaType::UInt && gqlObject["ProductID"].userType() != QMetaType::LongLong && gqlObject["ProductID"].userType() != QMetaType::ULongLong && gqlObject["ProductID"].userType() != QMetaType::Long && gqlObject["ProductID"].userType() != QMetaType::Short && gqlObject["ProductID"].userType() != QMetaType::ULong && gqlObject["ProductID"].userType() != QMetaType::UShort && gqlObject["ProductID"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductID").toLocal8Bit().constData();)

		return false;
	}
	ProductID = gqlObject["ProductID"].toInt();

	if (!gqlObject.ContainsParam("Name") || (gqlObject["Name"].userType() != QMetaType::QString && gqlObject["Name"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	Name = gqlObject["Name"].toString();

	if (!gqlObject.ContainsParam("Status") || (gqlObject["Status"].userType() != QMetaType::QString && gqlObject["Status"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	const QString statusStringValue = gqlObject["Status"].toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::V1_0::complextest::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::V1_0::complextest::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::V1_0::complextest::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("PartSerialNumber") || (gqlObject["PartSerialNumber"].userType() != QMetaType::QString && gqlObject["PartSerialNumber"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "PartSerialNumber").toLocal8Bit().constData();)

		return false;
	}
	PartSerialNumber = gqlObject["PartSerialNumber"].toString();

	if (!gqlObject.ContainsParam("Timestamp") || (gqlObject["Timestamp"].userType() != QMetaType::QString && gqlObject["Timestamp"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Timestamp").toLocal8Bit().constData();)

		return false;
	}
	Timestamp = gqlObject["Timestamp"].toString();

	if (!gqlObject.ContainsParam("ProductImage") || (gqlObject["ProductImage"].userType() != QMetaType::QString && gqlObject["ProductImage"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductImage").toLocal8Bit().constData();)

		return false;
	}
	ProductImage = gqlObject["ProductImage"].toString();

	if (gqlObject.ContainsParam("Inspections")){
		Inspections.emplace();
	}
	if (gqlObject.ContainsParam("Inspections") && (gqlObject.GetObjectsCount("Inspections") > 0)){
		const qsizetype inspectionsElementsCount = gqlObject.GetObjectsCount("Inspections");
		Inspections = imtsdl::TElementList<CInspection>();
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < inspectionsElementsCount; ++inspectionsIndex){
			const ::imtgql::CGqlParamObject* inspectionsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Inspections", inspectionsIndex);
			if (inspectionsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << inspectionsDataObjectPtr;
				return false;
			}
			CInspection tempInspections;
			if (!tempInspections.ReadFromGraphQlObject(*inspectionsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
			Inspections->append(tempInspections);
		}
	}

	return true;
}


bool CProductOverview::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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
			Status = ::sdl::V1_0::complextest::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::V1_0::complextest::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::V1_0::complextest::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
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

	if (gqlObject.ContainsParam("Inspections")){
		Inspections.emplace();
	}
	if (gqlObject.ContainsParam("Inspections") && (gqlObject.GetObjectsCount("Inspections") > 0)){
		const qsizetype inspectionsElementsCount = gqlObject.GetObjectsCount("Inspections");
		Inspections = imtsdl::TElementList<CInspection>();
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < inspectionsElementsCount; ++inspectionsIndex){
			const ::imtgql::CGqlParamObject* inspectionsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Inspections", inspectionsIndex);
			if (inspectionsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << inspectionsDataObjectPtr;
				return false;
			}
			CInspection tempInspections;
			if (!tempInspections.OptReadFromGraphQlObject(*inspectionsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
			Inspections->append(tempInspections);
		}
	}

	return true;
}


bool CProductOverview::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!ProductID){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductID").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["ProductID"] = QJsonValue::fromVariant(*ProductID);

	if (!Name){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["Name"] = QJsonValue::fromVariant(*Name);

	if (!Status){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	QString statusStringValue;
	switch (*Status){
	case ::sdl::V1_0::complextest::StatusCode::NONE:
		statusStringValue = "NONE";
		break;
	case ::sdl::V1_0::complextest::StatusCode::OK:
		statusStringValue = "OK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::WARNING:
		statusStringValue = "WARNING";
		break;
	case ::sdl::V1_0::complextest::StatusCode::NOK:
		statusStringValue = "NOK";
		break;
	case ::sdl::V1_0::complextest::StatusCode::FAILED:
		statusStringValue = "FAILED";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["Status"] = QJsonValue::fromVariant(statusStringValue);

	if (!PartSerialNumber){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "PartSerialNumber").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["PartSerialNumber"] = QJsonValue::fromVariant(*PartSerialNumber);

	if (!Timestamp){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Timestamp").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["Timestamp"] = QJsonValue::fromVariant(*Timestamp);

	if (!ProductImage){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductImage").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["ProductImage"] = QJsonValue::fromVariant(*ProductImage);

	if (Inspections){
		QJsonArray newInspectionsArray;
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < Inspections->size(); ++inspectionsIndex){
			QJsonObject newInspectionsJsonObject;
			if (!Inspections->at(inspectionsIndex)->WriteToJsonObject(newInspectionsJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
			newInspectionsArray << newInspectionsJsonObject;
		}
		jsonObject["Inspections"] = newInspectionsArray;
	}

	jsonObject["__typename"] = "ProductOverview";

	return true;
}


bool CProductOverview::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("ProductID") || ! jsonObject["ProductID"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductID").toLocal8Bit().constData();)

		return false;
	}
	ProductID = jsonObject["ProductID"].toInt();

	if (!jsonObject.contains("Name") || ! jsonObject["Name"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	Name = jsonObject["Name"].toString();

	if (!jsonObject.contains("Status") || ! jsonObject["Status"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

		return false;
	}
	const QString statusStringValue = jsonObject["Status"].toString();
	if(statusStringValue == "NONE"){
		Status = ::sdl::V1_0::complextest::StatusCode::NONE;
	}
	else if(statusStringValue == "OK"){
		Status = ::sdl::V1_0::complextest::StatusCode::OK;
	}
	else if(statusStringValue == "WARNING"){
		Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
	}
	else if(statusStringValue == "NOK"){
		Status = ::sdl::V1_0::complextest::StatusCode::NOK;
	}
	else if(statusStringValue == "FAILED"){
		Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (!jsonObject.contains("PartSerialNumber") || ! jsonObject["PartSerialNumber"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "PartSerialNumber").toLocal8Bit().constData();)

		return false;
	}
	PartSerialNumber = jsonObject["PartSerialNumber"].toString();

	if (!jsonObject.contains("Timestamp") || ! jsonObject["Timestamp"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Timestamp").toLocal8Bit().constData();)

		return false;
	}
	Timestamp = jsonObject["Timestamp"].toString();

	if (!jsonObject.contains("ProductImage") || ! jsonObject["ProductImage"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductImage").toLocal8Bit().constData();)

		return false;
	}
	ProductImage = jsonObject["ProductImage"].toString();

	if (jsonObject.contains("Inspections") && jsonObject["Inspections"].isArray()){
		const QJsonArray inspectionsJsonArray = jsonObject["Inspections"].toArray();
		const qsizetype inspectionsArrayCount = inspectionsJsonArray.size();
		Inspections = imtsdl::TElementList<CInspection>();
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < inspectionsArrayCount; ++inspectionsIndex){
			CInspection tempInspections;
			if (!tempInspections.ReadFromJsonObject(inspectionsJsonArray[inspectionsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
			Inspections->append(tempInspections);
		}
	}

	return true;
}


bool CProductOverview::OptReadFromJsonObject(const QJsonObject& jsonObject)
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
			Status = ::sdl::V1_0::complextest::StatusCode::NONE;
		}
		else if(statusStringValue == "OK"){
			Status = ::sdl::V1_0::complextest::StatusCode::OK;
		}
		else if(statusStringValue == "WARNING"){
			Status = ::sdl::V1_0::complextest::StatusCode::WARNING;
		}
		else if(statusStringValue == "NOK"){
			Status = ::sdl::V1_0::complextest::StatusCode::NOK;
		}
		else if(statusStringValue == "FAILED"){
			Status = ::sdl::V1_0::complextest::StatusCode::FAILED;
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
		Inspections = imtsdl::TElementList<CInspection>();
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < inspectionsArrayCount; ++inspectionsIndex){
			CInspection tempInspections;
			if (!tempInspections.OptReadFromJsonObject(inspectionsJsonArray[inspectionsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
			Inspections->append(tempInspections);
		}
	}

	return true;
}


// serialize methods

QByteArray CLocalizedText::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CLocalizedText::operator==(const CLocalizedText& other) const
{
	return 
				text == other.text &&
				locale == other.locale;
}


bool CLocalizedText::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "LocalizedText", modelIndex);

	if (!text){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "text").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("text", *text, modelIndex);

	if (!locale){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "locale").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("locale", *locale, modelIndex);


	return true;
}


bool CLocalizedText::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant textData = model.GetData("text", modelIndex);
	if (textData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "text").toLocal8Bit().constData();)

		return false;
	}
	text = textData.toString();

	QVariant localeData = model.GetData("locale", modelIndex);
	if (localeData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "locale").toLocal8Bit().constData();)

		return false;
	}
	locale = localeData.toString();

	return true;
}


bool CLocalizedText::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CLocalizedText::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!text){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "text").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("text", QVariant(*text));

	if (!locale){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "locale").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("locale", QVariant(*locale));

	gqlObject.InsertParam("__typename", QVariant("LocalizedText"));

	return true;
}


bool CLocalizedText::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("text") || (gqlObject["text"].userType() != QMetaType::QString && gqlObject["text"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "text").toLocal8Bit().constData();)

		return false;
	}
	text = gqlObject["text"].toString();

	if (!gqlObject.ContainsParam("locale") || (gqlObject["locale"].userType() != QMetaType::QString && gqlObject["locale"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "locale").toLocal8Bit().constData();)

		return false;
	}
	locale = gqlObject["locale"].toString();

	return true;
}


bool CLocalizedText::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("text") && (gqlObject["text"].userType() == QMetaType::QString || gqlObject["text"].userType() == QMetaType::QByteArray)){
		text = gqlObject["text"].toString();
	}

	if (gqlObject.ContainsParam("locale") && (gqlObject["locale"].userType() == QMetaType::QString || gqlObject["locale"].userType() == QMetaType::QByteArray)){
		locale = gqlObject["locale"].toString();
	}

	return true;
}


bool CLocalizedText::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!text){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "text").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["text"] = QJsonValue::fromVariant(*text);

	if (!locale){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "locale").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["locale"] = QJsonValue::fromVariant(*locale);

	jsonObject["__typename"] = "LocalizedText";

	return true;
}


bool CLocalizedText::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("text") || ! jsonObject["text"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "text").toLocal8Bit().constData();)

		return false;
	}
	text = jsonObject["text"].toString();

	if (!jsonObject.contains("locale") || ! jsonObject["locale"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "locale").toLocal8Bit().constData();)

		return false;
	}
	locale = jsonObject["locale"].toString();

	return true;
}


bool CLocalizedText::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

QByteArray CCoordinates::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CCoordinates::operator==(const CCoordinates& other) const
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


bool CCoordinates::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Coordinates", modelIndex);

	if (!x){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "x").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("x", *x, modelIndex);

	if (!y){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "y").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("y", *y, modelIndex);

	if (!z){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "z").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("z", *z, modelIndex);


	return true;
}


bool CCoordinates::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant xData = model.GetData("x", modelIndex);
	if (xData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "x").toLocal8Bit().constData();)

		return false;
	}
	x = xData.toFloat();

	QVariant yData = model.GetData("y", modelIndex);
	if (yData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "y").toLocal8Bit().constData();)

		return false;
	}
	y = yData.toFloat();

	QVariant zData = model.GetData("z", modelIndex);
	if (zData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "z").toLocal8Bit().constData();)

		return false;
	}
	z = zData.toFloat();

	return true;
}


bool CCoordinates::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CCoordinates::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!x){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "x").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("x", QVariant(*x));

	if (!y){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "y").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("y", QVariant(*y));

	if (!z){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "z").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("z", QVariant(*z));

	gqlObject.InsertParam("__typename", QVariant("Coordinates"));

	return true;
}


bool CCoordinates::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("x") || (gqlObject["x"].userType() != QMetaType::Float && gqlObject["x"].userType() != QMetaType::Double && gqlObject["x"].userType() != QMetaType::Int && gqlObject["x"].userType() != QMetaType::UInt && gqlObject["x"].userType() != QMetaType::LongLong && gqlObject["x"].userType() != QMetaType::ULongLong && gqlObject["x"].userType() != QMetaType::Long && gqlObject["x"].userType() != QMetaType::Short && gqlObject["x"].userType() != QMetaType::ULong && gqlObject["x"].userType() != QMetaType::UShort && gqlObject["x"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "x").toLocal8Bit().constData();)

		return false;
	}
	x = gqlObject["x"].toDouble();

	if (!gqlObject.ContainsParam("y") || (gqlObject["y"].userType() != QMetaType::Float && gqlObject["y"].userType() != QMetaType::Double && gqlObject["y"].userType() != QMetaType::Int && gqlObject["y"].userType() != QMetaType::UInt && gqlObject["y"].userType() != QMetaType::LongLong && gqlObject["y"].userType() != QMetaType::ULongLong && gqlObject["y"].userType() != QMetaType::Long && gqlObject["y"].userType() != QMetaType::Short && gqlObject["y"].userType() != QMetaType::ULong && gqlObject["y"].userType() != QMetaType::UShort && gqlObject["y"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "y").toLocal8Bit().constData();)

		return false;
	}
	y = gqlObject["y"].toDouble();

	if (!gqlObject.ContainsParam("z") || (gqlObject["z"].userType() != QMetaType::Float && gqlObject["z"].userType() != QMetaType::Double && gqlObject["z"].userType() != QMetaType::Int && gqlObject["z"].userType() != QMetaType::UInt && gqlObject["z"].userType() != QMetaType::LongLong && gqlObject["z"].userType() != QMetaType::ULongLong && gqlObject["z"].userType() != QMetaType::Long && gqlObject["z"].userType() != QMetaType::Short && gqlObject["z"].userType() != QMetaType::ULong && gqlObject["z"].userType() != QMetaType::UShort && gqlObject["z"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "z").toLocal8Bit().constData();)

		return false;
	}
	z = gqlObject["z"].toDouble();

	return true;
}


bool CCoordinates::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CCoordinates::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!x){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "x").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["x"] = QJsonValue::fromVariant(*x);

	if (!y){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "y").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["y"] = QJsonValue::fromVariant(*y);

	if (!z){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "z").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["z"] = QJsonValue::fromVariant(*z);

	jsonObject["__typename"] = "Coordinates";

	return true;
}


bool CCoordinates::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("x") || ! jsonObject["x"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "x").toLocal8Bit().constData();)

		return false;
	}
	x = jsonObject["x"].toDouble();

	if (!jsonObject.contains("y") || ! jsonObject["y"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "y").toLocal8Bit().constData();)

		return false;
	}
	y = jsonObject["y"].toDouble();

	if (!jsonObject.contains("z") || ! jsonObject["z"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "z").toLocal8Bit().constData();)

		return false;
	}
	z = jsonObject["z"].toDouble();

	return true;
}


bool CCoordinates::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

QByteArray CExtendedMetaData::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CExtendedMetaData::operator==(const CExtendedMetaData& other) const
{
	return 
				key == other.key &&
				value == other.value;
}


bool CExtendedMetaData::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "ExtendedMetaData", modelIndex);

	if (key){
		model.SetData("key", *key, modelIndex);
	}

	if (value){
		QVariant valueVariantValue;
		if (const CCoordinates* coordinatesVal = std::get_if<CCoordinates>(value.GetPtr())){
			if (!coordinatesVal->WriteToModel(*(model.AddTreeModel("value", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("value", modelIndex) != nullptr){
				model.GetTreeItemModel("value", modelIndex)->SetData("__typename", "Coordinates", 0);
			}
		}
		else if (const double* doubleVal = std::get_if<double>(value.GetPtr())){
			model.SetData("", *doubleVal);
		}
		else if (const bool* booleanVal = std::get_if<bool>(value.GetPtr())){
			model.SetData("", *booleanVal);
		}
		else if (const QString* stringVal = std::get_if<QString>(value.GetPtr())){
			model.SetData("", *stringVal);
		}

	}


	return true;
}


bool CExtendedMetaData::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant keyData = model.GetData("key", modelIndex);
	if (!keyData.isNull()){
		key = keyData.toString();
	}

	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
		QString valueTypename = model.GetTreeItemModel("value", modelIndex)->GetData("__typename").toString();
		if (valueTypename == "Coordinates") {
			CCoordinates valueConvert;
			const bool isvalueRead = valueConvert.ReadFromModel(*model.GetTreeItemModel("value", modelIndex)); 
			if (!isvalueRead){
				return false;
			}
			value = ExtendedMetaDataUnionType(valueConvert);
		}
		else if (valueData.canConvert<double>()){
			value = ExtendedMetaDataUnionType(valueData.value<double>());
		}
		else if (valueData.canConvert<bool>()){
			value = ExtendedMetaDataUnionType(valueData.value<bool>());
		}
		else if (valueData.canConvert<QString>()){
			value = ExtendedMetaDataUnionType(valueData.value<QString>());
		}
	}

	return true;
}


bool CExtendedMetaData::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant keyData = model.GetData("key", modelIndex);
	if (!keyData.isNull()){
		key = keyData.toString();
	}

	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
		QString valueTypename = model.GetTreeItemModel("value", modelIndex)->GetData("__typename").toString();
		if (valueTypename == "Coordinates") {
			CCoordinates valueConvert;
			const bool isvalueRead = valueConvert.ReadFromModel(*model.GetTreeItemModel("value", modelIndex)); 
			if (!isvalueRead){
				return false;
			}
			value = ExtendedMetaDataUnionType(valueConvert);
		}
		else if (valueData.canConvert<double>()){
			value = ExtendedMetaDataUnionType(valueData.value<double>());
		}
		else if (valueData.canConvert<bool>()){
			value = ExtendedMetaDataUnionType(valueData.value<bool>());
		}
		else if (valueData.canConvert<QString>()){
			value = ExtendedMetaDataUnionType(valueData.value<QString>());
		}
	}

	return true;
}


bool CExtendedMetaData::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (key){
		gqlObject.InsertParam("key", QVariant(*key));
	}

	if (value){
		::imtgql::CGqlParamObject valueDataObject;
		if (const CCoordinates* coordinatesVal = std::get_if<CCoordinates>(value.GetPtr())){
			if (!coordinatesVal->WriteToGraphQlObject(valueDataObject)){
				return false;
			}
			valueDataObject.InsertParam("__typename", QVariant("Coordinates"));
		}
		else if (const double* doubleVal = std::get_if<double>(value.GetPtr())){
			valueDataObject.InsertParam("value", *doubleVal);
		}
		else if (const bool* booleanVal = std::get_if<bool>(value.GetPtr())){
			valueDataObject.InsertParam("value", *booleanVal);
		}
		else if (const QString* stringVal = std::get_if<QString>(value.GetPtr())){
			valueDataObject.InsertParam("value", *stringVal);
		}
		gqlObject.InsertParam("value", valueDataObject);
	}

	gqlObject.InsertParam("__typename", QVariant("ExtendedMetaData"));

	return true;
}


bool CExtendedMetaData::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("key") && (gqlObject["key"].userType() == QMetaType::QString || gqlObject["key"].userType() == QMetaType::QByteArray)){
		key = gqlObject["key"].toString();
	}

	if (gqlObject.ContainsParam("value")){
		const ::imtgql::CGqlParamObject* valueDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("value");
		if (!valueDataObjectPtr->ContainsParam("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "value").toLocal8Bit().constData();)

			return false;
		}
		QString valueTypename = valueDataObjectPtr->GetParamArgumentValue("__typename").toString();
		QVariant valueVariantValue = gqlObject["value"];
		if (valueTypename == "Coordinates") {
			CCoordinates valueConvert;
			const bool isValueRead = valueConvert.ReadFromGraphQlObject(*valueDataObjectPtr);
			if (!isValueRead){
				return false;
			}
			value = ExtendedMetaDataUnionType(valueConvert);
		}
		else if (valueVariantValue.canConvert<double>()){
			value = ExtendedMetaDataUnionType(valueVariantValue.value<double>());
		}
		else if (valueVariantValue.canConvert<bool>()){
			value = ExtendedMetaDataUnionType(valueVariantValue.value<bool>());
		}
		else if (valueVariantValue.canConvert<QString>()){
			value = ExtendedMetaDataUnionType(valueVariantValue.value<QString>());
		}
	}

	return true;
}


bool CExtendedMetaData::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("key") && (gqlObject["key"].userType() == QMetaType::QString || gqlObject["key"].userType() == QMetaType::QByteArray)){
		key = gqlObject["key"].toString();
	}

	if (gqlObject.ContainsParam("value")){
		const ::imtgql::CGqlParamObject* valueDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("value");
		if (!valueDataObjectPtr->ContainsParam("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "value").toLocal8Bit().constData();)

			return false;
		}
		QString valueTypename = valueDataObjectPtr->GetParamArgumentValue("__typename").toString();
		QVariant valueVariantValue = gqlObject["value"];
		if (valueTypename == "Coordinates") {
			CCoordinates valueConvert;
			const bool isValueRead = valueConvert.ReadFromGraphQlObject(*valueDataObjectPtr);
			if (!isValueRead){
				return false;
			}
			value = ExtendedMetaDataUnionType(valueConvert);
		}
		else if (valueVariantValue.canConvert<double>()){
			value = ExtendedMetaDataUnionType(valueVariantValue.value<double>());
		}
		else if (valueVariantValue.canConvert<bool>()){
			value = ExtendedMetaDataUnionType(valueVariantValue.value<bool>());
		}
		else if (valueVariantValue.canConvert<QString>()){
			value = ExtendedMetaDataUnionType(valueVariantValue.value<QString>());
		}
	}

	return true;
}


bool CExtendedMetaData::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (key){
		jsonObject["key"] = QJsonValue::fromVariant(*key);
	}

	if (value){
		if (const CCoordinates* coordinatesVal = std::get_if<CCoordinates>(value.GetPtr())){
			QJsonObject valueJsonObject;
			const bool isvalueAdded = coordinatesVal->WriteToJsonObject(valueJsonObject);
			if (!isvalueAdded){
				return false;
			}
			valueJsonObject["__typename"] = "Coordinates";
			jsonObject["value"] = valueJsonObject;
		}
		else if (const double* doubleVal = std::get_if<double>(value.GetPtr())){
			if (!value){
				return false;
			}
			jsonObject["value"] = QJsonValue::fromVariant(*doubleVal);
		}
		else if (const bool* booleanVal = std::get_if<bool>(value.GetPtr())){
			if (!value){
				return false;
			}
			jsonObject["value"] = QJsonValue::fromVariant(*booleanVal);
		}
		else if (const QString* stringVal = std::get_if<QString>(value.GetPtr())){
			if (!value){
				return false;
			}
			jsonObject["value"] = QJsonValue::fromVariant(*stringVal);
		}

	}

	jsonObject["__typename"] = "ExtendedMetaData";

	return true;
}


bool CExtendedMetaData::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("key") && jsonObject["key"].isString()){
		key = jsonObject["key"].toString();
	}

	if (jsonObject.contains("value")){
		const QJsonObject valueObject = jsonObject.value("value").toObject();
		if (!jsonObject.value("value").isObject() || !valueObject.contains("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "value").toLocal8Bit().constData();)

			return false;
		}
		QString valueTypename = valueObject.value("__typename").toString();
		QVariant valueVariantValue = jsonObject["value"].toVariant();
		if (valueTypename == "Coordinates") {
			CCoordinates valueConvert;
			const bool isvalueRead = valueConvert.ReadFromJsonObject(jsonObject["value"].toObject());
			if (!isvalueRead){
				return false;
			}
			value = ExtendedMetaDataUnionType(valueConvert);
		}
		else if (valueVariantValue.canConvert<double>()){
			value = ExtendedMetaDataUnionType(valueVariantValue.value<double>());
		}
		else if (valueVariantValue.canConvert<bool>()){
			value = ExtendedMetaDataUnionType(valueVariantValue.value<bool>());
		}
		else if (valueVariantValue.canConvert<QString>()){
			value = ExtendedMetaDataUnionType(valueVariantValue.value<QString>());
		}
	}

	return true;
}


bool CExtendedMetaData::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("key") && jsonObject["key"].isString()){
		key = jsonObject["key"].toString();
	}

	if (jsonObject.contains("value")){
		const QJsonObject valueObject = jsonObject.value("value").toObject();
		if (!jsonObject.value("value").isObject() || !valueObject.contains("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "value").toLocal8Bit().constData();)

			return false;
		}
		QString valueTypename = valueObject.value("__typename").toString();
		QVariant valueVariantValue = jsonObject["value"].toVariant();
		if (valueTypename == "Coordinates") {
			CCoordinates valueConvert;
			const bool isvalueRead = valueConvert.ReadFromJsonObject(jsonObject["value"].toObject());
			if (!isvalueRead){
				return false;
			}
			value = ExtendedMetaDataUnionType(valueConvert);
		}
		else if (valueVariantValue.canConvert<double>()){
			value = ExtendedMetaDataUnionType(valueVariantValue.value<double>());
		}
		else if (valueVariantValue.canConvert<bool>()){
			value = ExtendedMetaDataUnionType(valueVariantValue.value<bool>());
		}
		else if (valueVariantValue.canConvert<QString>()){
			value = ExtendedMetaDataUnionType(valueVariantValue.value<QString>());
		}
	}

	return true;
}


// serialize methods

QByteArray CResultMetaData::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CResultMetaData::operator==(const CResultMetaData& other) const
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


bool CResultMetaData::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "ResultMetaData", modelIndex);

	if (!resultId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "resultId").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}

	}
	if (extendedMetaData){
		::imtbase::CTreeItemModel* newExtendedMetaDataModelPtr = model.AddTreeModel("extendedMetaData", modelIndex);
		newExtendedMetaDataModelPtr->setIsArray(true);
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaData->size(); ++extendedMetaDataIndex){
			newExtendedMetaDataModelPtr->InsertNewItem();
			if (!(extendedMetaData->at(extendedMetaDataIndex)->WriteToModel(*newExtendedMetaDataModelPtr, extendedMetaDataIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
		}
	}

	return true;
}


bool CResultMetaData::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant resultIdData = model.GetData("resultId", modelIndex);
	if (resultIdData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "resultId").toLocal8Bit().constData();)

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
		description = CLocalizedText();
		const bool isDescriptionRead = description->ReadFromModel(*descriptionDataModelPtr, modelIndex);
		if (!isDescriptionRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* extendedMetaDataModel = model.GetTreeItemModel("extendedMetaData", modelIndex);
	if (extendedMetaDataModel != nullptr){
		int extendedMetaDataCount = extendedMetaDataModel->GetItemsCount();
		imtsdl::TElementList<CExtendedMetaData> extendedMetaDataList;
		for (int extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataCount; ++extendedMetaDataIndex){
			CExtendedMetaData t_extendedMetaData;
			if (!t_extendedMetaData.ReadFromModel(*extendedMetaDataModel, extendedMetaDataIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
			extendedMetaDataList << t_extendedMetaData;
		}
		extendedMetaData = extendedMetaDataList;

	}

	return true;
}


bool CResultMetaData::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
		description = CLocalizedText();
		const bool isDescriptionRead = description->ReadFromModel(*descriptionDataModelPtr, modelIndex);
		if (!isDescriptionRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* extendedMetaDataModel = model.GetTreeItemModel("extendedMetaData", modelIndex);
	if (extendedMetaDataModel != nullptr){
		int extendedMetaDataCount = extendedMetaDataModel->GetItemsCount();
		imtsdl::TElementList<CExtendedMetaData> extendedMetaDataList;
		for (int extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataCount; ++extendedMetaDataIndex){
			CExtendedMetaData t_extendedMetaData;
			if (!t_extendedMetaData.OptReadFromModel(*extendedMetaDataModel, extendedMetaDataIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
			extendedMetaDataList << t_extendedMetaData;
		}
		extendedMetaData = extendedMetaDataList;

	}

	return true;
}


bool CResultMetaData::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!resultId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "resultId").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("description", descriptionGqlObject);
	}

	if (extendedMetaData){
		QList<::imtgql::CGqlParamObject> extendedMetaDataDataObjectList;
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaData->size(); ++extendedMetaDataIndex){
			::imtgql::CGqlParamObject newExtendedMetaDataGqlObject;
			if (!extendedMetaData->at(extendedMetaDataIndex)->WriteToGraphQlObject(newExtendedMetaDataGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
			extendedMetaDataDataObjectList << newExtendedMetaDataGqlObject;
		}
		gqlObject.InsertParam("extendedMetaData", extendedMetaDataDataObjectList);
	}

	gqlObject.InsertParam("__typename", QVariant("ResultMetaData"));

	return true;
}


bool CResultMetaData::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("resultId") || (gqlObject["resultId"].userType() != QMetaType::QString && gqlObject["resultId"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "resultId").toLocal8Bit().constData();)

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
		description = CLocalizedText();
		const bool isDescriptionRead = description->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("description"));
		if (!isDescriptionRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("extendedMetaData")){
		extendedMetaData.emplace();
	}
	if (gqlObject.ContainsParam("extendedMetaData") && (gqlObject.GetObjectsCount("extendedMetaData") > 0)){
		const qsizetype extendedMetaDataElementsCount = gqlObject.GetObjectsCount("extendedMetaData");
		extendedMetaData = imtsdl::TElementList<CExtendedMetaData>();
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataElementsCount; ++extendedMetaDataIndex){
			const ::imtgql::CGqlParamObject* extendedMetaDataDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("extendedMetaData", extendedMetaDataIndex);
			if (extendedMetaDataDataObjectPtr == nullptr){
				qDebug() << "invalid type" << extendedMetaDataDataObjectPtr;
				return false;
			}
			CExtendedMetaData tempExtendedMetaData;
			if (!tempExtendedMetaData.ReadFromGraphQlObject(*extendedMetaDataDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
			extendedMetaData->append(tempExtendedMetaData);
		}
	}

	return true;
}


bool CResultMetaData::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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
		description = CLocalizedText();
		const bool isDescriptionRead = description->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("description"));
		if (!isDescriptionRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("extendedMetaData")){
		extendedMetaData.emplace();
	}
	if (gqlObject.ContainsParam("extendedMetaData") && (gqlObject.GetObjectsCount("extendedMetaData") > 0)){
		const qsizetype extendedMetaDataElementsCount = gqlObject.GetObjectsCount("extendedMetaData");
		extendedMetaData = imtsdl::TElementList<CExtendedMetaData>();
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataElementsCount; ++extendedMetaDataIndex){
			const ::imtgql::CGqlParamObject* extendedMetaDataDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("extendedMetaData", extendedMetaDataIndex);
			if (extendedMetaDataDataObjectPtr == nullptr){
				qDebug() << "invalid type" << extendedMetaDataDataObjectPtr;
				return false;
			}
			CExtendedMetaData tempExtendedMetaData;
			if (!tempExtendedMetaData.OptReadFromGraphQlObject(*extendedMetaDataDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
			extendedMetaData->append(tempExtendedMetaData);
		}
	}

	return true;
}


bool CResultMetaData::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!resultId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "resultId").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["resultId"] = QString::fromUtf8(*resultId);

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["description"] = descriptionJsonObject;
	}

	if (extendedMetaData){
		QJsonArray newExtendedMetaDataArray;
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaData->size(); ++extendedMetaDataIndex){
			QJsonObject newExtendedMetaDataJsonObject;
			if (!extendedMetaData->at(extendedMetaDataIndex)->WriteToJsonObject(newExtendedMetaDataJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
			newExtendedMetaDataArray << newExtendedMetaDataJsonObject;
		}
		jsonObject["extendedMetaData"] = newExtendedMetaDataArray;
	}

	jsonObject["__typename"] = "ResultMetaData";

	return true;
}


bool CResultMetaData::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("resultId") || ! jsonObject["resultId"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "resultId").toLocal8Bit().constData();)

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
		description = CLocalizedText();
		const bool isDescriptionRead = description->ReadFromJsonObject(jsonObject["description"].toObject());
		if (!isDescriptionRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("extendedMetaData") && jsonObject["extendedMetaData"].isArray()){
		const QJsonArray extendedMetaDataJsonArray = jsonObject["extendedMetaData"].toArray();
		const qsizetype extendedMetaDataArrayCount = extendedMetaDataJsonArray.size();
		extendedMetaData = imtsdl::TElementList<CExtendedMetaData>();
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataArrayCount; ++extendedMetaDataIndex){
			CExtendedMetaData tempExtendedMetaData;
			if (!tempExtendedMetaData.ReadFromJsonObject(extendedMetaDataJsonArray[extendedMetaDataIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
			extendedMetaData->append(tempExtendedMetaData);
		}
	}

	return true;
}


bool CResultMetaData::OptReadFromJsonObject(const QJsonObject& jsonObject)
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
		description = CLocalizedText();
		const bool isDescriptionRead = description->OptReadFromJsonObject(jsonObject["description"].toObject());
		if (!isDescriptionRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("extendedMetaData") && jsonObject["extendedMetaData"].isArray()){
		const QJsonArray extendedMetaDataJsonArray = jsonObject["extendedMetaData"].toArray();
		const qsizetype extendedMetaDataArrayCount = extendedMetaDataJsonArray.size();
		extendedMetaData = imtsdl::TElementList<CExtendedMetaData>();
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataArrayCount; ++extendedMetaDataIndex){
			CExtendedMetaData tempExtendedMetaData;
			if (!tempExtendedMetaData.OptReadFromJsonObject(extendedMetaDataJsonArray[extendedMetaDataIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
			extendedMetaData->append(tempExtendedMetaData);
		}
	}

	return true;
}


// serialize methods

QByteArray COverallResultValues::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool COverallResultValues::operator==(const COverallResultValues& other) const
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


bool COverallResultValues::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "OverallResultValues", modelIndex);

	if (name){
		model.SetData("name", *name, modelIndex);
	}

	if (!valueId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "valueId").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("valueId", *valueId, modelIndex);

	if (!measuredValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "measuredValue").toLocal8Bit().constData();)

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


bool COverallResultValues::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	QVariant valueIdData = model.GetData("valueId", modelIndex);
	if (valueIdData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "valueId").toLocal8Bit().constData();)

		return false;
	}
	valueId = valueIdData.toString();

	QVariant measuredValueData = model.GetData("measuredValue", modelIndex);
	if (measuredValueData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "measuredValue").toLocal8Bit().constData();)

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


bool COverallResultValues::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool COverallResultValues::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (name){
		gqlObject.InsertParam("name", QVariant(*name));
	}

	if (!valueId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "valueId").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("valueId", QVariant(*valueId));

	if (!measuredValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "measuredValue").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("measuredValue", QVariant(*measuredValue));

	if (lowLimit){
		gqlObject.InsertParam("lowLimit", QVariant(*lowLimit));
	}

	if (highLimit){
		gqlObject.InsertParam("highLimit", QVariant(*highLimit));
	}

	gqlObject.InsertParam("__typename", QVariant("OverallResultValues"));

	return true;
}


bool COverallResultValues::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (!gqlObject.ContainsParam("valueId") || (gqlObject["valueId"].userType() != QMetaType::QString && gqlObject["valueId"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "valueId").toLocal8Bit().constData();)

		return false;
	}
	valueId = gqlObject["valueId"].toString();

	if (!gqlObject.ContainsParam("measuredValue") || (gqlObject["measuredValue"].userType() != QMetaType::Float && gqlObject["measuredValue"].userType() != QMetaType::Double && gqlObject["measuredValue"].userType() != QMetaType::Int && gqlObject["measuredValue"].userType() != QMetaType::UInt && gqlObject["measuredValue"].userType() != QMetaType::LongLong && gqlObject["measuredValue"].userType() != QMetaType::ULongLong && gqlObject["measuredValue"].userType() != QMetaType::Long && gqlObject["measuredValue"].userType() != QMetaType::Short && gqlObject["measuredValue"].userType() != QMetaType::ULong && gqlObject["measuredValue"].userType() != QMetaType::UShort && gqlObject["measuredValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "measuredValue").toLocal8Bit().constData();)

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


bool COverallResultValues::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool COverallResultValues::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (name){
		jsonObject["name"] = QJsonValue::fromVariant(*name);
	}

	if (!valueId){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "valueId").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["valueId"] = QJsonValue::fromVariant(*valueId);

	if (!measuredValue){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "measuredValue").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["measuredValue"] = QJsonValue::fromVariant(*measuredValue);

	if (lowLimit){
		jsonObject["lowLimit"] = QJsonValue::fromVariant(*lowLimit);
	}

	if (highLimit){
		jsonObject["highLimit"] = QJsonValue::fromVariant(*highLimit);
	}

	jsonObject["__typename"] = "OverallResultValues";

	return true;
}


bool COverallResultValues::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (!jsonObject.contains("valueId") || ! jsonObject["valueId"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "valueId").toLocal8Bit().constData();)

		return false;
	}
	valueId = jsonObject["valueId"].toString();

	if (!jsonObject.contains("measuredValue") || ! jsonObject["measuredValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "measuredValue").toLocal8Bit().constData();)

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


bool COverallResultValues::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

QByteArray CUnionTestingType::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CUnionTestingType::operator==(const CUnionTestingType& other) const
{
	return 
				simpleUnion == other.simpleUnion &&
				complexUnion == other.complexUnion &&
				mixedUnion == other.mixedUnion;
}


bool CUnionTestingType::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "UnionTestingType", modelIndex);

	if (simpleUnion){
		QVariant simpleUnionVariantValue;
		if (const double* doubleVal = std::get_if<double>(simpleUnion.GetPtr())){
			model.SetData("", *doubleVal);
		}
		else if (const bool* booleanVal = std::get_if<bool>(simpleUnion.GetPtr())){
			model.SetData("", *booleanVal);
		}
		else if (const QString* stringVal = std::get_if<QString>(simpleUnion.GetPtr())){
			model.SetData("", *stringVal);
		}

	}

	if (complexUnion){
		QVariant complexUnionVariantValue;
		if (const CCDMResultVarString* cDMResultVarStringVal = std::get_if<CCDMResultVarString>(complexUnion.GetPtr())){
			if (!cDMResultVarStringVal->WriteToModel(*(model.AddTreeModel("complexUnion", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("complexUnion", modelIndex) != nullptr){
				model.GetTreeItemModel("complexUnion", modelIndex)->SetData("__typename", "CDMResultVarString", 0);
			}
		}
		else if (const CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<CCDMResultVarRecursive>(complexUnion.GetPtr())){
			if (!cDMResultVarRecursiveVal->WriteToModel(*(model.AddTreeModel("complexUnion", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("complexUnion", modelIndex) != nullptr){
				model.GetTreeItemModel("complexUnion", modelIndex)->SetData("__typename", "CDMResultVarRecursive", 0);
			}
		}

	}

	if (mixedUnion){
		QVariant mixedUnionVariantValue;
		if (const CCoordinates* coordinatesVal = std::get_if<CCoordinates>(mixedUnion.GetPtr())){
			if (!coordinatesVal->WriteToModel(*(model.AddTreeModel("mixedUnion", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("mixedUnion", modelIndex) != nullptr){
				model.GetTreeItemModel("mixedUnion", modelIndex)->SetData("__typename", "Coordinates", 0);
			}
		}
		else if (const double* doubleVal = std::get_if<double>(mixedUnion.GetPtr())){
			model.SetData("", *doubleVal);
		}
		else if (const bool* booleanVal = std::get_if<bool>(mixedUnion.GetPtr())){
			model.SetData("", *booleanVal);
		}
		else if (const QString* stringVal = std::get_if<QString>(mixedUnion.GetPtr())){
			model.SetData("", *stringVal);
		}

	}


	return true;
}


bool CUnionTestingType::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant simpleUnionData = model.GetData("simpleUnion", modelIndex);
	if (!simpleUnionData.isNull()){
		if (simpleUnionData.canConvert<double>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionData.value<double>());
		}
		else if (simpleUnionData.canConvert<bool>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionData.value<bool>());
		}
		else if (simpleUnionData.canConvert<QString>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionData.value<QString>());
		}
	}

	QVariant complexUnionData = model.GetData("complexUnion", modelIndex);
	if (!complexUnionData.isNull()){
		QString complexUnionTypename = model.GetTreeItemModel("complexUnion", modelIndex)->GetData("__typename").toString();
		if (complexUnionTypename == "CDMResultVarString") {
			CCDMResultVarString complexUnionConvert;
			const bool iscomplexUnionRead = complexUnionConvert.ReadFromModel(*model.GetTreeItemModel("complexUnion", modelIndex)); 
			if (!iscomplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
		else if (complexUnionTypename == "CDMResultVarRecursive") {
			CCDMResultVarRecursive complexUnionConvert;
			const bool iscomplexUnionRead = complexUnionConvert.ReadFromModel(*model.GetTreeItemModel("complexUnion", modelIndex)); 
			if (!iscomplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
	}

	QVariant mixedUnionData = model.GetData("mixedUnion", modelIndex);
	if (!mixedUnionData.isNull()){
		QString mixedUnionTypename = model.GetTreeItemModel("mixedUnion", modelIndex)->GetData("__typename").toString();
		if (mixedUnionTypename == "Coordinates") {
			CCoordinates mixedUnionConvert;
			const bool ismixedUnionRead = mixedUnionConvert.ReadFromModel(*model.GetTreeItemModel("mixedUnion", modelIndex)); 
			if (!ismixedUnionRead){
				return false;
			}
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionConvert);
		}
		else if (mixedUnionData.canConvert<double>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionData.value<double>());
		}
		else if (mixedUnionData.canConvert<bool>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionData.value<bool>());
		}
		else if (mixedUnionData.canConvert<QString>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionData.value<QString>());
		}
	}

	return true;
}


bool CUnionTestingType::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant simpleUnionData = model.GetData("simpleUnion", modelIndex);
	if (!simpleUnionData.isNull()){
		if (simpleUnionData.canConvert<double>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionData.value<double>());
		}
		else if (simpleUnionData.canConvert<bool>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionData.value<bool>());
		}
		else if (simpleUnionData.canConvert<QString>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionData.value<QString>());
		}
	}

	QVariant complexUnionData = model.GetData("complexUnion", modelIndex);
	if (!complexUnionData.isNull()){
		QString complexUnionTypename = model.GetTreeItemModel("complexUnion", modelIndex)->GetData("__typename").toString();
		if (complexUnionTypename == "CDMResultVarString") {
			CCDMResultVarString complexUnionConvert;
			const bool iscomplexUnionRead = complexUnionConvert.ReadFromModel(*model.GetTreeItemModel("complexUnion", modelIndex)); 
			if (!iscomplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
		else if (complexUnionTypename == "CDMResultVarRecursive") {
			CCDMResultVarRecursive complexUnionConvert;
			const bool iscomplexUnionRead = complexUnionConvert.ReadFromModel(*model.GetTreeItemModel("complexUnion", modelIndex)); 
			if (!iscomplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
	}

	QVariant mixedUnionData = model.GetData("mixedUnion", modelIndex);
	if (!mixedUnionData.isNull()){
		QString mixedUnionTypename = model.GetTreeItemModel("mixedUnion", modelIndex)->GetData("__typename").toString();
		if (mixedUnionTypename == "Coordinates") {
			CCoordinates mixedUnionConvert;
			const bool ismixedUnionRead = mixedUnionConvert.ReadFromModel(*model.GetTreeItemModel("mixedUnion", modelIndex)); 
			if (!ismixedUnionRead){
				return false;
			}
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionConvert);
		}
		else if (mixedUnionData.canConvert<double>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionData.value<double>());
		}
		else if (mixedUnionData.canConvert<bool>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionData.value<bool>());
		}
		else if (mixedUnionData.canConvert<QString>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionData.value<QString>());
		}
	}

	return true;
}


bool CUnionTestingType::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (simpleUnion){
		::imtgql::CGqlParamObject simpleUnionDataObject;
		if (const double* doubleVal = std::get_if<double>(simpleUnion.GetPtr())){
			simpleUnionDataObject.InsertParam("simpleUnion", *doubleVal);
		}
		else if (const bool* booleanVal = std::get_if<bool>(simpleUnion.GetPtr())){
			simpleUnionDataObject.InsertParam("simpleUnion", *booleanVal);
		}
		else if (const QString* stringVal = std::get_if<QString>(simpleUnion.GetPtr())){
			simpleUnionDataObject.InsertParam("simpleUnion", *stringVal);
		}
		gqlObject.InsertParam("simpleUnion", simpleUnionDataObject);
	}

	if (complexUnion){
		::imtgql::CGqlParamObject complexUnionDataObject;
		if (const CCDMResultVarString* cDMResultVarStringVal = std::get_if<CCDMResultVarString>(complexUnion.GetPtr())){
			if (!cDMResultVarStringVal->WriteToGraphQlObject(complexUnionDataObject)){
				return false;
			}
			complexUnionDataObject.InsertParam("__typename", QVariant("CDMResultVarString"));
		}
		else if (const CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<CCDMResultVarRecursive>(complexUnion.GetPtr())){
			if (!cDMResultVarRecursiveVal->WriteToGraphQlObject(complexUnionDataObject)){
				return false;
			}
			complexUnionDataObject.InsertParam("__typename", QVariant("CDMResultVarRecursive"));
		}
		gqlObject.InsertParam("complexUnion", complexUnionDataObject);
	}

	if (mixedUnion){
		::imtgql::CGqlParamObject mixedUnionDataObject;
		if (const CCoordinates* coordinatesVal = std::get_if<CCoordinates>(mixedUnion.GetPtr())){
			if (!coordinatesVal->WriteToGraphQlObject(mixedUnionDataObject)){
				return false;
			}
			mixedUnionDataObject.InsertParam("__typename", QVariant("Coordinates"));
		}
		else if (const double* doubleVal = std::get_if<double>(mixedUnion.GetPtr())){
			mixedUnionDataObject.InsertParam("mixedUnion", *doubleVal);
		}
		else if (const bool* booleanVal = std::get_if<bool>(mixedUnion.GetPtr())){
			mixedUnionDataObject.InsertParam("mixedUnion", *booleanVal);
		}
		else if (const QString* stringVal = std::get_if<QString>(mixedUnion.GetPtr())){
			mixedUnionDataObject.InsertParam("mixedUnion", *stringVal);
		}
		gqlObject.InsertParam("mixedUnion", mixedUnionDataObject);
	}

	gqlObject.InsertParam("__typename", QVariant("UnionTestingType"));

	return true;
}


bool CUnionTestingType::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("simpleUnion")){
		QVariant simpleUnionVariantValue = gqlObject["simpleUnion"];
		if (simpleUnionVariantValue.canConvert<double>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionVariantValue.value<double>());
		}
		else if (simpleUnionVariantValue.canConvert<bool>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionVariantValue.value<bool>());
		}
		else if (simpleUnionVariantValue.canConvert<QString>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionVariantValue.value<QString>());
		}
	}

	if (gqlObject.ContainsParam("complexUnion")){
		const ::imtgql::CGqlParamObject* complexUnionDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("complexUnion");
		if (!complexUnionDataObjectPtr->ContainsParam("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "complexUnion").toLocal8Bit().constData();)

			return false;
		}
		QString complexUnionTypename = complexUnionDataObjectPtr->GetParamArgumentValue("__typename").toString();
		if (complexUnionTypename == "CDMResultVarString") {
			CCDMResultVarString complexUnionConvert;
			const bool isComplexUnionRead = complexUnionConvert.ReadFromGraphQlObject(*complexUnionDataObjectPtr);
			if (!isComplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
		else if (complexUnionTypename == "CDMResultVarRecursive") {
			CCDMResultVarRecursive complexUnionConvert;
			const bool isComplexUnionRead = complexUnionConvert.ReadFromGraphQlObject(*complexUnionDataObjectPtr);
			if (!isComplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
	}

	if (gqlObject.ContainsParam("mixedUnion")){
		const ::imtgql::CGqlParamObject* mixedUnionDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("mixedUnion");
		if (!mixedUnionDataObjectPtr->ContainsParam("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedUnion").toLocal8Bit().constData();)

			return false;
		}
		QString mixedUnionTypename = mixedUnionDataObjectPtr->GetParamArgumentValue("__typename").toString();
		QVariant mixedUnionVariantValue = gqlObject["mixedUnion"];
		if (mixedUnionTypename == "Coordinates") {
			CCoordinates mixedUnionConvert;
			const bool isMixedUnionRead = mixedUnionConvert.ReadFromGraphQlObject(*mixedUnionDataObjectPtr);
			if (!isMixedUnionRead){
				return false;
			}
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionConvert);
		}
		else if (mixedUnionVariantValue.canConvert<double>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionVariantValue.value<double>());
		}
		else if (mixedUnionVariantValue.canConvert<bool>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionVariantValue.value<bool>());
		}
		else if (mixedUnionVariantValue.canConvert<QString>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionVariantValue.value<QString>());
		}
	}

	return true;
}


bool CUnionTestingType::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("simpleUnion")){
		QVariant simpleUnionVariantValue = gqlObject["simpleUnion"];
		if (simpleUnionVariantValue.canConvert<double>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionVariantValue.value<double>());
		}
		else if (simpleUnionVariantValue.canConvert<bool>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionVariantValue.value<bool>());
		}
		else if (simpleUnionVariantValue.canConvert<QString>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionVariantValue.value<QString>());
		}
	}

	if (gqlObject.ContainsParam("complexUnion")){
		const ::imtgql::CGqlParamObject* complexUnionDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("complexUnion");
		if (!complexUnionDataObjectPtr->ContainsParam("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "complexUnion").toLocal8Bit().constData();)

			return false;
		}
		QString complexUnionTypename = complexUnionDataObjectPtr->GetParamArgumentValue("__typename").toString();
		if (complexUnionTypename == "CDMResultVarString") {
			CCDMResultVarString complexUnionConvert;
			const bool isComplexUnionRead = complexUnionConvert.ReadFromGraphQlObject(*complexUnionDataObjectPtr);
			if (!isComplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
		else if (complexUnionTypename == "CDMResultVarRecursive") {
			CCDMResultVarRecursive complexUnionConvert;
			const bool isComplexUnionRead = complexUnionConvert.ReadFromGraphQlObject(*complexUnionDataObjectPtr);
			if (!isComplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
	}

	if (gqlObject.ContainsParam("mixedUnion")){
		const ::imtgql::CGqlParamObject* mixedUnionDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("mixedUnion");
		if (!mixedUnionDataObjectPtr->ContainsParam("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedUnion").toLocal8Bit().constData();)

			return false;
		}
		QString mixedUnionTypename = mixedUnionDataObjectPtr->GetParamArgumentValue("__typename").toString();
		QVariant mixedUnionVariantValue = gqlObject["mixedUnion"];
		if (mixedUnionTypename == "Coordinates") {
			CCoordinates mixedUnionConvert;
			const bool isMixedUnionRead = mixedUnionConvert.ReadFromGraphQlObject(*mixedUnionDataObjectPtr);
			if (!isMixedUnionRead){
				return false;
			}
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionConvert);
		}
		else if (mixedUnionVariantValue.canConvert<double>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionVariantValue.value<double>());
		}
		else if (mixedUnionVariantValue.canConvert<bool>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionVariantValue.value<bool>());
		}
		else if (mixedUnionVariantValue.canConvert<QString>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionVariantValue.value<QString>());
		}
	}

	return true;
}


bool CUnionTestingType::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (simpleUnion){
		if (const double* doubleVal = std::get_if<double>(simpleUnion.GetPtr())){
			if (!simpleUnion){
				return false;
			}
			jsonObject["simpleUnion"] = QJsonValue::fromVariant(*doubleVal);
		}
		else if (const bool* booleanVal = std::get_if<bool>(simpleUnion.GetPtr())){
			if (!simpleUnion){
				return false;
			}
			jsonObject["simpleUnion"] = QJsonValue::fromVariant(*booleanVal);
		}
		else if (const QString* stringVal = std::get_if<QString>(simpleUnion.GetPtr())){
			if (!simpleUnion){
				return false;
			}
			jsonObject["simpleUnion"] = QJsonValue::fromVariant(*stringVal);
		}

	}

	if (complexUnion){
		if (const CCDMResultVarString* cDMResultVarStringVal = std::get_if<CCDMResultVarString>(complexUnion.GetPtr())){
			QJsonObject complexUnionJsonObject;
			const bool iscomplexUnionAdded = cDMResultVarStringVal->WriteToJsonObject(complexUnionJsonObject);
			if (!iscomplexUnionAdded){
				return false;
			}
			complexUnionJsonObject["__typename"] = "CDMResultVarString";
			jsonObject["complexUnion"] = complexUnionJsonObject;
		}
		else if (const CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<CCDMResultVarRecursive>(complexUnion.GetPtr())){
			QJsonObject complexUnionJsonObject;
			const bool iscomplexUnionAdded = cDMResultVarRecursiveVal->WriteToJsonObject(complexUnionJsonObject);
			if (!iscomplexUnionAdded){
				return false;
			}
			complexUnionJsonObject["__typename"] = "CDMResultVarRecursive";
			jsonObject["complexUnion"] = complexUnionJsonObject;
		}

	}

	if (mixedUnion){
		if (const CCoordinates* coordinatesVal = std::get_if<CCoordinates>(mixedUnion.GetPtr())){
			QJsonObject mixedUnionJsonObject;
			const bool ismixedUnionAdded = coordinatesVal->WriteToJsonObject(mixedUnionJsonObject);
			if (!ismixedUnionAdded){
				return false;
			}
			mixedUnionJsonObject["__typename"] = "Coordinates";
			jsonObject["mixedUnion"] = mixedUnionJsonObject;
		}
		else if (const double* doubleVal = std::get_if<double>(mixedUnion.GetPtr())){
			if (!mixedUnion){
				return false;
			}
			jsonObject["mixedUnion"] = QJsonValue::fromVariant(*doubleVal);
		}
		else if (const bool* booleanVal = std::get_if<bool>(mixedUnion.GetPtr())){
			if (!mixedUnion){
				return false;
			}
			jsonObject["mixedUnion"] = QJsonValue::fromVariant(*booleanVal);
		}
		else if (const QString* stringVal = std::get_if<QString>(mixedUnion.GetPtr())){
			if (!mixedUnion){
				return false;
			}
			jsonObject["mixedUnion"] = QJsonValue::fromVariant(*stringVal);
		}

	}

	jsonObject["__typename"] = "UnionTestingType";

	return true;
}


bool CUnionTestingType::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("simpleUnion")){
		QVariant simpleUnionVariantValue = jsonObject["simpleUnion"].toVariant();
		if (simpleUnionVariantValue.canConvert<double>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionVariantValue.value<double>());
		}
		else if (simpleUnionVariantValue.canConvert<bool>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionVariantValue.value<bool>());
		}
		else if (simpleUnionVariantValue.canConvert<QString>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionVariantValue.value<QString>());
		}
	}

	if (jsonObject.contains("complexUnion")){
		const QJsonObject complexUnionObject = jsonObject.value("complexUnion").toObject();
		if (!jsonObject.value("complexUnion").isObject() || !complexUnionObject.contains("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "complexUnion").toLocal8Bit().constData();)

			return false;
		}
		QString complexUnionTypename = complexUnionObject.value("__typename").toString();
		if (complexUnionTypename == "CDMResultVarString") {
			CCDMResultVarString complexUnionConvert;
			const bool iscomplexUnionRead = complexUnionConvert.ReadFromJsonObject(jsonObject["complexUnion"].toObject());
			if (!iscomplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
		else if (complexUnionTypename == "CDMResultVarRecursive") {
			CCDMResultVarRecursive complexUnionConvert;
			const bool iscomplexUnionRead = complexUnionConvert.ReadFromJsonObject(jsonObject["complexUnion"].toObject());
			if (!iscomplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
	}

	if (jsonObject.contains("mixedUnion")){
		const QJsonObject mixedUnionObject = jsonObject.value("mixedUnion").toObject();
		if (!jsonObject.value("mixedUnion").isObject() || !mixedUnionObject.contains("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedUnion").toLocal8Bit().constData();)

			return false;
		}
		QString mixedUnionTypename = mixedUnionObject.value("__typename").toString();
		QVariant mixedUnionVariantValue = jsonObject["mixedUnion"].toVariant();
		if (mixedUnionTypename == "Coordinates") {
			CCoordinates mixedUnionConvert;
			const bool ismixedUnionRead = mixedUnionConvert.ReadFromJsonObject(jsonObject["mixedUnion"].toObject());
			if (!ismixedUnionRead){
				return false;
			}
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionConvert);
		}
		else if (mixedUnionVariantValue.canConvert<double>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionVariantValue.value<double>());
		}
		else if (mixedUnionVariantValue.canConvert<bool>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionVariantValue.value<bool>());
		}
		else if (mixedUnionVariantValue.canConvert<QString>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionVariantValue.value<QString>());
		}
	}

	return true;
}


bool CUnionTestingType::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("simpleUnion")){
		QVariant simpleUnionVariantValue = jsonObject["simpleUnion"].toVariant();
		if (simpleUnionVariantValue.canConvert<double>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionVariantValue.value<double>());
		}
		else if (simpleUnionVariantValue.canConvert<bool>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionVariantValue.value<bool>());
		}
		else if (simpleUnionVariantValue.canConvert<QString>()){
			simpleUnion = ExtendedMetaDataUnionSimpleType(simpleUnionVariantValue.value<QString>());
		}
	}

	if (jsonObject.contains("complexUnion")){
		const QJsonObject complexUnionObject = jsonObject.value("complexUnion").toObject();
		if (!jsonObject.value("complexUnion").isObject() || !complexUnionObject.contains("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "complexUnion").toLocal8Bit().constData();)

			return false;
		}
		QString complexUnionTypename = complexUnionObject.value("__typename").toString();
		if (complexUnionTypename == "CDMResultVarString") {
			CCDMResultVarString complexUnionConvert;
			const bool iscomplexUnionRead = complexUnionConvert.ReadFromJsonObject(jsonObject["complexUnion"].toObject());
			if (!iscomplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
		else if (complexUnionTypename == "CDMResultVarRecursive") {
			CCDMResultVarRecursive complexUnionConvert;
			const bool iscomplexUnionRead = complexUnionConvert.ReadFromJsonObject(jsonObject["complexUnion"].toObject());
			if (!iscomplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
	}

	if (jsonObject.contains("mixedUnion")){
		const QJsonObject mixedUnionObject = jsonObject.value("mixedUnion").toObject();
		if (!jsonObject.value("mixedUnion").isObject() || !mixedUnionObject.contains("__typename")){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedUnion").toLocal8Bit().constData();)

			return false;
		}
		QString mixedUnionTypename = mixedUnionObject.value("__typename").toString();
		QVariant mixedUnionVariantValue = jsonObject["mixedUnion"].toVariant();
		if (mixedUnionTypename == "Coordinates") {
			CCoordinates mixedUnionConvert;
			const bool ismixedUnionRead = mixedUnionConvert.ReadFromJsonObject(jsonObject["mixedUnion"].toObject());
			if (!ismixedUnionRead){
				return false;
			}
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionConvert);
		}
		else if (mixedUnionVariantValue.canConvert<double>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionVariantValue.value<double>());
		}
		else if (mixedUnionVariantValue.canConvert<bool>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionVariantValue.value<bool>());
		}
		else if (mixedUnionVariantValue.canConvert<QString>()){
			mixedUnion = ExtendedMetaDataUnionType(mixedUnionVariantValue.value<QString>());
		}
	}

	return true;
}


// serialize methods

QByteArray CCDMResultVarRecursive::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CCDMResultVarRecursive::operator==(const CCDMResultVarRecursive& other) const
{
	return 
				dataModelTitle == other.dataModelTitle &&
				dataModelVersion == other.dataModelVersion &&
				resultMetaData == other.resultMetaData &&
				resultContent == other.resultContent &&
				overallResultValues == other.overallResultValues;
}


bool CCDMResultVarRecursive::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "CDMResultVarRecursive", modelIndex);

	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("dataModelTitle", *dataModelTitle, modelIndex);

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("dataModelVersion", *dataModelVersion, modelIndex);


	if (resultMetaData){
		::imtbase::CTreeItemModel* resultMetaDataNewModelPtr = model.AddTreeModel("resultMetaData", modelIndex);
		const bool isResultMetaDataAdded = resultMetaData->WriteToModel(*resultMetaDataNewModelPtr, 0);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}

	}
	if (resultContent){
		::imtbase::CTreeItemModel* newResultContentModelPtr = model.AddTreeModel("resultContent", modelIndex);
		newResultContentModelPtr->setIsArray(true);
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			QVariant resultContentVariantValue;
			if (const CCDMResultVarString* cDMResultVarStringVal = std::get_if<CCDMResultVarString>(resultContent->at(resultContentIndex).GetPtr())){
				newResultContentModelPtr->InsertNewItem();
				if(!cDMResultVarStringVal->WriteToModel(*newResultContentModelPtr, resultContentIndex)){
					return false;
				}
				newResultContentModelPtr->SetData("__typename", "CDMResultVarString", resultContentIndex);
			}
			else if (const CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<CCDMResultVarRecursive>(resultContent->at(resultContentIndex).GetPtr())){
				newResultContentModelPtr->InsertNewItem();
				if(!cDMResultVarRecursiveVal->WriteToModel(*newResultContentModelPtr, resultContentIndex)){
					return false;
				}
				newResultContentModelPtr->SetData("__typename", "CDMResultVarRecursive", resultContentIndex);
			}
		}
	}

	if (overallResultValues){
		::imtbase::CTreeItemModel* newOverallResultValuesModelPtr = model.AddTreeModel("overallResultValues", modelIndex);
		newOverallResultValuesModelPtr->setIsArray(true);
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			newOverallResultValuesModelPtr->InsertNewItem();
			if (!(overallResultValues->at(overallResultValuesIndex)->WriteToModel(*newOverallResultValuesModelPtr, overallResultValuesIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
		}
	}

	return true;
}


bool CCDMResultVarRecursive::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant dataModelTitleData = model.GetData("dataModelTitle", modelIndex);
	if (dataModelTitleData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = dataModelTitleData.toString();

	QVariant dataModelVersionData = model.GetData("dataModelVersion", modelIndex);
	if (dataModelVersionData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = dataModelVersionData.toString();

	::imtbase::CTreeItemModel* resultMetaDataDataModelPtr = model.GetTreeItemModel("resultMetaData", modelIndex);
	if (resultMetaDataDataModelPtr != nullptr){
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		imtsdl::TElementList<CDMResultUnionType> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			CDMResultUnionType resultContentData;
			QVariant resultContentVariantValue = resultContentModel->GetData(QByteArray(), resultContentIndex);
			QString resultContentDataTypename = resultContentModel->GetData("__typename", resultContentIndex).toString();
			if (resultContentDataTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = CDMResultUnionType(resultContentDataConvert);
			}
			else if (resultContentDataTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = CDMResultUnionType(resultContentDataConvert);
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
		imtsdl::TElementList<COverallResultValues> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			COverallResultValues t_overallResultValues;
			if (!t_overallResultValues.ReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesList << t_overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool CCDMResultVarRecursive::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		imtsdl::TElementList<CDMResultUnionType> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			CDMResultUnionType resultContentData;
			QVariant resultContentVariantValue = resultContentModel->GetData(QByteArray(), resultContentIndex);
			QString resultContentDataTypename = resultContentModel->GetData("__typename", resultContentIndex).toString();
			if (resultContentDataTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = CDMResultUnionType(resultContentDataConvert);
			}
			else if (resultContentDataTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = CDMResultUnionType(resultContentDataConvert);
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
		imtsdl::TElementList<COverallResultValues> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			COverallResultValues t_overallResultValues;
			if (!t_overallResultValues.OptReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesList << t_overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool CCDMResultVarRecursive::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("dataModelTitle", QVariant(*dataModelTitle));

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("dataModelVersion", QVariant(*dataModelVersion));

	if (resultMetaData){
		::imtgql::CGqlParamObject resultMetaDataGqlObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToGraphQlObject(resultMetaDataGqlObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("resultMetaData", resultMetaDataGqlObject);
	}

	if (resultContent){
		QList<::imtgql::CGqlParamObject> resultContentDataObjectList;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			::imtgql::CGqlParamObject resultContentDataObject;
			if (const CCDMResultVarString* cDMResultVarStringVal = std::get_if<CCDMResultVarString>(resultContent->at(resultContentIndex).GetPtr())){
				if (!cDMResultVarStringVal->WriteToGraphQlObject(resultContentDataObject)){
					return false;
				}
				resultContentDataObject.InsertParam("__typename", QVariant("CDMResultVarString"));
			}
			else if (const CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<CCDMResultVarRecursive>(resultContent->at(resultContentIndex).GetPtr())){
				if (!cDMResultVarRecursiveVal->WriteToGraphQlObject(resultContentDataObject)){
					return false;
				}
				resultContentDataObject.InsertParam("__typename", QVariant("CDMResultVarRecursive"));
			}
			resultContentDataObjectList << resultContentDataObject;
		}
		gqlObject.InsertParam("resultContent", resultContentDataObjectList);
	}

	if (overallResultValues){
		QList<::imtgql::CGqlParamObject> overallResultValuesDataObjectList;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			::imtgql::CGqlParamObject newOverallResultValuesGqlObject;
			if (!overallResultValues->at(overallResultValuesIndex)->WriteToGraphQlObject(newOverallResultValuesGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesDataObjectList << newOverallResultValuesGqlObject;
		}
		gqlObject.InsertParam("overallResultValues", overallResultValuesDataObjectList);
	}

	gqlObject.InsertParam("__typename", QVariant("CDMResultVarRecursive"));

	return true;
}


bool CCDMResultVarRecursive::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("dataModelTitle") || (gqlObject["dataModelTitle"].userType() != QMetaType::QString && gqlObject["dataModelTitle"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = gqlObject["dataModelTitle"].toString();

	if (!gqlObject.ContainsParam("dataModelVersion") || (gqlObject["dataModelVersion"].userType() != QMetaType::QString && gqlObject["dataModelVersion"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = gqlObject["dataModelVersion"].toString();

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent")){
		resultContent.emplace();
	}
	if (gqlObject.ContainsParam("resultContent")){
		const QList<const ::imtgql::CGqlParamObject*> resultContentDataList = gqlObject.GetParamArgumentObjectPtrList("resultContent");
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = imtsdl::TElementList<CDMResultUnionType>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			const ::imtgql::CGqlParamObject* tempResultContent = resultContentDataList[resultContentIndex];
			if (tempResultContent == nullptr){
				return false;
			}
			QString resultContentDataValueTypename = tempResultContent->GetParamArgumentValue("__typename").toString();
			CDMResultUnionType resultContentDataValue;
			if (resultContentDataValueTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataValueConvert;
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*tempResultContent);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else if (resultContentDataValueTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataValueConvert;
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*tempResultContent);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else{
				return false;
			}
			resultContent->append(resultContentDataValue);
		}
	}

	if (gqlObject.ContainsParam("overallResultValues")){
		overallResultValues.emplace();
	}
	if (gqlObject.ContainsParam("overallResultValues") && (gqlObject.GetObjectsCount("overallResultValues") > 0)){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = imtsdl::TElementList<COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr){
				qDebug() << "invalid type" << overallResultValuesDataObjectPtr;
				return false;
			}
			COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResultVarRecursive::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("dataModelTitle") && (gqlObject["dataModelTitle"].userType() == QMetaType::QString || gqlObject["dataModelTitle"].userType() == QMetaType::QByteArray)){
		dataModelTitle = gqlObject["dataModelTitle"].toString();
	}

	if (gqlObject.ContainsParam("dataModelVersion") && (gqlObject["dataModelVersion"].userType() == QMetaType::QString || gqlObject["dataModelVersion"].userType() == QMetaType::QByteArray)){
		dataModelVersion = gqlObject["dataModelVersion"].toString();
	}

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent")){
		resultContent.emplace();
	}
	if (gqlObject.ContainsParam("resultContent")){
		const QList<const ::imtgql::CGqlParamObject*> resultContentDataList = gqlObject.GetParamArgumentObjectPtrList("resultContent");
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = imtsdl::TElementList<CDMResultUnionType>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			const ::imtgql::CGqlParamObject* tempResultContent = resultContentDataList[resultContentIndex];
			if (tempResultContent == nullptr){
				return false;
			}
			QString resultContentDataValueTypename = tempResultContent->GetParamArgumentValue("__typename").toString();
			CDMResultUnionType resultContentDataValue;
			if (resultContentDataValueTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataValueConvert;
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*tempResultContent);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else if (resultContentDataValueTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataValueConvert;
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*tempResultContent);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else{
				return false;
			}
			resultContent->append(resultContentDataValue);
		}
	}

	if (gqlObject.ContainsParam("overallResultValues")){
		overallResultValues.emplace();
	}
	if (gqlObject.ContainsParam("overallResultValues") && (gqlObject.GetObjectsCount("overallResultValues") > 0)){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = imtsdl::TElementList<COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr){
				qDebug() << "invalid type" << overallResultValuesDataObjectPtr;
				return false;
			}
			COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResultVarRecursive::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["dataModelTitle"] = QJsonValue::fromVariant(*dataModelTitle);

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["dataModelVersion"] = QJsonValue::fromVariant(*dataModelVersion);

	if (resultMetaData){
		QJsonObject resultMetaDataJsonObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToJsonObject(resultMetaDataJsonObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["resultMetaData"] = resultMetaDataJsonObject;
	}

	if (resultContent){
		QJsonArray newResultContentArray;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			if (const CCDMResultVarString* cDMResultVarStringVal = std::get_if<CCDMResultVarString>(resultContent->at(resultContentIndex).GetPtr())){
				QJsonObject resultContentJsonObject;
				const bool isresultContentAdded = cDMResultVarStringVal->WriteToJsonObject(resultContentJsonObject);
				if (!isresultContentAdded){
					return false;
				}
				resultContentJsonObject["__typename"] = "CDMResultVarString";
				newResultContentArray << resultContentJsonObject;
			}
			else if (const CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<CCDMResultVarRecursive>(resultContent->at(resultContentIndex).GetPtr())){
				QJsonObject resultContentJsonObject;
				const bool isresultContentAdded = cDMResultVarRecursiveVal->WriteToJsonObject(resultContentJsonObject);
				if (!isresultContentAdded){
					return false;
				}
				resultContentJsonObject["__typename"] = "CDMResultVarRecursive";
				newResultContentArray << resultContentJsonObject;
			}
		}
		jsonObject["resultContent"] = newResultContentArray;
	}

	if (overallResultValues){
		QJsonArray newOverallResultValuesArray;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			QJsonObject newOverallResultValuesJsonObject;
			if (!overallResultValues->at(overallResultValuesIndex)->WriteToJsonObject(newOverallResultValuesJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			newOverallResultValuesArray << newOverallResultValuesJsonObject;
		}
		jsonObject["overallResultValues"] = newOverallResultValuesArray;
	}

	jsonObject["__typename"] = "CDMResultVarRecursive";

	return true;
}


bool CCDMResultVarRecursive::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("dataModelTitle") || ! jsonObject["dataModelTitle"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = jsonObject["dataModelTitle"].toString();

	if (!jsonObject.contains("dataModelVersion") || ! jsonObject["dataModelVersion"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = jsonObject["dataModelVersion"].toString();

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("resultContent")){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = imtsdl::TElementList<CDMResultUnionType>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			const QVariant tempResultContent = resultContentJsonArray[resultContentIndex].toVariant();
			QString resultContentDataValueTypename = resultContentJsonArray[resultContentIndex].toObject().value("__typename").toString();
			CDMResultUnionType resultContentDataValue;
			if (resultContentDataValueTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else if (resultContentDataValueTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
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
		overallResultValues = imtsdl::TElementList<COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResultVarRecursive::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("dataModelTitle") && jsonObject["dataModelTitle"].isString()){
		dataModelTitle = jsonObject["dataModelTitle"].toString();
	}

	if (jsonObject.contains("dataModelVersion") && jsonObject["dataModelVersion"].isString()){
		dataModelVersion = jsonObject["dataModelVersion"].toString();
	}

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("resultContent")){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = imtsdl::TElementList<CDMResultUnionType>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			const QVariant tempResultContent = resultContentJsonArray[resultContentIndex].toVariant();
			QString resultContentDataValueTypename = resultContentJsonArray[resultContentIndex].toObject().value("__typename").toString();
			CDMResultUnionType resultContentDataValue;
			if (resultContentDataValueTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else if (resultContentDataValueTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
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
		overallResultValues = imtsdl::TElementList<COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


// serialize methods

QByteArray CCDMResultVarString::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CCDMResultVarString::operator==(const CCDMResultVarString& other) const
{
	return 
				dataModelTitle == other.dataModelTitle &&
				dataModelVersion == other.dataModelVersion &&
				resultMetaData == other.resultMetaData &&
				resultContent == other.resultContent &&
				overallResultValues == other.overallResultValues;
}


bool CCDMResultVarString::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "CDMResultVarString", modelIndex);

	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("dataModelTitle", *dataModelTitle, modelIndex);

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("dataModelVersion", *dataModelVersion, modelIndex);


	if (resultMetaData){
		::imtbase::CTreeItemModel* resultMetaDataNewModelPtr = model.AddTreeModel("resultMetaData", modelIndex);
		const bool isResultMetaDataAdded = resultMetaData->WriteToModel(*resultMetaDataNewModelPtr, 0);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}

	}
	if (resultContent){
		::imtbase::CTreeItemModel* newResultContentModelPtr = model.AddTreeModel("resultContent", modelIndex);
		newResultContentModelPtr->setIsArray(true);
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			newResultContentModelPtr->InsertNewItem();
			newResultContentModelPtr->SetData(QByteArray(), *resultContent->at(resultContentIndex), resultContentIndex);
		}
	}

	if (overallResultValues){
		::imtbase::CTreeItemModel* newOverallResultValuesModelPtr = model.AddTreeModel("overallResultValues", modelIndex);
		newOverallResultValuesModelPtr->setIsArray(true);
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			newOverallResultValuesModelPtr->InsertNewItem();
			if (!(overallResultValues->at(overallResultValuesIndex)->WriteToModel(*newOverallResultValuesModelPtr, overallResultValuesIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
		}
	}

	return true;
}


bool CCDMResultVarString::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant dataModelTitleData = model.GetData("dataModelTitle", modelIndex);
	if (dataModelTitleData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = dataModelTitleData.toString();

	QVariant dataModelVersionData = model.GetData("dataModelVersion", modelIndex);
	if (dataModelVersionData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = dataModelVersionData.toString();

	::imtbase::CTreeItemModel* resultMetaDataDataModelPtr = model.GetTreeItemModel("resultMetaData", modelIndex);
	if (resultMetaDataDataModelPtr != nullptr){
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		imtsdl::TElementList<QString> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			QString t_resultContent = resultContentModel->GetData(QByteArray(), resultContentIndex).toString();
			resultContentList << t_resultContent;
		}
		resultContent = resultContentList;

	}

	::imtbase::CTreeItemModel* overallResultValuesModel = model.GetTreeItemModel("overallResultValues", modelIndex);
	if (overallResultValuesModel != nullptr){
		int overallResultValuesCount = overallResultValuesModel->GetItemsCount();
		imtsdl::TElementList<COverallResultValues> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			COverallResultValues t_overallResultValues;
			if (!t_overallResultValues.ReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesList << t_overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool CCDMResultVarString::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		imtsdl::TElementList<QString> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			QString t_resultContent = resultContentModel->GetData(QByteArray(), resultContentIndex).toString();
			resultContentList << t_resultContent;
		}
		resultContent = resultContentList;

	}

	::imtbase::CTreeItemModel* overallResultValuesModel = model.GetTreeItemModel("overallResultValues", modelIndex);
	if (overallResultValuesModel != nullptr){
		int overallResultValuesCount = overallResultValuesModel->GetItemsCount();
		imtsdl::TElementList<COverallResultValues> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			COverallResultValues t_overallResultValues;
			if (!t_overallResultValues.OptReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesList << t_overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool CCDMResultVarString::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("dataModelTitle", QVariant(*dataModelTitle));

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("dataModelVersion", QVariant(*dataModelVersion));

	if (resultMetaData){
		::imtgql::CGqlParamObject resultMetaDataGqlObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToGraphQlObject(resultMetaDataGqlObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("resultMetaData", resultMetaDataGqlObject);
	}

	if (resultContent){
		QVariantList resultContentDataObjectList;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			resultContentDataObjectList << *resultContent->at(resultContentIndex);
		}
		gqlObject.InsertParam("resultContent", resultContentDataObjectList);
	}

	if (overallResultValues){
		QList<::imtgql::CGqlParamObject> overallResultValuesDataObjectList;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			::imtgql::CGqlParamObject newOverallResultValuesGqlObject;
			if (!overallResultValues->at(overallResultValuesIndex)->WriteToGraphQlObject(newOverallResultValuesGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesDataObjectList << newOverallResultValuesGqlObject;
		}
		gqlObject.InsertParam("overallResultValues", overallResultValuesDataObjectList);
	}

	gqlObject.InsertParam("__typename", QVariant("CDMResultVarString"));

	return true;
}


bool CCDMResultVarString::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("dataModelTitle") || (gqlObject["dataModelTitle"].userType() != QMetaType::QString && gqlObject["dataModelTitle"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = gqlObject["dataModelTitle"].toString();

	if (!gqlObject.ContainsParam("dataModelVersion") || (gqlObject["dataModelVersion"].userType() != QMetaType::QString && gqlObject["dataModelVersion"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = gqlObject["dataModelVersion"].toString();

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent")){
		resultContent.emplace();
	}
	if (gqlObject.ContainsParam("resultContent") && !(gqlObject["resultContent"].isNull())){
		const QVariant resultContentData = gqlObject["resultContent"];
		const QVariantList resultContentDataList = resultContentData.toList();
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = imtsdl::TElementList<QString>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			QString tempResultContent = resultContentDataList[resultContentIndex].toString();
			resultContent->append(tempResultContent);
		}
	}

	if (gqlObject.ContainsParam("overallResultValues")){
		overallResultValues.emplace();
	}
	if (gqlObject.ContainsParam("overallResultValues") && (gqlObject.GetObjectsCount("overallResultValues") > 0)){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = imtsdl::TElementList<COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr){
				qDebug() << "invalid type" << overallResultValuesDataObjectPtr;
				return false;
			}
			COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResultVarString::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("dataModelTitle") && (gqlObject["dataModelTitle"].userType() == QMetaType::QString || gqlObject["dataModelTitle"].userType() == QMetaType::QByteArray)){
		dataModelTitle = gqlObject["dataModelTitle"].toString();
	}

	if (gqlObject.ContainsParam("dataModelVersion") && (gqlObject["dataModelVersion"].userType() == QMetaType::QString || gqlObject["dataModelVersion"].userType() == QMetaType::QByteArray)){
		dataModelVersion = gqlObject["dataModelVersion"].toString();
	}

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent")){
		resultContent.emplace();
	}
	if (gqlObject.ContainsParam("resultContent") && !(gqlObject["resultContent"].isNull())){
		const QVariant resultContentData = gqlObject["resultContent"];
		const QVariantList resultContentDataList = resultContentData.toList();
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = imtsdl::TElementList<QString>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			QString tempResultContent = resultContentDataList[resultContentIndex].toString();
			resultContent->append(tempResultContent);
		}
	}

	if (gqlObject.ContainsParam("overallResultValues")){
		overallResultValues.emplace();
	}
	if (gqlObject.ContainsParam("overallResultValues") && (gqlObject.GetObjectsCount("overallResultValues") > 0)){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = imtsdl::TElementList<COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr){
				qDebug() << "invalid type" << overallResultValuesDataObjectPtr;
				return false;
			}
			COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResultVarString::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["dataModelTitle"] = QJsonValue::fromVariant(*dataModelTitle);

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["dataModelVersion"] = QJsonValue::fromVariant(*dataModelVersion);

	if (resultMetaData){
		QJsonObject resultMetaDataJsonObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToJsonObject(resultMetaDataJsonObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["resultMetaData"] = resultMetaDataJsonObject;
	}

	if (resultContent){
		QJsonArray newResultContentArray;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			newResultContentArray << *resultContent->at(resultContentIndex);
		}
		jsonObject["resultContent"] = newResultContentArray;
	}

	if (overallResultValues){
		QJsonArray newOverallResultValuesArray;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			QJsonObject newOverallResultValuesJsonObject;
			if (!overallResultValues->at(overallResultValuesIndex)->WriteToJsonObject(newOverallResultValuesJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			newOverallResultValuesArray << newOverallResultValuesJsonObject;
		}
		jsonObject["overallResultValues"] = newOverallResultValuesArray;
	}

	jsonObject["__typename"] = "CDMResultVarString";

	return true;
}


bool CCDMResultVarString::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("dataModelTitle") || ! jsonObject["dataModelTitle"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = jsonObject["dataModelTitle"].toString();

	if (!jsonObject.contains("dataModelVersion") || ! jsonObject["dataModelVersion"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = jsonObject["dataModelVersion"].toString();

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("resultContent") && jsonObject["resultContent"].isArray()){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = imtsdl::TElementList<QString>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			QString tempResultContent = resultContentJsonArray[resultContentIndex].toString();
			resultContent->append(tempResultContent);
		}
	}

	if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isArray()){
		const QJsonArray overallResultValuesJsonArray = jsonObject["overallResultValues"].toArray();
		const qsizetype overallResultValuesArrayCount = overallResultValuesJsonArray.size();
		overallResultValues = imtsdl::TElementList<COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResultVarString::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("dataModelTitle") && jsonObject["dataModelTitle"].isString()){
		dataModelTitle = jsonObject["dataModelTitle"].toString();
	}

	if (jsonObject.contains("dataModelVersion") && jsonObject["dataModelVersion"].isString()){
		dataModelVersion = jsonObject["dataModelVersion"].toString();
	}

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("resultContent") && jsonObject["resultContent"].isArray()){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = imtsdl::TElementList<QString>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			QString tempResultContent = resultContentJsonArray[resultContentIndex].toString();
			resultContent->append(tempResultContent);
		}
	}

	if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isArray()){
		const QJsonArray overallResultValuesJsonArray = jsonObject["overallResultValues"].toArray();
		const qsizetype overallResultValuesArrayCount = overallResultValuesJsonArray.size();
		overallResultValues = imtsdl::TElementList<COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


// serialize methods

QByteArray CCDMResult::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CCDMResult::operator==(const CCDMResult& other) const
{
	return 
				dataModelTitle == other.dataModelTitle &&
				dataModelVersion == other.dataModelVersion &&
				resultMetaData == other.resultMetaData &&
				resultContent == other.resultContent &&
				overallResultValues == other.overallResultValues;
}


bool CCDMResult::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "CDMResult", modelIndex);

	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("dataModelTitle", *dataModelTitle, modelIndex);

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("dataModelVersion", *dataModelVersion, modelIndex);


	if (resultMetaData){
		::imtbase::CTreeItemModel* resultMetaDataNewModelPtr = model.AddTreeModel("resultMetaData", modelIndex);
		const bool isResultMetaDataAdded = resultMetaData->WriteToModel(*resultMetaDataNewModelPtr, 0);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}

	}
	if (resultContent){
		::imtbase::CTreeItemModel* newResultContentModelPtr = model.AddTreeModel("resultContent", modelIndex);
		newResultContentModelPtr->setIsArray(true);
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			QVariant resultContentVariantValue;
			if (const CCDMResultVarString* cDMResultVarStringVal = std::get_if<CCDMResultVarString>(resultContent->at(resultContentIndex).GetPtr())){
				newResultContentModelPtr->InsertNewItem();
				if(!cDMResultVarStringVal->WriteToModel(*newResultContentModelPtr, resultContentIndex)){
					return false;
				}
				newResultContentModelPtr->SetData("__typename", "CDMResultVarString", resultContentIndex);
			}
			else if (const CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<CCDMResultVarRecursive>(resultContent->at(resultContentIndex).GetPtr())){
				newResultContentModelPtr->InsertNewItem();
				if(!cDMResultVarRecursiveVal->WriteToModel(*newResultContentModelPtr, resultContentIndex)){
					return false;
				}
				newResultContentModelPtr->SetData("__typename", "CDMResultVarRecursive", resultContentIndex);
			}
		}
	}

	if (overallResultValues){
		::imtbase::CTreeItemModel* newOverallResultValuesModelPtr = model.AddTreeModel("overallResultValues", modelIndex);
		newOverallResultValuesModelPtr->setIsArray(true);
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			newOverallResultValuesModelPtr->InsertNewItem();
			if (!(overallResultValues->at(overallResultValuesIndex)->WriteToModel(*newOverallResultValuesModelPtr, overallResultValuesIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
		}
	}

	return true;
}


bool CCDMResult::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant dataModelTitleData = model.GetData("dataModelTitle", modelIndex);
	if (dataModelTitleData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = dataModelTitleData.toString();

	QVariant dataModelVersionData = model.GetData("dataModelVersion", modelIndex);
	if (dataModelVersionData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = dataModelVersionData.toString();

	::imtbase::CTreeItemModel* resultMetaDataDataModelPtr = model.GetTreeItemModel("resultMetaData", modelIndex);
	if (resultMetaDataDataModelPtr != nullptr){
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		imtsdl::TElementList<CDMResultUnionType> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			CDMResultUnionType resultContentData;
			QVariant resultContentVariantValue = resultContentModel->GetData(QByteArray(), resultContentIndex);
			QString resultContentDataTypename = resultContentModel->GetData("__typename", resultContentIndex).toString();
			if (resultContentDataTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = CDMResultUnionType(resultContentDataConvert);
			}
			else if (resultContentDataTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = CDMResultUnionType(resultContentDataConvert);
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
		imtsdl::TElementList<COverallResultValues> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			COverallResultValues t_overallResultValues;
			if (!t_overallResultValues.ReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesList << t_overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool CCDMResult::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		imtsdl::TElementList<CDMResultUnionType> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			CDMResultUnionType resultContentData;
			QVariant resultContentVariantValue = resultContentModel->GetData(QByteArray(), resultContentIndex);
			QString resultContentDataTypename = resultContentModel->GetData("__typename", resultContentIndex).toString();
			if (resultContentDataTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = CDMResultUnionType(resultContentDataConvert);
			}
			else if (resultContentDataTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = CDMResultUnionType(resultContentDataConvert);
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
		imtsdl::TElementList<COverallResultValues> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			COverallResultValues t_overallResultValues;
			if (!t_overallResultValues.OptReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesList << t_overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool CCDMResult::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("dataModelTitle", QVariant(*dataModelTitle));

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("dataModelVersion", QVariant(*dataModelVersion));

	if (resultMetaData){
		::imtgql::CGqlParamObject resultMetaDataGqlObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToGraphQlObject(resultMetaDataGqlObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("resultMetaData", resultMetaDataGqlObject);
	}

	if (resultContent){
		QList<::imtgql::CGqlParamObject> resultContentDataObjectList;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			::imtgql::CGqlParamObject resultContentDataObject;
			if (const CCDMResultVarString* cDMResultVarStringVal = std::get_if<CCDMResultVarString>(resultContent->at(resultContentIndex).GetPtr())){
				if (!cDMResultVarStringVal->WriteToGraphQlObject(resultContentDataObject)){
					return false;
				}
				resultContentDataObject.InsertParam("__typename", QVariant("CDMResultVarString"));
			}
			else if (const CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<CCDMResultVarRecursive>(resultContent->at(resultContentIndex).GetPtr())){
				if (!cDMResultVarRecursiveVal->WriteToGraphQlObject(resultContentDataObject)){
					return false;
				}
				resultContentDataObject.InsertParam("__typename", QVariant("CDMResultVarRecursive"));
			}
			resultContentDataObjectList << resultContentDataObject;
		}
		gqlObject.InsertParam("resultContent", resultContentDataObjectList);
	}

	if (overallResultValues){
		QList<::imtgql::CGqlParamObject> overallResultValuesDataObjectList;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			::imtgql::CGqlParamObject newOverallResultValuesGqlObject;
			if (!overallResultValues->at(overallResultValuesIndex)->WriteToGraphQlObject(newOverallResultValuesGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesDataObjectList << newOverallResultValuesGqlObject;
		}
		gqlObject.InsertParam("overallResultValues", overallResultValuesDataObjectList);
	}

	gqlObject.InsertParam("__typename", QVariant("CDMResult"));

	return true;
}


bool CCDMResult::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("dataModelTitle") || (gqlObject["dataModelTitle"].userType() != QMetaType::QString && gqlObject["dataModelTitle"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = gqlObject["dataModelTitle"].toString();

	if (!gqlObject.ContainsParam("dataModelVersion") || (gqlObject["dataModelVersion"].userType() != QMetaType::QString && gqlObject["dataModelVersion"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = gqlObject["dataModelVersion"].toString();

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent")){
		resultContent.emplace();
	}
	if (gqlObject.ContainsParam("resultContent")){
		const QList<const ::imtgql::CGqlParamObject*> resultContentDataList = gqlObject.GetParamArgumentObjectPtrList("resultContent");
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = imtsdl::TElementList<CDMResultUnionType>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			const ::imtgql::CGqlParamObject* tempResultContent = resultContentDataList[resultContentIndex];
			if (tempResultContent == nullptr){
				return false;
			}
			QString resultContentDataValueTypename = tempResultContent->GetParamArgumentValue("__typename").toString();
			CDMResultUnionType resultContentDataValue;
			if (resultContentDataValueTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataValueConvert;
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*tempResultContent);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else if (resultContentDataValueTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataValueConvert;
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*tempResultContent);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else{
				return false;
			}
			resultContent->append(resultContentDataValue);
		}
	}

	if (gqlObject.ContainsParam("overallResultValues")){
		overallResultValues.emplace();
	}
	if (gqlObject.ContainsParam("overallResultValues") && (gqlObject.GetObjectsCount("overallResultValues") > 0)){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = imtsdl::TElementList<COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr){
				qDebug() << "invalid type" << overallResultValuesDataObjectPtr;
				return false;
			}
			COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResult::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("dataModelTitle") && (gqlObject["dataModelTitle"].userType() == QMetaType::QString || gqlObject["dataModelTitle"].userType() == QMetaType::QByteArray)){
		dataModelTitle = gqlObject["dataModelTitle"].toString();
	}

	if (gqlObject.ContainsParam("dataModelVersion") && (gqlObject["dataModelVersion"].userType() == QMetaType::QString || gqlObject["dataModelVersion"].userType() == QMetaType::QByteArray)){
		dataModelVersion = gqlObject["dataModelVersion"].toString();
	}

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent")){
		resultContent.emplace();
	}
	if (gqlObject.ContainsParam("resultContent")){
		const QList<const ::imtgql::CGqlParamObject*> resultContentDataList = gqlObject.GetParamArgumentObjectPtrList("resultContent");
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = imtsdl::TElementList<CDMResultUnionType>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			const ::imtgql::CGqlParamObject* tempResultContent = resultContentDataList[resultContentIndex];
			if (tempResultContent == nullptr){
				return false;
			}
			QString resultContentDataValueTypename = tempResultContent->GetParamArgumentValue("__typename").toString();
			CDMResultUnionType resultContentDataValue;
			if (resultContentDataValueTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataValueConvert;
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*tempResultContent);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else if (resultContentDataValueTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataValueConvert;
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*tempResultContent);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else{
				return false;
			}
			resultContent->append(resultContentDataValue);
		}
	}

	if (gqlObject.ContainsParam("overallResultValues")){
		overallResultValues.emplace();
	}
	if (gqlObject.ContainsParam("overallResultValues") && (gqlObject.GetObjectsCount("overallResultValues") > 0)){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = imtsdl::TElementList<COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr){
				qDebug() << "invalid type" << overallResultValuesDataObjectPtr;
				return false;
			}
			COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResult::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["dataModelTitle"] = QJsonValue::fromVariant(*dataModelTitle);

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["dataModelVersion"] = QJsonValue::fromVariant(*dataModelVersion);

	if (resultMetaData){
		QJsonObject resultMetaDataJsonObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToJsonObject(resultMetaDataJsonObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["resultMetaData"] = resultMetaDataJsonObject;
	}

	if (resultContent){
		QJsonArray newResultContentArray;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			if (const CCDMResultVarString* cDMResultVarStringVal = std::get_if<CCDMResultVarString>(resultContent->at(resultContentIndex).GetPtr())){
				QJsonObject resultContentJsonObject;
				const bool isresultContentAdded = cDMResultVarStringVal->WriteToJsonObject(resultContentJsonObject);
				if (!isresultContentAdded){
					return false;
				}
				resultContentJsonObject["__typename"] = "CDMResultVarString";
				newResultContentArray << resultContentJsonObject;
			}
			else if (const CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<CCDMResultVarRecursive>(resultContent->at(resultContentIndex).GetPtr())){
				QJsonObject resultContentJsonObject;
				const bool isresultContentAdded = cDMResultVarRecursiveVal->WriteToJsonObject(resultContentJsonObject);
				if (!isresultContentAdded){
					return false;
				}
				resultContentJsonObject["__typename"] = "CDMResultVarRecursive";
				newResultContentArray << resultContentJsonObject;
			}
		}
		jsonObject["resultContent"] = newResultContentArray;
	}

	if (overallResultValues){
		QJsonArray newOverallResultValuesArray;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			QJsonObject newOverallResultValuesJsonObject;
			if (!overallResultValues->at(overallResultValuesIndex)->WriteToJsonObject(newOverallResultValuesJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			newOverallResultValuesArray << newOverallResultValuesJsonObject;
		}
		jsonObject["overallResultValues"] = newOverallResultValuesArray;
	}

	jsonObject["__typename"] = "CDMResult";

	return true;
}


bool CCDMResult::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("dataModelTitle") || ! jsonObject["dataModelTitle"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = jsonObject["dataModelTitle"].toString();

	if (!jsonObject.contains("dataModelVersion") || ! jsonObject["dataModelVersion"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = jsonObject["dataModelVersion"].toString();

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("resultContent")){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = imtsdl::TElementList<CDMResultUnionType>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			const QVariant tempResultContent = resultContentJsonArray[resultContentIndex].toVariant();
			QString resultContentDataValueTypename = resultContentJsonArray[resultContentIndex].toObject().value("__typename").toString();
			CDMResultUnionType resultContentDataValue;
			if (resultContentDataValueTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else if (resultContentDataValueTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
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
		overallResultValues = imtsdl::TElementList<COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool CCDMResult::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("dataModelTitle") && jsonObject["dataModelTitle"].isString()){
		dataModelTitle = jsonObject["dataModelTitle"].toString();
	}

	if (jsonObject.contains("dataModelVersion") && jsonObject["dataModelVersion"].isString()){
		dataModelVersion = jsonObject["dataModelVersion"].toString();
	}

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("resultContent")){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = imtsdl::TElementList<CDMResultUnionType>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			const QVariant tempResultContent = resultContentJsonArray[resultContentIndex].toVariant();
			QString resultContentDataValueTypename = resultContentJsonArray[resultContentIndex].toObject().value("__typename").toString();
			CDMResultUnionType resultContentDataValue;
			if (resultContentDataValueTypename == "CDMResultVarString") {
				CCDMResultVarString resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else if (resultContentDataValueTypename == "CDMResultVarRecursive") {
				CCDMResultVarRecursive resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
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
		overallResultValues = imtsdl::TElementList<COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


// serialize methods

QByteArray CPointsInCoordinateFormat::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPointsInCoordinateFormat::operator==(const CPointsInCoordinateFormat& other) const
{
	return 
				pointAsCoordinates == other.pointAsCoordinates;
}


bool CPointsInCoordinateFormat::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "PointsInCoordinateFormat", modelIndex);

	if (pointAsCoordinates){
		model.SetData("pointAsCoordinates", *pointAsCoordinates, modelIndex);
	}


	return true;
}


bool CPointsInCoordinateFormat::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant pointAsCoordinatesData = model.GetData("pointAsCoordinates", modelIndex);
	if (!pointAsCoordinatesData.isNull()){
		pointAsCoordinates = pointAsCoordinatesData.toBool();
	}

	return true;
}


bool CPointsInCoordinateFormat::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant pointAsCoordinatesData = model.GetData("pointAsCoordinates", modelIndex);
	if (!pointAsCoordinatesData.isNull()){
		pointAsCoordinates = pointAsCoordinatesData.toBool();
	}

	return true;
}


bool CPointsInCoordinateFormat::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (pointAsCoordinates){
		gqlObject.InsertParam("pointAsCoordinates", QVariant(*pointAsCoordinates));
	}

	gqlObject.InsertParam("__typename", QVariant("PointsInCoordinateFormat"));

	return true;
}


bool CPointsInCoordinateFormat::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("pointAsCoordinates") && (gqlObject["pointAsCoordinates"].userType() == QMetaType::Bool)){
		pointAsCoordinates = gqlObject["pointAsCoordinates"].toBool();
	}

	return true;
}


bool CPointsInCoordinateFormat::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("pointAsCoordinates") && (gqlObject["pointAsCoordinates"].userType() == QMetaType::Bool)){
		pointAsCoordinates = gqlObject["pointAsCoordinates"].toBool();
	}

	return true;
}


bool CPointsInCoordinateFormat::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (pointAsCoordinates){
		jsonObject["pointAsCoordinates"] = QJsonValue::fromVariant(*pointAsCoordinates);
	}

	jsonObject["__typename"] = "PointsInCoordinateFormat";

	return true;
}


bool CPointsInCoordinateFormat::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("pointAsCoordinates") && jsonObject["pointAsCoordinates"].isBool()){
		pointAsCoordinates = jsonObject["pointAsCoordinates"].toBool();
	}

	return true;
}


bool CPointsInCoordinateFormat::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("pointAsCoordinates") && jsonObject["pointAsCoordinates"].isBool()){
		pointAsCoordinates = jsonObject["pointAsCoordinates"].toBool();
	}

	return true;
}


// serialize methods

QByteArray CCollectionViewParamsTest::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CCollectionViewParamsTest::operator==(const CCollectionViewParamsTest& other) const
{
	return 
				count == other.count &&
				offset == other.offset &&
				filterModel == other.filterModel;
}


bool CCollectionViewParamsTest::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "CollectionViewParamsTest", modelIndex);

	if (count){
		model.SetData("count", *count, modelIndex);
	}

	if (offset){
		model.SetData("offset", *offset, modelIndex);
	}


	if (filterModel){
		::imtbase::CTreeItemModel* filterModelNewModelPtr = model.AddTreeModel("filterModel", modelIndex);
		const bool isFilterModelAdded = filterModel->WriteToModel(*filterModelNewModelPtr, 0);
		if (!isFilterModelAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}

	}

	return true;
}


bool CCollectionViewParamsTest::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant countData = model.GetData("count", modelIndex);
	if (!countData.isNull()){
		count = countData.toInt();
	}

	QVariant offsetData = model.GetData("offset", modelIndex);
	if (!offsetData.isNull()){
		offset = offsetData.toInt();
	}

	::imtbase::CTreeItemModel* filterModelDataModelPtr = model.GetTreeItemModel("filterModel", modelIndex);
	if (filterModelDataModelPtr != nullptr){
		filterModel = sdl::V1_0::imtbase::CComplexCollectionFilter();
		const bool isFilterModelRead = filterModel->ReadFromModel(*filterModelDataModelPtr, modelIndex);
		if (!isFilterModelRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CCollectionViewParamsTest::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant countData = model.GetData("count", modelIndex);
	if (!countData.isNull()){
		count = countData.toInt();
	}

	QVariant offsetData = model.GetData("offset", modelIndex);
	if (!offsetData.isNull()){
		offset = offsetData.toInt();
	}

	::imtbase::CTreeItemModel* filterModelDataModelPtr = model.GetTreeItemModel("filterModel", modelIndex);
	if (filterModelDataModelPtr != nullptr){
		filterModel = sdl::V1_0::imtbase::CComplexCollectionFilter();
		const bool isFilterModelRead = filterModel->ReadFromModel(*filterModelDataModelPtr, modelIndex);
		if (!isFilterModelRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CCollectionViewParamsTest::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (count){
		gqlObject.InsertParam("count", QVariant(*count));
	}

	if (offset){
		gqlObject.InsertParam("offset", QVariant(*offset));
	}

	if (filterModel){
		::imtgql::CGqlParamObject filterModelGqlObject;
		const bool isFilterModelAdded = filterModel->WriteToGraphQlObject(filterModelGqlObject);
		if (!isFilterModelAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("filterModel", filterModelGqlObject);
	}

	gqlObject.InsertParam("__typename", QVariant("CollectionViewParamsTest"));

	return true;
}


bool CCollectionViewParamsTest::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("count") && (gqlObject["count"].userType() == QMetaType::Int || gqlObject["count"].userType() == QMetaType::UInt || gqlObject["count"].userType() == QMetaType::LongLong || gqlObject["count"].userType() == QMetaType::ULongLong || gqlObject["count"].userType() == QMetaType::Long || gqlObject["count"].userType() == QMetaType::Short || gqlObject["count"].userType() == QMetaType::ULong || gqlObject["count"].userType() == QMetaType::UShort || gqlObject["count"].userType() == QMetaType::UChar)){
		count = gqlObject["count"].toInt();
	}

	if (gqlObject.ContainsParam("offset") && (gqlObject["offset"].userType() == QMetaType::Int || gqlObject["offset"].userType() == QMetaType::UInt || gqlObject["offset"].userType() == QMetaType::LongLong || gqlObject["offset"].userType() == QMetaType::ULongLong || gqlObject["offset"].userType() == QMetaType::Long || gqlObject["offset"].userType() == QMetaType::Short || gqlObject["offset"].userType() == QMetaType::ULong || gqlObject["offset"].userType() == QMetaType::UShort || gqlObject["offset"].userType() == QMetaType::UChar)){
		offset = gqlObject["offset"].toInt();
	}

	if (gqlObject.ContainsParam("filterModel") && (gqlObject.GetParamArgumentObjectPtr("filterModel") != nullptr)){
		filterModel = sdl::V1_0::imtbase::CComplexCollectionFilter();
		const bool isFilterModelRead = filterModel->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("filterModel"));
		if (!isFilterModelRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CCollectionViewParamsTest::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("count") && (gqlObject["count"].userType() == QMetaType::Int || gqlObject["count"].userType() == QMetaType::UInt || gqlObject["count"].userType() == QMetaType::LongLong || gqlObject["count"].userType() == QMetaType::ULongLong || gqlObject["count"].userType() == QMetaType::Long || gqlObject["count"].userType() == QMetaType::Short || gqlObject["count"].userType() == QMetaType::ULong || gqlObject["count"].userType() == QMetaType::UShort || gqlObject["count"].userType() == QMetaType::UChar)){
		count = gqlObject["count"].toInt();
	}

	if (gqlObject.ContainsParam("offset") && (gqlObject["offset"].userType() == QMetaType::Int || gqlObject["offset"].userType() == QMetaType::UInt || gqlObject["offset"].userType() == QMetaType::LongLong || gqlObject["offset"].userType() == QMetaType::ULongLong || gqlObject["offset"].userType() == QMetaType::Long || gqlObject["offset"].userType() == QMetaType::Short || gqlObject["offset"].userType() == QMetaType::ULong || gqlObject["offset"].userType() == QMetaType::UShort || gqlObject["offset"].userType() == QMetaType::UChar)){
		offset = gqlObject["offset"].toInt();
	}

	if (gqlObject.ContainsParam("filterModel") && (gqlObject.GetParamArgumentObjectPtr("filterModel") != nullptr)){
		filterModel = sdl::V1_0::imtbase::CComplexCollectionFilter();
		const bool isFilterModelRead = filterModel->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("filterModel"));
		if (!isFilterModelRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CCollectionViewParamsTest::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (count){
		jsonObject["count"] = QJsonValue::fromVariant(*count);
	}

	if (offset){
		jsonObject["offset"] = QJsonValue::fromVariant(*offset);
	}

	if (filterModel){
		QJsonObject filterModelJsonObject;
		const bool isFilterModelAdded = filterModel->WriteToJsonObject(filterModelJsonObject);
		if (!isFilterModelAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["filterModel"] = filterModelJsonObject;
	}

	jsonObject["__typename"] = "CollectionViewParamsTest";

	return true;
}


bool CCollectionViewParamsTest::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("count") && jsonObject["count"].isDouble()){
		count = jsonObject["count"].toInt();
	}

	if (jsonObject.contains("offset") && jsonObject["offset"].isDouble()){
		offset = jsonObject["offset"].toInt();
	}

	if (jsonObject.contains("filterModel") && jsonObject["filterModel"].isObject()){
		filterModel = sdl::V1_0::imtbase::CComplexCollectionFilter();
		const bool isFilterModelRead = filterModel->ReadFromJsonObject(jsonObject["filterModel"].toObject());
		if (!isFilterModelRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CCollectionViewParamsTest::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("count") && jsonObject["count"].isDouble()){
		count = jsonObject["count"].toInt();
	}

	if (jsonObject.contains("offset") && jsonObject["offset"].isDouble()){
		offset = jsonObject["offset"].toInt();
	}

	if (jsonObject.contains("filterModel") && jsonObject["filterModel"].isObject()){
		filterModel = sdl::V1_0::imtbase::CComplexCollectionFilter();
		const bool isFilterModelRead = filterModel->OptReadFromJsonObject(jsonObject["filterModel"].toObject());
		if (!isFilterModelRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


// serialize methods

CResultObject::CResultObject(QObject* parent): ::imtbase::CItemModelBase(parent){

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


QVariant CResultObject::GetAreaResult()
{
	sdl::V1_0::complextest::StatusCode valueType = AreaResult.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CResultObject::SetAreaResult(const QVariant& v)
{

	AreaResult.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		AreaResult = (sdl::V1_0::complextest::StatusCode)key;
	}
	areaResultChanged();
}


bool CResultObject::hasAreaResult()
{
	 return AreaResult.HasValue();
}


QVariant CResultObject::GetExpectedMinValue()
{
	return ExpectedMinValue.value();
}


void CResultObject::SetExpectedMinValue(const QVariant& v)
{

	ExpectedMinValue = v.value<double>();
	expectedMinValueChanged();
}


bool CResultObject::hasExpectedMinValue()
{
	 return ExpectedMinValue.HasValue();
}


QVariant CResultObject::GetExpectedMaxValue()
{
	return ExpectedMaxValue.value();
}


void CResultObject::SetExpectedMaxValue(const QVariant& v)
{

	ExpectedMaxValue = v.value<double>();
	expectedMaxValueChanged();
}


bool CResultObject::hasExpectedMaxValue()
{
	 return ExpectedMaxValue.HasValue();
}


QVariant CResultObject::GetMeasuredValue()
{
	return MeasuredValue.value();
}


void CResultObject::SetMeasuredValue(const QVariant& v)
{

	MeasuredValue = v.value<double>();
	measuredValueChanged();
}


bool CResultObject::hasMeasuredValue()
{
	 return MeasuredValue.HasValue();
}


QVariant CResultObject::GetMinMeasuredValue()
{
	return MinMeasuredValue.value();
}


void CResultObject::SetMinMeasuredValue(const QVariant& v)
{

	MinMeasuredValue = v.value<double>();
	minMeasuredValueChanged();
}


bool CResultObject::hasMinMeasuredValue()
{
	 return MinMeasuredValue.HasValue();
}


QVariant CResultObject::GetMaxMeasuredValue()
{
	return MaxMeasuredValue.value();
}


void CResultObject::SetMaxMeasuredValue(const QVariant& v)
{

	MaxMeasuredValue = v.value<double>();
	maxMeasuredValueChanged();
}


bool CResultObject::hasMaxMeasuredValue()
{
	 return MaxMeasuredValue.HasValue();
}


QVariant CResultObject::GetMeanMeasuredValue()
{
	return MeanMeasuredValue.value();
}


void CResultObject::SetMeanMeasuredValue(const QVariant& v)
{

	MeanMeasuredValue = v.value<double>();
	meanMeasuredValueChanged();
}


bool CResultObject::hasMeanMeasuredValue()
{
	 return MeanMeasuredValue.HasValue();
}


QVariant CResultObject::GetMeasurementType()
{
	sdl::V1_0::complextest::MeasurementType valueType = MeasurementType.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::MeasurementType>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CResultObject::SetMeasurementType(const QVariant& v)
{

	MeasurementType.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::MeasurementType>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		MeasurementType = (sdl::V1_0::complextest::MeasurementType)key;
	}
	measurementTypeChanged();
}


bool CResultObject::hasMeasurementType()
{
	 return MeasurementType.HasValue();
}


QVariant CResultObject::GetMeasurementUnit()
{
	sdl::V1_0::complextest::MeasurementUnit valueType = MeasurementUnit.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::MeasurementUnit>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CResultObject::SetMeasurementUnit(const QVariant& v)
{

	MeasurementUnit.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::MeasurementUnit>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		MeasurementUnit = (sdl::V1_0::complextest::MeasurementUnit)key;
	}
	measurementUnitChanged();
}


bool CResultObject::hasMeasurementUnit()
{
	 return MeasurementUnit.HasValue();
}


QVariant CResultObject::GetLength()
{
	return Length.value();
}


void CResultObject::SetLength(const QVariant& v)
{

	Length = v.value<double>();
	lengthChanged();
}


bool CResultObject::hasLength()
{
	 return Length.HasValue();
}


QVariant CResultObject::GetErrorType()
{
	sdl::V1_0::complextest::ErrorCode valueType = ErrorType.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::ErrorCode>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CResultObject::SetErrorType(const QVariant& v)
{

	ErrorType.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::ErrorCode>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		ErrorType = (sdl::V1_0::complextest::ErrorCode)key;
	}
	errorTypeChanged();
}


bool CResultObject::hasErrorType()
{
	 return ErrorType.HasValue();
}


QVariant CResultObject::GetGeometry()
{
	if (!m_geometryQObjectPtr.isValid()){
		m_geometryQObjectPtr = CreateObject("Geometry");
		auto itemPtr = m_geometryQObjectPtr.value<sdl::V1_0::complextest::CGeometryObject*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = Geometry;
	}
	return m_geometryQObjectPtr;
}


void CResultObject::SetGeometry(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::complextest::CGeometryObject* itemPtr = v.value<sdl::V1_0::complextest::CGeometryObject*>();
		if (itemPtr != nullptr)  Geometry = static_cast<const decltype(Geometry)::value_type&>(*itemPtr);
	}
	else {
		Geometry = nullptr;
	}
	m_geometryQObjectPtr = v;

	geometryChanged();
}


bool CResultObject::hasGeometry()
{
	 return Geometry.HasValue();
}


void CResultObject::emplaceGeometry()
{
	ResetGeometry();
	Geometry.emplace();
}


void CResultObject::ResetGeometry()
{
	auto* itemPtr = m_geometryQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_geometryQObjectPtr = QVariant();
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


QVariant CResultObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "Geometry"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CGeometryObject(this));
	}
	return QVariant();
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





bool CResultObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CResultObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CResultObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CResultObjectList::append(sdl::V1_0::complextest::CResultObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CResultObjectList* sdl::V1_0::complextest::CResultObjectList::copyMe()
{
	sdl::V1_0::complextest::CResultObjectList* retVal = new sdl::V1_0::complextest::CResultObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CResultObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CResultObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CResultObjectList::addElement(sdl::V1_0::complextest::CResultObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CResultObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CResultObjectList::isEqualWithModel(sdl::V1_0::complextest::CResultObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CResultObjectList::insert(int index, sdl::V1_0::complextest::CResultObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CResultObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CResultObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CResultObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CResultObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_areaResult"){
		return QVariant::fromValue(at(index)->AreaResult.value());
	}
	if (nameId == "m_expectedMinValue"){
		return QVariant::fromValue(at(index)->ExpectedMinValue.value());
	}
	if (nameId == "m_expectedMaxValue"){
		return QVariant::fromValue(at(index)->ExpectedMaxValue.value());
	}
	if (nameId == "m_measuredValue"){
		return QVariant::fromValue(at(index)->MeasuredValue.value());
	}
	if (nameId == "m_minMeasuredValue"){
		return QVariant::fromValue(at(index)->MinMeasuredValue.value());
	}
	if (nameId == "m_maxMeasuredValue"){
		return QVariant::fromValue(at(index)->MaxMeasuredValue.value());
	}
	if (nameId == "m_meanMeasuredValue"){
		return QVariant::fromValue(at(index)->MeanMeasuredValue.value());
	}
	if (nameId == "m_measurementType"){
		return QVariant::fromValue(at(index)->MeasurementType.value());
	}
	if (nameId == "m_measurementUnit"){
		return QVariant::fromValue(at(index)->MeasurementUnit.value());
	}
	if (nameId == "m_length"){
		return QVariant::fromValue(at(index)->Length.value());
	}
	if (nameId == "m_errorType"){
		return QVariant::fromValue(at(index)->ErrorType.value());
	}
	if (nameId == "m_geometry"){
		return itemPtr->GetGeometry();
	}

	return QVariant();
}
CAreaObject::CAreaObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CAreaObject::areaNameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAreaObject::statusChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAreaObject::iconPositionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAreaObject::errorTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAreaObject::resultsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CAreaObject::GetAreaName()
{
	return AreaName.value();
}


void CAreaObject::SetAreaName(const QVariant& v)
{

	AreaName = v.value<QString>();
	areaNameChanged();
}


bool CAreaObject::hasAreaName()
{
	 return AreaName.HasValue();
}


QVariant CAreaObject::GetStatus()
{
	sdl::V1_0::complextest::StatusCode valueType = Status.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CAreaObject::SetStatus(const QVariant& v)
{

	Status.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		Status = (sdl::V1_0::complextest::StatusCode)key;
	}
	statusChanged();
}


bool CAreaObject::hasStatus()
{
	 return Status.HasValue();
}


QVariant CAreaObject::GetIconPosition()
{
	if (!m_iconPositionQObjectPtr.isValid()){
		m_iconPositionQObjectPtr = CreateObject("IconPosition");
		auto itemPtr = m_iconPositionQObjectPtr.value<sdl::V1_0::complextest::CPointObject*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = IconPosition;
	}
	return m_iconPositionQObjectPtr;
}


void CAreaObject::SetIconPosition(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::complextest::CPointObject* itemPtr = v.value<sdl::V1_0::complextest::CPointObject*>();
		if (itemPtr != nullptr)  IconPosition = static_cast<const decltype(IconPosition)::value_type&>(*itemPtr);
	}
	else {
		IconPosition = nullptr;
	}
	m_iconPositionQObjectPtr = v;

	iconPositionChanged();
}


bool CAreaObject::hasIconPosition()
{
	 return IconPosition.HasValue();
}


void CAreaObject::emplaceIconPosition()
{
	ResetIconPosition();
	IconPosition.emplace();
}


void CAreaObject::ResetIconPosition()
{
	auto* itemPtr = m_iconPositionQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_iconPositionQObjectPtr = QVariant();
}


QVariant CAreaObject::GetErrorType()
{
	sdl::V1_0::complextest::ErrorCode valueType = ErrorType.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::ErrorCode>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CAreaObject::SetErrorType(const QVariant& v)
{

	ErrorType.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::ErrorCode>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		ErrorType = (sdl::V1_0::complextest::ErrorCode)key;
	}
	errorTypeChanged();
}


bool CAreaObject::hasErrorType()
{
	 return ErrorType.HasValue();
}


QVariant CAreaObject::GetResults()
{
	if (!m_resultsQObjectPtr.isValid()){
		m_resultsQObjectPtr = CreateObject("Results");
		auto itemPtr = m_resultsQObjectPtr.value<sdl::V1_0::complextest::CResultObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = Results;
	}
	return m_resultsQObjectPtr;
}


void CAreaObject::SetResults(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::complextest::CResultObjectList* itemPtr = v.value<sdl::V1_0::complextest::CResultObjectList*>();
		if (itemPtr != nullptr)  Results = static_cast<const decltype(Results)::value_type&>(*itemPtr);
	}
	else {
		Results = nullptr;
	}
	m_resultsQObjectPtr = v;

	resultsChanged();
}


bool CAreaObject::hasResults()
{
	 return Results.HasValue();
}


void CAreaObject::emplaceResults()
{
	ResetResults();
	Results.emplace();
}


void CAreaObject::ResetResults()
{
	auto* itemPtr = m_resultsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_resultsQObjectPtr = QVariant();
}


QVariant CAreaObject::createResultsArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::CResultObject());
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


QVariant CAreaObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "IconPosition"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CPointObject(this));
	}
	if (key == "Results"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CResultObjectList(this));
	}
	return QVariant();
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





bool CAreaObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CAreaObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CAreaObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CAreaObjectList::append(sdl::V1_0::complextest::CAreaObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CAreaObjectList* sdl::V1_0::complextest::CAreaObjectList::copyMe()
{
	sdl::V1_0::complextest::CAreaObjectList* retVal = new sdl::V1_0::complextest::CAreaObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CAreaObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CAreaObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CAreaObjectList::addElement(sdl::V1_0::complextest::CAreaObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CAreaObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CAreaObjectList::isEqualWithModel(sdl::V1_0::complextest::CAreaObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CAreaObjectList::insert(int index, sdl::V1_0::complextest::CAreaObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CAreaObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CAreaObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CAreaObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CAreaObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_areaName"){
		return QVariant::fromValue(at(index)->AreaName.value());
	}
	if (nameId == "m_status"){
		return QVariant::fromValue(at(index)->Status.value());
	}
	if (nameId == "m_iconPosition"){
		return itemPtr->GetIconPosition();
	}
	if (nameId == "m_errorType"){
		return QVariant::fromValue(at(index)->ErrorType.value());
	}
	if (nameId == "m_results"){
		return itemPtr->GetResults();
	}

	return QVariant();
}
CAnalyzerObject::CAnalyzerObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CAnalyzerObject::analyzerNameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAnalyzerObject::analyzerResultChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CAnalyzerObject::areasChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CAnalyzerObject::GetAnalyzerName()
{
	return AnalyzerName.value();
}


void CAnalyzerObject::SetAnalyzerName(const QVariant& v)
{

	AnalyzerName = v.value<QString>();
	analyzerNameChanged();
}


bool CAnalyzerObject::hasAnalyzerName()
{
	 return AnalyzerName.HasValue();
}


QVariant CAnalyzerObject::GetAnalyzerResult()
{
	sdl::V1_0::complextest::StatusCode valueType = AnalyzerResult.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CAnalyzerObject::SetAnalyzerResult(const QVariant& v)
{

	AnalyzerResult.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		AnalyzerResult = (sdl::V1_0::complextest::StatusCode)key;
	}
	analyzerResultChanged();
}


bool CAnalyzerObject::hasAnalyzerResult()
{
	 return AnalyzerResult.HasValue();
}


QVariant CAnalyzerObject::GetAreas()
{
	if (!m_areasQObjectPtr.isValid()){
		m_areasQObjectPtr = CreateObject("Areas");
		auto itemPtr = m_areasQObjectPtr.value<sdl::V1_0::complextest::CAreaObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = Areas;
	}
	return m_areasQObjectPtr;
}


void CAnalyzerObject::SetAreas(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::complextest::CAreaObjectList* itemPtr = v.value<sdl::V1_0::complextest::CAreaObjectList*>();
		if (itemPtr != nullptr)  Areas = static_cast<const decltype(Areas)::value_type&>(*itemPtr);
	}
	else {
		Areas = nullptr;
	}
	m_areasQObjectPtr = v;

	areasChanged();
}


bool CAnalyzerObject::hasAreas()
{
	 return Areas.HasValue();
}


void CAnalyzerObject::emplaceAreas()
{
	ResetAreas();
	Areas.emplace();
}


void CAnalyzerObject::ResetAreas()
{
	auto* itemPtr = m_areasQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_areasQObjectPtr = QVariant();
}


QVariant CAnalyzerObject::createAreasArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::CAreaObject());
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


QVariant CAnalyzerObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "Areas"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CAreaObjectList(this));
	}
	return QVariant();
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





bool CAnalyzerObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CAnalyzerObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CAnalyzerObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CAnalyzerObjectList::append(sdl::V1_0::complextest::CAnalyzerObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CAnalyzerObjectList* sdl::V1_0::complextest::CAnalyzerObjectList::copyMe()
{
	sdl::V1_0::complextest::CAnalyzerObjectList* retVal = new sdl::V1_0::complextest::CAnalyzerObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CAnalyzerObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CAnalyzerObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CAnalyzerObjectList::addElement(sdl::V1_0::complextest::CAnalyzerObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CAnalyzerObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CAnalyzerObjectList::isEqualWithModel(sdl::V1_0::complextest::CAnalyzerObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CAnalyzerObjectList::insert(int index, sdl::V1_0::complextest::CAnalyzerObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CAnalyzerObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CAnalyzerObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CAnalyzerObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CAnalyzerObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_analyzerName"){
		return QVariant::fromValue(at(index)->AnalyzerName.value());
	}
	if (nameId == "m_analyzerResult"){
		return QVariant::fromValue(at(index)->AnalyzerResult.value());
	}
	if (nameId == "m_areas"){
		return itemPtr->GetAreas();
	}

	return QVariant();
}
CInspectionObject::CInspectionObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CInspectionObject::iDChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CInspectionObject::typeIDChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CInspectionObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CInspectionObject::statusChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CInspectionObject::analyzersChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CInspectionObject::GetID()
{
	return ID.value();
}


void CInspectionObject::SetID(const QVariant& v)
{

	ID = v.value<QString>();
	iDChanged();
}


bool CInspectionObject::hasID()
{
	 return ID.HasValue();
}


QVariant CInspectionObject::GetTypeID()
{
	return TypeID.value();
}


void CInspectionObject::SetTypeID(const QVariant& v)
{

	TypeID = v.value<QString>();
	typeIDChanged();
}


bool CInspectionObject::hasTypeID()
{
	 return TypeID.HasValue();
}


QVariant CInspectionObject::GetName()
{
	return Name.value();
}


void CInspectionObject::SetName(const QVariant& v)
{

	Name = v.value<QString>();
	nameChanged();
}


bool CInspectionObject::hasName()
{
	 return Name.HasValue();
}


QVariant CInspectionObject::GetStatus()
{
	sdl::V1_0::complextest::StatusCode valueType = Status.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CInspectionObject::SetStatus(const QVariant& v)
{

	Status.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		Status = (sdl::V1_0::complextest::StatusCode)key;
	}
	statusChanged();
}


bool CInspectionObject::hasStatus()
{
	 return Status.HasValue();
}


QVariant CInspectionObject::GetAnalyzers()
{
	if (!m_analyzersQObjectPtr.isValid()){
		m_analyzersQObjectPtr = CreateObject("Analyzers");
		auto itemPtr = m_analyzersQObjectPtr.value<sdl::V1_0::complextest::CAnalyzerObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = Analyzers;
	}
	return m_analyzersQObjectPtr;
}


void CInspectionObject::SetAnalyzers(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::complextest::CAnalyzerObjectList* itemPtr = v.value<sdl::V1_0::complextest::CAnalyzerObjectList*>();
		if (itemPtr != nullptr)  Analyzers = static_cast<const decltype(Analyzers)::value_type&>(*itemPtr);
	}
	else {
		Analyzers = nullptr;
	}
	m_analyzersQObjectPtr = v;

	analyzersChanged();
}


bool CInspectionObject::hasAnalyzers()
{
	 return Analyzers.HasValue();
}


void CInspectionObject::emplaceAnalyzers()
{
	ResetAnalyzers();
	Analyzers.emplace();
}


void CInspectionObject::ResetAnalyzers()
{
	auto* itemPtr = m_analyzersQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_analyzersQObjectPtr = QVariant();
}


QVariant CInspectionObject::createAnalyzersArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::CAnalyzerObject());
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


QVariant CInspectionObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "Analyzers"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CAnalyzerObjectList(this));
	}
	return QVariant();
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





bool CInspectionObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CInspectionObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CInspectionObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CInspectionObjectList::append(sdl::V1_0::complextest::CInspectionObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CInspectionObjectList* sdl::V1_0::complextest::CInspectionObjectList::copyMe()
{
	sdl::V1_0::complextest::CInspectionObjectList* retVal = new sdl::V1_0::complextest::CInspectionObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CInspectionObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CInspectionObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CInspectionObjectList::addElement(sdl::V1_0::complextest::CInspectionObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CInspectionObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CInspectionObjectList::isEqualWithModel(sdl::V1_0::complextest::CInspectionObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CInspectionObjectList::insert(int index, sdl::V1_0::complextest::CInspectionObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CInspectionObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CInspectionObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CInspectionObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CInspectionObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_iD"){
		return QVariant::fromValue(at(index)->ID.value());
	}
	if (nameId == "m_typeID"){
		return QVariant::fromValue(at(index)->TypeID.value());
	}
	if (nameId == "m_name"){
		return QVariant::fromValue(at(index)->Name.value());
	}
	if (nameId == "m_status"){
		return QVariant::fromValue(at(index)->Status.value());
	}
	if (nameId == "m_analyzers"){
		return itemPtr->GetAnalyzers();
	}

	return QVariant();
}
CProductOverviewObject::CProductOverviewObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CProductOverviewObject::productIDChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::statusChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::partSerialNumberChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::timestampChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::productImageChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CProductOverviewObject::inspectionsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CProductOverviewObject::GetProductID()
{
	return ProductID.value();
}


void CProductOverviewObject::SetProductID(const QVariant& v)
{

	ProductID = v.value<int>();
	productIDChanged();
}


bool CProductOverviewObject::hasProductID()
{
	 return ProductID.HasValue();
}


QVariant CProductOverviewObject::GetName()
{
	return Name.value();
}


void CProductOverviewObject::SetName(const QVariant& v)
{

	Name = v.value<QString>();
	nameChanged();
}


bool CProductOverviewObject::hasName()
{
	 return Name.HasValue();
}


QVariant CProductOverviewObject::GetStatus()
{
	sdl::V1_0::complextest::StatusCode valueType = Status.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CProductOverviewObject::SetStatus(const QVariant& v)
{

	Status.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::StatusCode>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		Status = (sdl::V1_0::complextest::StatusCode)key;
	}
	statusChanged();
}


bool CProductOverviewObject::hasStatus()
{
	 return Status.HasValue();
}


QVariant CProductOverviewObject::GetPartSerialNumber()
{
	return PartSerialNumber.value();
}


void CProductOverviewObject::SetPartSerialNumber(const QVariant& v)
{

	PartSerialNumber = v.value<QString>();
	partSerialNumberChanged();
}


bool CProductOverviewObject::hasPartSerialNumber()
{
	 return PartSerialNumber.HasValue();
}


QVariant CProductOverviewObject::GetTimestamp()
{
	return Timestamp.value();
}


void CProductOverviewObject::SetTimestamp(const QVariant& v)
{

	Timestamp = v.value<QString>();
	timestampChanged();
}


bool CProductOverviewObject::hasTimestamp()
{
	 return Timestamp.HasValue();
}


QVariant CProductOverviewObject::GetProductImage()
{
	return ProductImage.value();
}


void CProductOverviewObject::SetProductImage(const QVariant& v)
{

	ProductImage = v.value<QString>();
	productImageChanged();
}


bool CProductOverviewObject::hasProductImage()
{
	 return ProductImage.HasValue();
}


QVariant CProductOverviewObject::GetInspections()
{
	if (!m_inspectionsQObjectPtr.isValid()){
		m_inspectionsQObjectPtr = CreateObject("Inspections");
		auto itemPtr = m_inspectionsQObjectPtr.value<sdl::V1_0::complextest::CInspectionObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = Inspections;
	}
	return m_inspectionsQObjectPtr;
}


void CProductOverviewObject::SetInspections(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::complextest::CInspectionObjectList* itemPtr = v.value<sdl::V1_0::complextest::CInspectionObjectList*>();
		if (itemPtr != nullptr)  Inspections = static_cast<const decltype(Inspections)::value_type&>(*itemPtr);
	}
	else {
		Inspections = nullptr;
	}
	m_inspectionsQObjectPtr = v;

	inspectionsChanged();
}


bool CProductOverviewObject::hasInspections()
{
	 return Inspections.HasValue();
}


void CProductOverviewObject::emplaceInspections()
{
	ResetInspections();
	Inspections.emplace();
}


void CProductOverviewObject::ResetInspections()
{
	auto* itemPtr = m_inspectionsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_inspectionsQObjectPtr = QVariant();
}


QVariant CProductOverviewObject::createInspectionsArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::CInspectionObject());
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


QVariant CProductOverviewObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "Inspections"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CInspectionObjectList(this));
	}
	return QVariant();
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





bool CProductOverviewObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CProductOverviewObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CProductOverviewObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CProductOverviewObjectList::append(sdl::V1_0::complextest::CProductOverviewObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CProductOverviewObjectList* sdl::V1_0::complextest::CProductOverviewObjectList::copyMe()
{
	sdl::V1_0::complextest::CProductOverviewObjectList* retVal = new sdl::V1_0::complextest::CProductOverviewObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CProductOverviewObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CProductOverviewObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CProductOverviewObjectList::addElement(sdl::V1_0::complextest::CProductOverviewObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CProductOverviewObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CProductOverviewObjectList::isEqualWithModel(sdl::V1_0::complextest::CProductOverviewObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CProductOverviewObjectList::insert(int index, sdl::V1_0::complextest::CProductOverviewObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CProductOverviewObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CProductOverviewObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CProductOverviewObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CProductOverviewObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_productID"){
		return QVariant::fromValue(at(index)->ProductID.value());
	}
	if (nameId == "m_name"){
		return QVariant::fromValue(at(index)->Name.value());
	}
	if (nameId == "m_status"){
		return QVariant::fromValue(at(index)->Status.value());
	}
	if (nameId == "m_partSerialNumber"){
		return QVariant::fromValue(at(index)->PartSerialNumber.value());
	}
	if (nameId == "m_timestamp"){
		return QVariant::fromValue(at(index)->Timestamp.value());
	}
	if (nameId == "m_productImage"){
		return QVariant::fromValue(at(index)->ProductImage.value());
	}
	if (nameId == "m_inspections"){
		return itemPtr->GetInspections();
	}

	return QVariant();
}
CLocalizedTextObject::CLocalizedTextObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CLocalizedTextObject::textChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CLocalizedTextObject::localeChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CLocalizedTextObject::GetText()
{
	return text.value();
}


void CLocalizedTextObject::SetText(const QVariant& v)
{

	text = v.value<QString>();
	textChanged();
}


bool CLocalizedTextObject::hasText()
{
	 return text.HasValue();
}


QVariant CLocalizedTextObject::GetLocale()
{
	return locale.value();
}


void CLocalizedTextObject::SetLocale(const QVariant& v)
{

	locale = v.value<QString>();
	localeChanged();
}


bool CLocalizedTextObject::hasLocale()
{
	 return locale.HasValue();
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


QVariant CLocalizedTextObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
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





bool CLocalizedTextObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CLocalizedTextObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CLocalizedTextObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CLocalizedTextObjectList::append(sdl::V1_0::complextest::CLocalizedTextObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CLocalizedTextObjectList* sdl::V1_0::complextest::CLocalizedTextObjectList::copyMe()
{
	sdl::V1_0::complextest::CLocalizedTextObjectList* retVal = new sdl::V1_0::complextest::CLocalizedTextObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CLocalizedTextObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CLocalizedTextObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CLocalizedTextObjectList::addElement(sdl::V1_0::complextest::CLocalizedTextObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CLocalizedTextObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CLocalizedTextObjectList::isEqualWithModel(sdl::V1_0::complextest::CLocalizedTextObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CLocalizedTextObjectList::insert(int index, sdl::V1_0::complextest::CLocalizedTextObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CLocalizedTextObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CLocalizedTextObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CLocalizedTextObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CLocalizedTextObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_text"){
		return QVariant::fromValue(at(index)->text.value());
	}
	if (nameId == "m_locale"){
		return QVariant::fromValue(at(index)->locale.value());
	}

	return QVariant();
}
CCoordinatesObject::CCoordinatesObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CCoordinatesObject::xChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCoordinatesObject::yChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCoordinatesObject::zChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCoordinatesObject::GetX()
{
	return x.value();
}


void CCoordinatesObject::SetX(const QVariant& v)
{

	x = v.value<double>();
	xChanged();
}


bool CCoordinatesObject::hasX()
{
	 return x.HasValue();
}


QVariant CCoordinatesObject::GetY()
{
	return y.value();
}


void CCoordinatesObject::SetY(const QVariant& v)
{

	y = v.value<double>();
	yChanged();
}


bool CCoordinatesObject::hasY()
{
	 return y.HasValue();
}


QVariant CCoordinatesObject::GetZ()
{
	return z.value();
}


void CCoordinatesObject::SetZ(const QVariant& v)
{

	z = v.value<double>();
	zChanged();
}


bool CCoordinatesObject::hasZ()
{
	 return z.HasValue();
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


QVariant CCoordinatesObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
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





bool CCoordinatesObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CCoordinatesObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CCoordinatesObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCoordinatesObjectList::append(sdl::V1_0::complextest::CCoordinatesObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CCoordinatesObjectList* sdl::V1_0::complextest::CCoordinatesObjectList::copyMe()
{
	sdl::V1_0::complextest::CCoordinatesObjectList* retVal = new sdl::V1_0::complextest::CCoordinatesObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CCoordinatesObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CCoordinatesObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CCoordinatesObjectList::addElement(sdl::V1_0::complextest::CCoordinatesObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CCoordinatesObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CCoordinatesObjectList::isEqualWithModel(sdl::V1_0::complextest::CCoordinatesObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CCoordinatesObjectList::insert(int index, sdl::V1_0::complextest::CCoordinatesObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CCoordinatesObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CCoordinatesObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CCoordinatesObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CCoordinatesObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_x"){
		return QVariant::fromValue(at(index)->x.value());
	}
	if (nameId == "m_y"){
		return QVariant::fromValue(at(index)->y.value());
	}
	if (nameId == "m_z"){
		return QVariant::fromValue(at(index)->z.value());
	}

	return QVariant();
}
CExtendedMetaDataObject::CExtendedMetaDataObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CExtendedMetaDataObject::keyChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CExtendedMetaDataObject::valueChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CExtendedMetaDataObject::GetKey()
{
	return key.value();
}


void CExtendedMetaDataObject::SetKey(const QVariant& v)
{

	key = v.value<QString>();
	keyChanged();
}


bool CExtendedMetaDataObject::hasKey()
{
	 return key.HasValue();
}


QVariant CExtendedMetaDataObject::GetValue()
{
	if (m_valueQObjectPtr.isValid()){
		if (const CCoordinates* val = std::get_if<CCoordinates>((value).GetPtr())){
			CCoordinatesObject *newObjectPtr = new CCoordinatesObject(this);
			static_cast<decltype(*newObjectPtr)&>(*newObjectPtr) = *val;
			m_valueQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
		if (const double* val = std::get_if<double>((value).GetPtr())){
			m_valueQObjectPtr = QVariant::fromValue(val);
		}
		if (const bool* val = std::get_if<bool>((value).GetPtr())){
			m_valueQObjectPtr = QVariant::fromValue(val);
		}
		if (const QString* val = std::get_if<QString>((value).GetPtr())){
			m_valueQObjectPtr = QVariant::fromValue(val);
		}
	}
	return m_valueQObjectPtr;
}


void CExtendedMetaDataObject::SetValue(const QVariant& v)
{

	if (v.isValid()){
		if (const CCoordinatesObject* val = v.value<const CCoordinatesObject*>()){
			*value = *val;
		}
		if (const double* val = v.value<const double*>()){
			*value = *val;
		}
		if (const bool* val = v.value<const bool*>()){
			*value = *val;
		}
		if (const QString* val = v.value<const QString*>()){
			*value = *val;
		}
	}
	else {
		value = nullptr;
	}
	m_valueQObjectPtr = v;

	valueChanged();
}


bool CExtendedMetaDataObject::hasValue()
{
	 return value.HasValue();
}


void CExtendedMetaDataObject::emplaceValue()
{
	ResetValue();
	value.emplace();
}


void CExtendedMetaDataObject::ResetValue()
{
	auto* itemPtr = m_valueQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_valueQObjectPtr = QVariant();
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


QVariant CExtendedMetaDataObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
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





bool CExtendedMetaDataObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CExtendedMetaDataObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CExtendedMetaDataObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CExtendedMetaDataObjectList::append(sdl::V1_0::complextest::CExtendedMetaDataObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CExtendedMetaDataObjectList* sdl::V1_0::complextest::CExtendedMetaDataObjectList::copyMe()
{
	sdl::V1_0::complextest::CExtendedMetaDataObjectList* retVal = new sdl::V1_0::complextest::CExtendedMetaDataObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CExtendedMetaDataObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CExtendedMetaDataObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CExtendedMetaDataObjectList::addElement(sdl::V1_0::complextest::CExtendedMetaDataObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CExtendedMetaDataObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CExtendedMetaDataObjectList::isEqualWithModel(sdl::V1_0::complextest::CExtendedMetaDataObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CExtendedMetaDataObjectList::insert(int index, sdl::V1_0::complextest::CExtendedMetaDataObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CExtendedMetaDataObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CExtendedMetaDataObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CExtendedMetaDataObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CExtendedMetaDataObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_key"){
		return QVariant::fromValue(at(index)->key.value());
	}
	if (nameId == "m_value"){
		return itemPtr->GetValue();
	}

	return QVariant();
}
CResultMetaDataObject::CResultMetaDataObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CResultMetaDataObject::resultIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::creationTimeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::partIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::productIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::classificationChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::descriptionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CResultMetaDataObject::extendedMetaDataChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CResultMetaDataObject::GetResultId()
{
	return resultId.value();
}


void CResultMetaDataObject::SetResultId(const QVariant& v)
{

	resultId = v.value<QString>().toUtf8();
	resultIdChanged();
}


bool CResultMetaDataObject::hasResultId()
{
	 return resultId.HasValue();
}


QVariant CResultMetaDataObject::GetCreationTime()
{
	return creationTime.value();
}


void CResultMetaDataObject::SetCreationTime(const QVariant& v)
{

	creationTime = v.value<QString>();
	creationTimeChanged();
}


bool CResultMetaDataObject::hasCreationTime()
{
	 return creationTime.HasValue();
}


QVariant CResultMetaDataObject::GetPartId()
{
	return partId.value();
}


void CResultMetaDataObject::SetPartId(const QVariant& v)
{

	partId = v.value<QString>();
	partIdChanged();
}


bool CResultMetaDataObject::hasPartId()
{
	 return partId.HasValue();
}


QVariant CResultMetaDataObject::GetProductId()
{
	return productId.value();
}


void CResultMetaDataObject::SetProductId(const QVariant& v)
{

	productId = v.value<QString>();
	productIdChanged();
}


bool CResultMetaDataObject::hasProductId()
{
	 return productId.HasValue();
}


QVariant CResultMetaDataObject::GetName()
{
	return name.value();
}


void CResultMetaDataObject::SetName(const QVariant& v)
{

	name = v.value<QString>();
	nameChanged();
}


bool CResultMetaDataObject::hasName()
{
	 return name.HasValue();
}


QVariant CResultMetaDataObject::GetClassification()
{
	return classification.value();
}


void CResultMetaDataObject::SetClassification(const QVariant& v)
{

	classification = v.value<int>();
	classificationChanged();
}


bool CResultMetaDataObject::hasClassification()
{
	 return classification.HasValue();
}


QVariant CResultMetaDataObject::GetDescription()
{
	if (!m_descriptionQObjectPtr.isValid()){
		m_descriptionQObjectPtr = CreateObject("description");
		auto itemPtr = m_descriptionQObjectPtr.value<sdl::V1_0::complextest::CLocalizedTextObject*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = description;
	}
	return m_descriptionQObjectPtr;
}


void CResultMetaDataObject::SetDescription(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::complextest::CLocalizedTextObject* itemPtr = v.value<sdl::V1_0::complextest::CLocalizedTextObject*>();
		if (itemPtr != nullptr)  description = static_cast<const decltype(description)::value_type&>(*itemPtr);
	}
	else {
		description = nullptr;
	}
	m_descriptionQObjectPtr = v;

	descriptionChanged();
}


bool CResultMetaDataObject::hasDescription()
{
	 return description.HasValue();
}


void CResultMetaDataObject::emplaceDescription()
{
	ResetDescription();
	description.emplace();
}


void CResultMetaDataObject::ResetDescription()
{
	auto* itemPtr = m_descriptionQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_descriptionQObjectPtr = QVariant();
}


QVariant CResultMetaDataObject::GetExtendedMetaData()
{
	if (!m_extendedMetaDataQObjectPtr.isValid()){
		m_extendedMetaDataQObjectPtr = CreateObject("extendedMetaData");
		auto itemPtr = m_extendedMetaDataQObjectPtr.value<sdl::V1_0::complextest::CExtendedMetaDataObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = extendedMetaData;
	}
	return m_extendedMetaDataQObjectPtr;
}


void CResultMetaDataObject::SetExtendedMetaData(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::complextest::CExtendedMetaDataObjectList* itemPtr = v.value<sdl::V1_0::complextest::CExtendedMetaDataObjectList*>();
		if (itemPtr != nullptr)  extendedMetaData = static_cast<const decltype(extendedMetaData)::value_type&>(*itemPtr);
	}
	else {
		extendedMetaData = nullptr;
	}
	m_extendedMetaDataQObjectPtr = v;

	extendedMetaDataChanged();
}


bool CResultMetaDataObject::hasExtendedMetaData()
{
	 return extendedMetaData.HasValue();
}


void CResultMetaDataObject::emplaceExtendedMetaData()
{
	ResetExtendedMetaData();
	extendedMetaData.emplace();
}


void CResultMetaDataObject::ResetExtendedMetaData()
{
	auto* itemPtr = m_extendedMetaDataQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_extendedMetaDataQObjectPtr = QVariant();
}


QVariant CResultMetaDataObject::createExtendedMetaDataArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::CExtendedMetaDataObject());
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


QVariant CResultMetaDataObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "description"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CLocalizedTextObject(this));
	}
	if (key == "extendedMetaData"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CExtendedMetaDataObjectList(this));
	}
	return QVariant();
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





bool CResultMetaDataObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CResultMetaDataObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CResultMetaDataObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CResultMetaDataObjectList::append(sdl::V1_0::complextest::CResultMetaDataObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CResultMetaDataObjectList* sdl::V1_0::complextest::CResultMetaDataObjectList::copyMe()
{
	sdl::V1_0::complextest::CResultMetaDataObjectList* retVal = new sdl::V1_0::complextest::CResultMetaDataObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CResultMetaDataObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CResultMetaDataObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CResultMetaDataObjectList::addElement(sdl::V1_0::complextest::CResultMetaDataObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CResultMetaDataObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CResultMetaDataObjectList::isEqualWithModel(sdl::V1_0::complextest::CResultMetaDataObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CResultMetaDataObjectList::insert(int index, sdl::V1_0::complextest::CResultMetaDataObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CResultMetaDataObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CResultMetaDataObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CResultMetaDataObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CResultMetaDataObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_resultId"){
		return QVariant::fromValue(at(index)->resultId.value());
	}
	if (nameId == "m_creationTime"){
		return QVariant::fromValue(at(index)->creationTime.value());
	}
	if (nameId == "m_partId"){
		return QVariant::fromValue(at(index)->partId.value());
	}
	if (nameId == "m_productId"){
		return QVariant::fromValue(at(index)->productId.value());
	}
	if (nameId == "m_name"){
		return QVariant::fromValue(at(index)->name.value());
	}
	if (nameId == "m_classification"){
		return QVariant::fromValue(at(index)->classification.value());
	}
	if (nameId == "m_description"){
		return itemPtr->GetDescription();
	}
	if (nameId == "m_extendedMetaData"){
		return itemPtr->GetExtendedMetaData();
	}

	return QVariant();
}
COverallResultValuesObject::COverallResultValuesObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &COverallResultValuesObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COverallResultValuesObject::valueIdChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COverallResultValuesObject::measuredValueChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COverallResultValuesObject::lowLimitChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COverallResultValuesObject::highLimitChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant COverallResultValuesObject::GetName()
{
	return name.value();
}


void COverallResultValuesObject::SetName(const QVariant& v)
{

	name = v.value<QString>();
	nameChanged();
}


bool COverallResultValuesObject::hasName()
{
	 return name.HasValue();
}


QVariant COverallResultValuesObject::GetValueId()
{
	return valueId.value();
}


void COverallResultValuesObject::SetValueId(const QVariant& v)
{

	valueId = v.value<QString>();
	valueIdChanged();
}


bool COverallResultValuesObject::hasValueId()
{
	 return valueId.HasValue();
}


QVariant COverallResultValuesObject::GetMeasuredValue()
{
	return measuredValue.value();
}


void COverallResultValuesObject::SetMeasuredValue(const QVariant& v)
{

	measuredValue = v.value<double>();
	measuredValueChanged();
}


bool COverallResultValuesObject::hasMeasuredValue()
{
	 return measuredValue.HasValue();
}


QVariant COverallResultValuesObject::GetLowLimit()
{
	return lowLimit.value();
}


void COverallResultValuesObject::SetLowLimit(const QVariant& v)
{

	lowLimit = v.value<double>();
	lowLimitChanged();
}


bool COverallResultValuesObject::hasLowLimit()
{
	 return lowLimit.HasValue();
}


QVariant COverallResultValuesObject::GetHighLimit()
{
	return highLimit.value();
}


void COverallResultValuesObject::SetHighLimit(const QVariant& v)
{

	highLimit = v.value<double>();
	highLimitChanged();
}


bool COverallResultValuesObject::hasHighLimit()
{
	 return highLimit.HasValue();
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


QVariant COverallResultValuesObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
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





bool COverallResultValuesObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int COverallResultValuesObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap COverallResultValuesObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void COverallResultValuesObjectList::append(sdl::V1_0::complextest::COverallResultValuesObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::COverallResultValuesObjectList* sdl::V1_0::complextest::COverallResultValuesObjectList::copyMe()
{
	sdl::V1_0::complextest::COverallResultValuesObjectList* retVal = new sdl::V1_0::complextest::COverallResultValuesObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::COverallResultValuesObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::COverallResultValuesObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::COverallResultValuesObjectList::addElement(sdl::V1_0::complextest::COverallResultValuesObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::COverallResultValuesObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::COverallResultValuesObjectList::isEqualWithModel(sdl::V1_0::complextest::COverallResultValuesObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::COverallResultValuesObjectList::insert(int index, sdl::V1_0::complextest::COverallResultValuesObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::COverallResultValuesObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::COverallResultValuesObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::COverallResultValuesObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::COverallResultValuesObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_name"){
		return QVariant::fromValue(at(index)->name.value());
	}
	if (nameId == "m_valueId"){
		return QVariant::fromValue(at(index)->valueId.value());
	}
	if (nameId == "m_measuredValue"){
		return QVariant::fromValue(at(index)->measuredValue.value());
	}
	if (nameId == "m_lowLimit"){
		return QVariant::fromValue(at(index)->lowLimit.value());
	}
	if (nameId == "m_highLimit"){
		return QVariant::fromValue(at(index)->highLimit.value());
	}

	return QVariant();
}
CUnionTestingTypeObject::CUnionTestingTypeObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CUnionTestingTypeObject::simpleUnionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CUnionTestingTypeObject::complexUnionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CUnionTestingTypeObject::mixedUnionChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CUnionTestingTypeObject::GetSimpleUnion()
{
	if (m_simpleUnionQObjectPtr.isValid()){
		if (const double* val = std::get_if<double>((simpleUnion).GetPtr())){
			m_simpleUnionQObjectPtr = QVariant::fromValue(val);
		}
		if (const bool* val = std::get_if<bool>((simpleUnion).GetPtr())){
			m_simpleUnionQObjectPtr = QVariant::fromValue(val);
		}
		if (const QString* val = std::get_if<QString>((simpleUnion).GetPtr())){
			m_simpleUnionQObjectPtr = QVariant::fromValue(val);
		}
	}
	return m_simpleUnionQObjectPtr;
}


void CUnionTestingTypeObject::SetSimpleUnion(const QVariant& v)
{

	if (v.isValid()){
		if (const double* val = v.value<const double*>()){
			*simpleUnion = *val;
		}
		if (const bool* val = v.value<const bool*>()){
			*simpleUnion = *val;
		}
		if (const QString* val = v.value<const QString*>()){
			*simpleUnion = *val;
		}
	}
	else {
		simpleUnion = nullptr;
	}
	m_simpleUnionQObjectPtr = v;

	simpleUnionChanged();
}


bool CUnionTestingTypeObject::hasSimpleUnion()
{
	 return simpleUnion.HasValue();
}


void CUnionTestingTypeObject::emplaceSimpleUnion()
{
	ResetSimpleUnion();
	simpleUnion.emplace();
}


void CUnionTestingTypeObject::ResetSimpleUnion()
{
	auto* itemPtr = m_simpleUnionQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_simpleUnionQObjectPtr = QVariant();
}


QVariant CUnionTestingTypeObject::GetComplexUnion()
{
	if (m_complexUnionQObjectPtr.isValid()){
		if (const CCDMResultVarString* val = std::get_if<CCDMResultVarString>((complexUnion).GetPtr())){
			CCDMResultVarStringObject *newObjectPtr = new CCDMResultVarStringObject(this);
			static_cast<decltype(*newObjectPtr)&>(*newObjectPtr) = *val;
			m_complexUnionQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
		if (const CCDMResultVarRecursive* val = std::get_if<CCDMResultVarRecursive>((complexUnion).GetPtr())){
			CCDMResultVarRecursiveObject *newObjectPtr = new CCDMResultVarRecursiveObject(this);
			static_cast<decltype(*newObjectPtr)&>(*newObjectPtr) = *val;
			m_complexUnionQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
	}
	return m_complexUnionQObjectPtr;
}


void CUnionTestingTypeObject::SetComplexUnion(const QVariant& v)
{

	if (v.isValid()){
		if (const CCDMResultVarStringObject* val = v.value<const CCDMResultVarStringObject*>()){
			*complexUnion = *val;
		}
		if (const CCDMResultVarRecursiveObject* val = v.value<const CCDMResultVarRecursiveObject*>()){
			*complexUnion = *val;
		}
	}
	else {
		complexUnion = nullptr;
	}
	m_complexUnionQObjectPtr = v;

	complexUnionChanged();
}


bool CUnionTestingTypeObject::hasComplexUnion()
{
	 return complexUnion.HasValue();
}


void CUnionTestingTypeObject::emplaceComplexUnion()
{
	ResetComplexUnion();
	complexUnion.emplace();
}


void CUnionTestingTypeObject::ResetComplexUnion()
{
	auto* itemPtr = m_complexUnionQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_complexUnionQObjectPtr = QVariant();
}


QVariant CUnionTestingTypeObject::GetMixedUnion()
{
	if (m_mixedUnionQObjectPtr.isValid()){
		if (const CCoordinates* val = std::get_if<CCoordinates>((mixedUnion).GetPtr())){
			CCoordinatesObject *newObjectPtr = new CCoordinatesObject(this);
			static_cast<decltype(*newObjectPtr)&>(*newObjectPtr) = *val;
			m_mixedUnionQObjectPtr = QVariant::fromValue(newObjectPtr);
		}
		if (const double* val = std::get_if<double>((mixedUnion).GetPtr())){
			m_mixedUnionQObjectPtr = QVariant::fromValue(val);
		}
		if (const bool* val = std::get_if<bool>((mixedUnion).GetPtr())){
			m_mixedUnionQObjectPtr = QVariant::fromValue(val);
		}
		if (const QString* val = std::get_if<QString>((mixedUnion).GetPtr())){
			m_mixedUnionQObjectPtr = QVariant::fromValue(val);
		}
	}
	return m_mixedUnionQObjectPtr;
}


void CUnionTestingTypeObject::SetMixedUnion(const QVariant& v)
{

	if (v.isValid()){
		if (const CCoordinatesObject* val = v.value<const CCoordinatesObject*>()){
			*mixedUnion = *val;
		}
		if (const double* val = v.value<const double*>()){
			*mixedUnion = *val;
		}
		if (const bool* val = v.value<const bool*>()){
			*mixedUnion = *val;
		}
		if (const QString* val = v.value<const QString*>()){
			*mixedUnion = *val;
		}
	}
	else {
		mixedUnion = nullptr;
	}
	m_mixedUnionQObjectPtr = v;

	mixedUnionChanged();
}


bool CUnionTestingTypeObject::hasMixedUnion()
{
	 return mixedUnion.HasValue();
}


void CUnionTestingTypeObject::emplaceMixedUnion()
{
	ResetMixedUnion();
	mixedUnion.emplace();
}


void CUnionTestingTypeObject::ResetMixedUnion()
{
	auto* itemPtr = m_mixedUnionQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_mixedUnionQObjectPtr = QVariant();
}


QString CUnionTestingTypeObject::toJson() const
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


bool CUnionTestingTypeObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CUnionTestingTypeObject::fromObject(const QJsonObject& jsonObject)
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


QString CUnionTestingTypeObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CUnionTestingTypeObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CUnionTestingTypeObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "simpleUnion")){
		return "simpleUnion";
	}
	if (propertyName == (QString("m_") + "complexUnion")){
		return "complexUnion";
	}
	if (propertyName == (QString("m_") + "mixedUnion")){
		return "mixedUnion";
	}

	return propertyName;
}





bool CUnionTestingTypeObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CUnionTestingTypeObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CUnionTestingTypeObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CUnionTestingTypeObjectList::append(sdl::V1_0::complextest::CUnionTestingTypeObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CUnionTestingTypeObjectList* sdl::V1_0::complextest::CUnionTestingTypeObjectList::copyMe()
{
	sdl::V1_0::complextest::CUnionTestingTypeObjectList* retVal = new sdl::V1_0::complextest::CUnionTestingTypeObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CUnionTestingTypeObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CUnionTestingTypeObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CUnionTestingTypeObjectList::addElement(sdl::V1_0::complextest::CUnionTestingTypeObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CUnionTestingTypeObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CUnionTestingTypeObjectList::isEqualWithModel(sdl::V1_0::complextest::CUnionTestingTypeObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CUnionTestingTypeObjectList::insert(int index, sdl::V1_0::complextest::CUnionTestingTypeObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CUnionTestingTypeObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CUnionTestingTypeObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CUnionTestingTypeObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CUnionTestingTypeObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_simpleUnion"){
		return itemPtr->GetSimpleUnion();
	}
	if (nameId == "m_complexUnion"){
		return itemPtr->GetComplexUnion();
	}
	if (nameId == "m_mixedUnion"){
		return itemPtr->GetMixedUnion();
	}

	return QVariant();
}
CCDMResultVarRecursiveObject::CCDMResultVarRecursiveObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CCDMResultVarRecursiveObject::dataModelTitleChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarRecursiveObject::dataModelVersionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarRecursiveObject::resultMetaDataChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarRecursiveObject::resultContentChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarRecursiveObject::overallResultValuesChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCDMResultVarRecursiveObject::GetDataModelTitle()
{
	return dataModelTitle.value();
}


void CCDMResultVarRecursiveObject::SetDataModelTitle(const QVariant& v)
{

	dataModelTitle = v.value<QString>();
	dataModelTitleChanged();
}


bool CCDMResultVarRecursiveObject::hasDataModelTitle()
{
	 return dataModelTitle.HasValue();
}


QVariant CCDMResultVarRecursiveObject::GetDataModelVersion()
{
	return dataModelVersion.value();
}


void CCDMResultVarRecursiveObject::SetDataModelVersion(const QVariant& v)
{

	dataModelVersion = v.value<QString>();
	dataModelVersionChanged();
}


bool CCDMResultVarRecursiveObject::hasDataModelVersion()
{
	 return dataModelVersion.HasValue();
}


QVariant CCDMResultVarRecursiveObject::GetResultMetaData()
{
	if (!m_resultMetaDataQObjectPtr.isValid()){
		m_resultMetaDataQObjectPtr = CreateObject("resultMetaData");
		auto itemPtr = m_resultMetaDataQObjectPtr.value<sdl::V1_0::complextest::CResultMetaDataObject*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = resultMetaData;
	}
	return m_resultMetaDataQObjectPtr;
}


void CCDMResultVarRecursiveObject::SetResultMetaData(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::complextest::CResultMetaDataObject* itemPtr = v.value<sdl::V1_0::complextest::CResultMetaDataObject*>();
		if (itemPtr != nullptr)  resultMetaData = static_cast<const decltype(resultMetaData)::value_type&>(*itemPtr);
	}
	else {
		resultMetaData = nullptr;
	}
	m_resultMetaDataQObjectPtr = v;

	resultMetaDataChanged();
}


bool CCDMResultVarRecursiveObject::hasResultMetaData()
{
	 return resultMetaData.HasValue();
}


void CCDMResultVarRecursiveObject::emplaceResultMetaData()
{
	ResetResultMetaData();
	resultMetaData.emplace();
}


void CCDMResultVarRecursiveObject::ResetResultMetaData()
{
	auto* itemPtr = m_resultMetaDataQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_resultMetaDataQObjectPtr = QVariant();
}


QVariant CCDMResultVarRecursiveObject::GetResultContent()
{
	if (!m_resultContentQObjectPtr.isValid()){
		m_resultContentQObjectPtr = CreateObject("resultContent");
		auto itemPtr = m_resultContentQObjectPtr.value<sdl::V1_0::complextest::CCDMResultUnionTypeObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = resultContent;
	}
	return m_resultContentQObjectPtr;
}


void CCDMResultVarRecursiveObject::SetResultContent(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::complextest::CCDMResultUnionTypeObjectList* itemPtr = v.value<sdl::V1_0::complextest::CCDMResultUnionTypeObjectList*>();
		if (itemPtr != nullptr)  resultContent = static_cast<const decltype(resultContent)::value_type&>(*itemPtr);
	}
	else {
		resultContent = nullptr;
	}
	m_resultContentQObjectPtr = v;

	resultContentChanged();
}


bool CCDMResultVarRecursiveObject::hasResultContent()
{
	 return resultContent.HasValue();
}


void CCDMResultVarRecursiveObject::emplaceResultContent()
{
	ResetResultContent();
	resultContent.emplace();
}


void CCDMResultVarRecursiveObject::ResetResultContent()
{
	auto* itemPtr = m_resultContentQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_resultContentQObjectPtr = QVariant();
}


QVariant CCDMResultVarRecursiveObject::GetOverallResultValues()
{
	if (!m_overallResultValuesQObjectPtr.isValid()){
		m_overallResultValuesQObjectPtr = CreateObject("overallResultValues");
		auto itemPtr = m_overallResultValuesQObjectPtr.value<sdl::V1_0::complextest::COverallResultValuesObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = overallResultValues;
	}
	return m_overallResultValuesQObjectPtr;
}


void CCDMResultVarRecursiveObject::SetOverallResultValues(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::complextest::COverallResultValuesObjectList* itemPtr = v.value<sdl::V1_0::complextest::COverallResultValuesObjectList*>();
		if (itemPtr != nullptr)  overallResultValues = static_cast<const decltype(overallResultValues)::value_type&>(*itemPtr);
	}
	else {
		overallResultValues = nullptr;
	}
	m_overallResultValuesQObjectPtr = v;

	overallResultValuesChanged();
}


bool CCDMResultVarRecursiveObject::hasOverallResultValues()
{
	 return overallResultValues.HasValue();
}


void CCDMResultVarRecursiveObject::emplaceOverallResultValues()
{
	ResetOverallResultValues();
	overallResultValues.emplace();
}


void CCDMResultVarRecursiveObject::ResetOverallResultValues()
{
	auto* itemPtr = m_overallResultValuesQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_overallResultValuesQObjectPtr = QVariant();
}


QVariant CCDMResultVarRecursiveObject::createOverallResultValuesArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::COverallResultValuesObject());
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


QVariant CCDMResultVarRecursiveObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "resultMetaData"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CResultMetaDataObject(this));
	}
	if (key == "resultContent"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CCDMResultUnionTypeObjectList(this));
	}
	if (key == "overallResultValues"){
		return QVariant::fromValue(new sdl::V1_0::complextest::COverallResultValuesObjectList(this));
	}
	return QVariant();
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





bool CCDMResultVarRecursiveObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CCDMResultVarRecursiveObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CCDMResultVarRecursiveObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCDMResultVarRecursiveObjectList::append(sdl::V1_0::complextest::CCDMResultVarRecursiveObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList* sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::copyMe()
{
	sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList* retVal = new sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::addElement(sdl::V1_0::complextest::CCDMResultVarRecursiveObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::isEqualWithModel(sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::insert(int index, sdl::V1_0::complextest::CCDMResultVarRecursiveObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CCDMResultVarRecursiveObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CCDMResultVarRecursiveObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_dataModelTitle"){
		return QVariant::fromValue(at(index)->dataModelTitle.value());
	}
	if (nameId == "m_dataModelVersion"){
		return QVariant::fromValue(at(index)->dataModelVersion.value());
	}
	if (nameId == "m_resultMetaData"){
		return itemPtr->GetResultMetaData();
	}
	if (nameId == "m_resultContent"){
		return itemPtr->GetResultContent();
	}
	if (nameId == "m_overallResultValues"){
		return itemPtr->GetOverallResultValues();
	}

	return QVariant();
}
CCDMResultVarStringObject::CCDMResultVarStringObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CCDMResultVarStringObject::dataModelTitleChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarStringObject::dataModelVersionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarStringObject::resultMetaDataChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarStringObject::resultContentChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultVarStringObject::overallResultValuesChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCDMResultVarStringObject::GetDataModelTitle()
{
	return dataModelTitle.value();
}


void CCDMResultVarStringObject::SetDataModelTitle(const QVariant& v)
{

	dataModelTitle = v.value<QString>();
	dataModelTitleChanged();
}


bool CCDMResultVarStringObject::hasDataModelTitle()
{
	 return dataModelTitle.HasValue();
}


QVariant CCDMResultVarStringObject::GetDataModelVersion()
{
	return dataModelVersion.value();
}


void CCDMResultVarStringObject::SetDataModelVersion(const QVariant& v)
{

	dataModelVersion = v.value<QString>();
	dataModelVersionChanged();
}


bool CCDMResultVarStringObject::hasDataModelVersion()
{
	 return dataModelVersion.HasValue();
}


QVariant CCDMResultVarStringObject::GetResultMetaData()
{
	if (!m_resultMetaDataQObjectPtr.isValid()){
		m_resultMetaDataQObjectPtr = CreateObject("resultMetaData");
		auto itemPtr = m_resultMetaDataQObjectPtr.value<sdl::V1_0::complextest::CResultMetaDataObject*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = resultMetaData;
	}
	return m_resultMetaDataQObjectPtr;
}


void CCDMResultVarStringObject::SetResultMetaData(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::complextest::CResultMetaDataObject* itemPtr = v.value<sdl::V1_0::complextest::CResultMetaDataObject*>();
		if (itemPtr != nullptr)  resultMetaData = static_cast<const decltype(resultMetaData)::value_type&>(*itemPtr);
	}
	else {
		resultMetaData = nullptr;
	}
	m_resultMetaDataQObjectPtr = v;

	resultMetaDataChanged();
}


bool CCDMResultVarStringObject::hasResultMetaData()
{
	 return resultMetaData.HasValue();
}


void CCDMResultVarStringObject::emplaceResultMetaData()
{
	ResetResultMetaData();
	resultMetaData.emplace();
}


void CCDMResultVarStringObject::ResetResultMetaData()
{
	auto* itemPtr = m_resultMetaDataQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_resultMetaDataQObjectPtr = QVariant();
}


QVariant CCDMResultVarStringObject::GetResultContent()
{
	QList<QString> tempResultContentList;
	for (const auto& tempValue: resultContent.value()){
		tempResultContentList << *tempValue;
	}
	return QVariant::fromValue(tempResultContentList);
}


void CCDMResultVarStringObject::SetResultContent(const QVariant& v)
{

	if (!resultContent){
		resultContent.emplace();
	}
	else{
		resultContent->clear();
	}
	for (const auto& tempValue: v.value<QList<QString>>()){
		istd::TSharedNullable<QString> value(tempValue);
		resultContent->append(value);
	}

	resultContentChanged();
}


bool CCDMResultVarStringObject::hasResultContent()
{
	 return resultContent.HasValue();
}


QVariant CCDMResultVarStringObject::GetOverallResultValues()
{
	if (!m_overallResultValuesQObjectPtr.isValid()){
		m_overallResultValuesQObjectPtr = CreateObject("overallResultValues");
		auto itemPtr = m_overallResultValuesQObjectPtr.value<sdl::V1_0::complextest::COverallResultValuesObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = overallResultValues;
	}
	return m_overallResultValuesQObjectPtr;
}


void CCDMResultVarStringObject::SetOverallResultValues(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::complextest::COverallResultValuesObjectList* itemPtr = v.value<sdl::V1_0::complextest::COverallResultValuesObjectList*>();
		if (itemPtr != nullptr)  overallResultValues = static_cast<const decltype(overallResultValues)::value_type&>(*itemPtr);
	}
	else {
		overallResultValues = nullptr;
	}
	m_overallResultValuesQObjectPtr = v;

	overallResultValuesChanged();
}


bool CCDMResultVarStringObject::hasOverallResultValues()
{
	 return overallResultValues.HasValue();
}


void CCDMResultVarStringObject::emplaceOverallResultValues()
{
	ResetOverallResultValues();
	overallResultValues.emplace();
}


void CCDMResultVarStringObject::ResetOverallResultValues()
{
	auto* itemPtr = m_overallResultValuesQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_overallResultValuesQObjectPtr = QVariant();
}


QVariant CCDMResultVarStringObject::createOverallResultValuesArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::COverallResultValuesObject());
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


QVariant CCDMResultVarStringObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "resultMetaData"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CResultMetaDataObject(this));
	}
	if (key == "overallResultValues"){
		return QVariant::fromValue(new sdl::V1_0::complextest::COverallResultValuesObjectList(this));
	}
	return QVariant();
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





bool CCDMResultVarStringObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CCDMResultVarStringObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CCDMResultVarStringObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCDMResultVarStringObjectList::append(sdl::V1_0::complextest::CCDMResultVarStringObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CCDMResultVarStringObjectList* sdl::V1_0::complextest::CCDMResultVarStringObjectList::copyMe()
{
	sdl::V1_0::complextest::CCDMResultVarStringObjectList* retVal = new sdl::V1_0::complextest::CCDMResultVarStringObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CCDMResultVarStringObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CCDMResultVarStringObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CCDMResultVarStringObjectList::addElement(sdl::V1_0::complextest::CCDMResultVarStringObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CCDMResultVarStringObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CCDMResultVarStringObjectList::isEqualWithModel(sdl::V1_0::complextest::CCDMResultVarStringObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CCDMResultVarStringObjectList::insert(int index, sdl::V1_0::complextest::CCDMResultVarStringObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CCDMResultVarStringObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CCDMResultVarStringObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CCDMResultVarStringObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CCDMResultVarStringObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_dataModelTitle"){
		return QVariant::fromValue(at(index)->dataModelTitle.value());
	}
	if (nameId == "m_dataModelVersion"){
		return QVariant::fromValue(at(index)->dataModelVersion.value());
	}
	if (nameId == "m_resultMetaData"){
		return itemPtr->GetResultMetaData();
	}
	if (nameId == "m_resultContent"){
		return QVariant::fromValue(at(index)->resultContent.value());
	}
	if (nameId == "m_overallResultValues"){
		return itemPtr->GetOverallResultValues();
	}

	return QVariant();
}
CCDMResultObject::CCDMResultObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CCDMResultObject::dataModelTitleChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultObject::dataModelVersionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultObject::resultMetaDataChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultObject::resultContentChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCDMResultObject::overallResultValuesChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCDMResultObject::GetDataModelTitle()
{
	return dataModelTitle.value();
}


void CCDMResultObject::SetDataModelTitle(const QVariant& v)
{

	dataModelTitle = v.value<QString>();
	dataModelTitleChanged();
}


bool CCDMResultObject::hasDataModelTitle()
{
	 return dataModelTitle.HasValue();
}


QVariant CCDMResultObject::GetDataModelVersion()
{
	return dataModelVersion.value();
}


void CCDMResultObject::SetDataModelVersion(const QVariant& v)
{

	dataModelVersion = v.value<QString>();
	dataModelVersionChanged();
}


bool CCDMResultObject::hasDataModelVersion()
{
	 return dataModelVersion.HasValue();
}


QVariant CCDMResultObject::GetResultMetaData()
{
	if (!m_resultMetaDataQObjectPtr.isValid()){
		m_resultMetaDataQObjectPtr = CreateObject("resultMetaData");
		auto itemPtr = m_resultMetaDataQObjectPtr.value<sdl::V1_0::complextest::CResultMetaDataObject*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = resultMetaData;
	}
	return m_resultMetaDataQObjectPtr;
}


void CCDMResultObject::SetResultMetaData(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::complextest::CResultMetaDataObject* itemPtr = v.value<sdl::V1_0::complextest::CResultMetaDataObject*>();
		if (itemPtr != nullptr)  resultMetaData = static_cast<const decltype(resultMetaData)::value_type&>(*itemPtr);
	}
	else {
		resultMetaData = nullptr;
	}
	m_resultMetaDataQObjectPtr = v;

	resultMetaDataChanged();
}


bool CCDMResultObject::hasResultMetaData()
{
	 return resultMetaData.HasValue();
}


void CCDMResultObject::emplaceResultMetaData()
{
	ResetResultMetaData();
	resultMetaData.emplace();
}


void CCDMResultObject::ResetResultMetaData()
{
	auto* itemPtr = m_resultMetaDataQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_resultMetaDataQObjectPtr = QVariant();
}


QVariant CCDMResultObject::GetResultContent()
{
	if (!m_resultContentQObjectPtr.isValid()){
		m_resultContentQObjectPtr = CreateObject("resultContent");
		auto itemPtr = m_resultContentQObjectPtr.value<sdl::V1_0::complextest::CCDMResultUnionTypeObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = resultContent;
	}
	return m_resultContentQObjectPtr;
}


void CCDMResultObject::SetResultContent(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::complextest::CCDMResultUnionTypeObjectList* itemPtr = v.value<sdl::V1_0::complextest::CCDMResultUnionTypeObjectList*>();
		if (itemPtr != nullptr)  resultContent = static_cast<const decltype(resultContent)::value_type&>(*itemPtr);
	}
	else {
		resultContent = nullptr;
	}
	m_resultContentQObjectPtr = v;

	resultContentChanged();
}


bool CCDMResultObject::hasResultContent()
{
	 return resultContent.HasValue();
}


void CCDMResultObject::emplaceResultContent()
{
	ResetResultContent();
	resultContent.emplace();
}


void CCDMResultObject::ResetResultContent()
{
	auto* itemPtr = m_resultContentQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_resultContentQObjectPtr = QVariant();
}


QVariant CCDMResultObject::GetOverallResultValues()
{
	if (!m_overallResultValuesQObjectPtr.isValid()){
		m_overallResultValuesQObjectPtr = CreateObject("overallResultValues");
		auto itemPtr = m_overallResultValuesQObjectPtr.value<sdl::V1_0::complextest::COverallResultValuesObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = overallResultValues;
	}
	return m_overallResultValuesQObjectPtr;
}


void CCDMResultObject::SetOverallResultValues(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::complextest::COverallResultValuesObjectList* itemPtr = v.value<sdl::V1_0::complextest::COverallResultValuesObjectList*>();
		if (itemPtr != nullptr)  overallResultValues = static_cast<const decltype(overallResultValues)::value_type&>(*itemPtr);
	}
	else {
		overallResultValues = nullptr;
	}
	m_overallResultValuesQObjectPtr = v;

	overallResultValuesChanged();
}


bool CCDMResultObject::hasOverallResultValues()
{
	 return overallResultValues.HasValue();
}


void CCDMResultObject::emplaceOverallResultValues()
{
	ResetOverallResultValues();
	overallResultValues.emplace();
}


void CCDMResultObject::ResetOverallResultValues()
{
	auto* itemPtr = m_overallResultValuesQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_overallResultValuesQObjectPtr = QVariant();
}


QVariant CCDMResultObject::createOverallResultValuesArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::complextest::COverallResultValuesObject());
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


QVariant CCDMResultObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "resultMetaData"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CResultMetaDataObject(this));
	}
	if (key == "resultContent"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CCDMResultUnionTypeObjectList(this));
	}
	if (key == "overallResultValues"){
		return QVariant::fromValue(new sdl::V1_0::complextest::COverallResultValuesObjectList(this));
	}
	return QVariant();
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





bool CCDMResultObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CCDMResultObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CCDMResultObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCDMResultObjectList::append(sdl::V1_0::complextest::CCDMResultObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CCDMResultObjectList* sdl::V1_0::complextest::CCDMResultObjectList::copyMe()
{
	sdl::V1_0::complextest::CCDMResultObjectList* retVal = new sdl::V1_0::complextest::CCDMResultObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CCDMResultObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CCDMResultObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CCDMResultObjectList::addElement(sdl::V1_0::complextest::CCDMResultObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CCDMResultObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CCDMResultObjectList::isEqualWithModel(sdl::V1_0::complextest::CCDMResultObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CCDMResultObjectList::insert(int index, sdl::V1_0::complextest::CCDMResultObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CCDMResultObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CCDMResultObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CCDMResultObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CCDMResultObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_dataModelTitle"){
		return QVariant::fromValue(at(index)->dataModelTitle.value());
	}
	if (nameId == "m_dataModelVersion"){
		return QVariant::fromValue(at(index)->dataModelVersion.value());
	}
	if (nameId == "m_resultMetaData"){
		return itemPtr->GetResultMetaData();
	}
	if (nameId == "m_resultContent"){
		return itemPtr->GetResultContent();
	}
	if (nameId == "m_overallResultValues"){
		return itemPtr->GetOverallResultValues();
	}

	return QVariant();
}
CPointsInCoordinateFormatObject::CPointsInCoordinateFormatObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CPointsInCoordinateFormatObject::pointAsCoordinatesChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPointsInCoordinateFormatObject::GetPointAsCoordinates()
{
	return pointAsCoordinates.value();
}


void CPointsInCoordinateFormatObject::SetPointAsCoordinates(const QVariant& v)
{

	pointAsCoordinates = v.value<bool>();
	pointAsCoordinatesChanged();
}


bool CPointsInCoordinateFormatObject::hasPointAsCoordinates()
{
	 return pointAsCoordinates.HasValue();
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


QVariant CPointsInCoordinateFormatObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CPointsInCoordinateFormatObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "pointAsCoordinates")){
		return "pointAsCoordinates";
	}

	return propertyName;
}





bool CPointsInCoordinateFormatObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CPointsInCoordinateFormatObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CPointsInCoordinateFormatObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CPointsInCoordinateFormatObjectList::append(sdl::V1_0::complextest::CPointsInCoordinateFormatObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList* sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::copyMe()
{
	sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList* retVal = new sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::addElement(sdl::V1_0::complextest::CPointsInCoordinateFormatObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::isEqualWithModel(sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::insert(int index, sdl::V1_0::complextest::CPointsInCoordinateFormatObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CPointsInCoordinateFormatObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CPointsInCoordinateFormatObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_pointAsCoordinates"){
		return QVariant::fromValue(at(index)->pointAsCoordinates.value());
	}

	return QVariant();
}
CCollectionViewParamsTestObject::CCollectionViewParamsTestObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CCollectionViewParamsTestObject::countChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCollectionViewParamsTestObject::offsetChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CCollectionViewParamsTestObject::filterModelChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CCollectionViewParamsTestObject::GetCount()
{
	return count.value();
}


void CCollectionViewParamsTestObject::SetCount(const QVariant& v)
{

	count = v.value<int>();
	countChanged();
}


bool CCollectionViewParamsTestObject::hasCount()
{
	 return count.HasValue();
}


QVariant CCollectionViewParamsTestObject::GetOffset()
{
	return offset.value();
}


void CCollectionViewParamsTestObject::SetOffset(const QVariant& v)
{

	offset = v.value<int>();
	offsetChanged();
}


bool CCollectionViewParamsTestObject::hasOffset()
{
	 return offset.HasValue();
}


QVariant CCollectionViewParamsTestObject::GetFilterModel()
{
	if (!m_filterModelQObjectPtr.isValid()){
		m_filterModelQObjectPtr = CreateObject("filterModel");
		auto itemPtr = m_filterModelQObjectPtr.value<sdl::V1_0::imtbase::CComplexCollectionFilterObject*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = filterModel;
	}
	return m_filterModelQObjectPtr;
}


void CCollectionViewParamsTestObject::SetFilterModel(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::imtbase::CComplexCollectionFilterObject* itemPtr = v.value<sdl::V1_0::imtbase::CComplexCollectionFilterObject*>();
		if (itemPtr != nullptr)  filterModel = static_cast<const decltype(filterModel)::value_type&>(*itemPtr);
	}
	else {
		filterModel = nullptr;
	}
	m_filterModelQObjectPtr = v;

	filterModelChanged();
}


bool CCollectionViewParamsTestObject::hasFilterModel()
{
	 return filterModel.HasValue();
}


void CCollectionViewParamsTestObject::emplaceFilterModel()
{
	ResetFilterModel();
	filterModel.emplace();
}


void CCollectionViewParamsTestObject::ResetFilterModel()
{
	auto* itemPtr = m_filterModelQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_filterModelQObjectPtr = QVariant();
}


QString CCollectionViewParamsTestObject::toJson() const
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


bool CCollectionViewParamsTestObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CCollectionViewParamsTestObject::fromObject(const QJsonObject& jsonObject)
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


QString CCollectionViewParamsTestObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CCollectionViewParamsTestObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "filterModel"){
		return QVariant::fromValue(new sdl::V1_0::imtbase::CComplexCollectionFilterObject(this));
	}
	return QVariant();
}


QString CCollectionViewParamsTestObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "count")){
		return "count";
	}
	if (propertyName == (QString("m_") + "offset")){
		return "offset";
	}
	if (propertyName == (QString("m_") + "filterModel")){
		return "filterModel";
	}

	return propertyName;
}





bool CCollectionViewParamsTestObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CCollectionViewParamsTestObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CCollectionViewParamsTestObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCollectionViewParamsTestObjectList::append(sdl::V1_0::complextest::CCollectionViewParamsTestObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CCollectionViewParamsTestObjectList* sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::copyMe()
{
	sdl::V1_0::complextest::CCollectionViewParamsTestObjectList* retVal = new sdl::V1_0::complextest::CCollectionViewParamsTestObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::addElement(sdl::V1_0::complextest::CCollectionViewParamsTestObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::isEqualWithModel(sdl::V1_0::complextest::CCollectionViewParamsTestObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::insert(int index, sdl::V1_0::complextest::CCollectionViewParamsTestObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CCollectionViewParamsTestObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CCollectionViewParamsTestObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_count"){
		return QVariant::fromValue(at(index)->count.value());
	}
	if (nameId == "m_offset"){
		return QVariant::fromValue(at(index)->offset.value());
	}
	if (nameId == "m_filterModel"){
		return itemPtr->GetFilterModel();
	}

	return QVariant();
}



QVariant CExtendedMetaDataUnionTypeObjectList::GetOrCreateCachedObject(int index) const
{
	QVariant retVal;
	if (this->m_objectDataTypeMap.contains(index)){
		retVal = this->m_objectDataTypeMap[index];
	}
	else{
		if (const CCoordinates* val = std::get_if<CCoordinates>(this->at(index).GetPtr())){
			CCoordinatesObject *newObjectPtr = new CCoordinatesObject();
			static_cast<decltype(*newObjectPtr)&>(*newObjectPtr) = *val;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		if (const double* val = std::get_if<double>(this->at(index).GetPtr())){
			retVal =  QVariant::fromValue(val);
		}

		if (const bool* val = std::get_if<bool>(this->at(index).GetPtr())){
			retVal =  QVariant::fromValue(val);
		}

		if (const QString* val = std::get_if<QString>(this->at(index).GetPtr())){
			retVal =  QVariant::fromValue(val);
		}

		this->m_objectDataTypeMap.insert(index, retVal);
	}
	return retVal;
}


bool CExtendedMetaDataUnionTypeObjectList::containsKey(const QString& nameId, int /*index*/)
{
	return nameId == "item";
}


int CExtendedMetaDataUnionTypeObjectList::getItemsCount()
{
	return rowCount();
}


QVariantMap CExtendedMetaDataUnionTypeObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CExtendedMetaDataUnionTypeObjectList::append(QVariant item)
{
	beginInsertRows(QModelIndex(), count(), 0);

	if (item.canConvert<CCoordinatesObject*>()){
		CCoordinatesObject* val = item.value<CCoordinatesObject*>();
		CCoordinates* newItemPtr = dynamic_cast<CCoordinates*>(val);
		append(istd::TSharedNullable<ExtendedMetaDataUnionType>(*newItemPtr));
	}

	if (item.canConvert<double>()){
		double val = item.value<double>();
		append(istd::TSharedNullable<ExtendedMetaDataUnionType>(val));
	}

	if (item.canConvert<bool>()){
		bool val = item.value<bool>();
		append(istd::TSharedNullable<ExtendedMetaDataUnionType>(val));
	}

	if (item.canConvert<QString>()){
		QString val = item.value<QString>();
		append(istd::TSharedNullable<ExtendedMetaDataUnionType>(val));
	}

	ClearCache();
	endInsertRows();
}


sdl::V1_0::complextest::CExtendedMetaDataUnionTypeObjectList* CExtendedMetaDataUnionTypeObjectList::copyMe()
{
	sdl::V1_0::complextest::CExtendedMetaDataUnionTypeObjectList* objectListPtr = new sdl::V1_0::complextest::CExtendedMetaDataUnionTypeObjectList();

	for (int i = 0; i < this->rowCount(); i++){
		QVariant item = this->getData("item", i);

		if (item.canConvert<CCoordinatesObject*>()){
			CCoordinatesObject* val = item.value<CCoordinatesObject*>();
			objectListPtr->addElement(QVariant::fromValue(val->copyMe()));
		}

		if (item.canConvert<double>()){
			double val = item.value<double>();
			objectListPtr->addElement(val);
		}

		if (item.canConvert<bool>()){
			bool val = item.value<bool>();
			objectListPtr->addElement(val);
		}

		if (item.canConvert<QString>()){
			QString val = item.value<QString>();
			objectListPtr->addElement(val);
		}
	}
	return objectListPtr;
}


QString CExtendedMetaDataUnionTypeObjectList::toJson()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<CCoordinatesObject*>()){
			CCoordinatesObject* val = item.value<CCoordinatesObject*>();
			retVal += val->toJson();
		}

		if (item.canConvert<double>()){
			double val = item.value<double>();
			retVal += QString::number(val);
		}

		if (item.canConvert<bool>()){
			bool val = item.value<bool>();
			retVal += QString(val ? "true" : "false");
		}

		if (item.canConvert<QString>()){
			QString val = item.value<QString>();
			retVal += QString(val);
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


QString CExtendedMetaDataUnionTypeObjectList::toGraphQL()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<CCoordinatesObject*>()){
			CCoordinatesObject* val = item.value<CCoordinatesObject*>();
			retVal += val->toGraphQL();
		}

		if (item.canConvert<double>()){
			double val = item.value<double>();
			retVal += QString::number(val);
		}

		if (item.canConvert<bool>()){
			bool val = item.value<bool>();
			retVal += QString(val ? "true" : "false");
		}

		if (item.canConvert<QString>()){
			QString val = item.value<QString>();
			retVal += QString(val);
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


void CExtendedMetaDataUnionTypeObjectList::addElement(QVariant item)
{
	append(item);
}

void CExtendedMetaDataUnionTypeObjectList::removeElement(int index)
{
	remove(index);
}

bool CExtendedMetaDataUnionTypeObjectList::isEqualWithModel(sdl::V1_0::complextest::CExtendedMetaDataUnionTypeObjectList* otherModelPtr)
{
	if (otherModelPtr == nullptr){
		return false;
	}

	if (this == otherModelPtr){
		return false;
	}

	if (this->rowCount() != otherModelPtr->rowCount()){
		return false;
	}

	for (int i = 0; i < this->rowCount(); i++){
		QVariant selfItem = this->getData("item", i);
		QVariant otherItem = otherModelPtr->getData("item", i);
		if (selfItem != otherItem){
			return false;
		}

	}

	return true;
}


void CExtendedMetaDataUnionTypeObjectList::insert(int index, QVariant item)
{
	if (index < 0 || index > size()) return;
	beginInsertRows(QModelIndex(), count(), 0);

	if (item.canConvert<CCoordinatesObject*>()){
		CCoordinatesObject* val = item.value<CCoordinatesObject*>();
		CCoordinates* newItemPtr = dynamic_cast<CCoordinates*>(val);
		append(istd::TSharedNullable<ExtendedMetaDataUnionType>(*newItemPtr));
	}

	if (item.canConvert<double>()){
		double val = item.value<double>();
		append(istd::TSharedNullable<ExtendedMetaDataUnionType>(val));
	}

	if (item.canConvert<bool>()){
		bool val = item.value<bool>();
		append(istd::TSharedNullable<ExtendedMetaDataUnionType>(val));
	}

	if (item.canConvert<QString>()){
		QString val = item.value<QString>();
		append(istd::TSharedNullable<ExtendedMetaDataUnionType>(val));
	}

	ClearCache();
	endInsertRows();
}


void CExtendedMetaDataUnionTypeObjectList::remove(int index)
{
	BaseClass::remove(index);
}


void CExtendedMetaDataUnionTypeObjectList::clear()
{
	BaseClass::clear();
}


QVariant CExtendedMetaDataUnionTypeObjectList::getData(const QString& nameId, int index)
{
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		QVariant retVal = GetOrCreateCachedObject(index);
		return retVal;
	}
	return QVariant();
}



QVariant CExtendedMetaDataUnionSimpleTypeObjectList::GetOrCreateCachedObject(int index) const
{
	QVariant retVal;
	if (this->m_objectDataTypeMap.contains(index)){
		retVal = this->m_objectDataTypeMap[index];
	}
	else{
		if (const double* val = std::get_if<double>(this->at(index).GetPtr())){
			retVal =  QVariant::fromValue(val);
		}

		if (const bool* val = std::get_if<bool>(this->at(index).GetPtr())){
			retVal =  QVariant::fromValue(val);
		}

		if (const QString* val = std::get_if<QString>(this->at(index).GetPtr())){
			retVal =  QVariant::fromValue(val);
		}

		this->m_objectDataTypeMap.insert(index, retVal);
	}
	return retVal;
}


bool CExtendedMetaDataUnionSimpleTypeObjectList::containsKey(const QString& nameId, int /*index*/)
{
	return nameId == "item";
}


int CExtendedMetaDataUnionSimpleTypeObjectList::getItemsCount()
{
	return rowCount();
}


QVariantMap CExtendedMetaDataUnionSimpleTypeObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CExtendedMetaDataUnionSimpleTypeObjectList::append(QVariant item)
{
	beginInsertRows(QModelIndex(), count(), 0);

	if (item.canConvert<double>()){
		double val = item.value<double>();
		append(istd::TSharedNullable<ExtendedMetaDataUnionSimpleType>(val));
	}

	if (item.canConvert<bool>()){
		bool val = item.value<bool>();
		append(istd::TSharedNullable<ExtendedMetaDataUnionSimpleType>(val));
	}

	if (item.canConvert<QString>()){
		QString val = item.value<QString>();
		append(istd::TSharedNullable<ExtendedMetaDataUnionSimpleType>(val));
	}

	ClearCache();
	endInsertRows();
}


sdl::V1_0::complextest::CExtendedMetaDataUnionSimpleTypeObjectList* CExtendedMetaDataUnionSimpleTypeObjectList::copyMe()
{
	sdl::V1_0::complextest::CExtendedMetaDataUnionSimpleTypeObjectList* objectListPtr = new sdl::V1_0::complextest::CExtendedMetaDataUnionSimpleTypeObjectList();

	for (int i = 0; i < this->rowCount(); i++){
		QVariant item = this->getData("item", i);

		if (item.canConvert<double>()){
			double val = item.value<double>();
			objectListPtr->addElement(val);
		}

		if (item.canConvert<bool>()){
			bool val = item.value<bool>();
			objectListPtr->addElement(val);
		}

		if (item.canConvert<QString>()){
			QString val = item.value<QString>();
			objectListPtr->addElement(val);
		}
	}
	return objectListPtr;
}


QString CExtendedMetaDataUnionSimpleTypeObjectList::toJson()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<double>()){
			double val = item.value<double>();
			retVal += QString::number(val);
		}

		if (item.canConvert<bool>()){
			bool val = item.value<bool>();
			retVal += QString(val ? "true" : "false");
		}

		if (item.canConvert<QString>()){
			QString val = item.value<QString>();
			retVal += QString(val);
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


QString CExtendedMetaDataUnionSimpleTypeObjectList::toGraphQL()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<double>()){
			double val = item.value<double>();
			retVal += QString::number(val);
		}

		if (item.canConvert<bool>()){
			bool val = item.value<bool>();
			retVal += QString(val ? "true" : "false");
		}

		if (item.canConvert<QString>()){
			QString val = item.value<QString>();
			retVal += QString(val);
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


void CExtendedMetaDataUnionSimpleTypeObjectList::addElement(QVariant item)
{
	append(item);
}

void CExtendedMetaDataUnionSimpleTypeObjectList::removeElement(int index)
{
	remove(index);
}

bool CExtendedMetaDataUnionSimpleTypeObjectList::isEqualWithModel(sdl::V1_0::complextest::CExtendedMetaDataUnionSimpleTypeObjectList* otherModelPtr)
{
	if (otherModelPtr == nullptr){
		return false;
	}

	if (this == otherModelPtr){
		return false;
	}

	if (this->rowCount() != otherModelPtr->rowCount()){
		return false;
	}

	for (int i = 0; i < this->rowCount(); i++){
		QVariant selfItem = this->getData("item", i);
		QVariant otherItem = otherModelPtr->getData("item", i);
		if (selfItem != otherItem){
			return false;
		}

	}

	return true;
}


void CExtendedMetaDataUnionSimpleTypeObjectList::insert(int index, QVariant item)
{
	if (index < 0 || index > size()) return;
	beginInsertRows(QModelIndex(), count(), 0);

	if (item.canConvert<double>()){
		double val = item.value<double>();
		append(istd::TSharedNullable<ExtendedMetaDataUnionSimpleType>(val));
	}

	if (item.canConvert<bool>()){
		bool val = item.value<bool>();
		append(istd::TSharedNullable<ExtendedMetaDataUnionSimpleType>(val));
	}

	if (item.canConvert<QString>()){
		QString val = item.value<QString>();
		append(istd::TSharedNullable<ExtendedMetaDataUnionSimpleType>(val));
	}

	ClearCache();
	endInsertRows();
}


void CExtendedMetaDataUnionSimpleTypeObjectList::remove(int index)
{
	BaseClass::remove(index);
}


void CExtendedMetaDataUnionSimpleTypeObjectList::clear()
{
	BaseClass::clear();
}


QVariant CExtendedMetaDataUnionSimpleTypeObjectList::getData(const QString& nameId, int index)
{
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		QVariant retVal = GetOrCreateCachedObject(index);
		return retVal;
	}
	return QVariant();
}



QVariant CCDMResultUnionTypeObjectList::GetOrCreateCachedObject(int index) const
{
	QVariant retVal;
	if (this->m_objectDataTypeMap.contains(index)){
		retVal = this->m_objectDataTypeMap[index];
	}
	else{
		if (const CCDMResultVarString* val = std::get_if<CCDMResultVarString>(this->at(index).GetPtr())){
			CCDMResultVarStringObject *newObjectPtr = new CCDMResultVarStringObject();
			static_cast<decltype(*newObjectPtr)&>(*newObjectPtr) = *val;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		if (const CCDMResultVarRecursive* val = std::get_if<CCDMResultVarRecursive>(this->at(index).GetPtr())){
			CCDMResultVarRecursiveObject *newObjectPtr = new CCDMResultVarRecursiveObject();
			static_cast<decltype(*newObjectPtr)&>(*newObjectPtr) = *val;
			retVal = QVariant::fromValue(newObjectPtr);
		}

		this->m_objectDataTypeMap.insert(index, retVal);
	}
	return retVal;
}


bool CCDMResultUnionTypeObjectList::containsKey(const QString& nameId, int /*index*/)
{
	return nameId == "item";
}


int CCDMResultUnionTypeObjectList::getItemsCount()
{
	return rowCount();
}


QVariantMap CCDMResultUnionTypeObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CCDMResultUnionTypeObjectList::append(QVariant item)
{
	beginInsertRows(QModelIndex(), count(), 0);

	if (item.canConvert<CCDMResultVarStringObject*>()){
		CCDMResultVarStringObject* val = item.value<CCDMResultVarStringObject*>();
		CCDMResultVarString* newItemPtr = dynamic_cast<CCDMResultVarString*>(val);
		append(istd::TSharedNullable<CDMResultUnionType>(*newItemPtr));
	}

	if (item.canConvert<CCDMResultVarRecursiveObject*>()){
		CCDMResultVarRecursiveObject* val = item.value<CCDMResultVarRecursiveObject*>();
		CCDMResultVarRecursive* newItemPtr = dynamic_cast<CCDMResultVarRecursive*>(val);
		append(istd::TSharedNullable<CDMResultUnionType>(*newItemPtr));
	}

	ClearCache();
	endInsertRows();
}


sdl::V1_0::complextest::CCDMResultUnionTypeObjectList* CCDMResultUnionTypeObjectList::copyMe()
{
	sdl::V1_0::complextest::CCDMResultUnionTypeObjectList* objectListPtr = new sdl::V1_0::complextest::CCDMResultUnionTypeObjectList();

	for (int i = 0; i < this->rowCount(); i++){
		QVariant item = this->getData("item", i);

		if (item.canConvert<CCDMResultVarStringObject*>()){
			CCDMResultVarStringObject* val = item.value<CCDMResultVarStringObject*>();
			objectListPtr->addElement(QVariant::fromValue(val->copyMe()));
		}

		if (item.canConvert<CCDMResultVarRecursiveObject*>()){
			CCDMResultVarRecursiveObject* val = item.value<CCDMResultVarRecursiveObject*>();
			objectListPtr->addElement(QVariant::fromValue(val->copyMe()));
		}
	}
	return objectListPtr;
}


QString CCDMResultUnionTypeObjectList::toJson()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<CCDMResultVarStringObject*>()){
			CCDMResultVarStringObject* val = item.value<CCDMResultVarStringObject*>();
			retVal += val->toJson();
		}

		if (item.canConvert<CCDMResultVarRecursiveObject*>()){
			CCDMResultVarRecursiveObject* val = item.value<CCDMResultVarRecursiveObject*>();
			retVal += val->toJson();
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


QString CCDMResultUnionTypeObjectList::toGraphQL()
{
	QString retVal = QStringLiteral("[");

	for (int i = 0; i < this->rowCount(); i++){
		if (i > 0 && i < this->rowCount() - 1){
			retVal += QStringLiteral(", ");
		}

		QVariant item = this->getData("item", i);

		if (item.canConvert<CCDMResultVarStringObject*>()){
			CCDMResultVarStringObject* val = item.value<CCDMResultVarStringObject*>();
			retVal += val->toGraphQL();
		}

		if (item.canConvert<CCDMResultVarRecursiveObject*>()){
			CCDMResultVarRecursiveObject* val = item.value<CCDMResultVarRecursiveObject*>();
			retVal += val->toGraphQL();
		}
	}

	retVal += QStringLiteral("]");

	return retVal;
}


void CCDMResultUnionTypeObjectList::addElement(QVariant item)
{
	append(item);
}

void CCDMResultUnionTypeObjectList::removeElement(int index)
{
	remove(index);
}

bool CCDMResultUnionTypeObjectList::isEqualWithModel(sdl::V1_0::complextest::CCDMResultUnionTypeObjectList* otherModelPtr)
{
	if (otherModelPtr == nullptr){
		return false;
	}

	if (this == otherModelPtr){
		return false;
	}

	if (this->rowCount() != otherModelPtr->rowCount()){
		return false;
	}

	for (int i = 0; i < this->rowCount(); i++){
		QVariant selfItem = this->getData("item", i);
		QVariant otherItem = otherModelPtr->getData("item", i);
		if (selfItem != otherItem){
			return false;
		}

	}

	return true;
}


void CCDMResultUnionTypeObjectList::insert(int index, QVariant item)
{
	if (index < 0 || index > size()) return;
	beginInsertRows(QModelIndex(), count(), 0);

	if (item.canConvert<CCDMResultVarStringObject*>()){
		CCDMResultVarStringObject* val = item.value<CCDMResultVarStringObject*>();
		CCDMResultVarString* newItemPtr = dynamic_cast<CCDMResultVarString*>(val);
		append(istd::TSharedNullable<CDMResultUnionType>(*newItemPtr));
	}

	if (item.canConvert<CCDMResultVarRecursiveObject*>()){
		CCDMResultVarRecursiveObject* val = item.value<CCDMResultVarRecursiveObject*>();
		CCDMResultVarRecursive* newItemPtr = dynamic_cast<CCDMResultVarRecursive*>(val);
		append(istd::TSharedNullable<CDMResultUnionType>(*newItemPtr));
	}

	ClearCache();
	endInsertRows();
}


void CCDMResultUnionTypeObjectList::remove(int index)
{
	BaseClass::remove(index);
}


void CCDMResultUnionTypeObjectList::clear()
{
	BaseClass::clear();
}


QVariant CCDMResultUnionTypeObjectList::getData(const QString& nameId, int index)
{
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		QVariant retVal = GetOrCreateCachedObject(index);
		return retVal;
	}
	return QVariant();
}


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
														m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.isRequiredPointsRequested = true;
														m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.isOptionalPointsRequested = geometryRequestedIds.contains("OptionalPoints");
														const imtgql::CGqlFieldObject* pointsRequestedFieldsPtr = geometryRequestedFieldsPtr->GetFieldArgumentObjectPtr("Points");
														if (pointsRequestedFieldsPtr != nullptr){
															const QByteArrayList pointsRequestedIds = pointsRequestedFieldsPtr->GetFieldIds();
															if (!pointsRequestedIds.isEmpty()){
																m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.Points.isXRequested = true;
																m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.Points.isYRequested = true;
															}
														}
														const imtgql::CGqlFieldObject* requiredPointsRequestedFieldsPtr = geometryRequestedFieldsPtr->GetFieldArgumentObjectPtr("RequiredPoints");
														if (requiredPointsRequestedFieldsPtr != nullptr){
															const QByteArrayList requiredPointsRequestedIds = requiredPointsRequestedFieldsPtr->GetFieldIds();
															if (!requiredPointsRequestedIds.isEmpty()){
																m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.RequiredPoints.isXRequested = true;
																m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.RequiredPoints.isYRequested = true;
															}
														}
														const imtgql::CGqlFieldObject* optionalPointsRequestedFieldsPtr = geometryRequestedFieldsPtr->GetFieldArgumentObjectPtr("OptionalPoints");
														if (optionalPointsRequestedFieldsPtr != nullptr){
															const QByteArrayList optionalPointsRequestedIds = optionalPointsRequestedFieldsPtr->GetFieldIds();
															if (!optionalPointsRequestedIds.isEmpty()){
																m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.OptionalPoints.isXRequested = true;
																m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.OptionalPoints.isYRequested = true;
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
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to create GQL Object").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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
bool CComplexUnion2GqlHandlerCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
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


QJsonObject CComplexUnion2GqlHandlerCompBase::CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const QByteArray commandId = gqlRequest.GetCommandId();
	QJsonObject modelObj;
	QJsonObject dataModelObj;

	// GetLastProductionResults
	if (commandId == CGetLastProductionResultsGqlRequest::GetCommandId()){
		CGetLastProductionResultsGqlRequest getLastProductionResultsGqlRequest(gqlRequest, false);
		if (!getLastProductionResultsGqlRequest.IsValid()){
			errorMessage = QString("Bad request. Unexpected request for command-ID: '%1'").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage);

			return QJsonObject();
		}

		CProductOverview replyPayload = OnGetLastProductionResults(getLastProductionResultsGqlRequest, gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			SendErrorMessage(0, QString("The derived call [OnGetLastProductionResults] returned an error: %1").arg(errorMessage));

			return QJsonObject();
		}

		const bool isModelCreated = replyPayload.WriteToJsonObject(dataModelObj);
		if (!isModelCreated){
			errorMessage = QString("Internal error. Unable to create response for command-ID: '%1'").arg(qPrintable(commandId));
			SendCriticalMessage(0, errorMessage);

			return QJsonObject();
		}

		modelObj.insert(QStringLiteral("data"), dataModelObj); return modelObj;
	}

	// GetLastProductionResultsCDM
	if (commandId == CGetLastProductionResultsCDMGqlRequest::GetCommandId()){
		CGetLastProductionResultsCDMGqlRequest getLastProductionResultsCDMGqlRequest(gqlRequest, false);
		if (!getLastProductionResultsCDMGqlRequest.IsValid()){
			errorMessage = QString("Bad request. Unexpected request for command-ID: '%1'").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage);

			return QJsonObject();
		}

		CCDMResult replyPayload = OnGetLastProductionResultsCDM(getLastProductionResultsCDMGqlRequest, gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			SendErrorMessage(0, QString("The derived call [OnGetLastProductionResultsCDM] returned an error: %1").arg(errorMessage));

			return QJsonObject();
		}

		const bool isModelCreated = replyPayload.WriteToJsonObject(dataModelObj);
		if (!isModelCreated){
			errorMessage = QString("Internal error. Unable to create response for command-ID: '%1'").arg(qPrintable(commandId));
			SendCriticalMessage(0, errorMessage);

			return QJsonObject();
		}

		modelObj.insert(QStringLiteral("data"), dataModelObj); return modelObj;
	}

	errorMessage = QString("Bad request. Unexpected command-ID: '%1'").arg(qPrintable(commandId));
	SendErrorMessage(0, errorMessage);

	return QJsonObject();
}


} // namespace sdl::V1_0::complextest
