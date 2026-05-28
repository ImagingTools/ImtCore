#include "ImtBaseTypes.h"
#include "ImtBaseTypes_fwd.h"


namespace sdl::V1_0::imtbase
{


QByteArray CTimeRange::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CTimeRange::operator==(const CTimeRange& other) const
{
	return 
				Begin == other.Begin &&
				End == other.End;
}


bool CTimeRange::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "TimeRange", modelIndex);

	if (Begin){
		model.SetData("Begin", *Begin, modelIndex);
	}

	if (End){
		model.SetData("End", *End, modelIndex);
	}


	return true;
}


bool CTimeRange::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant beginData = model.GetData("Begin", modelIndex);
	if (!beginData.isNull()){
		Begin = beginData.toString();
	}

	QVariant endData = model.GetData("End", modelIndex);
	if (!endData.isNull()){
		End = endData.toString();
	}

	return true;
}


bool CTimeRange::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant beginData = model.GetData("Begin", modelIndex);
	if (!beginData.isNull()){
		Begin = beginData.toString();
	}

	QVariant endData = model.GetData("End", modelIndex);
	if (!endData.isNull()){
		End = endData.toString();
	}

	return true;
}


bool CTimeRange::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (Begin){
		gqlObject.InsertParam("Begin", QVariant(*Begin));
	}

	if (End){
		gqlObject.InsertParam("End", QVariant(*End));
	}

	gqlObject.InsertParam("__typename", QVariant("TimeRange"));

	return true;
}


bool CTimeRange::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("Begin") && (gqlObject["Begin"].userType() == QMetaType::QString || gqlObject["Begin"].userType() == QMetaType::QByteArray)){
		Begin = gqlObject["Begin"].toString();
	}

	if (gqlObject.ContainsParam("End") && (gqlObject["End"].userType() == QMetaType::QString || gqlObject["End"].userType() == QMetaType::QByteArray)){
		End = gqlObject["End"].toString();
	}

	return true;
}


bool CTimeRange::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("Begin") && (gqlObject["Begin"].userType() == QMetaType::QString || gqlObject["Begin"].userType() == QMetaType::QByteArray)){
		Begin = gqlObject["Begin"].toString();
	}

	if (gqlObject.ContainsParam("End") && (gqlObject["End"].userType() == QMetaType::QString || gqlObject["End"].userType() == QMetaType::QByteArray)){
		End = gqlObject["End"].toString();
	}

	return true;
}


bool CTimeRange::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (Begin){
		jsonObject["Begin"] = QJsonValue::fromVariant(*Begin);
	}

	if (End){
		jsonObject["End"] = QJsonValue::fromVariant(*End);
	}

	jsonObject["__typename"] = "TimeRange";

	return true;
}


bool CTimeRange::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("Begin") && jsonObject["Begin"].isString()){
		Begin = jsonObject["Begin"].toString();
	}

	if (jsonObject.contains("End") && jsonObject["End"].isString()){
		End = jsonObject["End"].toString();
	}

	return true;
}


bool CTimeRange::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("Begin") && jsonObject["Begin"].isString()){
		Begin = jsonObject["Begin"].toString();
	}

	if (jsonObject.contains("End") && jsonObject["End"].isString()){
		End = jsonObject["End"].toString();
	}

	return true;
}


// serialize methods

QByteArray CSdlSize::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSdlSize::operator==(const CSdlSize& other) const
{
	return 
				width.has_value() == other.width.has_value() &&
				((width.has_value() && other.width.has_value()) ?
					qFuzzyCompare(*width, *other.width) : true) &&
				height.has_value() == other.height.has_value() &&
				((height.has_value() && other.height.has_value()) ?
					qFuzzyCompare(*height, *other.height) : true);
}


bool CSdlSize::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "SdlSize", modelIndex);

	if (!width){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "width").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("width", *width, modelIndex);

	if (!height){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "height").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("height", *height, modelIndex);


	return true;
}


bool CSdlSize::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant widthData = model.GetData("width", modelIndex);
	if (widthData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "width").toLocal8Bit().constData();)

		return false;
	}
	width = widthData.toDouble();

	QVariant heightData = model.GetData("height", modelIndex);
	if (heightData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "height").toLocal8Bit().constData();)

		return false;
	}
	height = heightData.toDouble();

	return true;
}


bool CSdlSize::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant widthData = model.GetData("width", modelIndex);
	if (!widthData.isNull()){
		width = widthData.toDouble();
	}

	QVariant heightData = model.GetData("height", modelIndex);
	if (!heightData.isNull()){
		height = heightData.toDouble();
	}

	return true;
}


bool CSdlSize::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!width){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "width").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("width", QVariant(*width));

	if (!height){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "height").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("height", QVariant(*height));

	gqlObject.InsertParam("__typename", QVariant("SdlSize"));

	return true;
}


bool CSdlSize::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("width") || (gqlObject["width"].userType() != QMetaType::Float && gqlObject["width"].userType() != QMetaType::Double && gqlObject["width"].userType() != QMetaType::Int && gqlObject["width"].userType() != QMetaType::UInt && gqlObject["width"].userType() != QMetaType::LongLong && gqlObject["width"].userType() != QMetaType::ULongLong && gqlObject["width"].userType() != QMetaType::Long && gqlObject["width"].userType() != QMetaType::Short && gqlObject["width"].userType() != QMetaType::ULong && gqlObject["width"].userType() != QMetaType::UShort && gqlObject["width"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "width").toLocal8Bit().constData();)

		return false;
	}
	width = gqlObject["width"].toDouble();

	if (!gqlObject.ContainsParam("height") || (gqlObject["height"].userType() != QMetaType::Float && gqlObject["height"].userType() != QMetaType::Double && gqlObject["height"].userType() != QMetaType::Int && gqlObject["height"].userType() != QMetaType::UInt && gqlObject["height"].userType() != QMetaType::LongLong && gqlObject["height"].userType() != QMetaType::ULongLong && gqlObject["height"].userType() != QMetaType::Long && gqlObject["height"].userType() != QMetaType::Short && gqlObject["height"].userType() != QMetaType::ULong && gqlObject["height"].userType() != QMetaType::UShort && gqlObject["height"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "height").toLocal8Bit().constData();)

		return false;
	}
	height = gqlObject["height"].toDouble();

	return true;
}


bool CSdlSize::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("width") && (gqlObject["width"].userType() == QMetaType::Float || gqlObject["width"].userType() == QMetaType::Double || gqlObject["width"].userType() == QMetaType::Int || gqlObject["width"].userType() == QMetaType::UInt || gqlObject["width"].userType() == QMetaType::LongLong || gqlObject["width"].userType() == QMetaType::ULongLong || gqlObject["width"].userType() == QMetaType::Long || gqlObject["width"].userType() == QMetaType::Short || gqlObject["width"].userType() == QMetaType::ULong || gqlObject["width"].userType() == QMetaType::UShort || gqlObject["width"].userType() == QMetaType::UChar)){
		width = gqlObject["width"].toDouble();
	}

	if (gqlObject.ContainsParam("height") && (gqlObject["height"].userType() == QMetaType::Float || gqlObject["height"].userType() == QMetaType::Double || gqlObject["height"].userType() == QMetaType::Int || gqlObject["height"].userType() == QMetaType::UInt || gqlObject["height"].userType() == QMetaType::LongLong || gqlObject["height"].userType() == QMetaType::ULongLong || gqlObject["height"].userType() == QMetaType::Long || gqlObject["height"].userType() == QMetaType::Short || gqlObject["height"].userType() == QMetaType::ULong || gqlObject["height"].userType() == QMetaType::UShort || gqlObject["height"].userType() == QMetaType::UChar)){
		height = gqlObject["height"].toDouble();
	}

	return true;
}


bool CSdlSize::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!width){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "width").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["width"] = QJsonValue::fromVariant(*width);

	if (!height){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "height").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["height"] = QJsonValue::fromVariant(*height);

	jsonObject["__typename"] = "SdlSize";

	return true;
}


bool CSdlSize::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("width") || ! jsonObject["width"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "width").toLocal8Bit().constData();)

		return false;
	}
	width = jsonObject["width"].toDouble();

	if (!jsonObject.contains("height") || ! jsonObject["height"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "height").toLocal8Bit().constData();)

		return false;
	}
	height = jsonObject["height"].toDouble();

	return true;
}


bool CSdlSize::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("width") && jsonObject["width"].isDouble()){
		width = jsonObject["width"].toDouble();
	}

	if (jsonObject.contains("height") && jsonObject["height"].isDouble()){
		height = jsonObject["height"].toDouble();
	}

	return true;
}


// serialize methods

QByteArray CSdlPoint::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSdlPoint::operator==(const CSdlPoint& other) const
{
	return 
				x.has_value() == other.x.has_value() &&
				((x.has_value() && other.x.has_value()) ?
					qFuzzyCompare(*x, *other.x) : true) &&
				y.has_value() == other.y.has_value() &&
				((y.has_value() && other.y.has_value()) ?
					qFuzzyCompare(*y, *other.y) : true);
}


bool CSdlPoint::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "SdlPoint", modelIndex);

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


	return true;
}


bool CSdlPoint::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant xData = model.GetData("x", modelIndex);
	if (xData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "x").toLocal8Bit().constData();)

		return false;
	}
	x = xData.toDouble();

	QVariant yData = model.GetData("y", modelIndex);
	if (yData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "y").toLocal8Bit().constData();)

		return false;
	}
	y = yData.toDouble();

	return true;
}


bool CSdlPoint::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant xData = model.GetData("x", modelIndex);
	if (!xData.isNull()){
		x = xData.toDouble();
	}

	QVariant yData = model.GetData("y", modelIndex);
	if (!yData.isNull()){
		y = yData.toDouble();
	}

	return true;
}


bool CSdlPoint::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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

	gqlObject.InsertParam("__typename", QVariant("SdlPoint"));

	return true;
}


bool CSdlPoint::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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

	return true;
}


bool CSdlPoint::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("x") && (gqlObject["x"].userType() == QMetaType::Float || gqlObject["x"].userType() == QMetaType::Double || gqlObject["x"].userType() == QMetaType::Int || gqlObject["x"].userType() == QMetaType::UInt || gqlObject["x"].userType() == QMetaType::LongLong || gqlObject["x"].userType() == QMetaType::ULongLong || gqlObject["x"].userType() == QMetaType::Long || gqlObject["x"].userType() == QMetaType::Short || gqlObject["x"].userType() == QMetaType::ULong || gqlObject["x"].userType() == QMetaType::UShort || gqlObject["x"].userType() == QMetaType::UChar)){
		x = gqlObject["x"].toDouble();
	}

	if (gqlObject.ContainsParam("y") && (gqlObject["y"].userType() == QMetaType::Float || gqlObject["y"].userType() == QMetaType::Double || gqlObject["y"].userType() == QMetaType::Int || gqlObject["y"].userType() == QMetaType::UInt || gqlObject["y"].userType() == QMetaType::LongLong || gqlObject["y"].userType() == QMetaType::ULongLong || gqlObject["y"].userType() == QMetaType::Long || gqlObject["y"].userType() == QMetaType::Short || gqlObject["y"].userType() == QMetaType::ULong || gqlObject["y"].userType() == QMetaType::UShort || gqlObject["y"].userType() == QMetaType::UChar)){
		y = gqlObject["y"].toDouble();
	}

	return true;
}


bool CSdlPoint::WriteToJsonObject(QJsonObject& jsonObject) const
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

	jsonObject["__typename"] = "SdlPoint";

	return true;
}


bool CSdlPoint::ReadFromJsonObject(const QJsonObject& jsonObject)
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

	return true;
}


bool CSdlPoint::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("x") && jsonObject["x"].isDouble()){
		x = jsonObject["x"].toDouble();
	}

	if (jsonObject.contains("y") && jsonObject["y"].isDouble()){
		y = jsonObject["y"].toDouble();
	}

	return true;
}


// serialize methods

QByteArray CParamTypeIds::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CParamTypeIds::operator==(const CParamTypeIds& other) const
{
	return 
				UrlParam == other.UrlParam &&
				IdParam == other.IdParam &&
				TextParam == other.TextParam &&
				TextView == other.TextView &&
				SelectionParam == other.SelectionParam &&
				SchedulerParam == other.SchedulerParam &&
				BackupSettings == other.BackupSettings &&
				DatabaseAccessSettings == other.DatabaseAccessSettings &&
				ParamsSet == other.ParamsSet &&
				FileNameParam == other.FileNameParam &&
				IntegerParam == other.IntegerParam &&
				DoubleParam == other.DoubleParam &&
				PasswordParam == other.PasswordParam &&
				EnableableParam == other.EnableableParam;
}


bool CParamTypeIds::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "ParamTypeIds", modelIndex);

	if (UrlParam){
		model.SetData("UrlParam", *UrlParam, modelIndex);
	}

	if (IdParam){
		model.SetData("IdParam", *IdParam, modelIndex);
	}

	if (TextParam){
		model.SetData("TextParam", *TextParam, modelIndex);
	}

	if (TextView){
		model.SetData("TextView", *TextView, modelIndex);
	}

	if (SelectionParam){
		model.SetData("SelectionParam", *SelectionParam, modelIndex);
	}

	if (SchedulerParam){
		model.SetData("SchedulerParam", *SchedulerParam, modelIndex);
	}

	if (BackupSettings){
		model.SetData("BackupSettings", *BackupSettings, modelIndex);
	}

	if (DatabaseAccessSettings){
		model.SetData("DatabaseAccessSettings", *DatabaseAccessSettings, modelIndex);
	}

	if (ParamsSet){
		model.SetData("ParamsSet", *ParamsSet, modelIndex);
	}

	if (FileNameParam){
		model.SetData("FileNameParam", *FileNameParam, modelIndex);
	}

	if (IntegerParam){
		model.SetData("IntegerParam", *IntegerParam, modelIndex);
	}

	if (DoubleParam){
		model.SetData("DoubleParam", *DoubleParam, modelIndex);
	}

	if (PasswordParam){
		model.SetData("PasswordParam", *PasswordParam, modelIndex);
	}

	if (EnableableParam){
		model.SetData("EnableableParam", *EnableableParam, modelIndex);
	}


	return true;
}


bool CParamTypeIds::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant urlParamData = model.GetData("UrlParam", modelIndex);
	if (!urlParamData.isNull()){
		UrlParam = urlParamData.toByteArray();
	}

	QVariant idParamData = model.GetData("IdParam", modelIndex);
	if (!idParamData.isNull()){
		IdParam = idParamData.toByteArray();
	}

	QVariant textParamData = model.GetData("TextParam", modelIndex);
	if (!textParamData.isNull()){
		TextParam = textParamData.toByteArray();
	}

	QVariant textViewData = model.GetData("TextView", modelIndex);
	if (!textViewData.isNull()){
		TextView = textViewData.toByteArray();
	}

	QVariant selectionParamData = model.GetData("SelectionParam", modelIndex);
	if (!selectionParamData.isNull()){
		SelectionParam = selectionParamData.toByteArray();
	}

	QVariant schedulerParamData = model.GetData("SchedulerParam", modelIndex);
	if (!schedulerParamData.isNull()){
		SchedulerParam = schedulerParamData.toByteArray();
	}

	QVariant backupSettingsData = model.GetData("BackupSettings", modelIndex);
	if (!backupSettingsData.isNull()){
		BackupSettings = backupSettingsData.toByteArray();
	}

	QVariant databaseAccessSettingsData = model.GetData("DatabaseAccessSettings", modelIndex);
	if (!databaseAccessSettingsData.isNull()){
		DatabaseAccessSettings = databaseAccessSettingsData.toByteArray();
	}

	QVariant paramsSetData = model.GetData("ParamsSet", modelIndex);
	if (!paramsSetData.isNull()){
		ParamsSet = paramsSetData.toByteArray();
	}

	QVariant fileNameParamData = model.GetData("FileNameParam", modelIndex);
	if (!fileNameParamData.isNull()){
		FileNameParam = fileNameParamData.toByteArray();
	}

	QVariant integerParamData = model.GetData("IntegerParam", modelIndex);
	if (!integerParamData.isNull()){
		IntegerParam = integerParamData.toByteArray();
	}

	QVariant doubleParamData = model.GetData("DoubleParam", modelIndex);
	if (!doubleParamData.isNull()){
		DoubleParam = doubleParamData.toByteArray();
	}

	QVariant passwordParamData = model.GetData("PasswordParam", modelIndex);
	if (!passwordParamData.isNull()){
		PasswordParam = passwordParamData.toByteArray();
	}

	QVariant enableableParamData = model.GetData("EnableableParam", modelIndex);
	if (!enableableParamData.isNull()){
		EnableableParam = enableableParamData.toByteArray();
	}

	return true;
}


bool CParamTypeIds::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant urlParamData = model.GetData("UrlParam", modelIndex);
	if (!urlParamData.isNull()){
		UrlParam = urlParamData.toByteArray();
	}

	QVariant idParamData = model.GetData("IdParam", modelIndex);
	if (!idParamData.isNull()){
		IdParam = idParamData.toByteArray();
	}

	QVariant textParamData = model.GetData("TextParam", modelIndex);
	if (!textParamData.isNull()){
		TextParam = textParamData.toByteArray();
	}

	QVariant textViewData = model.GetData("TextView", modelIndex);
	if (!textViewData.isNull()){
		TextView = textViewData.toByteArray();
	}

	QVariant selectionParamData = model.GetData("SelectionParam", modelIndex);
	if (!selectionParamData.isNull()){
		SelectionParam = selectionParamData.toByteArray();
	}

	QVariant schedulerParamData = model.GetData("SchedulerParam", modelIndex);
	if (!schedulerParamData.isNull()){
		SchedulerParam = schedulerParamData.toByteArray();
	}

	QVariant backupSettingsData = model.GetData("BackupSettings", modelIndex);
	if (!backupSettingsData.isNull()){
		BackupSettings = backupSettingsData.toByteArray();
	}

	QVariant databaseAccessSettingsData = model.GetData("DatabaseAccessSettings", modelIndex);
	if (!databaseAccessSettingsData.isNull()){
		DatabaseAccessSettings = databaseAccessSettingsData.toByteArray();
	}

	QVariant paramsSetData = model.GetData("ParamsSet", modelIndex);
	if (!paramsSetData.isNull()){
		ParamsSet = paramsSetData.toByteArray();
	}

	QVariant fileNameParamData = model.GetData("FileNameParam", modelIndex);
	if (!fileNameParamData.isNull()){
		FileNameParam = fileNameParamData.toByteArray();
	}

	QVariant integerParamData = model.GetData("IntegerParam", modelIndex);
	if (!integerParamData.isNull()){
		IntegerParam = integerParamData.toByteArray();
	}

	QVariant doubleParamData = model.GetData("DoubleParam", modelIndex);
	if (!doubleParamData.isNull()){
		DoubleParam = doubleParamData.toByteArray();
	}

	QVariant passwordParamData = model.GetData("PasswordParam", modelIndex);
	if (!passwordParamData.isNull()){
		PasswordParam = passwordParamData.toByteArray();
	}

	QVariant enableableParamData = model.GetData("EnableableParam", modelIndex);
	if (!enableableParamData.isNull()){
		EnableableParam = enableableParamData.toByteArray();
	}

	return true;
}


bool CParamTypeIds::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (UrlParam){
		gqlObject.InsertParam("UrlParam", QVariant(*UrlParam));
	}

	if (IdParam){
		gqlObject.InsertParam("IdParam", QVariant(*IdParam));
	}

	if (TextParam){
		gqlObject.InsertParam("TextParam", QVariant(*TextParam));
	}

	if (TextView){
		gqlObject.InsertParam("TextView", QVariant(*TextView));
	}

	if (SelectionParam){
		gqlObject.InsertParam("SelectionParam", QVariant(*SelectionParam));
	}

	if (SchedulerParam){
		gqlObject.InsertParam("SchedulerParam", QVariant(*SchedulerParam));
	}

	if (BackupSettings){
		gqlObject.InsertParam("BackupSettings", QVariant(*BackupSettings));
	}

	if (DatabaseAccessSettings){
		gqlObject.InsertParam("DatabaseAccessSettings", QVariant(*DatabaseAccessSettings));
	}

	if (ParamsSet){
		gqlObject.InsertParam("ParamsSet", QVariant(*ParamsSet));
	}

	if (FileNameParam){
		gqlObject.InsertParam("FileNameParam", QVariant(*FileNameParam));
	}

	if (IntegerParam){
		gqlObject.InsertParam("IntegerParam", QVariant(*IntegerParam));
	}

	if (DoubleParam){
		gqlObject.InsertParam("DoubleParam", QVariant(*DoubleParam));
	}

	if (PasswordParam){
		gqlObject.InsertParam("PasswordParam", QVariant(*PasswordParam));
	}

	if (EnableableParam){
		gqlObject.InsertParam("EnableableParam", QVariant(*EnableableParam));
	}

	gqlObject.InsertParam("__typename", QVariant("ParamTypeIds"));

	return true;
}


bool CParamTypeIds::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("UrlParam") && (gqlObject["UrlParam"].userType() == QMetaType::QString || gqlObject["UrlParam"].userType() == QMetaType::QByteArray)){
		UrlParam = gqlObject["UrlParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("IdParam") && (gqlObject["IdParam"].userType() == QMetaType::QString || gqlObject["IdParam"].userType() == QMetaType::QByteArray)){
		IdParam = gqlObject["IdParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("TextParam") && (gqlObject["TextParam"].userType() == QMetaType::QString || gqlObject["TextParam"].userType() == QMetaType::QByteArray)){
		TextParam = gqlObject["TextParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("TextView") && (gqlObject["TextView"].userType() == QMetaType::QString || gqlObject["TextView"].userType() == QMetaType::QByteArray)){
		TextView = gqlObject["TextView"].toByteArray();
	}

	if (gqlObject.ContainsParam("SelectionParam") && (gqlObject["SelectionParam"].userType() == QMetaType::QString || gqlObject["SelectionParam"].userType() == QMetaType::QByteArray)){
		SelectionParam = gqlObject["SelectionParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("SchedulerParam") && (gqlObject["SchedulerParam"].userType() == QMetaType::QString || gqlObject["SchedulerParam"].userType() == QMetaType::QByteArray)){
		SchedulerParam = gqlObject["SchedulerParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("BackupSettings") && (gqlObject["BackupSettings"].userType() == QMetaType::QString || gqlObject["BackupSettings"].userType() == QMetaType::QByteArray)){
		BackupSettings = gqlObject["BackupSettings"].toByteArray();
	}

	if (gqlObject.ContainsParam("DatabaseAccessSettings") && (gqlObject["DatabaseAccessSettings"].userType() == QMetaType::QString || gqlObject["DatabaseAccessSettings"].userType() == QMetaType::QByteArray)){
		DatabaseAccessSettings = gqlObject["DatabaseAccessSettings"].toByteArray();
	}

	if (gqlObject.ContainsParam("ParamsSet") && (gqlObject["ParamsSet"].userType() == QMetaType::QString || gqlObject["ParamsSet"].userType() == QMetaType::QByteArray)){
		ParamsSet = gqlObject["ParamsSet"].toByteArray();
	}

	if (gqlObject.ContainsParam("FileNameParam") && (gqlObject["FileNameParam"].userType() == QMetaType::QString || gqlObject["FileNameParam"].userType() == QMetaType::QByteArray)){
		FileNameParam = gqlObject["FileNameParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("IntegerParam") && (gqlObject["IntegerParam"].userType() == QMetaType::QString || gqlObject["IntegerParam"].userType() == QMetaType::QByteArray)){
		IntegerParam = gqlObject["IntegerParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("DoubleParam") && (gqlObject["DoubleParam"].userType() == QMetaType::QString || gqlObject["DoubleParam"].userType() == QMetaType::QByteArray)){
		DoubleParam = gqlObject["DoubleParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("PasswordParam") && (gqlObject["PasswordParam"].userType() == QMetaType::QString || gqlObject["PasswordParam"].userType() == QMetaType::QByteArray)){
		PasswordParam = gqlObject["PasswordParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("EnableableParam") && (gqlObject["EnableableParam"].userType() == QMetaType::QString || gqlObject["EnableableParam"].userType() == QMetaType::QByteArray)){
		EnableableParam = gqlObject["EnableableParam"].toByteArray();
	}

	return true;
}


bool CParamTypeIds::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("UrlParam") && (gqlObject["UrlParam"].userType() == QMetaType::QString || gqlObject["UrlParam"].userType() == QMetaType::QByteArray)){
		UrlParam = gqlObject["UrlParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("IdParam") && (gqlObject["IdParam"].userType() == QMetaType::QString || gqlObject["IdParam"].userType() == QMetaType::QByteArray)){
		IdParam = gqlObject["IdParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("TextParam") && (gqlObject["TextParam"].userType() == QMetaType::QString || gqlObject["TextParam"].userType() == QMetaType::QByteArray)){
		TextParam = gqlObject["TextParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("TextView") && (gqlObject["TextView"].userType() == QMetaType::QString || gqlObject["TextView"].userType() == QMetaType::QByteArray)){
		TextView = gqlObject["TextView"].toByteArray();
	}

	if (gqlObject.ContainsParam("SelectionParam") && (gqlObject["SelectionParam"].userType() == QMetaType::QString || gqlObject["SelectionParam"].userType() == QMetaType::QByteArray)){
		SelectionParam = gqlObject["SelectionParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("SchedulerParam") && (gqlObject["SchedulerParam"].userType() == QMetaType::QString || gqlObject["SchedulerParam"].userType() == QMetaType::QByteArray)){
		SchedulerParam = gqlObject["SchedulerParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("BackupSettings") && (gqlObject["BackupSettings"].userType() == QMetaType::QString || gqlObject["BackupSettings"].userType() == QMetaType::QByteArray)){
		BackupSettings = gqlObject["BackupSettings"].toByteArray();
	}

	if (gqlObject.ContainsParam("DatabaseAccessSettings") && (gqlObject["DatabaseAccessSettings"].userType() == QMetaType::QString || gqlObject["DatabaseAccessSettings"].userType() == QMetaType::QByteArray)){
		DatabaseAccessSettings = gqlObject["DatabaseAccessSettings"].toByteArray();
	}

	if (gqlObject.ContainsParam("ParamsSet") && (gqlObject["ParamsSet"].userType() == QMetaType::QString || gqlObject["ParamsSet"].userType() == QMetaType::QByteArray)){
		ParamsSet = gqlObject["ParamsSet"].toByteArray();
	}

	if (gqlObject.ContainsParam("FileNameParam") && (gqlObject["FileNameParam"].userType() == QMetaType::QString || gqlObject["FileNameParam"].userType() == QMetaType::QByteArray)){
		FileNameParam = gqlObject["FileNameParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("IntegerParam") && (gqlObject["IntegerParam"].userType() == QMetaType::QString || gqlObject["IntegerParam"].userType() == QMetaType::QByteArray)){
		IntegerParam = gqlObject["IntegerParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("DoubleParam") && (gqlObject["DoubleParam"].userType() == QMetaType::QString || gqlObject["DoubleParam"].userType() == QMetaType::QByteArray)){
		DoubleParam = gqlObject["DoubleParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("PasswordParam") && (gqlObject["PasswordParam"].userType() == QMetaType::QString || gqlObject["PasswordParam"].userType() == QMetaType::QByteArray)){
		PasswordParam = gqlObject["PasswordParam"].toByteArray();
	}

	if (gqlObject.ContainsParam("EnableableParam") && (gqlObject["EnableableParam"].userType() == QMetaType::QString || gqlObject["EnableableParam"].userType() == QMetaType::QByteArray)){
		EnableableParam = gqlObject["EnableableParam"].toByteArray();
	}

	return true;
}


bool CParamTypeIds::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (UrlParam){
		jsonObject["UrlParam"] = QString::fromUtf8(*UrlParam);
	}

	if (IdParam){
		jsonObject["IdParam"] = QString::fromUtf8(*IdParam);
	}

	if (TextParam){
		jsonObject["TextParam"] = QString::fromUtf8(*TextParam);
	}

	if (TextView){
		jsonObject["TextView"] = QString::fromUtf8(*TextView);
	}

	if (SelectionParam){
		jsonObject["SelectionParam"] = QString::fromUtf8(*SelectionParam);
	}

	if (SchedulerParam){
		jsonObject["SchedulerParam"] = QString::fromUtf8(*SchedulerParam);
	}

	if (BackupSettings){
		jsonObject["BackupSettings"] = QString::fromUtf8(*BackupSettings);
	}

	if (DatabaseAccessSettings){
		jsonObject["DatabaseAccessSettings"] = QString::fromUtf8(*DatabaseAccessSettings);
	}

	if (ParamsSet){
		jsonObject["ParamsSet"] = QString::fromUtf8(*ParamsSet);
	}

	if (FileNameParam){
		jsonObject["FileNameParam"] = QString::fromUtf8(*FileNameParam);
	}

	if (IntegerParam){
		jsonObject["IntegerParam"] = QString::fromUtf8(*IntegerParam);
	}

	if (DoubleParam){
		jsonObject["DoubleParam"] = QString::fromUtf8(*DoubleParam);
	}

	if (PasswordParam){
		jsonObject["PasswordParam"] = QString::fromUtf8(*PasswordParam);
	}

	if (EnableableParam){
		jsonObject["EnableableParam"] = QString::fromUtf8(*EnableableParam);
	}

	jsonObject["__typename"] = "ParamTypeIds";

	return true;
}


bool CParamTypeIds::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("UrlParam") && jsonObject["UrlParam"].isString()){
		UrlParam = jsonObject["UrlParam"].toString().toUtf8();
	}

	if (jsonObject.contains("IdParam") && jsonObject["IdParam"].isString()){
		IdParam = jsonObject["IdParam"].toString().toUtf8();
	}

	if (jsonObject.contains("TextParam") && jsonObject["TextParam"].isString()){
		TextParam = jsonObject["TextParam"].toString().toUtf8();
	}

	if (jsonObject.contains("TextView") && jsonObject["TextView"].isString()){
		TextView = jsonObject["TextView"].toString().toUtf8();
	}

	if (jsonObject.contains("SelectionParam") && jsonObject["SelectionParam"].isString()){
		SelectionParam = jsonObject["SelectionParam"].toString().toUtf8();
	}

	if (jsonObject.contains("SchedulerParam") && jsonObject["SchedulerParam"].isString()){
		SchedulerParam = jsonObject["SchedulerParam"].toString().toUtf8();
	}

	if (jsonObject.contains("BackupSettings") && jsonObject["BackupSettings"].isString()){
		BackupSettings = jsonObject["BackupSettings"].toString().toUtf8();
	}

	if (jsonObject.contains("DatabaseAccessSettings") && jsonObject["DatabaseAccessSettings"].isString()){
		DatabaseAccessSettings = jsonObject["DatabaseAccessSettings"].toString().toUtf8();
	}

	if (jsonObject.contains("ParamsSet") && jsonObject["ParamsSet"].isString()){
		ParamsSet = jsonObject["ParamsSet"].toString().toUtf8();
	}

	if (jsonObject.contains("FileNameParam") && jsonObject["FileNameParam"].isString()){
		FileNameParam = jsonObject["FileNameParam"].toString().toUtf8();
	}

	if (jsonObject.contains("IntegerParam") && jsonObject["IntegerParam"].isString()){
		IntegerParam = jsonObject["IntegerParam"].toString().toUtf8();
	}

	if (jsonObject.contains("DoubleParam") && jsonObject["DoubleParam"].isString()){
		DoubleParam = jsonObject["DoubleParam"].toString().toUtf8();
	}

	if (jsonObject.contains("PasswordParam") && jsonObject["PasswordParam"].isString()){
		PasswordParam = jsonObject["PasswordParam"].toString().toUtf8();
	}

	if (jsonObject.contains("EnableableParam") && jsonObject["EnableableParam"].isString()){
		EnableableParam = jsonObject["EnableableParam"].toString().toUtf8();
	}

	return true;
}


bool CParamTypeIds::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("UrlParam") && jsonObject["UrlParam"].isString()){
		UrlParam = jsonObject["UrlParam"].toString().toUtf8();
	}

	if (jsonObject.contains("IdParam") && jsonObject["IdParam"].isString()){
		IdParam = jsonObject["IdParam"].toString().toUtf8();
	}

	if (jsonObject.contains("TextParam") && jsonObject["TextParam"].isString()){
		TextParam = jsonObject["TextParam"].toString().toUtf8();
	}

	if (jsonObject.contains("TextView") && jsonObject["TextView"].isString()){
		TextView = jsonObject["TextView"].toString().toUtf8();
	}

	if (jsonObject.contains("SelectionParam") && jsonObject["SelectionParam"].isString()){
		SelectionParam = jsonObject["SelectionParam"].toString().toUtf8();
	}

	if (jsonObject.contains("SchedulerParam") && jsonObject["SchedulerParam"].isString()){
		SchedulerParam = jsonObject["SchedulerParam"].toString().toUtf8();
	}

	if (jsonObject.contains("BackupSettings") && jsonObject["BackupSettings"].isString()){
		BackupSettings = jsonObject["BackupSettings"].toString().toUtf8();
	}

	if (jsonObject.contains("DatabaseAccessSettings") && jsonObject["DatabaseAccessSettings"].isString()){
		DatabaseAccessSettings = jsonObject["DatabaseAccessSettings"].toString().toUtf8();
	}

	if (jsonObject.contains("ParamsSet") && jsonObject["ParamsSet"].isString()){
		ParamsSet = jsonObject["ParamsSet"].toString().toUtf8();
	}

	if (jsonObject.contains("FileNameParam") && jsonObject["FileNameParam"].isString()){
		FileNameParam = jsonObject["FileNameParam"].toString().toUtf8();
	}

	if (jsonObject.contains("IntegerParam") && jsonObject["IntegerParam"].isString()){
		IntegerParam = jsonObject["IntegerParam"].toString().toUtf8();
	}

	if (jsonObject.contains("DoubleParam") && jsonObject["DoubleParam"].isString()){
		DoubleParam = jsonObject["DoubleParam"].toString().toUtf8();
	}

	if (jsonObject.contains("PasswordParam") && jsonObject["PasswordParam"].isString()){
		PasswordParam = jsonObject["PasswordParam"].toString().toUtf8();
	}

	if (jsonObject.contains("EnableableParam") && jsonObject["EnableableParam"].isString()){
		EnableableParam = jsonObject["EnableableParam"].toString().toUtf8();
	}

	return true;
}


// serialize methods

QByteArray CUrlParam::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CUrlParam::operator==(const CUrlParam& other) const
{
	return 
				scheme == other.scheme &&
				host == other.host &&
				port == other.port &&
				path == other.path;
}


bool CUrlParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "UrlParam", modelIndex);

	if (scheme){
		model.SetData("scheme", *scheme, modelIndex);
	}

	if (host){
		model.SetData("host", *host, modelIndex);
	}

	if (port){
		model.SetData("port", *port, modelIndex);
	}

	if (path){
		model.SetData("path", *path, modelIndex);
	}


	return true;
}


bool CUrlParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant schemeData = model.GetData("scheme", modelIndex);
	if (!schemeData.isNull()){
		scheme = schemeData.toString();
	}

	QVariant hostData = model.GetData("host", modelIndex);
	if (!hostData.isNull()){
		host = hostData.toString();
	}

	QVariant portData = model.GetData("port", modelIndex);
	if (!portData.isNull()){
		port = portData.toInt();
	}

	QVariant pathData = model.GetData("path", modelIndex);
	if (!pathData.isNull()){
		path = pathData.toString();
	}

	return true;
}


bool CUrlParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant schemeData = model.GetData("scheme", modelIndex);
	if (!schemeData.isNull()){
		scheme = schemeData.toString();
	}

	QVariant hostData = model.GetData("host", modelIndex);
	if (!hostData.isNull()){
		host = hostData.toString();
	}

	QVariant portData = model.GetData("port", modelIndex);
	if (!portData.isNull()){
		port = portData.toInt();
	}

	QVariant pathData = model.GetData("path", modelIndex);
	if (!pathData.isNull()){
		path = pathData.toString();
	}

	return true;
}


bool CUrlParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (scheme){
		gqlObject.InsertParam("scheme", QVariant(*scheme));
	}

	if (host){
		gqlObject.InsertParam("host", QVariant(*host));
	}

	if (port){
		gqlObject.InsertParam("port", QVariant(*port));
	}

	if (path){
		gqlObject.InsertParam("path", QVariant(*path));
	}

	gqlObject.InsertParam("__typename", QVariant("UrlParam"));

	return true;
}


bool CUrlParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("scheme") && (gqlObject["scheme"].userType() == QMetaType::QString || gqlObject["scheme"].userType() == QMetaType::QByteArray)){
		scheme = gqlObject["scheme"].toString();
	}

	if (gqlObject.ContainsParam("host") && (gqlObject["host"].userType() == QMetaType::QString || gqlObject["host"].userType() == QMetaType::QByteArray)){
		host = gqlObject["host"].toString();
	}

	if (gqlObject.ContainsParam("port") && (gqlObject["port"].userType() == QMetaType::Int || gqlObject["port"].userType() == QMetaType::UInt || gqlObject["port"].userType() == QMetaType::LongLong || gqlObject["port"].userType() == QMetaType::ULongLong || gqlObject["port"].userType() == QMetaType::Long || gqlObject["port"].userType() == QMetaType::Short || gqlObject["port"].userType() == QMetaType::ULong || gqlObject["port"].userType() == QMetaType::UShort || gqlObject["port"].userType() == QMetaType::UChar)){
		port = gqlObject["port"].toInt();
	}

	if (gqlObject.ContainsParam("path") && (gqlObject["path"].userType() == QMetaType::QString || gqlObject["path"].userType() == QMetaType::QByteArray)){
		path = gqlObject["path"].toString();
	}

	return true;
}


bool CUrlParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("scheme") && (gqlObject["scheme"].userType() == QMetaType::QString || gqlObject["scheme"].userType() == QMetaType::QByteArray)){
		scheme = gqlObject["scheme"].toString();
	}

	if (gqlObject.ContainsParam("host") && (gqlObject["host"].userType() == QMetaType::QString || gqlObject["host"].userType() == QMetaType::QByteArray)){
		host = gqlObject["host"].toString();
	}

	if (gqlObject.ContainsParam("port") && (gqlObject["port"].userType() == QMetaType::Int || gqlObject["port"].userType() == QMetaType::UInt || gqlObject["port"].userType() == QMetaType::LongLong || gqlObject["port"].userType() == QMetaType::ULongLong || gqlObject["port"].userType() == QMetaType::Long || gqlObject["port"].userType() == QMetaType::Short || gqlObject["port"].userType() == QMetaType::ULong || gqlObject["port"].userType() == QMetaType::UShort || gqlObject["port"].userType() == QMetaType::UChar)){
		port = gqlObject["port"].toInt();
	}

	if (gqlObject.ContainsParam("path") && (gqlObject["path"].userType() == QMetaType::QString || gqlObject["path"].userType() == QMetaType::QByteArray)){
		path = gqlObject["path"].toString();
	}

	return true;
}


bool CUrlParam::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (scheme){
		jsonObject["scheme"] = QJsonValue::fromVariant(*scheme);
	}

	if (host){
		jsonObject["host"] = QJsonValue::fromVariant(*host);
	}

	if (port){
		jsonObject["port"] = QJsonValue::fromVariant(*port);
	}

	if (path){
		jsonObject["path"] = QJsonValue::fromVariant(*path);
	}

	jsonObject["__typename"] = "UrlParam";

	return true;
}


bool CUrlParam::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("scheme") && jsonObject["scheme"].isString()){
		scheme = jsonObject["scheme"].toString();
	}

	if (jsonObject.contains("host") && jsonObject["host"].isString()){
		host = jsonObject["host"].toString();
	}

	if (jsonObject.contains("port") && jsonObject["port"].isDouble()){
		port = jsonObject["port"].toInt();
	}

	if (jsonObject.contains("path") && jsonObject["path"].isString()){
		path = jsonObject["path"].toString();
	}

	return true;
}


bool CUrlParam::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("scheme") && jsonObject["scheme"].isString()){
		scheme = jsonObject["scheme"].toString();
	}

	if (jsonObject.contains("host") && jsonObject["host"].isString()){
		host = jsonObject["host"].toString();
	}

	if (jsonObject.contains("port") && jsonObject["port"].isDouble()){
		port = jsonObject["port"].toInt();
	}

	if (jsonObject.contains("path") && jsonObject["path"].isString()){
		path = jsonObject["path"].toString();
	}

	return true;
}


// serialize methods

QByteArray CIdParam::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CIdParam::operator==(const CIdParam& other) const
{
	return 
				id == other.id;
}


bool CIdParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "IdParam", modelIndex);

	if (id){
		model.SetData("id", *id, modelIndex);
	}


	return true;
}


bool CIdParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant idData = model.GetData("id", modelIndex);
	if (!idData.isNull()){
		id = idData.toByteArray();
	}

	return true;
}


bool CIdParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant idData = model.GetData("id", modelIndex);
	if (!idData.isNull()){
		id = idData.toByteArray();
	}

	return true;
}


bool CIdParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (id){
		gqlObject.InsertParam("id", QVariant(*id));
	}

	gqlObject.InsertParam("__typename", QVariant("IdParam"));

	return true;
}


bool CIdParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("id") && (gqlObject["id"].userType() == QMetaType::QString || gqlObject["id"].userType() == QMetaType::QByteArray)){
		id = gqlObject["id"].toByteArray();
	}

	return true;
}


bool CIdParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("id") && (gqlObject["id"].userType() == QMetaType::QString || gqlObject["id"].userType() == QMetaType::QByteArray)){
		id = gqlObject["id"].toByteArray();
	}

	return true;
}


bool CIdParam::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (id){
		jsonObject["id"] = QString::fromUtf8(*id);
	}

	jsonObject["__typename"] = "IdParam";

	return true;
}


bool CIdParam::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("id") && jsonObject["id"].isString()){
		id = jsonObject["id"].toString().toUtf8();
	}

	return true;
}


bool CIdParam::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("id") && jsonObject["id"].isString()){
		id = jsonObject["id"].toString().toUtf8();
	}

	return true;
}


// serialize methods

QByteArray CTextParam::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CTextParam::operator==(const CTextParam& other) const
{
	return 
				text == other.text;
}


bool CTextParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "TextParam", modelIndex);

	if (text){
		model.SetData("text", *text, modelIndex);
	}


	return true;
}


bool CTextParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant textData = model.GetData("text", modelIndex);
	if (!textData.isNull()){
		text = textData.toString();
	}

	return true;
}


bool CTextParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant textData = model.GetData("text", modelIndex);
	if (!textData.isNull()){
		text = textData.toString();
	}

	return true;
}


bool CTextParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (text){
		gqlObject.InsertParam("text", QVariant(*text));
	}

	gqlObject.InsertParam("__typename", QVariant("TextParam"));

	return true;
}


bool CTextParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("text") && (gqlObject["text"].userType() == QMetaType::QString || gqlObject["text"].userType() == QMetaType::QByteArray)){
		text = gqlObject["text"].toString();
	}

	return true;
}


bool CTextParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("text") && (gqlObject["text"].userType() == QMetaType::QString || gqlObject["text"].userType() == QMetaType::QByteArray)){
		text = gqlObject["text"].toString();
	}

	return true;
}


bool CTextParam::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (text){
		jsonObject["text"] = QJsonValue::fromVariant(*text);
	}

	jsonObject["__typename"] = "TextParam";

	return true;
}


bool CTextParam::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("text") && jsonObject["text"].isString()){
		text = jsonObject["text"].toString();
	}

	return true;
}


bool CTextParam::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("text") && jsonObject["text"].isString()){
		text = jsonObject["text"].toString();
	}

	return true;
}


// serialize methods

QByteArray CEnableableParam::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CEnableableParam::operator==(const CEnableableParam& other) const
{
	return 
				value == other.value;
}


bool CEnableableParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "EnableableParam", modelIndex);

	if (value){
		model.SetData("value", *value, modelIndex);
	}


	return true;
}


bool CEnableableParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
		value = valueData.toBool();
	}

	return true;
}


bool CEnableableParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
		value = valueData.toBool();
	}

	return true;
}


bool CEnableableParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (value){
		gqlObject.InsertParam("value", QVariant(*value));
	}

	gqlObject.InsertParam("__typename", QVariant("EnableableParam"));

	return true;
}


bool CEnableableParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("value") && (gqlObject["value"].userType() == QMetaType::Bool)){
		value = gqlObject["value"].toBool();
	}

	return true;
}


bool CEnableableParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("value") && (gqlObject["value"].userType() == QMetaType::Bool)){
		value = gqlObject["value"].toBool();
	}

	return true;
}


bool CEnableableParam::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (value){
		jsonObject["value"] = QJsonValue::fromVariant(*value);
	}

	jsonObject["__typename"] = "EnableableParam";

	return true;
}


bool CEnableableParam::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("value") && jsonObject["value"].isBool()){
		value = jsonObject["value"].toBool();
	}

	return true;
}


bool CEnableableParam::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("value") && jsonObject["value"].isBool()){
		value = jsonObject["value"].toBool();
	}

	return true;
}


// serialize methods

QByteArray CIntegerParam::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CIntegerParam::operator==(const CIntegerParam& other) const
{
	return 
				value == other.value;
}


bool CIntegerParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "IntegerParam", modelIndex);

	if (value){
		model.SetData("value", *value, modelIndex);
	}


	return true;
}


bool CIntegerParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
		value = valueData.toInt();
	}

	return true;
}


bool CIntegerParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
		value = valueData.toInt();
	}

	return true;
}


bool CIntegerParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (value){
		gqlObject.InsertParam("value", QVariant(*value));
	}

	gqlObject.InsertParam("__typename", QVariant("IntegerParam"));

	return true;
}


bool CIntegerParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("value") && (gqlObject["value"].userType() == QMetaType::Int || gqlObject["value"].userType() == QMetaType::UInt || gqlObject["value"].userType() == QMetaType::LongLong || gqlObject["value"].userType() == QMetaType::ULongLong || gqlObject["value"].userType() == QMetaType::Long || gqlObject["value"].userType() == QMetaType::Short || gqlObject["value"].userType() == QMetaType::ULong || gqlObject["value"].userType() == QMetaType::UShort || gqlObject["value"].userType() == QMetaType::UChar)){
		value = gqlObject["value"].toInt();
	}

	return true;
}


bool CIntegerParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("value") && (gqlObject["value"].userType() == QMetaType::Int || gqlObject["value"].userType() == QMetaType::UInt || gqlObject["value"].userType() == QMetaType::LongLong || gqlObject["value"].userType() == QMetaType::ULongLong || gqlObject["value"].userType() == QMetaType::Long || gqlObject["value"].userType() == QMetaType::Short || gqlObject["value"].userType() == QMetaType::ULong || gqlObject["value"].userType() == QMetaType::UShort || gqlObject["value"].userType() == QMetaType::UChar)){
		value = gqlObject["value"].toInt();
	}

	return true;
}


bool CIntegerParam::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (value){
		jsonObject["value"] = QJsonValue::fromVariant(*value);
	}

	jsonObject["__typename"] = "IntegerParam";

	return true;
}


bool CIntegerParam::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("value") && jsonObject["value"].isDouble()){
		value = jsonObject["value"].toInt();
	}

	return true;
}


bool CIntegerParam::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("value") && jsonObject["value"].isDouble()){
		value = jsonObject["value"].toInt();
	}

	return true;
}


// serialize methods

QByteArray CDoubleParam::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CDoubleParam::operator==(const CDoubleParam& other) const
{
	return 
				value.has_value() == other.value.has_value() &&
				((value.has_value() && other.value.has_value()) ?
					qFuzzyCompare(*value, *other.value) : true);
}


bool CDoubleParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "DoubleParam", modelIndex);

	if (value){
		model.SetData("value", *value, modelIndex);
	}


	return true;
}


bool CDoubleParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
		value = valueData.toDouble();
	}

	return true;
}


bool CDoubleParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
		value = valueData.toDouble();
	}

	return true;
}


bool CDoubleParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (value){
		gqlObject.InsertParam("value", QVariant(*value));
	}

	gqlObject.InsertParam("__typename", QVariant("DoubleParam"));

	return true;
}


bool CDoubleParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("value") && (gqlObject["value"].userType() == QMetaType::Float || gqlObject["value"].userType() == QMetaType::Double || gqlObject["value"].userType() == QMetaType::Int || gqlObject["value"].userType() == QMetaType::UInt || gqlObject["value"].userType() == QMetaType::LongLong || gqlObject["value"].userType() == QMetaType::ULongLong || gqlObject["value"].userType() == QMetaType::Long || gqlObject["value"].userType() == QMetaType::Short || gqlObject["value"].userType() == QMetaType::ULong || gqlObject["value"].userType() == QMetaType::UShort || gqlObject["value"].userType() == QMetaType::UChar)){
		value = gqlObject["value"].toDouble();
	}

	return true;
}


bool CDoubleParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("value") && (gqlObject["value"].userType() == QMetaType::Float || gqlObject["value"].userType() == QMetaType::Double || gqlObject["value"].userType() == QMetaType::Int || gqlObject["value"].userType() == QMetaType::UInt || gqlObject["value"].userType() == QMetaType::LongLong || gqlObject["value"].userType() == QMetaType::ULongLong || gqlObject["value"].userType() == QMetaType::Long || gqlObject["value"].userType() == QMetaType::Short || gqlObject["value"].userType() == QMetaType::ULong || gqlObject["value"].userType() == QMetaType::UShort || gqlObject["value"].userType() == QMetaType::UChar)){
		value = gqlObject["value"].toDouble();
	}

	return true;
}


bool CDoubleParam::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (value){
		jsonObject["value"] = QJsonValue::fromVariant(*value);
	}

	jsonObject["__typename"] = "DoubleParam";

	return true;
}


bool CDoubleParam::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("value") && jsonObject["value"].isDouble()){
		value = jsonObject["value"].toDouble();
	}

	return true;
}


bool CDoubleParam::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("value") && jsonObject["value"].isDouble()){
		value = jsonObject["value"].toDouble();
	}

	return true;
}


// serialize methods

QByteArray COption::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool COption::operator==(const COption& other) const
{
	return 
				id == other.id &&
				name == other.name &&
				description == other.description &&
				enabled == other.enabled;
}


bool COption::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Option", modelIndex);

	if (id){
		model.SetData("id", *id, modelIndex);
	}

	if (name){
		model.SetData("name", *name, modelIndex);
	}

	if (description){
		model.SetData("description", *description, modelIndex);
	}

	if (enabled){
		model.SetData("enabled", *enabled, modelIndex);
	}


	return true;
}


bool COption::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant idData = model.GetData("id", modelIndex);
	if (!idData.isNull()){
		id = idData.toByteArray();
	}

	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	QVariant descriptionData = model.GetData("description", modelIndex);
	if (!descriptionData.isNull()){
		description = descriptionData.toString();
	}

	QVariant enabledData = model.GetData("enabled", modelIndex);
	if (!enabledData.isNull()){
		enabled = enabledData.toBool();
	}

	return true;
}


bool COption::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant idData = model.GetData("id", modelIndex);
	if (!idData.isNull()){
		id = idData.toByteArray();
	}

	QVariant nameData = model.GetData("name", modelIndex);
	if (!nameData.isNull()){
		name = nameData.toString();
	}

	QVariant descriptionData = model.GetData("description", modelIndex);
	if (!descriptionData.isNull()){
		description = descriptionData.toString();
	}

	QVariant enabledData = model.GetData("enabled", modelIndex);
	if (!enabledData.isNull()){
		enabled = enabledData.toBool();
	}

	return true;
}


bool COption::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (id){
		gqlObject.InsertParam("id", QVariant(*id));
	}

	if (name){
		gqlObject.InsertParam("name", QVariant(*name));
	}

	if (description){
		gqlObject.InsertParam("description", QVariant(*description));
	}

	if (enabled){
		gqlObject.InsertParam("enabled", QVariant(*enabled));
	}

	gqlObject.InsertParam("__typename", QVariant("Option"));

	return true;
}


bool COption::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("id") && (gqlObject["id"].userType() == QMetaType::QString || gqlObject["id"].userType() == QMetaType::QByteArray)){
		id = gqlObject["id"].toByteArray();
	}

	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (gqlObject.ContainsParam("description") && (gqlObject["description"].userType() == QMetaType::QString || gqlObject["description"].userType() == QMetaType::QByteArray)){
		description = gqlObject["description"].toString();
	}

	if (gqlObject.ContainsParam("enabled") && (gqlObject["enabled"].userType() == QMetaType::Bool)){
		enabled = gqlObject["enabled"].toBool();
	}

	return true;
}


bool COption::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("id") && (gqlObject["id"].userType() == QMetaType::QString || gqlObject["id"].userType() == QMetaType::QByteArray)){
		id = gqlObject["id"].toByteArray();
	}

	if (gqlObject.ContainsParam("name") && (gqlObject["name"].userType() == QMetaType::QString || gqlObject["name"].userType() == QMetaType::QByteArray)){
		name = gqlObject["name"].toString();
	}

	if (gqlObject.ContainsParam("description") && (gqlObject["description"].userType() == QMetaType::QString || gqlObject["description"].userType() == QMetaType::QByteArray)){
		description = gqlObject["description"].toString();
	}

	if (gqlObject.ContainsParam("enabled") && (gqlObject["enabled"].userType() == QMetaType::Bool)){
		enabled = gqlObject["enabled"].toBool();
	}

	return true;
}


bool COption::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (id){
		jsonObject["id"] = QString::fromUtf8(*id);
	}

	if (name){
		jsonObject["name"] = QJsonValue::fromVariant(*name);
	}

	if (description){
		jsonObject["description"] = QJsonValue::fromVariant(*description);
	}

	if (enabled){
		jsonObject["enabled"] = QJsonValue::fromVariant(*enabled);
	}

	jsonObject["__typename"] = "Option";

	return true;
}


bool COption::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("id") && jsonObject["id"].isString()){
		id = jsonObject["id"].toString().toUtf8();
	}

	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (jsonObject.contains("description") && jsonObject["description"].isString()){
		description = jsonObject["description"].toString();
	}

	if (jsonObject.contains("enabled") && jsonObject["enabled"].isBool()){
		enabled = jsonObject["enabled"].toBool();
	}

	return true;
}


bool COption::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("id") && jsonObject["id"].isString()){
		id = jsonObject["id"].toString().toUtf8();
	}

	if (jsonObject.contains("name") && jsonObject["name"].isString()){
		name = jsonObject["name"].toString();
	}

	if (jsonObject.contains("description") && jsonObject["description"].isString()){
		description = jsonObject["description"].toString();
	}

	if (jsonObject.contains("enabled") && jsonObject["enabled"].isBool()){
		enabled = jsonObject["enabled"].toBool();
	}

	return true;
}


// serialize methods

QByteArray COptionsList::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool COptionsList::operator==(const COptionsList& other) const
{
	return 
				totalCount == other.totalCount &&
				offset == other.offset &&
				options == other.options;
}


bool COptionsList::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "OptionsList", modelIndex);

	if (totalCount){
		model.SetData("totalCount", *totalCount, modelIndex);
	}

	if (offset){
		model.SetData("offset", *offset, modelIndex);
	}

	if (options){
		::imtbase::CTreeItemModel* newOptionsModelPtr = model.AddTreeModel("options", modelIndex);
		newOptionsModelPtr->setIsArray(true);
		for (qsizetype optionsIndex = 0; optionsIndex < options->size(); ++optionsIndex){
			newOptionsModelPtr->InsertNewItem();
			if (!(options->at(optionsIndex)->WriteToModel(*newOptionsModelPtr, optionsIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "options").toLocal8Bit().constData();)

				return false;
			}
		}
	}

	return true;
}


bool COptionsList::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant totalCountData = model.GetData("totalCount", modelIndex);
	if (!totalCountData.isNull()){
		totalCount = totalCountData.toInt();
	}

	QVariant offsetData = model.GetData("offset", modelIndex);
	if (!offsetData.isNull()){
		offset = offsetData.toInt();
	}

	::imtbase::CTreeItemModel* optionsModel = model.GetTreeItemModel("options", modelIndex);
	if (optionsModel != nullptr){
		int optionsCount = optionsModel->GetItemsCount();
		imtsdl::TElementList<COption::V1_0> optionsList;
		for (int optionsIndex = 0; optionsIndex < optionsCount; ++optionsIndex){
			COption::V1_0 t_options;
			if (!t_options.ReadFromModel(*optionsModel, optionsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "options").toLocal8Bit().constData();)

				return false;
			}
			optionsList << t_options;
		}
		options = optionsList;

	}

	return true;
}


bool COptionsList::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant totalCountData = model.GetData("totalCount", modelIndex);
	if (!totalCountData.isNull()){
		totalCount = totalCountData.toInt();
	}

	QVariant offsetData = model.GetData("offset", modelIndex);
	if (!offsetData.isNull()){
		offset = offsetData.toInt();
	}

	::imtbase::CTreeItemModel* optionsModel = model.GetTreeItemModel("options", modelIndex);
	if (optionsModel != nullptr){
		int optionsCount = optionsModel->GetItemsCount();
		imtsdl::TElementList<COption::V1_0> optionsList;
		for (int optionsIndex = 0; optionsIndex < optionsCount; ++optionsIndex){
			COption::V1_0 t_options;
			if (!t_options.OptReadFromModel(*optionsModel, optionsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "options").toLocal8Bit().constData();)

				return false;
			}
			optionsList << t_options;
		}
		options = optionsList;

	}

	return true;
}


bool COptionsList::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (totalCount){
		gqlObject.InsertParam("totalCount", QVariant(*totalCount));
	}

	if (offset){
		gqlObject.InsertParam("offset", QVariant(*offset));
	}

	if (options){
		QList<::imtgql::CGqlParamObject> optionsDataObjectList;
		for (qsizetype optionsIndex = 0; optionsIndex < options->size(); ++optionsIndex){
			::imtgql::CGqlParamObject newOptionsGqlObject;
			if (!options->at(optionsIndex)->WriteToGraphQlObject(newOptionsGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "options").toLocal8Bit().constData();)

				return false;
			}
			optionsDataObjectList << newOptionsGqlObject;
		}
		gqlObject.InsertParam("options", optionsDataObjectList);
	}

	gqlObject.InsertParam("__typename", QVariant("OptionsList"));

	return true;
}


bool COptionsList::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("totalCount") && (gqlObject["totalCount"].userType() == QMetaType::Int || gqlObject["totalCount"].userType() == QMetaType::UInt || gqlObject["totalCount"].userType() == QMetaType::LongLong || gqlObject["totalCount"].userType() == QMetaType::ULongLong || gqlObject["totalCount"].userType() == QMetaType::Long || gqlObject["totalCount"].userType() == QMetaType::Short || gqlObject["totalCount"].userType() == QMetaType::ULong || gqlObject["totalCount"].userType() == QMetaType::UShort || gqlObject["totalCount"].userType() == QMetaType::UChar)){
		totalCount = gqlObject["totalCount"].toInt();
	}

	if (gqlObject.ContainsParam("offset") && (gqlObject["offset"].userType() == QMetaType::Int || gqlObject["offset"].userType() == QMetaType::UInt || gqlObject["offset"].userType() == QMetaType::LongLong || gqlObject["offset"].userType() == QMetaType::ULongLong || gqlObject["offset"].userType() == QMetaType::Long || gqlObject["offset"].userType() == QMetaType::Short || gqlObject["offset"].userType() == QMetaType::ULong || gqlObject["offset"].userType() == QMetaType::UShort || gqlObject["offset"].userType() == QMetaType::UChar)){
		offset = gqlObject["offset"].toInt();
	}

	if (gqlObject.ContainsParam("options")){
		options.emplace();
	}
	if (gqlObject.ContainsParam("options") && (gqlObject.GetObjectsCount("options") > 0)){
		const qsizetype optionsElementsCount = gqlObject.GetObjectsCount("options");
		options = imtsdl::TElementList<COption::V1_0>();
		for (qsizetype optionsIndex = 0; optionsIndex < optionsElementsCount; ++optionsIndex){
			const ::imtgql::CGqlParamObject* optionsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("options", optionsIndex);
			if (optionsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << optionsDataObjectPtr;
				return false;
			}
			COption::V1_0 tempOptions;
			if (!tempOptions.ReadFromGraphQlObject(*optionsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "options").toLocal8Bit().constData();)

				return false;
			}
			options->append(tempOptions);
		}
	}

	return true;
}


bool COptionsList::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("totalCount") && (gqlObject["totalCount"].userType() == QMetaType::Int || gqlObject["totalCount"].userType() == QMetaType::UInt || gqlObject["totalCount"].userType() == QMetaType::LongLong || gqlObject["totalCount"].userType() == QMetaType::ULongLong || gqlObject["totalCount"].userType() == QMetaType::Long || gqlObject["totalCount"].userType() == QMetaType::Short || gqlObject["totalCount"].userType() == QMetaType::ULong || gqlObject["totalCount"].userType() == QMetaType::UShort || gqlObject["totalCount"].userType() == QMetaType::UChar)){
		totalCount = gqlObject["totalCount"].toInt();
	}

	if (gqlObject.ContainsParam("offset") && (gqlObject["offset"].userType() == QMetaType::Int || gqlObject["offset"].userType() == QMetaType::UInt || gqlObject["offset"].userType() == QMetaType::LongLong || gqlObject["offset"].userType() == QMetaType::ULongLong || gqlObject["offset"].userType() == QMetaType::Long || gqlObject["offset"].userType() == QMetaType::Short || gqlObject["offset"].userType() == QMetaType::ULong || gqlObject["offset"].userType() == QMetaType::UShort || gqlObject["offset"].userType() == QMetaType::UChar)){
		offset = gqlObject["offset"].toInt();
	}

	if (gqlObject.ContainsParam("options")){
		options.emplace();
	}
	if (gqlObject.ContainsParam("options") && (gqlObject.GetObjectsCount("options") > 0)){
		const qsizetype optionsElementsCount = gqlObject.GetObjectsCount("options");
		options = imtsdl::TElementList<COption::V1_0>();
		for (qsizetype optionsIndex = 0; optionsIndex < optionsElementsCount; ++optionsIndex){
			const ::imtgql::CGqlParamObject* optionsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("options", optionsIndex);
			if (optionsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << optionsDataObjectPtr;
				return false;
			}
			COption::V1_0 tempOptions;
			if (!tempOptions.OptReadFromGraphQlObject(*optionsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "options").toLocal8Bit().constData();)

				return false;
			}
			options->append(tempOptions);
		}
	}

	return true;
}


bool COptionsList::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (totalCount){
		jsonObject["totalCount"] = QJsonValue::fromVariant(*totalCount);
	}

	if (offset){
		jsonObject["offset"] = QJsonValue::fromVariant(*offset);
	}

	if (options){
		QJsonArray newOptionsArray;
		for (qsizetype optionsIndex = 0; optionsIndex < options->size(); ++optionsIndex){
			QJsonObject newOptionsJsonObject;
			if (!options->at(optionsIndex)->WriteToJsonObject(newOptionsJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "options").toLocal8Bit().constData();)

				return false;
			}
			newOptionsArray << newOptionsJsonObject;
		}
		jsonObject["options"] = newOptionsArray;
	}

	jsonObject["__typename"] = "OptionsList";

	return true;
}


bool COptionsList::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("totalCount") && jsonObject["totalCount"].isDouble()){
		totalCount = jsonObject["totalCount"].toInt();
	}

	if (jsonObject.contains("offset") && jsonObject["offset"].isDouble()){
		offset = jsonObject["offset"].toInt();
	}

	if (jsonObject.contains("options") && jsonObject["options"].isArray()){
		const QJsonArray optionsJsonArray = jsonObject["options"].toArray();
		const qsizetype optionsArrayCount = optionsJsonArray.size();
		options = imtsdl::TElementList<COption::V1_0>();
		for (qsizetype optionsIndex = 0; optionsIndex < optionsArrayCount; ++optionsIndex){
			COption::V1_0 tempOptions;
			if (!tempOptions.ReadFromJsonObject(optionsJsonArray[optionsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "options").toLocal8Bit().constData();)

				return false;
			}
			options->append(tempOptions);
		}
	}

	return true;
}


bool COptionsList::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("totalCount") && jsonObject["totalCount"].isDouble()){
		totalCount = jsonObject["totalCount"].toInt();
	}

	if (jsonObject.contains("offset") && jsonObject["offset"].isDouble()){
		offset = jsonObject["offset"].toInt();
	}

	if (jsonObject.contains("options") && jsonObject["options"].isArray()){
		const QJsonArray optionsJsonArray = jsonObject["options"].toArray();
		const qsizetype optionsArrayCount = optionsJsonArray.size();
		options = imtsdl::TElementList<COption::V1_0>();
		for (qsizetype optionsIndex = 0; optionsIndex < optionsArrayCount; ++optionsIndex){
			COption::V1_0 tempOptions;
			if (!tempOptions.OptReadFromJsonObject(optionsJsonArray[optionsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "options").toLocal8Bit().constData();)

				return false;
			}
			options->append(tempOptions);
		}
	}

	return true;
}


// serialize methods

QByteArray CSelectionParam::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSelectionParam::operator==(const CSelectionParam& other) const
{
	return 
				selectedIndex == other.selectedIndex &&
				constraints == other.constraints;
}


bool CSelectionParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "SelectionParam", modelIndex);

	if (selectedIndex){
		model.SetData("selectedIndex", *selectedIndex, modelIndex);
	}


	if (constraints){
		::imtbase::CTreeItemModel* constraintsNewModelPtr = model.AddTreeModel("constraints", modelIndex);
		const bool isConstraintsAdded = constraints->WriteToModel(*constraintsNewModelPtr, 0);
		if (!isConstraintsAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "constraints").toLocal8Bit().constData();)

			return false;
		}

	}

	return true;
}


bool CSelectionParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant selectedIndexData = model.GetData("selectedIndex", modelIndex);
	if (!selectedIndexData.isNull()){
		selectedIndex = selectedIndexData.toInt();
	}

	::imtbase::CTreeItemModel* constraintsDataModelPtr = model.GetTreeItemModel("constraints", modelIndex);
	if (constraintsDataModelPtr != nullptr){
		constraints = COptionsList::V1_0();
		const bool isConstraintsRead = constraints->ReadFromModel(*constraintsDataModelPtr, modelIndex);
		if (!isConstraintsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "constraints").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CSelectionParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant selectedIndexData = model.GetData("selectedIndex", modelIndex);
	if (!selectedIndexData.isNull()){
		selectedIndex = selectedIndexData.toInt();
	}

	::imtbase::CTreeItemModel* constraintsDataModelPtr = model.GetTreeItemModel("constraints", modelIndex);
	if (constraintsDataModelPtr != nullptr){
		constraints = COptionsList::V1_0();
		const bool isConstraintsRead = constraints->ReadFromModel(*constraintsDataModelPtr, modelIndex);
		if (!isConstraintsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "constraints").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CSelectionParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (selectedIndex){
		gqlObject.InsertParam("selectedIndex", QVariant(*selectedIndex));
	}

	if (constraints){
		::imtgql::CGqlParamObject constraintsGqlObject;
		const bool isConstraintsAdded = constraints->WriteToGraphQlObject(constraintsGqlObject);
		if (!isConstraintsAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "constraints").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("constraints", constraintsGqlObject);
	}

	gqlObject.InsertParam("__typename", QVariant("SelectionParam"));

	return true;
}


bool CSelectionParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("selectedIndex") && (gqlObject["selectedIndex"].userType() == QMetaType::Int || gqlObject["selectedIndex"].userType() == QMetaType::UInt || gqlObject["selectedIndex"].userType() == QMetaType::LongLong || gqlObject["selectedIndex"].userType() == QMetaType::ULongLong || gqlObject["selectedIndex"].userType() == QMetaType::Long || gqlObject["selectedIndex"].userType() == QMetaType::Short || gqlObject["selectedIndex"].userType() == QMetaType::ULong || gqlObject["selectedIndex"].userType() == QMetaType::UShort || gqlObject["selectedIndex"].userType() == QMetaType::UChar)){
		selectedIndex = gqlObject["selectedIndex"].toInt();
	}

	if (gqlObject.ContainsParam("constraints") && (gqlObject.GetParamArgumentObjectPtr("constraints") != nullptr)){
		constraints = COptionsList::V1_0();
		const bool isConstraintsRead = constraints->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("constraints"));
		if (!isConstraintsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "constraints").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CSelectionParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("selectedIndex") && (gqlObject["selectedIndex"].userType() == QMetaType::Int || gqlObject["selectedIndex"].userType() == QMetaType::UInt || gqlObject["selectedIndex"].userType() == QMetaType::LongLong || gqlObject["selectedIndex"].userType() == QMetaType::ULongLong || gqlObject["selectedIndex"].userType() == QMetaType::Long || gqlObject["selectedIndex"].userType() == QMetaType::Short || gqlObject["selectedIndex"].userType() == QMetaType::ULong || gqlObject["selectedIndex"].userType() == QMetaType::UShort || gqlObject["selectedIndex"].userType() == QMetaType::UChar)){
		selectedIndex = gqlObject["selectedIndex"].toInt();
	}

	if (gqlObject.ContainsParam("constraints") && (gqlObject.GetParamArgumentObjectPtr("constraints") != nullptr)){
		constraints = COptionsList::V1_0();
		const bool isConstraintsRead = constraints->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("constraints"));
		if (!isConstraintsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "constraints").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CSelectionParam::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (selectedIndex){
		jsonObject["selectedIndex"] = QJsonValue::fromVariant(*selectedIndex);
	}

	if (constraints){
		QJsonObject constraintsJsonObject;
		const bool isConstraintsAdded = constraints->WriteToJsonObject(constraintsJsonObject);
		if (!isConstraintsAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "constraints").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["constraints"] = constraintsJsonObject;
	}

	jsonObject["__typename"] = "SelectionParam";

	return true;
}


bool CSelectionParam::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("selectedIndex") && jsonObject["selectedIndex"].isDouble()){
		selectedIndex = jsonObject["selectedIndex"].toInt();
	}

	if (jsonObject.contains("constraints") && jsonObject["constraints"].isObject()){
		constraints = COptionsList::V1_0();
		const bool isConstraintsRead = constraints->ReadFromJsonObject(jsonObject["constraints"].toObject());
		if (!isConstraintsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "constraints").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


bool CSelectionParam::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("selectedIndex") && jsonObject["selectedIndex"].isDouble()){
		selectedIndex = jsonObject["selectedIndex"].toInt();
	}

	if (jsonObject.contains("constraints") && jsonObject["constraints"].isObject()){
		constraints = COptionsList::V1_0();
		const bool isConstraintsRead = constraints->OptReadFromJsonObject(jsonObject["constraints"].toObject());
		if (!isConstraintsRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "constraints").toLocal8Bit().constData();)

			return false;
		}
	}

	return true;
}


// serialize methods

QByteArray CSchedulerParam::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSchedulerParam::operator==(const CSchedulerParam& other) const
{
	return 
				startTime == other.startTime &&
				interval == other.interval;
}


bool CSchedulerParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "SchedulerParam", modelIndex);

	if (startTime){
		model.SetData("startTime", *startTime, modelIndex);
	}

	if (interval){
		model.SetData("interval", *interval, modelIndex);
	}


	return true;
}


bool CSchedulerParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant startTimeData = model.GetData("startTime", modelIndex);
	if (!startTimeData.isNull()){
		startTime = startTimeData.toString();
	}

	QVariant intervalData = model.GetData("interval", modelIndex);
	if (!intervalData.isNull()){
		interval = intervalData.toInt();
	}

	return true;
}


bool CSchedulerParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant startTimeData = model.GetData("startTime", modelIndex);
	if (!startTimeData.isNull()){
		startTime = startTimeData.toString();
	}

	QVariant intervalData = model.GetData("interval", modelIndex);
	if (!intervalData.isNull()){
		interval = intervalData.toInt();
	}

	return true;
}


bool CSchedulerParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (startTime){
		gqlObject.InsertParam("startTime", QVariant(*startTime));
	}

	if (interval){
		gqlObject.InsertParam("interval", QVariant(*interval));
	}

	gqlObject.InsertParam("__typename", QVariant("SchedulerParam"));

	return true;
}


bool CSchedulerParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("startTime") && (gqlObject["startTime"].userType() == QMetaType::QString || gqlObject["startTime"].userType() == QMetaType::QByteArray)){
		startTime = gqlObject["startTime"].toString();
	}

	if (gqlObject.ContainsParam("interval") && (gqlObject["interval"].userType() == QMetaType::Int || gqlObject["interval"].userType() == QMetaType::UInt || gqlObject["interval"].userType() == QMetaType::LongLong || gqlObject["interval"].userType() == QMetaType::ULongLong || gqlObject["interval"].userType() == QMetaType::Long || gqlObject["interval"].userType() == QMetaType::Short || gqlObject["interval"].userType() == QMetaType::ULong || gqlObject["interval"].userType() == QMetaType::UShort || gqlObject["interval"].userType() == QMetaType::UChar)){
		interval = gqlObject["interval"].toInt();
	}

	return true;
}


bool CSchedulerParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("startTime") && (gqlObject["startTime"].userType() == QMetaType::QString || gqlObject["startTime"].userType() == QMetaType::QByteArray)){
		startTime = gqlObject["startTime"].toString();
	}

	if (gqlObject.ContainsParam("interval") && (gqlObject["interval"].userType() == QMetaType::Int || gqlObject["interval"].userType() == QMetaType::UInt || gqlObject["interval"].userType() == QMetaType::LongLong || gqlObject["interval"].userType() == QMetaType::ULongLong || gqlObject["interval"].userType() == QMetaType::Long || gqlObject["interval"].userType() == QMetaType::Short || gqlObject["interval"].userType() == QMetaType::ULong || gqlObject["interval"].userType() == QMetaType::UShort || gqlObject["interval"].userType() == QMetaType::UChar)){
		interval = gqlObject["interval"].toInt();
	}

	return true;
}


bool CSchedulerParam::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (startTime){
		jsonObject["startTime"] = QJsonValue::fromVariant(*startTime);
	}

	if (interval){
		jsonObject["interval"] = QJsonValue::fromVariant(*interval);
	}

	jsonObject["__typename"] = "SchedulerParam";

	return true;
}


bool CSchedulerParam::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("startTime") && jsonObject["startTime"].isString()){
		startTime = jsonObject["startTime"].toString();
	}

	if (jsonObject.contains("interval") && jsonObject["interval"].isDouble()){
		interval = jsonObject["interval"].toInt();
	}

	return true;
}


bool CSchedulerParam::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("startTime") && jsonObject["startTime"].isString()){
		startTime = jsonObject["startTime"].toString();
	}

	if (jsonObject.contains("interval") && jsonObject["interval"].isDouble()){
		interval = jsonObject["interval"].toInt();
	}

	return true;
}


// serialize methods

QByteArray CBackupSettings::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CBackupSettings::operator==(const CBackupSettings& other) const
{
	return 
				schedulerParam == other.schedulerParam &&
				folderPath == other.folderPath;
}


bool CBackupSettings::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "BackupSettings", modelIndex);


	if (schedulerParam){
		::imtbase::CTreeItemModel* schedulerParamNewModelPtr = model.AddTreeModel("schedulerParam", modelIndex);
		const bool isSchedulerParamAdded = schedulerParam->WriteToModel(*schedulerParamNewModelPtr, 0);
		if (!isSchedulerParamAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam").toLocal8Bit().constData();)

			return false;
		}

	}
	if (folderPath){
		model.SetData("folderPath", *folderPath, modelIndex);
	}


	return true;
}


bool CBackupSettings::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* schedulerParamDataModelPtr = model.GetTreeItemModel("schedulerParam", modelIndex);
	if (schedulerParamDataModelPtr != nullptr){
		schedulerParam = CSchedulerParam::V1_0();
		const bool isSchedulerParamRead = schedulerParam->ReadFromModel(*schedulerParamDataModelPtr, modelIndex);
		if (!isSchedulerParamRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam").toLocal8Bit().constData();)

			return false;
		}
	}

	QVariant folderPathData = model.GetData("folderPath", modelIndex);
	if (!folderPathData.isNull()){
		folderPath = folderPathData.toString();
	}

	return true;
}


bool CBackupSettings::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* schedulerParamDataModelPtr = model.GetTreeItemModel("schedulerParam", modelIndex);
	if (schedulerParamDataModelPtr != nullptr){
		schedulerParam = CSchedulerParam::V1_0();
		const bool isSchedulerParamRead = schedulerParam->ReadFromModel(*schedulerParamDataModelPtr, modelIndex);
		if (!isSchedulerParamRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam").toLocal8Bit().constData();)

			return false;
		}
	}

	QVariant folderPathData = model.GetData("folderPath", modelIndex);
	if (!folderPathData.isNull()){
		folderPath = folderPathData.toString();
	}

	return true;
}


bool CBackupSettings::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (schedulerParam){
		::imtgql::CGqlParamObject schedulerParamGqlObject;
		const bool isSchedulerParamAdded = schedulerParam->WriteToGraphQlObject(schedulerParamGqlObject);
		if (!isSchedulerParamAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("schedulerParam", schedulerParamGqlObject);
	}

	if (folderPath){
		gqlObject.InsertParam("folderPath", QVariant(*folderPath));
	}

	gqlObject.InsertParam("__typename", QVariant("BackupSettings"));

	return true;
}


bool CBackupSettings::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("schedulerParam") && (gqlObject.GetParamArgumentObjectPtr("schedulerParam") != nullptr)){
		schedulerParam = CSchedulerParam::V1_0();
		const bool isSchedulerParamRead = schedulerParam->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("schedulerParam"));
		if (!isSchedulerParamRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("folderPath") && (gqlObject["folderPath"].userType() == QMetaType::QString || gqlObject["folderPath"].userType() == QMetaType::QByteArray)){
		folderPath = gqlObject["folderPath"].toString();
	}

	return true;
}


bool CBackupSettings::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("schedulerParam") && (gqlObject.GetParamArgumentObjectPtr("schedulerParam") != nullptr)){
		schedulerParam = CSchedulerParam::V1_0();
		const bool isSchedulerParamRead = schedulerParam->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("schedulerParam"));
		if (!isSchedulerParamRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("folderPath") && (gqlObject["folderPath"].userType() == QMetaType::QString || gqlObject["folderPath"].userType() == QMetaType::QByteArray)){
		folderPath = gqlObject["folderPath"].toString();
	}

	return true;
}


bool CBackupSettings::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (schedulerParam){
		QJsonObject schedulerParamJsonObject;
		const bool isSchedulerParamAdded = schedulerParam->WriteToJsonObject(schedulerParamJsonObject);
		if (!isSchedulerParamAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["schedulerParam"] = schedulerParamJsonObject;
	}

	if (folderPath){
		jsonObject["folderPath"] = QJsonValue::fromVariant(*folderPath);
	}

	jsonObject["__typename"] = "BackupSettings";

	return true;
}


bool CBackupSettings::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("schedulerParam") && jsonObject["schedulerParam"].isObject()){
		schedulerParam = CSchedulerParam::V1_0();
		const bool isSchedulerParamRead = schedulerParam->ReadFromJsonObject(jsonObject["schedulerParam"].toObject());
		if (!isSchedulerParamRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("folderPath") && jsonObject["folderPath"].isString()){
		folderPath = jsonObject["folderPath"].toString();
	}

	return true;
}


bool CBackupSettings::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("schedulerParam") && jsonObject["schedulerParam"].isObject()){
		schedulerParam = CSchedulerParam::V1_0();
		const bool isSchedulerParamRead = schedulerParam->OptReadFromJsonObject(jsonObject["schedulerParam"].toObject());
		if (!isSchedulerParamRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("folderPath") && jsonObject["folderPath"].isString()){
		folderPath = jsonObject["folderPath"].toString();
	}

	return true;
}


// serialize methods

QByteArray CDatabaseAccessSettings::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CDatabaseAccessSettings::operator==(const CDatabaseAccessSettings& other) const
{
	return 
				dbName == other.dbName &&
				host == other.host &&
				port == other.port &&
				dbPath == other.dbPath &&
				username == other.username &&
				password == other.password;
}


bool CDatabaseAccessSettings::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "DatabaseAccessSettings", modelIndex);

	if (dbName){
		model.SetData("dbName", *dbName, modelIndex);
	}

	if (host){
		model.SetData("host", *host, modelIndex);
	}

	if (port){
		model.SetData("port", *port, modelIndex);
	}

	if (dbPath){
		model.SetData("dbPath", *dbPath, modelIndex);
	}

	if (username){
		model.SetData("username", *username, modelIndex);
	}

	if (password){
		model.SetData("password", *password, modelIndex);
	}


	return true;
}


bool CDatabaseAccessSettings::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant dbNameData = model.GetData("dbName", modelIndex);
	if (!dbNameData.isNull()){
		dbName = dbNameData.toString();
	}

	QVariant hostData = model.GetData("host", modelIndex);
	if (!hostData.isNull()){
		host = hostData.toString();
	}

	QVariant portData = model.GetData("port", modelIndex);
	if (!portData.isNull()){
		port = portData.toInt();
	}

	QVariant dbPathData = model.GetData("dbPath", modelIndex);
	if (!dbPathData.isNull()){
		dbPath = dbPathData.toString();
	}

	QVariant usernameData = model.GetData("username", modelIndex);
	if (!usernameData.isNull()){
		username = usernameData.toString();
	}

	QVariant passwordData = model.GetData("password", modelIndex);
	if (!passwordData.isNull()){
		password = passwordData.toString();
	}

	return true;
}


bool CDatabaseAccessSettings::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant dbNameData = model.GetData("dbName", modelIndex);
	if (!dbNameData.isNull()){
		dbName = dbNameData.toString();
	}

	QVariant hostData = model.GetData("host", modelIndex);
	if (!hostData.isNull()){
		host = hostData.toString();
	}

	QVariant portData = model.GetData("port", modelIndex);
	if (!portData.isNull()){
		port = portData.toInt();
	}

	QVariant dbPathData = model.GetData("dbPath", modelIndex);
	if (!dbPathData.isNull()){
		dbPath = dbPathData.toString();
	}

	QVariant usernameData = model.GetData("username", modelIndex);
	if (!usernameData.isNull()){
		username = usernameData.toString();
	}

	QVariant passwordData = model.GetData("password", modelIndex);
	if (!passwordData.isNull()){
		password = passwordData.toString();
	}

	return true;
}


bool CDatabaseAccessSettings::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (dbName){
		gqlObject.InsertParam("dbName", QVariant(*dbName));
	}

	if (host){
		gqlObject.InsertParam("host", QVariant(*host));
	}

	if (port){
		gqlObject.InsertParam("port", QVariant(*port));
	}

	if (dbPath){
		gqlObject.InsertParam("dbPath", QVariant(*dbPath));
	}

	if (username){
		gqlObject.InsertParam("username", QVariant(*username));
	}

	if (password){
		gqlObject.InsertParam("password", QVariant(*password));
	}

	gqlObject.InsertParam("__typename", QVariant("DatabaseAccessSettings"));

	return true;
}


bool CDatabaseAccessSettings::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("dbName") && (gqlObject["dbName"].userType() == QMetaType::QString || gqlObject["dbName"].userType() == QMetaType::QByteArray)){
		dbName = gqlObject["dbName"].toString();
	}

	if (gqlObject.ContainsParam("host") && (gqlObject["host"].userType() == QMetaType::QString || gqlObject["host"].userType() == QMetaType::QByteArray)){
		host = gqlObject["host"].toString();
	}

	if (gqlObject.ContainsParam("port") && (gqlObject["port"].userType() == QMetaType::Int || gqlObject["port"].userType() == QMetaType::UInt || gqlObject["port"].userType() == QMetaType::LongLong || gqlObject["port"].userType() == QMetaType::ULongLong || gqlObject["port"].userType() == QMetaType::Long || gqlObject["port"].userType() == QMetaType::Short || gqlObject["port"].userType() == QMetaType::ULong || gqlObject["port"].userType() == QMetaType::UShort || gqlObject["port"].userType() == QMetaType::UChar)){
		port = gqlObject["port"].toInt();
	}

	if (gqlObject.ContainsParam("dbPath") && (gqlObject["dbPath"].userType() == QMetaType::QString || gqlObject["dbPath"].userType() == QMetaType::QByteArray)){
		dbPath = gqlObject["dbPath"].toString();
	}

	if (gqlObject.ContainsParam("username") && (gqlObject["username"].userType() == QMetaType::QString || gqlObject["username"].userType() == QMetaType::QByteArray)){
		username = gqlObject["username"].toString();
	}

	if (gqlObject.ContainsParam("password") && (gqlObject["password"].userType() == QMetaType::QString || gqlObject["password"].userType() == QMetaType::QByteArray)){
		password = gqlObject["password"].toString();
	}

	return true;
}


bool CDatabaseAccessSettings::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("dbName") && (gqlObject["dbName"].userType() == QMetaType::QString || gqlObject["dbName"].userType() == QMetaType::QByteArray)){
		dbName = gqlObject["dbName"].toString();
	}

	if (gqlObject.ContainsParam("host") && (gqlObject["host"].userType() == QMetaType::QString || gqlObject["host"].userType() == QMetaType::QByteArray)){
		host = gqlObject["host"].toString();
	}

	if (gqlObject.ContainsParam("port") && (gqlObject["port"].userType() == QMetaType::Int || gqlObject["port"].userType() == QMetaType::UInt || gqlObject["port"].userType() == QMetaType::LongLong || gqlObject["port"].userType() == QMetaType::ULongLong || gqlObject["port"].userType() == QMetaType::Long || gqlObject["port"].userType() == QMetaType::Short || gqlObject["port"].userType() == QMetaType::ULong || gqlObject["port"].userType() == QMetaType::UShort || gqlObject["port"].userType() == QMetaType::UChar)){
		port = gqlObject["port"].toInt();
	}

	if (gqlObject.ContainsParam("dbPath") && (gqlObject["dbPath"].userType() == QMetaType::QString || gqlObject["dbPath"].userType() == QMetaType::QByteArray)){
		dbPath = gqlObject["dbPath"].toString();
	}

	if (gqlObject.ContainsParam("username") && (gqlObject["username"].userType() == QMetaType::QString || gqlObject["username"].userType() == QMetaType::QByteArray)){
		username = gqlObject["username"].toString();
	}

	if (gqlObject.ContainsParam("password") && (gqlObject["password"].userType() == QMetaType::QString || gqlObject["password"].userType() == QMetaType::QByteArray)){
		password = gqlObject["password"].toString();
	}

	return true;
}


bool CDatabaseAccessSettings::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (dbName){
		jsonObject["dbName"] = QJsonValue::fromVariant(*dbName);
	}

	if (host){
		jsonObject["host"] = QJsonValue::fromVariant(*host);
	}

	if (port){
		jsonObject["port"] = QJsonValue::fromVariant(*port);
	}

	if (dbPath){
		jsonObject["dbPath"] = QJsonValue::fromVariant(*dbPath);
	}

	if (username){
		jsonObject["username"] = QJsonValue::fromVariant(*username);
	}

	if (password){
		jsonObject["password"] = QJsonValue::fromVariant(*password);
	}

	jsonObject["__typename"] = "DatabaseAccessSettings";

	return true;
}


bool CDatabaseAccessSettings::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("dbName") && jsonObject["dbName"].isString()){
		dbName = jsonObject["dbName"].toString();
	}

	if (jsonObject.contains("host") && jsonObject["host"].isString()){
		host = jsonObject["host"].toString();
	}

	if (jsonObject.contains("port") && jsonObject["port"].isDouble()){
		port = jsonObject["port"].toInt();
	}

	if (jsonObject.contains("dbPath") && jsonObject["dbPath"].isString()){
		dbPath = jsonObject["dbPath"].toString();
	}

	if (jsonObject.contains("username") && jsonObject["username"].isString()){
		username = jsonObject["username"].toString();
	}

	if (jsonObject.contains("password") && jsonObject["password"].isString()){
		password = jsonObject["password"].toString();
	}

	return true;
}


bool CDatabaseAccessSettings::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("dbName") && jsonObject["dbName"].isString()){
		dbName = jsonObject["dbName"].toString();
	}

	if (jsonObject.contains("host") && jsonObject["host"].isString()){
		host = jsonObject["host"].toString();
	}

	if (jsonObject.contains("port") && jsonObject["port"].isDouble()){
		port = jsonObject["port"].toInt();
	}

	if (jsonObject.contains("dbPath") && jsonObject["dbPath"].isString()){
		dbPath = jsonObject["dbPath"].toString();
	}

	if (jsonObject.contains("username") && jsonObject["username"].isString()){
		username = jsonObject["username"].toString();
	}

	if (jsonObject.contains("password") && jsonObject["password"].isString()){
		password = jsonObject["password"].toString();
	}

	return true;
}


// serialize methods

QByteArray CFileNameParam::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CFileNameParam::operator==(const CFileNameParam& other) const
{
	return 
				pathType == other.pathType &&
				path == other.path;
}


bool CFileNameParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "FileNameParam", modelIndex);

	if (pathType){
		model.SetData("pathType", *pathType, modelIndex);
	}

	if (path){
		model.SetData("path", *path, modelIndex);
	}


	return true;
}


bool CFileNameParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant pathTypeData = model.GetData("pathType", modelIndex);
	if (!pathTypeData.isNull()){
		pathType = pathTypeData.toInt();
	}

	QVariant pathData = model.GetData("path", modelIndex);
	if (!pathData.isNull()){
		path = pathData.toString();
	}

	return true;
}


bool CFileNameParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant pathTypeData = model.GetData("pathType", modelIndex);
	if (!pathTypeData.isNull()){
		pathType = pathTypeData.toInt();
	}

	QVariant pathData = model.GetData("path", modelIndex);
	if (!pathData.isNull()){
		path = pathData.toString();
	}

	return true;
}


bool CFileNameParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (pathType){
		gqlObject.InsertParam("pathType", QVariant(*pathType));
	}

	if (path){
		gqlObject.InsertParam("path", QVariant(*path));
	}

	gqlObject.InsertParam("__typename", QVariant("FileNameParam"));

	return true;
}


bool CFileNameParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("pathType") && (gqlObject["pathType"].userType() == QMetaType::Int || gqlObject["pathType"].userType() == QMetaType::UInt || gqlObject["pathType"].userType() == QMetaType::LongLong || gqlObject["pathType"].userType() == QMetaType::ULongLong || gqlObject["pathType"].userType() == QMetaType::Long || gqlObject["pathType"].userType() == QMetaType::Short || gqlObject["pathType"].userType() == QMetaType::ULong || gqlObject["pathType"].userType() == QMetaType::UShort || gqlObject["pathType"].userType() == QMetaType::UChar)){
		pathType = gqlObject["pathType"].toInt();
	}

	if (gqlObject.ContainsParam("path") && (gqlObject["path"].userType() == QMetaType::QString || gqlObject["path"].userType() == QMetaType::QByteArray)){
		path = gqlObject["path"].toString();
	}

	return true;
}


bool CFileNameParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("pathType") && (gqlObject["pathType"].userType() == QMetaType::Int || gqlObject["pathType"].userType() == QMetaType::UInt || gqlObject["pathType"].userType() == QMetaType::LongLong || gqlObject["pathType"].userType() == QMetaType::ULongLong || gqlObject["pathType"].userType() == QMetaType::Long || gqlObject["pathType"].userType() == QMetaType::Short || gqlObject["pathType"].userType() == QMetaType::ULong || gqlObject["pathType"].userType() == QMetaType::UShort || gqlObject["pathType"].userType() == QMetaType::UChar)){
		pathType = gqlObject["pathType"].toInt();
	}

	if (gqlObject.ContainsParam("path") && (gqlObject["path"].userType() == QMetaType::QString || gqlObject["path"].userType() == QMetaType::QByteArray)){
		path = gqlObject["path"].toString();
	}

	return true;
}


bool CFileNameParam::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (pathType){
		jsonObject["pathType"] = QJsonValue::fromVariant(*pathType);
	}

	if (path){
		jsonObject["path"] = QJsonValue::fromVariant(*path);
	}

	jsonObject["__typename"] = "FileNameParam";

	return true;
}


bool CFileNameParam::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("pathType") && jsonObject["pathType"].isDouble()){
		pathType = jsonObject["pathType"].toInt();
	}

	if (jsonObject.contains("path") && jsonObject["path"].isString()){
		path = jsonObject["path"].toString();
	}

	return true;
}


bool CFileNameParam::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("pathType") && jsonObject["pathType"].isDouble()){
		pathType = jsonObject["pathType"].toInt();
	}

	if (jsonObject.contains("path") && jsonObject["path"].isString()){
		path = jsonObject["path"].toString();
	}

	return true;
}


// serialize methods

QByteArray CParamsSet::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CParamsSet::operator==(const CParamsSet& other) const
{
	return 
				paramIds == other.paramIds &&
				paramTypeIds == other.paramTypeIds &&
				paramNames == other.paramNames &&
				paramDescriptions == other.paramDescriptions &&
				parameters == other.parameters;
}


bool CParamsSet::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "ParamsSet", modelIndex);

	if (paramIds){
		::imtbase::CTreeItemModel* newParamIdsModelPtr = model.AddTreeModel("paramIds", modelIndex);
		newParamIdsModelPtr->setIsArray(true);
		for (qsizetype paramIdsIndex = 0; paramIdsIndex < paramIds->size(); ++paramIdsIndex){
			newParamIdsModelPtr->InsertNewItem();
			newParamIdsModelPtr->SetData(QByteArray(), *paramIds->at(paramIdsIndex), paramIdsIndex);
		}
	}

	if (paramTypeIds){
		::imtbase::CTreeItemModel* newParamTypeIdsModelPtr = model.AddTreeModel("paramTypeIds", modelIndex);
		newParamTypeIdsModelPtr->setIsArray(true);
		for (qsizetype paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIds->size(); ++paramTypeIdsIndex){
			newParamTypeIdsModelPtr->InsertNewItem();
			newParamTypeIdsModelPtr->SetData(QByteArray(), *paramTypeIds->at(paramTypeIdsIndex), paramTypeIdsIndex);
		}
	}

	if (paramNames){
		::imtbase::CTreeItemModel* newParamNamesModelPtr = model.AddTreeModel("paramNames", modelIndex);
		newParamNamesModelPtr->setIsArray(true);
		for (qsizetype paramNamesIndex = 0; paramNamesIndex < paramNames->size(); ++paramNamesIndex){
			newParamNamesModelPtr->InsertNewItem();
			newParamNamesModelPtr->SetData(QByteArray(), *paramNames->at(paramNamesIndex), paramNamesIndex);
		}
	}

	if (paramDescriptions){
		::imtbase::CTreeItemModel* newParamDescriptionsModelPtr = model.AddTreeModel("paramDescriptions", modelIndex);
		newParamDescriptionsModelPtr->setIsArray(true);
		for (qsizetype paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptions->size(); ++paramDescriptionsIndex){
			newParamDescriptionsModelPtr->InsertNewItem();
			newParamDescriptionsModelPtr->SetData(QByteArray(), *paramDescriptions->at(paramDescriptionsIndex), paramDescriptionsIndex);
		}
	}

	if (parameters){
		::imtbase::CTreeItemModel* newParametersModelPtr = model.AddTreeModel("parameters", modelIndex);
		newParametersModelPtr->setIsArray(true);
		for (qsizetype parametersIndex = 0; parametersIndex < parameters->size(); ++parametersIndex){
			newParametersModelPtr->InsertNewItem();
			newParametersModelPtr->SetData(QByteArray(), *parameters->at(parametersIndex), parametersIndex);
		}
	}


	return true;
}


bool CParamsSet::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* paramIdsModel = model.GetTreeItemModel("paramIds", modelIndex);
	if (paramIdsModel != nullptr){
		int paramIdsCount = paramIdsModel->GetItemsCount();
		imtsdl::TElementList<QByteArray> paramIdsList;
		for (int paramIdsIndex = 0; paramIdsIndex < paramIdsCount; ++paramIdsIndex){
			QByteArray t_paramIds = paramIdsModel->GetData(QByteArray(), paramIdsIndex).toByteArray();
			paramIdsList << t_paramIds;
		}
		paramIds = paramIdsList;

	}

	::imtbase::CTreeItemModel* paramTypeIdsModel = model.GetTreeItemModel("paramTypeIds", modelIndex);
	if (paramTypeIdsModel != nullptr){
		int paramTypeIdsCount = paramTypeIdsModel->GetItemsCount();
		imtsdl::TElementList<QByteArray> paramTypeIdsList;
		for (int paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIdsCount; ++paramTypeIdsIndex){
			QByteArray t_paramTypeIds = paramTypeIdsModel->GetData(QByteArray(), paramTypeIdsIndex).toByteArray();
			paramTypeIdsList << t_paramTypeIds;
		}
		paramTypeIds = paramTypeIdsList;

	}

	::imtbase::CTreeItemModel* paramNamesModel = model.GetTreeItemModel("paramNames", modelIndex);
	if (paramNamesModel != nullptr){
		int paramNamesCount = paramNamesModel->GetItemsCount();
		imtsdl::TElementList<QString> paramNamesList;
		for (int paramNamesIndex = 0; paramNamesIndex < paramNamesCount; ++paramNamesIndex){
			QString t_paramNames = paramNamesModel->GetData(QByteArray(), paramNamesIndex).toString();
			paramNamesList << t_paramNames;
		}
		paramNames = paramNamesList;

	}

	::imtbase::CTreeItemModel* paramDescriptionsModel = model.GetTreeItemModel("paramDescriptions", modelIndex);
	if (paramDescriptionsModel != nullptr){
		int paramDescriptionsCount = paramDescriptionsModel->GetItemsCount();
		imtsdl::TElementList<QString> paramDescriptionsList;
		for (int paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptionsCount; ++paramDescriptionsIndex){
			QString t_paramDescriptions = paramDescriptionsModel->GetData(QByteArray(), paramDescriptionsIndex).toString();
			paramDescriptionsList << t_paramDescriptions;
		}
		paramDescriptions = paramDescriptionsList;

	}

	::imtbase::CTreeItemModel* parametersModel = model.GetTreeItemModel("parameters", modelIndex);
	if (parametersModel != nullptr){
		int parametersCount = parametersModel->GetItemsCount();
		imtsdl::TElementList<QString> parametersList;
		for (int parametersIndex = 0; parametersIndex < parametersCount; ++parametersIndex){
			QString t_parameters = parametersModel->GetData(QByteArray(), parametersIndex).toString();
			parametersList << t_parameters;
		}
		parameters = parametersList;

	}

	return true;
}


bool CParamsSet::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* paramIdsModel = model.GetTreeItemModel("paramIds", modelIndex);
	if (paramIdsModel != nullptr){
		int paramIdsCount = paramIdsModel->GetItemsCount();
		imtsdl::TElementList<QByteArray> paramIdsList;
		for (int paramIdsIndex = 0; paramIdsIndex < paramIdsCount; ++paramIdsIndex){
			QByteArray t_paramIds = paramIdsModel->GetData(QByteArray(), paramIdsIndex).toByteArray();
			paramIdsList << t_paramIds;
		}
		paramIds = paramIdsList;

	}

	::imtbase::CTreeItemModel* paramTypeIdsModel = model.GetTreeItemModel("paramTypeIds", modelIndex);
	if (paramTypeIdsModel != nullptr){
		int paramTypeIdsCount = paramTypeIdsModel->GetItemsCount();
		imtsdl::TElementList<QByteArray> paramTypeIdsList;
		for (int paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIdsCount; ++paramTypeIdsIndex){
			QByteArray t_paramTypeIds = paramTypeIdsModel->GetData(QByteArray(), paramTypeIdsIndex).toByteArray();
			paramTypeIdsList << t_paramTypeIds;
		}
		paramTypeIds = paramTypeIdsList;

	}

	::imtbase::CTreeItemModel* paramNamesModel = model.GetTreeItemModel("paramNames", modelIndex);
	if (paramNamesModel != nullptr){
		int paramNamesCount = paramNamesModel->GetItemsCount();
		imtsdl::TElementList<QString> paramNamesList;
		for (int paramNamesIndex = 0; paramNamesIndex < paramNamesCount; ++paramNamesIndex){
			QString t_paramNames = paramNamesModel->GetData(QByteArray(), paramNamesIndex).toString();
			paramNamesList << t_paramNames;
		}
		paramNames = paramNamesList;

	}

	::imtbase::CTreeItemModel* paramDescriptionsModel = model.GetTreeItemModel("paramDescriptions", modelIndex);
	if (paramDescriptionsModel != nullptr){
		int paramDescriptionsCount = paramDescriptionsModel->GetItemsCount();
		imtsdl::TElementList<QString> paramDescriptionsList;
		for (int paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptionsCount; ++paramDescriptionsIndex){
			QString t_paramDescriptions = paramDescriptionsModel->GetData(QByteArray(), paramDescriptionsIndex).toString();
			paramDescriptionsList << t_paramDescriptions;
		}
		paramDescriptions = paramDescriptionsList;

	}

	::imtbase::CTreeItemModel* parametersModel = model.GetTreeItemModel("parameters", modelIndex);
	if (parametersModel != nullptr){
		int parametersCount = parametersModel->GetItemsCount();
		imtsdl::TElementList<QString> parametersList;
		for (int parametersIndex = 0; parametersIndex < parametersCount; ++parametersIndex){
			QString t_parameters = parametersModel->GetData(QByteArray(), parametersIndex).toString();
			parametersList << t_parameters;
		}
		parameters = parametersList;

	}

	return true;
}


bool CParamsSet::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (paramIds){
		QVariantList paramIdsDataObjectList;
		for (qsizetype paramIdsIndex = 0; paramIdsIndex < paramIds->size(); ++paramIdsIndex){
			paramIdsDataObjectList << *paramIds->at(paramIdsIndex);
		}
		gqlObject.InsertParam("paramIds", paramIdsDataObjectList);
	}

	if (paramTypeIds){
		QVariantList paramTypeIdsDataObjectList;
		for (qsizetype paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIds->size(); ++paramTypeIdsIndex){
			paramTypeIdsDataObjectList << *paramTypeIds->at(paramTypeIdsIndex);
		}
		gqlObject.InsertParam("paramTypeIds", paramTypeIdsDataObjectList);
	}

	if (paramNames){
		QVariantList paramNamesDataObjectList;
		for (qsizetype paramNamesIndex = 0; paramNamesIndex < paramNames->size(); ++paramNamesIndex){
			paramNamesDataObjectList << *paramNames->at(paramNamesIndex);
		}
		gqlObject.InsertParam("paramNames", paramNamesDataObjectList);
	}

	if (paramDescriptions){
		QVariantList paramDescriptionsDataObjectList;
		for (qsizetype paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptions->size(); ++paramDescriptionsIndex){
			paramDescriptionsDataObjectList << *paramDescriptions->at(paramDescriptionsIndex);
		}
		gqlObject.InsertParam("paramDescriptions", paramDescriptionsDataObjectList);
	}

	if (parameters){
		QVariantList parametersDataObjectList;
		for (qsizetype parametersIndex = 0; parametersIndex < parameters->size(); ++parametersIndex){
			parametersDataObjectList << *parameters->at(parametersIndex);
		}
		gqlObject.InsertParam("parameters", parametersDataObjectList);
	}

	gqlObject.InsertParam("__typename", QVariant("ParamsSet"));

	return true;
}


bool CParamsSet::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("paramIds")){
		paramIds.emplace();
	}
	if (gqlObject.ContainsParam("paramIds") && !(gqlObject["paramIds"].isNull())){
		const QVariant paramIdsData = gqlObject["paramIds"];
		const QVariantList paramIdsDataList = paramIdsData.toList();
		const qsizetype paramIdsElementsCount = paramIdsDataList.size();
		paramIds = imtsdl::TElementList<QByteArray>();
		for (qsizetype paramIdsIndex = 0; paramIdsIndex < paramIdsElementsCount; ++paramIdsIndex){
			QByteArray tempParamIds = paramIdsDataList[paramIdsIndex].toByteArray();
			paramIds->append(tempParamIds);
		}
	}

	if (gqlObject.ContainsParam("paramTypeIds")){
		paramTypeIds.emplace();
	}
	if (gqlObject.ContainsParam("paramTypeIds") && !(gqlObject["paramTypeIds"].isNull())){
		const QVariant paramTypeIdsData = gqlObject["paramTypeIds"];
		const QVariantList paramTypeIdsDataList = paramTypeIdsData.toList();
		const qsizetype paramTypeIdsElementsCount = paramTypeIdsDataList.size();
		paramTypeIds = imtsdl::TElementList<QByteArray>();
		for (qsizetype paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIdsElementsCount; ++paramTypeIdsIndex){
			QByteArray tempParamTypeIds = paramTypeIdsDataList[paramTypeIdsIndex].toByteArray();
			paramTypeIds->append(tempParamTypeIds);
		}
	}

	if (gqlObject.ContainsParam("paramNames")){
		paramNames.emplace();
	}
	if (gqlObject.ContainsParam("paramNames") && !(gqlObject["paramNames"].isNull())){
		const QVariant paramNamesData = gqlObject["paramNames"];
		const QVariantList paramNamesDataList = paramNamesData.toList();
		const qsizetype paramNamesElementsCount = paramNamesDataList.size();
		paramNames = imtsdl::TElementList<QString>();
		for (qsizetype paramNamesIndex = 0; paramNamesIndex < paramNamesElementsCount; ++paramNamesIndex){
			QString tempParamNames = paramNamesDataList[paramNamesIndex].toString();
			paramNames->append(tempParamNames);
		}
	}

	if (gqlObject.ContainsParam("paramDescriptions")){
		paramDescriptions.emplace();
	}
	if (gqlObject.ContainsParam("paramDescriptions") && !(gqlObject["paramDescriptions"].isNull())){
		const QVariant paramDescriptionsData = gqlObject["paramDescriptions"];
		const QVariantList paramDescriptionsDataList = paramDescriptionsData.toList();
		const qsizetype paramDescriptionsElementsCount = paramDescriptionsDataList.size();
		paramDescriptions = imtsdl::TElementList<QString>();
		for (qsizetype paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptionsElementsCount; ++paramDescriptionsIndex){
			QString tempParamDescriptions = paramDescriptionsDataList[paramDescriptionsIndex].toString();
			paramDescriptions->append(tempParamDescriptions);
		}
	}

	if (gqlObject.ContainsParam("parameters")){
		parameters.emplace();
	}
	if (gqlObject.ContainsParam("parameters") && !(gqlObject["parameters"].isNull())){
		const QVariant parametersData = gqlObject["parameters"];
		const QVariantList parametersDataList = parametersData.toList();
		const qsizetype parametersElementsCount = parametersDataList.size();
		parameters = imtsdl::TElementList<QString>();
		for (qsizetype parametersIndex = 0; parametersIndex < parametersElementsCount; ++parametersIndex){
			QString tempParameters = parametersDataList[parametersIndex].toString();
			parameters->append(tempParameters);
		}
	}

	return true;
}


bool CParamsSet::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("paramIds")){
		paramIds.emplace();
	}
	if (gqlObject.ContainsParam("paramIds") && !(gqlObject["paramIds"].isNull())){
		const QVariant paramIdsData = gqlObject["paramIds"];
		const QVariantList paramIdsDataList = paramIdsData.toList();
		const qsizetype paramIdsElementsCount = paramIdsDataList.size();
		paramIds = imtsdl::TElementList<QByteArray>();
		for (qsizetype paramIdsIndex = 0; paramIdsIndex < paramIdsElementsCount; ++paramIdsIndex){
			QByteArray tempParamIds = paramIdsDataList[paramIdsIndex].toByteArray();
			paramIds->append(tempParamIds);
		}
	}

	if (gqlObject.ContainsParam("paramTypeIds")){
		paramTypeIds.emplace();
	}
	if (gqlObject.ContainsParam("paramTypeIds") && !(gqlObject["paramTypeIds"].isNull())){
		const QVariant paramTypeIdsData = gqlObject["paramTypeIds"];
		const QVariantList paramTypeIdsDataList = paramTypeIdsData.toList();
		const qsizetype paramTypeIdsElementsCount = paramTypeIdsDataList.size();
		paramTypeIds = imtsdl::TElementList<QByteArray>();
		for (qsizetype paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIdsElementsCount; ++paramTypeIdsIndex){
			QByteArray tempParamTypeIds = paramTypeIdsDataList[paramTypeIdsIndex].toByteArray();
			paramTypeIds->append(tempParamTypeIds);
		}
	}

	if (gqlObject.ContainsParam("paramNames")){
		paramNames.emplace();
	}
	if (gqlObject.ContainsParam("paramNames") && !(gqlObject["paramNames"].isNull())){
		const QVariant paramNamesData = gqlObject["paramNames"];
		const QVariantList paramNamesDataList = paramNamesData.toList();
		const qsizetype paramNamesElementsCount = paramNamesDataList.size();
		paramNames = imtsdl::TElementList<QString>();
		for (qsizetype paramNamesIndex = 0; paramNamesIndex < paramNamesElementsCount; ++paramNamesIndex){
			QString tempParamNames = paramNamesDataList[paramNamesIndex].toString();
			paramNames->append(tempParamNames);
		}
	}

	if (gqlObject.ContainsParam("paramDescriptions")){
		paramDescriptions.emplace();
	}
	if (gqlObject.ContainsParam("paramDescriptions") && !(gqlObject["paramDescriptions"].isNull())){
		const QVariant paramDescriptionsData = gqlObject["paramDescriptions"];
		const QVariantList paramDescriptionsDataList = paramDescriptionsData.toList();
		const qsizetype paramDescriptionsElementsCount = paramDescriptionsDataList.size();
		paramDescriptions = imtsdl::TElementList<QString>();
		for (qsizetype paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptionsElementsCount; ++paramDescriptionsIndex){
			QString tempParamDescriptions = paramDescriptionsDataList[paramDescriptionsIndex].toString();
			paramDescriptions->append(tempParamDescriptions);
		}
	}

	if (gqlObject.ContainsParam("parameters")){
		parameters.emplace();
	}
	if (gqlObject.ContainsParam("parameters") && !(gqlObject["parameters"].isNull())){
		const QVariant parametersData = gqlObject["parameters"];
		const QVariantList parametersDataList = parametersData.toList();
		const qsizetype parametersElementsCount = parametersDataList.size();
		parameters = imtsdl::TElementList<QString>();
		for (qsizetype parametersIndex = 0; parametersIndex < parametersElementsCount; ++parametersIndex){
			QString tempParameters = parametersDataList[parametersIndex].toString();
			parameters->append(tempParameters);
		}
	}

	return true;
}


bool CParamsSet::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (paramIds){
		QJsonArray newParamIdsArray;
		for (qsizetype paramIdsIndex = 0; paramIdsIndex < paramIds->size(); ++paramIdsIndex){
			newParamIdsArray << QString(*paramIds->at(paramIdsIndex));
		}
		jsonObject["paramIds"] = newParamIdsArray;
	}

	if (paramTypeIds){
		QJsonArray newParamTypeIdsArray;
		for (qsizetype paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIds->size(); ++paramTypeIdsIndex){
			newParamTypeIdsArray << QString(*paramTypeIds->at(paramTypeIdsIndex));
		}
		jsonObject["paramTypeIds"] = newParamTypeIdsArray;
	}

	if (paramNames){
		QJsonArray newParamNamesArray;
		for (qsizetype paramNamesIndex = 0; paramNamesIndex < paramNames->size(); ++paramNamesIndex){
			newParamNamesArray << *paramNames->at(paramNamesIndex);
		}
		jsonObject["paramNames"] = newParamNamesArray;
	}

	if (paramDescriptions){
		QJsonArray newParamDescriptionsArray;
		for (qsizetype paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptions->size(); ++paramDescriptionsIndex){
			newParamDescriptionsArray << *paramDescriptions->at(paramDescriptionsIndex);
		}
		jsonObject["paramDescriptions"] = newParamDescriptionsArray;
	}

	if (parameters){
		QJsonArray newParametersArray;
		for (qsizetype parametersIndex = 0; parametersIndex < parameters->size(); ++parametersIndex){
			newParametersArray << *parameters->at(parametersIndex);
		}
		jsonObject["parameters"] = newParametersArray;
	}

	jsonObject["__typename"] = "ParamsSet";

	return true;
}


bool CParamsSet::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("paramIds") && jsonObject["paramIds"].isArray()){
		const QJsonArray paramIdsJsonArray = jsonObject["paramIds"].toArray();
		const qsizetype paramIdsArrayCount = paramIdsJsonArray.size();
		paramIds = imtsdl::TElementList<QByteArray>();
		for (qsizetype paramIdsIndex = 0; paramIdsIndex < paramIdsArrayCount; ++paramIdsIndex){
			QByteArray tempParamIds = paramIdsJsonArray[paramIdsIndex].toString().toUtf8();
			paramIds->append(tempParamIds);
		}
	}

	if (jsonObject.contains("paramTypeIds") && jsonObject["paramTypeIds"].isArray()){
		const QJsonArray paramTypeIdsJsonArray = jsonObject["paramTypeIds"].toArray();
		const qsizetype paramTypeIdsArrayCount = paramTypeIdsJsonArray.size();
		paramTypeIds = imtsdl::TElementList<QByteArray>();
		for (qsizetype paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIdsArrayCount; ++paramTypeIdsIndex){
			QByteArray tempParamTypeIds = paramTypeIdsJsonArray[paramTypeIdsIndex].toString().toUtf8();
			paramTypeIds->append(tempParamTypeIds);
		}
	}

	if (jsonObject.contains("paramNames") && jsonObject["paramNames"].isArray()){
		const QJsonArray paramNamesJsonArray = jsonObject["paramNames"].toArray();
		const qsizetype paramNamesArrayCount = paramNamesJsonArray.size();
		paramNames = imtsdl::TElementList<QString>();
		for (qsizetype paramNamesIndex = 0; paramNamesIndex < paramNamesArrayCount; ++paramNamesIndex){
			QString tempParamNames = paramNamesJsonArray[paramNamesIndex].toString();
			paramNames->append(tempParamNames);
		}
	}

	if (jsonObject.contains("paramDescriptions") && jsonObject["paramDescriptions"].isArray()){
		const QJsonArray paramDescriptionsJsonArray = jsonObject["paramDescriptions"].toArray();
		const qsizetype paramDescriptionsArrayCount = paramDescriptionsJsonArray.size();
		paramDescriptions = imtsdl::TElementList<QString>();
		for (qsizetype paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptionsArrayCount; ++paramDescriptionsIndex){
			QString tempParamDescriptions = paramDescriptionsJsonArray[paramDescriptionsIndex].toString();
			paramDescriptions->append(tempParamDescriptions);
		}
	}

	if (jsonObject.contains("parameters") && jsonObject["parameters"].isArray()){
		const QJsonArray parametersJsonArray = jsonObject["parameters"].toArray();
		const qsizetype parametersArrayCount = parametersJsonArray.size();
		parameters = imtsdl::TElementList<QString>();
		for (qsizetype parametersIndex = 0; parametersIndex < parametersArrayCount; ++parametersIndex){
			QString tempParameters = parametersJsonArray[parametersIndex].toString();
			parameters->append(tempParameters);
		}
	}

	return true;
}


bool CParamsSet::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("paramIds") && jsonObject["paramIds"].isArray()){
		const QJsonArray paramIdsJsonArray = jsonObject["paramIds"].toArray();
		const qsizetype paramIdsArrayCount = paramIdsJsonArray.size();
		paramIds = imtsdl::TElementList<QByteArray>();
		for (qsizetype paramIdsIndex = 0; paramIdsIndex < paramIdsArrayCount; ++paramIdsIndex){
			QByteArray tempParamIds = paramIdsJsonArray[paramIdsIndex].toString().toUtf8();
			paramIds->append(tempParamIds);
		}
	}

	if (jsonObject.contains("paramTypeIds") && jsonObject["paramTypeIds"].isArray()){
		const QJsonArray paramTypeIdsJsonArray = jsonObject["paramTypeIds"].toArray();
		const qsizetype paramTypeIdsArrayCount = paramTypeIdsJsonArray.size();
		paramTypeIds = imtsdl::TElementList<QByteArray>();
		for (qsizetype paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIdsArrayCount; ++paramTypeIdsIndex){
			QByteArray tempParamTypeIds = paramTypeIdsJsonArray[paramTypeIdsIndex].toString().toUtf8();
			paramTypeIds->append(tempParamTypeIds);
		}
	}

	if (jsonObject.contains("paramNames") && jsonObject["paramNames"].isArray()){
		const QJsonArray paramNamesJsonArray = jsonObject["paramNames"].toArray();
		const qsizetype paramNamesArrayCount = paramNamesJsonArray.size();
		paramNames = imtsdl::TElementList<QString>();
		for (qsizetype paramNamesIndex = 0; paramNamesIndex < paramNamesArrayCount; ++paramNamesIndex){
			QString tempParamNames = paramNamesJsonArray[paramNamesIndex].toString();
			paramNames->append(tempParamNames);
		}
	}

	if (jsonObject.contains("paramDescriptions") && jsonObject["paramDescriptions"].isArray()){
		const QJsonArray paramDescriptionsJsonArray = jsonObject["paramDescriptions"].toArray();
		const qsizetype paramDescriptionsArrayCount = paramDescriptionsJsonArray.size();
		paramDescriptions = imtsdl::TElementList<QString>();
		for (qsizetype paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptionsArrayCount; ++paramDescriptionsIndex){
			QString tempParamDescriptions = paramDescriptionsJsonArray[paramDescriptionsIndex].toString();
			paramDescriptions->append(tempParamDescriptions);
		}
	}

	if (jsonObject.contains("parameters") && jsonObject["parameters"].isArray()){
		const QJsonArray parametersJsonArray = jsonObject["parameters"].toArray();
		const qsizetype parametersArrayCount = parametersJsonArray.size();
		parameters = imtsdl::TElementList<QString>();
		for (qsizetype parametersIndex = 0; parametersIndex < parametersArrayCount; ++parametersIndex){
			QString tempParameters = parametersJsonArray[parametersIndex].toString();
			parameters->append(tempParameters);
		}
	}

	return true;
}


// serialize methods

QByteArray CMimeType::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CMimeType::operator==(const CMimeType& other) const
{
	return 
				type == other.type &&
				tree == other.tree &&
				subType == other.subType &&
				suffix == other.suffix &&
				parameters == other.parameters;
}


bool CMimeType::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "MimeType", modelIndex);

	if (!type){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "type").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("type", *type, modelIndex);

	if (tree){
		::imtbase::CTreeItemModel* newTreeModelPtr = model.AddTreeModel("tree", modelIndex);
		newTreeModelPtr->setIsArray(true);
		for (qsizetype treeIndex = 0; treeIndex < tree->size(); ++treeIndex){
			newTreeModelPtr->InsertNewItem();
			newTreeModelPtr->SetData(QByteArray(), *tree->at(treeIndex), treeIndex);
		}
	}

	if (!subType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "subType").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("subType", *subType, modelIndex);

	if (suffix){
		model.SetData("suffix", *suffix, modelIndex);
	}

	if (parameters){
		::imtbase::CTreeItemModel* newParametersModelPtr = model.AddTreeModel("parameters", modelIndex);
		newParametersModelPtr->setIsArray(true);
		for (qsizetype parametersIndex = 0; parametersIndex < parameters->size(); ++parametersIndex){
			newParametersModelPtr->InsertNewItem();
			newParametersModelPtr->SetData(QByteArray(), *parameters->at(parametersIndex), parametersIndex);
		}
	}


	return true;
}


bool CMimeType::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant typeData = model.GetData("type", modelIndex);
	if (typeData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "type").toLocal8Bit().constData();)

		return false;
	}
	type = typeData.toString();

	::imtbase::CTreeItemModel* treeModel = model.GetTreeItemModel("tree", modelIndex);
	if (treeModel != nullptr){
		int treeCount = treeModel->GetItemsCount();
		imtsdl::TElementList<QString> treeList;
		for (int treeIndex = 0; treeIndex < treeCount; ++treeIndex){
			QString t_tree = treeModel->GetData(QByteArray(), treeIndex).toString();
			treeList << t_tree;
		}
		tree = treeList;

	}

	QVariant subTypeData = model.GetData("subType", modelIndex);
	if (subTypeData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "subType").toLocal8Bit().constData();)

		return false;
	}
	subType = subTypeData.toString();

	QVariant suffixData = model.GetData("suffix", modelIndex);
	if (!suffixData.isNull()){
		suffix = suffixData.toString();
	}

	::imtbase::CTreeItemModel* parametersModel = model.GetTreeItemModel("parameters", modelIndex);
	if (parametersModel != nullptr){
		int parametersCount = parametersModel->GetItemsCount();
		imtsdl::TElementList<QString> parametersList;
		for (int parametersIndex = 0; parametersIndex < parametersCount; ++parametersIndex){
			QString t_parameters = parametersModel->GetData(QByteArray(), parametersIndex).toString();
			parametersList << t_parameters;
		}
		parameters = parametersList;

	}

	return true;
}


bool CMimeType::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant typeData = model.GetData("type", modelIndex);
	if (!typeData.isNull()){
		type = typeData.toString();
	}

	::imtbase::CTreeItemModel* treeModel = model.GetTreeItemModel("tree", modelIndex);
	if (treeModel != nullptr){
		int treeCount = treeModel->GetItemsCount();
		imtsdl::TElementList<QString> treeList;
		for (int treeIndex = 0; treeIndex < treeCount; ++treeIndex){
			QString t_tree = treeModel->GetData(QByteArray(), treeIndex).toString();
			treeList << t_tree;
		}
		tree = treeList;

	}

	QVariant subTypeData = model.GetData("subType", modelIndex);
	if (!subTypeData.isNull()){
		subType = subTypeData.toString();
	}

	QVariant suffixData = model.GetData("suffix", modelIndex);
	if (!suffixData.isNull()){
		suffix = suffixData.toString();
	}

	::imtbase::CTreeItemModel* parametersModel = model.GetTreeItemModel("parameters", modelIndex);
	if (parametersModel != nullptr){
		int parametersCount = parametersModel->GetItemsCount();
		imtsdl::TElementList<QString> parametersList;
		for (int parametersIndex = 0; parametersIndex < parametersCount; ++parametersIndex){
			QString t_parameters = parametersModel->GetData(QByteArray(), parametersIndex).toString();
			parametersList << t_parameters;
		}
		parameters = parametersList;

	}

	return true;
}


bool CMimeType::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!type){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "type").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("type", QVariant(*type));

	if (tree){
		QVariantList treeDataObjectList;
		for (qsizetype treeIndex = 0; treeIndex < tree->size(); ++treeIndex){
			treeDataObjectList << *tree->at(treeIndex);
		}
		gqlObject.InsertParam("tree", treeDataObjectList);
	}

	if (!subType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "subType").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("subType", QVariant(*subType));

	if (suffix){
		gqlObject.InsertParam("suffix", QVariant(*suffix));
	}

	if (parameters){
		QVariantList parametersDataObjectList;
		for (qsizetype parametersIndex = 0; parametersIndex < parameters->size(); ++parametersIndex){
			parametersDataObjectList << *parameters->at(parametersIndex);
		}
		gqlObject.InsertParam("parameters", parametersDataObjectList);
	}

	gqlObject.InsertParam("__typename", QVariant("MimeType"));

	return true;
}


bool CMimeType::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("type") || (gqlObject["type"].userType() != QMetaType::QString && gqlObject["type"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "type").toLocal8Bit().constData();)

		return false;
	}
	type = gqlObject["type"].toString();

	if (gqlObject.ContainsParam("tree")){
		tree.emplace();
	}
	if (gqlObject.ContainsParam("tree") && !(gqlObject["tree"].isNull())){
		const QVariant treeData = gqlObject["tree"];
		const QVariantList treeDataList = treeData.toList();
		const qsizetype treeElementsCount = treeDataList.size();
		tree = imtsdl::TElementList<QString>();
		for (qsizetype treeIndex = 0; treeIndex < treeElementsCount; ++treeIndex){
			QString tempTree = treeDataList[treeIndex].toString();
			tree->append(tempTree);
		}
	}

	if (!gqlObject.ContainsParam("subType") || (gqlObject["subType"].userType() != QMetaType::QString && gqlObject["subType"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "subType").toLocal8Bit().constData();)

		return false;
	}
	subType = gqlObject["subType"].toString();

	if (gqlObject.ContainsParam("suffix") && (gqlObject["suffix"].userType() == QMetaType::QString || gqlObject["suffix"].userType() == QMetaType::QByteArray)){
		suffix = gqlObject["suffix"].toString();
	}

	if (gqlObject.ContainsParam("parameters")){
		parameters.emplace();
	}
	if (gqlObject.ContainsParam("parameters") && !(gqlObject["parameters"].isNull())){
		const QVariant parametersData = gqlObject["parameters"];
		const QVariantList parametersDataList = parametersData.toList();
		const qsizetype parametersElementsCount = parametersDataList.size();
		parameters = imtsdl::TElementList<QString>();
		for (qsizetype parametersIndex = 0; parametersIndex < parametersElementsCount; ++parametersIndex){
			QString tempParameters = parametersDataList[parametersIndex].toString();
			parameters->append(tempParameters);
		}
	}

	return true;
}


bool CMimeType::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("type") && (gqlObject["type"].userType() == QMetaType::QString || gqlObject["type"].userType() == QMetaType::QByteArray)){
		type = gqlObject["type"].toString();
	}

	if (gqlObject.ContainsParam("tree")){
		tree.emplace();
	}
	if (gqlObject.ContainsParam("tree") && !(gqlObject["tree"].isNull())){
		const QVariant treeData = gqlObject["tree"];
		const QVariantList treeDataList = treeData.toList();
		const qsizetype treeElementsCount = treeDataList.size();
		tree = imtsdl::TElementList<QString>();
		for (qsizetype treeIndex = 0; treeIndex < treeElementsCount; ++treeIndex){
			QString tempTree = treeDataList[treeIndex].toString();
			tree->append(tempTree);
		}
	}

	if (gqlObject.ContainsParam("subType") && (gqlObject["subType"].userType() == QMetaType::QString || gqlObject["subType"].userType() == QMetaType::QByteArray)){
		subType = gqlObject["subType"].toString();
	}

	if (gqlObject.ContainsParam("suffix") && (gqlObject["suffix"].userType() == QMetaType::QString || gqlObject["suffix"].userType() == QMetaType::QByteArray)){
		suffix = gqlObject["suffix"].toString();
	}

	if (gqlObject.ContainsParam("parameters")){
		parameters.emplace();
	}
	if (gqlObject.ContainsParam("parameters") && !(gqlObject["parameters"].isNull())){
		const QVariant parametersData = gqlObject["parameters"];
		const QVariantList parametersDataList = parametersData.toList();
		const qsizetype parametersElementsCount = parametersDataList.size();
		parameters = imtsdl::TElementList<QString>();
		for (qsizetype parametersIndex = 0; parametersIndex < parametersElementsCount; ++parametersIndex){
			QString tempParameters = parametersDataList[parametersIndex].toString();
			parameters->append(tempParameters);
		}
	}

	return true;
}


bool CMimeType::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!type){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "type").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["type"] = QJsonValue::fromVariant(*type);

	if (tree){
		QJsonArray newTreeArray;
		for (qsizetype treeIndex = 0; treeIndex < tree->size(); ++treeIndex){
			newTreeArray << *tree->at(treeIndex);
		}
		jsonObject["tree"] = newTreeArray;
	}

	if (!subType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "subType").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["subType"] = QJsonValue::fromVariant(*subType);

	if (suffix){
		jsonObject["suffix"] = QJsonValue::fromVariant(*suffix);
	}

	if (parameters){
		QJsonArray newParametersArray;
		for (qsizetype parametersIndex = 0; parametersIndex < parameters->size(); ++parametersIndex){
			newParametersArray << *parameters->at(parametersIndex);
		}
		jsonObject["parameters"] = newParametersArray;
	}

	jsonObject["__typename"] = "MimeType";

	return true;
}


bool CMimeType::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("type") || ! jsonObject["type"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "type").toLocal8Bit().constData();)

		return false;
	}
	type = jsonObject["type"].toString();

	if (jsonObject.contains("tree") && jsonObject["tree"].isArray()){
		const QJsonArray treeJsonArray = jsonObject["tree"].toArray();
		const qsizetype treeArrayCount = treeJsonArray.size();
		tree = imtsdl::TElementList<QString>();
		for (qsizetype treeIndex = 0; treeIndex < treeArrayCount; ++treeIndex){
			QString tempTree = treeJsonArray[treeIndex].toString();
			tree->append(tempTree);
		}
	}

	if (!jsonObject.contains("subType") || ! jsonObject["subType"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "subType").toLocal8Bit().constData();)

		return false;
	}
	subType = jsonObject["subType"].toString();

	if (jsonObject.contains("suffix") && jsonObject["suffix"].isString()){
		suffix = jsonObject["suffix"].toString();
	}

	if (jsonObject.contains("parameters") && jsonObject["parameters"].isArray()){
		const QJsonArray parametersJsonArray = jsonObject["parameters"].toArray();
		const qsizetype parametersArrayCount = parametersJsonArray.size();
		parameters = imtsdl::TElementList<QString>();
		for (qsizetype parametersIndex = 0; parametersIndex < parametersArrayCount; ++parametersIndex){
			QString tempParameters = parametersJsonArray[parametersIndex].toString();
			parameters->append(tempParameters);
		}
	}

	return true;
}


bool CMimeType::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("type") && jsonObject["type"].isString()){
		type = jsonObject["type"].toString();
	}

	if (jsonObject.contains("tree") && jsonObject["tree"].isArray()){
		const QJsonArray treeJsonArray = jsonObject["tree"].toArray();
		const qsizetype treeArrayCount = treeJsonArray.size();
		tree = imtsdl::TElementList<QString>();
		for (qsizetype treeIndex = 0; treeIndex < treeArrayCount; ++treeIndex){
			QString tempTree = treeJsonArray[treeIndex].toString();
			tree->append(tempTree);
		}
	}

	if (jsonObject.contains("subType") && jsonObject["subType"].isString()){
		subType = jsonObject["subType"].toString();
	}

	if (jsonObject.contains("suffix") && jsonObject["suffix"].isString()){
		suffix = jsonObject["suffix"].toString();
	}

	if (jsonObject.contains("parameters") && jsonObject["parameters"].isArray()){
		const QJsonArray parametersJsonArray = jsonObject["parameters"].toArray();
		const qsizetype parametersArrayCount = parametersJsonArray.size();
		parameters = imtsdl::TElementList<QString>();
		for (qsizetype parametersIndex = 0; parametersIndex < parametersArrayCount; ++parametersIndex){
			QString tempParameters = parametersJsonArray[parametersIndex].toString();
			parameters->append(tempParameters);
		}
	}

	return true;
}


// serialize methods

CTimeRangeObject::CTimeRangeObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CTimeRangeObject::beginChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CTimeRangeObject::endChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CTimeRangeObject::GetBegin()
{
	if (Begin){
		return Begin.value();
	}

	return QVariant();
}


void CTimeRangeObject::SetBegin(const QVariant& v)
{

	Begin = v.value<QString>();
	beginChanged();
}


bool CTimeRangeObject::hasBegin()
{
	 return Begin.HasValue();
}


QVariant CTimeRangeObject::GetEnd()
{
	if (End){
		return End.value();
	}

	return QVariant();
}


void CTimeRangeObject::SetEnd(const QVariant& v)
{

	End = v.value<QString>();
	endChanged();
}


bool CTimeRangeObject::hasEnd()
{
	 return End.HasValue();
}


QString CTimeRangeObject::toJson() const
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


bool CTimeRangeObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CTimeRangeObject::fromObject(const QJsonObject& jsonObject)
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


QString CTimeRangeObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CTimeRangeObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CTimeRangeObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "begin")){
		return "Begin";
	}
	if (propertyName == (QString("m_") + "end")){
		return "End";
	}

	return propertyName;
}





bool CTimeRangeObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CTimeRangeObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CTimeRangeObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CTimeRangeObjectList::append(sdl::V1_0::imtbase::CTimeRangeObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CTimeRangeObjectList* sdl::V1_0::imtbase::CTimeRangeObjectList::copyMe()
{
	sdl::V1_0::imtbase::CTimeRangeObjectList* retVal = new sdl::V1_0::imtbase::CTimeRangeObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CTimeRangeObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CTimeRangeObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CTimeRangeObjectList::addElement(sdl::V1_0::imtbase::CTimeRangeObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CTimeRangeObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CTimeRangeObjectList::isEqualWithModel(sdl::V1_0::imtbase::CTimeRangeObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CTimeRangeObjectList::insert(int index, sdl::V1_0::imtbase::CTimeRangeObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CTimeRangeObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CTimeRangeObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CTimeRangeObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CTimeRangeObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_begin"){
		return QVariant::fromValue(at(index)->Begin.value());
	}
	if (nameId == "m_end"){
		return QVariant::fromValue(at(index)->End.value());
	}

	return QVariant();
}
CSdlSizeObject::CSdlSizeObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSdlSizeObject::widthChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSdlSizeObject::heightChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSdlSizeObject::GetWidth()
{
	if (width){
		return width.value();
	}

	return QVariant();
}


void CSdlSizeObject::SetWidth(const QVariant& v)
{

	width = v.value<double>();
	widthChanged();
}


bool CSdlSizeObject::hasWidth()
{
	 return width.HasValue();
}


QVariant CSdlSizeObject::GetHeight()
{
	if (height){
		return height.value();
	}

	return QVariant();
}


void CSdlSizeObject::SetHeight(const QVariant& v)
{

	height = v.value<double>();
	heightChanged();
}


bool CSdlSizeObject::hasHeight()
{
	 return height.HasValue();
}


QString CSdlSizeObject::toJson() const
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


bool CSdlSizeObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSdlSizeObject::fromObject(const QJsonObject& jsonObject)
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


QString CSdlSizeObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSdlSizeObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CSdlSizeObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "width")){
		return "width";
	}
	if (propertyName == (QString("m_") + "height")){
		return "height";
	}

	return propertyName;
}





bool CSdlSizeObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSdlSizeObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSdlSizeObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSdlSizeObjectList::append(sdl::V1_0::imtbase::CSdlSizeObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CSdlSizeObjectList* sdl::V1_0::imtbase::CSdlSizeObjectList::copyMe()
{
	sdl::V1_0::imtbase::CSdlSizeObjectList* retVal = new sdl::V1_0::imtbase::CSdlSizeObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CSdlSizeObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CSdlSizeObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CSdlSizeObjectList::addElement(sdl::V1_0::imtbase::CSdlSizeObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CSdlSizeObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CSdlSizeObjectList::isEqualWithModel(sdl::V1_0::imtbase::CSdlSizeObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CSdlSizeObjectList::insert(int index, sdl::V1_0::imtbase::CSdlSizeObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CSdlSizeObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CSdlSizeObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CSdlSizeObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CSdlSizeObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_width"){
		return QVariant::fromValue(at(index)->width.value());
	}
	if (nameId == "m_height"){
		return QVariant::fromValue(at(index)->height.value());
	}

	return QVariant();
}
CSdlPointObject::CSdlPointObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSdlPointObject::xChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSdlPointObject::yChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSdlPointObject::GetX()
{
	if (x){
		return x.value();
	}

	return QVariant();
}


void CSdlPointObject::SetX(const QVariant& v)
{

	x = v.value<double>();
	xChanged();
}


bool CSdlPointObject::hasX()
{
	 return x.HasValue();
}


QVariant CSdlPointObject::GetY()
{
	if (y){
		return y.value();
	}

	return QVariant();
}


void CSdlPointObject::SetY(const QVariant& v)
{

	y = v.value<double>();
	yChanged();
}


bool CSdlPointObject::hasY()
{
	 return y.HasValue();
}


QString CSdlPointObject::toJson() const
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


bool CSdlPointObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSdlPointObject::fromObject(const QJsonObject& jsonObject)
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


QString CSdlPointObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSdlPointObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CSdlPointObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "x")){
		return "x";
	}
	if (propertyName == (QString("m_") + "y")){
		return "y";
	}

	return propertyName;
}





bool CSdlPointObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSdlPointObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSdlPointObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSdlPointObjectList::append(sdl::V1_0::imtbase::CSdlPointObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CSdlPointObjectList* sdl::V1_0::imtbase::CSdlPointObjectList::copyMe()
{
	sdl::V1_0::imtbase::CSdlPointObjectList* retVal = new sdl::V1_0::imtbase::CSdlPointObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CSdlPointObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CSdlPointObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CSdlPointObjectList::addElement(sdl::V1_0::imtbase::CSdlPointObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CSdlPointObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CSdlPointObjectList::isEqualWithModel(sdl::V1_0::imtbase::CSdlPointObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CSdlPointObjectList::insert(int index, sdl::V1_0::imtbase::CSdlPointObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CSdlPointObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CSdlPointObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CSdlPointObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CSdlPointObject*>();
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

	return QVariant();
}
CParamTypeIdsObject::CParamTypeIdsObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CParamTypeIdsObject::urlParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::idParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::textParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::textViewChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::selectionParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::schedulerParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::backupSettingsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::databaseAccessSettingsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::paramsSetChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::fileNameParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::integerParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::doubleParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::passwordParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamTypeIdsObject::enableableParamChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CParamTypeIdsObject::GetUrlParam()
{
	if (UrlParam){
		return UrlParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetUrlParam(const QVariant& v)
{

	UrlParam = v.value<QString>().toUtf8();
	urlParamChanged();
}


bool CParamTypeIdsObject::hasUrlParam()
{
	 return UrlParam.HasValue();
}


QVariant CParamTypeIdsObject::GetIdParam()
{
	if (IdParam){
		return IdParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetIdParam(const QVariant& v)
{

	IdParam = v.value<QString>().toUtf8();
	idParamChanged();
}


bool CParamTypeIdsObject::hasIdParam()
{
	 return IdParam.HasValue();
}


QVariant CParamTypeIdsObject::GetTextParam()
{
	if (TextParam){
		return TextParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetTextParam(const QVariant& v)
{

	TextParam = v.value<QString>().toUtf8();
	textParamChanged();
}


bool CParamTypeIdsObject::hasTextParam()
{
	 return TextParam.HasValue();
}


QVariant CParamTypeIdsObject::GetTextView()
{
	if (TextView){
		return TextView.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetTextView(const QVariant& v)
{

	TextView = v.value<QString>().toUtf8();
	textViewChanged();
}


bool CParamTypeIdsObject::hasTextView()
{
	 return TextView.HasValue();
}


QVariant CParamTypeIdsObject::GetSelectionParam()
{
	if (SelectionParam){
		return SelectionParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetSelectionParam(const QVariant& v)
{

	SelectionParam = v.value<QString>().toUtf8();
	selectionParamChanged();
}


bool CParamTypeIdsObject::hasSelectionParam()
{
	 return SelectionParam.HasValue();
}


QVariant CParamTypeIdsObject::GetSchedulerParam()
{
	if (SchedulerParam){
		return SchedulerParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetSchedulerParam(const QVariant& v)
{

	SchedulerParam = v.value<QString>().toUtf8();
	schedulerParamChanged();
}


bool CParamTypeIdsObject::hasSchedulerParam()
{
	 return SchedulerParam.HasValue();
}


QVariant CParamTypeIdsObject::GetBackupSettings()
{
	if (BackupSettings){
		return BackupSettings.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetBackupSettings(const QVariant& v)
{

	BackupSettings = v.value<QString>().toUtf8();
	backupSettingsChanged();
}


bool CParamTypeIdsObject::hasBackupSettings()
{
	 return BackupSettings.HasValue();
}


QVariant CParamTypeIdsObject::GetDatabaseAccessSettings()
{
	if (DatabaseAccessSettings){
		return DatabaseAccessSettings.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetDatabaseAccessSettings(const QVariant& v)
{

	DatabaseAccessSettings = v.value<QString>().toUtf8();
	databaseAccessSettingsChanged();
}


bool CParamTypeIdsObject::hasDatabaseAccessSettings()
{
	 return DatabaseAccessSettings.HasValue();
}


QVariant CParamTypeIdsObject::GetParamsSet()
{
	if (ParamsSet){
		return ParamsSet.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetParamsSet(const QVariant& v)
{

	ParamsSet = v.value<QString>().toUtf8();
	paramsSetChanged();
}


bool CParamTypeIdsObject::hasParamsSet()
{
	 return ParamsSet.HasValue();
}


QVariant CParamTypeIdsObject::GetFileNameParam()
{
	if (FileNameParam){
		return FileNameParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetFileNameParam(const QVariant& v)
{

	FileNameParam = v.value<QString>().toUtf8();
	fileNameParamChanged();
}


bool CParamTypeIdsObject::hasFileNameParam()
{
	 return FileNameParam.HasValue();
}


QVariant CParamTypeIdsObject::GetIntegerParam()
{
	if (IntegerParam){
		return IntegerParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetIntegerParam(const QVariant& v)
{

	IntegerParam = v.value<QString>().toUtf8();
	integerParamChanged();
}


bool CParamTypeIdsObject::hasIntegerParam()
{
	 return IntegerParam.HasValue();
}


QVariant CParamTypeIdsObject::GetDoubleParam()
{
	if (DoubleParam){
		return DoubleParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetDoubleParam(const QVariant& v)
{

	DoubleParam = v.value<QString>().toUtf8();
	doubleParamChanged();
}


bool CParamTypeIdsObject::hasDoubleParam()
{
	 return DoubleParam.HasValue();
}


QVariant CParamTypeIdsObject::GetPasswordParam()
{
	if (PasswordParam){
		return PasswordParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetPasswordParam(const QVariant& v)
{

	PasswordParam = v.value<QString>().toUtf8();
	passwordParamChanged();
}


bool CParamTypeIdsObject::hasPasswordParam()
{
	 return PasswordParam.HasValue();
}


QVariant CParamTypeIdsObject::GetEnableableParam()
{
	if (EnableableParam){
		return EnableableParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetEnableableParam(const QVariant& v)
{

	EnableableParam = v.value<QString>().toUtf8();
	enableableParamChanged();
}


bool CParamTypeIdsObject::hasEnableableParam()
{
	 return EnableableParam.HasValue();
}


QString CParamTypeIdsObject::toJson() const
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


bool CParamTypeIdsObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CParamTypeIdsObject::fromObject(const QJsonObject& jsonObject)
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


QString CParamTypeIdsObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CParamTypeIdsObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CParamTypeIdsObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "urlParam")){
		return "UrlParam";
	}
	if (propertyName == (QString("m_") + "idParam")){
		return "IdParam";
	}
	if (propertyName == (QString("m_") + "textParam")){
		return "TextParam";
	}
	if (propertyName == (QString("m_") + "textView")){
		return "TextView";
	}
	if (propertyName == (QString("m_") + "selectionParam")){
		return "SelectionParam";
	}
	if (propertyName == (QString("m_") + "schedulerParam")){
		return "SchedulerParam";
	}
	if (propertyName == (QString("m_") + "backupSettings")){
		return "BackupSettings";
	}
	if (propertyName == (QString("m_") + "databaseAccessSettings")){
		return "DatabaseAccessSettings";
	}
	if (propertyName == (QString("m_") + "paramsSet")){
		return "ParamsSet";
	}
	if (propertyName == (QString("m_") + "fileNameParam")){
		return "FileNameParam";
	}
	if (propertyName == (QString("m_") + "integerParam")){
		return "IntegerParam";
	}
	if (propertyName == (QString("m_") + "doubleParam")){
		return "DoubleParam";
	}
	if (propertyName == (QString("m_") + "passwordParam")){
		return "PasswordParam";
	}
	if (propertyName == (QString("m_") + "enableableParam")){
		return "EnableableParam";
	}

	return propertyName;
}





bool CParamTypeIdsObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CParamTypeIdsObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CParamTypeIdsObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CParamTypeIdsObjectList::append(sdl::V1_0::imtbase::CParamTypeIdsObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CParamTypeIdsObjectList* sdl::V1_0::imtbase::CParamTypeIdsObjectList::copyMe()
{
	sdl::V1_0::imtbase::CParamTypeIdsObjectList* retVal = new sdl::V1_0::imtbase::CParamTypeIdsObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CParamTypeIdsObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CParamTypeIdsObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CParamTypeIdsObjectList::addElement(sdl::V1_0::imtbase::CParamTypeIdsObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CParamTypeIdsObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CParamTypeIdsObjectList::isEqualWithModel(sdl::V1_0::imtbase::CParamTypeIdsObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CParamTypeIdsObjectList::insert(int index, sdl::V1_0::imtbase::CParamTypeIdsObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CParamTypeIdsObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CParamTypeIdsObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CParamTypeIdsObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CParamTypeIdsObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_urlParam"){
		return QVariant::fromValue(at(index)->UrlParam.value());
	}
	if (nameId == "m_idParam"){
		return QVariant::fromValue(at(index)->IdParam.value());
	}
	if (nameId == "m_textParam"){
		return QVariant::fromValue(at(index)->TextParam.value());
	}
	if (nameId == "m_textView"){
		return QVariant::fromValue(at(index)->TextView.value());
	}
	if (nameId == "m_selectionParam"){
		return QVariant::fromValue(at(index)->SelectionParam.value());
	}
	if (nameId == "m_schedulerParam"){
		return QVariant::fromValue(at(index)->SchedulerParam.value());
	}
	if (nameId == "m_backupSettings"){
		return QVariant::fromValue(at(index)->BackupSettings.value());
	}
	if (nameId == "m_databaseAccessSettings"){
		return QVariant::fromValue(at(index)->DatabaseAccessSettings.value());
	}
	if (nameId == "m_paramsSet"){
		return QVariant::fromValue(at(index)->ParamsSet.value());
	}
	if (nameId == "m_fileNameParam"){
		return QVariant::fromValue(at(index)->FileNameParam.value());
	}
	if (nameId == "m_integerParam"){
		return QVariant::fromValue(at(index)->IntegerParam.value());
	}
	if (nameId == "m_doubleParam"){
		return QVariant::fromValue(at(index)->DoubleParam.value());
	}
	if (nameId == "m_passwordParam"){
		return QVariant::fromValue(at(index)->PasswordParam.value());
	}
	if (nameId == "m_enableableParam"){
		return QVariant::fromValue(at(index)->EnableableParam.value());
	}

	return QVariant();
}
CUrlParamObject::CUrlParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CUrlParamObject::schemeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CUrlParamObject::hostChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CUrlParamObject::portChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CUrlParamObject::pathChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CUrlParamObject::GetScheme()
{
	if (scheme){
		return scheme.value();
	}

	return QVariant();
}


void CUrlParamObject::SetScheme(const QVariant& v)
{

	scheme = v.value<QString>();
	schemeChanged();
}


bool CUrlParamObject::hasScheme()
{
	 return scheme.HasValue();
}


QVariant CUrlParamObject::GetHost()
{
	if (host){
		return host.value();
	}

	return QVariant();
}


void CUrlParamObject::SetHost(const QVariant& v)
{

	host = v.value<QString>();
	hostChanged();
}


bool CUrlParamObject::hasHost()
{
	 return host.HasValue();
}


QVariant CUrlParamObject::GetPort()
{
	if (port){
		return port.value();
	}

	return QVariant();
}


void CUrlParamObject::SetPort(const QVariant& v)
{

	port = v.value<int>();
	portChanged();
}


bool CUrlParamObject::hasPort()
{
	 return port.HasValue();
}


QVariant CUrlParamObject::GetPath()
{
	if (path){
		return path.value();
	}

	return QVariant();
}


void CUrlParamObject::SetPath(const QVariant& v)
{

	path = v.value<QString>();
	pathChanged();
}


bool CUrlParamObject::hasPath()
{
	 return path.HasValue();
}


QString CUrlParamObject::toJson() const
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


bool CUrlParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CUrlParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CUrlParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CUrlParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CUrlParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "scheme")){
		return "scheme";
	}
	if (propertyName == (QString("m_") + "host")){
		return "host";
	}
	if (propertyName == (QString("m_") + "port")){
		return "port";
	}
	if (propertyName == (QString("m_") + "path")){
		return "path";
	}

	return propertyName;
}





bool CUrlParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CUrlParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CUrlParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CUrlParamObjectList::append(sdl::V1_0::imtbase::CUrlParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CUrlParamObjectList* sdl::V1_0::imtbase::CUrlParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CUrlParamObjectList* retVal = new sdl::V1_0::imtbase::CUrlParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CUrlParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CUrlParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CUrlParamObjectList::addElement(sdl::V1_0::imtbase::CUrlParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CUrlParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CUrlParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CUrlParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CUrlParamObjectList::insert(int index, sdl::V1_0::imtbase::CUrlParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CUrlParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CUrlParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CUrlParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CUrlParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_scheme"){
		return QVariant::fromValue(at(index)->scheme.value());
	}
	if (nameId == "m_host"){
		return QVariant::fromValue(at(index)->host.value());
	}
	if (nameId == "m_port"){
		return QVariant::fromValue(at(index)->port.value());
	}
	if (nameId == "m_path"){
		return QVariant::fromValue(at(index)->path.value());
	}

	return QVariant();
}
CIdParamObject::CIdParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CIdParamObject::idChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CIdParamObject::GetId()
{
	if (id){
		return id.value();
	}

	return QVariant();
}


void CIdParamObject::SetId(const QVariant& v)
{

	id = v.value<QString>().toUtf8();
	idChanged();
}


bool CIdParamObject::hasId()
{
	 return id.HasValue();
}


QString CIdParamObject::toJson() const
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


bool CIdParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CIdParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CIdParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CIdParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CIdParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "id")){
		return "id";
	}

	return propertyName;
}





bool CIdParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CIdParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CIdParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CIdParamObjectList::append(sdl::V1_0::imtbase::CIdParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CIdParamObjectList* sdl::V1_0::imtbase::CIdParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CIdParamObjectList* retVal = new sdl::V1_0::imtbase::CIdParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CIdParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CIdParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CIdParamObjectList::addElement(sdl::V1_0::imtbase::CIdParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CIdParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CIdParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CIdParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CIdParamObjectList::insert(int index, sdl::V1_0::imtbase::CIdParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CIdParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CIdParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CIdParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CIdParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_id"){
		return QVariant::fromValue(at(index)->id.value());
	}

	return QVariant();
}
CTextParamObject::CTextParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CTextParamObject::textChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CTextParamObject::GetText()
{
	if (text){
		return text.value();
	}

	return QVariant();
}


void CTextParamObject::SetText(const QVariant& v)
{

	text = v.value<QString>();
	textChanged();
}


bool CTextParamObject::hasText()
{
	 return text.HasValue();
}


QString CTextParamObject::toJson() const
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


bool CTextParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CTextParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CTextParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CTextParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CTextParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "text")){
		return "text";
	}

	return propertyName;
}





bool CTextParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CTextParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CTextParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CTextParamObjectList::append(sdl::V1_0::imtbase::CTextParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CTextParamObjectList* sdl::V1_0::imtbase::CTextParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CTextParamObjectList* retVal = new sdl::V1_0::imtbase::CTextParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CTextParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CTextParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CTextParamObjectList::addElement(sdl::V1_0::imtbase::CTextParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CTextParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CTextParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CTextParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CTextParamObjectList::insert(int index, sdl::V1_0::imtbase::CTextParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CTextParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CTextParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CTextParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CTextParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_text"){
		return QVariant::fromValue(at(index)->text.value());
	}

	return QVariant();
}
CEnableableParamObject::CEnableableParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CEnableableParamObject::valueChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CEnableableParamObject::GetValue()
{
	if (value){
		return value.value();
	}

	return QVariant();
}


void CEnableableParamObject::SetValue(const QVariant& v)
{

	value = v.value<bool>();
	valueChanged();
}


bool CEnableableParamObject::hasValue()
{
	 return value.HasValue();
}


QString CEnableableParamObject::toJson() const
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


bool CEnableableParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CEnableableParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CEnableableParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CEnableableParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CEnableableParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "value")){
		return "value";
	}

	return propertyName;
}





bool CEnableableParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CEnableableParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CEnableableParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CEnableableParamObjectList::append(sdl::V1_0::imtbase::CEnableableParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CEnableableParamObjectList* sdl::V1_0::imtbase::CEnableableParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CEnableableParamObjectList* retVal = new sdl::V1_0::imtbase::CEnableableParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CEnableableParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CEnableableParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CEnableableParamObjectList::addElement(sdl::V1_0::imtbase::CEnableableParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CEnableableParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CEnableableParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CEnableableParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CEnableableParamObjectList::insert(int index, sdl::V1_0::imtbase::CEnableableParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CEnableableParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CEnableableParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CEnableableParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CEnableableParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_value"){
		return QVariant::fromValue(at(index)->value.value());
	}

	return QVariant();
}
CIntegerParamObject::CIntegerParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CIntegerParamObject::valueChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CIntegerParamObject::GetValue()
{
	if (value){
		return value.value();
	}

	return QVariant();
}


void CIntegerParamObject::SetValue(const QVariant& v)
{

	value = v.value<int>();
	valueChanged();
}


bool CIntegerParamObject::hasValue()
{
	 return value.HasValue();
}


QString CIntegerParamObject::toJson() const
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


bool CIntegerParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CIntegerParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CIntegerParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CIntegerParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CIntegerParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "value")){
		return "value";
	}

	return propertyName;
}





bool CIntegerParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CIntegerParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CIntegerParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CIntegerParamObjectList::append(sdl::V1_0::imtbase::CIntegerParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CIntegerParamObjectList* sdl::V1_0::imtbase::CIntegerParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CIntegerParamObjectList* retVal = new sdl::V1_0::imtbase::CIntegerParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CIntegerParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CIntegerParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CIntegerParamObjectList::addElement(sdl::V1_0::imtbase::CIntegerParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CIntegerParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CIntegerParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CIntegerParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CIntegerParamObjectList::insert(int index, sdl::V1_0::imtbase::CIntegerParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CIntegerParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CIntegerParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CIntegerParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CIntegerParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_value"){
		return QVariant::fromValue(at(index)->value.value());
	}

	return QVariant();
}
CDoubleParamObject::CDoubleParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CDoubleParamObject::valueChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CDoubleParamObject::GetValue()
{
	if (value){
		return value.value();
	}

	return QVariant();
}


void CDoubleParamObject::SetValue(const QVariant& v)
{

	value = v.value<double>();
	valueChanged();
}


bool CDoubleParamObject::hasValue()
{
	 return value.HasValue();
}


QString CDoubleParamObject::toJson() const
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


bool CDoubleParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CDoubleParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CDoubleParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CDoubleParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CDoubleParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "value")){
		return "value";
	}

	return propertyName;
}





bool CDoubleParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CDoubleParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CDoubleParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CDoubleParamObjectList::append(sdl::V1_0::imtbase::CDoubleParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CDoubleParamObjectList* sdl::V1_0::imtbase::CDoubleParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CDoubleParamObjectList* retVal = new sdl::V1_0::imtbase::CDoubleParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CDoubleParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CDoubleParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CDoubleParamObjectList::addElement(sdl::V1_0::imtbase::CDoubleParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CDoubleParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CDoubleParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CDoubleParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CDoubleParamObjectList::insert(int index, sdl::V1_0::imtbase::CDoubleParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CDoubleParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CDoubleParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CDoubleParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CDoubleParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_value"){
		return QVariant::fromValue(at(index)->value.value());
	}

	return QVariant();
}
COptionObject::COptionObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &COptionObject::idChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COptionObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COptionObject::descriptionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COptionObject::enabledChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant COptionObject::GetId()
{
	if (id){
		return id.value();
	}

	return QVariant();
}


void COptionObject::SetId(const QVariant& v)
{

	id = v.value<QString>().toUtf8();
	idChanged();
}


bool COptionObject::hasId()
{
	 return id.HasValue();
}


QVariant COptionObject::GetName()
{
	if (name){
		return name.value();
	}

	return QVariant();
}


void COptionObject::SetName(const QVariant& v)
{

	name = v.value<QString>();
	nameChanged();
}


bool COptionObject::hasName()
{
	 return name.HasValue();
}


QVariant COptionObject::GetDescription()
{
	if (description){
		return description.value();
	}

	return QVariant();
}


void COptionObject::SetDescription(const QVariant& v)
{

	description = v.value<QString>();
	descriptionChanged();
}


bool COptionObject::hasDescription()
{
	 return description.HasValue();
}


QVariant COptionObject::GetEnabled()
{
	if (enabled){
		return enabled.value();
	}

	return QVariant();
}


void COptionObject::SetEnabled(const QVariant& v)
{

	enabled = v.value<bool>();
	enabledChanged();
}


bool COptionObject::hasEnabled()
{
	 return enabled.HasValue();
}


QString COptionObject::toJson() const
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


bool COptionObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool COptionObject::fromObject(const QJsonObject& jsonObject)
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


QString COptionObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant COptionObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString COptionObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "id")){
		return "id";
	}
	if (propertyName == (QString("m_") + "name")){
		return "name";
	}
	if (propertyName == (QString("m_") + "description")){
		return "description";
	}
	if (propertyName == (QString("m_") + "enabled")){
		return "enabled";
	}

	return propertyName;
}





bool COptionObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int COptionObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap COptionObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void COptionObjectList::append(sdl::V1_0::imtbase::COptionObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::COptionObjectList* sdl::V1_0::imtbase::COptionObjectList::copyMe()
{
	sdl::V1_0::imtbase::COptionObjectList* retVal = new sdl::V1_0::imtbase::COptionObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::COptionObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::COptionObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::COptionObjectList::addElement(sdl::V1_0::imtbase::COptionObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::COptionObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::COptionObjectList::isEqualWithModel(sdl::V1_0::imtbase::COptionObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::COptionObjectList::insert(int index, sdl::V1_0::imtbase::COptionObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::COptionObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::COptionObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::COptionObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::COptionObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_id"){
		return QVariant::fromValue(at(index)->id.value());
	}
	if (nameId == "m_name"){
		return QVariant::fromValue(at(index)->name.value());
	}
	if (nameId == "m_description"){
		return QVariant::fromValue(at(index)->description.value());
	}
	if (nameId == "m_enabled"){
		return QVariant::fromValue(at(index)->enabled.value());
	}

	return QVariant();
}
COptionsListObject::COptionsListObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &COptionsListObject::totalCountChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COptionsListObject::offsetChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COptionsListObject::optionsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant COptionsListObject::GetTotalCount()
{
	if (totalCount){
		return totalCount.value();
	}

	return QVariant();
}


void COptionsListObject::SetTotalCount(const QVariant& v)
{

	totalCount = v.value<int>();
	totalCountChanged();
}


bool COptionsListObject::hasTotalCount()
{
	 return totalCount.HasValue();
}


QVariant COptionsListObject::GetOffset()
{
	if (offset){
		return offset.value();
	}

	return QVariant();
}


void COptionsListObject::SetOffset(const QVariant& v)
{

	offset = v.value<int>();
	offsetChanged();
}


bool COptionsListObject::hasOffset()
{
	 return offset.HasValue();
}


QVariant COptionsListObject::GetOptions()
{
	if (options){
		if (!m_optionsQObjectPtr.isValid()){
			m_optionsQObjectPtr = CreateObject("options");
			auto itemPtr = m_optionsQObjectPtr.value<sdl::V1_0::imtbase::COptionObjectList*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = options;
		}
		return m_optionsQObjectPtr;
	}

	return QVariant();
}


void COptionsListObject::SetOptions(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::imtbase::COptionObjectList* itemPtr = v.value<sdl::V1_0::imtbase::COptionObjectList*>();
		if (itemPtr != nullptr)  options = static_cast<const decltype(options)::value_type&>(*itemPtr);
	}
	else {
		options = nullptr;
	}
	m_optionsQObjectPtr = v;

	optionsChanged();
}


bool COptionsListObject::hasOptions()
{
	 return options.HasValue();
}


void COptionsListObject::emplaceOptions()
{
	ResetOptions();
	options.emplace();
}


void COptionsListObject::ResetOptions()
{
	auto* itemPtr = m_optionsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_optionsQObjectPtr = QVariant();
}


QVariant COptionsListObject::createOptionsArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::V1_0::imtbase::COptionObject());
}


QString COptionsListObject::toJson() const
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


bool COptionsListObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool COptionsListObject::fromObject(const QJsonObject& jsonObject)
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


QString COptionsListObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant COptionsListObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "options"){
		return QVariant::fromValue(new sdl::V1_0::imtbase::COptionObjectList(this));
	}
	return QVariant();
}


QString COptionsListObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "totalCount")){
		return "totalCount";
	}
	if (propertyName == (QString("m_") + "offset")){
		return "offset";
	}
	if (propertyName == (QString("m_") + "options")){
		return "options";
	}

	return propertyName;
}





bool COptionsListObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int COptionsListObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap COptionsListObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void COptionsListObjectList::append(sdl::V1_0::imtbase::COptionsListObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::COptionsListObjectList* sdl::V1_0::imtbase::COptionsListObjectList::copyMe()
{
	sdl::V1_0::imtbase::COptionsListObjectList* retVal = new sdl::V1_0::imtbase::COptionsListObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::COptionsListObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::COptionsListObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::COptionsListObjectList::addElement(sdl::V1_0::imtbase::COptionsListObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::COptionsListObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::COptionsListObjectList::isEqualWithModel(sdl::V1_0::imtbase::COptionsListObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::COptionsListObjectList::insert(int index, sdl::V1_0::imtbase::COptionsListObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::COptionsListObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::COptionsListObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::COptionsListObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::COptionsListObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_totalCount"){
		return QVariant::fromValue(at(index)->totalCount.value());
	}
	if (nameId == "m_offset"){
		return QVariant::fromValue(at(index)->offset.value());
	}
	if (nameId == "m_options"){
		return itemPtr->GetOptions();
	}

	return QVariant();
}
CSelectionParamObject::CSelectionParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSelectionParamObject::selectedIndexChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSelectionParamObject::constraintsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSelectionParamObject::GetSelectedIndex()
{
	if (selectedIndex){
		return selectedIndex.value();
	}

	return QVariant();
}


void CSelectionParamObject::SetSelectedIndex(const QVariant& v)
{

	selectedIndex = v.value<int>();
	selectedIndexChanged();
}


bool CSelectionParamObject::hasSelectedIndex()
{
	 return selectedIndex.HasValue();
}


QVariant CSelectionParamObject::GetConstraints()
{
	if (constraints){
		if (!m_constraintsQObjectPtr.isValid()){
			m_constraintsQObjectPtr = CreateObject("constraints");
			auto itemPtr = m_constraintsQObjectPtr.value<sdl::V1_0::imtbase::COptionsListObject*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = constraints;
		}
		return m_constraintsQObjectPtr;
	}

	return QVariant();
}


void CSelectionParamObject::SetConstraints(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::imtbase::COptionsListObject* itemPtr = v.value<sdl::V1_0::imtbase::COptionsListObject*>();
		if (itemPtr != nullptr)  constraints = static_cast<const decltype(constraints)::value_type&>(*itemPtr);
	}
	else {
		constraints = nullptr;
	}
	m_constraintsQObjectPtr = v;

	constraintsChanged();
}


bool CSelectionParamObject::hasConstraints()
{
	 return constraints.HasValue();
}


void CSelectionParamObject::emplaceConstraints()
{
	ResetConstraints();
	constraints.emplace();
}


void CSelectionParamObject::ResetConstraints()
{
	auto* itemPtr = m_constraintsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_constraintsQObjectPtr = QVariant();
}


QString CSelectionParamObject::toJson() const
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


bool CSelectionParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSelectionParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CSelectionParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSelectionParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "constraints"){
		return QVariant::fromValue(new sdl::V1_0::imtbase::COptionsListObject(this));
	}
	return QVariant();
}


QString CSelectionParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "selectedIndex")){
		return "selectedIndex";
	}
	if (propertyName == (QString("m_") + "constraints")){
		return "constraints";
	}

	return propertyName;
}





bool CSelectionParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSelectionParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSelectionParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSelectionParamObjectList::append(sdl::V1_0::imtbase::CSelectionParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CSelectionParamObjectList* sdl::V1_0::imtbase::CSelectionParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CSelectionParamObjectList* retVal = new sdl::V1_0::imtbase::CSelectionParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CSelectionParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CSelectionParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CSelectionParamObjectList::addElement(sdl::V1_0::imtbase::CSelectionParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CSelectionParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CSelectionParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CSelectionParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CSelectionParamObjectList::insert(int index, sdl::V1_0::imtbase::CSelectionParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CSelectionParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CSelectionParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CSelectionParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CSelectionParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_selectedIndex"){
		return QVariant::fromValue(at(index)->selectedIndex.value());
	}
	if (nameId == "m_constraints"){
		return itemPtr->GetConstraints();
	}

	return QVariant();
}
CSchedulerParamObject::CSchedulerParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CSchedulerParamObject::startTimeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSchedulerParamObject::intervalChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSchedulerParamObject::GetStartTime()
{
	if (startTime){
		return startTime.value();
	}

	return QVariant();
}


void CSchedulerParamObject::SetStartTime(const QVariant& v)
{

	startTime = v.value<QString>();
	startTimeChanged();
}


bool CSchedulerParamObject::hasStartTime()
{
	 return startTime.HasValue();
}


QVariant CSchedulerParamObject::GetInterval()
{
	if (interval){
		return interval.value();
	}

	return QVariant();
}


void CSchedulerParamObject::SetInterval(const QVariant& v)
{

	interval = v.value<int>();
	intervalChanged();
}


bool CSchedulerParamObject::hasInterval()
{
	 return interval.HasValue();
}


QString CSchedulerParamObject::toJson() const
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


bool CSchedulerParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CSchedulerParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CSchedulerParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CSchedulerParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CSchedulerParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "startTime")){
		return "startTime";
	}
	if (propertyName == (QString("m_") + "interval")){
		return "interval";
	}

	return propertyName;
}





bool CSchedulerParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CSchedulerParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CSchedulerParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CSchedulerParamObjectList::append(sdl::V1_0::imtbase::CSchedulerParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CSchedulerParamObjectList* sdl::V1_0::imtbase::CSchedulerParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CSchedulerParamObjectList* retVal = new sdl::V1_0::imtbase::CSchedulerParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CSchedulerParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CSchedulerParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CSchedulerParamObjectList::addElement(sdl::V1_0::imtbase::CSchedulerParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CSchedulerParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CSchedulerParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CSchedulerParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CSchedulerParamObjectList::insert(int index, sdl::V1_0::imtbase::CSchedulerParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CSchedulerParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CSchedulerParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CSchedulerParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CSchedulerParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_startTime"){
		return QVariant::fromValue(at(index)->startTime.value());
	}
	if (nameId == "m_interval"){
		return QVariant::fromValue(at(index)->interval.value());
	}

	return QVariant();
}
CBackupSettingsObject::CBackupSettingsObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CBackupSettingsObject::schedulerParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CBackupSettingsObject::folderPathChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CBackupSettingsObject::GetSchedulerParam()
{
	if (schedulerParam){
		if (!m_schedulerParamQObjectPtr.isValid()){
			m_schedulerParamQObjectPtr = CreateObject("schedulerParam");
			auto itemPtr = m_schedulerParamQObjectPtr.value<sdl::V1_0::imtbase::CSchedulerParamObject*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = schedulerParam;
		}
		return m_schedulerParamQObjectPtr;
	}

	return QVariant();
}


void CBackupSettingsObject::SetSchedulerParam(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::imtbase::CSchedulerParamObject* itemPtr = v.value<sdl::V1_0::imtbase::CSchedulerParamObject*>();
		if (itemPtr != nullptr)  schedulerParam = static_cast<const decltype(schedulerParam)::value_type&>(*itemPtr);
	}
	else {
		schedulerParam = nullptr;
	}
	m_schedulerParamQObjectPtr = v;

	schedulerParamChanged();
}


bool CBackupSettingsObject::hasSchedulerParam()
{
	 return schedulerParam.HasValue();
}


void CBackupSettingsObject::emplaceSchedulerParam()
{
	ResetSchedulerParam();
	schedulerParam.emplace();
}


void CBackupSettingsObject::ResetSchedulerParam()
{
	auto* itemPtr = m_schedulerParamQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_schedulerParamQObjectPtr = QVariant();
}


QVariant CBackupSettingsObject::GetFolderPath()
{
	if (folderPath){
		return folderPath.value();
	}

	return QVariant();
}


void CBackupSettingsObject::SetFolderPath(const QVariant& v)
{

	folderPath = v.value<QString>();
	folderPathChanged();
}


bool CBackupSettingsObject::hasFolderPath()
{
	 return folderPath.HasValue();
}


QString CBackupSettingsObject::toJson() const
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


bool CBackupSettingsObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CBackupSettingsObject::fromObject(const QJsonObject& jsonObject)
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


QString CBackupSettingsObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CBackupSettingsObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "schedulerParam"){
		return QVariant::fromValue(new sdl::V1_0::imtbase::CSchedulerParamObject(this));
	}
	return QVariant();
}


QString CBackupSettingsObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "schedulerParam")){
		return "schedulerParam";
	}
	if (propertyName == (QString("m_") + "folderPath")){
		return "folderPath";
	}

	return propertyName;
}





bool CBackupSettingsObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CBackupSettingsObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CBackupSettingsObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CBackupSettingsObjectList::append(sdl::V1_0::imtbase::CBackupSettingsObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CBackupSettingsObjectList* sdl::V1_0::imtbase::CBackupSettingsObjectList::copyMe()
{
	sdl::V1_0::imtbase::CBackupSettingsObjectList* retVal = new sdl::V1_0::imtbase::CBackupSettingsObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CBackupSettingsObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CBackupSettingsObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CBackupSettingsObjectList::addElement(sdl::V1_0::imtbase::CBackupSettingsObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CBackupSettingsObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CBackupSettingsObjectList::isEqualWithModel(sdl::V1_0::imtbase::CBackupSettingsObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CBackupSettingsObjectList::insert(int index, sdl::V1_0::imtbase::CBackupSettingsObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CBackupSettingsObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CBackupSettingsObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CBackupSettingsObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CBackupSettingsObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_schedulerParam"){
		return itemPtr->GetSchedulerParam();
	}
	if (nameId == "m_folderPath"){
		return QVariant::fromValue(at(index)->folderPath.value());
	}

	return QVariant();
}
CDatabaseAccessSettingsObject::CDatabaseAccessSettingsObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CDatabaseAccessSettingsObject::dbNameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDatabaseAccessSettingsObject::hostChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDatabaseAccessSettingsObject::portChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDatabaseAccessSettingsObject::dbPathChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDatabaseAccessSettingsObject::usernameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDatabaseAccessSettingsObject::passwordChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CDatabaseAccessSettingsObject::GetDbName()
{
	if (dbName){
		return dbName.value();
	}

	return QVariant();
}


void CDatabaseAccessSettingsObject::SetDbName(const QVariant& v)
{

	dbName = v.value<QString>();
	dbNameChanged();
}


bool CDatabaseAccessSettingsObject::hasDbName()
{
	 return dbName.HasValue();
}


QVariant CDatabaseAccessSettingsObject::GetHost()
{
	if (host){
		return host.value();
	}

	return QVariant();
}


void CDatabaseAccessSettingsObject::SetHost(const QVariant& v)
{

	host = v.value<QString>();
	hostChanged();
}


bool CDatabaseAccessSettingsObject::hasHost()
{
	 return host.HasValue();
}


QVariant CDatabaseAccessSettingsObject::GetPort()
{
	if (port){
		return port.value();
	}

	return QVariant();
}


void CDatabaseAccessSettingsObject::SetPort(const QVariant& v)
{

	port = v.value<int>();
	portChanged();
}


bool CDatabaseAccessSettingsObject::hasPort()
{
	 return port.HasValue();
}


QVariant CDatabaseAccessSettingsObject::GetDbPath()
{
	if (dbPath){
		return dbPath.value();
	}

	return QVariant();
}


void CDatabaseAccessSettingsObject::SetDbPath(const QVariant& v)
{

	dbPath = v.value<QString>();
	dbPathChanged();
}


bool CDatabaseAccessSettingsObject::hasDbPath()
{
	 return dbPath.HasValue();
}


QVariant CDatabaseAccessSettingsObject::GetUsername()
{
	if (username){
		return username.value();
	}

	return QVariant();
}


void CDatabaseAccessSettingsObject::SetUsername(const QVariant& v)
{

	username = v.value<QString>();
	usernameChanged();
}


bool CDatabaseAccessSettingsObject::hasUsername()
{
	 return username.HasValue();
}


QVariant CDatabaseAccessSettingsObject::GetPassword()
{
	if (password){
		return password.value();
	}

	return QVariant();
}


void CDatabaseAccessSettingsObject::SetPassword(const QVariant& v)
{

	password = v.value<QString>();
	passwordChanged();
}


bool CDatabaseAccessSettingsObject::hasPassword()
{
	 return password.HasValue();
}


QString CDatabaseAccessSettingsObject::toJson() const
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


bool CDatabaseAccessSettingsObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CDatabaseAccessSettingsObject::fromObject(const QJsonObject& jsonObject)
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


QString CDatabaseAccessSettingsObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CDatabaseAccessSettingsObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CDatabaseAccessSettingsObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "dbName")){
		return "dbName";
	}
	if (propertyName == (QString("m_") + "host")){
		return "host";
	}
	if (propertyName == (QString("m_") + "port")){
		return "port";
	}
	if (propertyName == (QString("m_") + "dbPath")){
		return "dbPath";
	}
	if (propertyName == (QString("m_") + "username")){
		return "username";
	}
	if (propertyName == (QString("m_") + "password")){
		return "password";
	}

	return propertyName;
}





bool CDatabaseAccessSettingsObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CDatabaseAccessSettingsObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CDatabaseAccessSettingsObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CDatabaseAccessSettingsObjectList::append(sdl::V1_0::imtbase::CDatabaseAccessSettingsObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList* sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::copyMe()
{
	sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList* retVal = new sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::addElement(sdl::V1_0::imtbase::CDatabaseAccessSettingsObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::isEqualWithModel(sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::insert(int index, sdl::V1_0::imtbase::CDatabaseAccessSettingsObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CDatabaseAccessSettingsObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CDatabaseAccessSettingsObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_dbName"){
		return QVariant::fromValue(at(index)->dbName.value());
	}
	if (nameId == "m_host"){
		return QVariant::fromValue(at(index)->host.value());
	}
	if (nameId == "m_port"){
		return QVariant::fromValue(at(index)->port.value());
	}
	if (nameId == "m_dbPath"){
		return QVariant::fromValue(at(index)->dbPath.value());
	}
	if (nameId == "m_username"){
		return QVariant::fromValue(at(index)->username.value());
	}
	if (nameId == "m_password"){
		return QVariant::fromValue(at(index)->password.value());
	}

	return QVariant();
}
CFileNameParamObject::CFileNameParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CFileNameParamObject::pathTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFileNameParamObject::pathChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CFileNameParamObject::GetPathType()
{
	if (pathType){
		return pathType.value();
	}

	return QVariant();
}


void CFileNameParamObject::SetPathType(const QVariant& v)
{

	pathType = v.value<int>();
	pathTypeChanged();
}


bool CFileNameParamObject::hasPathType()
{
	 return pathType.HasValue();
}


QVariant CFileNameParamObject::GetPath()
{
	if (path){
		return path.value();
	}

	return QVariant();
}


void CFileNameParamObject::SetPath(const QVariant& v)
{

	path = v.value<QString>();
	pathChanged();
}


bool CFileNameParamObject::hasPath()
{
	 return path.HasValue();
}


QString CFileNameParamObject::toJson() const
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


bool CFileNameParamObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CFileNameParamObject::fromObject(const QJsonObject& jsonObject)
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


QString CFileNameParamObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CFileNameParamObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CFileNameParamObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "pathType")){
		return "pathType";
	}
	if (propertyName == (QString("m_") + "path")){
		return "path";
	}

	return propertyName;
}





bool CFileNameParamObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CFileNameParamObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CFileNameParamObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CFileNameParamObjectList::append(sdl::V1_0::imtbase::CFileNameParamObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CFileNameParamObjectList* sdl::V1_0::imtbase::CFileNameParamObjectList::copyMe()
{
	sdl::V1_0::imtbase::CFileNameParamObjectList* retVal = new sdl::V1_0::imtbase::CFileNameParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CFileNameParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CFileNameParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CFileNameParamObjectList::addElement(sdl::V1_0::imtbase::CFileNameParamObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CFileNameParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CFileNameParamObjectList::isEqualWithModel(sdl::V1_0::imtbase::CFileNameParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CFileNameParamObjectList::insert(int index, sdl::V1_0::imtbase::CFileNameParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CFileNameParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CFileNameParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CFileNameParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CFileNameParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_pathType"){
		return QVariant::fromValue(at(index)->pathType.value());
	}
	if (nameId == "m_path"){
		return QVariant::fromValue(at(index)->path.value());
	}

	return QVariant();
}
CParamsSetObject::CParamsSetObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CParamsSetObject::paramIdsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamsSetObject::paramTypeIdsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamsSetObject::paramNamesChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamsSetObject::paramDescriptionsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamsSetObject::parametersChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CParamsSetObject::GetParamIds()
{
	if (paramIds){
		QList<QString> tempParamIdsList;
		for (const auto& tempValue: paramIds.value()){
			tempParamIdsList << *tempValue;
		}
		return QVariant::fromValue(tempParamIdsList);
	}

	return QVariant();
}


void CParamsSetObject::SetParamIds(const QVariant& v)
{

	if (!paramIds){
		paramIds.emplace();
	}
	else{
		paramIds->clear();
	}
	for (const auto& tempValue: v.value<QList<QString>>()){
		istd::TSharedNullable<QByteArray> value(tempValue.toUtf8());
		paramIds->append(value);
	}

	paramIdsChanged();
}


bool CParamsSetObject::hasParamIds()
{
	 return paramIds.HasValue();
}


QVariant CParamsSetObject::GetParamTypeIds()
{
	if (paramTypeIds){
		QList<QString> tempParamTypeIdsList;
		for (const auto& tempValue: paramTypeIds.value()){
			tempParamTypeIdsList << *tempValue;
		}
		return QVariant::fromValue(tempParamTypeIdsList);
	}

	return QVariant();
}


void CParamsSetObject::SetParamTypeIds(const QVariant& v)
{

	if (!paramTypeIds){
		paramTypeIds.emplace();
	}
	else{
		paramTypeIds->clear();
	}
	for (const auto& tempValue: v.value<QList<QString>>()){
		istd::TSharedNullable<QByteArray> value(tempValue.toUtf8());
		paramTypeIds->append(value);
	}

	paramTypeIdsChanged();
}


bool CParamsSetObject::hasParamTypeIds()
{
	 return paramTypeIds.HasValue();
}


QVariant CParamsSetObject::GetParamNames()
{
	if (paramNames){
		QList<QString> tempParamNamesList;
		for (const auto& tempValue: paramNames.value()){
			tempParamNamesList << *tempValue;
		}
		return QVariant::fromValue(tempParamNamesList);
	}

	return QVariant();
}


void CParamsSetObject::SetParamNames(const QVariant& v)
{

	if (!paramNames){
		paramNames.emplace();
	}
	else{
		paramNames->clear();
	}
	for (const auto& tempValue: v.value<QList<QString>>()){
		istd::TSharedNullable<QString> value(tempValue);
		paramNames->append(value);
	}

	paramNamesChanged();
}


bool CParamsSetObject::hasParamNames()
{
	 return paramNames.HasValue();
}


QVariant CParamsSetObject::GetParamDescriptions()
{
	if (paramDescriptions){
		QList<QString> tempParamDescriptionsList;
		for (const auto& tempValue: paramDescriptions.value()){
			tempParamDescriptionsList << *tempValue;
		}
		return QVariant::fromValue(tempParamDescriptionsList);
	}

	return QVariant();
}


void CParamsSetObject::SetParamDescriptions(const QVariant& v)
{

	if (!paramDescriptions){
		paramDescriptions.emplace();
	}
	else{
		paramDescriptions->clear();
	}
	for (const auto& tempValue: v.value<QList<QString>>()){
		istd::TSharedNullable<QString> value(tempValue);
		paramDescriptions->append(value);
	}

	paramDescriptionsChanged();
}


bool CParamsSetObject::hasParamDescriptions()
{
	 return paramDescriptions.HasValue();
}


QVariant CParamsSetObject::GetParameters()
{
	if (parameters){
		QList<QString> tempParametersList;
		for (const auto& tempValue: parameters.value()){
			tempParametersList << *tempValue;
		}
		return QVariant::fromValue(tempParametersList);
	}

	return QVariant();
}


void CParamsSetObject::SetParameters(const QVariant& v)
{

	if (!parameters){
		parameters.emplace();
	}
	else{
		parameters->clear();
	}
	for (const auto& tempValue: v.value<QList<QString>>()){
		istd::TSharedNullable<QString> value(tempValue);
		parameters->append(value);
	}

	parametersChanged();
}


bool CParamsSetObject::hasParameters()
{
	 return parameters.HasValue();
}


QString CParamsSetObject::toJson() const
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


bool CParamsSetObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CParamsSetObject::fromObject(const QJsonObject& jsonObject)
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


QString CParamsSetObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CParamsSetObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CParamsSetObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "paramIds")){
		return "paramIds";
	}
	if (propertyName == (QString("m_") + "paramTypeIds")){
		return "paramTypeIds";
	}
	if (propertyName == (QString("m_") + "paramNames")){
		return "paramNames";
	}
	if (propertyName == (QString("m_") + "paramDescriptions")){
		return "paramDescriptions";
	}
	if (propertyName == (QString("m_") + "parameters")){
		return "parameters";
	}

	return propertyName;
}





bool CParamsSetObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CParamsSetObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CParamsSetObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CParamsSetObjectList::append(sdl::V1_0::imtbase::CParamsSetObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CParamsSetObjectList* sdl::V1_0::imtbase::CParamsSetObjectList::copyMe()
{
	sdl::V1_0::imtbase::CParamsSetObjectList* retVal = new sdl::V1_0::imtbase::CParamsSetObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CParamsSetObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CParamsSetObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CParamsSetObjectList::addElement(sdl::V1_0::imtbase::CParamsSetObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CParamsSetObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CParamsSetObjectList::isEqualWithModel(sdl::V1_0::imtbase::CParamsSetObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CParamsSetObjectList::insert(int index, sdl::V1_0::imtbase::CParamsSetObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CParamsSetObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CParamsSetObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CParamsSetObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CParamsSetObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_paramIds"){
		return QVariant::fromValue(at(index)->paramIds.value());
	}
	if (nameId == "m_paramTypeIds"){
		return QVariant::fromValue(at(index)->paramTypeIds.value());
	}
	if (nameId == "m_paramNames"){
		return QVariant::fromValue(at(index)->paramNames.value());
	}
	if (nameId == "m_paramDescriptions"){
		return QVariant::fromValue(at(index)->paramDescriptions.value());
	}
	if (nameId == "m_parameters"){
		return QVariant::fromValue(at(index)->parameters.value());
	}

	return QVariant();
}
CMimeTypeObject::CMimeTypeObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CMimeTypeObject::typeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CMimeTypeObject::treeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CMimeTypeObject::subTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CMimeTypeObject::suffixChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CMimeTypeObject::parametersChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CMimeTypeObject::GetType()
{
	if (type){
		return type.value();
	}

	return QVariant();
}


void CMimeTypeObject::SetType(const QVariant& v)
{

	type = v.value<QString>();
	typeChanged();
}


bool CMimeTypeObject::hasType()
{
	 return type.HasValue();
}


QVariant CMimeTypeObject::GetTree()
{
	if (tree){
		QList<QString> tempTreeList;
		for (const auto& tempValue: tree.value()){
			tempTreeList << *tempValue;
		}
		return QVariant::fromValue(tempTreeList);
	}

	return QVariant();
}


void CMimeTypeObject::SetTree(const QVariant& v)
{

	if (!tree){
		tree.emplace();
	}
	else{
		tree->clear();
	}
	for (const auto& tempValue: v.value<QList<QString>>()){
		istd::TSharedNullable<QString> value(tempValue);
		tree->append(value);
	}

	treeChanged();
}


bool CMimeTypeObject::hasTree()
{
	 return tree.HasValue();
}


QVariant CMimeTypeObject::GetSubType()
{
	if (subType){
		return subType.value();
	}

	return QVariant();
}


void CMimeTypeObject::SetSubType(const QVariant& v)
{

	subType = v.value<QString>();
	subTypeChanged();
}


bool CMimeTypeObject::hasSubType()
{
	 return subType.HasValue();
}


QVariant CMimeTypeObject::GetSuffix()
{
	if (suffix){
		return suffix.value();
	}

	return QVariant();
}


void CMimeTypeObject::SetSuffix(const QVariant& v)
{

	suffix = v.value<QString>();
	suffixChanged();
}


bool CMimeTypeObject::hasSuffix()
{
	 return suffix.HasValue();
}


QVariant CMimeTypeObject::GetParameters()
{
	if (parameters){
		QList<QString> tempParametersList;
		for (const auto& tempValue: parameters.value()){
			tempParametersList << *tempValue;
		}
		return QVariant::fromValue(tempParametersList);
	}

	return QVariant();
}


void CMimeTypeObject::SetParameters(const QVariant& v)
{

	if (!parameters){
		parameters.emplace();
	}
	else{
		parameters->clear();
	}
	for (const auto& tempValue: v.value<QList<QString>>()){
		istd::TSharedNullable<QString> value(tempValue);
		parameters->append(value);
	}

	parametersChanged();
}


bool CMimeTypeObject::hasParameters()
{
	 return parameters.HasValue();
}


QString CMimeTypeObject::toJson() const
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


bool CMimeTypeObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CMimeTypeObject::fromObject(const QJsonObject& jsonObject)
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


QString CMimeTypeObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CMimeTypeObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CMimeTypeObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "type")){
		return "type";
	}
	if (propertyName == (QString("m_") + "tree")){
		return "tree";
	}
	if (propertyName == (QString("m_") + "subType")){
		return "subType";
	}
	if (propertyName == (QString("m_") + "suffix")){
		return "suffix";
	}
	if (propertyName == (QString("m_") + "parameters")){
		return "parameters";
	}

	return propertyName;
}





bool CMimeTypeObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CMimeTypeObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CMimeTypeObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CMimeTypeObjectList::append(sdl::V1_0::imtbase::CMimeTypeObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::imtbase::CMimeTypeObjectList* sdl::V1_0::imtbase::CMimeTypeObjectList::copyMe()
{
	sdl::V1_0::imtbase::CMimeTypeObjectList* retVal = new sdl::V1_0::imtbase::CMimeTypeObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::imtbase::CMimeTypeObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::imtbase::CMimeTypeObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::imtbase::CMimeTypeObjectList::addElement(sdl::V1_0::imtbase::CMimeTypeObject* item)
{
	append(item);
}


void sdl::V1_0::imtbase::CMimeTypeObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::imtbase::CMimeTypeObjectList::isEqualWithModel(sdl::V1_0::imtbase::CMimeTypeObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::imtbase::CMimeTypeObjectList::insert(int index, sdl::V1_0::imtbase::CMimeTypeObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::imtbase::CMimeTypeObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::imtbase::CMimeTypeObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::imtbase::CMimeTypeObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::imtbase::CMimeTypeObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_type"){
		return QVariant::fromValue(at(index)->type.value());
	}
	if (nameId == "m_tree"){
		return QVariant::fromValue(at(index)->tree.value());
	}
	if (nameId == "m_subType"){
		return QVariant::fromValue(at(index)->subType.value());
	}
	if (nameId == "m_suffix"){
		return QVariant::fromValue(at(index)->suffix.value());
	}
	if (nameId == "m_parameters"){
		return QVariant::fromValue(at(index)->parameters.value());
	}

	return QVariant();
}
} // namespace sdl::V1_0::imtbase
