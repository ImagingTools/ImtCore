#include "ComplexUnion1.h"


namespace sdl::V1_0::complextest
{


QByteArray CPoint::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CPoint::operator==(const CPoint& other) const
{
	return 
				X.has_value() == other.X.has_value() &&
				((X.has_value() && other.X.has_value()) ?
					qFuzzyCompare(*X, *other.X) : true) &&
				Y.has_value() == other.Y.has_value() &&
				((Y.has_value() && other.Y.has_value()) ?
					qFuzzyCompare(*Y, *other.Y) : true);
}


bool V1_0::complextest::CPoint::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool V1_0::complextest::CPoint::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool V1_0::complextest::CPoint::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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


bool V1_0::complextest::CPoint::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool V1_0::complextest::CPoint::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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


bool V1_0::complextest::CPoint::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("X") && (gqlObject["X"].userType() == QMetaType::Float || gqlObject["X"].userType() == QMetaType::Double || gqlObject["X"].userType() == QMetaType::Int || gqlObject["X"].userType() == QMetaType::UInt || gqlObject["X"].userType() == QMetaType::LongLong || gqlObject["X"].userType() == QMetaType::ULongLong || gqlObject["X"].userType() == QMetaType::Long || gqlObject["X"].userType() == QMetaType::Short || gqlObject["X"].userType() == QMetaType::ULong || gqlObject["X"].userType() == QMetaType::UShort || gqlObject["X"].userType() == QMetaType::UChar)){
		X = gqlObject["X"].toDouble();
	}

	if (gqlObject.ContainsParam("Y") && (gqlObject["Y"].userType() == QMetaType::Float || gqlObject["Y"].userType() == QMetaType::Double || gqlObject["Y"].userType() == QMetaType::Int || gqlObject["Y"].userType() == QMetaType::UInt || gqlObject["Y"].userType() == QMetaType::LongLong || gqlObject["Y"].userType() == QMetaType::ULongLong || gqlObject["Y"].userType() == QMetaType::Long || gqlObject["Y"].userType() == QMetaType::Short || gqlObject["Y"].userType() == QMetaType::ULong || gqlObject["Y"].userType() == QMetaType::UShort || gqlObject["Y"].userType() == QMetaType::UChar)){
		Y = gqlObject["Y"].toDouble();
	}

	return true;
}


bool V1_0::complextest::CPoint::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool V1_0::complextest::CPoint::ReadFromJsonObject(const QJsonObject& jsonObject)
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


bool V1_0::complextest::CPoint::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("X") && jsonObject["X"].isDouble()){
		X = jsonObject["X"].toDouble();
	}

	if (jsonObject.contains("Y") && jsonObject["Y"].isDouble()){
		Y = jsonObject["Y"].toDouble();
	}

	return true;
}




QByteArray CGeometry::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CGeometry::operator==(const CGeometry& other) const
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


bool V1_0::complextest::CGeometry::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool V1_0::complextest::CGeometry::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
	imtsdl::TElementList<V1_0::complextest::CPoint> pointsList;
	for (int pointsIndex = 0; pointsIndex < pointsCount; ++pointsIndex){
		V1_0::complextest::CPoint t_points;
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
	imtsdl::TElementList<V1_0::complextest::CPoint> requiredPointsList;
	for (int requiredPointsIndex = 0; requiredPointsIndex < requiredPointsCount; ++requiredPointsIndex){
		V1_0::complextest::CPoint t_requiredPoints;
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
		imtsdl::TElementList<V1_0::complextest::CPoint> optionalPointsList;
		for (int optionalPointsIndex = 0; optionalPointsIndex < optionalPointsCount; ++optionalPointsIndex){
			V1_0::complextest::CPoint t_optionalPoints;
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


bool V1_0::complextest::CGeometry::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
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
		imtsdl::TElementList<V1_0::complextest::CPoint> pointsList;
		for (int pointsIndex = 0; pointsIndex < pointsCount; ++pointsIndex){
			V1_0::complextest::CPoint t_points;
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
		imtsdl::TElementList<V1_0::complextest::CPoint> requiredPointsList;
		for (int requiredPointsIndex = 0; requiredPointsIndex < requiredPointsCount; ++requiredPointsIndex){
			V1_0::complextest::CPoint t_requiredPoints;
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
		imtsdl::TElementList<V1_0::complextest::CPoint> optionalPointsList;
		for (int optionalPointsIndex = 0; optionalPointsIndex < optionalPointsCount; ++optionalPointsIndex){
			V1_0::complextest::CPoint t_optionalPoints;
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


bool V1_0::complextest::CGeometry::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool V1_0::complextest::CGeometry::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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
	const qsizetype pointsElementsCount = gqlObject.GetObjectsCount("Points");
	Points = imtsdl::TElementList<V1_0::complextest::CPoint>();
	for (qsizetype pointsIndex = 0; pointsIndex < pointsElementsCount; ++pointsIndex){
		const ::imtgql::CGqlParamObject* pointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Points", pointsIndex);
		if (pointsDataObjectPtr == nullptr){
			qDebug() << "invalid type" << pointsDataObjectPtr;
			return false;
		}
		V1_0::complextest::CPoint tempPoints;
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
	const qsizetype requiredPointsElementsCount = gqlObject.GetObjectsCount("RequiredPoints");
	if (requiredPointsElementsCount <= 0){
	I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' is empty").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

		return false;
	}
	RequiredPoints = imtsdl::TElementList<V1_0::complextest::CPoint>();
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < requiredPointsElementsCount; ++requiredPointsIndex){
		const ::imtgql::CGqlParamObject* requiredPointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("RequiredPoints", requiredPointsIndex);
		if (requiredPointsDataObjectPtr == nullptr){
			qDebug() << "invalid type" << requiredPointsDataObjectPtr;
			return false;
		}
		V1_0::complextest::CPoint tempRequiredPoints;
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
		const qsizetype optionalPointsElementsCount = gqlObject.GetObjectsCount("OptionalPoints");
		OptionalPoints = imtsdl::TElementList<V1_0::complextest::CPoint>();
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < optionalPointsElementsCount; ++optionalPointsIndex){
			const ::imtgql::CGqlParamObject* optionalPointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("OptionalPoints", optionalPointsIndex);
			if (optionalPointsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << optionalPointsDataObjectPtr;
				return false;
			}
			V1_0::complextest::CPoint tempOptionalPoints;
			if (!tempOptionalPoints.ReadFromGraphQlObject(*optionalPointsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
			OptionalPoints->append(tempOptionalPoints);
		}
	}

	return true;
}


bool V1_0::complextest::CGeometry::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
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
		const qsizetype pointsElementsCount = gqlObject.GetObjectsCount("Points");
		Points = imtsdl::TElementList<V1_0::complextest::CPoint>();
		for (qsizetype pointsIndex = 0; pointsIndex < pointsElementsCount; ++pointsIndex){
			const ::imtgql::CGqlParamObject* pointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Points", pointsIndex);
			if (pointsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << pointsDataObjectPtr;
				return false;
			}
			V1_0::complextest::CPoint tempPoints;
			if (!tempPoints.OptReadFromGraphQlObject(*pointsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

				return false;
			}
			Points->append(tempPoints);
		}
	}

	if (gqlObject.ContainsParam("RequiredPoints") && (gqlObject.GetObjectsCount("RequiredPoints") > 0)){
		const qsizetype requiredPointsElementsCount = gqlObject.GetObjectsCount("RequiredPoints");
		if (requiredPointsElementsCount <= 0){
		I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Field: '%3' is empty").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		RequiredPoints = imtsdl::TElementList<V1_0::complextest::CPoint>();
		for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < requiredPointsElementsCount; ++requiredPointsIndex){
			const ::imtgql::CGqlParamObject* requiredPointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("RequiredPoints", requiredPointsIndex);
			if (requiredPointsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << requiredPointsDataObjectPtr;
				return false;
			}
			V1_0::complextest::CPoint tempRequiredPoints;
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
		const qsizetype optionalPointsElementsCount = gqlObject.GetObjectsCount("OptionalPoints");
		OptionalPoints = imtsdl::TElementList<V1_0::complextest::CPoint>();
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < optionalPointsElementsCount; ++optionalPointsIndex){
			const ::imtgql::CGqlParamObject* optionalPointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("OptionalPoints", optionalPointsIndex);
			if (optionalPointsDataObjectPtr == nullptr){
				qDebug() << "invalid type" << optionalPointsDataObjectPtr;
				return false;
			}
			V1_0::complextest::CPoint tempOptionalPoints;
			if (!tempOptionalPoints.OptReadFromGraphQlObject(*optionalPointsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
			OptionalPoints->append(tempOptionalPoints);
		}
	}

	return true;
}


bool V1_0::complextest::CGeometry::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool V1_0::complextest::CGeometry::ReadFromJsonObject(const QJsonObject& jsonObject)
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
	Points = imtsdl::TElementList<V1_0::complextest::CPoint>();
	for (qsizetype pointsIndex = 0; pointsIndex < pointsArrayCount; ++pointsIndex){
		V1_0::complextest::CPoint tempPoints;
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
	RequiredPoints = imtsdl::TElementList<V1_0::complextest::CPoint>();
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < requiredPointsArrayCount; ++requiredPointsIndex){
		V1_0::complextest::CPoint tempRequiredPoints;
		if (!tempRequiredPoints.ReadFromJsonObject(requiredPointsJsonArray[requiredPointsIndex].toObject())){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		RequiredPoints->append(tempRequiredPoints);
	}

	if (jsonObject.contains("OptionalPoints") && jsonObject["OptionalPoints"].isArray()){
		const QJsonArray optionalPointsJsonArray = jsonObject["OptionalPoints"].toArray();
		const qsizetype optionalPointsArrayCount = optionalPointsJsonArray.size();
		OptionalPoints = imtsdl::TElementList<V1_0::complextest::CPoint>();
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < optionalPointsArrayCount; ++optionalPointsIndex){
			V1_0::complextest::CPoint tempOptionalPoints;
			if (!tempOptionalPoints.ReadFromJsonObject(optionalPointsJsonArray[optionalPointsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
			OptionalPoints->append(tempOptionalPoints);
		}
	}

	return true;
}


bool V1_0::complextest::CGeometry::OptReadFromJsonObject(const QJsonObject& jsonObject)
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
		Points = imtsdl::TElementList<V1_0::complextest::CPoint>();
		for (qsizetype pointsIndex = 0; pointsIndex < pointsArrayCount; ++pointsIndex){
			V1_0::complextest::CPoint tempPoints;
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
		RequiredPoints = imtsdl::TElementList<V1_0::complextest::CPoint>();
		for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < requiredPointsArrayCount; ++requiredPointsIndex){
			V1_0::complextest::CPoint tempRequiredPoints;
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
		OptionalPoints = imtsdl::TElementList<V1_0::complextest::CPoint>();
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < optionalPointsArrayCount; ++optionalPointsIndex){
			V1_0::complextest::CPoint tempOptionalPoints;
			if (!tempOptionalPoints.OptReadFromJsonObject(optionalPointsJsonArray[optionalPointsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
			OptionalPoints->append(tempOptionalPoints);
		}
	}

	return true;
}




CPointObject::CPointObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CPointObject::xChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CPointObject::yChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CPointObject::GetX()
{
	return CPoint::X ? CPoint::X.value() : QVariant();
}


void CPointObject::SetX(const QVariant& v)
{
	CPoint::X = v.value<double>();
	xChanged();
}


bool CPointObject::hasX()
{
	 return CPoint::X.HasValue();
}


QVariant CPointObject::GetY()
{
	return CPoint::Y ? CPoint::Y.value() : QVariant();
}


void CPointObject::SetY(const QVariant& v)
{
	CPoint::Y = v.value<double>();
	yChanged();
}


bool CPointObject::hasY()
{
	 return CPoint::Y.HasValue();
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


void CPointObjectList::append(sdl::V1_0::complextest::CPointObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CPointObjectList* sdl::V1_0::complextest::CPointObjectList::copyMe()
{
	sdl::V1_0::complextest::CPointObjectList* retVal = new sdl::V1_0::complextest::CPointObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CPointObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CPointObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CPointObjectList::addElement(sdl::V1_0::complextest::CPointObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CPointObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CPointObjectList::isEqualWithModel(sdl::V1_0::complextest::CPointObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CPointObjectList::insert(int index, sdl::V1_0::complextest::CPointObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CPointObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CPointObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CPointObjectList::getData(const QString& nameId, int index) const
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CPointObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_x"){
		return Version_1_0->at(index)->X ? QVariant::fromValue(Version_1_0->at(index)->X.value()) : QVariant();
	}
	if (nameId == "m_y"){
		return Version_1_0->at(index)->Y ? QVariant::fromValue(Version_1_0->at(index)->Y.value()) : QVariant();
	}

	return QVariant();
}
CGeometryObject::CGeometryObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CGeometryObject::geometryTypeChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGeometryObject::radiusChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGeometryObject::pointsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGeometryObject::requiredPointsChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CGeometryObject::optionalPointsChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CGeometryObject::GetGeometryType()
{
	if (!GeometryType) {return {};}
	sdl::V1_0::complextest::GeometryType valueType = CGeometry::GeometryType.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::GeometryType>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CGeometryObject::SetGeometryType(const QVariant& v)
{
	CGeometry::GeometryType.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::complextest::GeometryType>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		CGeometry::GeometryType = (sdl::V1_0::complextest::GeometryType)key;
	}
	geometryTypeChanged();
}


bool CGeometryObject::hasGeometryType()
{
	 return CGeometry::GeometryType.HasValue();
}


QVariant CGeometryObject::GetRadius()
{
	return CGeometry::Radius ? CGeometry::Radius.value() : QVariant();
}


void CGeometryObject::SetRadius(const QVariant& v)
{
	CGeometry::Radius = v.value<double>();
	radiusChanged();
}


bool CGeometryObject::hasRadius()
{
	 return CGeometry::Radius.HasValue();
}


QVariant CGeometryObject::GetPoints()
{
	if (!m_pointsQObjectPtr.isValid()){
		m_pointsQObjectPtr = CreateObject("Points");
		auto itemPtr = m_pointsQObjectPtr.value<sdl::V1_0::complextest::CPointObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CGeometry::Points;
	}
	return m_pointsQObjectPtr;
}


void CGeometryObject::SetPoints(const QVariant& v)
{
	if (v.isValid()){
		sdl::V1_0::complextest::CPointObjectList* itemPtr = v.value<sdl::V1_0::complextest::CPointObjectList*>();
		if (itemPtr != nullptr) CGeometry::Points = itemPtr->Version_1_0;
	}
	else {
		CGeometry::Points = nullptr;
	}
	m_pointsQObjectPtr = v;

	pointsChanged();
}


bool CGeometryObject::hasPoints()
{
	 return CGeometry::Points.HasValue();
}


void CGeometryObject::emplacePoints()
{
	ResetPoints();
	CGeometry::Points.emplace();
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
	return QVariant::fromValue(new sdl::V1_0::complextest::CPointObject());
}


QVariant CGeometryObject::GetRequiredPoints()
{
	if (!m_requiredPointsQObjectPtr.isValid()){
		m_requiredPointsQObjectPtr = CreateObject("RequiredPoints");
		auto itemPtr = m_requiredPointsQObjectPtr.value<sdl::V1_0::complextest::CPointObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CGeometry::RequiredPoints;
	}
	return m_requiredPointsQObjectPtr;
}


void CGeometryObject::SetRequiredPoints(const QVariant& v)
{
	if (v.isValid()){
		sdl::V1_0::complextest::CPointObjectList* itemPtr = v.value<sdl::V1_0::complextest::CPointObjectList*>();
		if (itemPtr != nullptr) CGeometry::RequiredPoints = itemPtr->Version_1_0;
	}
	else {
		CGeometry::RequiredPoints = nullptr;
	}
	m_requiredPointsQObjectPtr = v;

	requiredPointsChanged();
}


bool CGeometryObject::hasRequiredPoints()
{
	 return CGeometry::RequiredPoints.HasValue();
}


void CGeometryObject::emplaceRequiredPoints()
{
	ResetRequiredPoints();
	CGeometry::RequiredPoints.emplace();
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
	return QVariant::fromValue(new sdl::V1_0::complextest::CPointObject());
}


QVariant CGeometryObject::GetOptionalPoints()
{
	if (!m_optionalPointsQObjectPtr.isValid()){
		m_optionalPointsQObjectPtr = CreateObject("OptionalPoints");
		auto itemPtr = m_optionalPointsQObjectPtr.value<sdl::V1_0::complextest::CPointObjectList*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CGeometry::OptionalPoints;
	}
	return m_optionalPointsQObjectPtr;
}


void CGeometryObject::SetOptionalPoints(const QVariant& v)
{
	if (v.isValid()){
		sdl::V1_0::complextest::CPointObjectList* itemPtr = v.value<sdl::V1_0::complextest::CPointObjectList*>();
		if (itemPtr != nullptr) CGeometry::OptionalPoints = itemPtr->Version_1_0;
	}
	else {
		CGeometry::OptionalPoints = nullptr;
	}
	m_optionalPointsQObjectPtr = v;

	optionalPointsChanged();
}


bool CGeometryObject::hasOptionalPoints()
{
	 return CGeometry::OptionalPoints.HasValue();
}


void CGeometryObject::emplaceOptionalPoints()
{
	ResetOptionalPoints();
	CGeometry::OptionalPoints.emplace();
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
	return QVariant::fromValue(new sdl::V1_0::complextest::CPointObject());
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
		return QVariant::fromValue(new sdl::V1_0::complextest::CPointObjectList(this));
	}
	if (key == "RequiredPoints"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CPointObjectList(this));
	}
	if (key == "OptionalPoints"){
		return QVariant::fromValue(new sdl::V1_0::complextest::CPointObjectList(this));
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


void CGeometryObjectList::append(sdl::V1_0::complextest::CGeometryObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::complextest::CGeometryObjectList* sdl::V1_0::complextest::CGeometryObjectList::copyMe()
{
	sdl::V1_0::complextest::CGeometryObjectList* retVal = new sdl::V1_0::complextest::CGeometryObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::complextest::CGeometryObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::complextest::CGeometryObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::complextest::CGeometryObjectList::addElement(sdl::V1_0::complextest::CGeometryObject* item)
{
	append(item);
}


void sdl::V1_0::complextest::CGeometryObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::complextest::CGeometryObjectList::isEqualWithModel(sdl::V1_0::complextest::CGeometryObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::complextest::CGeometryObjectList::insert(int index, sdl::V1_0::complextest::CGeometryObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::complextest::CGeometryObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::complextest::CGeometryObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::complextest::CGeometryObjectList::getData(const QString& nameId, int index) const
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::complextest::CGeometryObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_geometryType"){
		return Version_1_0->at(index)->GeometryType ? QVariant::fromValue(Version_1_0->at(index)->GeometryType.value()) : QVariant();
	}
	if (nameId == "m_radius"){
		return Version_1_0->at(index)->Radius ? QVariant::fromValue(Version_1_0->at(index)->Radius.value()) : QVariant();
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
} // namespace sdl::V1_0::complextest
