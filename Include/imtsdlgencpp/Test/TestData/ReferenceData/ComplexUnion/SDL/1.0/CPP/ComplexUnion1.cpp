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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("X", *X, modelIndex);

	if (!Y){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

		return false;
	}
	model.SetData("Y", *Y, modelIndex);


	return true;
}


bool V1_0::complextest::CPoint::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant xData = model.GetData("X", modelIndex);
	if (xData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	X = xData.toFloat();

	QVariant yData = model.GetData("Y", modelIndex);
	if (yData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("X", QVariant(*X));

	if (!Y){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

		return false;
	}
	gqlObject.InsertParam("Y", QVariant(*Y));

	gqlObject.InsertParam("__typename", QVariant("Point"));

	return true;
}


bool V1_0::complextest::CPoint::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("X") || (gqlObject["X"].userType() != QMetaType::Float && gqlObject["X"].userType() != QMetaType::Double && gqlObject["X"].userType() != QMetaType::Int && gqlObject["X"].userType() != QMetaType::UInt && gqlObject["X"].userType() != QMetaType::LongLong && gqlObject["X"].userType() != QMetaType::ULongLong && gqlObject["X"].userType() != QMetaType::Long && gqlObject["X"].userType() != QMetaType::Short && gqlObject["X"].userType() != QMetaType::ULong && gqlObject["X"].userType() != QMetaType::UShort && gqlObject["X"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	X = gqlObject["X"].toDouble();

	if (!gqlObject.ContainsParam("Y") || (gqlObject["Y"].userType() != QMetaType::Float && gqlObject["Y"].userType() != QMetaType::Double && gqlObject["Y"].userType() != QMetaType::Int && gqlObject["Y"].userType() != QMetaType::UInt && gqlObject["Y"].userType() != QMetaType::LongLong && gqlObject["Y"].userType() != QMetaType::ULongLong && gqlObject["Y"].userType() != QMetaType::Long && gqlObject["Y"].userType() != QMetaType::Short && gqlObject["Y"].userType() != QMetaType::ULong && gqlObject["Y"].userType() != QMetaType::UShort && gqlObject["Y"].userType() != QMetaType::UChar)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["X"] = QJsonValue::fromVariant(*X);

	if (!Y){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

		return false;
	}
	jsonObject["Y"] = QJsonValue::fromVariant(*Y);

	jsonObject["__typename"] = "Point";

	return true;
}


bool V1_0::complextest::CPoint::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("X") || ! jsonObject["X"].isDouble()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "X").toLocal8Bit().constData();)

		return false;
	}
	X = jsonObject["X"].toDouble();

	if (!jsonObject.contains("Y") || ! jsonObject["Y"].isDouble()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Y").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "GeometryType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

		return false;
	}
	::imtbase::CTreeItemModel* newPointsModelPtr = model.AddTreeModel("Points", modelIndex);
	newPointsModelPtr->setIsArray(true);
	for (qsizetype pointsIndex = 0; pointsIndex < Points->size(); ++pointsIndex){
		if (!Points->at(pointsIndex)){
			newPointsModelPtr->InsertNewItem();
			newPointsModelPtr->SetData(QByteArray(), QVariant(), pointsIndex);
			continue;
		}
		newPointsModelPtr->InsertNewItem();
		if (!(Points->at(pointsIndex)->WriteToModel(*newPointsModelPtr, pointsIndex))){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

			return false;
		}
	}
	if (!RequiredPoints){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

		return false;
	}
	::imtbase::CTreeItemModel* newRequiredPointsModelPtr = model.AddTreeModel("RequiredPoints", modelIndex);
	newRequiredPointsModelPtr->setIsArray(true);
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < RequiredPoints->size(); ++requiredPointsIndex){
		if (!RequiredPoints->at(requiredPointsIndex)){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Array field '%3' contains a null element").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		newRequiredPointsModelPtr->InsertNewItem();
		if (!(RequiredPoints->at(requiredPointsIndex)->WriteToModel(*newRequiredPointsModelPtr, requiredPointsIndex))){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
	}
	if (OptionalPoints){
		::imtbase::CTreeItemModel* newOptionalPointsModelPtr = model.AddTreeModel("OptionalPoints", modelIndex);
		newOptionalPointsModelPtr->setIsArray(true);
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < OptionalPoints->size(); ++optionalPointsIndex){
			if (!OptionalPoints->at(optionalPointsIndex)){
				newOptionalPointsModelPtr->InsertNewItem();
				newOptionalPointsModelPtr->SetData(QByteArray(), QVariant(), optionalPointsIndex);
				continue;
			}
			newOptionalPointsModelPtr->InsertNewItem();
			if (!(OptionalPoints->at(optionalPointsIndex)->WriteToModel(*newOptionalPointsModelPtr, optionalPointsIndex))){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
		}
	}
	else if (OptionalPoints.IsNull()){
		model.SetData("OptionalPoints", QVariant(), modelIndex);
	}

	return true;
}


bool V1_0::complextest::CGeometry::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	QVariant geometryTypeData = model.GetData("GeometryType", modelIndex);
	if (geometryTypeData.isNull()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' not exists, but required").arg(__FILE__, QString::number(__LINE__), "GeometryType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

		return false;
	}

	QVariant radiusData = model.GetData("Radius", modelIndex);
	if (!radiusData.isNull()){
		Radius = radiusData.toFloat();
	}

	::imtbase::CTreeItemModel* pointsModel = model.GetTreeItemModel("Points", modelIndex);
	if (pointsModel == nullptr){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' is missing, but required.").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

		return false;
	}
	int pointsCount = pointsModel->GetItemsCount();
	imtsdl::TElementList<V1_0::complextest::CPoint> pointsList;
	for (int pointsIndex = 0; pointsIndex < pointsCount; ++pointsIndex){
		if (pointsModel->ContainsKey(QByteArray(), pointsIndex) && !pointsModel->GetData(QByteArray(), pointsIndex).isValid()){
			pointsList.AppendNull();
			continue;
		}
		V1_0::complextest::CPoint t_points;
		if (!t_points.ReadFromModel(*pointsModel, pointsIndex)){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

			return false;
		}
		pointsList << t_points;
	}
	Points = pointsList;


	::imtbase::CTreeItemModel* requiredPointsModel = model.GetTreeItemModel("RequiredPoints", modelIndex);
	if (requiredPointsModel == nullptr){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field '%3' is missing, but required.").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

		return false;
	}
	int requiredPointsCount = requiredPointsModel->GetItemsCount();
	imtsdl::TElementList<V1_0::complextest::CPoint> requiredPointsList;
	for (int requiredPointsIndex = 0; requiredPointsIndex < requiredPointsCount; ++requiredPointsIndex){
		if (requiredPointsModel->ContainsKey(QByteArray(), requiredPointsIndex) && !requiredPointsModel->GetData(QByteArray(), requiredPointsIndex).isValid()){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Array field '%3' contains a null element").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		V1_0::complextest::CPoint t_requiredPoints;
		if (!t_requiredPoints.ReadFromModel(*requiredPointsModel, requiredPointsIndex)){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		requiredPointsList << t_requiredPoints;
	}
	RequiredPoints = requiredPointsList;


	::imtbase::CTreeItemModel* optionalPointsModel = model.GetTreeItemModel("OptionalPoints", modelIndex);
	if (model.ContainsKey("OptionalPoints", modelIndex) && optionalPointsModel == nullptr){
		OptionalPoints.SetNull();
	}
	if (optionalPointsModel != nullptr){
		int optionalPointsCount = optionalPointsModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::CPoint> optionalPointsList;
		for (int optionalPointsIndex = 0; optionalPointsIndex < optionalPointsCount; ++optionalPointsIndex){
			if (optionalPointsModel->ContainsKey(QByteArray(), optionalPointsIndex) && !optionalPointsModel->GetData(QByteArray(), optionalPointsIndex).isValid()){
				optionalPointsList.AppendNull();
				continue;
			}
			V1_0::complextest::CPoint t_optionalPoints;
			if (!t_optionalPoints.ReadFromModel(*optionalPointsModel, optionalPointsIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

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
			if (pointsModel->ContainsKey(QByteArray(), pointsIndex) && !pointsModel->GetData(QByteArray(), pointsIndex).isValid()){
				pointsList.AppendNull();
				continue;
			}
			V1_0::complextest::CPoint t_points;
			if (!t_points.OptReadFromModel(*pointsModel, pointsIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

				return false;
			}
			pointsList << t_points;
		}
		Points = pointsList;

	}

	::imtbase::CTreeItemModel* requiredPointsModel = model.GetTreeItemModel("RequiredPoints", modelIndex);
	if (requiredPointsModel != nullptr){
		int requiredPointsCount = requiredPointsModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::CPoint> requiredPointsList;
		for (int requiredPointsIndex = 0; requiredPointsIndex < requiredPointsCount; ++requiredPointsIndex){
			if (requiredPointsModel->ContainsKey(QByteArray(), requiredPointsIndex) && !requiredPointsModel->GetData(QByteArray(), requiredPointsIndex).isValid()){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Array field '%3' contains a null element").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

				return false;
			}
			V1_0::complextest::CPoint t_requiredPoints;
			if (!t_requiredPoints.OptReadFromModel(*requiredPointsModel, requiredPointsIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

				return false;
			}
			requiredPointsList << t_requiredPoints;
		}
		RequiredPoints = requiredPointsList;

	}

	::imtbase::CTreeItemModel* optionalPointsModel = model.GetTreeItemModel("OptionalPoints", modelIndex);
	if (model.ContainsKey("OptionalPoints", modelIndex) && optionalPointsModel == nullptr){
		OptionalPoints.SetNull();
	}
	if (optionalPointsModel != nullptr){
		int optionalPointsCount = optionalPointsModel->GetItemsCount();
		imtsdl::TElementList<V1_0::complextest::CPoint> optionalPointsList;
		for (int optionalPointsIndex = 0; optionalPointsIndex < optionalPointsCount; ++optionalPointsIndex){
			if (optionalPointsModel->ContainsKey(QByteArray(), optionalPointsIndex) && !optionalPointsModel->GetData(QByteArray(), optionalPointsIndex).isValid()){
				optionalPointsList.AppendNull();
				continue;
			}
			V1_0::complextest::CPoint t_optionalPoints;
			if (!t_optionalPoints.OptReadFromModel(*optionalPointsModel, optionalPointsIndex)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "GeometryType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

		return false;
	}
	QList<::imtgql::CGqlParamObject> pointsDataObjectList;
	for (qsizetype pointsIndex = 0; pointsIndex < Points->size(); ++pointsIndex){
		if (!Points->at(pointsIndex)){
			pointsDataObjectList << ::imtgql::CGqlParamObject::CreateNull();
			continue;
		}
		::imtgql::CGqlParamObject newPointsGqlObject;
		if (!Points->at(pointsIndex)->WriteToGraphQlObject(newPointsGqlObject)){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

			return false;
		}
		pointsDataObjectList << newPointsGqlObject;
	}
	gqlObject.InsertParam("Points", pointsDataObjectList);

	if (!RequiredPoints){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

		return false;
	}
	QList<::imtgql::CGqlParamObject> requiredPointsDataObjectList;
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < RequiredPoints->size(); ++requiredPointsIndex){
		if (!RequiredPoints->at(requiredPointsIndex)){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Array field '%3' contains a null element").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		::imtgql::CGqlParamObject newRequiredPointsGqlObject;
		if (!RequiredPoints->at(requiredPointsIndex)->WriteToGraphQlObject(newRequiredPointsGqlObject)){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		requiredPointsDataObjectList << newRequiredPointsGqlObject;
	}
	gqlObject.InsertParam("RequiredPoints", requiredPointsDataObjectList);

	if (OptionalPoints){
		QList<::imtgql::CGqlParamObject> optionalPointsDataObjectList;
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < OptionalPoints->size(); ++optionalPointsIndex){
			if (!OptionalPoints->at(optionalPointsIndex)){
				optionalPointsDataObjectList << ::imtgql::CGqlParamObject::CreateNull();
				continue;
			}
			::imtgql::CGqlParamObject newOptionalPointsGqlObject;
			if (!OptionalPoints->at(optionalPointsIndex)->WriteToGraphQlObject(newOptionalPointsGqlObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
			optionalPointsDataObjectList << newOptionalPointsGqlObject;
		}
		gqlObject.InsertParam("OptionalPoints", optionalPointsDataObjectList);
	}
	else if (OptionalPoints.IsNull()){
		gqlObject.InsertParam("OptionalPoints", QVariant());
	}

	gqlObject.InsertParam("__typename", QVariant("Geometry"));

	return true;
}


bool V1_0::complextest::CGeometry::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (!gqlObject.ContainsParam("GeometryType") || (gqlObject["GeometryType"].userType() != QMetaType::QString && gqlObject["GeometryType"].userType() != QMetaType::QByteArray)){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "GeometryType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

		return false;
	}

	if (gqlObject.ContainsParam("Radius") && (gqlObject["Radius"].userType() == QMetaType::Float || gqlObject["Radius"].userType() == QMetaType::Double || gqlObject["Radius"].userType() == QMetaType::Int || gqlObject["Radius"].userType() == QMetaType::UInt || gqlObject["Radius"].userType() == QMetaType::LongLong || gqlObject["Radius"].userType() == QMetaType::ULongLong || gqlObject["Radius"].userType() == QMetaType::Long || gqlObject["Radius"].userType() == QMetaType::Short || gqlObject["Radius"].userType() == QMetaType::ULong || gqlObject["Radius"].userType() == QMetaType::UShort || gqlObject["Radius"].userType() == QMetaType::UChar)){
		Radius = gqlObject["Radius"].toDouble();
	}

	if (!gqlObject.ContainsParam("Points") || (gqlObject["Points"].isNull())){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

		return false;
	}
	const qsizetype pointsElementsCount = gqlObject.GetObjectsCount("Points");
	Points = imtsdl::TElementList<V1_0::complextest::CPoint>();
	for (qsizetype pointsIndex = 0; pointsIndex < pointsElementsCount; ++pointsIndex){
		const ::imtgql::CGqlParamObject* pointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Points", pointsIndex);
		if (pointsDataObjectPtr == nullptr || pointsDataObjectPtr->IsNull()){
			Points->AppendNull();
			continue;
		}
		V1_0::complextest::CPoint tempPoints;
		if (!tempPoints.ReadFromGraphQlObject(*pointsDataObjectPtr)){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

			return false;
		}
		Points->append(tempPoints);
	}

	if (!gqlObject.ContainsParam("RequiredPoints") || (gqlObject["RequiredPoints"].isNull())){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

		return false;
	}
	const qsizetype requiredPointsElementsCount = gqlObject.GetObjectsCount("RequiredPoints");
	RequiredPoints = imtsdl::TElementList<V1_0::complextest::CPoint>();
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < requiredPointsElementsCount; ++requiredPointsIndex){
		const ::imtgql::CGqlParamObject* requiredPointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("RequiredPoints", requiredPointsIndex);
		if (requiredPointsDataObjectPtr == nullptr || requiredPointsDataObjectPtr->IsNull()){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Array field '%3' contains a null element").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		V1_0::complextest::CPoint tempRequiredPoints;
		if (!tempRequiredPoints.ReadFromGraphQlObject(*requiredPointsDataObjectPtr)){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		RequiredPoints->append(tempRequiredPoints);
	}

	if (gqlObject.ContainsParam("OptionalPoints") && gqlObject["OptionalPoints"].isNull()){
		OptionalPoints.SetNull();
	}
	if (gqlObject.ContainsParam("OptionalPoints") && !(gqlObject["OptionalPoints"].isNull())){
		const qsizetype optionalPointsElementsCount = gqlObject.GetObjectsCount("OptionalPoints");
		OptionalPoints = imtsdl::TElementList<V1_0::complextest::CPoint>();
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < optionalPointsElementsCount; ++optionalPointsIndex){
			const ::imtgql::CGqlParamObject* optionalPointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("OptionalPoints", optionalPointsIndex);
			if (optionalPointsDataObjectPtr == nullptr || optionalPointsDataObjectPtr->IsNull()){
				OptionalPoints->AppendNull();
				continue;
			}
			V1_0::complextest::CPoint tempOptionalPoints;
			if (!tempOptionalPoints.ReadFromGraphQlObject(*optionalPointsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

			return false;
		}
	}

	if (gqlObject.ContainsParam("Radius") && (gqlObject["Radius"].userType() == QMetaType::Float || gqlObject["Radius"].userType() == QMetaType::Double || gqlObject["Radius"].userType() == QMetaType::Int || gqlObject["Radius"].userType() == QMetaType::UInt || gqlObject["Radius"].userType() == QMetaType::LongLong || gqlObject["Radius"].userType() == QMetaType::ULongLong || gqlObject["Radius"].userType() == QMetaType::Long || gqlObject["Radius"].userType() == QMetaType::Short || gqlObject["Radius"].userType() == QMetaType::ULong || gqlObject["Radius"].userType() == QMetaType::UShort || gqlObject["Radius"].userType() == QMetaType::UChar)){
		Radius = gqlObject["Radius"].toDouble();
	}

	if (gqlObject.ContainsParam("Points") && !(gqlObject["Points"].isNull())){
		const qsizetype pointsElementsCount = gqlObject.GetObjectsCount("Points");
		Points = imtsdl::TElementList<V1_0::complextest::CPoint>();
		for (qsizetype pointsIndex = 0; pointsIndex < pointsElementsCount; ++pointsIndex){
			const ::imtgql::CGqlParamObject* pointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("Points", pointsIndex);
			if (pointsDataObjectPtr == nullptr || pointsDataObjectPtr->IsNull()){
				Points->AppendNull();
				continue;
			}
			V1_0::complextest::CPoint tempPoints;
			if (!tempPoints.OptReadFromGraphQlObject(*pointsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

				return false;
			}
			Points->append(tempPoints);
		}
	}

	if (gqlObject.ContainsParam("RequiredPoints") && !(gqlObject["RequiredPoints"].isNull())){
		const qsizetype requiredPointsElementsCount = gqlObject.GetObjectsCount("RequiredPoints");
		RequiredPoints = imtsdl::TElementList<V1_0::complextest::CPoint>();
		for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < requiredPointsElementsCount; ++requiredPointsIndex){
			const ::imtgql::CGqlParamObject* requiredPointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("RequiredPoints", requiredPointsIndex);
			if (requiredPointsDataObjectPtr == nullptr || requiredPointsDataObjectPtr->IsNull()){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Array field '%3' contains a null element").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

				return false;
			}
			V1_0::complextest::CPoint tempRequiredPoints;
			if (!tempRequiredPoints.OptReadFromGraphQlObject(*requiredPointsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

				return false;
			}
			RequiredPoints->append(tempRequiredPoints);
		}
	}

	if (gqlObject.ContainsParam("OptionalPoints") && gqlObject["OptionalPoints"].isNull()){
		OptionalPoints.SetNull();
	}
	if (gqlObject.ContainsParam("OptionalPoints") && !(gqlObject["OptionalPoints"].isNull())){
		const qsizetype optionalPointsElementsCount = gqlObject.GetObjectsCount("OptionalPoints");
		OptionalPoints = imtsdl::TElementList<V1_0::complextest::CPoint>();
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < optionalPointsElementsCount; ++optionalPointsIndex){
			const ::imtgql::CGqlParamObject* optionalPointsDataObjectPtr = gqlObject.GetParamArgumentObjectPtr("OptionalPoints", optionalPointsIndex);
			if (optionalPointsDataObjectPtr == nullptr || optionalPointsDataObjectPtr->IsNull()){
				OptionalPoints->AppendNull();
				continue;
			}
			V1_0::complextest::CPoint tempOptionalPoints;
			if (!tempOptionalPoints.OptReadFromGraphQlObject(*optionalPointsDataObjectPtr)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "GeometryType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

		return false;
	}
	QJsonArray newPointsArray;
	for (qsizetype pointsIndex = 0; pointsIndex < Points->size(); ++pointsIndex){
		if (!Points->at(pointsIndex)){
			newPointsArray << QJsonValue(QJsonValue::Null);
			continue;
		}
		QJsonObject newPointsJsonObject;
		if (!Points->at(pointsIndex)->WriteToJsonObject(newPointsJsonObject)){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

			return false;
		}
		newPointsArray << newPointsJsonObject;
	}
	jsonObject["Points"] = newPointsArray;

	if (!RequiredPoints){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

		return false;
	}
	QJsonArray newRequiredPointsArray;
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < RequiredPoints->size(); ++requiredPointsIndex){
		if (!RequiredPoints->at(requiredPointsIndex)){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Array field '%3' contains a null element").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		QJsonObject newRequiredPointsJsonObject;
		if (!RequiredPoints->at(requiredPointsIndex)->WriteToJsonObject(newRequiredPointsJsonObject)){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		newRequiredPointsArray << newRequiredPointsJsonObject;
	}
	jsonObject["RequiredPoints"] = newRequiredPointsArray;

	if (OptionalPoints){
		QJsonArray newOptionalPointsArray;
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < OptionalPoints->size(); ++optionalPointsIndex){
			if (!OptionalPoints->at(optionalPointsIndex)){
				newOptionalPointsArray << QJsonValue(QJsonValue::Null);
				continue;
			}
			QJsonObject newOptionalPointsJsonObject;
			if (!OptionalPoints->at(optionalPointsIndex)->WriteToJsonObject(newOptionalPointsJsonObject)){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
			newOptionalPointsArray << newOptionalPointsJsonObject;
		}
		jsonObject["OptionalPoints"] = newOptionalPointsArray;
	}
	else if (OptionalPoints.IsNull()){
		jsonObject["OptionalPoints"] = QJsonValue(QJsonValue::Null);
	}

	jsonObject["__typename"] = "Geometry";

	return true;
}


bool V1_0::complextest::CGeometry::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (!jsonObject.contains("GeometryType") || ! jsonObject["GeometryType"].isString()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "GeometryType").toLocal8Bit().constData();)

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
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

		return false;
	}

	if (jsonObject.contains("Radius") && jsonObject["Radius"].isDouble()){
		Radius = jsonObject["Radius"].toDouble();
	}

	if (!jsonObject.contains("Points") || ! jsonObject["Points"].isArray()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

		return false;
	}
	const QJsonArray pointsJsonArray = jsonObject["Points"].toArray();
	const qsizetype pointsArrayCount = pointsJsonArray.size();
	Points = imtsdl::TElementList<V1_0::complextest::CPoint>();
	for (qsizetype pointsIndex = 0; pointsIndex < pointsArrayCount; ++pointsIndex){
		if (pointsJsonArray[pointsIndex].isNull()){
			Points->AppendNull();
			continue;
		}
		V1_0::complextest::CPoint tempPoints;
		if (!tempPoints.ReadFromJsonObject(pointsJsonArray[pointsIndex].toObject())){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

			return false;
		}
		Points->append(tempPoints);
	}

	if (!jsonObject.contains("RequiredPoints") || ! jsonObject["RequiredPoints"].isArray()){
		I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Field: '%3' doesn't exist, but required").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

		return false;
	}
	const QJsonArray requiredPointsJsonArray = jsonObject["RequiredPoints"].toArray();
	const qsizetype requiredPointsArrayCount = requiredPointsJsonArray.size();
	RequiredPoints = imtsdl::TElementList<V1_0::complextest::CPoint>();
	for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < requiredPointsArrayCount; ++requiredPointsIndex){
		if (requiredPointsJsonArray[requiredPointsIndex].isNull()){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Array field '%3' contains a null element").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		V1_0::complextest::CPoint tempRequiredPoints;
		if (!tempRequiredPoints.ReadFromJsonObject(requiredPointsJsonArray[requiredPointsIndex].toObject())){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

			return false;
		}
		RequiredPoints->append(tempRequiredPoints);
	}

if (jsonObject.contains("OptionalPoints") && jsonObject["OptionalPoints"].isNull()){
		OptionalPoints.SetNull();
	}
		if (jsonObject.contains("OptionalPoints") && jsonObject["OptionalPoints"].isArray()){
		const QJsonArray optionalPointsJsonArray = jsonObject["OptionalPoints"].toArray();
		const qsizetype optionalPointsArrayCount = optionalPointsJsonArray.size();
		OptionalPoints = imtsdl::TElementList<V1_0::complextest::CPoint>();
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < optionalPointsArrayCount; ++optionalPointsIndex){
			if (optionalPointsJsonArray[optionalPointsIndex].isNull()){
				OptionalPoints->AppendNull();
				continue;
			}
			V1_0::complextest::CPoint tempOptionalPoints;
			if (!tempOptionalPoints.ReadFromJsonObject(optionalPointsJsonArray[optionalPointsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "GeometryType", geometryTypeStringValue);)

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
			if (pointsJsonArray[pointsIndex].isNull()){
				Points->AppendNull();
				continue;
			}
			V1_0::complextest::CPoint tempPoints;
			if (!tempPoints.OptReadFromJsonObject(pointsJsonArray[pointsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "Points").toLocal8Bit().constData();)

				return false;
			}
			Points->append(tempPoints);
		}
	}

	if (jsonObject.contains("RequiredPoints") && jsonObject["RequiredPoints"].isArray()){
		const QJsonArray requiredPointsJsonArray = jsonObject["RequiredPoints"].toArray();
		const qsizetype requiredPointsArrayCount = requiredPointsJsonArray.size();
		RequiredPoints = imtsdl::TElementList<V1_0::complextest::CPoint>();
		for (qsizetype requiredPointsIndex = 0; requiredPointsIndex < requiredPointsArrayCount; ++requiredPointsIndex){
			if (requiredPointsJsonArray[requiredPointsIndex].isNull()){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Array field '%3' contains a null element").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

				return false;
			}
			V1_0::complextest::CPoint tempRequiredPoints;
			if (!tempRequiredPoints.OptReadFromJsonObject(requiredPointsJsonArray[requiredPointsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "RequiredPoints").toLocal8Bit().constData();)

				return false;
			}
			RequiredPoints->append(tempRequiredPoints);
		}
	}

if (jsonObject.contains("OptionalPoints") && jsonObject["OptionalPoints"].isNull()){
		OptionalPoints.SetNull();
	}
		if (jsonObject.contains("OptionalPoints") && jsonObject["OptionalPoints"].isArray()){
		const QJsonArray optionalPointsJsonArray = jsonObject["OptionalPoints"].toArray();
		const qsizetype optionalPointsArrayCount = optionalPointsJsonArray.size();
		OptionalPoints = imtsdl::TElementList<V1_0::complextest::CPoint>();
		for (qsizetype optionalPointsIndex = 0; optionalPointsIndex < optionalPointsArrayCount; ++optionalPointsIndex){
			if (optionalPointsJsonArray[optionalPointsIndex].isNull()){
				OptionalPoints->AppendNull();
				continue;
			}
			V1_0::complextest::CPoint tempOptionalPoints;
			if (!tempOptionalPoints.OptReadFromJsonObject(optionalPointsJsonArray[optionalPointsIndex].toObject())){
				I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "OptionalPoints").toLocal8Bit().constData();)

				return false;
			}
			OptionalPoints->append(tempOptionalPoints);
		}
	}

	return true;
}




} // namespace sdl::V1_0::complextest
