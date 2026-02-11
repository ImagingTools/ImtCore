// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ComplexUnion1.h"


namespace sdl::complextest::ComplexUnion1
{


QByteArray CPoint::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPoint::V1_0::operator==(const V1_0& other) const
{
	return 
				X.has_value() == other.X.has_value() &&
				((X.has_value() && other.X.has_value()) ?
					qFuzzyCompare(*X, *other.X) : true) &&
				Y.has_value() == other.Y.has_value() &&
				((Y.has_value() && other.Y.has_value()) ?
					qFuzzyCompare(*Y, *other.Y) : true);
}


bool CPoint::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Point", modelIndex);

	if (!X){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("X", *X, modelIndex);

	if (!Y){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("Y", *Y, modelIndex);


	return true;
}


bool CPoint::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant xData = model.GetData("X", modelIndex);
	if (xData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	X = xData.toFloat();

	QVariant yData = model.GetData("Y", modelIndex);
	if (yData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

		return false;
	}
	Y = yData.toFloat();

	return true;
}


bool CPoint::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant xData = model.GetData("X", modelIndex);
	if (!xData.isNull()){
		X = xData.toFloat();
	}

	QVariant yData = model.GetData("Y", modelIndex);
	if (!yData.isNull()){
		Y = yData.toFloat();
	}

	return true;
}


bool CPoint::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!X){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("X", QVariant(*X));

	if (!Y){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("Y", QVariant(*Y));

	gqlObject.InsertParam("__typename", QVariant("Point"));

	return true;
}


bool CPoint::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("X") || (gqlObject["X"].userType() != QMetaType::Float && gqlObject["X"].userType() != QMetaType::Double && gqlObject["X"].userType() != QMetaType::Int && gqlObject["X"].userType() != QMetaType::UInt && gqlObject["X"].userType() != QMetaType::LongLong && gqlObject["X"].userType() != QMetaType::ULongLong && gqlObject["X"].userType() != QMetaType::Long && gqlObject["X"].userType() != QMetaType::Short && gqlObject["X"].userType() != QMetaType::ULong && gqlObject["X"].userType() != QMetaType::UShort && gqlObject["X"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	X = gqlObject["X"].toDouble();

	if (!gqlObject.ContainsParam("Y") || (gqlObject["Y"].userType() != QMetaType::Float && gqlObject["Y"].userType() != QMetaType::Double && gqlObject["Y"].userType() != QMetaType::Int && gqlObject["Y"].userType() != QMetaType::UInt && gqlObject["Y"].userType() != QMetaType::LongLong && gqlObject["Y"].userType() != QMetaType::ULongLong && gqlObject["Y"].userType() != QMetaType::Long && gqlObject["Y"].userType() != QMetaType::Short && gqlObject["Y"].userType() != QMetaType::ULong && gqlObject["Y"].userType() != QMetaType::UShort && gqlObject["Y"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

		return false;
	}
	Y = gqlObject["Y"].toDouble();

	return true;
}


bool CPoint::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("X") && (gqlObject["X"].userType() == QMetaType::Float || gqlObject["X"].userType() == QMetaType::Double || gqlObject["X"].userType() == QMetaType::Int || gqlObject["X"].userType() == QMetaType::UInt || gqlObject["X"].userType() == QMetaType::LongLong || gqlObject["X"].userType() == QMetaType::ULongLong || gqlObject["X"].userType() == QMetaType::Long || gqlObject["X"].userType() == QMetaType::Short || gqlObject["X"].userType() == QMetaType::ULong || gqlObject["X"].userType() == QMetaType::UShort || gqlObject["X"].userType() == QMetaType::UChar)){
		X = gqlObject["X"].toDouble();
	}

	if (gqlObject.ContainsParam("Y") && (gqlObject["Y"].userType() == QMetaType::Float || gqlObject["Y"].userType() == QMetaType::Double || gqlObject["Y"].userType() == QMetaType::Int || gqlObject["Y"].userType() == QMetaType::UInt || gqlObject["Y"].userType() == QMetaType::LongLong || gqlObject["Y"].userType() == QMetaType::ULongLong || gqlObject["Y"].userType() == QMetaType::Long || gqlObject["Y"].userType() == QMetaType::Short || gqlObject["Y"].userType() == QMetaType::ULong || gqlObject["Y"].userType() == QMetaType::UShort || gqlObject["Y"].userType() == QMetaType::UChar)){
		Y = gqlObject["Y"].toDouble();
	}

	return true;
}


bool CPoint::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!X){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["X"] = QJsonValue::fromVariant(*X);

	if (!Y){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["Y"] = QJsonValue::fromVariant(*Y);

	jsonObject["__typename"] = "Point";

	return true;
}


bool CPoint::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("X") || ! jsonObject["X"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	X = jsonObject["X"].toDouble();

	if (!jsonObject.contains("Y") || ! jsonObject["Y"].isDouble()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

		return false;
	}
	Y = jsonObject["Y"].toDouble();

	return true;
}


bool CPoint::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("X") && jsonObject["X"].isDouble()){
		X = jsonObject["X"].toDouble();
	}

	if (jsonObject.contains("Y") && jsonObject["Y"].isDouble()){
		Y = jsonObject["Y"].toDouble();
	}

	return true;
}


// serialize methods

bool CPoint::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
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


bool CPoint::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
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


bool CPoint::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
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


bool CPoint::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
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


bool CPoint::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
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


bool CPoint::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
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


bool CPoint::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
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


bool CPoint::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
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


bool CPoint::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
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




QByteArray CGeometry::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CGeometry::V1_0::operator==(const V1_0& other) const
{
	return 
				GeometryType == other.GeometryType &&
				Radius.has_value() == other.Radius.has_value() &&
				((Radius.has_value() && other.Radius.has_value()) ?
					qFuzzyCompare(*Radius, *other.Radius) : true) &&
				Points == other.Points &&
				RequiredPoints == other.RequiredPoints &&
				OptionalPoints == other.OptionalPoints;
}


bool CGeometry::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "Geometry", modelIndex);

	if (!GeometryType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "GeometryType").toLocal8Bit().constData();)

		return false;
	}
	QString geometryTypeStringValue;
	switch (*GeometryType){
	case GeometryType::LINE:
		geometryTypeStringValue = "LINE";
		break;
	case GeometryType::POLYGON:
		geometryTypeStringValue = "POLYGON";
		break;
	case GeometryType::CIRCLE:
		geometryTypeStringValue = "CIRCLE";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	model.SetData("GeometryType", geometryTypeStringValue, modelIndex);

	if (Radius){
		model.SetData("Radius", *Radius, modelIndex);
	}

	if (!Points){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

		return false;
	}
	::imtbase::CTreeItemModel* newPointsModelPtr = model.AddTreeModel("Points", modelIndex);
	newPointsModelPtr->setIsArray(true);
	for (qsizetype pointsIndex = 0; pointsIndex < Points->size(); ++pointsIndex){
		newPointsModelPtr->InsertNewItem();
		if (!(Points->at(pointsIndex)->WriteToModel(*newPointsModelPtr, pointsIndex))){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

			return false;
		}
	}
	if (!RequiredPoints || RequiredPoints->isEmpty()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

		return false;
	}
	::imtbase::CTreeItemModel* newRequiredPointsModelPtr = model.AddTreeModel("RequiredPoints", modelIndex);
	newRequiredPointsModelPtr->setIsArray(true);
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < RequiredPoints->size(); ++requiredPointsIndex){
		newRequiredPointsModelPtr->InsertNewItem();
		if (!(RequiredPoints->at(requiredPointsIndex)->WriteToModel(*newRequiredPointsModelPtr, requiredPointsIndex))){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
	}
	if (OptionalPoints){
		::imtbase::CTreeItemModel* newOptionalPointsModelPtr = model.AddTreeModel("OptionalPoints", modelIndex);
		newOptionalPointsModelPtr->setIsArray(true);
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < OptionalPoints->size(); ++optionalPointsIndex){
			newOptionalPointsModelPtr->InsertNewItem();
			if (!(OptionalPoints->at(optionalPointsIndex)->WriteToModel(*newOptionalPointsModelPtr, optionalPointsIndex))){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
		}
	}

	return true;
}


bool CGeometry::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant geometryTypeData = model.GetData("GeometryType", modelIndex);
	if (geometryTypeData.isNull()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "GeometryType").toLocal8Bit().constData();)

		return false;
	}
	QString geometryTypeStringValue = geometryTypeData.toString();
	if(geometryTypeStringValue == "LINE"){
		GeometryType = GeometryType::LINE;
	}
	else if(geometryTypeStringValue == "POLYGON"){
		GeometryType = GeometryType::POLYGON;
	}
	else if(geometryTypeStringValue == "CIRCLE"){
		GeometryType = GeometryType::CIRCLE;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

		return false;
	}

	QVariant radiusData = model.GetData("Radius", modelIndex);
	if (!radiusData.isNull()){
		Radius = radiusData.toFloat();
	}

	::imtbase::CTreeItemModel* pointsModel = model.GetTreeItemModel("Points", modelIndex);
	if (pointsModel == nullptr){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is missing, but required.").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

		return false;
	}
	int pointsCount = pointsModel->GetItemsCount();
	imtsdl::TElementList<CPoint::V1_0> pointsList;
	for (int pointsIndex = 0; pointsIndex < pointsCount; ++pointsIndex){
		CPoint::V1_0 t_points;
		if (!t_points.ReadFromModel(*pointsModel, pointsIndex)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

			return false;
		}
		pointsList << t_points;
	}
	Points = pointsList;


	::imtbase::CTreeItemModel* requiredPointsModel = model.GetTreeItemModel("RequiredPoints", modelIndex);
	if (requiredPointsModel == nullptr){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is missing, but required.").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

		return false;
	}
	int requiredPointsCount = requiredPointsModel->GetItemsCount();
	if (requiredPointsCount <= 0){
	I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is empty").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

		return false;
	}
	imtsdl::TElementList<CPoint::V1_0> requiredPointsList;
	for (int requiredPointsIndex = 0; requiredPointsIndex < requiredPointsCount; ++requiredPointsIndex){
		CPoint::V1_0 t_requiredPoints;
		if (!t_requiredPoints.ReadFromModel(*requiredPointsModel, requiredPointsIndex)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		requiredPointsList << t_requiredPoints;
	}
	RequiredPoints = requiredPointsList;


	::imtbase::CTreeItemModel* optionalPointsModel = model.GetTreeItemModel("OptionalPoints", modelIndex);
	if (optionalPointsModel != nullptr){
		int optionalPointsCount = optionalPointsModel->GetItemsCount();
		imtsdl::TElementList<CPoint::V1_0> optionalPointsList;
		for (int optionalPointsIndex = 0; optionalPointsIndex < optionalPointsCount; ++optionalPointsIndex){
			CPoint::V1_0 t_optionalPoints;
			if (!t_optionalPoints.ReadFromModel(*optionalPointsModel, optionalPointsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
			optionalPointsList << t_optionalPoints;
		}
		OptionalPoints = optionalPointsList;

	}

	return true;
}


bool CGeometry::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant geometryTypeData = model.GetData("GeometryType", modelIndex);
	if (!geometryTypeData.isNull()){
		QString geometryTypeStringValue = geometryTypeData.toString();
		if(geometryTypeStringValue == "LINE"){
			GeometryType = GeometryType::LINE;
		}
		else if(geometryTypeStringValue == "POLYGON"){
			GeometryType = GeometryType::POLYGON;
		}
		else if(geometryTypeStringValue == "CIRCLE"){
			GeometryType = GeometryType::CIRCLE;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

			return false;
		}
	}

	QVariant radiusData = model.GetData("Radius", modelIndex);
	if (!radiusData.isNull()){
		Radius = radiusData.toFloat();
	}

	::imtbase::CTreeItemModel* pointsModel = model.GetTreeItemModel("Points", modelIndex);
	if (pointsModel != nullptr){
		int pointsCount = pointsModel->GetItemsCount();
		imtsdl::TElementList<CPoint::V1_0> pointsList;
		for (int pointsIndex = 0; pointsIndex < pointsCount; ++pointsIndex){
			CPoint::V1_0 t_points;
			if (!t_points.OptReadFromModel(*pointsModel, pointsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

				return false;
			}
			pointsList << t_points;
		}
		Points = pointsList;

	}

	::imtbase::CTreeItemModel* requiredPointsModel = model.GetTreeItemModel("RequiredPoints", modelIndex);
	if (requiredPointsModel != nullptr){
		int requiredPointsCount = requiredPointsModel->GetItemsCount();
		if (requiredPointsCount <= 0){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field '%3' is empty").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		imtsdl::TElementList<CPoint::V1_0> requiredPointsList;
		for (int requiredPointsIndex = 0; requiredPointsIndex < requiredPointsCount; ++requiredPointsIndex){
			CPoint::V1_0 t_requiredPoints;
			if (!t_requiredPoints.OptReadFromModel(*requiredPointsModel, requiredPointsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

				return false;
			}
			requiredPointsList << t_requiredPoints;
		}
		RequiredPoints = requiredPointsList;

	}

	::imtbase::CTreeItemModel* optionalPointsModel = model.GetTreeItemModel("OptionalPoints", modelIndex);
	if (optionalPointsModel != nullptr){
		int optionalPointsCount = optionalPointsModel->GetItemsCount();
		imtsdl::TElementList<CPoint::V1_0> optionalPointsList;
		for (int optionalPointsIndex = 0; optionalPointsIndex < optionalPointsCount; ++optionalPointsIndex){
			CPoint::V1_0 t_optionalPoints;
			if (!t_optionalPoints.OptReadFromModel(*optionalPointsModel, optionalPointsIndex)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
			optionalPointsList << t_optionalPoints;
		}
		OptionalPoints = optionalPointsList;

	}

	return true;
}


bool CGeometry::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (!GeometryType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "GeometryType").toLocal8Bit().constData();)

		return false;
	}
	QString geometryTypeStringValue;
	switch (*GeometryType){
	case GeometryType::LINE:
		geometryTypeStringValue = "LINE";
		break;
	case GeometryType::POLYGON:
		geometryTypeStringValue = "POLYGON";
		break;
	case GeometryType::CIRCLE:
		geometryTypeStringValue = "CIRCLE";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	gqlObject.InsertParam("GeometryType", QVariant(geometryTypeStringValue));

	if (Radius){
		gqlObject.InsertParam("Radius", QVariant(*Radius));
	}

	if (!Points){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

		return false;
	}
	QList<::imtgql::CGqlParamObject> pointsDataObjectList;
	for (qsizetype pointsIndex = 0; pointsIndex < Points->size(); ++pointsIndex){
		::imtgql::CGqlParamObject newPointsGqlObject;
		if (!Points->at(pointsIndex)->WriteToGraphQlObject(newPointsGqlObject)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

			return false;
		}
		pointsDataObjectList << newPointsGqlObject;
	}
	gqlObject.InsertParam("Points", pointsDataObjectList);

	if (!RequiredPoints || RequiredPoints->isEmpty()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

		return false;
	}
	QList<::imtgql::CGqlParamObject> requiredPointsDataObjectList;
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < RequiredPoints->size(); ++requiredPointsIndex){
		::imtgql::CGqlParamObject newRequiredPointsGqlObject;
		if (!RequiredPoints->at(requiredPointsIndex)->WriteToGraphQlObject(newRequiredPointsGqlObject)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		requiredPointsDataObjectList << newRequiredPointsGqlObject;
	}
	gqlObject.InsertParam("RequiredPoints", requiredPointsDataObjectList);

	if (OptionalPoints){
		QList<::imtgql::CGqlParamObject> optionalPointsDataObjectList;
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < OptionalPoints->size(); ++optionalPointsIndex){
			::imtgql::CGqlParamObject newOptionalPointsGqlObject;
			if (!OptionalPoints->at(optionalPointsIndex)->WriteToGraphQlObject(newOptionalPointsGqlObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
			optionalPointsDataObjectList << newOptionalPointsGqlObject;
		}
		gqlObject.InsertParam("OptionalPoints", optionalPointsDataObjectList);
	}

	gqlObject.InsertParam("__typename", QVariant("Geometry"));

	return true;
}


bool CGeometry::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("GeometryType") || (gqlObject["GeometryType"].userType() != QMetaType::QString && gqlObject["GeometryType"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "GeometryType").toLocal8Bit().constData();)

		return false;
	}
	const QString geometryTypeStringValue = gqlObject["GeometryType"].toString();
	if(geometryTypeStringValue == "LINE"){
		GeometryType = GeometryType::LINE;
	}
	else if(geometryTypeStringValue == "POLYGON"){
		GeometryType = GeometryType::POLYGON;
	}
	else if(geometryTypeStringValue == "CIRCLE"){
		GeometryType = GeometryType::CIRCLE;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

		return false;
	}

	if (gqlObject.ContainsParam("Radius") && (gqlObject["Radius"].userType() == QMetaType::Float || gqlObject["Radius"].userType() == QMetaType::Double || gqlObject["Radius"].userType() == QMetaType::Int || gqlObject["Radius"].userType() == QMetaType::UInt || gqlObject["Radius"].userType() == QMetaType::LongLong || gqlObject["Radius"].userType() == QMetaType::ULongLong || gqlObject["Radius"].userType() == QMetaType::Long || gqlObject["Radius"].userType() == QMetaType::Short || gqlObject["Radius"].userType() == QMetaType::ULong || gqlObject["Radius"].userType() == QMetaType::UShort || gqlObject["Radius"].userType() == QMetaType::UChar)){
		Radius = gqlObject["Radius"].toDouble();
	}

	if (!gqlObject.ContainsParam("Points")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

		return false;
	}
	const qsizetype pointsElementsCount = gqlObject.GetObjectsCount("points");
	Points = imtsdl::TElementList<CPoint::V1_0>();
	for (qsizetype pointsIndex = 0; pointsIndex < pointsElementsCount; ++pointsIndex){
		const ::imtgql::CGqlParamObject* pointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Points", pointsIndex);
		if (pointsDataObjectPtr == nullptr){
			qDebug() << "invalid type" << pointsDataObjectPtr;
			return false;
		}
		CPoint::V1_0 tempPoints;
		if (!tempPoints.ReadFromGraphQlObject(*pointsDataObjectPtr)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

			return false;
		}
		Points->append(tempPoints);
	}

	if (!gqlObject.ContainsParam("RequiredPoints")){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

		return false;
	}
	const qsizetype requiredPointsElementsCount = gqlObject.GetObjectsCount("requiredPoints");
	if (requiredPointsElementsCount <= 0){
	I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' is empty").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

		return false;
	}
	RequiredPoints = imtsdl::TElementList<CPoint::V1_0>();
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < requiredPointsElementsCount; ++requiredPointsIndex){
		const ::imtgql::CGqlParamObject* requiredPointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("RequiredPoints", requiredPointsIndex);
		if (requiredPointsDataObjectPtr == nullptr){
			qDebug() << "invalid type" << requiredPointsDataObjectPtr;
			return false;
		}
		CPoint::V1_0 tempRequiredPoints;
		if (!tempRequiredPoints.ReadFromGraphQlObject(*requiredPointsDataObjectPtr)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		RequiredPoints->append(tempRequiredPoints);
	}

	if (gqlObject.ContainsParam("OptionalPoints")){
		OptionalPoints.emplace();
	}
	if (gqlObject.ContainsParam("OptionalPoints") && (gqlObject.GetObjectsCount("OptionalPoints") > 0)){
		const qsizetype optionalPointsElementsCount = gqlObject.GetObjectsCount("optionalPoints");
		OptionalPoints = imtsdl::TElementList<CPoint::V1_0>();
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < optionalPointsElementsCount; ++optionalPointsIndex){
			const ::imtgql::CGqlParamObject* optionalPointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("OptionalPoints", optionalPointsIndex);
			if (optionalPointsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << optionalPointsDataObjectPtr;
				return false;
			}
			CPoint::V1_0 tempOptionalPoints;
			if (!tempOptionalPoints.ReadFromGraphQlObject(*optionalPointsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
			OptionalPoints->append(tempOptionalPoints);
		}
	}

	return true;
}


bool CGeometry::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("GeometryType") && (gqlObject["GeometryType"].userType() == QMetaType::QString || gqlObject["GeometryType"].userType() == QMetaType::QByteArray)){
		const QString geometryTypeStringValue = gqlObject["GeometryType"].toString();
		if(geometryTypeStringValue == "LINE"){
			GeometryType = GeometryType::LINE;
		}
		else if(geometryTypeStringValue == "POLYGON"){
			GeometryType = GeometryType::POLYGON;
		}
		else if(geometryTypeStringValue == "CIRCLE"){
			GeometryType = GeometryType::CIRCLE;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("Radius") && (gqlObject["Radius"].userType() == QMetaType::Float || gqlObject["Radius"].userType() == QMetaType::Double || gqlObject["Radius"].userType() == QMetaType::Int || gqlObject["Radius"].userType() == QMetaType::UInt || gqlObject["Radius"].userType() == QMetaType::LongLong || gqlObject["Radius"].userType() == QMetaType::ULongLong || gqlObject["Radius"].userType() == QMetaType::Long || gqlObject["Radius"].userType() == QMetaType::Short || gqlObject["Radius"].userType() == QMetaType::ULong || gqlObject["Radius"].userType() == QMetaType::UShort || gqlObject["Radius"].userType() == QMetaType::UChar)){
		Radius = gqlObject["Radius"].toDouble();
	}

	if (gqlObject.ContainsParam("Points")){
		Points.emplace();
	}
	if (gqlObject.ContainsParam("Points") && (gqlObject.GetObjectsCount("Points") > 0)){
		const qsizetype pointsElementsCount = gqlObject.GetObjectsCount("points");
		Points = imtsdl::TElementList<CPoint::V1_0>();
		for (qsizetype pointsIndex = 0; pointsIndex < pointsElementsCount; ++pointsIndex){
			const ::imtgql::CGqlParamObject* pointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Points", pointsIndex);
			if (pointsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << pointsDataObjectPtr;
				return false;
			}
			CPoint::V1_0 tempPoints;
			if (!tempPoints.OptReadFromGraphQlObject(*pointsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

				return false;
			}
			Points->append(tempPoints);
		}
	}

	if (gqlObject.ContainsParam("RequiredPoints") && (gqlObject.GetObjectsCount("RequiredPoints") > 0)){
		const qsizetype requiredPointsElementsCount = gqlObject.GetObjectsCount("requiredPoints");
		if (requiredPointsElementsCount <= 0){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' is empty").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		RequiredPoints = imtsdl::TElementList<CPoint::V1_0>();
		for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < requiredPointsElementsCount; ++requiredPointsIndex){
			const ::imtgql::CGqlParamObject* requiredPointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("RequiredPoints", requiredPointsIndex);
			if (requiredPointsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << requiredPointsDataObjectPtr;
				return false;
			}
			CPoint::V1_0 tempRequiredPoints;
			if (!tempRequiredPoints.OptReadFromGraphQlObject(*requiredPointsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

				return false;
			}
			RequiredPoints->append(tempRequiredPoints);
		}
	}

	if (gqlObject.ContainsParam("OptionalPoints")){
		OptionalPoints.emplace();
	}
	if (gqlObject.ContainsParam("OptionalPoints") && (gqlObject.GetObjectsCount("OptionalPoints") > 0)){
		const qsizetype optionalPointsElementsCount = gqlObject.GetObjectsCount("optionalPoints");
		OptionalPoints = imtsdl::TElementList<CPoint::V1_0>();
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < optionalPointsElementsCount; ++optionalPointsIndex){
			const ::imtgql::CGqlParamObject* optionalPointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("OptionalPoints", optionalPointsIndex);
			if (optionalPointsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << optionalPointsDataObjectPtr;
				return false;
			}
			CPoint::V1_0 tempOptionalPoints;
			if (!tempOptionalPoints.OptReadFromGraphQlObject(*optionalPointsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
			OptionalPoints->append(tempOptionalPoints);
		}
	}

	return true;
}


bool CGeometry::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (!GeometryType){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "GeometryType").toLocal8Bit().constData();)

		return false;
	}
	QString geometryTypeStringValue;
	switch (*GeometryType){
	case GeometryType::LINE:
		geometryTypeStringValue = "LINE";
		break;
	case GeometryType::POLYGON:
		geometryTypeStringValue = "POLYGON";
		break;
	case GeometryType::CIRCLE:
		geometryTypeStringValue = "CIRCLE";
		break;
	default:
		Q_ASSERT(false);
		break;
	}
	jsonObject["GeometryType"] = QJsonValue::fromVariant(geometryTypeStringValue);

	if (Radius){
		jsonObject["Radius"] = QJsonValue::fromVariant(*Radius);
	}

	if (!Points){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

		return false;
	}
	QJsonArray newPointsArray;
	for (qsizetype pointsIndex = 0; pointsIndex < Points->size(); ++pointsIndex){
		QJsonObject newPointsJsonObject;
		if (!Points->at(pointsIndex)->WriteToJsonObject(newPointsJsonObject)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

			return false;
		}
		newPointsArray << newPointsJsonObject;
	}
	jsonObject["Points"] = newPointsArray;

	if (!RequiredPoints || RequiredPoints->isEmpty()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

		return false;
	}
	QJsonArray newRequiredPointsArray;
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < RequiredPoints->size(); ++requiredPointsIndex){
		QJsonObject newRequiredPointsJsonObject;
		if (!RequiredPoints->at(requiredPointsIndex)->WriteToJsonObject(newRequiredPointsJsonObject)){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		newRequiredPointsArray << newRequiredPointsJsonObject;
	}
	jsonObject["RequiredPoints"] = newRequiredPointsArray;

	if (OptionalPoints){
		QJsonArray newOptionalPointsArray;
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < OptionalPoints->size(); ++optionalPointsIndex){
			QJsonObject newOptionalPointsJsonObject;
			if (!OptionalPoints->at(optionalPointsIndex)->WriteToJsonObject(newOptionalPointsJsonObject)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
			newOptionalPointsArray << newOptionalPointsJsonObject;
		}
		jsonObject["OptionalPoints"] = newOptionalPointsArray;
	}

	jsonObject["__typename"] = "Geometry";

	return true;
}


bool CGeometry::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("GeometryType") || ! jsonObject["GeometryType"].isString()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "GeometryType").toLocal8Bit().constData();)

		return false;
	}
	const QString geometryTypeStringValue = jsonObject["GeometryType"].toString();
	if(geometryTypeStringValue == "LINE"){
		GeometryType = GeometryType::LINE;
	}
	else if(geometryTypeStringValue == "POLYGON"){
		GeometryType = GeometryType::POLYGON;
	}
	else if(geometryTypeStringValue == "CIRCLE"){
		GeometryType = GeometryType::CIRCLE;
	}
	else {
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

		return false;
	}

	if (jsonObject.contains("Radius") && jsonObject["Radius"].isDouble()){
		Radius = jsonObject["Radius"].toDouble();
	}

	if (!jsonObject.contains("Points") || ! jsonObject["Points"].isArray()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

		return false;
	}
	const QJsonArray pointsJsonArray = jsonObject["Points"].toArray();
	const qsizetype pointsArrayCount = pointsJsonArray.size();
	Points = imtsdl::TElementList<CPoint::V1_0>();
	for (qsizetype pointsIndex = 0; pointsIndex < pointsArrayCount; ++pointsIndex){
		CPoint::V1_0 tempPoints;
		if (!tempPoints.ReadFromJsonObject(pointsJsonArray[pointsIndex].toObject())){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

			return false;
		}
		Points->append(tempPoints);
	}

	if (!jsonObject.contains("RequiredPoints") || ! jsonObject["RequiredPoints"].isArray()){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

		return false;
	}
	const QJsonArray requiredPointsJsonArray = jsonObject["RequiredPoints"].toArray();
	const qsizetype requiredPointsArrayCount = requiredPointsJsonArray.size();
	if (requiredPointsArrayCount <= 0){
	I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' is empty").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

		return false;
	}
	RequiredPoints = imtsdl::TElementList<CPoint::V1_0>();
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < requiredPointsArrayCount; ++requiredPointsIndex){
		CPoint::V1_0 tempRequiredPoints;
		if (!tempRequiredPoints.ReadFromJsonObject(requiredPointsJsonArray[requiredPointsIndex].toObject())){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		RequiredPoints->append(tempRequiredPoints);
	}

	if (jsonObject.contains("OptionalPoints") && jsonObject["OptionalPoints"].isArray()){
		const QJsonArray optionalPointsJsonArray = jsonObject["OptionalPoints"].toArray();
		const qsizetype optionalPointsArrayCount = optionalPointsJsonArray.size();
		OptionalPoints = imtsdl::TElementList<CPoint::V1_0>();
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < optionalPointsArrayCount; ++optionalPointsIndex){
			CPoint::V1_0 tempOptionalPoints;
			if (!tempOptionalPoints.ReadFromJsonObject(optionalPointsJsonArray[optionalPointsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
			OptionalPoints->append(tempOptionalPoints);
		}
	}

	return true;
}


bool CGeometry::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("GeometryType") && jsonObject["GeometryType"].isString()){
		const QString geometryTypeStringValue = jsonObject["GeometryType"].toString();
		if(geometryTypeStringValue == "LINE"){
			GeometryType = GeometryType::LINE;
		}
		else if(geometryTypeStringValue == "POLYGON"){
			GeometryType = GeometryType::POLYGON;
		}
		else if(geometryTypeStringValue == "CIRCLE"){
			GeometryType = GeometryType::CIRCLE;
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

			return false;
		}
	}

	if (jsonObject.contains("Radius") && jsonObject["Radius"].isDouble()){
		Radius = jsonObject["Radius"].toDouble();
	}

	if (jsonObject.contains("Points") && jsonObject["Points"].isArray()){
		const QJsonArray pointsJsonArray = jsonObject["Points"].toArray();
		const qsizetype pointsArrayCount = pointsJsonArray.size();
		Points = imtsdl::TElementList<CPoint::V1_0>();
		for (qsizetype pointsIndex = 0; pointsIndex < pointsArrayCount; ++pointsIndex){
			CPoint::V1_0 tempPoints;
			if (!tempPoints.OptReadFromJsonObject(pointsJsonArray[pointsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

				return false;
			}
			Points->append(tempPoints);
		}
	}

	if (jsonObject.contains("RequiredPoints") && jsonObject["RequiredPoints"].isArray()){
		const QJsonArray requiredPointsJsonArray = jsonObject["RequiredPoints"].toArray();
		const qsizetype requiredPointsArrayCount = requiredPointsJsonArray.size();
		if (requiredPointsArrayCount <= 0){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' is empty").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		RequiredPoints = imtsdl::TElementList<CPoint::V1_0>();
		for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < requiredPointsArrayCount; ++requiredPointsIndex){
			CPoint::V1_0 tempRequiredPoints;
			if (!tempRequiredPoints.OptReadFromJsonObject(requiredPointsJsonArray[requiredPointsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

				return false;
			}
			RequiredPoints->append(tempRequiredPoints);
		}
	}

	if (jsonObject.contains("OptionalPoints") && jsonObject["OptionalPoints"].isArray()){
		const QJsonArray optionalPointsJsonArray = jsonObject["OptionalPoints"].toArray();
		const qsizetype optionalPointsArrayCount = optionalPointsJsonArray.size();
		OptionalPoints = imtsdl::TElementList<CPoint::V1_0>();
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < optionalPointsArrayCount; ++optionalPointsIndex){
			CPoint::V1_0 tempOptionalPoints;
			if (!tempOptionalPoints.OptReadFromJsonObject(optionalPointsJsonArray[optionalPointsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
			OptionalPoints->append(tempOptionalPoints);
		}
	}

	return true;
}


// serialize methods

bool CGeometry::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
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


bool CGeometry::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
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


bool CGeometry::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
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


bool CGeometry::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
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


bool CGeometry::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
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


bool CGeometry::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
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


bool CGeometry::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
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


bool CGeometry::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
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


bool CGeometry::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
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




CPointObject::CPointObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CPointObject::xChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPointObject::yChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPointObject::GetX()
{
	if (Version_1_0->X.has_value()){
		return Version_1_0->X.value();
	}

	return QVariant();
}


void CPointObject::SetX(const QVariant& v)
{
	Version_1_0->X = v.value<double>();
	xChanged();
}


bool CPointObject::hasX()
{
	 return Version_1_0->X.HasValue();
}


QVariant CPointObject::GetY()
{
	if (Version_1_0->Y.has_value()){
		return Version_1_0->Y.value();
	}

	return QVariant();
}


void CPointObject::SetY(const QVariant& v)
{
	Version_1_0->Y = v.value<double>();
	yChanged();
}


bool CPointObject::hasY()
{
	 return Version_1_0->Y.HasValue();
}


QString CPointObject::toJson() const
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


bool CPointObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CPointObject::fromObject(const QJsonObject& jsonObject)
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


QString CPointObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CPointObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	return QVariant();
}


QString CPointObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "x")){
		return "X";
	}
	if (propertyName == (QString("m_") + "y")){
		return "Y";
	}

	return propertyName;
}





bool CPointObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CPointObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CPointObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CPointObjectList::append(sdl::complextest::ComplexUnion1::CPointObject* item)
{
	BaseClass::append(item);
}


sdl::complextest::ComplexUnion1::CPointObjectList* sdl::complextest::ComplexUnion1::CPointObjectList::copyMe()
{
	sdl::complextest::ComplexUnion1::CPointObjectList* retVal = new sdl::complextest::ComplexUnion1::CPointObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::complextest::ComplexUnion1::CPointObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::complextest::ComplexUnion1::CPointObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::complextest::ComplexUnion1::CPointObjectList::addElement(sdl::complextest::ComplexUnion1::CPointObject* item)
{
	append(item);
}


void sdl::complextest::ComplexUnion1::CPointObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::complextest::ComplexUnion1::CPointObjectList::isEqualWithModel(sdl::complextest::ComplexUnion1::CPointObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::complextest::ComplexUnion1::CPointObjectList::insert(int index, sdl::complextest::ComplexUnion1::CPointObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::complextest::ComplexUnion1::CPointObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::complextest::ComplexUnion1::CPointObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::complextest::ComplexUnion1::CPointObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::complextest::ComplexUnion1::CPointObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_x"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->X.value());
		}
		if (nameId == "m_y"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->Y.value());
		}
	return QVariant();
}
CGeometryObject::CGeometryObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CGeometryObject::geometryTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGeometryObject::radiusChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGeometryObject::pointsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGeometryObject::requiredPointsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGeometryObject::optionalPointsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CGeometryObject::GetGeometryType()
{
	if (Version_1_0->GeometryType.has_value()){
		sdl::complextest::ComplexUnion1::GeometryType valueType = Version_1_0->GeometryType.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::ComplexUnion1::GeometryType>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QVariant();
}


void CGeometryObject::SetGeometryType(const QVariant& v)
{
	Version_1_0->GeometryType.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::complextest::ComplexUnion1::GeometryType>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		Version_1_0->GeometryType = (sdl::complextest::ComplexUnion1::GeometryType)key;
	}
	geometryTypeChanged();
}


bool CGeometryObject::hasGeometryType()
{
	 return Version_1_0->GeometryType.HasValue();
}


QVariant CGeometryObject::GetRadius()
{
	if (Version_1_0->Radius.has_value()){
		return Version_1_0->Radius.value();
	}

	return QVariant();
}


void CGeometryObject::SetRadius(const QVariant& v)
{
	Version_1_0->Radius = v.value<double>();
	radiusChanged();
}


bool CGeometryObject::hasRadius()
{
	 return Version_1_0->Radius.HasValue();
}


QVariant CGeometryObject::GetPoints()
{
	if (Version_1_0->Points.has_value()){
		if (!m_pointsQObjectPtr.isValid()){
			m_pointsQObjectPtr = CreateObject("Points");
			auto itemPtr = m_pointsQObjectPtr.value<sdl::complextest::ComplexUnion1::CPointObjectList*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->Points;
		}
		return m_pointsQObjectPtr;
	}

	return QVariant();
}


void CGeometryObject::SetPoints(const QVariant& v)
{
	if (v.isValid()){
		sdl::complextest::ComplexUnion1::CPointObjectList* itemPtr = v.value<sdl::complextest::ComplexUnion1::CPointObjectList*>();
		if (itemPtr != nullptr)  Version_1_0->Points = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->Points = nullptr;
	}
	m_pointsQObjectPtr = v;

	pointsChanged();
}


bool CGeometryObject::hasPoints()
{
	 return Version_1_0->Points.HasValue();
}


void CGeometryObject::emplacePoints()
{
	Version_1_0->Points.emplace();

}


void CGeometryObject::ResetPoints()
{
	auto* itemPtr = m_pointsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_pointsQObjectPtr = QVariant();
}


QVariant CGeometryObject::createPointsArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::complextest::ComplexUnion1::CPointObject());
}


QVariant CGeometryObject::GetRequiredPoints()
{
	if (Version_1_0->RequiredPoints.has_value()){
		if (!m_requiredPointsQObjectPtr.isValid()){
			m_requiredPointsQObjectPtr = CreateObject("RequiredPoints");
			auto itemPtr = m_requiredPointsQObjectPtr.value<sdl::complextest::ComplexUnion1::CPointObjectList*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->RequiredPoints;
		}
		return m_requiredPointsQObjectPtr;
	}

	return QVariant();
}


void CGeometryObject::SetRequiredPoints(const QVariant& v)
{
	if (v.isValid()){
		sdl::complextest::ComplexUnion1::CPointObjectList* itemPtr = v.value<sdl::complextest::ComplexUnion1::CPointObjectList*>();
		if (itemPtr != nullptr)  Version_1_0->RequiredPoints = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->RequiredPoints = nullptr;
	}
	m_requiredPointsQObjectPtr = v;

	requiredPointsChanged();
}


bool CGeometryObject::hasRequiredPoints()
{
	 return Version_1_0->RequiredPoints.HasValue();
}


void CGeometryObject::emplaceRequiredPoints()
{
	Version_1_0->RequiredPoints.emplace();

}


void CGeometryObject::ResetRequiredPoints()
{
	auto* itemPtr = m_requiredPointsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_requiredPointsQObjectPtr = QVariant();
}


QVariant CGeometryObject::createRequiredPointsArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::complextest::ComplexUnion1::CPointObject());
}


QVariant CGeometryObject::GetOptionalPoints()
{
	if (Version_1_0->OptionalPoints.has_value()){
		if (!m_optionalPointsQObjectPtr.isValid()){
			m_optionalPointsQObjectPtr = CreateObject("OptionalPoints");
			auto itemPtr = m_optionalPointsQObjectPtr.value<sdl::complextest::ComplexUnion1::CPointObjectList*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->OptionalPoints;
		}
		return m_optionalPointsQObjectPtr;
	}

	return QVariant();
}


void CGeometryObject::SetOptionalPoints(const QVariant& v)
{
	if (v.isValid()){
		sdl::complextest::ComplexUnion1::CPointObjectList* itemPtr = v.value<sdl::complextest::ComplexUnion1::CPointObjectList*>();
		if (itemPtr != nullptr)  Version_1_0->OptionalPoints = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->OptionalPoints = nullptr;
	}
	m_optionalPointsQObjectPtr = v;

	optionalPointsChanged();
}


bool CGeometryObject::hasOptionalPoints()
{
	 return Version_1_0->OptionalPoints.HasValue();
}


void CGeometryObject::emplaceOptionalPoints()
{
	Version_1_0->OptionalPoints.emplace();

}


void CGeometryObject::ResetOptionalPoints()
{
	auto* itemPtr = m_optionalPointsQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_optionalPointsQObjectPtr = QVariant();
}


QVariant CGeometryObject::createOptionalPointsArrayElement(const QVariant& v)
{
	Q_UNUSED(v);
	return QVariant::fromValue(new sdl::complextest::ComplexUnion1::CPointObject());
}


QString CGeometryObject::toJson() const
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


bool CGeometryObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CGeometryObject::fromObject(const QJsonObject& jsonObject)
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


QString CGeometryObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CGeometryObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "Points"){
		return QVariant::fromValue(new sdl::complextest::ComplexUnion1::CPointObjectList(this));
	}
	if (key == "RequiredPoints"){
		return QVariant::fromValue(new sdl::complextest::ComplexUnion1::CPointObjectList(this));
	}
	if (key == "OptionalPoints"){
		return QVariant::fromValue(new sdl::complextest::ComplexUnion1::CPointObjectList(this));
	}
	return QVariant();
}


QString CGeometryObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "geometryType")){
		return "GeometryType";
	}
	if (propertyName == (QString("m_") + "radius")){
		return "Radius";
	}
	if (propertyName == (QString("m_") + "points")){
		return "Points";
	}
	if (propertyName == (QString("m_") + "requiredPoints")){
		return "RequiredPoints";
	}
	if (propertyName == (QString("m_") + "optionalPoints")){
		return "OptionalPoints";
	}

	return propertyName;
}





bool CGeometryObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CGeometryObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CGeometryObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CGeometryObjectList::append(sdl::complextest::ComplexUnion1::CGeometryObject* item)
{
	BaseClass::append(item);
}


sdl::complextest::ComplexUnion1::CGeometryObjectList* sdl::complextest::ComplexUnion1::CGeometryObjectList::copyMe()
{
	sdl::complextest::ComplexUnion1::CGeometryObjectList* retVal = new sdl::complextest::ComplexUnion1::CGeometryObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::complextest::ComplexUnion1::CGeometryObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::complextest::ComplexUnion1::CGeometryObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::complextest::ComplexUnion1::CGeometryObjectList::addElement(sdl::complextest::ComplexUnion1::CGeometryObject* item)
{
	append(item);
}


void sdl::complextest::ComplexUnion1::CGeometryObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::complextest::ComplexUnion1::CGeometryObjectList::isEqualWithModel(sdl::complextest::ComplexUnion1::CGeometryObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::complextest::ComplexUnion1::CGeometryObjectList::insert(int index, sdl::complextest::ComplexUnion1::CGeometryObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::complextest::ComplexUnion1::CGeometryObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::complextest::ComplexUnion1::CGeometryObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::complextest::ComplexUnion1::CGeometryObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::complextest::ComplexUnion1::CGeometryObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_geometryType"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->GeometryType.value());
		}
		if (nameId == "m_radius"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->Radius.value());
		}
		if (nameId == "m_points"){
			return itemPtr->GetPoints();
		}
		if (nameId == "m_requiredPoints"){
			return itemPtr->GetRequiredPoints();
		}
		if (nameId == "m_optionalPoints"){
			return itemPtr->GetOptionalPoints();
		}
	return QVariant();
}
} // namespace sdl::complextest::ComplexUnion1
