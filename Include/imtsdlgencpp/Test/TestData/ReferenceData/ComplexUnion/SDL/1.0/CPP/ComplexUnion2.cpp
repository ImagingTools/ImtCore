#include "ComplexUnion2.h"


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


bool V1_0::complextest::CResult::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Result", modelIndex);

	if (!AreaResult){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaResult").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMinValue").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("ExpectedMinValue", *ExpectedMinValue, modelIndex);

	if (!ExpectedMaxValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMaxValue").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("ExpectedMaxValue", *ExpectedMaxValue, modelIndex);

	if (!MeasuredValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("MeasuredValue", *MeasuredValue, modelIndex);

	if (!MinMeasuredValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MinMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("MinMeasuredValue", *MinMeasuredValue, modelIndex);

	if (!MaxMeasuredValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MaxMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("MaxMeasuredValue", *MaxMeasuredValue, modelIndex);

	if (!MeanMeasuredValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeanMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("MeanMeasuredValue", *MeanMeasuredValue, modelIndex);

	if (!MeasurementType){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Length").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("Length", *Length, modelIndex);

	if (!ErrorType){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}
	::imtbase::CTreeItemModel* geometryNewModelPtr = model.AddTreeModel("Geometry", modelIndex);
	const bool isGeometryAdded = Geometry->WriteToModel(*geometryNewModelPtr, 0);
	if (!isGeometryAdded){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}


	return true;
}


bool V1_0::complextest::CResult::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant areaResultData = model.GetData("AreaResult", modelIndex);
	if (areaResultData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "AreaResult").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", areaResultStringValue);)

		return false;
	}

	QVariant expectedMinValueData = model.GetData("ExpectedMinValue", modelIndex);
	if (expectedMinValueData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMinValue").toLocal8Bit().constData();)

		return false;
	}
	ExpectedMinValue = expectedMinValueData.toFloat();

	QVariant expectedMaxValueData = model.GetData("ExpectedMaxValue", modelIndex);
	if (expectedMaxValueData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMaxValue").toLocal8Bit().constData();)

		return false;
	}
	ExpectedMaxValue = expectedMaxValueData.toFloat();

	QVariant measuredValueData = model.GetData("MeasuredValue", modelIndex);
	if (measuredValueData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MeasuredValue = measuredValueData.toFloat();

	QVariant minMeasuredValueData = model.GetData("MinMeasuredValue", modelIndex);
	if (minMeasuredValueData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MinMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MinMeasuredValue = minMeasuredValueData.toFloat();

	QVariant maxMeasuredValueData = model.GetData("MaxMeasuredValue", modelIndex);
	if (maxMeasuredValueData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MaxMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MaxMeasuredValue = maxMeasuredValueData.toFloat();

	QVariant meanMeasuredValueData = model.GetData("MeanMeasuredValue", modelIndex);
	if (meanMeasuredValueData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MeanMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MeanMeasuredValue = meanMeasuredValueData.toFloat();

	QVariant measurementTypeData = model.GetData("MeasurementType", modelIndex);
	if (measurementTypeData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

		return false;
	}

	QVariant measurementUnitData = model.GetData("MeasurementUnit", modelIndex);
	if (measurementUnitData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit", measurementUnitStringValue);)

		return false;
	}

	QVariant lengthData = model.GetData("Length", modelIndex);
	if (lengthData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Length").toLocal8Bit().constData();)

		return false;
	}
	Length = lengthData.toFloat();

	QVariant errorTypeData = model.GetData("ErrorType", modelIndex);
	if (errorTypeData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* geometryDataModelPtr = model.GetTreeItemModel("Geometry", modelIndex);
	if (geometryDataModelPtr == nullptr){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' is missing, but required").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}
	Geometry = V1_0::complextest::CGeometry();
	const bool isGeometryRead = Geometry->ReadFromModel(*geometryDataModelPtr, modelIndex);
	if (!isGeometryRead){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}


	return true;
}


bool V1_0::complextest::CResult::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", areaResultStringValue);)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit", measurementUnitStringValue);)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* geometryDataModelPtr = model.GetTreeItemModel("Geometry", modelIndex);
	if (geometryDataModelPtr != nullptr){
		Geometry = V1_0::complextest::CGeometry();
		const bool isGeometryRead = Geometry->ReadFromModel(*geometryDataModelPtr, modelIndex);
		if (!isGeometryRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool V1_0::complextest::CResult::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!AreaResult){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaResult").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMinValue").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("ExpectedMinValue", QVariant(*ExpectedMinValue));

	if (!ExpectedMaxValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMaxValue").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("ExpectedMaxValue", QVariant(*ExpectedMaxValue));

	if (!MeasuredValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("MeasuredValue", QVariant(*MeasuredValue));

	if (!MinMeasuredValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MinMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("MinMeasuredValue", QVariant(*MinMeasuredValue));

	if (!MaxMeasuredValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MaxMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("MaxMeasuredValue", QVariant(*MaxMeasuredValue));

	if (!MeanMeasuredValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeanMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("MeanMeasuredValue", QVariant(*MeanMeasuredValue));

	if (!MeasurementType){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Length").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("Length", QVariant(*Length));

	if (!ErrorType){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("Geometry", geometryGqlObject);

	gqlObject.InsertParam("__typename", QVariant("Result"));

	return true;
}


bool V1_0::complextest::CResult::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("AreaResult") || (gqlObject["AreaResult"].userType() != QMetaType::QString && gqlObject["AreaResult"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaResult").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", areaResultStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("ExpectedMinValue") || (gqlObject["ExpectedMinValue"].userType() != QMetaType::Float && gqlObject["ExpectedMinValue"].userType() != QMetaType::Double && gqlObject["ExpectedMinValue"].userType() != QMetaType::Int && gqlObject["ExpectedMinValue"].userType() != QMetaType::UInt && gqlObject["ExpectedMinValue"].userType() != QMetaType::LongLong && gqlObject["ExpectedMinValue"].userType() != QMetaType::ULongLong && gqlObject["ExpectedMinValue"].userType() != QMetaType::Long && gqlObject["ExpectedMinValue"].userType() != QMetaType::Short && gqlObject["ExpectedMinValue"].userType() != QMetaType::ULong && gqlObject["ExpectedMinValue"].userType() != QMetaType::UShort && gqlObject["ExpectedMinValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMinValue").toLocal8Bit().constData();)

		return false;
	}
	ExpectedMinValue = gqlObject["ExpectedMinValue"].toDouble();

	if (!gqlObject.ContainsParam("ExpectedMaxValue") || (gqlObject["ExpectedMaxValue"].userType() != QMetaType::Float && gqlObject["ExpectedMaxValue"].userType() != QMetaType::Double && gqlObject["ExpectedMaxValue"].userType() != QMetaType::Int && gqlObject["ExpectedMaxValue"].userType() != QMetaType::UInt && gqlObject["ExpectedMaxValue"].userType() != QMetaType::LongLong && gqlObject["ExpectedMaxValue"].userType() != QMetaType::ULongLong && gqlObject["ExpectedMaxValue"].userType() != QMetaType::Long && gqlObject["ExpectedMaxValue"].userType() != QMetaType::Short && gqlObject["ExpectedMaxValue"].userType() != QMetaType::ULong && gqlObject["ExpectedMaxValue"].userType() != QMetaType::UShort && gqlObject["ExpectedMaxValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMaxValue").toLocal8Bit().constData();)

		return false;
	}
	ExpectedMaxValue = gqlObject["ExpectedMaxValue"].toDouble();

	if (!gqlObject.ContainsParam("MeasuredValue") || (gqlObject["MeasuredValue"].userType() != QMetaType::Float && gqlObject["MeasuredValue"].userType() != QMetaType::Double && gqlObject["MeasuredValue"].userType() != QMetaType::Int && gqlObject["MeasuredValue"].userType() != QMetaType::UInt && gqlObject["MeasuredValue"].userType() != QMetaType::LongLong && gqlObject["MeasuredValue"].userType() != QMetaType::ULongLong && gqlObject["MeasuredValue"].userType() != QMetaType::Long && gqlObject["MeasuredValue"].userType() != QMetaType::Short && gqlObject["MeasuredValue"].userType() != QMetaType::ULong && gqlObject["MeasuredValue"].userType() != QMetaType::UShort && gqlObject["MeasuredValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MeasuredValue = gqlObject["MeasuredValue"].toDouble();

	if (!gqlObject.ContainsParam("MinMeasuredValue") || (gqlObject["MinMeasuredValue"].userType() != QMetaType::Float && gqlObject["MinMeasuredValue"].userType() != QMetaType::Double && gqlObject["MinMeasuredValue"].userType() != QMetaType::Int && gqlObject["MinMeasuredValue"].userType() != QMetaType::UInt && gqlObject["MinMeasuredValue"].userType() != QMetaType::LongLong && gqlObject["MinMeasuredValue"].userType() != QMetaType::ULongLong && gqlObject["MinMeasuredValue"].userType() != QMetaType::Long && gqlObject["MinMeasuredValue"].userType() != QMetaType::Short && gqlObject["MinMeasuredValue"].userType() != QMetaType::ULong && gqlObject["MinMeasuredValue"].userType() != QMetaType::UShort && gqlObject["MinMeasuredValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MinMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MinMeasuredValue = gqlObject["MinMeasuredValue"].toDouble();

	if (!gqlObject.ContainsParam("MaxMeasuredValue") || (gqlObject["MaxMeasuredValue"].userType() != QMetaType::Float && gqlObject["MaxMeasuredValue"].userType() != QMetaType::Double && gqlObject["MaxMeasuredValue"].userType() != QMetaType::Int && gqlObject["MaxMeasuredValue"].userType() != QMetaType::UInt && gqlObject["MaxMeasuredValue"].userType() != QMetaType::LongLong && gqlObject["MaxMeasuredValue"].userType() != QMetaType::ULongLong && gqlObject["MaxMeasuredValue"].userType() != QMetaType::Long && gqlObject["MaxMeasuredValue"].userType() != QMetaType::Short && gqlObject["MaxMeasuredValue"].userType() != QMetaType::ULong && gqlObject["MaxMeasuredValue"].userType() != QMetaType::UShort && gqlObject["MaxMeasuredValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MaxMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MaxMeasuredValue = gqlObject["MaxMeasuredValue"].toDouble();

	if (!gqlObject.ContainsParam("MeanMeasuredValue") || (gqlObject["MeanMeasuredValue"].userType() != QMetaType::Float && gqlObject["MeanMeasuredValue"].userType() != QMetaType::Double && gqlObject["MeanMeasuredValue"].userType() != QMetaType::Int && gqlObject["MeanMeasuredValue"].userType() != QMetaType::UInt && gqlObject["MeanMeasuredValue"].userType() != QMetaType::LongLong && gqlObject["MeanMeasuredValue"].userType() != QMetaType::ULongLong && gqlObject["MeanMeasuredValue"].userType() != QMetaType::Long && gqlObject["MeanMeasuredValue"].userType() != QMetaType::Short && gqlObject["MeanMeasuredValue"].userType() != QMetaType::ULong && gqlObject["MeanMeasuredValue"].userType() != QMetaType::UShort && gqlObject["MeanMeasuredValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeanMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MeanMeasuredValue = gqlObject["MeanMeasuredValue"].toDouble();

	if (!gqlObject.ContainsParam("MeasurementType") || (gqlObject["MeasurementType"].userType() != QMetaType::QString && gqlObject["MeasurementType"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("MeasurementUnit") || (gqlObject["MeasurementUnit"].userType() != QMetaType::QString && gqlObject["MeasurementUnit"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit", measurementUnitStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("Length") || (gqlObject["Length"].userType() != QMetaType::Float && gqlObject["Length"].userType() != QMetaType::Double && gqlObject["Length"].userType() != QMetaType::Int && gqlObject["Length"].userType() != QMetaType::UInt && gqlObject["Length"].userType() != QMetaType::LongLong && gqlObject["Length"].userType() != QMetaType::ULongLong && gqlObject["Length"].userType() != QMetaType::Long && gqlObject["Length"].userType() != QMetaType::Short && gqlObject["Length"].userType() != QMetaType::ULong && gqlObject["Length"].userType() != QMetaType::UShort && gqlObject["Length"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Length").toLocal8Bit().constData();)

		return false;
	}
	Length = gqlObject["Length"].toDouble();

	if (!gqlObject.ContainsParam("ErrorType") || (gqlObject["ErrorType"].userType() != QMetaType::QString && gqlObject["ErrorType"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("Geometry") || (gqlObject.GetParamArgumentObjectPtr("Geometry") == nullptr)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}
	Geometry = V1_0::complextest::CGeometry();
	const bool isGeometryRead = Geometry->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("Geometry"));
	if (!isGeometryRead){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}

	return true;
}


bool V1_0::complextest::CResult::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", areaResultStringValue);)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit", measurementUnitStringValue);)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("Geometry") && (gqlObject.GetParamArgumentObjectPtr("Geometry") != nullptr)){
		Geometry = V1_0::complextest::CGeometry();
		const bool isGeometryRead = Geometry->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("Geometry"));
		if (!isGeometryRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool V1_0::complextest::CResult::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!AreaResult){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaResult").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMinValue").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["ExpectedMinValue"] = QJsonValue::fromVariant(*ExpectedMinValue);

	if (!ExpectedMaxValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMaxValue").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["ExpectedMaxValue"] = QJsonValue::fromVariant(*ExpectedMaxValue);

	if (!MeasuredValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["MeasuredValue"] = QJsonValue::fromVariant(*MeasuredValue);

	if (!MinMeasuredValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MinMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["MinMeasuredValue"] = QJsonValue::fromVariant(*MinMeasuredValue);

	if (!MaxMeasuredValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MaxMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["MaxMeasuredValue"] = QJsonValue::fromVariant(*MaxMeasuredValue);

	if (!MeanMeasuredValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeanMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["MeanMeasuredValue"] = QJsonValue::fromVariant(*MeanMeasuredValue);

	if (!MeasurementType){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Length").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["Length"] = QJsonValue::fromVariant(*Length);

	if (!ErrorType){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["Geometry"] = geometryJsonObject;

	jsonObject["__typename"] = "Result";

	return true;
}


bool V1_0::complextest::CResult::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("AreaResult") || ! jsonObject["AreaResult"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaResult").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", areaResultStringValue);)

		return false;
	}

	if (!jsonObject.contains("ExpectedMinValue") || ! jsonObject["ExpectedMinValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMinValue").toLocal8Bit().constData();)

		return false;
	}
	ExpectedMinValue = jsonObject["ExpectedMinValue"].toDouble();

	if (!jsonObject.contains("ExpectedMaxValue") || ! jsonObject["ExpectedMaxValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ExpectedMaxValue").toLocal8Bit().constData();)

		return false;
	}
	ExpectedMaxValue = jsonObject["ExpectedMaxValue"].toDouble();

	if (!jsonObject.contains("MeasuredValue") || ! jsonObject["MeasuredValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MeasuredValue = jsonObject["MeasuredValue"].toDouble();

	if (!jsonObject.contains("MinMeasuredValue") || ! jsonObject["MinMeasuredValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MinMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MinMeasuredValue = jsonObject["MinMeasuredValue"].toDouble();

	if (!jsonObject.contains("MaxMeasuredValue") || ! jsonObject["MaxMeasuredValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MaxMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MaxMeasuredValue = jsonObject["MaxMeasuredValue"].toDouble();

	if (!jsonObject.contains("MeanMeasuredValue") || ! jsonObject["MeanMeasuredValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeanMeasuredValue").toLocal8Bit().constData();)

		return false;
	}
	MeanMeasuredValue = jsonObject["MeanMeasuredValue"].toDouble();

	if (!jsonObject.contains("MeasurementType") || ! jsonObject["MeasurementType"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

		return false;
	}

	if (!jsonObject.contains("MeasurementUnit") || ! jsonObject["MeasurementUnit"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit", measurementUnitStringValue);)

		return false;
	}

	if (!jsonObject.contains("Length") || ! jsonObject["Length"].isDouble()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Length").toLocal8Bit().constData();)

		return false;
	}
	Length = jsonObject["Length"].toDouble();

	if (!jsonObject.contains("ErrorType") || ! jsonObject["ErrorType"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	if (!jsonObject.contains("Geometry") || ! jsonObject["Geometry"].isObject()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}
	Geometry = V1_0::complextest::CGeometry();
	const bool isGeometryRead = Geometry->ReadFromJsonObject(jsonObject["Geometry"].toObject());
	if (!isGeometryRead){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

		return false;
	}

	return true;
}


bool V1_0::complextest::CResult::OptReadFromJsonObject(const QJsonObject& jsonObject)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", areaResultStringValue);)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementType", measurementTypeStringValue);)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "MeasurementUnit", measurementUnitStringValue);)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("Geometry") && jsonObject["Geometry"].isObject()){
		Geometry = V1_0::complextest::CGeometry();
		const bool isGeometryRead = Geometry->OptReadFromJsonObject(jsonObject["Geometry"].toObject());
		if (!isGeometryRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "Geometry").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}




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


bool V1_0::complextest::CArea::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Area", modelIndex);

	if (!AreaName){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaName").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("AreaName", *AreaName, modelIndex);

	if (!Status){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}
	::imtbase::CTreeItemModel* iconPositionNewModelPtr = model.AddTreeModel("IconPosition", modelIndex);
	const bool isIconPositionAdded = IconPosition->WriteToModel(*iconPositionNewModelPtr, 0);
	if (!isIconPositionAdded){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}

	if (!ErrorType){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

		return false;
	}
	::imtbase::CTreeItemModel* newResultsModelPtr = model.AddTreeModel("Results", modelIndex);
	newResultsModelPtr->setIsArray(true);
	for (qsizetype resultsIndex = 0; resultsIndex < Results->size(); ++resultsIndex){
		if (!Results->at(resultsIndex)){
			newResultsModelPtr->InsertNewItem();
			newResultsModelPtr->SetData(QByteArray(), QVariant(), resultsIndex);
			continue;
		}
		newResultsModelPtr->InsertNewItem();
		if (!(Results->at(resultsIndex)->WriteToModel(*newResultsModelPtr, resultsIndex))){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool V1_0::complextest::CArea::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant areaNameData = model.GetData("AreaName", modelIndex);
	if (areaNameData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "AreaName").toLocal8Bit().constData();)

		return false;
	}
	AreaName = areaNameData.toString();

	QVariant statusData = model.GetData("Status", modelIndex);
	if (statusData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* iconPositionDataModelPtr = model.GetTreeItemModel("IconPosition", modelIndex);
	if (iconPositionDataModelPtr == nullptr){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' is missing, but required").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}
	IconPosition = V1_0::complextest::CPoint();
	const bool isIconPositionRead = IconPosition->ReadFromModel(*iconPositionDataModelPtr, modelIndex);
	if (!isIconPositionRead){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}


	QVariant errorTypeData = model.GetData("ErrorType", modelIndex);
	if (errorTypeData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* resultsModel = model.GetTreeItemModel("Results", modelIndex);
	if (resultsModel == nullptr){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' is missing, but required.").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

		return false;
	}
	int resultsCount = resultsModel->GetItemsCount();
	imtsdl::TElementList<V1_0::complextest::CResult> resultsList;
	for (int resultsIndex = 0; resultsIndex < resultsCount; ++resultsIndex){
		if (resultsModel->ContainsKey(QByteArray(), resultsIndex) && !resultsModel->GetData(QByteArray(), resultsIndex).isValid()){
			resultsList.AppendNull();
			continue;
		}
		V1_0::complextest::CResult t_results;
		if (!t_results.ReadFromModel(*resultsModel, resultsIndex)){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

			return false;
		}
		resultsList << t_results;
	}
	Results = resultsList;


	return true;
}


bool V1_0::complextest::CArea::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* iconPositionDataModelPtr = model.GetTreeItemModel("IconPosition", modelIndex);
	if (iconPositionDataModelPtr != nullptr){
		IconPosition = V1_0::complextest::CPoint();
		const bool isIconPositionRead = IconPosition->ReadFromModel(*iconPositionDataModelPtr, modelIndex);
		if (!isIconPositionRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultsModel = model.GetTreeItemModel("Results", modelIndex);
	if (resultsModel != nullptr){
		int resultsCount = resultsModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::CResult> resultsList;
		for (int resultsIndex = 0; resultsIndex < resultsCount; ++resultsIndex){
			if (resultsModel->ContainsKey(QByteArray(), resultsIndex) && !resultsModel->GetData(QByteArray(), resultsIndex).isValid()){
				resultsList.AppendNull();
				continue;
			}
			V1_0::complextest::CResult t_results;
			if (!t_results.OptReadFromModel(*resultsModel, resultsIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

				return false;
			}
			resultsList << t_results;
		}
		Results = resultsList;

	}

	return true;
}


bool V1_0::complextest::CArea::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!AreaName){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaName").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("AreaName", QVariant(*AreaName));

	if (!Status){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("IconPosition", iconPositionGqlObject);

	if (!ErrorType){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

		return false;
	}
	QList<::imtgql::CGqlParamObject> resultsDataObjectList;
	for (qsizetype resultsIndex = 0; resultsIndex < Results->size(); ++resultsIndex){
		if (!Results->at(resultsIndex)){
			resultsDataObjectList << ::imtgql::CGqlParamObject::CreateNull();
			continue;
		}
		::imtgql::CGqlParamObject newResultsGqlObject;
		if (!Results->at(resultsIndex)->WriteToGraphQlObject(newResultsGqlObject)){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

			return false;
		}
		resultsDataObjectList << newResultsGqlObject;
	}
	gqlObject.InsertParam("Results", resultsDataObjectList);

	gqlObject.InsertParam("__typename", QVariant("Area"));

	return true;
}


bool V1_0::complextest::CArea::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("AreaName") || (gqlObject["AreaName"].userType() != QMetaType::QString && gqlObject["AreaName"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaName").toLocal8Bit().constData();)

		return false;
	}
	AreaName = gqlObject["AreaName"].toString();

	if (!gqlObject.ContainsParam("Status") || (gqlObject["Status"].userType() != QMetaType::QString && gqlObject["Status"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("IconPosition") || (gqlObject.GetParamArgumentObjectPtr("IconPosition") == nullptr)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}
	IconPosition = V1_0::complextest::CPoint();
	const bool isIconPositionRead = IconPosition->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("IconPosition"));
	if (!isIconPositionRead){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}

	if (!gqlObject.ContainsParam("ErrorType") || (gqlObject["ErrorType"].userType() != QMetaType::QString && gqlObject["ErrorType"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("Results") || (gqlObject["Results"].isNull())){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

		return false;
	}
	const qsizetype resultsElementsCount = gqlObject.GetObjectsCount("Results");
	Results = imtsdl::TElementList<V1_0::complextest::CResult>();
	for (qsizetype resultsIndex = 0; resultsIndex < resultsElementsCount; ++resultsIndex){
		const ::imtgql::CGqlParamObject* resultsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Results", resultsIndex);
		if (resultsDataObjectPtr == nullptr || resultsDataObjectPtr->IsNull()){
			Results->AppendNull();
			continue;
		}
		V1_0::complextest::CResult tempResults;
		if (!tempResults.ReadFromGraphQlObject(*resultsDataObjectPtr)){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

			return false;
		}
		Results->append(tempResults);
	}

	return true;
}


bool V1_0::complextest::CArea::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("IconPosition") && (gqlObject.GetParamArgumentObjectPtr("IconPosition") != nullptr)){
		IconPosition = V1_0::complextest::CPoint();
		const bool isIconPositionRead = IconPosition->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("IconPosition"));
		if (!isIconPositionRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("Results") && !(gqlObject["Results"].isNull())){
		const qsizetype resultsElementsCount = gqlObject.GetObjectsCount("Results");
		Results = imtsdl::TElementList<V1_0::complextest::CResult>();
		for (qsizetype resultsIndex = 0; resultsIndex < resultsElementsCount; ++resultsIndex){
			const ::imtgql::CGqlParamObject* resultsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Results", resultsIndex);
			if (resultsDataObjectPtr == nullptr || resultsDataObjectPtr->IsNull()){
				Results->AppendNull();
				continue;
			}
			V1_0::complextest::CResult tempResults;
			if (!tempResults.OptReadFromGraphQlObject(*resultsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

				return false;
			}
			Results->append(tempResults);
		}
	}

	return true;
}


bool V1_0::complextest::CArea::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!AreaName){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaName").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["AreaName"] = QJsonValue::fromVariant(*AreaName);

	if (!Status){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["IconPosition"] = iconPositionJsonObject;

	if (!ErrorType){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

		return false;
	}
	QJsonArray newResultsArray;
	for (qsizetype resultsIndex = 0; resultsIndex < Results->size(); ++resultsIndex){
		if (!Results->at(resultsIndex)){
			newResultsArray << QJsonValue(QJsonValue::Null);
			continue;
		}
		QJsonObject newResultsJsonObject;
		if (!Results->at(resultsIndex)->WriteToJsonObject(newResultsJsonObject)){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

			return false;
		}
		newResultsArray << newResultsJsonObject;
	}
	jsonObject["Results"] = newResultsArray;

	jsonObject["__typename"] = "Area";

	return true;
}


bool V1_0::complextest::CArea::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("AreaName") || ! jsonObject["AreaName"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AreaName").toLocal8Bit().constData();)

		return false;
	}
	AreaName = jsonObject["AreaName"].toString();

	if (!jsonObject.contains("Status") || ! jsonObject["Status"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (!jsonObject.contains("IconPosition") || ! jsonObject["IconPosition"].isObject()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}
	IconPosition = V1_0::complextest::CPoint();
	const bool isIconPositionRead = IconPosition->ReadFromJsonObject(jsonObject["IconPosition"].toObject());
	if (!isIconPositionRead){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

		return false;
	}

	if (!jsonObject.contains("ErrorType") || ! jsonObject["ErrorType"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ErrorType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

		return false;
	}

	if (!jsonObject.contains("Results") || ! jsonObject["Results"].isArray()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

		return false;
	}
	const QJsonArray resultsJsonArray = jsonObject["Results"].toArray();
	const qsizetype resultsArrayCount = resultsJsonArray.size();
	Results = imtsdl::TElementList<V1_0::complextest::CResult>();
	for (qsizetype resultsIndex = 0; resultsIndex < resultsArrayCount; ++resultsIndex){
		if (resultsJsonArray[resultsIndex].isNull()){
			Results->AppendNull();
			continue;
		}
		V1_0::complextest::CResult tempResults;
		if (!tempResults.ReadFromJsonObject(resultsJsonArray[resultsIndex].toObject())){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

			return false;
		}
		Results->append(tempResults);
	}

	return true;
}


bool V1_0::complextest::CArea::OptReadFromJsonObject(const QJsonObject& jsonObject)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("IconPosition") && jsonObject["IconPosition"].isObject()){
		IconPosition = V1_0::complextest::CPoint();
		const bool isIconPositionRead = IconPosition->OptReadFromJsonObject(jsonObject["IconPosition"].toObject());
		if (!isIconPositionRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "IconPosition").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "ErrorCode", errorTypeStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("Results") && jsonObject["Results"].isArray()){
		const QJsonArray resultsJsonArray = jsonObject["Results"].toArray();
		const qsizetype resultsArrayCount = resultsJsonArray.size();
		Results = imtsdl::TElementList<V1_0::complextest::CResult>();
		for (qsizetype resultsIndex = 0; resultsIndex < resultsArrayCount; ++resultsIndex){
			if (resultsJsonArray[resultsIndex].isNull()){
				Results->AppendNull();
				continue;
			}
			V1_0::complextest::CResult tempResults;
			if (!tempResults.OptReadFromJsonObject(resultsJsonArray[resultsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Results").toLocal8Bit().constData();)

				return false;
			}
			Results->append(tempResults);
		}
	}

	return true;
}




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


bool V1_0::complextest::CAnalyzer::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Analyzer", modelIndex);

	if (!AnalyzerName){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerName").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("AnalyzerName", *AnalyzerName, modelIndex);

	if (!AnalyzerResult){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerResult").toLocal8Bit().constData();)

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
			if (!Areas->at(areasIndex)){
				newAreasModelPtr->InsertNewItem();
				newAreasModelPtr->SetData(QByteArray(), QVariant(), areasIndex);
				continue;
			}
			newAreasModelPtr->InsertNewItem();
			if (!(Areas->at(areasIndex)->WriteToModel(*newAreasModelPtr, areasIndex))){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
		}
	}
	else if (Areas.IsNull()){
		model.SetData("Areas", QVariant(), modelIndex);
	}

	return true;
}


bool V1_0::complextest::CAnalyzer::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant analyzerNameData = model.GetData("AnalyzerName", modelIndex);
	if (analyzerNameData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerName").toLocal8Bit().constData();)

		return false;
	}
	AnalyzerName = analyzerNameData.toString();

	QVariant analyzerResultData = model.GetData("AnalyzerResult", modelIndex);
	if (analyzerResultData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerResult").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* areasModel = model.GetTreeItemModel("Areas", modelIndex);
	if (model.ContainsKey("Areas", modelIndex) && areasModel == nullptr){
		Areas.SetNull();
	}
	if (areasModel != nullptr){
		int areasCount = areasModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::CArea> areasList;
		for (int areasIndex = 0; areasIndex < areasCount; ++areasIndex){
			if (areasModel->ContainsKey(QByteArray(), areasIndex) && !areasModel->GetData(QByteArray(), areasIndex).isValid()){
				areasList.AppendNull();
				continue;
			}
			V1_0::complextest::CArea t_areas;
			if (!t_areas.ReadFromModel(*areasModel, areasIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
			areasList << t_areas;
		}
		Areas = areasList;

	}

	return true;
}


bool V1_0::complextest::CAnalyzer::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* areasModel = model.GetTreeItemModel("Areas", modelIndex);
	if (model.ContainsKey("Areas", modelIndex) && areasModel == nullptr){
		Areas.SetNull();
	}
	if (areasModel != nullptr){
		int areasCount = areasModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::CArea> areasList;
		for (int areasIndex = 0; areasIndex < areasCount; ++areasIndex){
			if (areasModel->ContainsKey(QByteArray(), areasIndex) && !areasModel->GetData(QByteArray(), areasIndex).isValid()){
				areasList.AppendNull();
				continue;
			}
			V1_0::complextest::CArea t_areas;
			if (!t_areas.OptReadFromModel(*areasModel, areasIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
			areasList << t_areas;
		}
		Areas = areasList;

	}

	return true;
}


bool V1_0::complextest::CAnalyzer::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!AnalyzerName){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerName").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("AnalyzerName", QVariant(*AnalyzerName));

	if (!AnalyzerResult){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerResult").toLocal8Bit().constData();)

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
			if (!Areas->at(areasIndex)){
				areasDataObjectList << ::imtgql::CGqlParamObject::CreateNull();
				continue;
			}
			::imtgql::CGqlParamObject newAreasGqlObject;
			if (!Areas->at(areasIndex)->WriteToGraphQlObject(newAreasGqlObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
			areasDataObjectList << newAreasGqlObject;
		}
		gqlObject.InsertParam("Areas", areasDataObjectList);
	}
	else if (Areas.IsNull()){
		gqlObject.InsertParam("Areas", QVariant());
	}

	gqlObject.InsertParam("__typename", QVariant("Analyzer"));

	return true;
}


bool V1_0::complextest::CAnalyzer::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("AnalyzerName") || (gqlObject["AnalyzerName"].userType() != QMetaType::QString && gqlObject["AnalyzerName"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerName").toLocal8Bit().constData();)

		return false;
	}
	AnalyzerName = gqlObject["AnalyzerName"].toString();

	if (!gqlObject.ContainsParam("AnalyzerResult") || (gqlObject["AnalyzerResult"].userType() != QMetaType::QString && gqlObject["AnalyzerResult"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerResult").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

		return false;
	}

	if (gqlObject.ContainsParam("Areas") && gqlObject["Areas"].isNull()){
		Areas.SetNull();
	}
	if (gqlObject.ContainsParam("Areas") && !(gqlObject["Areas"].isNull())){
		const qsizetype areasElementsCount = gqlObject.GetObjectsCount("Areas");
		Areas = imtsdl::TElementList<V1_0::complextest::CArea>();
		for (qsizetype areasIndex = 0; areasIndex < areasElementsCount; ++areasIndex){
			const ::imtgql::CGqlParamObject* areasDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Areas", areasIndex);
			if (areasDataObjectPtr == nullptr || areasDataObjectPtr->IsNull()){
				Areas->AppendNull();
				continue;
			}
			V1_0::complextest::CArea tempAreas;
			if (!tempAreas.ReadFromGraphQlObject(*areasDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
			Areas->append(tempAreas);
		}
	}

	return true;
}


bool V1_0::complextest::CAnalyzer::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("Areas") && gqlObject["Areas"].isNull()){
		Areas.SetNull();
	}
	if (gqlObject.ContainsParam("Areas") && !(gqlObject["Areas"].isNull())){
		const qsizetype areasElementsCount = gqlObject.GetObjectsCount("Areas");
		Areas = imtsdl::TElementList<V1_0::complextest::CArea>();
		for (qsizetype areasIndex = 0; areasIndex < areasElementsCount; ++areasIndex){
			const ::imtgql::CGqlParamObject* areasDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Areas", areasIndex);
			if (areasDataObjectPtr == nullptr || areasDataObjectPtr->IsNull()){
				Areas->AppendNull();
				continue;
			}
			V1_0::complextest::CArea tempAreas;
			if (!tempAreas.OptReadFromGraphQlObject(*areasDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
			Areas->append(tempAreas);
		}
	}

	return true;
}


bool V1_0::complextest::CAnalyzer::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!AnalyzerName){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerName").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["AnalyzerName"] = QJsonValue::fromVariant(*AnalyzerName);

	if (!AnalyzerResult){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerResult").toLocal8Bit().constData();)

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
			if (!Areas->at(areasIndex)){
				newAreasArray << QJsonValue(QJsonValue::Null);
				continue;
			}
			QJsonObject newAreasJsonObject;
			if (!Areas->at(areasIndex)->WriteToJsonObject(newAreasJsonObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
			newAreasArray << newAreasJsonObject;
		}
		jsonObject["Areas"] = newAreasArray;
	}
	else if (Areas.IsNull()){
		jsonObject["Areas"] = QJsonValue(QJsonValue::Null);
	}

	jsonObject["__typename"] = "Analyzer";

	return true;
}


bool V1_0::complextest::CAnalyzer::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("AnalyzerName") || ! jsonObject["AnalyzerName"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerName").toLocal8Bit().constData();)

		return false;
	}
	AnalyzerName = jsonObject["AnalyzerName"].toString();

	if (!jsonObject.contains("AnalyzerResult") || ! jsonObject["AnalyzerResult"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "AnalyzerResult").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

		return false;
	}

if (jsonObject.contains("Areas") && jsonObject["Areas"].isNull()){
		Areas.SetNull();
	}
		if (jsonObject.contains("Areas") && jsonObject["Areas"].isArray()){
		const QJsonArray areasJsonArray = jsonObject["Areas"].toArray();
		const qsizetype areasArrayCount = areasJsonArray.size();
		Areas = imtsdl::TElementList<V1_0::complextest::CArea>();
		for (qsizetype areasIndex = 0; areasIndex < areasArrayCount; ++areasIndex){
			if (areasJsonArray[areasIndex].isNull()){
				Areas->AppendNull();
				continue;
			}
			V1_0::complextest::CArea tempAreas;
			if (!tempAreas.ReadFromJsonObject(areasJsonArray[areasIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
			Areas->append(tempAreas);
		}
	}

	return true;
}


bool V1_0::complextest::CAnalyzer::OptReadFromJsonObject(const QJsonObject& jsonObject)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", analyzerResultStringValue);)

			return false;
		}
	}

if (jsonObject.contains("Areas") && jsonObject["Areas"].isNull()){
		Areas.SetNull();
	}
		if (jsonObject.contains("Areas") && jsonObject["Areas"].isArray()){
		const QJsonArray areasJsonArray = jsonObject["Areas"].toArray();
		const qsizetype areasArrayCount = areasJsonArray.size();
		Areas = imtsdl::TElementList<V1_0::complextest::CArea>();
		for (qsizetype areasIndex = 0; areasIndex < areasArrayCount; ++areasIndex){
			if (areasJsonArray[areasIndex].isNull()){
				Areas->AppendNull();
				continue;
			}
			V1_0::complextest::CArea tempAreas;
			if (!tempAreas.OptReadFromJsonObject(areasJsonArray[areasIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Areas").toLocal8Bit().constData();)

				return false;
			}
			Areas->append(tempAreas);
		}
	}

	return true;
}




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


bool V1_0::complextest::CInspection::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Inspection", modelIndex);

	if (!ID){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ID").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("ID", *ID, modelIndex);

	if (!TypeID){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "TypeID").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("TypeID", *TypeID, modelIndex);

	if (!Name){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("Name", *Name, modelIndex);

	if (!Status){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
			if (!Analyzers->at(analyzersIndex)){
				newAnalyzersModelPtr->InsertNewItem();
				newAnalyzersModelPtr->SetData(QByteArray(), QVariant(), analyzersIndex);
				continue;
			}
			newAnalyzersModelPtr->InsertNewItem();
			if (!(Analyzers->at(analyzersIndex)->WriteToModel(*newAnalyzersModelPtr, analyzersIndex))){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
		}
	}
	else if (Analyzers.IsNull()){
		model.SetData("Analyzers", QVariant(), modelIndex);
	}

	return true;
}


bool V1_0::complextest::CInspection::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant iDData = model.GetData("ID", modelIndex);
	if (iDData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ID").toLocal8Bit().constData();)

		return false;
	}
	ID = iDData.toString();

	QVariant typeIDData = model.GetData("TypeID", modelIndex);
	if (typeIDData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "TypeID").toLocal8Bit().constData();)

		return false;
	}
	TypeID = typeIDData.toString();

	QVariant nameData = model.GetData("Name", modelIndex);
	if (nameData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	Name = nameData.toString();

	QVariant statusData = model.GetData("Status", modelIndex);
	if (statusData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	::imtbase::CTreeItemModel* analyzersModel = model.GetTreeItemModel("Analyzers", modelIndex);
	if (model.ContainsKey("Analyzers", modelIndex) && analyzersModel == nullptr){
		Analyzers.SetNull();
	}
	if (analyzersModel != nullptr){
		int analyzersCount = analyzersModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::CAnalyzer> analyzersList;
		for (int analyzersIndex = 0; analyzersIndex < analyzersCount; ++analyzersIndex){
			if (analyzersModel->ContainsKey(QByteArray(), analyzersIndex) && !analyzersModel->GetData(QByteArray(), analyzersIndex).isValid()){
				analyzersList.AppendNull();
				continue;
			}
			V1_0::complextest::CAnalyzer t_analyzers;
			if (!t_analyzers.ReadFromModel(*analyzersModel, analyzersIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
			analyzersList << t_analyzers;
		}
		Analyzers = analyzersList;

	}

	return true;
}


bool V1_0::complextest::CInspection::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	::imtbase::CTreeItemModel* analyzersModel = model.GetTreeItemModel("Analyzers", modelIndex);
	if (model.ContainsKey("Analyzers", modelIndex) && analyzersModel == nullptr){
		Analyzers.SetNull();
	}
	if (analyzersModel != nullptr){
		int analyzersCount = analyzersModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::CAnalyzer> analyzersList;
		for (int analyzersIndex = 0; analyzersIndex < analyzersCount; ++analyzersIndex){
			if (analyzersModel->ContainsKey(QByteArray(), analyzersIndex) && !analyzersModel->GetData(QByteArray(), analyzersIndex).isValid()){
				analyzersList.AppendNull();
				continue;
			}
			V1_0::complextest::CAnalyzer t_analyzers;
			if (!t_analyzers.OptReadFromModel(*analyzersModel, analyzersIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
			analyzersList << t_analyzers;
		}
		Analyzers = analyzersList;

	}

	return true;
}


bool V1_0::complextest::CInspection::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!ID){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ID").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("ID", QVariant(*ID));

	if (!TypeID){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "TypeID").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("TypeID", QVariant(*TypeID));

	if (!Name){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("Name", QVariant(*Name));

	if (!Status){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
			if (!Analyzers->at(analyzersIndex)){
				analyzersDataObjectList << ::imtgql::CGqlParamObject::CreateNull();
				continue;
			}
			::imtgql::CGqlParamObject newAnalyzersGqlObject;
			if (!Analyzers->at(analyzersIndex)->WriteToGraphQlObject(newAnalyzersGqlObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
			analyzersDataObjectList << newAnalyzersGqlObject;
		}
		gqlObject.InsertParam("Analyzers", analyzersDataObjectList);
	}
	else if (Analyzers.IsNull()){
		gqlObject.InsertParam("Analyzers", QVariant());
	}

	gqlObject.InsertParam("__typename", QVariant("Inspection"));

	return true;
}


bool V1_0::complextest::CInspection::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("ID") || (gqlObject["ID"].userType() != QMetaType::QString && gqlObject["ID"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ID").toLocal8Bit().constData();)

		return false;
	}
	ID = gqlObject["ID"].toString();

	if (!gqlObject.ContainsParam("TypeID") || (gqlObject["TypeID"].userType() != QMetaType::QString && gqlObject["TypeID"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "TypeID").toLocal8Bit().constData();)

		return false;
	}
	TypeID = gqlObject["TypeID"].toString();

	if (!gqlObject.ContainsParam("Name") || (gqlObject["Name"].userType() != QMetaType::QString && gqlObject["Name"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	Name = gqlObject["Name"].toString();

	if (!gqlObject.ContainsParam("Status") || (gqlObject["Status"].userType() != QMetaType::QString && gqlObject["Status"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (gqlObject.ContainsParam("Analyzers") && gqlObject["Analyzers"].isNull()){
		Analyzers.SetNull();
	}
	if (gqlObject.ContainsParam("Analyzers") && !(gqlObject["Analyzers"].isNull())){
		const qsizetype analyzersElementsCount = gqlObject.GetObjectsCount("Analyzers");
		Analyzers = imtsdl::TElementList<V1_0::complextest::CAnalyzer>();
		for (qsizetype analyzersIndex = 0; analyzersIndex < analyzersElementsCount; ++analyzersIndex){
			const ::imtgql::CGqlParamObject* analyzersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Analyzers", analyzersIndex);
			if (analyzersDataObjectPtr == nullptr || analyzersDataObjectPtr->IsNull()){
				Analyzers->AppendNull();
				continue;
			}
			V1_0::complextest::CAnalyzer tempAnalyzers;
			if (!tempAnalyzers.ReadFromGraphQlObject(*analyzersDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
			Analyzers->append(tempAnalyzers);
		}
	}

	return true;
}


bool V1_0::complextest::CInspection::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("Analyzers") && gqlObject["Analyzers"].isNull()){
		Analyzers.SetNull();
	}
	if (gqlObject.ContainsParam("Analyzers") && !(gqlObject["Analyzers"].isNull())){
		const qsizetype analyzersElementsCount = gqlObject.GetObjectsCount("Analyzers");
		Analyzers = imtsdl::TElementList<V1_0::complextest::CAnalyzer>();
		for (qsizetype analyzersIndex = 0; analyzersIndex < analyzersElementsCount; ++analyzersIndex){
			const ::imtgql::CGqlParamObject* analyzersDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Analyzers", analyzersIndex);
			if (analyzersDataObjectPtr == nullptr || analyzersDataObjectPtr->IsNull()){
				Analyzers->AppendNull();
				continue;
			}
			V1_0::complextest::CAnalyzer tempAnalyzers;
			if (!tempAnalyzers.OptReadFromGraphQlObject(*analyzersDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
			Analyzers->append(tempAnalyzers);
		}
	}

	return true;
}


bool V1_0::complextest::CInspection::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!ID){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ID").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["ID"] = QJsonValue::fromVariant(*ID);

	if (!TypeID){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "TypeID").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["TypeID"] = QJsonValue::fromVariant(*TypeID);

	if (!Name){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["Name"] = QJsonValue::fromVariant(*Name);

	if (!Status){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
			if (!Analyzers->at(analyzersIndex)){
				newAnalyzersArray << QJsonValue(QJsonValue::Null);
				continue;
			}
			QJsonObject newAnalyzersJsonObject;
			if (!Analyzers->at(analyzersIndex)->WriteToJsonObject(newAnalyzersJsonObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
			newAnalyzersArray << newAnalyzersJsonObject;
		}
		jsonObject["Analyzers"] = newAnalyzersArray;
	}
	else if (Analyzers.IsNull()){
		jsonObject["Analyzers"] = QJsonValue(QJsonValue::Null);
	}

	jsonObject["__typename"] = "Inspection";

	return true;
}


bool V1_0::complextest::CInspection::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("ID") || ! jsonObject["ID"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ID").toLocal8Bit().constData();)

		return false;
	}
	ID = jsonObject["ID"].toString();

	if (!jsonObject.contains("TypeID") || ! jsonObject["TypeID"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "TypeID").toLocal8Bit().constData();)

		return false;
	}
	TypeID = jsonObject["TypeID"].toString();

	if (!jsonObject.contains("Name") || ! jsonObject["Name"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	Name = jsonObject["Name"].toString();

	if (!jsonObject.contains("Status") || ! jsonObject["Status"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

if (jsonObject.contains("Analyzers") && jsonObject["Analyzers"].isNull()){
		Analyzers.SetNull();
	}
		if (jsonObject.contains("Analyzers") && jsonObject["Analyzers"].isArray()){
		const QJsonArray analyzersJsonArray = jsonObject["Analyzers"].toArray();
		const qsizetype analyzersArrayCount = analyzersJsonArray.size();
		Analyzers = imtsdl::TElementList<V1_0::complextest::CAnalyzer>();
		for (qsizetype analyzersIndex = 0; analyzersIndex < analyzersArrayCount; ++analyzersIndex){
			if (analyzersJsonArray[analyzersIndex].isNull()){
				Analyzers->AppendNull();
				continue;
			}
			V1_0::complextest::CAnalyzer tempAnalyzers;
			if (!tempAnalyzers.ReadFromJsonObject(analyzersJsonArray[analyzersIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
			Analyzers->append(tempAnalyzers);
		}
	}

	return true;
}


bool V1_0::complextest::CInspection::OptReadFromJsonObject(const QJsonObject& jsonObject)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

			return false;
		}
	}

if (jsonObject.contains("Analyzers") && jsonObject["Analyzers"].isNull()){
		Analyzers.SetNull();
	}
		if (jsonObject.contains("Analyzers") && jsonObject["Analyzers"].isArray()){
		const QJsonArray analyzersJsonArray = jsonObject["Analyzers"].toArray();
		const qsizetype analyzersArrayCount = analyzersJsonArray.size();
		Analyzers = imtsdl::TElementList<V1_0::complextest::CAnalyzer>();
		for (qsizetype analyzersIndex = 0; analyzersIndex < analyzersArrayCount; ++analyzersIndex){
			if (analyzersJsonArray[analyzersIndex].isNull()){
				Analyzers->AppendNull();
				continue;
			}
			V1_0::complextest::CAnalyzer tempAnalyzers;
			if (!tempAnalyzers.OptReadFromJsonObject(analyzersJsonArray[analyzersIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Analyzers").toLocal8Bit().constData();)

				return false;
			}
			Analyzers->append(tempAnalyzers);
		}
	}

	return true;
}




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


bool V1_0::complextest::CProductOverview::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "ProductOverview", modelIndex);

	if (!ProductID){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductID").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("ProductID", *ProductID, modelIndex);

	if (!Name){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("Name", *Name, modelIndex);

	if (!Status){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "PartSerialNumber").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("PartSerialNumber", *PartSerialNumber, modelIndex);

	if (!Timestamp){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Timestamp").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("Timestamp", *Timestamp, modelIndex);

	if (!ProductImage){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductImage").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("ProductImage", *ProductImage, modelIndex);

	if (Inspections){
		::imtbase::CTreeItemModel* newInspectionsModelPtr = model.AddTreeModel("Inspections", modelIndex);
		newInspectionsModelPtr->setIsArray(true);
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < Inspections->size(); ++inspectionsIndex){
			if (!Inspections->at(inspectionsIndex)){
				newInspectionsModelPtr->InsertNewItem();
				newInspectionsModelPtr->SetData(QByteArray(), QVariant(), inspectionsIndex);
				continue;
			}
			newInspectionsModelPtr->InsertNewItem();
			if (!(Inspections->at(inspectionsIndex)->WriteToModel(*newInspectionsModelPtr, inspectionsIndex))){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
		}
	}
	else if (Inspections.IsNull()){
		model.SetData("Inspections", QVariant(), modelIndex);
	}

	return true;
}


bool V1_0::complextest::CProductOverview::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant productIDData = model.GetData("ProductID", modelIndex);
	if (productIDData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ProductID").toLocal8Bit().constData();)

		return false;
	}
	ProductID = productIDData.toInt();

	QVariant nameData = model.GetData("Name", modelIndex);
	if (nameData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	Name = nameData.toString();

	QVariant statusData = model.GetData("Status", modelIndex);
	if (statusData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	QVariant partSerialNumberData = model.GetData("PartSerialNumber", modelIndex);
	if (partSerialNumberData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "PartSerialNumber").toLocal8Bit().constData();)

		return false;
	}
	PartSerialNumber = partSerialNumberData.toString();

	QVariant timestampData = model.GetData("Timestamp", modelIndex);
	if (timestampData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Timestamp").toLocal8Bit().constData();)

		return false;
	}
	Timestamp = timestampData.toString();

	QVariant productImageData = model.GetData("ProductImage", modelIndex);
	if (productImageData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "ProductImage").toLocal8Bit().constData();)

		return false;
	}
	ProductImage = productImageData.toString();

	::imtbase::CTreeItemModel* inspectionsModel = model.GetTreeItemModel("Inspections", modelIndex);
	if (model.ContainsKey("Inspections", modelIndex) && inspectionsModel == nullptr){
		Inspections.SetNull();
	}
	if (inspectionsModel != nullptr){
		int inspectionsCount = inspectionsModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::CInspection> inspectionsList;
		for (int inspectionsIndex = 0; inspectionsIndex < inspectionsCount; ++inspectionsIndex){
			if (inspectionsModel->ContainsKey(QByteArray(), inspectionsIndex) && !inspectionsModel->GetData(QByteArray(), inspectionsIndex).isValid()){
				inspectionsList.AppendNull();
				continue;
			}
			V1_0::complextest::CInspection t_inspections;
			if (!t_inspections.ReadFromModel(*inspectionsModel, inspectionsIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
			inspectionsList << t_inspections;
		}
		Inspections = inspectionsList;

	}

	return true;
}


bool V1_0::complextest::CProductOverview::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

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
	if (model.ContainsKey("Inspections", modelIndex) && inspectionsModel == nullptr){
		Inspections.SetNull();
	}
	if (inspectionsModel != nullptr){
		int inspectionsCount = inspectionsModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::CInspection> inspectionsList;
		for (int inspectionsIndex = 0; inspectionsIndex < inspectionsCount; ++inspectionsIndex){
			if (inspectionsModel->ContainsKey(QByteArray(), inspectionsIndex) && !inspectionsModel->GetData(QByteArray(), inspectionsIndex).isValid()){
				inspectionsList.AppendNull();
				continue;
			}
			V1_0::complextest::CInspection t_inspections;
			if (!t_inspections.OptReadFromModel(*inspectionsModel, inspectionsIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
			inspectionsList << t_inspections;
		}
		Inspections = inspectionsList;

	}

	return true;
}


bool V1_0::complextest::CProductOverview::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!ProductID){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductID").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("ProductID", QVariant(*ProductID));

	if (!Name){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("Name", QVariant(*Name));

	if (!Status){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "PartSerialNumber").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("PartSerialNumber", QVariant(*PartSerialNumber));

	if (!Timestamp){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Timestamp").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("Timestamp", QVariant(*Timestamp));

	if (!ProductImage){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductImage").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("ProductImage", QVariant(*ProductImage));

	if (Inspections){
		QList<::imtgql::CGqlParamObject> inspectionsDataObjectList;
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < Inspections->size(); ++inspectionsIndex){
			if (!Inspections->at(inspectionsIndex)){
				inspectionsDataObjectList << ::imtgql::CGqlParamObject::CreateNull();
				continue;
			}
			::imtgql::CGqlParamObject newInspectionsGqlObject;
			if (!Inspections->at(inspectionsIndex)->WriteToGraphQlObject(newInspectionsGqlObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
			inspectionsDataObjectList << newInspectionsGqlObject;
		}
		gqlObject.InsertParam("Inspections", inspectionsDataObjectList);
	}
	else if (Inspections.IsNull()){
		gqlObject.InsertParam("Inspections", QVariant());
	}

	gqlObject.InsertParam("__typename", QVariant("ProductOverview"));

	return true;
}


bool V1_0::complextest::CProductOverview::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("ProductID") || (gqlObject["ProductID"].userType() != QMetaType::Int && gqlObject["ProductID"].userType() != QMetaType::UInt && gqlObject["ProductID"].userType() != QMetaType::LongLong && gqlObject["ProductID"].userType() != QMetaType::ULongLong && gqlObject["ProductID"].userType() != QMetaType::Long && gqlObject["ProductID"].userType() != QMetaType::Short && gqlObject["ProductID"].userType() != QMetaType::ULong && gqlObject["ProductID"].userType() != QMetaType::UShort && gqlObject["ProductID"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductID").toLocal8Bit().constData();)

		return false;
	}
	ProductID = gqlObject["ProductID"].toInt();

	if (!gqlObject.ContainsParam("Name") || (gqlObject["Name"].userType() != QMetaType::QString && gqlObject["Name"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	Name = gqlObject["Name"].toString();

	if (!gqlObject.ContainsParam("Status") || (gqlObject["Status"].userType() != QMetaType::QString && gqlObject["Status"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (!gqlObject.ContainsParam("PartSerialNumber") || (gqlObject["PartSerialNumber"].userType() != QMetaType::QString && gqlObject["PartSerialNumber"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "PartSerialNumber").toLocal8Bit().constData();)

		return false;
	}
	PartSerialNumber = gqlObject["PartSerialNumber"].toString();

	if (!gqlObject.ContainsParam("Timestamp") || (gqlObject["Timestamp"].userType() != QMetaType::QString && gqlObject["Timestamp"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Timestamp").toLocal8Bit().constData();)

		return false;
	}
	Timestamp = gqlObject["Timestamp"].toString();

	if (!gqlObject.ContainsParam("ProductImage") || (gqlObject["ProductImage"].userType() != QMetaType::QString && gqlObject["ProductImage"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductImage").toLocal8Bit().constData();)

		return false;
	}
	ProductImage = gqlObject["ProductImage"].toString();

	if (gqlObject.ContainsParam("Inspections") && gqlObject["Inspections"].isNull()){
		Inspections.SetNull();
	}
	if (gqlObject.ContainsParam("Inspections") && !(gqlObject["Inspections"].isNull())){
		const qsizetype inspectionsElementsCount = gqlObject.GetObjectsCount("Inspections");
		Inspections = imtsdl::TElementList<V1_0::complextest::CInspection>();
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < inspectionsElementsCount; ++inspectionsIndex){
			const ::imtgql::CGqlParamObject* inspectionsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Inspections", inspectionsIndex);
			if (inspectionsDataObjectPtr == nullptr || inspectionsDataObjectPtr->IsNull()){
				Inspections->AppendNull();
				continue;
			}
			V1_0::complextest::CInspection tempInspections;
			if (!tempInspections.ReadFromGraphQlObject(*inspectionsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
			Inspections->append(tempInspections);
		}
	}

	return true;
}


bool V1_0::complextest::CProductOverview::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

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

	if (gqlObject.ContainsParam("Inspections") && gqlObject["Inspections"].isNull()){
		Inspections.SetNull();
	}
	if (gqlObject.ContainsParam("Inspections") && !(gqlObject["Inspections"].isNull())){
		const qsizetype inspectionsElementsCount = gqlObject.GetObjectsCount("Inspections");
		Inspections = imtsdl::TElementList<V1_0::complextest::CInspection>();
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < inspectionsElementsCount; ++inspectionsIndex){
			const ::imtgql::CGqlParamObject* inspectionsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Inspections", inspectionsIndex);
			if (inspectionsDataObjectPtr == nullptr || inspectionsDataObjectPtr->IsNull()){
				Inspections->AppendNull();
				continue;
			}
			V1_0::complextest::CInspection tempInspections;
			if (!tempInspections.OptReadFromGraphQlObject(*inspectionsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
			Inspections->append(tempInspections);
		}
	}

	return true;
}


bool V1_0::complextest::CProductOverview::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!ProductID){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductID").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["ProductID"] = QJsonValue::fromVariant(*ProductID);

	if (!Name){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["Name"] = QJsonValue::fromVariant(*Name);

	if (!Status){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "PartSerialNumber").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["PartSerialNumber"] = QJsonValue::fromVariant(*PartSerialNumber);

	if (!Timestamp){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Timestamp").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["Timestamp"] = QJsonValue::fromVariant(*Timestamp);

	if (!ProductImage){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductImage").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["ProductImage"] = QJsonValue::fromVariant(*ProductImage);

	if (Inspections){
		QJsonArray newInspectionsArray;
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < Inspections->size(); ++inspectionsIndex){
			if (!Inspections->at(inspectionsIndex)){
				newInspectionsArray << QJsonValue(QJsonValue::Null);
				continue;
			}
			QJsonObject newInspectionsJsonObject;
			if (!Inspections->at(inspectionsIndex)->WriteToJsonObject(newInspectionsJsonObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
			newInspectionsArray << newInspectionsJsonObject;
		}
		jsonObject["Inspections"] = newInspectionsArray;
	}
	else if (Inspections.IsNull()){
		jsonObject["Inspections"] = QJsonValue(QJsonValue::Null);
	}

	jsonObject["__typename"] = "ProductOverview";

	return true;
}


bool V1_0::complextest::CProductOverview::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("ProductID") || ! jsonObject["ProductID"].isDouble()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductID").toLocal8Bit().constData();)

		return false;
	}
	ProductID = jsonObject["ProductID"].toInt();

	if (!jsonObject.contains("Name") || ! jsonObject["Name"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Name").toLocal8Bit().constData();)

		return false;
	}
	Name = jsonObject["Name"].toString();

	if (!jsonObject.contains("Status") || ! jsonObject["Status"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Status").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

		return false;
	}

	if (!jsonObject.contains("PartSerialNumber") || ! jsonObject["PartSerialNumber"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "PartSerialNumber").toLocal8Bit().constData();)

		return false;
	}
	PartSerialNumber = jsonObject["PartSerialNumber"].toString();

	if (!jsonObject.contains("Timestamp") || ! jsonObject["Timestamp"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Timestamp").toLocal8Bit().constData();)

		return false;
	}
	Timestamp = jsonObject["Timestamp"].toString();

	if (!jsonObject.contains("ProductImage") || ! jsonObject["ProductImage"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "ProductImage").toLocal8Bit().constData();)

		return false;
	}
	ProductImage = jsonObject["ProductImage"].toString();

if (jsonObject.contains("Inspections") && jsonObject["Inspections"].isNull()){
		Inspections.SetNull();
	}
		if (jsonObject.contains("Inspections") && jsonObject["Inspections"].isArray()){
		const QJsonArray inspectionsJsonArray = jsonObject["Inspections"].toArray();
		const qsizetype inspectionsArrayCount = inspectionsJsonArray.size();
		Inspections = imtsdl::TElementList<V1_0::complextest::CInspection>();
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < inspectionsArrayCount; ++inspectionsIndex){
			if (inspectionsJsonArray[inspectionsIndex].isNull()){
				Inspections->AppendNull();
				continue;
			}
			V1_0::complextest::CInspection tempInspections;
			if (!tempInspections.ReadFromJsonObject(inspectionsJsonArray[inspectionsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
			Inspections->append(tempInspections);
		}
	}

	return true;
}


bool V1_0::complextest::CProductOverview::OptReadFromJsonObject(const QJsonObject& jsonObject)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "StatusCode", statusStringValue);)

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

if (jsonObject.contains("Inspections") && jsonObject["Inspections"].isNull()){
		Inspections.SetNull();
	}
		if (jsonObject.contains("Inspections") && jsonObject["Inspections"].isArray()){
		const QJsonArray inspectionsJsonArray = jsonObject["Inspections"].toArray();
		const qsizetype inspectionsArrayCount = inspectionsJsonArray.size();
		Inspections = imtsdl::TElementList<V1_0::complextest::CInspection>();
		for (qsizetype inspectionsIndex = 0; inspectionsIndex < inspectionsArrayCount; ++inspectionsIndex){
			if (inspectionsJsonArray[inspectionsIndex].isNull()){
				Inspections->AppendNull();
				continue;
			}
			V1_0::complextest::CInspection tempInspections;
			if (!tempInspections.OptReadFromJsonObject(inspectionsJsonArray[inspectionsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Inspections").toLocal8Bit().constData();)

				return false;
			}
			Inspections->append(tempInspections);
		}
	}

	return true;
}




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


bool V1_0::complextest::CLocalizedText::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "LocalizedText", modelIndex);

	if (!text){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "text").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("text", *text, modelIndex);

	if (!locale){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "locale").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("locale", *locale, modelIndex);


	return true;
}


bool V1_0::complextest::CLocalizedText::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant textData = model.GetData("text", modelIndex);
	if (textData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "text").toLocal8Bit().constData();)

		return false;
	}
	text = textData.toString();

	QVariant localeData = model.GetData("locale", modelIndex);
	if (localeData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "locale").toLocal8Bit().constData();)

		return false;
	}
	locale = localeData.toString();

	return true;
}


bool V1_0::complextest::CLocalizedText::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool V1_0::complextest::CLocalizedText::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!text){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "text").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("text", QVariant(*text));

	if (!locale){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "locale").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("locale", QVariant(*locale));

	gqlObject.InsertParam("__typename", QVariant("LocalizedText"));

	return true;
}


bool V1_0::complextest::CLocalizedText::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("text") || (gqlObject["text"].userType() != QMetaType::QString && gqlObject["text"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "text").toLocal8Bit().constData();)

		return false;
	}
	text = gqlObject["text"].toString();

	if (!gqlObject.ContainsParam("locale") || (gqlObject["locale"].userType() != QMetaType::QString && gqlObject["locale"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "locale").toLocal8Bit().constData();)

		return false;
	}
	locale = gqlObject["locale"].toString();

	return true;
}


bool V1_0::complextest::CLocalizedText::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("text") && (gqlObject["text"].userType() == QMetaType::QString || gqlObject["text"].userType() == QMetaType::QByteArray)){
		text = gqlObject["text"].toString();
	}

	if (gqlObject.ContainsParam("locale") && (gqlObject["locale"].userType() == QMetaType::QString || gqlObject["locale"].userType() == QMetaType::QByteArray)){
		locale = gqlObject["locale"].toString();
	}

	return true;
}


bool V1_0::complextest::CLocalizedText::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!text){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "text").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["text"] = QJsonValue::fromVariant(*text);

	if (!locale){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "locale").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["locale"] = QJsonValue::fromVariant(*locale);

	jsonObject["__typename"] = "LocalizedText";

	return true;
}


bool V1_0::complextest::CLocalizedText::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("text") || ! jsonObject["text"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "text").toLocal8Bit().constData();)

		return false;
	}
	text = jsonObject["text"].toString();

	if (!jsonObject.contains("locale") || ! jsonObject["locale"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "locale").toLocal8Bit().constData();)

		return false;
	}
	locale = jsonObject["locale"].toString();

	return true;
}


bool V1_0::complextest::CLocalizedText::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("text") && jsonObject["text"].isString()){
		text = jsonObject["text"].toString();
	}

	if (jsonObject.contains("locale") && jsonObject["locale"].isString()){
		locale = jsonObject["locale"].toString();
	}

	return true;
}




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


bool V1_0::complextest::CCoordinates::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Coordinates", modelIndex);

	if (!x){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "x").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("x", *x, modelIndex);

	if (!y){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "y").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("y", *y, modelIndex);

	if (!z){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "z").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("z", *z, modelIndex);


	return true;
}


bool V1_0::complextest::CCoordinates::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant xData = model.GetData("x", modelIndex);
	if (xData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "x").toLocal8Bit().constData();)

		return false;
	}
	x = xData.toFloat();

	QVariant yData = model.GetData("y", modelIndex);
	if (yData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "y").toLocal8Bit().constData();)

		return false;
	}
	y = yData.toFloat();

	QVariant zData = model.GetData("z", modelIndex);
	if (zData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "z").toLocal8Bit().constData();)

		return false;
	}
	z = zData.toFloat();

	return true;
}


bool V1_0::complextest::CCoordinates::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool V1_0::complextest::CCoordinates::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!x){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "x").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("x", QVariant(*x));

	if (!y){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "y").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("y", QVariant(*y));

	if (!z){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "z").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("z", QVariant(*z));

	gqlObject.InsertParam("__typename", QVariant("Coordinates"));

	return true;
}


bool V1_0::complextest::CCoordinates::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("x") || (gqlObject["x"].userType() != QMetaType::Float && gqlObject["x"].userType() != QMetaType::Double && gqlObject["x"].userType() != QMetaType::Int && gqlObject["x"].userType() != QMetaType::UInt && gqlObject["x"].userType() != QMetaType::LongLong && gqlObject["x"].userType() != QMetaType::ULongLong && gqlObject["x"].userType() != QMetaType::Long && gqlObject["x"].userType() != QMetaType::Short && gqlObject["x"].userType() != QMetaType::ULong && gqlObject["x"].userType() != QMetaType::UShort && gqlObject["x"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "x").toLocal8Bit().constData();)

		return false;
	}
	x = gqlObject["x"].toDouble();

	if (!gqlObject.ContainsParam("y") || (gqlObject["y"].userType() != QMetaType::Float && gqlObject["y"].userType() != QMetaType::Double && gqlObject["y"].userType() != QMetaType::Int && gqlObject["y"].userType() != QMetaType::UInt && gqlObject["y"].userType() != QMetaType::LongLong && gqlObject["y"].userType() != QMetaType::ULongLong && gqlObject["y"].userType() != QMetaType::Long && gqlObject["y"].userType() != QMetaType::Short && gqlObject["y"].userType() != QMetaType::ULong && gqlObject["y"].userType() != QMetaType::UShort && gqlObject["y"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "y").toLocal8Bit().constData();)

		return false;
	}
	y = gqlObject["y"].toDouble();

	if (!gqlObject.ContainsParam("z") || (gqlObject["z"].userType() != QMetaType::Float && gqlObject["z"].userType() != QMetaType::Double && gqlObject["z"].userType() != QMetaType::Int && gqlObject["z"].userType() != QMetaType::UInt && gqlObject["z"].userType() != QMetaType::LongLong && gqlObject["z"].userType() != QMetaType::ULongLong && gqlObject["z"].userType() != QMetaType::Long && gqlObject["z"].userType() != QMetaType::Short && gqlObject["z"].userType() != QMetaType::ULong && gqlObject["z"].userType() != QMetaType::UShort && gqlObject["z"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "z").toLocal8Bit().constData();)

		return false;
	}
	z = gqlObject["z"].toDouble();

	return true;
}


bool V1_0::complextest::CCoordinates::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool V1_0::complextest::CCoordinates::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!x){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "x").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["x"] = QJsonValue::fromVariant(*x);

	if (!y){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "y").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["y"] = QJsonValue::fromVariant(*y);

	if (!z){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "z").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["z"] = QJsonValue::fromVariant(*z);

	jsonObject["__typename"] = "Coordinates";

	return true;
}


bool V1_0::complextest::CCoordinates::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("x") || ! jsonObject["x"].isDouble()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "x").toLocal8Bit().constData();)

		return false;
	}
	x = jsonObject["x"].toDouble();

	if (!jsonObject.contains("y") || ! jsonObject["y"].isDouble()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "y").toLocal8Bit().constData();)

		return false;
	}
	y = jsonObject["y"].toDouble();

	if (!jsonObject.contains("z") || ! jsonObject["z"].isDouble()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "z").toLocal8Bit().constData();)

		return false;
	}
	z = jsonObject["z"].toDouble();

	return true;
}


bool V1_0::complextest::CCoordinates::OptReadFromJsonObject(const QJsonObject& jsonObject)
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


bool V1_0::complextest::CExtendedMetaData::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "ExtendedMetaData", modelIndex);

	if (key){
		model.SetData("key", *key, modelIndex);
	}

	if (value){
		QVariant valueVariantValue;
		if (const V1_0::complextest::CCoordinates* coordinatesVal = std::get_if<V1_0::complextest::CCoordinates>(value.GetPtr())){
			if (!coordinatesVal->WriteToModel(*(model.AddTreeModel("value", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("value", modelIndex) != nullptr){
				model.GetTreeItemModel("value", modelIndex)->SetData("__typename", "Coordinates", 0);
			}
		}
		else if (const double* floatVal = std::get_if<double>(value.GetPtr())){
			model.SetData("", *floatVal);
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


bool V1_0::complextest::CExtendedMetaData::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant keyData = model.GetData("key", modelIndex);
	if (!keyData.isNull()){
		key = keyData.toString();
	}

	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
		QString valueTypename = model.GetTreeItemModel("value", modelIndex)->GetData("__typename").toString();
		if (valueTypename == "Coordinates") {
			V1_0::complextest::CCoordinates valueConvert;
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


bool V1_0::complextest::CExtendedMetaData::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant keyData = model.GetData("key", modelIndex);
	if (!keyData.isNull()){
		key = keyData.toString();
	}

	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
		QString valueTypename = model.GetTreeItemModel("value", modelIndex)->GetData("__typename").toString();
		if (valueTypename == "Coordinates") {
			V1_0::complextest::CCoordinates valueConvert;
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


bool V1_0::complextest::CExtendedMetaData::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (key){
		gqlObject.InsertParam("key", QVariant(*key));
	}

	if (value){
		::imtgql::CGqlParamObject valueDataObject;
		if (const V1_0::complextest::CCoordinates* coordinatesVal = std::get_if<V1_0::complextest::CCoordinates>(value.GetPtr())){
			if (!coordinatesVal->WriteToGraphQlObject(valueDataObject)){
				return false;
			}
			valueDataObject.InsertParam("__typename", QVariant("Coordinates"));
		}
		else if (const double* floatVal = std::get_if<double>(value.GetPtr())){
			valueDataObject.InsertParam("value", *floatVal);
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


bool V1_0::complextest::CExtendedMetaData::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("key") && (gqlObject["key"].userType() == QMetaType::QString || gqlObject["key"].userType() == QMetaType::QByteArray)){
		key = gqlObject["key"].toString();
	}

	if (gqlObject.ContainsParam("value")){
		const ::imtgql::CGqlParamObject* valueDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("value");
		if (!valueDataObjectPtr->ContainsParam("__typename")){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "value").toLocal8Bit().constData();)

			return false;
		}
		QString valueTypename = valueDataObjectPtr->GetParamArgumentValue("__typename").toString();
		QVariant valueVariantValue = gqlObject["value"];
		if (valueTypename == "Coordinates") {
			V1_0::complextest::CCoordinates valueConvert;
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


bool V1_0::complextest::CExtendedMetaData::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("key") && (gqlObject["key"].userType() == QMetaType::QString || gqlObject["key"].userType() == QMetaType::QByteArray)){
		key = gqlObject["key"].toString();
	}

	if (gqlObject.ContainsParam("value")){
		const ::imtgql::CGqlParamObject* valueDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("value");
		if (!valueDataObjectPtr->ContainsParam("__typename")){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "value").toLocal8Bit().constData();)

			return false;
		}
		QString valueTypename = valueDataObjectPtr->GetParamArgumentValue("__typename").toString();
		QVariant valueVariantValue = gqlObject["value"];
		if (valueTypename == "Coordinates") {
			V1_0::complextest::CCoordinates valueConvert;
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


bool V1_0::complextest::CExtendedMetaData::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (key){
		jsonObject["key"] = QJsonValue::fromVariant(*key);
	}

	if (value){
		if (const V1_0::complextest::CCoordinates* coordinatesVal = std::get_if<V1_0::complextest::CCoordinates>(value.GetPtr())){
			QJsonObject valueJsonObject;
			const bool isvalueAdded = coordinatesVal->WriteToJsonObject(valueJsonObject);
			if (!isvalueAdded){
				return false;
			}
			valueJsonObject["__typename"] = "Coordinates";
			jsonObject["value"] = valueJsonObject;
		}
		else if (const double* floatVal = std::get_if<double>(value.GetPtr())){
			if (!value){
				return false;
			}
			jsonObject["value"] = QJsonValue::fromVariant(*floatVal);
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


bool V1_0::complextest::CExtendedMetaData::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("key") && jsonObject["key"].isString()){
		key = jsonObject["key"].toString();
	}

	if (jsonObject.contains("value")){
		const QJsonObject valueObject = jsonObject.value("value").toObject();
		if (!jsonObject.value("value").isObject() || !valueObject.contains("__typename")){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "value").toLocal8Bit().constData();)

			return false;
		}
		QString valueTypename = valueObject.value("__typename").toString();
		QVariant valueVariantValue = jsonObject["value"].toVariant();
		if (valueTypename == "Coordinates") {
			V1_0::complextest::CCoordinates valueConvert;
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


bool V1_0::complextest::CExtendedMetaData::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("key") && jsonObject["key"].isString()){
		key = jsonObject["key"].toString();
	}

	if (jsonObject.contains("value")){
		const QJsonObject valueObject = jsonObject.value("value").toObject();
		if (!jsonObject.value("value").isObject() || !valueObject.contains("__typename")){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "value").toLocal8Bit().constData();)

			return false;
		}
		QString valueTypename = valueObject.value("__typename").toString();
		QVariant valueVariantValue = jsonObject["value"].toVariant();
		if (valueTypename == "Coordinates") {
			V1_0::complextest::CCoordinates valueConvert;
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


bool V1_0::complextest::CResultMetaData::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "ResultMetaData", modelIndex);

	if (!resultId){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "resultId").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}

	}
	if (extendedMetaData){
		::imtbase::CTreeItemModel* newExtendedMetaDataModelPtr = model.AddTreeModel("extendedMetaData", modelIndex);
		newExtendedMetaDataModelPtr->setIsArray(true);
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaData->size(); ++extendedMetaDataIndex){
			if (!extendedMetaData->at(extendedMetaDataIndex)){
				newExtendedMetaDataModelPtr->InsertNewItem();
				newExtendedMetaDataModelPtr->SetData(QByteArray(), QVariant(), extendedMetaDataIndex);
				continue;
			}
			newExtendedMetaDataModelPtr->InsertNewItem();
			if (!(extendedMetaData->at(extendedMetaDataIndex)->WriteToModel(*newExtendedMetaDataModelPtr, extendedMetaDataIndex))){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
		}
	}
	else if (extendedMetaData.IsNull()){
		model.SetData("extendedMetaData", QVariant(), modelIndex);
	}

	return true;
}


bool V1_0::complextest::CResultMetaData::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant resultIdData = model.GetData("resultId", modelIndex);
	if (resultIdData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "resultId").toLocal8Bit().constData();)

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
		description = V1_0::complextest::CLocalizedText();
		const bool isDescriptionRead = description->ReadFromModel(*descriptionDataModelPtr, modelIndex);
		if (!isDescriptionRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* extendedMetaDataModel = model.GetTreeItemModel("extendedMetaData", modelIndex);
	if (model.ContainsKey("extendedMetaData", modelIndex) && extendedMetaDataModel == nullptr){
		extendedMetaData.SetNull();
	}
	if (extendedMetaDataModel != nullptr){
		int extendedMetaDataCount = extendedMetaDataModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::CExtendedMetaData> extendedMetaDataList;
		for (int extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataCount; ++extendedMetaDataIndex){
			if (extendedMetaDataModel->ContainsKey(QByteArray(), extendedMetaDataIndex) && !extendedMetaDataModel->GetData(QByteArray(), extendedMetaDataIndex).isValid()){
				extendedMetaDataList.AppendNull();
				continue;
			}
			V1_0::complextest::CExtendedMetaData t_extendedMetaData;
			if (!t_extendedMetaData.ReadFromModel(*extendedMetaDataModel, extendedMetaDataIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
			extendedMetaDataList << t_extendedMetaData;
		}
		extendedMetaData = extendedMetaDataList;

	}

	return true;
}


bool V1_0::complextest::CResultMetaData::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
		description = V1_0::complextest::CLocalizedText();
		const bool isDescriptionRead = description->ReadFromModel(*descriptionDataModelPtr, modelIndex);
		if (!isDescriptionRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* extendedMetaDataModel = model.GetTreeItemModel("extendedMetaData", modelIndex);
	if (model.ContainsKey("extendedMetaData", modelIndex) && extendedMetaDataModel == nullptr){
		extendedMetaData.SetNull();
	}
	if (extendedMetaDataModel != nullptr){
		int extendedMetaDataCount = extendedMetaDataModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::CExtendedMetaData> extendedMetaDataList;
		for (int extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataCount; ++extendedMetaDataIndex){
			if (extendedMetaDataModel->ContainsKey(QByteArray(), extendedMetaDataIndex) && !extendedMetaDataModel->GetData(QByteArray(), extendedMetaDataIndex).isValid()){
				extendedMetaDataList.AppendNull();
				continue;
			}
			V1_0::complextest::CExtendedMetaData t_extendedMetaData;
			if (!t_extendedMetaData.OptReadFromModel(*extendedMetaDataModel, extendedMetaDataIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
			extendedMetaDataList << t_extendedMetaData;
		}
		extendedMetaData = extendedMetaDataList;

	}

	return true;
}


bool V1_0::complextest::CResultMetaData::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!resultId){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "resultId").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("description", descriptionGqlObject);
	}

	if (extendedMetaData){
		QList<::imtgql::CGqlParamObject> extendedMetaDataDataObjectList;
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaData->size(); ++extendedMetaDataIndex){
			if (!extendedMetaData->at(extendedMetaDataIndex)){
				extendedMetaDataDataObjectList << ::imtgql::CGqlParamObject::CreateNull();
				continue;
			}
			::imtgql::CGqlParamObject newExtendedMetaDataGqlObject;
			if (!extendedMetaData->at(extendedMetaDataIndex)->WriteToGraphQlObject(newExtendedMetaDataGqlObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
			extendedMetaDataDataObjectList << newExtendedMetaDataGqlObject;
		}
		gqlObject.InsertParam("extendedMetaData", extendedMetaDataDataObjectList);
	}
	else if (extendedMetaData.IsNull()){
		gqlObject.InsertParam("extendedMetaData", QVariant());
	}

	gqlObject.InsertParam("__typename", QVariant("ResultMetaData"));

	return true;
}


bool V1_0::complextest::CResultMetaData::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("resultId") || (gqlObject["resultId"].userType() != QMetaType::QString && gqlObject["resultId"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "resultId").toLocal8Bit().constData();)

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
		description = V1_0::complextest::CLocalizedText();
		const bool isDescriptionRead = description->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("description"));
		if (!isDescriptionRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("extendedMetaData") && gqlObject["extendedMetaData"].isNull()){
		extendedMetaData.SetNull();
	}
	if (gqlObject.ContainsParam("extendedMetaData") && !(gqlObject["extendedMetaData"].isNull())){
		const qsizetype extendedMetaDataElementsCount = gqlObject.GetObjectsCount("extendedMetaData");
		extendedMetaData = imtsdl::TElementList<V1_0::complextest::CExtendedMetaData>();
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataElementsCount; ++extendedMetaDataIndex){
			const ::imtgql::CGqlParamObject* extendedMetaDataDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("extendedMetaData", extendedMetaDataIndex);
			if (extendedMetaDataDataObjectPtr == nullptr || extendedMetaDataDataObjectPtr->IsNull()){
				extendedMetaData->AppendNull();
				continue;
			}
			V1_0::complextest::CExtendedMetaData tempExtendedMetaData;
			if (!tempExtendedMetaData.ReadFromGraphQlObject(*extendedMetaDataDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
			extendedMetaData->append(tempExtendedMetaData);
		}
	}

	return true;
}


bool V1_0::complextest::CResultMetaData::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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
		description = V1_0::complextest::CLocalizedText();
		const bool isDescriptionRead = description->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("description"));
		if (!isDescriptionRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("extendedMetaData") && gqlObject["extendedMetaData"].isNull()){
		extendedMetaData.SetNull();
	}
	if (gqlObject.ContainsParam("extendedMetaData") && !(gqlObject["extendedMetaData"].isNull())){
		const qsizetype extendedMetaDataElementsCount = gqlObject.GetObjectsCount("extendedMetaData");
		extendedMetaData = imtsdl::TElementList<V1_0::complextest::CExtendedMetaData>();
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataElementsCount; ++extendedMetaDataIndex){
			const ::imtgql::CGqlParamObject* extendedMetaDataDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("extendedMetaData", extendedMetaDataIndex);
			if (extendedMetaDataDataObjectPtr == nullptr || extendedMetaDataDataObjectPtr->IsNull()){
				extendedMetaData->AppendNull();
				continue;
			}
			V1_0::complextest::CExtendedMetaData tempExtendedMetaData;
			if (!tempExtendedMetaData.OptReadFromGraphQlObject(*extendedMetaDataDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
			extendedMetaData->append(tempExtendedMetaData);
		}
	}

	return true;
}


bool V1_0::complextest::CResultMetaData::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!resultId){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "resultId").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["description"] = descriptionJsonObject;
	}

	if (extendedMetaData){
		QJsonArray newExtendedMetaDataArray;
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaData->size(); ++extendedMetaDataIndex){
			if (!extendedMetaData->at(extendedMetaDataIndex)){
				newExtendedMetaDataArray << QJsonValue(QJsonValue::Null);
				continue;
			}
			QJsonObject newExtendedMetaDataJsonObject;
			if (!extendedMetaData->at(extendedMetaDataIndex)->WriteToJsonObject(newExtendedMetaDataJsonObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
			newExtendedMetaDataArray << newExtendedMetaDataJsonObject;
		}
		jsonObject["extendedMetaData"] = newExtendedMetaDataArray;
	}
	else if (extendedMetaData.IsNull()){
		jsonObject["extendedMetaData"] = QJsonValue(QJsonValue::Null);
	}

	jsonObject["__typename"] = "ResultMetaData";

	return true;
}


bool V1_0::complextest::CResultMetaData::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("resultId") || ! jsonObject["resultId"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "resultId").toLocal8Bit().constData();)

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
		description = V1_0::complextest::CLocalizedText();
		const bool isDescriptionRead = description->ReadFromJsonObject(jsonObject["description"].toObject());
		if (!isDescriptionRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}
	}

if (jsonObject.contains("extendedMetaData") && jsonObject["extendedMetaData"].isNull()){
		extendedMetaData.SetNull();
	}
		if (jsonObject.contains("extendedMetaData") && jsonObject["extendedMetaData"].isArray()){
		const QJsonArray extendedMetaDataJsonArray = jsonObject["extendedMetaData"].toArray();
		const qsizetype extendedMetaDataArrayCount = extendedMetaDataJsonArray.size();
		extendedMetaData = imtsdl::TElementList<V1_0::complextest::CExtendedMetaData>();
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataArrayCount; ++extendedMetaDataIndex){
			if (extendedMetaDataJsonArray[extendedMetaDataIndex].isNull()){
				extendedMetaData->AppendNull();
				continue;
			}
			V1_0::complextest::CExtendedMetaData tempExtendedMetaData;
			if (!tempExtendedMetaData.ReadFromJsonObject(extendedMetaDataJsonArray[extendedMetaDataIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
			extendedMetaData->append(tempExtendedMetaData);
		}
	}

	return true;
}


bool V1_0::complextest::CResultMetaData::OptReadFromJsonObject(const QJsonObject& jsonObject)
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
		description = V1_0::complextest::CLocalizedText();
		const bool isDescriptionRead = description->OptReadFromJsonObject(jsonObject["description"].toObject());
		if (!isDescriptionRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "description").toLocal8Bit().constData();)

			return false;
		}
	}

if (jsonObject.contains("extendedMetaData") && jsonObject["extendedMetaData"].isNull()){
		extendedMetaData.SetNull();
	}
		if (jsonObject.contains("extendedMetaData") && jsonObject["extendedMetaData"].isArray()){
		const QJsonArray extendedMetaDataJsonArray = jsonObject["extendedMetaData"].toArray();
		const qsizetype extendedMetaDataArrayCount = extendedMetaDataJsonArray.size();
		extendedMetaData = imtsdl::TElementList<V1_0::complextest::CExtendedMetaData>();
		for (qsizetype extendedMetaDataIndex = 0; extendedMetaDataIndex < extendedMetaDataArrayCount; ++extendedMetaDataIndex){
			if (extendedMetaDataJsonArray[extendedMetaDataIndex].isNull()){
				extendedMetaData->AppendNull();
				continue;
			}
			V1_0::complextest::CExtendedMetaData tempExtendedMetaData;
			if (!tempExtendedMetaData.OptReadFromJsonObject(extendedMetaDataJsonArray[extendedMetaDataIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "extendedMetaData").toLocal8Bit().constData();)

				return false;
			}
			extendedMetaData->append(tempExtendedMetaData);
		}
	}

	return true;
}




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


bool V1_0::complextest::COverallResultValues::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "OverallResultValues", modelIndex);

	if (name){
		model.SetData("name", *name, modelIndex);
	}

	if (!valueId){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "valueId").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("valueId", *valueId, modelIndex);

	if (!measuredValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "measuredValue").toLocal8Bit().constData();)

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


bool V1_0::complextest::COverallResultValues::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	QVariant valueIdData = model.GetData("valueId", modelIndex);
	if (valueIdData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "valueId").toLocal8Bit().constData();)

		return false;
	}
	valueId = valueIdData.toString();

	QVariant measuredValueData = model.GetData("measuredValue", modelIndex);
	if (measuredValueData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "measuredValue").toLocal8Bit().constData();)

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


bool V1_0::complextest::COverallResultValues::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool V1_0::complextest::COverallResultValues::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (name){
		gqlObject.InsertParam("name", QVariant(*name));
	}

	if (!valueId){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "valueId").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("valueId", QVariant(*valueId));

	if (!measuredValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "measuredValue").toLocal8Bit().constData();)

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


bool V1_0::complextest::COverallResultValues::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (!gqlObject.ContainsParam("valueId") || (gqlObject["valueId"].userType() != QMetaType::QString && gqlObject["valueId"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "valueId").toLocal8Bit().constData();)

		return false;
	}
	valueId = gqlObject["valueId"].toString();

	if (!gqlObject.ContainsParam("measuredValue") || (gqlObject["measuredValue"].userType() != QMetaType::Float && gqlObject["measuredValue"].userType() != QMetaType::Double && gqlObject["measuredValue"].userType() != QMetaType::Int && gqlObject["measuredValue"].userType() != QMetaType::UInt && gqlObject["measuredValue"].userType() != QMetaType::LongLong && gqlObject["measuredValue"].userType() != QMetaType::ULongLong && gqlObject["measuredValue"].userType() != QMetaType::Long && gqlObject["measuredValue"].userType() != QMetaType::Short && gqlObject["measuredValue"].userType() != QMetaType::ULong && gqlObject["measuredValue"].userType() != QMetaType::UShort && gqlObject["measuredValue"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "measuredValue").toLocal8Bit().constData();)

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


bool V1_0::complextest::COverallResultValues::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool V1_0::complextest::COverallResultValues::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (name){
		jsonObject["name"] = QJsonValue::fromVariant(*name);
	}

	if (!valueId){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "valueId").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["valueId"] = QJsonValue::fromVariant(*valueId);

	if (!measuredValue){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "measuredValue").toLocal8Bit().constData();)

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


bool V1_0::complextest::COverallResultValues::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (!jsonObject.contains("valueId") || ! jsonObject["valueId"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "valueId").toLocal8Bit().constData();)

		return false;
	}
	valueId = jsonObject["valueId"].toString();

	if (!jsonObject.contains("measuredValue") || ! jsonObject["measuredValue"].isDouble()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "measuredValue").toLocal8Bit().constData();)

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


bool V1_0::complextest::COverallResultValues::OptReadFromJsonObject(const QJsonObject& jsonObject)
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


bool V1_0::complextest::CUnionTestingType::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "UnionTestingType", modelIndex);

	if (simpleUnion){
		QVariant simpleUnionVariantValue;
		if (const double* floatVal = std::get_if<double>(simpleUnion.GetPtr())){
			model.SetData("", *floatVal);
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
		if (const V1_0::complextest::CCDMResultVarString* cDMResultVarStringVal = std::get_if<V1_0::complextest::CCDMResultVarString>(complexUnion.GetPtr())){
			if (!cDMResultVarStringVal->WriteToModel(*(model.AddTreeModel("complexUnion", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("complexUnion", modelIndex) != nullptr){
				model.GetTreeItemModel("complexUnion", modelIndex)->SetData("__typename", "CDMResultVarString", 0);
			}
		}
		else if (const V1_0::complextest::CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<V1_0::complextest::CCDMResultVarRecursive>(complexUnion.GetPtr())){
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
		if (const V1_0::complextest::CCoordinates* coordinatesVal = std::get_if<V1_0::complextest::CCoordinates>(mixedUnion.GetPtr())){
			if (!coordinatesVal->WriteToModel(*(model.AddTreeModel("mixedUnion", modelIndex)), 0)){
				return false;
			}
			if(model.GetTreeItemModel("mixedUnion", modelIndex) != nullptr){
				model.GetTreeItemModel("mixedUnion", modelIndex)->SetData("__typename", "Coordinates", 0);
			}
		}
		else if (const double* floatVal = std::get_if<double>(mixedUnion.GetPtr())){
			model.SetData("", *floatVal);
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


bool V1_0::complextest::CUnionTestingType::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
			V1_0::complextest::CCDMResultVarString complexUnionConvert;
			const bool iscomplexUnionRead = complexUnionConvert.ReadFromModel(*model.GetTreeItemModel("complexUnion", modelIndex)); 
			if (!iscomplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
		else if (complexUnionTypename == "CDMResultVarRecursive") {
			V1_0::complextest::CCDMResultVarRecursive complexUnionConvert;
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
			V1_0::complextest::CCoordinates mixedUnionConvert;
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


bool V1_0::complextest::CUnionTestingType::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
			V1_0::complextest::CCDMResultVarString complexUnionConvert;
			const bool iscomplexUnionRead = complexUnionConvert.ReadFromModel(*model.GetTreeItemModel("complexUnion", modelIndex)); 
			if (!iscomplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
		else if (complexUnionTypename == "CDMResultVarRecursive") {
			V1_0::complextest::CCDMResultVarRecursive complexUnionConvert;
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
			V1_0::complextest::CCoordinates mixedUnionConvert;
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


bool V1_0::complextest::CUnionTestingType::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (simpleUnion){
		::imtgql::CGqlParamObject simpleUnionDataObject;
		if (const double* floatVal = std::get_if<double>(simpleUnion.GetPtr())){
			simpleUnionDataObject.InsertParam("simpleUnion", *floatVal);
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
		if (const V1_0::complextest::CCDMResultVarString* cDMResultVarStringVal = std::get_if<V1_0::complextest::CCDMResultVarString>(complexUnion.GetPtr())){
			if (!cDMResultVarStringVal->WriteToGraphQlObject(complexUnionDataObject)){
				return false;
			}
			complexUnionDataObject.InsertParam("__typename", QVariant("CDMResultVarString"));
		}
		else if (const V1_0::complextest::CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<V1_0::complextest::CCDMResultVarRecursive>(complexUnion.GetPtr())){
			if (!cDMResultVarRecursiveVal->WriteToGraphQlObject(complexUnionDataObject)){
				return false;
			}
			complexUnionDataObject.InsertParam("__typename", QVariant("CDMResultVarRecursive"));
		}
		gqlObject.InsertParam("complexUnion", complexUnionDataObject);
	}

	if (mixedUnion){
		::imtgql::CGqlParamObject mixedUnionDataObject;
		if (const V1_0::complextest::CCoordinates* coordinatesVal = std::get_if<V1_0::complextest::CCoordinates>(mixedUnion.GetPtr())){
			if (!coordinatesVal->WriteToGraphQlObject(mixedUnionDataObject)){
				return false;
			}
			mixedUnionDataObject.InsertParam("__typename", QVariant("Coordinates"));
		}
		else if (const double* floatVal = std::get_if<double>(mixedUnion.GetPtr())){
			mixedUnionDataObject.InsertParam("mixedUnion", *floatVal);
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


bool V1_0::complextest::CUnionTestingType::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "complexUnion").toLocal8Bit().constData();)

			return false;
		}
		QString complexUnionTypename = complexUnionDataObjectPtr->GetParamArgumentValue("__typename").toString();
		if (complexUnionTypename == "CDMResultVarString") {
			V1_0::complextest::CCDMResultVarString complexUnionConvert;
			const bool isComplexUnionRead = complexUnionConvert.ReadFromGraphQlObject(*complexUnionDataObjectPtr);
			if (!isComplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
		else if (complexUnionTypename == "CDMResultVarRecursive") {
			V1_0::complextest::CCDMResultVarRecursive complexUnionConvert;
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedUnion").toLocal8Bit().constData();)

			return false;
		}
		QString mixedUnionTypename = mixedUnionDataObjectPtr->GetParamArgumentValue("__typename").toString();
		QVariant mixedUnionVariantValue = gqlObject["mixedUnion"];
		if (mixedUnionTypename == "Coordinates") {
			V1_0::complextest::CCoordinates mixedUnionConvert;
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


bool V1_0::complextest::CUnionTestingType::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "complexUnion").toLocal8Bit().constData();)

			return false;
		}
		QString complexUnionTypename = complexUnionDataObjectPtr->GetParamArgumentValue("__typename").toString();
		if (complexUnionTypename == "CDMResultVarString") {
			V1_0::complextest::CCDMResultVarString complexUnionConvert;
			const bool isComplexUnionRead = complexUnionConvert.ReadFromGraphQlObject(*complexUnionDataObjectPtr);
			if (!isComplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
		else if (complexUnionTypename == "CDMResultVarRecursive") {
			V1_0::complextest::CCDMResultVarRecursive complexUnionConvert;
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedUnion").toLocal8Bit().constData();)

			return false;
		}
		QString mixedUnionTypename = mixedUnionDataObjectPtr->GetParamArgumentValue("__typename").toString();
		QVariant mixedUnionVariantValue = gqlObject["mixedUnion"];
		if (mixedUnionTypename == "Coordinates") {
			V1_0::complextest::CCoordinates mixedUnionConvert;
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


bool V1_0::complextest::CUnionTestingType::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (simpleUnion){
		if (const double* floatVal = std::get_if<double>(simpleUnion.GetPtr())){
			if (!simpleUnion){
				return false;
			}
			jsonObject["simpleUnion"] = QJsonValue::fromVariant(*floatVal);
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
		if (const V1_0::complextest::CCDMResultVarString* cDMResultVarStringVal = std::get_if<V1_0::complextest::CCDMResultVarString>(complexUnion.GetPtr())){
			QJsonObject complexUnionJsonObject;
			const bool iscomplexUnionAdded = cDMResultVarStringVal->WriteToJsonObject(complexUnionJsonObject);
			if (!iscomplexUnionAdded){
				return false;
			}
			complexUnionJsonObject["__typename"] = "CDMResultVarString";
			jsonObject["complexUnion"] = complexUnionJsonObject;
		}
		else if (const V1_0::complextest::CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<V1_0::complextest::CCDMResultVarRecursive>(complexUnion.GetPtr())){
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
		if (const V1_0::complextest::CCoordinates* coordinatesVal = std::get_if<V1_0::complextest::CCoordinates>(mixedUnion.GetPtr())){
			QJsonObject mixedUnionJsonObject;
			const bool ismixedUnionAdded = coordinatesVal->WriteToJsonObject(mixedUnionJsonObject);
			if (!ismixedUnionAdded){
				return false;
			}
			mixedUnionJsonObject["__typename"] = "Coordinates";
			jsonObject["mixedUnion"] = mixedUnionJsonObject;
		}
		else if (const double* floatVal = std::get_if<double>(mixedUnion.GetPtr())){
			if (!mixedUnion){
				return false;
			}
			jsonObject["mixedUnion"] = QJsonValue::fromVariant(*floatVal);
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


bool V1_0::complextest::CUnionTestingType::ReadFromJsonObject(const QJsonObject& jsonObject)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "complexUnion").toLocal8Bit().constData();)

			return false;
		}
		QString complexUnionTypename = complexUnionObject.value("__typename").toString();
		if (complexUnionTypename == "CDMResultVarString") {
			V1_0::complextest::CCDMResultVarString complexUnionConvert;
			const bool iscomplexUnionRead = complexUnionConvert.ReadFromJsonObject(jsonObject["complexUnion"].toObject());
			if (!iscomplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
		else if (complexUnionTypename == "CDMResultVarRecursive") {
			V1_0::complextest::CCDMResultVarRecursive complexUnionConvert;
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedUnion").toLocal8Bit().constData();)

			return false;
		}
		QString mixedUnionTypename = mixedUnionObject.value("__typename").toString();
		QVariant mixedUnionVariantValue = jsonObject["mixedUnion"].toVariant();
		if (mixedUnionTypename == "Coordinates") {
			V1_0::complextest::CCoordinates mixedUnionConvert;
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


bool V1_0::complextest::CUnionTestingType::OptReadFromJsonObject(const QJsonObject& jsonObject)
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "complexUnion").toLocal8Bit().constData();)

			return false;
		}
		QString complexUnionTypename = complexUnionObject.value("__typename").toString();
		if (complexUnionTypename == "CDMResultVarString") {
			V1_0::complextest::CCDMResultVarString complexUnionConvert;
			const bool iscomplexUnionRead = complexUnionConvert.ReadFromJsonObject(jsonObject["complexUnion"].toObject());
			if (!iscomplexUnionRead){
				return false;
			}
			complexUnion = CDMResultUnionType(complexUnionConvert);
		}
		else if (complexUnionTypename == "CDMResultVarRecursive") {
			V1_0::complextest::CCDMResultVarRecursive complexUnionConvert;
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: __typename for field '%3' is missing, but expected").arg(__FILE__, QString::number(__LINE__), "mixedUnion").toLocal8Bit().constData();)

			return false;
		}
		QString mixedUnionTypename = mixedUnionObject.value("__typename").toString();
		QVariant mixedUnionVariantValue = jsonObject["mixedUnion"].toVariant();
		if (mixedUnionTypename == "Coordinates") {
			V1_0::complextest::CCoordinates mixedUnionConvert;
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


bool V1_0::complextest::CCDMResultVarRecursive::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "CDMResultVarRecursive", modelIndex);

	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("dataModelTitle", *dataModelTitle, modelIndex);

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("dataModelVersion", *dataModelVersion, modelIndex);


	if (resultMetaData){
		::imtbase::CTreeItemModel* resultMetaDataNewModelPtr = model.AddTreeModel("resultMetaData", modelIndex);
		const bool isResultMetaDataAdded = resultMetaData->WriteToModel(*resultMetaDataNewModelPtr, 0);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}

	}
	if (resultContent){
		::imtbase::CTreeItemModel* newResultContentModelPtr = model.AddTreeModel("resultContent", modelIndex);
		newResultContentModelPtr->setIsArray(true);
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			if (!resultContent->at(resultContentIndex)){
				newResultContentModelPtr->InsertNewItem();
				newResultContentModelPtr->SetData(QByteArray(), QVariant(), resultContentIndex);
				continue;
			}
			QVariant resultContentVariantValue;
			if (const V1_0::complextest::CCDMResultVarString* cDMResultVarStringVal = std::get_if<V1_0::complextest::CCDMResultVarString>(resultContent->at(resultContentIndex).GetPtr())){
				newResultContentModelPtr->InsertNewItem();
				if(!cDMResultVarStringVal->WriteToModel(*newResultContentModelPtr, resultContentIndex)){
					return false;
				}
				newResultContentModelPtr->SetData("__typename", "CDMResultVarString", resultContentIndex);
			}
			else if (const V1_0::complextest::CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<V1_0::complextest::CCDMResultVarRecursive>(resultContent->at(resultContentIndex).GetPtr())){
				newResultContentModelPtr->InsertNewItem();
				if(!cDMResultVarRecursiveVal->WriteToModel(*newResultContentModelPtr, resultContentIndex)){
					return false;
				}
				newResultContentModelPtr->SetData("__typename", "CDMResultVarRecursive", resultContentIndex);
			}
		}
	}
	else if (resultContent.IsNull()){
		model.SetData("resultContent", QVariant(), modelIndex);
	}

	if (overallResultValues){
		::imtbase::CTreeItemModel* newOverallResultValuesModelPtr = model.AddTreeModel("overallResultValues", modelIndex);
		newOverallResultValuesModelPtr->setIsArray(true);
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			if (!overallResultValues->at(overallResultValuesIndex)){
				newOverallResultValuesModelPtr->InsertNewItem();
				newOverallResultValuesModelPtr->SetData(QByteArray(), QVariant(), overallResultValuesIndex);
				continue;
			}
			newOverallResultValuesModelPtr->InsertNewItem();
			if (!(overallResultValues->at(overallResultValuesIndex)->WriteToModel(*newOverallResultValuesModelPtr, overallResultValuesIndex))){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
		}
	}
	else if (overallResultValues.IsNull()){
		model.SetData("overallResultValues", QVariant(), modelIndex);
	}

	return true;
}


bool V1_0::complextest::CCDMResultVarRecursive::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant dataModelTitleData = model.GetData("dataModelTitle", modelIndex);
	if (dataModelTitleData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = dataModelTitleData.toString();

	QVariant dataModelVersionData = model.GetData("dataModelVersion", modelIndex);
	if (dataModelVersionData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = dataModelVersionData.toString();

	::imtbase::CTreeItemModel* resultMetaDataDataModelPtr = model.GetTreeItemModel("resultMetaData", modelIndex);
	if (resultMetaDataDataModelPtr != nullptr){
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (model.ContainsKey("resultContent", modelIndex) && resultContentModel == nullptr){
		resultContent.SetNull();
	}
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::CDMResultUnionType> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			if (resultContentModel->ContainsKey(QByteArray(), resultContentIndex) && !resultContentModel->GetData(QByteArray(), resultContentIndex).isValid()){
				resultContentList.AppendNull();
				continue;
			}
			V1_0::complextest::CDMResultUnionType resultContentData;
			QVariant resultContentVariantValue = resultContentModel->GetData(QByteArray(), resultContentIndex);
			QString resultContentDataTypename = resultContentModel->GetData("__typename", resultContentIndex).toString();
			if (resultContentDataTypename == "CDMResultVarString") {
				V1_0::complextest::CCDMResultVarString resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = CDMResultUnionType(resultContentDataConvert);
			}
			else if (resultContentDataTypename == "CDMResultVarRecursive") {
				V1_0::complextest::CCDMResultVarRecursive resultContentDataConvert;
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
	if (model.ContainsKey("overallResultValues", modelIndex) && overallResultValuesModel == nullptr){
		overallResultValues.SetNull();
	}
	if (overallResultValuesModel != nullptr){
		int overallResultValuesCount = overallResultValuesModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::COverallResultValues> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			if (overallResultValuesModel->ContainsKey(QByteArray(), overallResultValuesIndex) && !overallResultValuesModel->GetData(QByteArray(), overallResultValuesIndex).isValid()){
				overallResultValuesList.AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues t_overallResultValues;
			if (!t_overallResultValues.ReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesList << t_overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool V1_0::complextest::CCDMResultVarRecursive::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (model.ContainsKey("resultContent", modelIndex) && resultContentModel == nullptr){
		resultContent.SetNull();
	}
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::CDMResultUnionType> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			if (resultContentModel->ContainsKey(QByteArray(), resultContentIndex) && !resultContentModel->GetData(QByteArray(), resultContentIndex).isValid()){
				resultContentList.AppendNull();
				continue;
			}
			V1_0::complextest::CDMResultUnionType resultContentData;
			QVariant resultContentVariantValue = resultContentModel->GetData(QByteArray(), resultContentIndex);
			QString resultContentDataTypename = resultContentModel->GetData("__typename", resultContentIndex).toString();
			if (resultContentDataTypename == "CDMResultVarString") {
				V1_0::complextest::CCDMResultVarString resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = CDMResultUnionType(resultContentDataConvert);
			}
			else if (resultContentDataTypename == "CDMResultVarRecursive") {
				V1_0::complextest::CCDMResultVarRecursive resultContentDataConvert;
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
	if (model.ContainsKey("overallResultValues", modelIndex) && overallResultValuesModel == nullptr){
		overallResultValues.SetNull();
	}
	if (overallResultValuesModel != nullptr){
		int overallResultValuesCount = overallResultValuesModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::COverallResultValues> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			if (overallResultValuesModel->ContainsKey(QByteArray(), overallResultValuesIndex) && !overallResultValuesModel->GetData(QByteArray(), overallResultValuesIndex).isValid()){
				overallResultValuesList.AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues t_overallResultValues;
			if (!t_overallResultValues.OptReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesList << t_overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool V1_0::complextest::CCDMResultVarRecursive::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("dataModelTitle", QVariant(*dataModelTitle));

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("dataModelVersion", QVariant(*dataModelVersion));

	if (resultMetaData){
		::imtgql::CGqlParamObject resultMetaDataGqlObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToGraphQlObject(resultMetaDataGqlObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("resultMetaData", resultMetaDataGqlObject);
	}

	if (resultContent){
		QList<::imtgql::CGqlParamObject> resultContentDataObjectList;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			if (!resultContent->at(resultContentIndex)){
				resultContentDataObjectList << ::imtgql::CGqlParamObject::CreateNull();
				continue;
			}
			::imtgql::CGqlParamObject resultContentDataObject;
			if (const V1_0::complextest::CCDMResultVarString* cDMResultVarStringVal = std::get_if<V1_0::complextest::CCDMResultVarString>(resultContent->at(resultContentIndex).GetPtr())){
				if (!cDMResultVarStringVal->WriteToGraphQlObject(resultContentDataObject)){
					return false;
				}
				resultContentDataObject.InsertParam("__typename", QVariant("CDMResultVarString"));
			}
			else if (const V1_0::complextest::CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<V1_0::complextest::CCDMResultVarRecursive>(resultContent->at(resultContentIndex).GetPtr())){
				if (!cDMResultVarRecursiveVal->WriteToGraphQlObject(resultContentDataObject)){
					return false;
				}
				resultContentDataObject.InsertParam("__typename", QVariant("CDMResultVarRecursive"));
			}
			resultContentDataObjectList << resultContentDataObject;
		}
		gqlObject.InsertParam("resultContent", resultContentDataObjectList);
	}
	else if (resultContent.IsNull()){
		gqlObject.InsertParam("resultContent", QVariant());
	}

	if (overallResultValues){
		QList<::imtgql::CGqlParamObject> overallResultValuesDataObjectList;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			if (!overallResultValues->at(overallResultValuesIndex)){
				overallResultValuesDataObjectList << ::imtgql::CGqlParamObject::CreateNull();
				continue;
			}
			::imtgql::CGqlParamObject newOverallResultValuesGqlObject;
			if (!overallResultValues->at(overallResultValuesIndex)->WriteToGraphQlObject(newOverallResultValuesGqlObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesDataObjectList << newOverallResultValuesGqlObject;
		}
		gqlObject.InsertParam("overallResultValues", overallResultValuesDataObjectList);
	}
	else if (overallResultValues.IsNull()){
		gqlObject.InsertParam("overallResultValues", QVariant());
	}

	gqlObject.InsertParam("__typename", QVariant("CDMResultVarRecursive"));

	return true;
}


bool V1_0::complextest::CCDMResultVarRecursive::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("dataModelTitle") || (gqlObject["dataModelTitle"].userType() != QMetaType::QString && gqlObject["dataModelTitle"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = gqlObject["dataModelTitle"].toString();

	if (!gqlObject.ContainsParam("dataModelVersion") || (gqlObject["dataModelVersion"].userType() != QMetaType::QString && gqlObject["dataModelVersion"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = gqlObject["dataModelVersion"].toString();

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent") && gqlObject["resultContent"].isNull()){
		resultContent.SetNull();
	}
	if (gqlObject.ContainsParam("resultContent") && !(gqlObject["resultContent"].isNull())){
		const QList<const ::imtgql::CGqlParamObject*> resultContentDataList = gqlObject.GetParamArgumentObjectPtrList("resultContent");
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = imtsdl::TElementList<V1_0::complextest::CDMResultUnionType>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			const ::imtgql::CGqlParamObject* tempResultContent = resultContentDataList[resultContentIndex];
			QString resultContentDataValueTypename = (tempResultContent == nullptr || tempResultContent->IsNull()) ? QString() : tempResultContent->GetParamArgumentValue("__typename").toString();
			if (resultContentDataList[resultContentIndex] == nullptr || resultContentDataList[resultContentIndex]->IsNull()){
				resultContent->AppendNull();
				continue;
			}
			V1_0::complextest::CDMResultUnionType resultContentDataValue;
			if (resultContentDataValueTypename == "CDMResultVarString") {
				V1_0::complextest::CCDMResultVarString resultContentDataValueConvert;
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*tempResultContent);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else if (resultContentDataValueTypename == "CDMResultVarRecursive") {
				V1_0::complextest::CCDMResultVarRecursive resultContentDataValueConvert;
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

	if (gqlObject.ContainsParam("overallResultValues") && gqlObject["overallResultValues"].isNull()){
		overallResultValues.SetNull();
	}
	if (gqlObject.ContainsParam("overallResultValues") && !(gqlObject["overallResultValues"].isNull())){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = imtsdl::TElementList<V1_0::complextest::COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr || overallResultValuesDataObjectPtr->IsNull()){
				overallResultValues->AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool V1_0::complextest::CCDMResultVarRecursive::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("dataModelTitle") && (gqlObject["dataModelTitle"].userType() == QMetaType::QString || gqlObject["dataModelTitle"].userType() == QMetaType::QByteArray)){
		dataModelTitle = gqlObject["dataModelTitle"].toString();
	}

	if (gqlObject.ContainsParam("dataModelVersion") && (gqlObject["dataModelVersion"].userType() == QMetaType::QString || gqlObject["dataModelVersion"].userType() == QMetaType::QByteArray)){
		dataModelVersion = gqlObject["dataModelVersion"].toString();
	}

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent") && gqlObject["resultContent"].isNull()){
		resultContent.SetNull();
	}
	if (gqlObject.ContainsParam("resultContent") && !(gqlObject["resultContent"].isNull())){
		const QList<const ::imtgql::CGqlParamObject*> resultContentDataList = gqlObject.GetParamArgumentObjectPtrList("resultContent");
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = imtsdl::TElementList<V1_0::complextest::CDMResultUnionType>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			const ::imtgql::CGqlParamObject* tempResultContent = resultContentDataList[resultContentIndex];
			QString resultContentDataValueTypename = (tempResultContent == nullptr || tempResultContent->IsNull()) ? QString() : tempResultContent->GetParamArgumentValue("__typename").toString();
			if (resultContentDataList[resultContentIndex] == nullptr || resultContentDataList[resultContentIndex]->IsNull()){
				resultContent->AppendNull();
				continue;
			}
			V1_0::complextest::CDMResultUnionType resultContentDataValue;
			if (resultContentDataValueTypename == "CDMResultVarString") {
				V1_0::complextest::CCDMResultVarString resultContentDataValueConvert;
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*tempResultContent);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else if (resultContentDataValueTypename == "CDMResultVarRecursive") {
				V1_0::complextest::CCDMResultVarRecursive resultContentDataValueConvert;
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

	if (gqlObject.ContainsParam("overallResultValues") && gqlObject["overallResultValues"].isNull()){
		overallResultValues.SetNull();
	}
	if (gqlObject.ContainsParam("overallResultValues") && !(gqlObject["overallResultValues"].isNull())){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = imtsdl::TElementList<V1_0::complextest::COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr || overallResultValuesDataObjectPtr->IsNull()){
				overallResultValues->AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool V1_0::complextest::CCDMResultVarRecursive::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["dataModelTitle"] = QJsonValue::fromVariant(*dataModelTitle);

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["dataModelVersion"] = QJsonValue::fromVariant(*dataModelVersion);

	if (resultMetaData){
		QJsonObject resultMetaDataJsonObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToJsonObject(resultMetaDataJsonObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["resultMetaData"] = resultMetaDataJsonObject;
	}

	if (resultContent){
		QJsonArray newResultContentArray;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			if (!resultContent->at(resultContentIndex)){
				newResultContentArray << QJsonValue(QJsonValue::Null);
				continue;
			}
			if (const V1_0::complextest::CCDMResultVarString* cDMResultVarStringVal = std::get_if<V1_0::complextest::CCDMResultVarString>(resultContent->at(resultContentIndex).GetPtr())){
				QJsonObject resultContentJsonObject;
				const bool isresultContentAdded = cDMResultVarStringVal->WriteToJsonObject(resultContentJsonObject);
				if (!isresultContentAdded){
					return false;
				}
				resultContentJsonObject["__typename"] = "CDMResultVarString";
				newResultContentArray << resultContentJsonObject;
			}
			else if (const V1_0::complextest::CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<V1_0::complextest::CCDMResultVarRecursive>(resultContent->at(resultContentIndex).GetPtr())){
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
	else if (resultContent.IsNull()){
		jsonObject["resultContent"] = QJsonValue(QJsonValue::Null);
	}

	if (overallResultValues){
		QJsonArray newOverallResultValuesArray;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			if (!overallResultValues->at(overallResultValuesIndex)){
				newOverallResultValuesArray << QJsonValue(QJsonValue::Null);
				continue;
			}
			QJsonObject newOverallResultValuesJsonObject;
			if (!overallResultValues->at(overallResultValuesIndex)->WriteToJsonObject(newOverallResultValuesJsonObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			newOverallResultValuesArray << newOverallResultValuesJsonObject;
		}
		jsonObject["overallResultValues"] = newOverallResultValuesArray;
	}
	else if (overallResultValues.IsNull()){
		jsonObject["overallResultValues"] = QJsonValue(QJsonValue::Null);
	}

	jsonObject["__typename"] = "CDMResultVarRecursive";

	return true;
}


bool V1_0::complextest::CCDMResultVarRecursive::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("dataModelTitle") || ! jsonObject["dataModelTitle"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = jsonObject["dataModelTitle"].toString();

	if (!jsonObject.contains("dataModelVersion") || ! jsonObject["dataModelVersion"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = jsonObject["dataModelVersion"].toString();

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

if (jsonObject.contains("resultContent") && jsonObject["resultContent"].isNull()){
		resultContent.SetNull();
	}
		if (jsonObject.contains("resultContent")){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = imtsdl::TElementList<V1_0::complextest::CDMResultUnionType>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			const QVariant tempResultContent = resultContentJsonArray[resultContentIndex].toVariant();
			QString resultContentDataValueTypename = resultContentJsonArray[resultContentIndex].toObject().value("__typename").toString();
			if (resultContentJsonArray[resultContentIndex].isNull()){
				resultContent->AppendNull();
				continue;
			}
			V1_0::complextest::CDMResultUnionType resultContentDataValue;
			if (resultContentDataValueTypename == "CDMResultVarString") {
				V1_0::complextest::CCDMResultVarString resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else if (resultContentDataValueTypename == "CDMResultVarRecursive") {
				V1_0::complextest::CCDMResultVarRecursive resultContentDataValueConvert;
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

if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isNull()){
		overallResultValues.SetNull();
	}
		if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isArray()){
		const QJsonArray overallResultValuesJsonArray = jsonObject["overallResultValues"].toArray();
		const qsizetype overallResultValuesArrayCount = overallResultValuesJsonArray.size();
		overallResultValues = imtsdl::TElementList<V1_0::complextest::COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			if (overallResultValuesJsonArray[overallResultValuesIndex].isNull()){
				overallResultValues->AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool V1_0::complextest::CCDMResultVarRecursive::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("dataModelTitle") && jsonObject["dataModelTitle"].isString()){
		dataModelTitle = jsonObject["dataModelTitle"].toString();
	}

	if (jsonObject.contains("dataModelVersion") && jsonObject["dataModelVersion"].isString()){
		dataModelVersion = jsonObject["dataModelVersion"].toString();
	}

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

if (jsonObject.contains("resultContent") && jsonObject["resultContent"].isNull()){
		resultContent.SetNull();
	}
		if (jsonObject.contains("resultContent")){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = imtsdl::TElementList<V1_0::complextest::CDMResultUnionType>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			const QVariant tempResultContent = resultContentJsonArray[resultContentIndex].toVariant();
			QString resultContentDataValueTypename = resultContentJsonArray[resultContentIndex].toObject().value("__typename").toString();
			if (resultContentJsonArray[resultContentIndex].isNull()){
				resultContent->AppendNull();
				continue;
			}
			V1_0::complextest::CDMResultUnionType resultContentDataValue;
			if (resultContentDataValueTypename == "CDMResultVarString") {
				V1_0::complextest::CCDMResultVarString resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else if (resultContentDataValueTypename == "CDMResultVarRecursive") {
				V1_0::complextest::CCDMResultVarRecursive resultContentDataValueConvert;
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

if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isNull()){
		overallResultValues.SetNull();
	}
		if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isArray()){
		const QJsonArray overallResultValuesJsonArray = jsonObject["overallResultValues"].toArray();
		const qsizetype overallResultValuesArrayCount = overallResultValuesJsonArray.size();
		overallResultValues = imtsdl::TElementList<V1_0::complextest::COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			if (overallResultValuesJsonArray[overallResultValuesIndex].isNull()){
				overallResultValues->AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}




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


bool V1_0::complextest::CCDMResultVarString::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "CDMResultVarString", modelIndex);

	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("dataModelTitle", *dataModelTitle, modelIndex);

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("dataModelVersion", *dataModelVersion, modelIndex);


	if (resultMetaData){
		::imtbase::CTreeItemModel* resultMetaDataNewModelPtr = model.AddTreeModel("resultMetaData", modelIndex);
		const bool isResultMetaDataAdded = resultMetaData->WriteToModel(*resultMetaDataNewModelPtr, 0);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}

	}
	if (resultContent){
		::imtbase::CTreeItemModel* newResultContentModelPtr = model.AddTreeModel("resultContent", modelIndex);
		newResultContentModelPtr->setIsArray(true);
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			if (!resultContent->at(resultContentIndex)){
				newResultContentModelPtr->InsertNewItem();
				newResultContentModelPtr->SetData(QByteArray(), QVariant(), resultContentIndex);
				continue;
			}
			newResultContentModelPtr->InsertNewItem();
			newResultContentModelPtr->SetData(QByteArray(), *resultContent->at(resultContentIndex), resultContentIndex);
		}
	}
	else if (resultContent.IsNull()){
		model.SetData("resultContent", QVariant(), modelIndex);
	}

	if (overallResultValues){
		::imtbase::CTreeItemModel* newOverallResultValuesModelPtr = model.AddTreeModel("overallResultValues", modelIndex);
		newOverallResultValuesModelPtr->setIsArray(true);
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			if (!overallResultValues->at(overallResultValuesIndex)){
				newOverallResultValuesModelPtr->InsertNewItem();
				newOverallResultValuesModelPtr->SetData(QByteArray(), QVariant(), overallResultValuesIndex);
				continue;
			}
			newOverallResultValuesModelPtr->InsertNewItem();
			if (!(overallResultValues->at(overallResultValuesIndex)->WriteToModel(*newOverallResultValuesModelPtr, overallResultValuesIndex))){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
		}
	}
	else if (overallResultValues.IsNull()){
		model.SetData("overallResultValues", QVariant(), modelIndex);
	}

	return true;
}


bool V1_0::complextest::CCDMResultVarString::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant dataModelTitleData = model.GetData("dataModelTitle", modelIndex);
	if (dataModelTitleData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = dataModelTitleData.toString();

	QVariant dataModelVersionData = model.GetData("dataModelVersion", modelIndex);
	if (dataModelVersionData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = dataModelVersionData.toString();

	::imtbase::CTreeItemModel* resultMetaDataDataModelPtr = model.GetTreeItemModel("resultMetaData", modelIndex);
	if (resultMetaDataDataModelPtr != nullptr){
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (model.ContainsKey("resultContent", modelIndex) && resultContentModel == nullptr){
		resultContent.SetNull();
	}
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		imtsdl::TElementList<QString> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			if (!resultContentModel->GetData(QByteArray(), resultContentIndex).isValid()){
				resultContentList.AppendNull();
				continue;
			}
			QString t_resultContent = resultContentModel->GetData(QByteArray(), resultContentIndex).toString();
			resultContentList << t_resultContent;
		}
		resultContent = resultContentList;

	}

	::imtbase::CTreeItemModel* overallResultValuesModel = model.GetTreeItemModel("overallResultValues", modelIndex);
	if (model.ContainsKey("overallResultValues", modelIndex) && overallResultValuesModel == nullptr){
		overallResultValues.SetNull();
	}
	if (overallResultValuesModel != nullptr){
		int overallResultValuesCount = overallResultValuesModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::COverallResultValues> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			if (overallResultValuesModel->ContainsKey(QByteArray(), overallResultValuesIndex) && !overallResultValuesModel->GetData(QByteArray(), overallResultValuesIndex).isValid()){
				overallResultValuesList.AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues t_overallResultValues;
			if (!t_overallResultValues.ReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesList << t_overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool V1_0::complextest::CCDMResultVarString::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (model.ContainsKey("resultContent", modelIndex) && resultContentModel == nullptr){
		resultContent.SetNull();
	}
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		imtsdl::TElementList<QString> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			if (!resultContentModel->GetData(QByteArray(), resultContentIndex).isValid()){
				resultContentList.AppendNull();
				continue;
			}
			QString t_resultContent = resultContentModel->GetData(QByteArray(), resultContentIndex).toString();
			resultContentList << t_resultContent;
		}
		resultContent = resultContentList;

	}

	::imtbase::CTreeItemModel* overallResultValuesModel = model.GetTreeItemModel("overallResultValues", modelIndex);
	if (model.ContainsKey("overallResultValues", modelIndex) && overallResultValuesModel == nullptr){
		overallResultValues.SetNull();
	}
	if (overallResultValuesModel != nullptr){
		int overallResultValuesCount = overallResultValuesModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::COverallResultValues> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			if (overallResultValuesModel->ContainsKey(QByteArray(), overallResultValuesIndex) && !overallResultValuesModel->GetData(QByteArray(), overallResultValuesIndex).isValid()){
				overallResultValuesList.AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues t_overallResultValues;
			if (!t_overallResultValues.OptReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesList << t_overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool V1_0::complextest::CCDMResultVarString::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("dataModelTitle", QVariant(*dataModelTitle));

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("dataModelVersion", QVariant(*dataModelVersion));

	if (resultMetaData){
		::imtgql::CGqlParamObject resultMetaDataGqlObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToGraphQlObject(resultMetaDataGqlObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("resultMetaData", resultMetaDataGqlObject);
	}

	if (resultContent){
		QVariantList resultContentDataObjectList;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			if (!resultContent->at(resultContentIndex)){
				resultContentDataObjectList << QVariant();
				continue;
			}
			resultContentDataObjectList << *resultContent->at(resultContentIndex);
		}
		gqlObject.InsertParam("resultContent", resultContentDataObjectList);
	}
	else if (resultContent.IsNull()){
		gqlObject.InsertParam("resultContent", QVariant());
	}

	if (overallResultValues){
		QList<::imtgql::CGqlParamObject> overallResultValuesDataObjectList;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			if (!overallResultValues->at(overallResultValuesIndex)){
				overallResultValuesDataObjectList << ::imtgql::CGqlParamObject::CreateNull();
				continue;
			}
			::imtgql::CGqlParamObject newOverallResultValuesGqlObject;
			if (!overallResultValues->at(overallResultValuesIndex)->WriteToGraphQlObject(newOverallResultValuesGqlObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesDataObjectList << newOverallResultValuesGqlObject;
		}
		gqlObject.InsertParam("overallResultValues", overallResultValuesDataObjectList);
	}
	else if (overallResultValues.IsNull()){
		gqlObject.InsertParam("overallResultValues", QVariant());
	}

	gqlObject.InsertParam("__typename", QVariant("CDMResultVarString"));

	return true;
}


bool V1_0::complextest::CCDMResultVarString::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("dataModelTitle") || (gqlObject["dataModelTitle"].userType() != QMetaType::QString && gqlObject["dataModelTitle"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = gqlObject["dataModelTitle"].toString();

	if (!gqlObject.ContainsParam("dataModelVersion") || (gqlObject["dataModelVersion"].userType() != QMetaType::QString && gqlObject["dataModelVersion"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = gqlObject["dataModelVersion"].toString();

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent") && gqlObject["resultContent"].isNull()){
		resultContent.SetNull();
	}
	if (gqlObject.ContainsParam("resultContent") && !(gqlObject["resultContent"].isNull())){
		const QVariant resultContentData = gqlObject["resultContent"];
		const QVariantList resultContentDataList = resultContentData.toList();
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = imtsdl::TElementList<QString>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			if (!resultContentDataList[resultContentIndex].isValid() || resultContentDataList[resultContentIndex].isNull()){
				resultContent->AppendNull();
				continue;
			}
			QString tempResultContent = resultContentDataList[resultContentIndex].toString();
			resultContent->append(tempResultContent);
		}
	}

	if (gqlObject.ContainsParam("overallResultValues") && gqlObject["overallResultValues"].isNull()){
		overallResultValues.SetNull();
	}
	if (gqlObject.ContainsParam("overallResultValues") && !(gqlObject["overallResultValues"].isNull())){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = imtsdl::TElementList<V1_0::complextest::COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr || overallResultValuesDataObjectPtr->IsNull()){
				overallResultValues->AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool V1_0::complextest::CCDMResultVarString::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("dataModelTitle") && (gqlObject["dataModelTitle"].userType() == QMetaType::QString || gqlObject["dataModelTitle"].userType() == QMetaType::QByteArray)){
		dataModelTitle = gqlObject["dataModelTitle"].toString();
	}

	if (gqlObject.ContainsParam("dataModelVersion") && (gqlObject["dataModelVersion"].userType() == QMetaType::QString || gqlObject["dataModelVersion"].userType() == QMetaType::QByteArray)){
		dataModelVersion = gqlObject["dataModelVersion"].toString();
	}

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent") && gqlObject["resultContent"].isNull()){
		resultContent.SetNull();
	}
	if (gqlObject.ContainsParam("resultContent") && !(gqlObject["resultContent"].isNull())){
		const QVariant resultContentData = gqlObject["resultContent"];
		const QVariantList resultContentDataList = resultContentData.toList();
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = imtsdl::TElementList<QString>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			if (!resultContentDataList[resultContentIndex].isValid() || resultContentDataList[resultContentIndex].isNull()){
				resultContent->AppendNull();
				continue;
			}
			QString tempResultContent = resultContentDataList[resultContentIndex].toString();
			resultContent->append(tempResultContent);
		}
	}

	if (gqlObject.ContainsParam("overallResultValues") && gqlObject["overallResultValues"].isNull()){
		overallResultValues.SetNull();
	}
	if (gqlObject.ContainsParam("overallResultValues") && !(gqlObject["overallResultValues"].isNull())){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = imtsdl::TElementList<V1_0::complextest::COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr || overallResultValuesDataObjectPtr->IsNull()){
				overallResultValues->AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool V1_0::complextest::CCDMResultVarString::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["dataModelTitle"] = QJsonValue::fromVariant(*dataModelTitle);

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["dataModelVersion"] = QJsonValue::fromVariant(*dataModelVersion);

	if (resultMetaData){
		QJsonObject resultMetaDataJsonObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToJsonObject(resultMetaDataJsonObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["resultMetaData"] = resultMetaDataJsonObject;
	}

	if (resultContent){
		QJsonArray newResultContentArray;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			if (!resultContent->at(resultContentIndex)){
				newResultContentArray << QJsonValue(QJsonValue::Null);
				continue;
			}
			newResultContentArray << *resultContent->at(resultContentIndex);
		}
		jsonObject["resultContent"] = newResultContentArray;
	}
	else if (resultContent.IsNull()){
		jsonObject["resultContent"] = QJsonValue(QJsonValue::Null);
	}

	if (overallResultValues){
		QJsonArray newOverallResultValuesArray;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			if (!overallResultValues->at(overallResultValuesIndex)){
				newOverallResultValuesArray << QJsonValue(QJsonValue::Null);
				continue;
			}
			QJsonObject newOverallResultValuesJsonObject;
			if (!overallResultValues->at(overallResultValuesIndex)->WriteToJsonObject(newOverallResultValuesJsonObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			newOverallResultValuesArray << newOverallResultValuesJsonObject;
		}
		jsonObject["overallResultValues"] = newOverallResultValuesArray;
	}
	else if (overallResultValues.IsNull()){
		jsonObject["overallResultValues"] = QJsonValue(QJsonValue::Null);
	}

	jsonObject["__typename"] = "CDMResultVarString";

	return true;
}


bool V1_0::complextest::CCDMResultVarString::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("dataModelTitle") || ! jsonObject["dataModelTitle"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = jsonObject["dataModelTitle"].toString();

	if (!jsonObject.contains("dataModelVersion") || ! jsonObject["dataModelVersion"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = jsonObject["dataModelVersion"].toString();

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

if (jsonObject.contains("resultContent") && jsonObject["resultContent"].isNull()){
		resultContent.SetNull();
	}
		if (jsonObject.contains("resultContent") && jsonObject["resultContent"].isArray()){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = imtsdl::TElementList<QString>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			if (resultContentJsonArray[resultContentIndex].isNull()){
				resultContent->AppendNull();
				continue;
			}
			QString tempResultContent = resultContentJsonArray[resultContentIndex].toString();
			resultContent->append(tempResultContent);
		}
	}

if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isNull()){
		overallResultValues.SetNull();
	}
		if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isArray()){
		const QJsonArray overallResultValuesJsonArray = jsonObject["overallResultValues"].toArray();
		const qsizetype overallResultValuesArrayCount = overallResultValuesJsonArray.size();
		overallResultValues = imtsdl::TElementList<V1_0::complextest::COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			if (overallResultValuesJsonArray[overallResultValuesIndex].isNull()){
				overallResultValues->AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool V1_0::complextest::CCDMResultVarString::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("dataModelTitle") && jsonObject["dataModelTitle"].isString()){
		dataModelTitle = jsonObject["dataModelTitle"].toString();
	}

	if (jsonObject.contains("dataModelVersion") && jsonObject["dataModelVersion"].isString()){
		dataModelVersion = jsonObject["dataModelVersion"].toString();
	}

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

if (jsonObject.contains("resultContent") && jsonObject["resultContent"].isNull()){
		resultContent.SetNull();
	}
		if (jsonObject.contains("resultContent") && jsonObject["resultContent"].isArray()){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = imtsdl::TElementList<QString>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			if (resultContentJsonArray[resultContentIndex].isNull()){
				resultContent->AppendNull();
				continue;
			}
			QString tempResultContent = resultContentJsonArray[resultContentIndex].toString();
			resultContent->append(tempResultContent);
		}
	}

if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isNull()){
		overallResultValues.SetNull();
	}
		if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isArray()){
		const QJsonArray overallResultValuesJsonArray = jsonObject["overallResultValues"].toArray();
		const qsizetype overallResultValuesArrayCount = overallResultValuesJsonArray.size();
		overallResultValues = imtsdl::TElementList<V1_0::complextest::COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			if (overallResultValuesJsonArray[overallResultValuesIndex].isNull()){
				overallResultValues->AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}




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


bool V1_0::complextest::CCDMResult::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "CDMResult", modelIndex);

	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("dataModelTitle", *dataModelTitle, modelIndex);

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("dataModelVersion", *dataModelVersion, modelIndex);


	if (resultMetaData){
		::imtbase::CTreeItemModel* resultMetaDataNewModelPtr = model.AddTreeModel("resultMetaData", modelIndex);
		const bool isResultMetaDataAdded = resultMetaData->WriteToModel(*resultMetaDataNewModelPtr, 0);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}

	}
	if (resultContent){
		::imtbase::CTreeItemModel* newResultContentModelPtr = model.AddTreeModel("resultContent", modelIndex);
		newResultContentModelPtr->setIsArray(true);
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			if (!resultContent->at(resultContentIndex)){
				newResultContentModelPtr->InsertNewItem();
				newResultContentModelPtr->SetData(QByteArray(), QVariant(), resultContentIndex);
				continue;
			}
			QVariant resultContentVariantValue;
			if (const V1_0::complextest::CCDMResultVarString* cDMResultVarStringVal = std::get_if<V1_0::complextest::CCDMResultVarString>(resultContent->at(resultContentIndex).GetPtr())){
				newResultContentModelPtr->InsertNewItem();
				if(!cDMResultVarStringVal->WriteToModel(*newResultContentModelPtr, resultContentIndex)){
					return false;
				}
				newResultContentModelPtr->SetData("__typename", "CDMResultVarString", resultContentIndex);
			}
			else if (const V1_0::complextest::CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<V1_0::complextest::CCDMResultVarRecursive>(resultContent->at(resultContentIndex).GetPtr())){
				newResultContentModelPtr->InsertNewItem();
				if(!cDMResultVarRecursiveVal->WriteToModel(*newResultContentModelPtr, resultContentIndex)){
					return false;
				}
				newResultContentModelPtr->SetData("__typename", "CDMResultVarRecursive", resultContentIndex);
			}
		}
	}
	else if (resultContent.IsNull()){
		model.SetData("resultContent", QVariant(), modelIndex);
	}

	if (overallResultValues){
		::imtbase::CTreeItemModel* newOverallResultValuesModelPtr = model.AddTreeModel("overallResultValues", modelIndex);
		newOverallResultValuesModelPtr->setIsArray(true);
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			if (!overallResultValues->at(overallResultValuesIndex)){
				newOverallResultValuesModelPtr->InsertNewItem();
				newOverallResultValuesModelPtr->SetData(QByteArray(), QVariant(), overallResultValuesIndex);
				continue;
			}
			newOverallResultValuesModelPtr->InsertNewItem();
			if (!(overallResultValues->at(overallResultValuesIndex)->WriteToModel(*newOverallResultValuesModelPtr, overallResultValuesIndex))){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
		}
	}
	else if (overallResultValues.IsNull()){
		model.SetData("overallResultValues", QVariant(), modelIndex);
	}

	return true;
}


bool V1_0::complextest::CCDMResult::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant dataModelTitleData = model.GetData("dataModelTitle", modelIndex);
	if (dataModelTitleData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = dataModelTitleData.toString();

	QVariant dataModelVersionData = model.GetData("dataModelVersion", modelIndex);
	if (dataModelVersionData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = dataModelVersionData.toString();

	::imtbase::CTreeItemModel* resultMetaDataDataModelPtr = model.GetTreeItemModel("resultMetaData", modelIndex);
	if (resultMetaDataDataModelPtr != nullptr){
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (model.ContainsKey("resultContent", modelIndex) && resultContentModel == nullptr){
		resultContent.SetNull();
	}
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::CDMResultUnionType> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			if (resultContentModel->ContainsKey(QByteArray(), resultContentIndex) && !resultContentModel->GetData(QByteArray(), resultContentIndex).isValid()){
				resultContentList.AppendNull();
				continue;
			}
			V1_0::complextest::CDMResultUnionType resultContentData;
			QVariant resultContentVariantValue = resultContentModel->GetData(QByteArray(), resultContentIndex);
			QString resultContentDataTypename = resultContentModel->GetData("__typename", resultContentIndex).toString();
			if (resultContentDataTypename == "CDMResultVarString") {
				V1_0::complextest::CCDMResultVarString resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = CDMResultUnionType(resultContentDataConvert);
			}
			else if (resultContentDataTypename == "CDMResultVarRecursive") {
				V1_0::complextest::CCDMResultVarRecursive resultContentDataConvert;
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
	if (model.ContainsKey("overallResultValues", modelIndex) && overallResultValuesModel == nullptr){
		overallResultValues.SetNull();
	}
	if (overallResultValuesModel != nullptr){
		int overallResultValuesCount = overallResultValuesModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::COverallResultValues> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			if (overallResultValuesModel->ContainsKey(QByteArray(), overallResultValuesIndex) && !overallResultValuesModel->GetData(QByteArray(), overallResultValuesIndex).isValid()){
				overallResultValuesList.AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues t_overallResultValues;
			if (!t_overallResultValues.ReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesList << t_overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool V1_0::complextest::CCDMResult::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromModel(*resultMetaDataDataModelPtr, modelIndex);
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	::imtbase::CTreeItemModel* resultContentModel = model.GetTreeItemModel("resultContent", modelIndex);
	if (model.ContainsKey("resultContent", modelIndex) && resultContentModel == nullptr){
		resultContent.SetNull();
	}
	if (resultContentModel != nullptr){
		int resultContentCount = resultContentModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::CDMResultUnionType> resultContentList;
		for (int resultContentIndex = 0; resultContentIndex < resultContentCount; ++resultContentIndex){
			if (resultContentModel->ContainsKey(QByteArray(), resultContentIndex) && !resultContentModel->GetData(QByteArray(), resultContentIndex).isValid()){
				resultContentList.AppendNull();
				continue;
			}
			V1_0::complextest::CDMResultUnionType resultContentData;
			QVariant resultContentVariantValue = resultContentModel->GetData(QByteArray(), resultContentIndex);
			QString resultContentDataTypename = resultContentModel->GetData("__typename", resultContentIndex).toString();
			if (resultContentDataTypename == "CDMResultVarString") {
				V1_0::complextest::CCDMResultVarString resultContentDataConvert;
				const bool isresultContentDataRead = resultContentDataConvert.ReadFromModel(*model.GetTreeItemModel("resultContent", resultContentIndex)); 
				if (!isresultContentDataRead){
					return false;
				}
				resultContentData = CDMResultUnionType(resultContentDataConvert);
			}
			else if (resultContentDataTypename == "CDMResultVarRecursive") {
				V1_0::complextest::CCDMResultVarRecursive resultContentDataConvert;
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
	if (model.ContainsKey("overallResultValues", modelIndex) && overallResultValuesModel == nullptr){
		overallResultValues.SetNull();
	}
	if (overallResultValuesModel != nullptr){
		int overallResultValuesCount = overallResultValuesModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::COverallResultValues> overallResultValuesList;
		for (int overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesCount; ++overallResultValuesIndex){
			if (overallResultValuesModel->ContainsKey(QByteArray(), overallResultValuesIndex) && !overallResultValuesModel->GetData(QByteArray(), overallResultValuesIndex).isValid()){
				overallResultValuesList.AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues t_overallResultValues;
			if (!t_overallResultValues.OptReadFromModel(*overallResultValuesModel, overallResultValuesIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesList << t_overallResultValues;
		}
		overallResultValues = overallResultValuesList;

	}

	return true;
}


bool V1_0::complextest::CCDMResult::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("dataModelTitle", QVariant(*dataModelTitle));

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("dataModelVersion", QVariant(*dataModelVersion));

	if (resultMetaData){
		::imtgql::CGqlParamObject resultMetaDataGqlObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToGraphQlObject(resultMetaDataGqlObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("resultMetaData", resultMetaDataGqlObject);
	}

	if (resultContent){
		QList<::imtgql::CGqlParamObject> resultContentDataObjectList;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			if (!resultContent->at(resultContentIndex)){
				resultContentDataObjectList << ::imtgql::CGqlParamObject::CreateNull();
				continue;
			}
			::imtgql::CGqlParamObject resultContentDataObject;
			if (const V1_0::complextest::CCDMResultVarString* cDMResultVarStringVal = std::get_if<V1_0::complextest::CCDMResultVarString>(resultContent->at(resultContentIndex).GetPtr())){
				if (!cDMResultVarStringVal->WriteToGraphQlObject(resultContentDataObject)){
					return false;
				}
				resultContentDataObject.InsertParam("__typename", QVariant("CDMResultVarString"));
			}
			else if (const V1_0::complextest::CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<V1_0::complextest::CCDMResultVarRecursive>(resultContent->at(resultContentIndex).GetPtr())){
				if (!cDMResultVarRecursiveVal->WriteToGraphQlObject(resultContentDataObject)){
					return false;
				}
				resultContentDataObject.InsertParam("__typename", QVariant("CDMResultVarRecursive"));
			}
			resultContentDataObjectList << resultContentDataObject;
		}
		gqlObject.InsertParam("resultContent", resultContentDataObjectList);
	}
	else if (resultContent.IsNull()){
		gqlObject.InsertParam("resultContent", QVariant());
	}

	if (overallResultValues){
		QList<::imtgql::CGqlParamObject> overallResultValuesDataObjectList;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			if (!overallResultValues->at(overallResultValuesIndex)){
				overallResultValuesDataObjectList << ::imtgql::CGqlParamObject::CreateNull();
				continue;
			}
			::imtgql::CGqlParamObject newOverallResultValuesGqlObject;
			if (!overallResultValues->at(overallResultValuesIndex)->WriteToGraphQlObject(newOverallResultValuesGqlObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValuesDataObjectList << newOverallResultValuesGqlObject;
		}
		gqlObject.InsertParam("overallResultValues", overallResultValuesDataObjectList);
	}
	else if (overallResultValues.IsNull()){
		gqlObject.InsertParam("overallResultValues", QVariant());
	}

	gqlObject.InsertParam("__typename", QVariant("CDMResult"));

	return true;
}


bool V1_0::complextest::CCDMResult::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("dataModelTitle") || (gqlObject["dataModelTitle"].userType() != QMetaType::QString && gqlObject["dataModelTitle"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = gqlObject["dataModelTitle"].toString();

	if (!gqlObject.ContainsParam("dataModelVersion") || (gqlObject["dataModelVersion"].userType() != QMetaType::QString && gqlObject["dataModelVersion"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = gqlObject["dataModelVersion"].toString();

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent") && gqlObject["resultContent"].isNull()){
		resultContent.SetNull();
	}
	if (gqlObject.ContainsParam("resultContent") && !(gqlObject["resultContent"].isNull())){
		const QList<const ::imtgql::CGqlParamObject*> resultContentDataList = gqlObject.GetParamArgumentObjectPtrList("resultContent");
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = imtsdl::TElementList<V1_0::complextest::CDMResultUnionType>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			const ::imtgql::CGqlParamObject* tempResultContent = resultContentDataList[resultContentIndex];
			QString resultContentDataValueTypename = (tempResultContent == nullptr || tempResultContent->IsNull()) ? QString() : tempResultContent->GetParamArgumentValue("__typename").toString();
			if (resultContentDataList[resultContentIndex] == nullptr || resultContentDataList[resultContentIndex]->IsNull()){
				resultContent->AppendNull();
				continue;
			}
			V1_0::complextest::CDMResultUnionType resultContentDataValue;
			if (resultContentDataValueTypename == "CDMResultVarString") {
				V1_0::complextest::CCDMResultVarString resultContentDataValueConvert;
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*tempResultContent);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else if (resultContentDataValueTypename == "CDMResultVarRecursive") {
				V1_0::complextest::CCDMResultVarRecursive resultContentDataValueConvert;
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

	if (gqlObject.ContainsParam("overallResultValues") && gqlObject["overallResultValues"].isNull()){
		overallResultValues.SetNull();
	}
	if (gqlObject.ContainsParam("overallResultValues") && !(gqlObject["overallResultValues"].isNull())){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = imtsdl::TElementList<V1_0::complextest::COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr || overallResultValuesDataObjectPtr->IsNull()){
				overallResultValues->AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool V1_0::complextest::CCDMResult::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("dataModelTitle") && (gqlObject["dataModelTitle"].userType() == QMetaType::QString || gqlObject["dataModelTitle"].userType() == QMetaType::QByteArray)){
		dataModelTitle = gqlObject["dataModelTitle"].toString();
	}

	if (gqlObject.ContainsParam("dataModelVersion") && (gqlObject["dataModelVersion"].userType() == QMetaType::QString || gqlObject["dataModelVersion"].userType() == QMetaType::QByteArray)){
		dataModelVersion = gqlObject["dataModelVersion"].toString();
	}

	if (gqlObject.ContainsParam("resultMetaData") && (gqlObject.GetParamArgumentObjectPtr("resultMetaData") != nullptr)){
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("resultMetaData"));
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("resultContent") && gqlObject["resultContent"].isNull()){
		resultContent.SetNull();
	}
	if (gqlObject.ContainsParam("resultContent") && !(gqlObject["resultContent"].isNull())){
		const QList<const ::imtgql::CGqlParamObject*> resultContentDataList = gqlObject.GetParamArgumentObjectPtrList("resultContent");
		const qsizetype resultContentElementsCount = resultContentDataList.size();
		resultContent = imtsdl::TElementList<V1_0::complextest::CDMResultUnionType>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentElementsCount; ++resultContentIndex){
			const ::imtgql::CGqlParamObject* tempResultContent = resultContentDataList[resultContentIndex];
			QString resultContentDataValueTypename = (tempResultContent == nullptr || tempResultContent->IsNull()) ? QString() : tempResultContent->GetParamArgumentValue("__typename").toString();
			if (resultContentDataList[resultContentIndex] == nullptr || resultContentDataList[resultContentIndex]->IsNull()){
				resultContent->AppendNull();
				continue;
			}
			V1_0::complextest::CDMResultUnionType resultContentDataValue;
			if (resultContentDataValueTypename == "CDMResultVarString") {
				V1_0::complextest::CCDMResultVarString resultContentDataValueConvert;
				const bool isResultContentRead = resultContentDataValueConvert.ReadFromGraphQlObject(*tempResultContent);
				if (!isResultContentRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else if (resultContentDataValueTypename == "CDMResultVarRecursive") {
				V1_0::complextest::CCDMResultVarRecursive resultContentDataValueConvert;
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

	if (gqlObject.ContainsParam("overallResultValues") && gqlObject["overallResultValues"].isNull()){
		overallResultValues.SetNull();
	}
	if (gqlObject.ContainsParam("overallResultValues") && !(gqlObject["overallResultValues"].isNull())){
		const qsizetype overallResultValuesElementsCount = gqlObject.GetObjectsCount("overallResultValues");
		overallResultValues = imtsdl::TElementList<V1_0::complextest::COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesElementsCount; ++overallResultValuesIndex){
			const ::imtgql::CGqlParamObject* overallResultValuesDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("overallResultValues", overallResultValuesIndex);
			if (overallResultValuesDataObjectPtr == nullptr || overallResultValuesDataObjectPtr->IsNull()){
				overallResultValues->AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromGraphQlObject(*overallResultValuesDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool V1_0::complextest::CCDMResult::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!dataModelTitle){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["dataModelTitle"] = QJsonValue::fromVariant(*dataModelTitle);

	if (!dataModelVersion){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["dataModelVersion"] = QJsonValue::fromVariant(*dataModelVersion);

	if (resultMetaData){
		QJsonObject resultMetaDataJsonObject;
		const bool isResultMetaDataAdded = resultMetaData->WriteToJsonObject(resultMetaDataJsonObject);
		if (!isResultMetaDataAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["resultMetaData"] = resultMetaDataJsonObject;
	}

	if (resultContent){
		QJsonArray newResultContentArray;
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContent->size(); ++resultContentIndex){
			if (!resultContent->at(resultContentIndex)){
				newResultContentArray << QJsonValue(QJsonValue::Null);
				continue;
			}
			if (const V1_0::complextest::CCDMResultVarString* cDMResultVarStringVal = std::get_if<V1_0::complextest::CCDMResultVarString>(resultContent->at(resultContentIndex).GetPtr())){
				QJsonObject resultContentJsonObject;
				const bool isresultContentAdded = cDMResultVarStringVal->WriteToJsonObject(resultContentJsonObject);
				if (!isresultContentAdded){
					return false;
				}
				resultContentJsonObject["__typename"] = "CDMResultVarString";
				newResultContentArray << resultContentJsonObject;
			}
			else if (const V1_0::complextest::CCDMResultVarRecursive* cDMResultVarRecursiveVal = std::get_if<V1_0::complextest::CCDMResultVarRecursive>(resultContent->at(resultContentIndex).GetPtr())){
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
	else if (resultContent.IsNull()){
		jsonObject["resultContent"] = QJsonValue(QJsonValue::Null);
	}

	if (overallResultValues){
		QJsonArray newOverallResultValuesArray;
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValues->size(); ++overallResultValuesIndex){
			if (!overallResultValues->at(overallResultValuesIndex)){
				newOverallResultValuesArray << QJsonValue(QJsonValue::Null);
				continue;
			}
			QJsonObject newOverallResultValuesJsonObject;
			if (!overallResultValues->at(overallResultValuesIndex)->WriteToJsonObject(newOverallResultValuesJsonObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			newOverallResultValuesArray << newOverallResultValuesJsonObject;
		}
		jsonObject["overallResultValues"] = newOverallResultValuesArray;
	}
	else if (overallResultValues.IsNull()){
		jsonObject["overallResultValues"] = QJsonValue(QJsonValue::Null);
	}

	jsonObject["__typename"] = "CDMResult";

	return true;
}


bool V1_0::complextest::CCDMResult::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("dataModelTitle") || ! jsonObject["dataModelTitle"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelTitle").toLocal8Bit().constData();)

		return false;
	}
	dataModelTitle = jsonObject["dataModelTitle"].toString();

	if (!jsonObject.contains("dataModelVersion") || ! jsonObject["dataModelVersion"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "dataModelVersion").toLocal8Bit().constData();)

		return false;
	}
	dataModelVersion = jsonObject["dataModelVersion"].toString();

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->ReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

if (jsonObject.contains("resultContent") && jsonObject["resultContent"].isNull()){
		resultContent.SetNull();
	}
		if (jsonObject.contains("resultContent")){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = imtsdl::TElementList<V1_0::complextest::CDMResultUnionType>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			const QVariant tempResultContent = resultContentJsonArray[resultContentIndex].toVariant();
			QString resultContentDataValueTypename = resultContentJsonArray[resultContentIndex].toObject().value("__typename").toString();
			if (resultContentJsonArray[resultContentIndex].isNull()){
				resultContent->AppendNull();
				continue;
			}
			V1_0::complextest::CDMResultUnionType resultContentDataValue;
			if (resultContentDataValueTypename == "CDMResultVarString") {
				V1_0::complextest::CCDMResultVarString resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else if (resultContentDataValueTypename == "CDMResultVarRecursive") {
				V1_0::complextest::CCDMResultVarRecursive resultContentDataValueConvert;
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

if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isNull()){
		overallResultValues.SetNull();
	}
		if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isArray()){
		const QJsonArray overallResultValuesJsonArray = jsonObject["overallResultValues"].toArray();
		const qsizetype overallResultValuesArrayCount = overallResultValuesJsonArray.size();
		overallResultValues = imtsdl::TElementList<V1_0::complextest::COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			if (overallResultValuesJsonArray[overallResultValuesIndex].isNull()){
				overallResultValues->AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.ReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}


bool V1_0::complextest::CCDMResult::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("dataModelTitle") && jsonObject["dataModelTitle"].isString()){
		dataModelTitle = jsonObject["dataModelTitle"].toString();
	}

	if (jsonObject.contains("dataModelVersion") && jsonObject["dataModelVersion"].isString()){
		dataModelVersion = jsonObject["dataModelVersion"].toString();
	}

	if (jsonObject.contains("resultMetaData") && jsonObject["resultMetaData"].isObject()){
		resultMetaData = V1_0::complextest::CResultMetaData();
		const bool isResultMetaDataRead = resultMetaData->OptReadFromJsonObject(jsonObject["resultMetaData"].toObject());
		if (!isResultMetaDataRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "resultMetaData").toLocal8Bit().constData();)

			return false;
		}
	}

if (jsonObject.contains("resultContent") && jsonObject["resultContent"].isNull()){
		resultContent.SetNull();
	}
		if (jsonObject.contains("resultContent")){
		const QJsonArray resultContentJsonArray = jsonObject["resultContent"].toArray();
		const qsizetype resultContentArrayCount = resultContentJsonArray.size();
		resultContent = imtsdl::TElementList<V1_0::complextest::CDMResultUnionType>();
		for (qsizetype resultContentIndex = 0; resultContentIndex < resultContentArrayCount; ++resultContentIndex){
			const QVariant tempResultContent = resultContentJsonArray[resultContentIndex].toVariant();
			QString resultContentDataValueTypename = resultContentJsonArray[resultContentIndex].toObject().value("__typename").toString();
			if (resultContentJsonArray[resultContentIndex].isNull()){
				resultContent->AppendNull();
				continue;
			}
			V1_0::complextest::CDMResultUnionType resultContentDataValue;
			if (resultContentDataValueTypename == "CDMResultVarString") {
				V1_0::complextest::CCDMResultVarString resultContentDataValueConvert;
				const bool isresultContentDataValueRead = resultContentDataValueConvert.ReadFromJsonObject(resultContentJsonArray[resultContentIndex].toObject());
				if (!isresultContentDataValueRead){
					return false;
				}
				resultContentDataValue = CDMResultUnionType(resultContentDataValueConvert);
			}
			else if (resultContentDataValueTypename == "CDMResultVarRecursive") {
				V1_0::complextest::CCDMResultVarRecursive resultContentDataValueConvert;
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

if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isNull()){
		overallResultValues.SetNull();
	}
		if (jsonObject.contains("overallResultValues") && jsonObject["overallResultValues"].isArray()){
		const QJsonArray overallResultValuesJsonArray = jsonObject["overallResultValues"].toArray();
		const qsizetype overallResultValuesArrayCount = overallResultValuesJsonArray.size();
		overallResultValues = imtsdl::TElementList<V1_0::complextest::COverallResultValues>();
		for (qsizetype overallResultValuesIndex = 0; overallResultValuesIndex < overallResultValuesArrayCount; ++overallResultValuesIndex){
			if (overallResultValuesJsonArray[overallResultValuesIndex].isNull()){
				overallResultValues->AppendNull();
				continue;
			}
			V1_0::complextest::COverallResultValues tempOverallResultValues;
			if (!tempOverallResultValues.OptReadFromJsonObject(overallResultValuesJsonArray[overallResultValuesIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "overallResultValues").toLocal8Bit().constData();)

				return false;
			}
			overallResultValues->append(tempOverallResultValues);
		}
	}

	return true;
}




QByteArray CPointsInCoordinateFormat::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPointsInCoordinateFormat::operator==(const CPointsInCoordinateFormat& other) const
{
	return 
				pointAsCoordinates == other.pointAsCoordinates;
}


bool V1_0::complextest::CPointsInCoordinateFormat::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "PointsInCoordinateFormat", modelIndex);

	if (pointAsCoordinates){
		model.SetData("pointAsCoordinates", *pointAsCoordinates, modelIndex);
	}


	return true;
}


bool V1_0::complextest::CPointsInCoordinateFormat::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant pointAsCoordinatesData = model.GetData("pointAsCoordinates", modelIndex);
	if (!pointAsCoordinatesData.isNull()){
		pointAsCoordinates = pointAsCoordinatesData.toBool();
	}

	return true;
}


bool V1_0::complextest::CPointsInCoordinateFormat::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant pointAsCoordinatesData = model.GetData("pointAsCoordinates", modelIndex);
	if (!pointAsCoordinatesData.isNull()){
		pointAsCoordinates = pointAsCoordinatesData.toBool();
	}

	return true;
}


bool V1_0::complextest::CPointsInCoordinateFormat::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (pointAsCoordinates){
		gqlObject.InsertParam("pointAsCoordinates", QVariant(*pointAsCoordinates));
	}

	gqlObject.InsertParam("__typename", QVariant("PointsInCoordinateFormat"));

	return true;
}


bool V1_0::complextest::CPointsInCoordinateFormat::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("pointAsCoordinates") && (gqlObject["pointAsCoordinates"].userType() == QMetaType::Bool)){
		pointAsCoordinates = gqlObject["pointAsCoordinates"].toBool();
	}

	return true;
}


bool V1_0::complextest::CPointsInCoordinateFormat::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("pointAsCoordinates") && (gqlObject["pointAsCoordinates"].userType() == QMetaType::Bool)){
		pointAsCoordinates = gqlObject["pointAsCoordinates"].toBool();
	}

	return true;
}


bool V1_0::complextest::CPointsInCoordinateFormat::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (pointAsCoordinates){
		jsonObject["pointAsCoordinates"] = QJsonValue::fromVariant(*pointAsCoordinates);
	}

	jsonObject["__typename"] = "PointsInCoordinateFormat";

	return true;
}


bool V1_0::complextest::CPointsInCoordinateFormat::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("pointAsCoordinates") && jsonObject["pointAsCoordinates"].isBool()){
		pointAsCoordinates = jsonObject["pointAsCoordinates"].toBool();
	}

	return true;
}


bool V1_0::complextest::CPointsInCoordinateFormat::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("pointAsCoordinates") && jsonObject["pointAsCoordinates"].isBool()){
		pointAsCoordinates = jsonObject["pointAsCoordinates"].toBool();
	}

	return true;
}




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


bool V1_0::complextest::CCollectionViewParamsTest::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}

	}

	return true;
}


bool V1_0::complextest::CCollectionViewParamsTest::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
		filterModel = V1_0::imtbase::CComplexCollectionFilter();
		const bool isFilterModelRead = filterModel->ReadFromModel(*filterModelDataModelPtr, modelIndex);
		if (!isFilterModelRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool V1_0::complextest::CCollectionViewParamsTest::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
		filterModel = V1_0::imtbase::CComplexCollectionFilter();
		const bool isFilterModelRead = filterModel->ReadFromModel(*filterModelDataModelPtr, modelIndex);
		if (!isFilterModelRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool V1_0::complextest::CCollectionViewParamsTest::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("filterModel", filterModelGqlObject);
	}

	gqlObject.InsertParam("__typename", QVariant("CollectionViewParamsTest"));

	return true;
}


bool V1_0::complextest::CCollectionViewParamsTest::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("count") && (gqlObject["count"].userType() == QMetaType::Int || gqlObject["count"].userType() == QMetaType::UInt || gqlObject["count"].userType() == QMetaType::LongLong || gqlObject["count"].userType() == QMetaType::ULongLong || gqlObject["count"].userType() == QMetaType::Long || gqlObject["count"].userType() == QMetaType::Short || gqlObject["count"].userType() == QMetaType::ULong || gqlObject["count"].userType() == QMetaType::UShort || gqlObject["count"].userType() == QMetaType::UChar)){
		count = gqlObject["count"].toInt();
	}

	if (gqlObject.ContainsParam("offset") && (gqlObject["offset"].userType() == QMetaType::Int || gqlObject["offset"].userType() == QMetaType::UInt || gqlObject["offset"].userType() == QMetaType::LongLong || gqlObject["offset"].userType() == QMetaType::ULongLong || gqlObject["offset"].userType() == QMetaType::Long || gqlObject["offset"].userType() == QMetaType::Short || gqlObject["offset"].userType() == QMetaType::ULong || gqlObject["offset"].userType() == QMetaType::UShort || gqlObject["offset"].userType() == QMetaType::UChar)){
		offset = gqlObject["offset"].toInt();
	}

	if (gqlObject.ContainsParam("filterModel") && (gqlObject.GetParamArgumentObjectPtr("filterModel") != nullptr)){
		filterModel = V1_0::imtbase::CComplexCollectionFilter();
		const bool isFilterModelRead = filterModel->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("filterModel"));
		if (!isFilterModelRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool V1_0::complextest::CCollectionViewParamsTest::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("count") && (gqlObject["count"].userType() == QMetaType::Int || gqlObject["count"].userType() == QMetaType::UInt || gqlObject["count"].userType() == QMetaType::LongLong || gqlObject["count"].userType() == QMetaType::ULongLong || gqlObject["count"].userType() == QMetaType::Long || gqlObject["count"].userType() == QMetaType::Short || gqlObject["count"].userType() == QMetaType::ULong || gqlObject["count"].userType() == QMetaType::UShort || gqlObject["count"].userType() == QMetaType::UChar)){
		count = gqlObject["count"].toInt();
	}

	if (gqlObject.ContainsParam("offset") && (gqlObject["offset"].userType() == QMetaType::Int || gqlObject["offset"].userType() == QMetaType::UInt || gqlObject["offset"].userType() == QMetaType::LongLong || gqlObject["offset"].userType() == QMetaType::ULongLong || gqlObject["offset"].userType() == QMetaType::Long || gqlObject["offset"].userType() == QMetaType::Short || gqlObject["offset"].userType() == QMetaType::ULong || gqlObject["offset"].userType() == QMetaType::UShort || gqlObject["offset"].userType() == QMetaType::UChar)){
		offset = gqlObject["offset"].toInt();
	}

	if (gqlObject.ContainsParam("filterModel") && (gqlObject.GetParamArgumentObjectPtr("filterModel") != nullptr)){
		filterModel = V1_0::imtbase::CComplexCollectionFilter();
		const bool isFilterModelRead = filterModel->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("filterModel"));
		if (!isFilterModelRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool V1_0::complextest::CCollectionViewParamsTest::WriteToJsonObject(QJsonObject& jsonObject) const
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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["filterModel"] = filterModelJsonObject;
	}

	jsonObject["__typename"] = "CollectionViewParamsTest";

	return true;
}


bool V1_0::complextest::CCollectionViewParamsTest::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("count") && jsonObject["count"].isDouble()){
		count = jsonObject["count"].toInt();
	}

	if (jsonObject.contains("offset") && jsonObject["offset"].isDouble()){
		offset = jsonObject["offset"].toInt();
	}

	if (jsonObject.contains("filterModel") && jsonObject["filterModel"].isObject()){
		filterModel = V1_0::imtbase::CComplexCollectionFilter();
		const bool isFilterModelRead = filterModel->ReadFromJsonObject(jsonObject["filterModel"].toObject());
		if (!isFilterModelRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool V1_0::complextest::CCollectionViewParamsTest::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("count") && jsonObject["count"].isDouble()){
		count = jsonObject["count"].toInt();
	}

	if (jsonObject.contains("offset") && jsonObject["offset"].isDouble()){
		offset = jsonObject["offset"].toInt();
	}

	if (jsonObject.contains("filterModel") && jsonObject["filterModel"].isObject()){
		filterModel = V1_0::imtbase::CComplexCollectionFilter();
		const bool isFilterModelRead = filterModel->OptReadFromJsonObject(jsonObject["filterModel"].toObject());
		if (!isFilterModelRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "filterModel").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
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

	// reading input arguments

	// reading requested fields
	const imtgql::CGqlFieldObject* requestedFieldsObjectPtr = nullptr;
	if (!gqlRequest.GetFields().GetFieldIds().isEmpty()){
		requestedFieldsObjectPtr = &gqlRequest.GetFields();
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
				const imtgql::CGqlFieldObject* InspectionsRequestedFieldsPtr = requestedFieldsObjectPtr->GetFieldArgumentObjectPtr("Inspections");
				if (InspectionsRequestedFieldsPtr != nullptr){
					const QByteArrayList InspectionsRequestedIds = InspectionsRequestedFieldsPtr->GetFieldIds();
					if (!InspectionsRequestedIds.isEmpty()){
						m_requestInfo.Inspections.isIDRequested = true;
						m_requestInfo.Inspections.isTypeIDRequested = true;
						m_requestInfo.Inspections.isNameRequested = true;
						m_requestInfo.Inspections.isStatusRequested = true;
						m_requestInfo.Inspections.isAnalyzersRequested = InspectionsRequestedIds.contains("Analyzers");
						const imtgql::CGqlFieldObject* Inspections_AnalyzersRequestedFieldsPtr = InspectionsRequestedFieldsPtr->GetFieldArgumentObjectPtr("Analyzers");
						if (Inspections_AnalyzersRequestedFieldsPtr != nullptr){
							const QByteArrayList Inspections_AnalyzersRequestedIds = Inspections_AnalyzersRequestedFieldsPtr->GetFieldIds();
							if (!Inspections_AnalyzersRequestedIds.isEmpty()){
								m_requestInfo.Inspections.Analyzers.isAnalyzerNameRequested = true;
								m_requestInfo.Inspections.Analyzers.isAnalyzerResultRequested = true;
								m_requestInfo.Inspections.Analyzers.isAreasRequested = Inspections_AnalyzersRequestedIds.contains("Areas");
								const imtgql::CGqlFieldObject* Inspections_Analyzers_AreasRequestedFieldsPtr = Inspections_AnalyzersRequestedFieldsPtr->GetFieldArgumentObjectPtr("Areas");
								if (Inspections_Analyzers_AreasRequestedFieldsPtr != nullptr){
									const QByteArrayList Inspections_Analyzers_AreasRequestedIds = Inspections_Analyzers_AreasRequestedFieldsPtr->GetFieldIds();
									if (!Inspections_Analyzers_AreasRequestedIds.isEmpty()){
										m_requestInfo.Inspections.Analyzers.Areas.isAreaNameRequested = true;
										m_requestInfo.Inspections.Analyzers.Areas.isStatusRequested = true;
										m_requestInfo.Inspections.Analyzers.Areas.isIconPositionRequested = true;
										m_requestInfo.Inspections.Analyzers.Areas.isErrorTypeRequested = true;
										m_requestInfo.Inspections.Analyzers.Areas.isResultsRequested = true;
										const imtgql::CGqlFieldObject* Inspections_Analyzers_Areas_IconPositionRequestedFieldsPtr = Inspections_Analyzers_AreasRequestedFieldsPtr->GetFieldArgumentObjectPtr("IconPosition");
										if (Inspections_Analyzers_Areas_IconPositionRequestedFieldsPtr != nullptr){
											const QByteArrayList Inspections_Analyzers_Areas_IconPositionRequestedIds = Inspections_Analyzers_Areas_IconPositionRequestedFieldsPtr->GetFieldIds();
											if (!Inspections_Analyzers_Areas_IconPositionRequestedIds.isEmpty()){
												m_requestInfo.Inspections.Analyzers.Areas.IconPosition.isXRequested = true;
												m_requestInfo.Inspections.Analyzers.Areas.IconPosition.isYRequested = true;
											}
										}
										const imtgql::CGqlFieldObject* Inspections_Analyzers_Areas_ResultsRequestedFieldsPtr = Inspections_Analyzers_AreasRequestedFieldsPtr->GetFieldArgumentObjectPtr("Results");
										if (Inspections_Analyzers_Areas_ResultsRequestedFieldsPtr != nullptr){
											const QByteArrayList Inspections_Analyzers_Areas_ResultsRequestedIds = Inspections_Analyzers_Areas_ResultsRequestedFieldsPtr->GetFieldIds();
											if (!Inspections_Analyzers_Areas_ResultsRequestedIds.isEmpty()){
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
												const imtgql::CGqlFieldObject* Inspections_Analyzers_Areas_Results_GeometryRequestedFieldsPtr = Inspections_Analyzers_Areas_ResultsRequestedFieldsPtr->GetFieldArgumentObjectPtr("Geometry");
												if (Inspections_Analyzers_Areas_Results_GeometryRequestedFieldsPtr != nullptr){
													const QByteArrayList Inspections_Analyzers_Areas_Results_GeometryRequestedIds = Inspections_Analyzers_Areas_Results_GeometryRequestedFieldsPtr->GetFieldIds();
													if (!Inspections_Analyzers_Areas_Results_GeometryRequestedIds.isEmpty()){
														m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.isGeometryTypeRequested = true;
														m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.isRadiusRequested = Inspections_Analyzers_Areas_Results_GeometryRequestedIds.contains("Radius");
														m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.isPointsRequested = true;
														m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.isRequiredPointsRequested = true;
														m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.isOptionalPointsRequested = Inspections_Analyzers_Areas_Results_GeometryRequestedIds.contains("OptionalPoints");
														const imtgql::CGqlFieldObject* Inspections_Analyzers_Areas_Results_Geometry_PointsRequestedFieldsPtr = Inspections_Analyzers_Areas_Results_GeometryRequestedFieldsPtr->GetFieldArgumentObjectPtr("Points");
														if (Inspections_Analyzers_Areas_Results_Geometry_PointsRequestedFieldsPtr != nullptr){
															const QByteArrayList Inspections_Analyzers_Areas_Results_Geometry_PointsRequestedIds = Inspections_Analyzers_Areas_Results_Geometry_PointsRequestedFieldsPtr->GetFieldIds();
															if (!Inspections_Analyzers_Areas_Results_Geometry_PointsRequestedIds.isEmpty()){
																m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.Points.isXRequested = true;
																m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.Points.isYRequested = true;
															}
														}
														const imtgql::CGqlFieldObject* Inspections_Analyzers_Areas_Results_Geometry_RequiredPointsRequestedFieldsPtr = Inspections_Analyzers_Areas_Results_GeometryRequestedFieldsPtr->GetFieldArgumentObjectPtr("RequiredPoints");
														if (Inspections_Analyzers_Areas_Results_Geometry_RequiredPointsRequestedFieldsPtr != nullptr){
															const QByteArrayList Inspections_Analyzers_Areas_Results_Geometry_RequiredPointsRequestedIds = Inspections_Analyzers_Areas_Results_Geometry_RequiredPointsRequestedFieldsPtr->GetFieldIds();
															if (!Inspections_Analyzers_Areas_Results_Geometry_RequiredPointsRequestedIds.isEmpty()){
																m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.RequiredPoints.isXRequested = true;
																m_requestInfo.Inspections.Analyzers.Areas.Results.Geometry.RequiredPoints.isYRequested = true;
															}
														}
														const imtgql::CGqlFieldObject* Inspections_Analyzers_Areas_Results_Geometry_OptionalPointsRequestedFieldsPtr = Inspections_Analyzers_Areas_Results_GeometryRequestedFieldsPtr->GetFieldArgumentObjectPtr("OptionalPoints");
														if (Inspections_Analyzers_Areas_Results_Geometry_OptionalPointsRequestedFieldsPtr != nullptr){
															const QByteArrayList Inspections_Analyzers_Areas_Results_Geometry_OptionalPointsRequestedIds = Inspections_Analyzers_Areas_Results_Geometry_OptionalPointsRequestedFieldsPtr->GetFieldIds();
															if (!Inspections_Analyzers_Areas_Results_Geometry_OptionalPointsRequestedIds.isEmpty()){
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
	if (!requestArguments.pointAsCoordinates->WriteToGraphQlObject(pointAsCoordinatesDataObject)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to create GQL Object").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

		return false;
	}
	gqlRequest.AddParam("pointAsCoordinates", pointAsCoordinatesDataObject);

	return true;
}


CGetLastProductionResultsCDMGqlRequest::CGetLastProductionResultsCDMGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead)
	: m_isValid(true)
{
	m_gqlContextPtr = gqlRequest.GetRequestContext();

	// reading input arguments
	const ::imtgql::CGqlParamObject* pointAsCoordinatesDataObjectPtr = gqlRequest.GetParamObject("pointAsCoordinates");
	if (pointAsCoordinatesDataObjectPtr != nullptr){
					bool isPointAsCoordinatesRead;
			if (optRead){
				isPointAsCoordinatesRead = m_requestedArguments.pointAsCoordinates.emplace().OptReadFromGraphQlObject(*pointAsCoordinatesDataObjectPtr);
			}
			else {
				isPointAsCoordinatesRead = m_requestedArguments.pointAsCoordinates.emplace().ReadFromGraphQlObject(*pointAsCoordinatesDataObjectPtr);
			}
			m_isValid = isPointAsCoordinatesRead;
			if (!isPointAsCoordinatesRead){
				return;
			}

	}

	// reading requested fields
	const imtgql::CGqlFieldObject* requestedFieldsObjectPtr = nullptr;
	if (!gqlRequest.GetFields().GetFieldIds().isEmpty()){
		requestedFieldsObjectPtr = &gqlRequest.GetFields();
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
						const imtgql::CGqlFieldObject* resultMetaData_descriptionRequestedFieldsPtr = resultMetaDataRequestedFieldsPtr->GetFieldArgumentObjectPtr("description");
						if (resultMetaData_descriptionRequestedFieldsPtr != nullptr){
							const QByteArrayList resultMetaData_descriptionRequestedIds = resultMetaData_descriptionRequestedFieldsPtr->GetFieldIds();
							if (!resultMetaData_descriptionRequestedIds.isEmpty()){
								m_requestInfo.resultMetaData.description.isTextRequested = true;
								m_requestInfo.resultMetaData.description.isLocaleRequested = true;
							}
						}
						const imtgql::CGqlFieldObject* resultMetaData_extendedMetaDataRequestedFieldsPtr = resultMetaDataRequestedFieldsPtr->GetFieldArgumentObjectPtr("extendedMetaData");
						if (resultMetaData_extendedMetaDataRequestedFieldsPtr != nullptr){
							const QByteArrayList resultMetaData_extendedMetaDataRequestedIds = resultMetaData_extendedMetaDataRequestedFieldsPtr->GetFieldIds();
							if (!resultMetaData_extendedMetaDataRequestedIds.isEmpty()){
								m_requestInfo.resultMetaData.extendedMetaData.isKeyRequested = resultMetaData_extendedMetaDataRequestedIds.contains("key");
								m_requestInfo.resultMetaData.extendedMetaData.isValueRequested = resultMetaData_extendedMetaDataRequestedIds.contains("value");
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
			errorMessage = QStringLiteral("Bad request. Unexpected request for command-ID: '%1'").arg(commandId);
			SendErrorMessage(0, errorMessage);

			return QJsonObject();
		}

		V1_0::complextest::CProductOverview replyPayload = OnGetLastProductionResults(getLastProductionResultsGqlRequest, gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			SendErrorMessage(0, QStringLiteral("The derived call [OnGetLastProductionResults] returned an error: %1").arg(errorMessage));

			return QJsonObject();
		}

		const bool isModelCreated = replyPayload.WriteToJsonObject(dataModelObj);
		if (!isModelCreated){
			errorMessage = QStringLiteral("Internal error. Unable to create response for command-ID: '%1'").arg(commandId);
			SendCriticalMessage(0, errorMessage);

			return QJsonObject();
		}

		modelObj.insert(QStringLiteral("data"), dataModelObj); return modelObj;
	}

	// GetLastProductionResultsCDM
	if (commandId == CGetLastProductionResultsCDMGqlRequest::GetCommandId()){
		CGetLastProductionResultsCDMGqlRequest getLastProductionResultsCDMGqlRequest(gqlRequest, false);
		if (!getLastProductionResultsCDMGqlRequest.IsValid()){
			errorMessage = QStringLiteral("Bad request. Unexpected request for command-ID: '%1'").arg(commandId);
			SendErrorMessage(0, errorMessage);

			return QJsonObject();
		}

		V1_0::complextest::CCDMResult replyPayload = OnGetLastProductionResultsCDM(getLastProductionResultsCDMGqlRequest, gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			SendErrorMessage(0, QStringLiteral("The derived call [OnGetLastProductionResultsCDM] returned an error: %1").arg(errorMessage));

			return QJsonObject();
		}

		const bool isModelCreated = replyPayload.WriteToJsonObject(dataModelObj);
		if (!isModelCreated){
			errorMessage = QStringLiteral("Internal error. Unable to create response for command-ID: '%1'").arg(commandId);
			SendCriticalMessage(0, errorMessage);

			return QJsonObject();
		}

		modelObj.insert(QStringLiteral("data"), dataModelObj); return modelObj;
	}

	errorMessage = QStringLiteral("Bad request. Unexpected command-ID: '%1'").arg(commandId);
	SendErrorMessage(0, errorMessage);

	return QJsonObject();
}


} // namespace sdl::V1_0::complextest
