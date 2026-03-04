#include "ImtBaseTypes.h"


namespace sdl::imtbase::ImtBaseTypes
{


QByteArray CTimeRange::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CTimeRange::V1_0::operator==(const V1_0& other) const
{
	return 
				Begin == other.Begin &&
				End == other.End;
}


bool CTimeRange::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CTimeRange::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CTimeRange::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CTimeRange::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CTimeRange::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("Begin") && (gqlObject["Begin"].userType() == QMetaType::QString || gqlObject["Begin"].userType() == QMetaType::QByteArray)){
		Begin = gqlObject["Begin"].toString();
	}

	if (gqlObject.ContainsParam("End") && (gqlObject["End"].userType() == QMetaType::QString || gqlObject["End"].userType() == QMetaType::QByteArray)){
		End = gqlObject["End"].toString();
	}

	return true;
}


bool CTimeRange::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("Begin") && (gqlObject["Begin"].userType() == QMetaType::QString || gqlObject["Begin"].userType() == QMetaType::QByteArray)){
		Begin = gqlObject["Begin"].toString();
	}

	if (gqlObject.ContainsParam("End") && (gqlObject["End"].userType() == QMetaType::QString || gqlObject["End"].userType() == QMetaType::QByteArray)){
		End = gqlObject["End"].toString();
	}

	return true;
}


bool CTimeRange::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CTimeRange::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("Begin") && jsonObject["Begin"].isString()){
		Begin = jsonObject["Begin"].toString();
	}

	if (jsonObject.contains("End") && jsonObject["End"].isString()){
		End = jsonObject["End"].toString();
	}

	return true;
}


bool CTimeRange::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CTimeRange::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CTimeRange::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CTimeRange::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CTimeRange::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CTimeRange::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CTimeRange::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CTimeRange::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CTimeRange::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CTimeRange::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CSdlSize::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSdlSize::V1_0::operator==(const V1_0& other) const
{
	return 
				width.has_value() == other.width.has_value() &&
				((width.has_value() && other.width.has_value()) ?
					qFuzzyCompare(*width, *other.width) : true) &&
				height.has_value() == other.height.has_value() &&
				((height.has_value() && other.height.has_value()) ?
					qFuzzyCompare(*height, *other.height) : true);
}


bool CSdlSize::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CSdlSize::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CSdlSize::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CSdlSize::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CSdlSize::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CSdlSize::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("width") && (gqlObject["width"].userType() == QMetaType::Float || gqlObject["width"].userType() == QMetaType::Double || gqlObject["width"].userType() == QMetaType::Int || gqlObject["width"].userType() == QMetaType::UInt || gqlObject["width"].userType() == QMetaType::LongLong || gqlObject["width"].userType() == QMetaType::ULongLong || gqlObject["width"].userType() == QMetaType::Long || gqlObject["width"].userType() == QMetaType::Short || gqlObject["width"].userType() == QMetaType::ULong || gqlObject["width"].userType() == QMetaType::UShort || gqlObject["width"].userType() == QMetaType::UChar)){
		width = gqlObject["width"].toDouble();
	}

	if (gqlObject.ContainsParam("height") && (gqlObject["height"].userType() == QMetaType::Float || gqlObject["height"].userType() == QMetaType::Double || gqlObject["height"].userType() == QMetaType::Int || gqlObject["height"].userType() == QMetaType::UInt || gqlObject["height"].userType() == QMetaType::LongLong || gqlObject["height"].userType() == QMetaType::ULongLong || gqlObject["height"].userType() == QMetaType::Long || gqlObject["height"].userType() == QMetaType::Short || gqlObject["height"].userType() == QMetaType::ULong || gqlObject["height"].userType() == QMetaType::UShort || gqlObject["height"].userType() == QMetaType::UChar)){
		height = gqlObject["height"].toDouble();
	}

	return true;
}


bool CSdlSize::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CSdlSize::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool CSdlSize::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CSdlSize::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CSdlSize::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CSdlSize::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CSdlSize::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CSdlSize::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CSdlSize::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CSdlSize::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CSdlSize::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CSdlSize::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CSdlPoint::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSdlPoint::V1_0::operator==(const V1_0& other) const
{
	return 
				x.has_value() == other.x.has_value() &&
				((x.has_value() && other.x.has_value()) ?
					qFuzzyCompare(*x, *other.x) : true) &&
				y.has_value() == other.y.has_value() &&
				((y.has_value() && other.y.has_value()) ?
					qFuzzyCompare(*y, *other.y) : true);
}


bool CSdlPoint::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CSdlPoint::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CSdlPoint::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CSdlPoint::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CSdlPoint::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CSdlPoint::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("x") && (gqlObject["x"].userType() == QMetaType::Float || gqlObject["x"].userType() == QMetaType::Double || gqlObject["x"].userType() == QMetaType::Int || gqlObject["x"].userType() == QMetaType::UInt || gqlObject["x"].userType() == QMetaType::LongLong || gqlObject["x"].userType() == QMetaType::ULongLong || gqlObject["x"].userType() == QMetaType::Long || gqlObject["x"].userType() == QMetaType::Short || gqlObject["x"].userType() == QMetaType::ULong || gqlObject["x"].userType() == QMetaType::UShort || gqlObject["x"].userType() == QMetaType::UChar)){
		x = gqlObject["x"].toDouble();
	}

	if (gqlObject.ContainsParam("y") && (gqlObject["y"].userType() == QMetaType::Float || gqlObject["y"].userType() == QMetaType::Double || gqlObject["y"].userType() == QMetaType::Int || gqlObject["y"].userType() == QMetaType::UInt || gqlObject["y"].userType() == QMetaType::LongLong || gqlObject["y"].userType() == QMetaType::ULongLong || gqlObject["y"].userType() == QMetaType::Long || gqlObject["y"].userType() == QMetaType::Short || gqlObject["y"].userType() == QMetaType::ULong || gqlObject["y"].userType() == QMetaType::UShort || gqlObject["y"].userType() == QMetaType::UChar)){
		y = gqlObject["y"].toDouble();
	}

	return true;
}


bool CSdlPoint::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CSdlPoint::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool CSdlPoint::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CSdlPoint::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CSdlPoint::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CSdlPoint::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CSdlPoint::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CSdlPoint::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CSdlPoint::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CSdlPoint::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CSdlPoint::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CSdlPoint::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CParamTypeIds::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CParamTypeIds::V1_0::operator==(const V1_0& other) const
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


bool CParamTypeIds::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CParamTypeIds::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CParamTypeIds::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CParamTypeIds::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CParamTypeIds::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CParamTypeIds::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CParamTypeIds::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (UrlParam){
		jsonObject["UrlParam"] = QJsonValue::fromVariant(*UrlParam);
	}

	if (IdParam){
		jsonObject["IdParam"] = QJsonValue::fromVariant(*IdParam);
	}

	if (TextParam){
		jsonObject["TextParam"] = QJsonValue::fromVariant(*TextParam);
	}

	if (TextView){
		jsonObject["TextView"] = QJsonValue::fromVariant(*TextView);
	}

	if (SelectionParam){
		jsonObject["SelectionParam"] = QJsonValue::fromVariant(*SelectionParam);
	}

	if (SchedulerParam){
		jsonObject["SchedulerParam"] = QJsonValue::fromVariant(*SchedulerParam);
	}

	if (BackupSettings){
		jsonObject["BackupSettings"] = QJsonValue::fromVariant(*BackupSettings);
	}

	if (DatabaseAccessSettings){
		jsonObject["DatabaseAccessSettings"] = QJsonValue::fromVariant(*DatabaseAccessSettings);
	}

	if (ParamsSet){
		jsonObject["ParamsSet"] = QJsonValue::fromVariant(*ParamsSet);
	}

	if (FileNameParam){
		jsonObject["FileNameParam"] = QJsonValue::fromVariant(*FileNameParam);
	}

	if (IntegerParam){
		jsonObject["IntegerParam"] = QJsonValue::fromVariant(*IntegerParam);
	}

	if (DoubleParam){
		jsonObject["DoubleParam"] = QJsonValue::fromVariant(*DoubleParam);
	}

	if (PasswordParam){
		jsonObject["PasswordParam"] = QJsonValue::fromVariant(*PasswordParam);
	}

	if (EnableableParam){
		jsonObject["EnableableParam"] = QJsonValue::fromVariant(*EnableableParam);
	}

	jsonObject["__typename"] = "ParamTypeIds";

	return true;
}


bool CParamTypeIds::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool CParamTypeIds::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CParamTypeIds::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CParamTypeIds::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CParamTypeIds::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CParamTypeIds::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CParamTypeIds::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CParamTypeIds::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CParamTypeIds::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CParamTypeIds::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CParamTypeIds::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CUrlParam::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CUrlParam::V1_0::operator==(const V1_0& other) const
{
	return 
				scheme == other.scheme &&
				host == other.host &&
				port == other.port &&
				path == other.path;
}


bool CUrlParam::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CUrlParam::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CUrlParam::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CUrlParam::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CUrlParam::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CUrlParam::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CUrlParam::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CUrlParam::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool CUrlParam::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CUrlParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CUrlParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CUrlParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CUrlParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CUrlParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CUrlParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CUrlParam::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CUrlParam::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CUrlParam::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CIdParam::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CIdParam::V1_0::operator==(const V1_0& other) const
{
	return 
				id == other.id;
}


bool CIdParam::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "IdParam", modelIndex);

	if (id){
		model.SetData("id", *id, modelIndex);
	}


	return true;
}


bool CIdParam::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant idData = model.GetData("id", modelIndex);
	if (!idData.isNull()){
		id = idData.toByteArray();
	}

	return true;
}


bool CIdParam::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant idData = model.GetData("id", modelIndex);
	if (!idData.isNull()){
		id = idData.toByteArray();
	}

	return true;
}


bool CIdParam::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (id){
		gqlObject.InsertParam("id", QVariant(*id));
	}

	gqlObject.InsertParam("__typename", QVariant("IdParam"));

	return true;
}


bool CIdParam::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("id") && (gqlObject["id"].userType() == QMetaType::QString || gqlObject["id"].userType() == QMetaType::QByteArray)){
		id = gqlObject["id"].toByteArray();
	}

	return true;
}


bool CIdParam::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("id") && (gqlObject["id"].userType() == QMetaType::QString || gqlObject["id"].userType() == QMetaType::QByteArray)){
		id = gqlObject["id"].toByteArray();
	}

	return true;
}


bool CIdParam::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (id){
		jsonObject["id"] = QJsonValue::fromVariant(*id);
	}

	jsonObject["__typename"] = "IdParam";

	return true;
}


bool CIdParam::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("id") && jsonObject["id"].isString()){
		id = jsonObject["id"].toString().toUtf8();
	}

	return true;
}


bool CIdParam::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("id") && jsonObject["id"].isString()){
		id = jsonObject["id"].toString().toUtf8();
	}

	return true;
}


// serialize methods

bool CIdParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CIdParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CIdParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CIdParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CIdParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CIdParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CIdParam::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CIdParam::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CIdParam::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CTextParam::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CTextParam::V1_0::operator==(const V1_0& other) const
{
	return 
				text == other.text;
}


bool CTextParam::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "TextParam", modelIndex);

	if (text){
		model.SetData("text", *text, modelIndex);
	}


	return true;
}


bool CTextParam::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant textData = model.GetData("text", modelIndex);
	if (!textData.isNull()){
		text = textData.toString();
	}

	return true;
}


bool CTextParam::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant textData = model.GetData("text", modelIndex);
	if (!textData.isNull()){
		text = textData.toString();
	}

	return true;
}


bool CTextParam::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (text){
		gqlObject.InsertParam("text", QVariant(*text));
	}

	gqlObject.InsertParam("__typename", QVariant("TextParam"));

	return true;
}


bool CTextParam::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("text") && (gqlObject["text"].userType() == QMetaType::QString || gqlObject["text"].userType() == QMetaType::QByteArray)){
		text = gqlObject["text"].toString();
	}

	return true;
}


bool CTextParam::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("text") && (gqlObject["text"].userType() == QMetaType::QString || gqlObject["text"].userType() == QMetaType::QByteArray)){
		text = gqlObject["text"].toString();
	}

	return true;
}


bool CTextParam::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (text){
		jsonObject["text"] = QJsonValue::fromVariant(*text);
	}

	jsonObject["__typename"] = "TextParam";

	return true;
}


bool CTextParam::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("text") && jsonObject["text"].isString()){
		text = jsonObject["text"].toString();
	}

	return true;
}


bool CTextParam::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("text") && jsonObject["text"].isString()){
		text = jsonObject["text"].toString();
	}

	return true;
}


// serialize methods

bool CTextParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CTextParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CTextParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CTextParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CTextParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CTextParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CTextParam::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CTextParam::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CTextParam::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CEnableableParam::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CEnableableParam::V1_0::operator==(const V1_0& other) const
{
	return 
				value == other.value;
}


bool CEnableableParam::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "EnableableParam", modelIndex);

	if (value){
		model.SetData("value", *value, modelIndex);
	}


	return true;
}


bool CEnableableParam::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
		value = valueData.toBool();
	}

	return true;
}


bool CEnableableParam::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
		value = valueData.toBool();
	}

	return true;
}


bool CEnableableParam::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (value){
		gqlObject.InsertParam("value", QVariant(*value));
	}

	gqlObject.InsertParam("__typename", QVariant("EnableableParam"));

	return true;
}


bool CEnableableParam::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("value") && (gqlObject["value"].userType() == QMetaType::Bool)){
		value = gqlObject["value"].toBool();
	}

	return true;
}


bool CEnableableParam::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("value") && (gqlObject["value"].userType() == QMetaType::Bool)){
		value = gqlObject["value"].toBool();
	}

	return true;
}


bool CEnableableParam::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (value){
		jsonObject["value"] = QJsonValue::fromVariant(*value);
	}

	jsonObject["__typename"] = "EnableableParam";

	return true;
}


bool CEnableableParam::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("value") && jsonObject["value"].isBool()){
		value = jsonObject["value"].toBool();
	}

	return true;
}


bool CEnableableParam::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("value") && jsonObject["value"].isBool()){
		value = jsonObject["value"].toBool();
	}

	return true;
}


// serialize methods

bool CEnableableParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CEnableableParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CEnableableParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CEnableableParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CEnableableParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CEnableableParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CEnableableParam::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CEnableableParam::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CEnableableParam::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CIntegerParam::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CIntegerParam::V1_0::operator==(const V1_0& other) const
{
	return 
				value == other.value;
}


bool CIntegerParam::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "IntegerParam", modelIndex);

	if (value){
		model.SetData("value", *value, modelIndex);
	}


	return true;
}


bool CIntegerParam::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
		value = valueData.toInt();
	}

	return true;
}


bool CIntegerParam::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
		value = valueData.toInt();
	}

	return true;
}


bool CIntegerParam::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (value){
		gqlObject.InsertParam("value", QVariant(*value));
	}

	gqlObject.InsertParam("__typename", QVariant("IntegerParam"));

	return true;
}


bool CIntegerParam::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("value") && (gqlObject["value"].userType() == QMetaType::Int || gqlObject["value"].userType() == QMetaType::UInt || gqlObject["value"].userType() == QMetaType::LongLong || gqlObject["value"].userType() == QMetaType::ULongLong || gqlObject["value"].userType() == QMetaType::Long || gqlObject["value"].userType() == QMetaType::Short || gqlObject["value"].userType() == QMetaType::ULong || gqlObject["value"].userType() == QMetaType::UShort || gqlObject["value"].userType() == QMetaType::UChar)){
		value = gqlObject["value"].toInt();
	}

	return true;
}


bool CIntegerParam::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("value") && (gqlObject["value"].userType() == QMetaType::Int || gqlObject["value"].userType() == QMetaType::UInt || gqlObject["value"].userType() == QMetaType::LongLong || gqlObject["value"].userType() == QMetaType::ULongLong || gqlObject["value"].userType() == QMetaType::Long || gqlObject["value"].userType() == QMetaType::Short || gqlObject["value"].userType() == QMetaType::ULong || gqlObject["value"].userType() == QMetaType::UShort || gqlObject["value"].userType() == QMetaType::UChar)){
		value = gqlObject["value"].toInt();
	}

	return true;
}


bool CIntegerParam::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (value){
		jsonObject["value"] = QJsonValue::fromVariant(*value);
	}

	jsonObject["__typename"] = "IntegerParam";

	return true;
}


bool CIntegerParam::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("value") && jsonObject["value"].isDouble()){
		value = jsonObject["value"].toInt();
	}

	return true;
}


bool CIntegerParam::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("value") && jsonObject["value"].isDouble()){
		value = jsonObject["value"].toInt();
	}

	return true;
}


// serialize methods

bool CIntegerParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CIntegerParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CIntegerParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CIntegerParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CIntegerParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CIntegerParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CIntegerParam::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CIntegerParam::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CIntegerParam::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CDoubleParam::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CDoubleParam::V1_0::operator==(const V1_0& other) const
{
	return 
				value.has_value() == other.value.has_value() &&
				((value.has_value() && other.value.has_value()) ?
					qFuzzyCompare(*value, *other.value) : true);
}


bool CDoubleParam::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "DoubleParam", modelIndex);

	if (value){
		model.SetData("value", *value, modelIndex);
	}


	return true;
}


bool CDoubleParam::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
		value = valueData.toDouble();
	}

	return true;
}


bool CDoubleParam::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant valueData = model.GetData("value", modelIndex);
	if (!valueData.isNull()){
		value = valueData.toDouble();
	}

	return true;
}


bool CDoubleParam::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (value){
		gqlObject.InsertParam("value", QVariant(*value));
	}

	gqlObject.InsertParam("__typename", QVariant("DoubleParam"));

	return true;
}


bool CDoubleParam::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("value") && (gqlObject["value"].userType() == QMetaType::Float || gqlObject["value"].userType() == QMetaType::Double || gqlObject["value"].userType() == QMetaType::Int || gqlObject["value"].userType() == QMetaType::UInt || gqlObject["value"].userType() == QMetaType::LongLong || gqlObject["value"].userType() == QMetaType::ULongLong || gqlObject["value"].userType() == QMetaType::Long || gqlObject["value"].userType() == QMetaType::Short || gqlObject["value"].userType() == QMetaType::ULong || gqlObject["value"].userType() == QMetaType::UShort || gqlObject["value"].userType() == QMetaType::UChar)){
		value = gqlObject["value"].toDouble();
	}

	return true;
}


bool CDoubleParam::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("value") && (gqlObject["value"].userType() == QMetaType::Float || gqlObject["value"].userType() == QMetaType::Double || gqlObject["value"].userType() == QMetaType::Int || gqlObject["value"].userType() == QMetaType::UInt || gqlObject["value"].userType() == QMetaType::LongLong || gqlObject["value"].userType() == QMetaType::ULongLong || gqlObject["value"].userType() == QMetaType::Long || gqlObject["value"].userType() == QMetaType::Short || gqlObject["value"].userType() == QMetaType::ULong || gqlObject["value"].userType() == QMetaType::UShort || gqlObject["value"].userType() == QMetaType::UChar)){
		value = gqlObject["value"].toDouble();
	}

	return true;
}


bool CDoubleParam::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (value){
		jsonObject["value"] = QJsonValue::fromVariant(*value);
	}

	jsonObject["__typename"] = "DoubleParam";

	return true;
}


bool CDoubleParam::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("value") && jsonObject["value"].isDouble()){
		value = jsonObject["value"].toDouble();
	}

	return true;
}


bool CDoubleParam::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("value") && jsonObject["value"].isDouble()){
		value = jsonObject["value"].toDouble();
	}

	return true;
}


// serialize methods

bool CDoubleParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CDoubleParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CDoubleParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CDoubleParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CDoubleParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CDoubleParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CDoubleParam::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CDoubleParam::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CDoubleParam::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray COption::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool COption::V1_0::operator==(const V1_0& other) const
{
	return 
				id == other.id &&
				name == other.name &&
				description == other.description &&
				enabled == other.enabled;
}


bool COption::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool COption::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool COption::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool COption::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool COption::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool COption::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool COption::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (id){
		jsonObject["id"] = QJsonValue::fromVariant(*id);
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


bool COption::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool COption::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool COption::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool COption::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool COption::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool COption::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool COption::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool COption::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool COption::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool COption::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool COption::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray COptionsList::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool COptionsList::V1_0::operator==(const V1_0& other) const
{
	return 
				totalCount == other.totalCount &&
				offset == other.offset &&
				options == other.options;
}


bool COptionsList::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool COptionsList::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool COptionsList::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool COptionsList::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool COptionsList::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool COptionsList::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool COptionsList::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool COptionsList::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool COptionsList::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool COptionsList::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool COptionsList::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool COptionsList::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool COptionsList::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool COptionsList::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool COptionsList::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool COptionsList::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool COptionsList::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool COptionsList::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CSelectionParam::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSelectionParam::V1_0::operator==(const V1_0& other) const
{
	return 
				selectedIndex == other.selectedIndex &&
				constraints == other.constraints;
}


bool CSelectionParam::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CSelectionParam::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CSelectionParam::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CSelectionParam::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CSelectionParam::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CSelectionParam::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CSelectionParam::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CSelectionParam::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool CSelectionParam::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CSelectionParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CSelectionParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CSelectionParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CSelectionParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CSelectionParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CSelectionParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CSelectionParam::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CSelectionParam::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CSelectionParam::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CSchedulerParam::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CSchedulerParam::V1_0::operator==(const V1_0& other) const
{
	return 
				startTime == other.startTime &&
				interval == other.interval;
}


bool CSchedulerParam::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CSchedulerParam::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CSchedulerParam::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CSchedulerParam::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CSchedulerParam::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("startTime") && (gqlObject["startTime"].userType() == QMetaType::QString || gqlObject["startTime"].userType() == QMetaType::QByteArray)){
		startTime = gqlObject["startTime"].toString();
	}

	if (gqlObject.ContainsParam("interval") && (gqlObject["interval"].userType() == QMetaType::Int || gqlObject["interval"].userType() == QMetaType::UInt || gqlObject["interval"].userType() == QMetaType::LongLong || gqlObject["interval"].userType() == QMetaType::ULongLong || gqlObject["interval"].userType() == QMetaType::Long || gqlObject["interval"].userType() == QMetaType::Short || gqlObject["interval"].userType() == QMetaType::ULong || gqlObject["interval"].userType() == QMetaType::UShort || gqlObject["interval"].userType() == QMetaType::UChar)){
		interval = gqlObject["interval"].toInt();
	}

	return true;
}


bool CSchedulerParam::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("startTime") && (gqlObject["startTime"].userType() == QMetaType::QString || gqlObject["startTime"].userType() == QMetaType::QByteArray)){
		startTime = gqlObject["startTime"].toString();
	}

	if (gqlObject.ContainsParam("interval") && (gqlObject["interval"].userType() == QMetaType::Int || gqlObject["interval"].userType() == QMetaType::UInt || gqlObject["interval"].userType() == QMetaType::LongLong || gqlObject["interval"].userType() == QMetaType::ULongLong || gqlObject["interval"].userType() == QMetaType::Long || gqlObject["interval"].userType() == QMetaType::Short || gqlObject["interval"].userType() == QMetaType::ULong || gqlObject["interval"].userType() == QMetaType::UShort || gqlObject["interval"].userType() == QMetaType::UChar)){
		interval = gqlObject["interval"].toInt();
	}

	return true;
}


bool CSchedulerParam::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CSchedulerParam::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("startTime") && jsonObject["startTime"].isString()){
		startTime = jsonObject["startTime"].toString();
	}

	if (jsonObject.contains("interval") && jsonObject["interval"].isDouble()){
		interval = jsonObject["interval"].toInt();
	}

	return true;
}


bool CSchedulerParam::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CSchedulerParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CSchedulerParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CSchedulerParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CSchedulerParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CSchedulerParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CSchedulerParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CSchedulerParam::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CSchedulerParam::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CSchedulerParam::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CBackupSettings::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CBackupSettings::V1_0::operator==(const V1_0& other) const
{
	return 
				schedulerParam == other.schedulerParam &&
				folderPath == other.folderPath;
}


bool CBackupSettings::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CBackupSettings::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CBackupSettings::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CBackupSettings::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CBackupSettings::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CBackupSettings::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CBackupSettings::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CBackupSettings::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool CBackupSettings::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CBackupSettings::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CBackupSettings::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CBackupSettings::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CBackupSettings::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CBackupSettings::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CBackupSettings::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CBackupSettings::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CBackupSettings::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CBackupSettings::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CDatabaseAccessSettings::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CDatabaseAccessSettings::V1_0::operator==(const V1_0& other) const
{
	return 
				dbName == other.dbName &&
				host == other.host &&
				port == other.port &&
				dbPath == other.dbPath &&
				username == other.username &&
				password == other.password;
}


bool CDatabaseAccessSettings::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CDatabaseAccessSettings::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CDatabaseAccessSettings::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CDatabaseAccessSettings::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CDatabaseAccessSettings::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CDatabaseAccessSettings::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CDatabaseAccessSettings::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CDatabaseAccessSettings::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool CDatabaseAccessSettings::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CDatabaseAccessSettings::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CDatabaseAccessSettings::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CDatabaseAccessSettings::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CDatabaseAccessSettings::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CDatabaseAccessSettings::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CDatabaseAccessSettings::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CDatabaseAccessSettings::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CDatabaseAccessSettings::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CDatabaseAccessSettings::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CFileNameParam::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CFileNameParam::V1_0::operator==(const V1_0& other) const
{
	return 
				pathType == other.pathType &&
				path == other.path;
}


bool CFileNameParam::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CFileNameParam::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CFileNameParam::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CFileNameParam::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CFileNameParam::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("pathType") && (gqlObject["pathType"].userType() == QMetaType::Int || gqlObject["pathType"].userType() == QMetaType::UInt || gqlObject["pathType"].userType() == QMetaType::LongLong || gqlObject["pathType"].userType() == QMetaType::ULongLong || gqlObject["pathType"].userType() == QMetaType::Long || gqlObject["pathType"].userType() == QMetaType::Short || gqlObject["pathType"].userType() == QMetaType::ULong || gqlObject["pathType"].userType() == QMetaType::UShort || gqlObject["pathType"].userType() == QMetaType::UChar)){
		pathType = gqlObject["pathType"].toInt();
	}

	if (gqlObject.ContainsParam("path") && (gqlObject["path"].userType() == QMetaType::QString || gqlObject["path"].userType() == QMetaType::QByteArray)){
		path = gqlObject["path"].toString();
	}

	return true;
}


bool CFileNameParam::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("pathType") && (gqlObject["pathType"].userType() == QMetaType::Int || gqlObject["pathType"].userType() == QMetaType::UInt || gqlObject["pathType"].userType() == QMetaType::LongLong || gqlObject["pathType"].userType() == QMetaType::ULongLong || gqlObject["pathType"].userType() == QMetaType::Long || gqlObject["pathType"].userType() == QMetaType::Short || gqlObject["pathType"].userType() == QMetaType::ULong || gqlObject["pathType"].userType() == QMetaType::UShort || gqlObject["pathType"].userType() == QMetaType::UChar)){
		pathType = gqlObject["pathType"].toInt();
	}

	if (gqlObject.ContainsParam("path") && (gqlObject["path"].userType() == QMetaType::QString || gqlObject["path"].userType() == QMetaType::QByteArray)){
		path = gqlObject["path"].toString();
	}

	return true;
}


bool CFileNameParam::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CFileNameParam::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("pathType") && jsonObject["pathType"].isDouble()){
		pathType = jsonObject["pathType"].toInt();
	}

	if (jsonObject.contains("path") && jsonObject["path"].isString()){
		path = jsonObject["path"].toString();
	}

	return true;
}


bool CFileNameParam::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CFileNameParam::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CFileNameParam::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CFileNameParam::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CFileNameParam::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CFileNameParam::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CFileNameParam::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CFileNameParam::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CFileNameParam::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CFileNameParam::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CParamsSet::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CParamsSet::V1_0::operator==(const V1_0& other) const
{
	return 
				paramIds == other.paramIds &&
				paramTypeIds == other.paramTypeIds &&
				paramNames == other.paramNames &&
				paramDescriptions == other.paramDescriptions &&
				parameters == other.parameters;
}


bool CParamsSet::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CParamsSet::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CParamsSet::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CParamsSet::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CParamsSet::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CParamsSet::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CParamsSet::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CParamsSet::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool CParamsSet::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CParamsSet::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CParamsSet::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CParamsSet::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CParamsSet::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CParamsSet::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CParamsSet::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CParamsSet::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CParamsSet::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CParamsSet::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




QByteArray CMimeType::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CMimeType::V1_0::operator==(const V1_0& other) const
{
	return 
				type == other.type &&
				tree == other.tree &&
				subType == other.subType &&
				suffix == other.suffix &&
				parameters == other.parameters;
}


bool CMimeType::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CMimeType::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CMimeType::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool CMimeType::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CMimeType::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CMimeType::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool CMimeType::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CMimeType::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool CMimeType::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
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

bool CMimeType::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CMimeType::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CMimeType::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
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


bool CMimeType::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CMimeType::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CMimeType::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
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


bool CMimeType::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

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


bool CMimeType::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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


bool CMimeType::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
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




CTimeRangeObject::CTimeRangeObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CTimeRangeObject::beginChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CTimeRangeObject::endChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CTimeRangeObject::GetBegin()
{
	if (Version_1_0 && Version_1_0->Begin){
		return Version_1_0->Begin.value();
	}

	return QVariant();
}


void CTimeRangeObject::SetBegin(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->Begin = v.value<QString>();
	beginChanged();
}


bool CTimeRangeObject::hasBegin()
{
	 return Version_1_0 && Version_1_0->Begin.HasValue();
}


QVariant CTimeRangeObject::GetEnd()
{
	if (Version_1_0 && Version_1_0->End){
		return Version_1_0->End.value();
	}

	return QVariant();
}


void CTimeRangeObject::SetEnd(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->End = v.value<QString>();
	endChanged();
}


bool CTimeRangeObject::hasEnd()
{
	 return Version_1_0 && Version_1_0->End.HasValue();
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


void CTimeRangeObjectList::append(sdl::imtbase::ImtBaseTypes::CTimeRangeObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList* sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList::addElement(sdl::imtbase::ImtBaseTypes::CTimeRangeObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CTimeRangeObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CTimeRangeObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CTimeRangeObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_begin"){
		return QVariant::fromValue(Version_1_0->at(index)->Begin.value());
	}
	if (nameId == "m_end"){
		return QVariant::fromValue(Version_1_0->at(index)->End.value());
	}

	return QVariant();
}
CSdlSizeObject::CSdlSizeObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CSdlSizeObject::widthChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSdlSizeObject::heightChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSdlSizeObject::GetWidth()
{
	if (Version_1_0 && Version_1_0->width){
		return Version_1_0->width.value();
	}

	return QVariant();
}


void CSdlSizeObject::SetWidth(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->width = v.value<double>();
	widthChanged();
}


bool CSdlSizeObject::hasWidth()
{
	 return Version_1_0 && Version_1_0->width.HasValue();
}


QVariant CSdlSizeObject::GetHeight()
{
	if (Version_1_0 && Version_1_0->height){
		return Version_1_0->height.value();
	}

	return QVariant();
}


void CSdlSizeObject::SetHeight(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->height = v.value<double>();
	heightChanged();
}


bool CSdlSizeObject::hasHeight()
{
	 return Version_1_0 && Version_1_0->height.HasValue();
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


void CSdlSizeObjectList::append(sdl::imtbase::ImtBaseTypes::CSdlSizeObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList* sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList::addElement(sdl::imtbase::ImtBaseTypes::CSdlSizeObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CSdlSizeObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CSdlSizeObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CSdlSizeObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_width"){
		return QVariant::fromValue(Version_1_0->at(index)->width.value());
	}
	if (nameId == "m_height"){
		return QVariant::fromValue(Version_1_0->at(index)->height.value());
	}

	return QVariant();
}
CSdlPointObject::CSdlPointObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CSdlPointObject::xChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSdlPointObject::yChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSdlPointObject::GetX()
{
	if (Version_1_0 && Version_1_0->x){
		return Version_1_0->x.value();
	}

	return QVariant();
}


void CSdlPointObject::SetX(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->x = v.value<double>();
	xChanged();
}


bool CSdlPointObject::hasX()
{
	 return Version_1_0 && Version_1_0->x.HasValue();
}


QVariant CSdlPointObject::GetY()
{
	if (Version_1_0 && Version_1_0->y){
		return Version_1_0->y.value();
	}

	return QVariant();
}


void CSdlPointObject::SetY(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->y = v.value<double>();
	yChanged();
}


bool CSdlPointObject::hasY()
{
	 return Version_1_0 && Version_1_0->y.HasValue();
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


void CSdlPointObjectList::append(sdl::imtbase::ImtBaseTypes::CSdlPointObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CSdlPointObjectList* sdl::imtbase::ImtBaseTypes::CSdlPointObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CSdlPointObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CSdlPointObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CSdlPointObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CSdlPointObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CSdlPointObjectList::addElement(sdl::imtbase::ImtBaseTypes::CSdlPointObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CSdlPointObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CSdlPointObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CSdlPointObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CSdlPointObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CSdlPointObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CSdlPointObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CSdlPointObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CSdlPointObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CSdlPointObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_x"){
		return QVariant::fromValue(Version_1_0->at(index)->x.value());
	}
	if (nameId == "m_y"){
		return QVariant::fromValue(Version_1_0->at(index)->y.value());
	}

	return QVariant();
}
CParamTypeIdsObject::CParamTypeIdsObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

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
	if (Version_1_0 && Version_1_0->UrlParam){
		return Version_1_0->UrlParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetUrlParam(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->UrlParam = v.value<QString>().toUtf8();
	urlParamChanged();
}


bool CParamTypeIdsObject::hasUrlParam()
{
	 return Version_1_0 && Version_1_0->UrlParam.HasValue();
}


QVariant CParamTypeIdsObject::GetIdParam()
{
	if (Version_1_0 && Version_1_0->IdParam){
		return Version_1_0->IdParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetIdParam(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->IdParam = v.value<QString>().toUtf8();
	idParamChanged();
}


bool CParamTypeIdsObject::hasIdParam()
{
	 return Version_1_0 && Version_1_0->IdParam.HasValue();
}


QVariant CParamTypeIdsObject::GetTextParam()
{
	if (Version_1_0 && Version_1_0->TextParam){
		return Version_1_0->TextParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetTextParam(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->TextParam = v.value<QString>().toUtf8();
	textParamChanged();
}


bool CParamTypeIdsObject::hasTextParam()
{
	 return Version_1_0 && Version_1_0->TextParam.HasValue();
}


QVariant CParamTypeIdsObject::GetTextView()
{
	if (Version_1_0 && Version_1_0->TextView){
		return Version_1_0->TextView.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetTextView(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->TextView = v.value<QString>().toUtf8();
	textViewChanged();
}


bool CParamTypeIdsObject::hasTextView()
{
	 return Version_1_0 && Version_1_0->TextView.HasValue();
}


QVariant CParamTypeIdsObject::GetSelectionParam()
{
	if (Version_1_0 && Version_1_0->SelectionParam){
		return Version_1_0->SelectionParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetSelectionParam(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->SelectionParam = v.value<QString>().toUtf8();
	selectionParamChanged();
}


bool CParamTypeIdsObject::hasSelectionParam()
{
	 return Version_1_0 && Version_1_0->SelectionParam.HasValue();
}


QVariant CParamTypeIdsObject::GetSchedulerParam()
{
	if (Version_1_0 && Version_1_0->SchedulerParam){
		return Version_1_0->SchedulerParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetSchedulerParam(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->SchedulerParam = v.value<QString>().toUtf8();
	schedulerParamChanged();
}


bool CParamTypeIdsObject::hasSchedulerParam()
{
	 return Version_1_0 && Version_1_0->SchedulerParam.HasValue();
}


QVariant CParamTypeIdsObject::GetBackupSettings()
{
	if (Version_1_0 && Version_1_0->BackupSettings){
		return Version_1_0->BackupSettings.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetBackupSettings(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->BackupSettings = v.value<QString>().toUtf8();
	backupSettingsChanged();
}


bool CParamTypeIdsObject::hasBackupSettings()
{
	 return Version_1_0 && Version_1_0->BackupSettings.HasValue();
}


QVariant CParamTypeIdsObject::GetDatabaseAccessSettings()
{
	if (Version_1_0 && Version_1_0->DatabaseAccessSettings){
		return Version_1_0->DatabaseAccessSettings.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetDatabaseAccessSettings(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->DatabaseAccessSettings = v.value<QString>().toUtf8();
	databaseAccessSettingsChanged();
}


bool CParamTypeIdsObject::hasDatabaseAccessSettings()
{
	 return Version_1_0 && Version_1_0->DatabaseAccessSettings.HasValue();
}


QVariant CParamTypeIdsObject::GetParamsSet()
{
	if (Version_1_0 && Version_1_0->ParamsSet){
		return Version_1_0->ParamsSet.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetParamsSet(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->ParamsSet = v.value<QString>().toUtf8();
	paramsSetChanged();
}


bool CParamTypeIdsObject::hasParamsSet()
{
	 return Version_1_0 && Version_1_0->ParamsSet.HasValue();
}


QVariant CParamTypeIdsObject::GetFileNameParam()
{
	if (Version_1_0 && Version_1_0->FileNameParam){
		return Version_1_0->FileNameParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetFileNameParam(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->FileNameParam = v.value<QString>().toUtf8();
	fileNameParamChanged();
}


bool CParamTypeIdsObject::hasFileNameParam()
{
	 return Version_1_0 && Version_1_0->FileNameParam.HasValue();
}


QVariant CParamTypeIdsObject::GetIntegerParam()
{
	if (Version_1_0 && Version_1_0->IntegerParam){
		return Version_1_0->IntegerParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetIntegerParam(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->IntegerParam = v.value<QString>().toUtf8();
	integerParamChanged();
}


bool CParamTypeIdsObject::hasIntegerParam()
{
	 return Version_1_0 && Version_1_0->IntegerParam.HasValue();
}


QVariant CParamTypeIdsObject::GetDoubleParam()
{
	if (Version_1_0 && Version_1_0->DoubleParam){
		return Version_1_0->DoubleParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetDoubleParam(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->DoubleParam = v.value<QString>().toUtf8();
	doubleParamChanged();
}


bool CParamTypeIdsObject::hasDoubleParam()
{
	 return Version_1_0 && Version_1_0->DoubleParam.HasValue();
}


QVariant CParamTypeIdsObject::GetPasswordParam()
{
	if (Version_1_0 && Version_1_0->PasswordParam){
		return Version_1_0->PasswordParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetPasswordParam(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->PasswordParam = v.value<QString>().toUtf8();
	passwordParamChanged();
}


bool CParamTypeIdsObject::hasPasswordParam()
{
	 return Version_1_0 && Version_1_0->PasswordParam.HasValue();
}


QVariant CParamTypeIdsObject::GetEnableableParam()
{
	if (Version_1_0 && Version_1_0->EnableableParam){
		return Version_1_0->EnableableParam.value();
	}

	return QVariant();
}


void CParamTypeIdsObject::SetEnableableParam(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->EnableableParam = v.value<QString>().toUtf8();
	enableableParamChanged();
}


bool CParamTypeIdsObject::hasEnableableParam()
{
	 return Version_1_0 && Version_1_0->EnableableParam.HasValue();
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


void CParamTypeIdsObjectList::append(sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList* sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList::addElement(sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CParamTypeIdsObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CParamTypeIdsObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_urlParam"){
		return QVariant::fromValue(Version_1_0->at(index)->UrlParam.value());
	}
	if (nameId == "m_idParam"){
		return QVariant::fromValue(Version_1_0->at(index)->IdParam.value());
	}
	if (nameId == "m_textParam"){
		return QVariant::fromValue(Version_1_0->at(index)->TextParam.value());
	}
	if (nameId == "m_textView"){
		return QVariant::fromValue(Version_1_0->at(index)->TextView.value());
	}
	if (nameId == "m_selectionParam"){
		return QVariant::fromValue(Version_1_0->at(index)->SelectionParam.value());
	}
	if (nameId == "m_schedulerParam"){
		return QVariant::fromValue(Version_1_0->at(index)->SchedulerParam.value());
	}
	if (nameId == "m_backupSettings"){
		return QVariant::fromValue(Version_1_0->at(index)->BackupSettings.value());
	}
	if (nameId == "m_databaseAccessSettings"){
		return QVariant::fromValue(Version_1_0->at(index)->DatabaseAccessSettings.value());
	}
	if (nameId == "m_paramsSet"){
		return QVariant::fromValue(Version_1_0->at(index)->ParamsSet.value());
	}
	if (nameId == "m_fileNameParam"){
		return QVariant::fromValue(Version_1_0->at(index)->FileNameParam.value());
	}
	if (nameId == "m_integerParam"){
		return QVariant::fromValue(Version_1_0->at(index)->IntegerParam.value());
	}
	if (nameId == "m_doubleParam"){
		return QVariant::fromValue(Version_1_0->at(index)->DoubleParam.value());
	}
	if (nameId == "m_passwordParam"){
		return QVariant::fromValue(Version_1_0->at(index)->PasswordParam.value());
	}
	if (nameId == "m_enableableParam"){
		return QVariant::fromValue(Version_1_0->at(index)->EnableableParam.value());
	}

	return QVariant();
}
CUrlParamObject::CUrlParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CUrlParamObject::schemeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CUrlParamObject::hostChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CUrlParamObject::portChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CUrlParamObject::pathChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CUrlParamObject::GetScheme()
{
	if (Version_1_0 && Version_1_0->scheme){
		return Version_1_0->scheme.value();
	}

	return QVariant();
}


void CUrlParamObject::SetScheme(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->scheme = v.value<QString>();
	schemeChanged();
}


bool CUrlParamObject::hasScheme()
{
	 return Version_1_0 && Version_1_0->scheme.HasValue();
}


QVariant CUrlParamObject::GetHost()
{
	if (Version_1_0 && Version_1_0->host){
		return Version_1_0->host.value();
	}

	return QVariant();
}


void CUrlParamObject::SetHost(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->host = v.value<QString>();
	hostChanged();
}


bool CUrlParamObject::hasHost()
{
	 return Version_1_0 && Version_1_0->host.HasValue();
}


QVariant CUrlParamObject::GetPort()
{
	if (Version_1_0 && Version_1_0->port){
		return Version_1_0->port.value();
	}

	return QVariant();
}


void CUrlParamObject::SetPort(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->port = v.value<int>();
	portChanged();
}


bool CUrlParamObject::hasPort()
{
	 return Version_1_0 && Version_1_0->port.HasValue();
}


QVariant CUrlParamObject::GetPath()
{
	if (Version_1_0 && Version_1_0->path){
		return Version_1_0->path.value();
	}

	return QVariant();
}


void CUrlParamObject::SetPath(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->path = v.value<QString>();
	pathChanged();
}


bool CUrlParamObject::hasPath()
{
	 return Version_1_0 && Version_1_0->path.HasValue();
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


void CUrlParamObjectList::append(sdl::imtbase::ImtBaseTypes::CUrlParamObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CUrlParamObjectList* sdl::imtbase::ImtBaseTypes::CUrlParamObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CUrlParamObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CUrlParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CUrlParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CUrlParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CUrlParamObjectList::addElement(sdl::imtbase::ImtBaseTypes::CUrlParamObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CUrlParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CUrlParamObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CUrlParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CUrlParamObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CUrlParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CUrlParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CUrlParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CUrlParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CUrlParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_scheme"){
		return QVariant::fromValue(Version_1_0->at(index)->scheme.value());
	}
	if (nameId == "m_host"){
		return QVariant::fromValue(Version_1_0->at(index)->host.value());
	}
	if (nameId == "m_port"){
		return QVariant::fromValue(Version_1_0->at(index)->port.value());
	}
	if (nameId == "m_path"){
		return QVariant::fromValue(Version_1_0->at(index)->path.value());
	}

	return QVariant();
}
CIdParamObject::CIdParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CIdParamObject::idChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CIdParamObject::GetId()
{
	if (Version_1_0 && Version_1_0->id){
		return Version_1_0->id.value();
	}

	return QVariant();
}


void CIdParamObject::SetId(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->id = v.value<QString>().toUtf8();
	idChanged();
}


bool CIdParamObject::hasId()
{
	 return Version_1_0 && Version_1_0->id.HasValue();
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


void CIdParamObjectList::append(sdl::imtbase::ImtBaseTypes::CIdParamObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CIdParamObjectList* sdl::imtbase::ImtBaseTypes::CIdParamObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CIdParamObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CIdParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CIdParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CIdParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CIdParamObjectList::addElement(sdl::imtbase::ImtBaseTypes::CIdParamObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CIdParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CIdParamObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CIdParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CIdParamObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CIdParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CIdParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CIdParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CIdParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CIdParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_id"){
		return QVariant::fromValue(Version_1_0->at(index)->id.value());
	}

	return QVariant();
}
CTextParamObject::CTextParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CTextParamObject::textChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CTextParamObject::GetText()
{
	if (Version_1_0 && Version_1_0->text){
		return Version_1_0->text.value();
	}

	return QVariant();
}


void CTextParamObject::SetText(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->text = v.value<QString>();
	textChanged();
}


bool CTextParamObject::hasText()
{
	 return Version_1_0 && Version_1_0->text.HasValue();
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


void CTextParamObjectList::append(sdl::imtbase::ImtBaseTypes::CTextParamObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CTextParamObjectList* sdl::imtbase::ImtBaseTypes::CTextParamObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CTextParamObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CTextParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CTextParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CTextParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CTextParamObjectList::addElement(sdl::imtbase::ImtBaseTypes::CTextParamObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CTextParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CTextParamObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CTextParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CTextParamObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CTextParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CTextParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CTextParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CTextParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CTextParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_text"){
		return QVariant::fromValue(Version_1_0->at(index)->text.value());
	}

	return QVariant();
}
CEnableableParamObject::CEnableableParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CEnableableParamObject::valueChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CEnableableParamObject::GetValue()
{
	if (Version_1_0 && Version_1_0->value){
		return Version_1_0->value.value();
	}

	return QVariant();
}


void CEnableableParamObject::SetValue(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->value = v.value<bool>();
	valueChanged();
}


bool CEnableableParamObject::hasValue()
{
	 return Version_1_0 && Version_1_0->value.HasValue();
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


void CEnableableParamObjectList::append(sdl::imtbase::ImtBaseTypes::CEnableableParamObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList* sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList::addElement(sdl::imtbase::ImtBaseTypes::CEnableableParamObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CEnableableParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CEnableableParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CEnableableParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_value"){
		return QVariant::fromValue(Version_1_0->at(index)->value.value());
	}

	return QVariant();
}
CIntegerParamObject::CIntegerParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CIntegerParamObject::valueChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CIntegerParamObject::GetValue()
{
	if (Version_1_0 && Version_1_0->value){
		return Version_1_0->value.value();
	}

	return QVariant();
}


void CIntegerParamObject::SetValue(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->value = v.value<int>();
	valueChanged();
}


bool CIntegerParamObject::hasValue()
{
	 return Version_1_0 && Version_1_0->value.HasValue();
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


void CIntegerParamObjectList::append(sdl::imtbase::ImtBaseTypes::CIntegerParamObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList* sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList::addElement(sdl::imtbase::ImtBaseTypes::CIntegerParamObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CIntegerParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CIntegerParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CIntegerParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_value"){
		return QVariant::fromValue(Version_1_0->at(index)->value.value());
	}

	return QVariant();
}
CDoubleParamObject::CDoubleParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CDoubleParamObject::valueChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CDoubleParamObject::GetValue()
{
	if (Version_1_0 && Version_1_0->value){
		return Version_1_0->value.value();
	}

	return QVariant();
}


void CDoubleParamObject::SetValue(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->value = v.value<double>();
	valueChanged();
}


bool CDoubleParamObject::hasValue()
{
	 return Version_1_0 && Version_1_0->value.HasValue();
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


void CDoubleParamObjectList::append(sdl::imtbase::ImtBaseTypes::CDoubleParamObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList* sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList::addElement(sdl::imtbase::ImtBaseTypes::CDoubleParamObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CDoubleParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CDoubleParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CDoubleParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_value"){
		return QVariant::fromValue(Version_1_0->at(index)->value.value());
	}

	return QVariant();
}
COptionObject::COptionObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &COptionObject::idChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COptionObject::nameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COptionObject::descriptionChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COptionObject::enabledChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant COptionObject::GetId()
{
	if (Version_1_0 && Version_1_0->id){
		return Version_1_0->id.value();
	}

	return QVariant();
}


void COptionObject::SetId(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->id = v.value<QString>().toUtf8();
	idChanged();
}


bool COptionObject::hasId()
{
	 return Version_1_0 && Version_1_0->id.HasValue();
}


QVariant COptionObject::GetName()
{
	if (Version_1_0 && Version_1_0->name){
		return Version_1_0->name.value();
	}

	return QVariant();
}


void COptionObject::SetName(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->name = v.value<QString>();
	nameChanged();
}


bool COptionObject::hasName()
{
	 return Version_1_0 && Version_1_0->name.HasValue();
}


QVariant COptionObject::GetDescription()
{
	if (Version_1_0 && Version_1_0->description){
		return Version_1_0->description.value();
	}

	return QVariant();
}


void COptionObject::SetDescription(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->description = v.value<QString>();
	descriptionChanged();
}


bool COptionObject::hasDescription()
{
	 return Version_1_0 && Version_1_0->description.HasValue();
}


QVariant COptionObject::GetEnabled()
{
	if (Version_1_0 && Version_1_0->enabled){
		return Version_1_0->enabled.value();
	}

	return QVariant();
}


void COptionObject::SetEnabled(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->enabled = v.value<bool>();
	enabledChanged();
}


bool COptionObject::hasEnabled()
{
	 return Version_1_0 && Version_1_0->enabled.HasValue();
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


void COptionObjectList::append(sdl::imtbase::ImtBaseTypes::COptionObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::COptionObjectList* sdl::imtbase::ImtBaseTypes::COptionObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::COptionObjectList* retVal = new sdl::imtbase::ImtBaseTypes::COptionObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::COptionObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::COptionObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::COptionObjectList::addElement(sdl::imtbase::ImtBaseTypes::COptionObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::COptionObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::COptionObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::COptionObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::COptionObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::COptionObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::COptionObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::COptionObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::COptionObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::COptionObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_id"){
		return QVariant::fromValue(Version_1_0->at(index)->id.value());
	}
	if (nameId == "m_name"){
		return QVariant::fromValue(Version_1_0->at(index)->name.value());
	}
	if (nameId == "m_description"){
		return QVariant::fromValue(Version_1_0->at(index)->description.value());
	}
	if (nameId == "m_enabled"){
		return QVariant::fromValue(Version_1_0->at(index)->enabled.value());
	}

	return QVariant();
}
COptionsListObject::COptionsListObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &COptionsListObject::totalCountChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COptionsListObject::offsetChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &COptionsListObject::optionsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant COptionsListObject::GetTotalCount()
{
	if (Version_1_0 && Version_1_0->totalCount){
		return Version_1_0->totalCount.value();
	}

	return QVariant();
}


void COptionsListObject::SetTotalCount(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->totalCount = v.value<int>();
	totalCountChanged();
}


bool COptionsListObject::hasTotalCount()
{
	 return Version_1_0 && Version_1_0->totalCount.HasValue();
}


QVariant COptionsListObject::GetOffset()
{
	if (Version_1_0 && Version_1_0->offset){
		return Version_1_0->offset.value();
	}

	return QVariant();
}


void COptionsListObject::SetOffset(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->offset = v.value<int>();
	offsetChanged();
}


bool COptionsListObject::hasOffset()
{
	 return Version_1_0 && Version_1_0->offset.HasValue();
}


QVariant COptionsListObject::GetOptions()
{
	if (Version_1_0 && Version_1_0->options){
		if (!m_optionsQObjectPtr.isValid()){
			m_optionsQObjectPtr = CreateObject("options");
			auto itemPtr = m_optionsQObjectPtr.value<sdl::imtbase::ImtBaseTypes::COptionObjectList*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->options;
		}
		return m_optionsQObjectPtr;
	}

	return QVariant();
}


void COptionsListObject::SetOptions(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	if (v.isValid()){
		sdl::imtbase::ImtBaseTypes::COptionObjectList* itemPtr = v.value<sdl::imtbase::ImtBaseTypes::COptionObjectList*>();
		if (itemPtr != nullptr)  Version_1_0->options = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->options = nullptr;
	}
	m_optionsQObjectPtr = v;

	optionsChanged();
}


bool COptionsListObject::hasOptions()
{
	 return Version_1_0 && Version_1_0->options.HasValue();
}


void COptionsListObject::emplaceOptions()
{
	ResetOptions();
	if(!Version_1_0){
		Version_1_0.emplace();
	}
	Version_1_0->options.emplace();
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
	return QVariant::fromValue(new sdl::imtbase::ImtBaseTypes::COptionObject());
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
		return QVariant::fromValue(new sdl::imtbase::ImtBaseTypes::COptionObjectList(this));
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


void COptionsListObjectList::append(sdl::imtbase::ImtBaseTypes::COptionsListObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::COptionsListObjectList* sdl::imtbase::ImtBaseTypes::COptionsListObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::COptionsListObjectList* retVal = new sdl::imtbase::ImtBaseTypes::COptionsListObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::COptionsListObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::COptionsListObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::COptionsListObjectList::addElement(sdl::imtbase::ImtBaseTypes::COptionsListObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::COptionsListObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::COptionsListObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::COptionsListObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::COptionsListObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::COptionsListObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::COptionsListObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::COptionsListObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::COptionsListObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::COptionsListObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_totalCount"){
		return QVariant::fromValue(Version_1_0->at(index)->totalCount.value());
	}
	if (nameId == "m_offset"){
		return QVariant::fromValue(Version_1_0->at(index)->offset.value());
	}
	if (nameId == "m_options"){
		return itemPtr->GetOptions();
	}

	return QVariant();
}
CSelectionParamObject::CSelectionParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CSelectionParamObject::selectedIndexChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSelectionParamObject::constraintsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSelectionParamObject::GetSelectedIndex()
{
	if (Version_1_0 && Version_1_0->selectedIndex){
		return Version_1_0->selectedIndex.value();
	}

	return QVariant();
}


void CSelectionParamObject::SetSelectedIndex(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->selectedIndex = v.value<int>();
	selectedIndexChanged();
}


bool CSelectionParamObject::hasSelectedIndex()
{
	 return Version_1_0 && Version_1_0->selectedIndex.HasValue();
}


QVariant CSelectionParamObject::GetConstraints()
{
	if (Version_1_0 && Version_1_0->constraints){
		if (!m_constraintsQObjectPtr.isValid()){
			m_constraintsQObjectPtr = CreateObject("constraints");
			auto itemPtr = m_constraintsQObjectPtr.value<sdl::imtbase::ImtBaseTypes::COptionsListObject*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->constraints;
		}
		return m_constraintsQObjectPtr;
	}

	return QVariant();
}


void CSelectionParamObject::SetConstraints(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	if (v.isValid()){
		sdl::imtbase::ImtBaseTypes::COptionsListObject* itemPtr = v.value<sdl::imtbase::ImtBaseTypes::COptionsListObject*>();
		if (itemPtr != nullptr)  Version_1_0->constraints = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->constraints = nullptr;
	}
	m_constraintsQObjectPtr = v;

	constraintsChanged();
}


bool CSelectionParamObject::hasConstraints()
{
	 return Version_1_0 && Version_1_0->constraints.HasValue();
}


void CSelectionParamObject::emplaceConstraints()
{
	ResetConstraints();
	if(!Version_1_0){
		Version_1_0.emplace();
	}
	Version_1_0->constraints.emplace();
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
		return QVariant::fromValue(new sdl::imtbase::ImtBaseTypes::COptionsListObject(this));
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


void CSelectionParamObjectList::append(sdl::imtbase::ImtBaseTypes::CSelectionParamObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList* sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList::addElement(sdl::imtbase::ImtBaseTypes::CSelectionParamObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CSelectionParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CSelectionParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CSelectionParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_selectedIndex"){
		return QVariant::fromValue(Version_1_0->at(index)->selectedIndex.value());
	}
	if (nameId == "m_constraints"){
		return itemPtr->GetConstraints();
	}

	return QVariant();
}
CSchedulerParamObject::CSchedulerParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CSchedulerParamObject::startTimeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CSchedulerParamObject::intervalChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CSchedulerParamObject::GetStartTime()
{
	if (Version_1_0 && Version_1_0->startTime){
		return Version_1_0->startTime.value();
	}

	return QVariant();
}


void CSchedulerParamObject::SetStartTime(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->startTime = v.value<QString>();
	startTimeChanged();
}


bool CSchedulerParamObject::hasStartTime()
{
	 return Version_1_0 && Version_1_0->startTime.HasValue();
}


QVariant CSchedulerParamObject::GetInterval()
{
	if (Version_1_0 && Version_1_0->interval){
		return Version_1_0->interval.value();
	}

	return QVariant();
}


void CSchedulerParamObject::SetInterval(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->interval = v.value<int>();
	intervalChanged();
}


bool CSchedulerParamObject::hasInterval()
{
	 return Version_1_0 && Version_1_0->interval.HasValue();
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


void CSchedulerParamObjectList::append(sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList* sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList::addElement(sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CSchedulerParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CSchedulerParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_startTime"){
		return QVariant::fromValue(Version_1_0->at(index)->startTime.value());
	}
	if (nameId == "m_interval"){
		return QVariant::fromValue(Version_1_0->at(index)->interval.value());
	}

	return QVariant();
}
CBackupSettingsObject::CBackupSettingsObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CBackupSettingsObject::schedulerParamChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CBackupSettingsObject::folderPathChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CBackupSettingsObject::GetSchedulerParam()
{
	if (Version_1_0 && Version_1_0->schedulerParam){
		if (!m_schedulerParamQObjectPtr.isValid()){
			m_schedulerParamQObjectPtr = CreateObject("schedulerParam");
			auto itemPtr = m_schedulerParamQObjectPtr.value<sdl::imtbase::ImtBaseTypes::CSchedulerParamObject*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->schedulerParam;
		}
		return m_schedulerParamQObjectPtr;
	}

	return QVariant();
}


void CBackupSettingsObject::SetSchedulerParam(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	if (v.isValid()){
		sdl::imtbase::ImtBaseTypes::CSchedulerParamObject* itemPtr = v.value<sdl::imtbase::ImtBaseTypes::CSchedulerParamObject*>();
		if (itemPtr != nullptr)  Version_1_0->schedulerParam = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->schedulerParam = nullptr;
	}
	m_schedulerParamQObjectPtr = v;

	schedulerParamChanged();
}


bool CBackupSettingsObject::hasSchedulerParam()
{
	 return Version_1_0 && Version_1_0->schedulerParam.HasValue();
}


void CBackupSettingsObject::emplaceSchedulerParam()
{
	ResetSchedulerParam();
	if(!Version_1_0){
		Version_1_0.emplace();
	}
	Version_1_0->schedulerParam.emplace();
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
	if (Version_1_0 && Version_1_0->folderPath){
		return Version_1_0->folderPath.value();
	}

	return QVariant();
}


void CBackupSettingsObject::SetFolderPath(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->folderPath = v.value<QString>();
	folderPathChanged();
}


bool CBackupSettingsObject::hasFolderPath()
{
	 return Version_1_0 && Version_1_0->folderPath.HasValue();
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
		return QVariant::fromValue(new sdl::imtbase::ImtBaseTypes::CSchedulerParamObject(this));
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


void CBackupSettingsObjectList::append(sdl::imtbase::ImtBaseTypes::CBackupSettingsObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList* sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList::addElement(sdl::imtbase::ImtBaseTypes::CBackupSettingsObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CBackupSettingsObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CBackupSettingsObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CBackupSettingsObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_schedulerParam"){
		return itemPtr->GetSchedulerParam();
	}
	if (nameId == "m_folderPath"){
		return QVariant::fromValue(Version_1_0->at(index)->folderPath.value());
	}

	return QVariant();
}
CDatabaseAccessSettingsObject::CDatabaseAccessSettingsObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CDatabaseAccessSettingsObject::dbNameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDatabaseAccessSettingsObject::hostChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDatabaseAccessSettingsObject::portChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDatabaseAccessSettingsObject::dbPathChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDatabaseAccessSettingsObject::usernameChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDatabaseAccessSettingsObject::passwordChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CDatabaseAccessSettingsObject::GetDbName()
{
	if (Version_1_0 && Version_1_0->dbName){
		return Version_1_0->dbName.value();
	}

	return QVariant();
}


void CDatabaseAccessSettingsObject::SetDbName(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->dbName = v.value<QString>();
	dbNameChanged();
}


bool CDatabaseAccessSettingsObject::hasDbName()
{
	 return Version_1_0 && Version_1_0->dbName.HasValue();
}


QVariant CDatabaseAccessSettingsObject::GetHost()
{
	if (Version_1_0 && Version_1_0->host){
		return Version_1_0->host.value();
	}

	return QVariant();
}


void CDatabaseAccessSettingsObject::SetHost(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->host = v.value<QString>();
	hostChanged();
}


bool CDatabaseAccessSettingsObject::hasHost()
{
	 return Version_1_0 && Version_1_0->host.HasValue();
}


QVariant CDatabaseAccessSettingsObject::GetPort()
{
	if (Version_1_0 && Version_1_0->port){
		return Version_1_0->port.value();
	}

	return QVariant();
}


void CDatabaseAccessSettingsObject::SetPort(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->port = v.value<int>();
	portChanged();
}


bool CDatabaseAccessSettingsObject::hasPort()
{
	 return Version_1_0 && Version_1_0->port.HasValue();
}


QVariant CDatabaseAccessSettingsObject::GetDbPath()
{
	if (Version_1_0 && Version_1_0->dbPath){
		return Version_1_0->dbPath.value();
	}

	return QVariant();
}


void CDatabaseAccessSettingsObject::SetDbPath(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->dbPath = v.value<QString>();
	dbPathChanged();
}


bool CDatabaseAccessSettingsObject::hasDbPath()
{
	 return Version_1_0 && Version_1_0->dbPath.HasValue();
}


QVariant CDatabaseAccessSettingsObject::GetUsername()
{
	if (Version_1_0 && Version_1_0->username){
		return Version_1_0->username.value();
	}

	return QVariant();
}


void CDatabaseAccessSettingsObject::SetUsername(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->username = v.value<QString>();
	usernameChanged();
}


bool CDatabaseAccessSettingsObject::hasUsername()
{
	 return Version_1_0 && Version_1_0->username.HasValue();
}


QVariant CDatabaseAccessSettingsObject::GetPassword()
{
	if (Version_1_0 && Version_1_0->password){
		return Version_1_0->password.value();
	}

	return QVariant();
}


void CDatabaseAccessSettingsObject::SetPassword(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->password = v.value<QString>();
	passwordChanged();
}


bool CDatabaseAccessSettingsObject::hasPassword()
{
	 return Version_1_0 && Version_1_0->password.HasValue();
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


void CDatabaseAccessSettingsObjectList::append(sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList* sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList::addElement(sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettingsObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_dbName"){
		return QVariant::fromValue(Version_1_0->at(index)->dbName.value());
	}
	if (nameId == "m_host"){
		return QVariant::fromValue(Version_1_0->at(index)->host.value());
	}
	if (nameId == "m_port"){
		return QVariant::fromValue(Version_1_0->at(index)->port.value());
	}
	if (nameId == "m_dbPath"){
		return QVariant::fromValue(Version_1_0->at(index)->dbPath.value());
	}
	if (nameId == "m_username"){
		return QVariant::fromValue(Version_1_0->at(index)->username.value());
	}
	if (nameId == "m_password"){
		return QVariant::fromValue(Version_1_0->at(index)->password.value());
	}

	return QVariant();
}
CFileNameParamObject::CFileNameParamObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CFileNameParamObject::pathTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CFileNameParamObject::pathChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CFileNameParamObject::GetPathType()
{
	if (Version_1_0 && Version_1_0->pathType){
		return Version_1_0->pathType.value();
	}

	return QVariant();
}


void CFileNameParamObject::SetPathType(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->pathType = v.value<int>();
	pathTypeChanged();
}


bool CFileNameParamObject::hasPathType()
{
	 return Version_1_0 && Version_1_0->pathType.HasValue();
}


QVariant CFileNameParamObject::GetPath()
{
	if (Version_1_0 && Version_1_0->path){
		return Version_1_0->path.value();
	}

	return QVariant();
}


void CFileNameParamObject::SetPath(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->path = v.value<QString>();
	pathChanged();
}


bool CFileNameParamObject::hasPath()
{
	 return Version_1_0 && Version_1_0->path.HasValue();
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


void CFileNameParamObjectList::append(sdl::imtbase::ImtBaseTypes::CFileNameParamObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList* sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList::addElement(sdl::imtbase::ImtBaseTypes::CFileNameParamObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CFileNameParamObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CFileNameParamObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CFileNameParamObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_pathType"){
		return QVariant::fromValue(Version_1_0->at(index)->pathType.value());
	}
	if (nameId == "m_path"){
		return QVariant::fromValue(Version_1_0->at(index)->path.value());
	}

	return QVariant();
}
CParamsSetObject::CParamsSetObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CParamsSetObject::paramIdsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamsSetObject::paramTypeIdsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamsSetObject::paramNamesChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamsSetObject::paramDescriptionsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CParamsSetObject::parametersChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CParamsSetObject::GetParamIds()
{
	if (Version_1_0 && Version_1_0->paramIds){
		QList<QString> tempParamIdsList;
		for (const auto& tempValue: Version_1_0->paramIds.value()){
			tempParamIdsList << *tempValue;
		}
		return QVariant::fromValue(tempParamIdsList);
	}

	return QVariant();
}


void CParamsSetObject::SetParamIds(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	if (!Version_1_0->paramIds){
		Version_1_0->paramIds.emplace();
	}
	else{
		Version_1_0->paramIds->clear();
	}
	for (const auto& tempValue: v.value<QList<QString>>()){
		istd::TSharedNullable<QByteArray> value(tempValue.toUtf8());
		Version_1_0->paramIds->append(value);
	}

	paramIdsChanged();
}


bool CParamsSetObject::hasParamIds()
{
	 return Version_1_0 && Version_1_0->paramIds.HasValue();
}


QVariant CParamsSetObject::GetParamTypeIds()
{
	if (Version_1_0 && Version_1_0->paramTypeIds){
		QList<QString> tempParamTypeIdsList;
		for (const auto& tempValue: Version_1_0->paramTypeIds.value()){
			tempParamTypeIdsList << *tempValue;
		}
		return QVariant::fromValue(tempParamTypeIdsList);
	}

	return QVariant();
}


void CParamsSetObject::SetParamTypeIds(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	if (!Version_1_0->paramTypeIds){
		Version_1_0->paramTypeIds.emplace();
	}
	else{
		Version_1_0->paramTypeIds->clear();
	}
	for (const auto& tempValue: v.value<QList<QString>>()){
		istd::TSharedNullable<QByteArray> value(tempValue.toUtf8());
		Version_1_0->paramTypeIds->append(value);
	}

	paramTypeIdsChanged();
}


bool CParamsSetObject::hasParamTypeIds()
{
	 return Version_1_0 && Version_1_0->paramTypeIds.HasValue();
}


QVariant CParamsSetObject::GetParamNames()
{
	if (Version_1_0 && Version_1_0->paramNames){
		QList<QString> tempParamNamesList;
		for (const auto& tempValue: Version_1_0->paramNames.value()){
			tempParamNamesList << *tempValue;
		}
		return QVariant::fromValue(tempParamNamesList);
	}

	return QVariant();
}


void CParamsSetObject::SetParamNames(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	if (!Version_1_0->paramNames){
		Version_1_0->paramNames.emplace();
	}
	else{
		Version_1_0->paramNames->clear();
	}
	for (const auto& tempValue: v.value<QList<QString>>()){
		istd::TSharedNullable<QString> value(tempValue);
		Version_1_0->paramNames->append(value);
	}

	paramNamesChanged();
}


bool CParamsSetObject::hasParamNames()
{
	 return Version_1_0 && Version_1_0->paramNames.HasValue();
}


QVariant CParamsSetObject::GetParamDescriptions()
{
	if (Version_1_0 && Version_1_0->paramDescriptions){
		QList<QString> tempParamDescriptionsList;
		for (const auto& tempValue: Version_1_0->paramDescriptions.value()){
			tempParamDescriptionsList << *tempValue;
		}
		return QVariant::fromValue(tempParamDescriptionsList);
	}

	return QVariant();
}


void CParamsSetObject::SetParamDescriptions(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	if (!Version_1_0->paramDescriptions){
		Version_1_0->paramDescriptions.emplace();
	}
	else{
		Version_1_0->paramDescriptions->clear();
	}
	for (const auto& tempValue: v.value<QList<QString>>()){
		istd::TSharedNullable<QString> value(tempValue);
		Version_1_0->paramDescriptions->append(value);
	}

	paramDescriptionsChanged();
}


bool CParamsSetObject::hasParamDescriptions()
{
	 return Version_1_0 && Version_1_0->paramDescriptions.HasValue();
}


QVariant CParamsSetObject::GetParameters()
{
	if (Version_1_0 && Version_1_0->parameters){
		QList<QString> tempParametersList;
		for (const auto& tempValue: Version_1_0->parameters.value()){
			tempParametersList << *tempValue;
		}
		return QVariant::fromValue(tempParametersList);
	}

	return QVariant();
}


void CParamsSetObject::SetParameters(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	if (!Version_1_0->parameters){
		Version_1_0->parameters.emplace();
	}
	else{
		Version_1_0->parameters->clear();
	}
	for (const auto& tempValue: v.value<QList<QString>>()){
		istd::TSharedNullable<QString> value(tempValue);
		Version_1_0->parameters->append(value);
	}

	parametersChanged();
}


bool CParamsSetObject::hasParameters()
{
	 return Version_1_0 && Version_1_0->parameters.HasValue();
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


void CParamsSetObjectList::append(sdl::imtbase::ImtBaseTypes::CParamsSetObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CParamsSetObjectList* sdl::imtbase::ImtBaseTypes::CParamsSetObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CParamsSetObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CParamsSetObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CParamsSetObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CParamsSetObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CParamsSetObjectList::addElement(sdl::imtbase::ImtBaseTypes::CParamsSetObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CParamsSetObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CParamsSetObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CParamsSetObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CParamsSetObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CParamsSetObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CParamsSetObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CParamsSetObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CParamsSetObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CParamsSetObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_paramIds"){
		return QVariant::fromValue(Version_1_0->at(index)->paramIds.value());
	}
	if (nameId == "m_paramTypeIds"){
		return QVariant::fromValue(Version_1_0->at(index)->paramTypeIds.value());
	}
	if (nameId == "m_paramNames"){
		return QVariant::fromValue(Version_1_0->at(index)->paramNames.value());
	}
	if (nameId == "m_paramDescriptions"){
		return QVariant::fromValue(Version_1_0->at(index)->paramDescriptions.value());
	}
	if (nameId == "m_parameters"){
		return QVariant::fromValue(Version_1_0->at(index)->parameters.value());
	}

	return QVariant();
}
CMimeTypeObject::CMimeTypeObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CMimeTypeObject::typeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CMimeTypeObject::treeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CMimeTypeObject::subTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CMimeTypeObject::suffixChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CMimeTypeObject::parametersChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CMimeTypeObject::GetType()
{
	if (Version_1_0 && Version_1_0->type){
		return Version_1_0->type.value();
	}

	return QVariant();
}


void CMimeTypeObject::SetType(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->type = v.value<QString>();
	typeChanged();
}


bool CMimeTypeObject::hasType()
{
	 return Version_1_0 && Version_1_0->type.HasValue();
}


QVariant CMimeTypeObject::GetTree()
{
	if (Version_1_0 && Version_1_0->tree){
		QList<QString> tempTreeList;
		for (const auto& tempValue: Version_1_0->tree.value()){
			tempTreeList << *tempValue;
		}
		return QVariant::fromValue(tempTreeList);
	}

	return QVariant();
}


void CMimeTypeObject::SetTree(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	if (!Version_1_0->tree){
		Version_1_0->tree.emplace();
	}
	else{
		Version_1_0->tree->clear();
	}
	for (const auto& tempValue: v.value<QList<QString>>()){
		istd::TSharedNullable<QString> value(tempValue);
		Version_1_0->tree->append(value);
	}

	treeChanged();
}


bool CMimeTypeObject::hasTree()
{
	 return Version_1_0 && Version_1_0->tree.HasValue();
}


QVariant CMimeTypeObject::GetSubType()
{
	if (Version_1_0 && Version_1_0->subType){
		return Version_1_0->subType.value();
	}

	return QVariant();
}


void CMimeTypeObject::SetSubType(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->subType = v.value<QString>();
	subTypeChanged();
}


bool CMimeTypeObject::hasSubType()
{
	 return Version_1_0 && Version_1_0->subType.HasValue();
}


QVariant CMimeTypeObject::GetSuffix()
{
	if (Version_1_0 && Version_1_0->suffix){
		return Version_1_0->suffix.value();
	}

	return QVariant();
}


void CMimeTypeObject::SetSuffix(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	Version_1_0->suffix = v.value<QString>();
	suffixChanged();
}


bool CMimeTypeObject::hasSuffix()
{
	 return Version_1_0 && Version_1_0->suffix.HasValue();
}


QVariant CMimeTypeObject::GetParameters()
{
	if (Version_1_0 && Version_1_0->parameters){
		QList<QString> tempParametersList;
		for (const auto& tempValue: Version_1_0->parameters.value()){
			tempParametersList << *tempValue;
		}
		return QVariant::fromValue(tempParametersList);
	}

	return QVariant();
}


void CMimeTypeObject::SetParameters(const QVariant& v)
{
	if (!Version_1_0){
		Version_1_0.emplace();
	}

	if (!Version_1_0->parameters){
		Version_1_0->parameters.emplace();
	}
	else{
		Version_1_0->parameters->clear();
	}
	for (const auto& tempValue: v.value<QList<QString>>()){
		istd::TSharedNullable<QString> value(tempValue);
		Version_1_0->parameters->append(value);
	}

	parametersChanged();
}


bool CMimeTypeObject::hasParameters()
{
	 return Version_1_0 && Version_1_0->parameters.HasValue();
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


void CMimeTypeObjectList::append(sdl::imtbase::ImtBaseTypes::CMimeTypeObject* item)
{
	BaseClass::append(item);
}


sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList* sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList::copyMe()
{
	sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList* retVal = new sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList::addElement(sdl::imtbase::ImtBaseTypes::CMimeTypeObject* item)
{
	append(item);
}


void sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList::isEqualWithModel(sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList::insert(int index, sdl::imtbase::ImtBaseTypes::CMimeTypeObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::imtbase::ImtBaseTypes::CMimeTypeObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::imtbase::ImtBaseTypes::CMimeTypeObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_type"){
		return QVariant::fromValue(Version_1_0->at(index)->type.value());
	}
	if (nameId == "m_tree"){
		return QVariant::fromValue(Version_1_0->at(index)->tree.value());
	}
	if (nameId == "m_subType"){
		return QVariant::fromValue(Version_1_0->at(index)->subType.value());
	}
	if (nameId == "m_suffix"){
		return QVariant::fromValue(Version_1_0->at(index)->suffix.value());
	}
	if (nameId == "m_parameters"){
		return QVariant::fromValue(Version_1_0->at(index)->parameters.value());
	}

	return QVariant();
}
} // namespace sdl::imtbase::ImtBaseTypes
