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
	if (!width){
		return false;
	}
	model.SetData("width", *width, modelIndex);

	if (!height){
		return false;
	}
	model.SetData("height", *height, modelIndex);


	return true;
}


bool CSdlSize::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant widthData = model.GetData("width", modelIndex);
	if (widthData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "width");)

		return false;
	}
	width = widthData.toDouble();

	QVariant heightData = model.GetData("height", modelIndex);
	if (heightData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "height");)

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
		return false;
	}
	gqlObject.InsertParam("width", QVariant(*width));

	if (!height){
		return false;
	}
	gqlObject.InsertParam("height", QVariant(*height));

	return true;
}


bool CSdlSize::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("width") || (gqlObject["width"].userType() != QMetaType::Float && gqlObject["width"].userType() != QMetaType::Double && gqlObject["width"].userType() != QMetaType::Int && gqlObject["width"].userType() != QMetaType::UInt && gqlObject["width"].userType() != QMetaType::LongLong && gqlObject["width"].userType() != QMetaType::ULongLong && gqlObject["width"].userType() != QMetaType::Long && gqlObject["width"].userType() != QMetaType::Short && gqlObject["width"].userType() != QMetaType::ULong && gqlObject["width"].userType() != QMetaType::UShort && gqlObject["width"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "width");)

		return false;
	}
	width = gqlObject["width"].toDouble();

	if (!gqlObject.ContainsParam("height") || (gqlObject["height"].userType() != QMetaType::Float && gqlObject["height"].userType() != QMetaType::Double && gqlObject["height"].userType() != QMetaType::Int && gqlObject["height"].userType() != QMetaType::UInt && gqlObject["height"].userType() != QMetaType::LongLong && gqlObject["height"].userType() != QMetaType::ULongLong && gqlObject["height"].userType() != QMetaType::Long && gqlObject["height"].userType() != QMetaType::Short && gqlObject["height"].userType() != QMetaType::ULong && gqlObject["height"].userType() != QMetaType::UShort && gqlObject["height"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "height");)

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
		return false;
	}
	jsonObject["width"] = QJsonValue::fromVariant(*width);

	if (!height){
		return false;
	}
	jsonObject["height"] = QJsonValue::fromVariant(*height);

	return true;
}


bool CSdlSize::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("width") || ! jsonObject["width"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "width");)

		return false;
	}
	width = jsonObject["width"].toDouble();

	if (!jsonObject.contains("height") || ! jsonObject["height"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "height");)

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
	if (!x){
		return false;
	}
	model.SetData("x", *x, modelIndex);

	if (!y){
		return false;
	}
	model.SetData("y", *y, modelIndex);


	return true;
}


bool CSdlPoint::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant xData = model.GetData("x", modelIndex);
	if (xData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "x");)

		return false;
	}
	x = xData.toDouble();

	QVariant yData = model.GetData("y", modelIndex);
	if (yData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "y");)

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
		return false;
	}
	gqlObject.InsertParam("x", QVariant(*x));

	if (!y){
		return false;
	}
	gqlObject.InsertParam("y", QVariant(*y));

	return true;
}


bool CSdlPoint::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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
		return false;
	}
	jsonObject["x"] = QJsonValue::fromVariant(*x);

	if (!y){
		return false;
	}
	jsonObject["y"] = QJsonValue::fromVariant(*y);

	return true;
}


bool CSdlPoint::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
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
			if (!(options->at(optionsIndex).WriteToModel(*newOptionsModelPtr, optionsIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "options");)

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
		QList<COption::V1_0> optionsList;
		for (int optionsIndex = 0; optionsIndex < optionsCount; ++optionsIndex){
			COption::V1_0 options;
			if (!options.ReadFromModel(*optionsModel, optionsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "options");)

				return false;
			}
			optionsList << options;
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
		QList<COption::V1_0> optionsList;
		for (int optionsIndex = 0; optionsIndex < optionsCount; ++optionsIndex){
			COption::V1_0 options;
			if (!options.OptReadFromModel(*optionsModel, optionsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "options");)

				return false;
			}
			optionsList << options;
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
			if (!options->at(optionsIndex).WriteToGraphQlObject(newOptionsGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "options");)

				return false;
			}
			optionsDataObjectList << newOptionsGqlObject;
		}
		gqlObject.InsertParam("options", optionsDataObjectList);
	}

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

	if (gqlObject.ContainsParam("options") && (gqlObject.GetObjectsCount("options") > 0)){
		const qsizetype optionsElementsCount = gqlObject.GetObjectsCount("options");
		options = QList<COption::V1_0>();
		for (qsizetype optionsIndex = 0; optionsIndex < optionsElementsCount; ++optionsIndex){
			const ::imtgql::CGqlParamObject* optionsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("options", optionsIndex);
			if (optionsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << optionsDataObjectPtr;
				return false;
			}
			COption::V1_0 tempOptions;
			if (!tempOptions.ReadFromGraphQlObject(*optionsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "options");)

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

	if (gqlObject.ContainsParam("options") && (gqlObject.GetObjectsCount("options") > 0)){
		const qsizetype optionsElementsCount = gqlObject.GetObjectsCount("options");
		options = QList<COption::V1_0>();
		for (qsizetype optionsIndex = 0; optionsIndex < optionsElementsCount; ++optionsIndex){
			const ::imtgql::CGqlParamObject* optionsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("options", optionsIndex);
			if (optionsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << optionsDataObjectPtr;
				return false;
			}
			COption::V1_0 tempOptions;
			if (!tempOptions.OptReadFromGraphQlObject(*optionsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "options");)

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
			if (!options->at(optionsIndex).WriteToJsonObject(newOptionsJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "options");)

				return false;
			}
			newOptionsArray << newOptionsJsonObject;
		}
		jsonObject["options"] = newOptionsArray;
	}

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
		options = QList<COption::V1_0>();
		for (qsizetype optionsIndex = 0; optionsIndex < optionsArrayCount; ++optionsIndex){
			COption::V1_0 tempOptions;
			if (!tempOptions.ReadFromJsonObject(optionsJsonArray[optionsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "options");)

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
		options = QList<COption::V1_0>();
		for (qsizetype optionsIndex = 0; optionsIndex < optionsArrayCount; ++optionsIndex){
			COption::V1_0 tempOptions;
			if (!tempOptions.OptReadFromJsonObject(optionsJsonArray[optionsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "options");)

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
	if (selectedIndex){
		model.SetData("selectedIndex", *selectedIndex, modelIndex);
	}


	if (constraints){
		::imtbase::CTreeItemModel* constraintsNewModelPtr = model.AddTreeModel("constraints", modelIndex);
		const bool isConstraintsAdded = constraints->WriteToModel(*constraintsNewModelPtr, 0);
		if (!isConstraintsAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "constraints");)

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
		const bool isConstraintsReaded = constraints->ReadFromModel(*constraintsDataModelPtr, modelIndex);
		if (!isConstraintsReaded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "constraints");)

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
		const bool isConstraintsReaded = constraints->ReadFromModel(*constraintsDataModelPtr, modelIndex);
		if (!isConstraintsReaded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "constraints");)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "constraints");)

			return false;
		}
		gqlObject.InsertParam("constraints", constraintsGqlObject);
	}

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "constraints");)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "constraints");)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "constraints");)

			return false;
		}
		jsonObject["constraints"] = constraintsJsonObject;
	}

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "constraints");)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "constraints");)

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

	if (schedulerParam){
		::imtbase::CTreeItemModel* schedulerParamNewModelPtr = model.AddTreeModel("schedulerParam", modelIndex);
		const bool isSchedulerParamAdded = schedulerParam->WriteToModel(*schedulerParamNewModelPtr, 0);
		if (!isSchedulerParamAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam");)

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
		const bool isSchedulerParamReaded = schedulerParam->ReadFromModel(*schedulerParamDataModelPtr, modelIndex);
		if (!isSchedulerParamReaded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam");)

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
		const bool isSchedulerParamReaded = schedulerParam->ReadFromModel(*schedulerParamDataModelPtr, modelIndex);
		if (!isSchedulerParamReaded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam");)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam");)

			return false;
		}
		gqlObject.InsertParam("schedulerParam", schedulerParamGqlObject);
	}

	if (folderPath){
		gqlObject.InsertParam("folderPath", QVariant(*folderPath));
	}

	return true;
}


bool CBackupSettings::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("schedulerParam") && (gqlObject.GetParamArgumentObjectPtr("schedulerParam") != nullptr)){
		schedulerParam = CSchedulerParam::V1_0();
		const bool isSchedulerParamRead = schedulerParam->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("schedulerParam"));
		if (!isSchedulerParamRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam");)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam");)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam");)

			return false;
		}
		jsonObject["schedulerParam"] = schedulerParamJsonObject;
	}

	if (folderPath){
		jsonObject["folderPath"] = QJsonValue::fromVariant(*folderPath);
	}

	return true;
}


bool CBackupSettings::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("schedulerParam") && jsonObject["schedulerParam"].isObject()){
		schedulerParam = CSchedulerParam::V1_0();
		const bool isSchedulerParamRead = schedulerParam->ReadFromJsonObject(jsonObject["schedulerParam"].toObject());
		if (!isSchedulerParamRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam");)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "schedulerParam");)

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
	if (paramIds){
		::imtbase::CTreeItemModel* newParamIdsModelPtr = model.AddTreeModel("paramIds", modelIndex);
		newParamIdsModelPtr->setIsArray(true);
		for (qsizetype paramIdsIndex = 0; paramIdsIndex < paramIds->size(); ++paramIdsIndex){
			newParamIdsModelPtr->InsertNewItem();
			newParamIdsModelPtr->SetData(QByteArray(), paramIds->at(paramIdsIndex), paramIdsIndex);
		}
	}

	if (paramTypeIds){
		::imtbase::CTreeItemModel* newParamTypeIdsModelPtr = model.AddTreeModel("paramTypeIds", modelIndex);
		newParamTypeIdsModelPtr->setIsArray(true);
		for (qsizetype paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIds->size(); ++paramTypeIdsIndex){
			newParamTypeIdsModelPtr->InsertNewItem();
			newParamTypeIdsModelPtr->SetData(QByteArray(), paramTypeIds->at(paramTypeIdsIndex), paramTypeIdsIndex);
		}
	}

	if (paramNames){
		::imtbase::CTreeItemModel* newParamNamesModelPtr = model.AddTreeModel("paramNames", modelIndex);
		newParamNamesModelPtr->setIsArray(true);
		for (qsizetype paramNamesIndex = 0; paramNamesIndex < paramNames->size(); ++paramNamesIndex){
			newParamNamesModelPtr->InsertNewItem();
			newParamNamesModelPtr->SetData(QByteArray(), paramNames->at(paramNamesIndex), paramNamesIndex);
		}
	}

	if (paramDescriptions){
		::imtbase::CTreeItemModel* newParamDescriptionsModelPtr = model.AddTreeModel("paramDescriptions", modelIndex);
		newParamDescriptionsModelPtr->setIsArray(true);
		for (qsizetype paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptions->size(); ++paramDescriptionsIndex){
			newParamDescriptionsModelPtr->InsertNewItem();
			newParamDescriptionsModelPtr->SetData(QByteArray(), paramDescriptions->at(paramDescriptionsIndex), paramDescriptionsIndex);
		}
	}

	if (parameters){
		::imtbase::CTreeItemModel* newParametersModelPtr = model.AddTreeModel("parameters", modelIndex);
		newParametersModelPtr->setIsArray(true);
		for (qsizetype parametersIndex = 0; parametersIndex < parameters->size(); ++parametersIndex){
			newParametersModelPtr->InsertNewItem();
			newParametersModelPtr->SetData(QByteArray(), parameters->at(parametersIndex), parametersIndex);
		}
	}


	return true;
}


bool CParamsSet::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* paramIdsModel = model.GetTreeItemModel("paramIds", modelIndex);
	if (paramIdsModel != nullptr){
		int paramIdsCount = paramIdsModel->GetItemsCount();
		QList<QByteArray> paramIdsList;
		for (int paramIdsIndex = 0; paramIdsIndex < paramIdsCount; ++paramIdsIndex){
			QByteArray paramIds = paramIdsModel->GetData(QByteArray(), paramIdsIndex).toByteArray();
			paramIdsList << paramIds;
		}
		paramIds = paramIdsList;

	}

	::imtbase::CTreeItemModel* paramTypeIdsModel = model.GetTreeItemModel("paramTypeIds", modelIndex);
	if (paramTypeIdsModel != nullptr){
		int paramTypeIdsCount = paramTypeIdsModel->GetItemsCount();
		QList<QByteArray> paramTypeIdsList;
		for (int paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIdsCount; ++paramTypeIdsIndex){
			QByteArray paramTypeIds = paramTypeIdsModel->GetData(QByteArray(), paramTypeIdsIndex).toByteArray();
			paramTypeIdsList << paramTypeIds;
		}
		paramTypeIds = paramTypeIdsList;

	}

	::imtbase::CTreeItemModel* paramNamesModel = model.GetTreeItemModel("paramNames", modelIndex);
	if (paramNamesModel != nullptr){
		int paramNamesCount = paramNamesModel->GetItemsCount();
		QList<QString> paramNamesList;
		for (int paramNamesIndex = 0; paramNamesIndex < paramNamesCount; ++paramNamesIndex){
			QString paramNames = paramNamesModel->GetData(QByteArray(), paramNamesIndex).toString();
			paramNamesList << paramNames;
		}
		paramNames = paramNamesList;

	}

	::imtbase::CTreeItemModel* paramDescriptionsModel = model.GetTreeItemModel("paramDescriptions", modelIndex);
	if (paramDescriptionsModel != nullptr){
		int paramDescriptionsCount = paramDescriptionsModel->GetItemsCount();
		QList<QString> paramDescriptionsList;
		for (int paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptionsCount; ++paramDescriptionsIndex){
			QString paramDescriptions = paramDescriptionsModel->GetData(QByteArray(), paramDescriptionsIndex).toString();
			paramDescriptionsList << paramDescriptions;
		}
		paramDescriptions = paramDescriptionsList;

	}

	::imtbase::CTreeItemModel* parametersModel = model.GetTreeItemModel("parameters", modelIndex);
	if (parametersModel != nullptr){
		int parametersCount = parametersModel->GetItemsCount();
		QList<QString> parametersList;
		for (int parametersIndex = 0; parametersIndex < parametersCount; ++parametersIndex){
			QString parameters = parametersModel->GetData(QByteArray(), parametersIndex).toString();
			parametersList << parameters;
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
		QList<QByteArray> paramIdsList;
		for (int paramIdsIndex = 0; paramIdsIndex < paramIdsCount; ++paramIdsIndex){
			QByteArray paramIds = paramIdsModel->GetData(QByteArray(), paramIdsIndex).toByteArray();
			paramIdsList << paramIds;
		}
		paramIds = paramIdsList;

	}

	::imtbase::CTreeItemModel* paramTypeIdsModel = model.GetTreeItemModel("paramTypeIds", modelIndex);
	if (paramTypeIdsModel != nullptr){
		int paramTypeIdsCount = paramTypeIdsModel->GetItemsCount();
		QList<QByteArray> paramTypeIdsList;
		for (int paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIdsCount; ++paramTypeIdsIndex){
			QByteArray paramTypeIds = paramTypeIdsModel->GetData(QByteArray(), paramTypeIdsIndex).toByteArray();
			paramTypeIdsList << paramTypeIds;
		}
		paramTypeIds = paramTypeIdsList;

	}

	::imtbase::CTreeItemModel* paramNamesModel = model.GetTreeItemModel("paramNames", modelIndex);
	if (paramNamesModel != nullptr){
		int paramNamesCount = paramNamesModel->GetItemsCount();
		QList<QString> paramNamesList;
		for (int paramNamesIndex = 0; paramNamesIndex < paramNamesCount; ++paramNamesIndex){
			QString paramNames = paramNamesModel->GetData(QByteArray(), paramNamesIndex).toString();
			paramNamesList << paramNames;
		}
		paramNames = paramNamesList;

	}

	::imtbase::CTreeItemModel* paramDescriptionsModel = model.GetTreeItemModel("paramDescriptions", modelIndex);
	if (paramDescriptionsModel != nullptr){
		int paramDescriptionsCount = paramDescriptionsModel->GetItemsCount();
		QList<QString> paramDescriptionsList;
		for (int paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptionsCount; ++paramDescriptionsIndex){
			QString paramDescriptions = paramDescriptionsModel->GetData(QByteArray(), paramDescriptionsIndex).toString();
			paramDescriptionsList << paramDescriptions;
		}
		paramDescriptions = paramDescriptionsList;

	}

	::imtbase::CTreeItemModel* parametersModel = model.GetTreeItemModel("parameters", modelIndex);
	if (parametersModel != nullptr){
		int parametersCount = parametersModel->GetItemsCount();
		QList<QString> parametersList;
		for (int parametersIndex = 0; parametersIndex < parametersCount; ++parametersIndex){
			QString parameters = parametersModel->GetData(QByteArray(), parametersIndex).toString();
			parametersList << parameters;
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
			paramIdsDataObjectList << paramIds->at(paramIdsIndex);
		}
		gqlObject.InsertParam("paramIds", paramIdsDataObjectList);
	}

	if (paramTypeIds){
		QVariantList paramTypeIdsDataObjectList;
		for (qsizetype paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIds->size(); ++paramTypeIdsIndex){
			paramTypeIdsDataObjectList << paramTypeIds->at(paramTypeIdsIndex);
		}
		gqlObject.InsertParam("paramTypeIds", paramTypeIdsDataObjectList);
	}

	if (paramNames){
		QVariantList paramNamesDataObjectList;
		for (qsizetype paramNamesIndex = 0; paramNamesIndex < paramNames->size(); ++paramNamesIndex){
			paramNamesDataObjectList << paramNames->at(paramNamesIndex);
		}
		gqlObject.InsertParam("paramNames", paramNamesDataObjectList);
	}

	if (paramDescriptions){
		QVariantList paramDescriptionsDataObjectList;
		for (qsizetype paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptions->size(); ++paramDescriptionsIndex){
			paramDescriptionsDataObjectList << paramDescriptions->at(paramDescriptionsIndex);
		}
		gqlObject.InsertParam("paramDescriptions", paramDescriptionsDataObjectList);
	}

	if (parameters){
		QVariantList parametersDataObjectList;
		for (qsizetype parametersIndex = 0; parametersIndex < parameters->size(); ++parametersIndex){
			parametersDataObjectList << parameters->at(parametersIndex);
		}
		gqlObject.InsertParam("parameters", parametersDataObjectList);
	}

	return true;
}


bool CParamsSet::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("paramIds") && !(gqlObject["paramIds"].isNull())){
		const QVariant paramIdsData = gqlObject["paramIds"];
		const QVariantList paramIdsDataList = paramIdsData.toList();
		const qsizetype paramIdsElementsCount = paramIdsDataList.size();
		paramIds = QList<QByteArray>();
		for (qsizetype paramIdsIndex = 0; paramIdsIndex < paramIdsElementsCount; ++paramIdsIndex){
			QByteArray tempParamIds = paramIdsDataList[paramIdsIndex].toByteArray();
			paramIds->append(tempParamIds);
		}
	}

	if (gqlObject.ContainsParam("paramTypeIds") && !(gqlObject["paramTypeIds"].isNull())){
		const QVariant paramTypeIdsData = gqlObject["paramTypeIds"];
		const QVariantList paramTypeIdsDataList = paramTypeIdsData.toList();
		const qsizetype paramTypeIdsElementsCount = paramTypeIdsDataList.size();
		paramTypeIds = QList<QByteArray>();
		for (qsizetype paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIdsElementsCount; ++paramTypeIdsIndex){
			QByteArray tempParamTypeIds = paramTypeIdsDataList[paramTypeIdsIndex].toByteArray();
			paramTypeIds->append(tempParamTypeIds);
		}
	}

	if (gqlObject.ContainsParam("paramNames") && !(gqlObject["paramNames"].isNull())){
		const QVariant paramNamesData = gqlObject["paramNames"];
		const QVariantList paramNamesDataList = paramNamesData.toList();
		const qsizetype paramNamesElementsCount = paramNamesDataList.size();
		paramNames = QList<QString>();
		for (qsizetype paramNamesIndex = 0; paramNamesIndex < paramNamesElementsCount; ++paramNamesIndex){
			QString tempParamNames = paramNamesDataList[paramNamesIndex].toString();
			paramNames->append(tempParamNames);
		}
	}

	if (gqlObject.ContainsParam("paramDescriptions") && !(gqlObject["paramDescriptions"].isNull())){
		const QVariant paramDescriptionsData = gqlObject["paramDescriptions"];
		const QVariantList paramDescriptionsDataList = paramDescriptionsData.toList();
		const qsizetype paramDescriptionsElementsCount = paramDescriptionsDataList.size();
		paramDescriptions = QList<QString>();
		for (qsizetype paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptionsElementsCount; ++paramDescriptionsIndex){
			QString tempParamDescriptions = paramDescriptionsDataList[paramDescriptionsIndex].toString();
			paramDescriptions->append(tempParamDescriptions);
		}
	}

	if (gqlObject.ContainsParam("parameters") && !(gqlObject["parameters"].isNull())){
		const QVariant parametersData = gqlObject["parameters"];
		const QVariantList parametersDataList = parametersData.toList();
		const qsizetype parametersElementsCount = parametersDataList.size();
		parameters = QList<QString>();
		for (qsizetype parametersIndex = 0; parametersIndex < parametersElementsCount; ++parametersIndex){
			QString tempParameters = parametersDataList[parametersIndex].toString();
			parameters->append(tempParameters);
		}
	}

	return true;
}


bool CParamsSet::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("paramIds") && !(gqlObject["paramIds"].isNull())){
		const QVariant paramIdsData = gqlObject["paramIds"];
		const QVariantList paramIdsDataList = paramIdsData.toList();
		const qsizetype paramIdsElementsCount = paramIdsDataList.size();
		paramIds = QList<QByteArray>();
		for (qsizetype paramIdsIndex = 0; paramIdsIndex < paramIdsElementsCount; ++paramIdsIndex){
			QByteArray tempParamIds = paramIdsDataList[paramIdsIndex].toByteArray();
			paramIds->append(tempParamIds);
		}
	}

	if (gqlObject.ContainsParam("paramTypeIds") && !(gqlObject["paramTypeIds"].isNull())){
		const QVariant paramTypeIdsData = gqlObject["paramTypeIds"];
		const QVariantList paramTypeIdsDataList = paramTypeIdsData.toList();
		const qsizetype paramTypeIdsElementsCount = paramTypeIdsDataList.size();
		paramTypeIds = QList<QByteArray>();
		for (qsizetype paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIdsElementsCount; ++paramTypeIdsIndex){
			QByteArray tempParamTypeIds = paramTypeIdsDataList[paramTypeIdsIndex].toByteArray();
			paramTypeIds->append(tempParamTypeIds);
		}
	}

	if (gqlObject.ContainsParam("paramNames") && !(gqlObject["paramNames"].isNull())){
		const QVariant paramNamesData = gqlObject["paramNames"];
		const QVariantList paramNamesDataList = paramNamesData.toList();
		const qsizetype paramNamesElementsCount = paramNamesDataList.size();
		paramNames = QList<QString>();
		for (qsizetype paramNamesIndex = 0; paramNamesIndex < paramNamesElementsCount; ++paramNamesIndex){
			QString tempParamNames = paramNamesDataList[paramNamesIndex].toString();
			paramNames->append(tempParamNames);
		}
	}

	if (gqlObject.ContainsParam("paramDescriptions") && !(gqlObject["paramDescriptions"].isNull())){
		const QVariant paramDescriptionsData = gqlObject["paramDescriptions"];
		const QVariantList paramDescriptionsDataList = paramDescriptionsData.toList();
		const qsizetype paramDescriptionsElementsCount = paramDescriptionsDataList.size();
		paramDescriptions = QList<QString>();
		for (qsizetype paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptionsElementsCount; ++paramDescriptionsIndex){
			QString tempParamDescriptions = paramDescriptionsDataList[paramDescriptionsIndex].toString();
			paramDescriptions->append(tempParamDescriptions);
		}
	}

	if (gqlObject.ContainsParam("parameters") && !(gqlObject["parameters"].isNull())){
		const QVariant parametersData = gqlObject["parameters"];
		const QVariantList parametersDataList = parametersData.toList();
		const qsizetype parametersElementsCount = parametersDataList.size();
		parameters = QList<QString>();
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
			newParamIdsArray << QString(paramIds->at(paramIdsIndex));
		}
		jsonObject["paramIds"] = newParamIdsArray;
	}

	if (paramTypeIds){
		QJsonArray newParamTypeIdsArray;
		for (qsizetype paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIds->size(); ++paramTypeIdsIndex){
			newParamTypeIdsArray << QString(paramTypeIds->at(paramTypeIdsIndex));
		}
		jsonObject["paramTypeIds"] = newParamTypeIdsArray;
	}

	if (paramNames){
		QJsonArray newParamNamesArray;
		for (qsizetype paramNamesIndex = 0; paramNamesIndex < paramNames->size(); ++paramNamesIndex){
			newParamNamesArray << paramNames->at(paramNamesIndex);
		}
		jsonObject["paramNames"] = newParamNamesArray;
	}

	if (paramDescriptions){
		QJsonArray newParamDescriptionsArray;
		for (qsizetype paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptions->size(); ++paramDescriptionsIndex){
			newParamDescriptionsArray << paramDescriptions->at(paramDescriptionsIndex);
		}
		jsonObject["paramDescriptions"] = newParamDescriptionsArray;
	}

	if (parameters){
		QJsonArray newParametersArray;
		for (qsizetype parametersIndex = 0; parametersIndex < parameters->size(); ++parametersIndex){
			newParametersArray << parameters->at(parametersIndex);
		}
		jsonObject["parameters"] = newParametersArray;
	}

	return true;
}


bool CParamsSet::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("paramIds") && jsonObject["paramIds"].isArray()){
		const QJsonArray paramIdsJsonArray = jsonObject["paramIds"].toArray();
		const qsizetype paramIdsArrayCount = paramIdsJsonArray.size();
		paramIds = QList<QByteArray>();
		for (qsizetype paramIdsIndex = 0; paramIdsIndex < paramIdsArrayCount; ++paramIdsIndex){
			QByteArray tempParamIds = paramIdsJsonArray[paramIdsIndex].toString().toUtf8();
			paramIds->append(tempParamIds);
		}
	}

	if (jsonObject.contains("paramTypeIds") && jsonObject["paramTypeIds"].isArray()){
		const QJsonArray paramTypeIdsJsonArray = jsonObject["paramTypeIds"].toArray();
		const qsizetype paramTypeIdsArrayCount = paramTypeIdsJsonArray.size();
		paramTypeIds = QList<QByteArray>();
		for (qsizetype paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIdsArrayCount; ++paramTypeIdsIndex){
			QByteArray tempParamTypeIds = paramTypeIdsJsonArray[paramTypeIdsIndex].toString().toUtf8();
			paramTypeIds->append(tempParamTypeIds);
		}
	}

	if (jsonObject.contains("paramNames") && jsonObject["paramNames"].isArray()){
		const QJsonArray paramNamesJsonArray = jsonObject["paramNames"].toArray();
		const qsizetype paramNamesArrayCount = paramNamesJsonArray.size();
		paramNames = QList<QString>();
		for (qsizetype paramNamesIndex = 0; paramNamesIndex < paramNamesArrayCount; ++paramNamesIndex){
			QString tempParamNames = paramNamesJsonArray[paramNamesIndex].toString();
			paramNames->append(tempParamNames);
		}
	}

	if (jsonObject.contains("paramDescriptions") && jsonObject["paramDescriptions"].isArray()){
		const QJsonArray paramDescriptionsJsonArray = jsonObject["paramDescriptions"].toArray();
		const qsizetype paramDescriptionsArrayCount = paramDescriptionsJsonArray.size();
		paramDescriptions = QList<QString>();
		for (qsizetype paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptionsArrayCount; ++paramDescriptionsIndex){
			QString tempParamDescriptions = paramDescriptionsJsonArray[paramDescriptionsIndex].toString();
			paramDescriptions->append(tempParamDescriptions);
		}
	}

	if (jsonObject.contains("parameters") && jsonObject["parameters"].isArray()){
		const QJsonArray parametersJsonArray = jsonObject["parameters"].toArray();
		const qsizetype parametersArrayCount = parametersJsonArray.size();
		parameters = QList<QString>();
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
		paramIds = QList<QByteArray>();
		for (qsizetype paramIdsIndex = 0; paramIdsIndex < paramIdsArrayCount; ++paramIdsIndex){
			QByteArray tempParamIds = paramIdsJsonArray[paramIdsIndex].toString().toUtf8();
			paramIds->append(tempParamIds);
		}
	}

	if (jsonObject.contains("paramTypeIds") && jsonObject["paramTypeIds"].isArray()){
		const QJsonArray paramTypeIdsJsonArray = jsonObject["paramTypeIds"].toArray();
		const qsizetype paramTypeIdsArrayCount = paramTypeIdsJsonArray.size();
		paramTypeIds = QList<QByteArray>();
		for (qsizetype paramTypeIdsIndex = 0; paramTypeIdsIndex < paramTypeIdsArrayCount; ++paramTypeIdsIndex){
			QByteArray tempParamTypeIds = paramTypeIdsJsonArray[paramTypeIdsIndex].toString().toUtf8();
			paramTypeIds->append(tempParamTypeIds);
		}
	}

	if (jsonObject.contains("paramNames") && jsonObject["paramNames"].isArray()){
		const QJsonArray paramNamesJsonArray = jsonObject["paramNames"].toArray();
		const qsizetype paramNamesArrayCount = paramNamesJsonArray.size();
		paramNames = QList<QString>();
		for (qsizetype paramNamesIndex = 0; paramNamesIndex < paramNamesArrayCount; ++paramNamesIndex){
			QString tempParamNames = paramNamesJsonArray[paramNamesIndex].toString();
			paramNames->append(tempParamNames);
		}
	}

	if (jsonObject.contains("paramDescriptions") && jsonObject["paramDescriptions"].isArray()){
		const QJsonArray paramDescriptionsJsonArray = jsonObject["paramDescriptions"].toArray();
		const qsizetype paramDescriptionsArrayCount = paramDescriptionsJsonArray.size();
		paramDescriptions = QList<QString>();
		for (qsizetype paramDescriptionsIndex = 0; paramDescriptionsIndex < paramDescriptionsArrayCount; ++paramDescriptionsIndex){
			QString tempParamDescriptions = paramDescriptionsJsonArray[paramDescriptionsIndex].toString();
			paramDescriptions->append(tempParamDescriptions);
		}
	}

	if (jsonObject.contains("parameters") && jsonObject["parameters"].isArray()){
		const QJsonArray parametersJsonArray = jsonObject["parameters"].toArray();
		const qsizetype parametersArrayCount = parametersJsonArray.size();
		parameters = QList<QString>();
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
	if (!type){
		return false;
	}
	model.SetData("type", *type, modelIndex);

	if (tree){
		::imtbase::CTreeItemModel* newTreeModelPtr = model.AddTreeModel("tree", modelIndex);
		newTreeModelPtr->setIsArray(true);
		for (qsizetype treeIndex = 0; treeIndex < tree->size(); ++treeIndex){
			newTreeModelPtr->InsertNewItem();
			newTreeModelPtr->SetData(QByteArray(), tree->at(treeIndex), treeIndex);
		}
	}

	if (!subType){
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
			newParametersModelPtr->SetData(QByteArray(), parameters->at(parametersIndex), parametersIndex);
		}
	}


	return true;
}


bool CMimeType::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant typeData = model.GetData("type", modelIndex);
	if (typeData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "type");)

		return false;
	}
	type = typeData.toString();

	::imtbase::CTreeItemModel* treeModel = model.GetTreeItemModel("tree", modelIndex);
	if (treeModel != nullptr){
		int treeCount = treeModel->GetItemsCount();
		QList<QString> treeList;
		for (int treeIndex = 0; treeIndex < treeCount; ++treeIndex){
			QString tree = treeModel->GetData(QByteArray(), treeIndex).toString();
			treeList << tree;
		}
		tree = treeList;

	}

	QVariant subTypeData = model.GetData("subType", modelIndex);
	if (subTypeData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "subType");)

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
		QList<QString> parametersList;
		for (int parametersIndex = 0; parametersIndex < parametersCount; ++parametersIndex){
			QString parameters = parametersModel->GetData(QByteArray(), parametersIndex).toString();
			parametersList << parameters;
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
		QList<QString> treeList;
		for (int treeIndex = 0; treeIndex < treeCount; ++treeIndex){
			QString tree = treeModel->GetData(QByteArray(), treeIndex).toString();
			treeList << tree;
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
		QList<QString> parametersList;
		for (int parametersIndex = 0; parametersIndex < parametersCount; ++parametersIndex){
			QString parameters = parametersModel->GetData(QByteArray(), parametersIndex).toString();
			parametersList << parameters;
		}
		parameters = parametersList;

	}

	return true;
}


bool CMimeType::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!type){
		return false;
	}
	gqlObject.InsertParam("type", QVariant(*type));

	if (tree){
		QVariantList treeDataObjectList;
		for (qsizetype treeIndex = 0; treeIndex < tree->size(); ++treeIndex){
			treeDataObjectList << tree->at(treeIndex);
		}
		gqlObject.InsertParam("tree", treeDataObjectList);
	}

	if (!subType){
		return false;
	}
	gqlObject.InsertParam("subType", QVariant(*subType));

	if (suffix){
		gqlObject.InsertParam("suffix", QVariant(*suffix));
	}

	if (parameters){
		QVariantList parametersDataObjectList;
		for (qsizetype parametersIndex = 0; parametersIndex < parameters->size(); ++parametersIndex){
			parametersDataObjectList << parameters->at(parametersIndex);
		}
		gqlObject.InsertParam("parameters", parametersDataObjectList);
	}

	return true;
}


bool CMimeType::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("type") || (gqlObject["type"].userType() != QMetaType::QString && gqlObject["type"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "type");)

		return false;
	}
	type = gqlObject["type"].toString();

	if (gqlObject.ContainsParam("tree") && !(gqlObject["tree"].isNull())){
		const QVariant treeData = gqlObject["tree"];
		const QVariantList treeDataList = treeData.toList();
		const qsizetype treeElementsCount = treeDataList.size();
		tree = QList<QString>();
		for (qsizetype treeIndex = 0; treeIndex < treeElementsCount; ++treeIndex){
			QString tempTree = treeDataList[treeIndex].toString();
			tree->append(tempTree);
		}
	}

	if (!gqlObject.ContainsParam("subType") || (gqlObject["subType"].userType() != QMetaType::QString && gqlObject["subType"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "subType");)

		return false;
	}
	subType = gqlObject["subType"].toString();

	if (gqlObject.ContainsParam("suffix") && (gqlObject["suffix"].userType() == QMetaType::QString || gqlObject["suffix"].userType() == QMetaType::QByteArray)){
		suffix = gqlObject["suffix"].toString();
	}

	if (gqlObject.ContainsParam("parameters") && !(gqlObject["parameters"].isNull())){
		const QVariant parametersData = gqlObject["parameters"];
		const QVariantList parametersDataList = parametersData.toList();
		const qsizetype parametersElementsCount = parametersDataList.size();
		parameters = QList<QString>();
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

	if (gqlObject.ContainsParam("tree") && !(gqlObject["tree"].isNull())){
		const QVariant treeData = gqlObject["tree"];
		const QVariantList treeDataList = treeData.toList();
		const qsizetype treeElementsCount = treeDataList.size();
		tree = QList<QString>();
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

	if (gqlObject.ContainsParam("parameters") && !(gqlObject["parameters"].isNull())){
		const QVariant parametersData = gqlObject["parameters"];
		const QVariantList parametersDataList = parametersData.toList();
		const qsizetype parametersElementsCount = parametersDataList.size();
		parameters = QList<QString>();
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
		return false;
	}
	jsonObject["type"] = QJsonValue::fromVariant(*type);

	if (tree){
		QJsonArray newTreeArray;
		for (qsizetype treeIndex = 0; treeIndex < tree->size(); ++treeIndex){
			newTreeArray << tree->at(treeIndex);
		}
		jsonObject["tree"] = newTreeArray;
	}

	if (!subType){
		return false;
	}
	jsonObject["subType"] = QJsonValue::fromVariant(*subType);

	if (suffix){
		jsonObject["suffix"] = QJsonValue::fromVariant(*suffix);
	}

	if (parameters){
		QJsonArray newParametersArray;
		for (qsizetype parametersIndex = 0; parametersIndex < parameters->size(); ++parametersIndex){
			newParametersArray << parameters->at(parametersIndex);
		}
		jsonObject["parameters"] = newParametersArray;
	}

	return true;
}


bool CMimeType::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("type") || ! jsonObject["type"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "type");)

		return false;
	}
	type = jsonObject["type"].toString();

	if (jsonObject.contains("tree") && jsonObject["tree"].isArray()){
		const QJsonArray treeJsonArray = jsonObject["tree"].toArray();
		const qsizetype treeArrayCount = treeJsonArray.size();
		tree = QList<QString>();
		for (qsizetype treeIndex = 0; treeIndex < treeArrayCount; ++treeIndex){
			QString tempTree = treeJsonArray[treeIndex].toString();
			tree->append(tempTree);
		}
	}

	if (!jsonObject.contains("subType") || ! jsonObject["subType"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "subType");)

		return false;
	}
	subType = jsonObject["subType"].toString();

	if (jsonObject.contains("suffix") && jsonObject["suffix"].isString()){
		suffix = jsonObject["suffix"].toString();
	}

	if (jsonObject.contains("parameters") && jsonObject["parameters"].isArray()){
		const QJsonArray parametersJsonArray = jsonObject["parameters"].toArray();
		const qsizetype parametersArrayCount = parametersJsonArray.size();
		parameters = QList<QString>();
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
		tree = QList<QString>();
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
		parameters = QList<QString>();
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




} // namespace sdl::imtbase::ImtBaseTypes
